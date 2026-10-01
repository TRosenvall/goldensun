/* DisplayMenuArrowCursor (EmitPartySprites) -- 0x0801aeec, PARK.
 * NON-MATCHING: 6 encodings of 133 differ (objcmp, production flags).  WAS 16.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/DisplayMenuArrowCursor.c \
 *     asm/rom_15000/rom_1aeec_a_a_a_a_a.s --func DisplayMenuArrowCursor
 *
 * BATCH 316b: the 16 -> 6 LEVER IS NOW ACTUALLY IN THE BODY.  Batch 297a found it,
 * batch 305 noticed the header had advanced while the body had not, and it still was
 * not applied.  It is applied here and it reproduces exactly:
 *
 *     int d;  unsigned short e;
 *     d = m->a[1].f0;      -- WIDE local: the zero_extend:SI load, feeds the ADD
 *     e = d;               -- NARROW local: the HImode value, feeds the COMPARE
 *     src = L342f8;
 *     if (e != 0) o->x = o->x + d;
 *
 * `e = d` is the ROM's `mov r3, r2`.  The compare load is `(set (reg:HI) (mem:HI))`
 * and the add load is `(zero_extend:SI (mem:HI))`; with both spelled as one local
 * cse merges them and one of the two sites re-loads.  Two locals OF DIFFERENT WIDTH
 * keep the modes apart, and THE DIRECTION IS LOAD-BEARING.  Re-measured in one
 * container this batch, from the 16-of-133 park body:
 *     int d + u16 e = d, compare on e, add on d          6   <- INSTALLED
 *     the same with d/e placed AFTER `src = ...`         6
 *     the same with `e = m->a[i].f0` re-read             6
 *     the same with d/e block-scoped inside each arm     6
 *     u16 d + int e (NARROW carries the add)            57   RELOCDIFF
 *     one local + `(unsigned short)d != 0` at the test   57   RELOCDIFF
 *
 * SPLIT SHAPE: NONE.  Re-ran tools/datacheck.py on the reference this batch: no
 * output, exit 0 -- no TEXT/DATA split.  Reference holds one function.
 * tools/shimcount.py on the candidate reports no pins -- NO fakematch.txt row.
 * The data labels `.L342f8` / `.L33ef8` are already `.global` in
 * asm/rom_15000/rom_1aeec_c_c_b.s, so no new export and no linker alias.
 *
 * ===================== THE REMAINING 6, READ OFF THE DUMP =====================
 *
 * Positions with operands (objdump -dz encodings, ref | ours):
 *     idx 21  ldrh r1,[r6,r2]   | adds r5,r3,#0
 *     idx 22  adds r5,r3,#0     | mov  ip,r0
 *     idx 23  mov  ip,r0        | ldrh r3,[r6,r2]
 *     idx 25  mov  r3,ip        | mov  r1,ip
 *     idx 35  mov  r1,r8        | mov  r2,r8
 *     idx 37  cmp  r1,#0        | cmp  r2,#0
 * idx 27 `and r3,r1` is identical in both (commutative, two-address).  So (1) is
 * ONE three-insn rotation -- WHERE the `ldrh` of `m->a[i].x` sits -- plus the two
 * register names that follow from it, and (2) is the hi->lo reload round-robin for
 * `i` in r8, coupled to (1) because idx 25 is itself a hi->lo reload.
 *
 * *** THE sched2 DUMP, NOT A GUESS.  Block 1 at production flags: ***
 *     t=0..1  416  r0=0x1ff              (pool load, 2 cycles)
 *     t=2     45   r3=r6+r2              (&m->a[i])
 *     t=3     53   r2=r2+0x10            (the x offset)
 *     t=4     412  r5=r3                 <-- THE ROM EMITS INSN 55 HERE
 *     t=5     57   ip=r0
 *     t=6..7  55   r3=zxn([r6+r2])       <-- the ldrh
 * Insn 55 IS NOT IN THE READY LIST UNTIL t=5 (latency 2 from 53), and at t=5 it
 * loses to 57.  So this is NOT reachable by any rank_for_schedule tie-break: 55
 * is not a candidate at t=4 at all.  The chain 45 -> 53 is FORCED -- 53 writes r2
 * and 45 reads it, an anti-dependence -- so 53 cannot move earlier either.
 * To close (1) the x load has to become ready at t=4, i.e. its ADDRESS must stop
 * depending on the `adds r2,#0x10`, and the ROM has that add.
 *
 * *** MEASURED NEGATIVE, AND IT RETIRES A TEMPTING CROSS-PARK TRANSFER. ***
 * The landed file-mate src/rom_15000/rom_18cac_a_c.c closed its last 2 with a
 * `*(unsigned short *)&win->w` cast on ONE struct-member read, equalising a sched2
 * dependent count through the MEM's alias set.  THAT LEVER DOES NOT CARRY HERE.
 * Crossed 5 spellings of the x read against 3 of the y read (15 variants, one
 * container):
 *     x member,         y member                        6   (this body)
 *     x member,         y `*(short *)&...`             26
 *     x member,         y through a union              26
 *     x `*(unsigned short *)&m->a[i].x`, any y    117-119, dsize -4..+8, RELOCDIFF
 *     x through a union / `(void *)` / `char *`   116-119,                RELOCDIFF
 * A cast on the X read destroys the shared `m + i*0x34` address CSE -- it changes
 * the ADDRESSING, so it is not a scheduling lever on this function.
 *
 * *** AND A 32-VARIANT CROSS FLOORS AT 6, so 6 is not a one-at-a-time artefact. ***
 * Crossed: position of `f = (iwram_3001800 >> 2) & 7;` (4 slots) x `if (i != 0)`
 * vs `if (i)` x store order (x-then-y / y-then-x) x `o = &m->a[i].oam` before the
 * stores vs writing them through `m->a[i].oam.x` and computing `o` after.
 *     f at top,  o first, x-then-y   ->   6   (both i-test spellings)
 *     f after the f2 early-return    ->  22
 *     y-then-x                       ->  52-63
 *     f after the arms               -> 130, dsize -4
 *     o computed after the stores    -> 130+
 * Only 2 of the 32 reach 6 and they differ only in the (inert) i-test spelling.
 */
struct OamSprite {
    unsigned char pad[4];
    unsigned int y:8;
    unsigned int affineMode:2;
    unsigned int objMode:2;
    unsigned int mosaic:1;
    unsigned int bpp:1;
    unsigned int shape:2;
    unsigned int x:9;
    unsigned int matrixNum:5;
    unsigned int size:2;
    unsigned int tileNum:10;
    unsigned int priority:2;
    unsigned int paletteNum:4;
};

struct Arrow {
    unsigned short f0;
    unsigned short f2;
    unsigned short slot;
    unsigned short tile;
    unsigned short x;
    short y;
    unsigned char pad0c[0x14];
    struct OamSprite oam;
    unsigned char pad2c[8];
};

struct Menu {
    unsigned char pad00[8];
    struct Arrow a[2];
    unsigned char pad70[0x394 - 0x70];
    unsigned short f394;
    unsigned short f396;
    unsigned short f398;
    unsigned short pad39a;
    unsigned short f39c;
};

extern unsigned char L342f8[] __asm__(".L342f8");
extern unsigned char L33ef8[] __asm__(".L33ef8");
extern unsigned int iwram_3001800;
extern int UploadSpriteGFX(int slot, int n, void *src);
extern int _GetFlag(int id);
extern void Func_8003dec(void *p, int n);

void DisplayMenuArrowCursor(struct Menu *m, int i)
{
    struct OamSprite *o;
    unsigned char *src;
    int f;
    int d;
    unsigned short e;

    f = (iwram_3001800 >> 2) & 7;
    if (m->a[i].f2 == 0)
        return;
    o = &m->a[i].oam;
    o->x = m->a[i].x;
    o->y = m->a[i].y;
    if (i != 0) {
        d = m->a[1].f0;
        e = d;
        src = L342f8;
        if (e != 0)
            o->x = o->x + d;
    } else {
        d = m->a[0].f0;
        e = d;
        src = L33ef8;
        if (e != 0)
            o->x = o->x - d;
    }
    o->tileNum = UploadSpriteGFX(m->a[i].slot, 0x80, src + f * 0x80);
    if (_GetFlag(0x103)) {
        if (*(unsigned short *)((unsigned char *)m + 0x2e2) == 1)
            o->objMode = 1;
        else
            o->objMode = 0;
    }
    Func_8003dec(o, 0xee);
    if (m->a[i].f0 != 0)
        m->a[i].f0--;
}
