/* Func_809b450 (0x0809b450) -- MATCHING.  LANDED IN BATCH 314, BRIEF B.
 * Was parked at "2 encodings of 145, sched2 tie, UNREACHABLE FROM SOURCE".
 * THE PARK'S PROOF WAS WRONG AT ITS FIFTH KEY; see THE CORRECTION below.
 *
 * tools/objcmp.py, PRODUCTION FLAGS (-O2 -mthumb -mthumb-interwork
 * -mcpu=arm7tdmi -fno-builtin -nostdinc -ffreestanding -fcall-used-r4
 * -Iinclude -- the tree default for this path; no Makefile row is needed):
 *   OK Func_809b450 -- 312 bytes, 145 encodings and 7 relocations identical
 * objcmp --whole agrees: OK whole file -- 312 bytes, 145 encodings, 7 relocations.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_8a000/rom_9ad70_c_a_c_c_c.c \
 *     asm/rom_8a000/rom_9ad70_c_a_c_c_c.s --whole
 *
 * SPLIT SHAPE: NONE.  grep -c thumb_func_start on the reference is 1 and
 * tools/datacheck.py is SILENT, so this is a WHOLE-FILE conversion to
 * src/rom_8a000/rom_9ad70_c_a_c_c_c.c with no export list.
 * tools/shimcount.py: CLEAN.  No register pins, no __asm__(".equ"), no
 * fakematch.txt row, no per-file flag override.
 *
 * ================= WHAT LANDED IT: ONE DECLARATION AND ONE ACCESS =========
 *
 *     struct W16 { unsigned char pad[0x16]; unsigned char f16; } *f28;
 *     s->f28->f16 = z.v;
 *
 * replacing `unsigned char *f28;` / `s->f28[0x16] = z.v;`.  Nothing else in
 * the file changed.
 *
 * ================= THE CORRECTION ========================================
 * The park had read rank_for_schedule's five keys for the residue
 *     ROM   mov r1,#0x21 / neg r1,r1 / strh r3,[r5,#8]
 *     ours  mov r1,#0x21 / strh r3,[r5,#8] / neg r1,r1
 * and called all five tied or skipped, so that INSN_LUID decided and the strh
 * won.  Four of the five readings hold.  THE FIFTH DOES NOT: it wrote
 * "depend_count.  234 -> {285, 237} = 2.  478 -> {467, 239} = 2" and took both
 * counts to be fixed by the ROM's own instruction stream.  From
 * -da -fsched-verbose=8 (t.c.23.sched2), insn 285 is
 *     (set (mem:QI (plus (reg r3) 22) 0) (reg:QI r1))        <- ALIAS SET 0
 * and its LOG_LINKS carry REG_DEP_OUTPUT 234, where 234 is
 *     (set (mem/s:HI (plus (reg r5) 8) 21) (reg:HI r3))      <- ALIAS SET 21
 * So 234's SECOND dependent is a MEMORY OUTPUT DEPENDENCE that exists only
 * because `f28` was spelled `unsigned char *`: alias set 0 is the char set and
 * it conflicts with EVERY other set, so the f28[0x16] store was made a
 * dependent of a store it cannot alias.  A struct-typed f28 puts that MEM in
 * the record's own set, the output dep disappears, 234 drops to ONE dependent
 * while 478 keeps TWO, and rank_for_schedule's "more dependents wins" picks
 * the neg.  INSN_LUID is never reached -- in EITHER direction.
 *
 * The generalisation, which is the batch's main mechanism: THE DEPENDENT COUNT
 * IS SOURCE-CONTROLLABLE THROUGH ALIAS SETS whenever one of the two competing
 * insns is a MEM.  A raw `unsigned char *` cast maximises the count (set 0
 * conflicts with everything); a struct or union member access takes the
 * record's set and can be made disjoint, or made to conflict on purpose.  See
 * src/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_c.c for the same lever
 * run in the OPPOSITE direction (a union used to ADD a dependent).
 *
 * MEASURED AND STILL TRUE OF THE ALIAS CHOICE: `*((u8 *)s + 0x1d) |= 1`
 * (NOT `s->f1d |= 1`) is still load-bearing and must stay a raw char access --
 * it is the QImode-OR lever recorded below, and it is a different statement
 * from the f28 one.  Only f28 moved.
 *
 * ================= LEVERS THAT GOT IT TO TWO, ALL STILL LOAD-BEARING ======
 *  - `*((u8 *)s + 0x1d) |= 1` (or a 1-bit bitfield), NOT `s->f1d |= 1`: the
 *    QImode struct-member OR leaves an `ior ... (subreg (reg:QI 0))` that CSE
 *    feeds from the a->f55 store's QI zero; combine then kills the use but
 *    leaves the QI zero SET (REG_UNUSED) alive through local-alloc, which puts
 *    it in r3 and pushes the `a+0x55` address pseudo off r3.  With it gone, the
 *    two plain `a->f55 = 0; a->f64 = 0;` stores get the same register and
 *    postreload move2add gives the ROM's `add r3,#0xf` from the 0x55 address.
 *  - the pooled zero for `s[0x26]` / the f28 byte is the struct HalfWord case.
 *
 * ================= MEASURED INERT OR WORSE (kept so nobody re-spends them) =
 * All 120 orders of the five tail statements (best = source order); all 40
 * placements of `z.v = 0` / `s = a->f50` (37 in the old base); the f08 value as
 * temp / `/ 32` / SpriteSlot pointer / a `tile:10` bitfield in SpriteSlot;
 * f08 through a separate `struct SprBits` or a char union; byte 5/7 through
 * separate B5/B7 structs (92); `m = -0x21` byte-AND spellings before or after
 * f08 (21); `s->c6 = 1` before `s->c5 = 0` (12); `do{}while(0)` / `asm("")`
 * between the statements (size changes); an explicit `q = a + 0x55; ...;
 * q += 0xf` pointer with an int zero (30).  From batch 295: the coupled
 * "f08 value as temp" pair at four declaration placements (68/110/136/120);
 * `__asm__ ("" : "+r" (s))` barriers at three positions (4, 6, 67); every one
 * of 17 flags alone and in all 136 pairs, best 2, with -fno-schedule-insns2
 * WORSE -- the positive control that sched2 ran on the ROM too.
 * NOTE for the record: -fno-schedule-insns is inert here because sched1 DOES
 * NOT RUN in this build (the -da sequence is 17.lreg 18.greg 19.flow2 20.ce2
 * 23.sched2 25.jump2 26.mach), so that row was never evidence of anything.
 */
struct SpriteSlot {
    unsigned short size;
    unsigned short vramOffset;
};

struct Sprite {
    unsigned char pad00[5];
    unsigned char c0 : 5;
    unsigned char c5 : 1;
    unsigned char c6 : 2;
    unsigned char pad06;
    unsigned char e0 : 6;
    unsigned char e6 : 2;
    unsigned short f08 : 10;
    unsigned short b10 : 2;
    unsigned short b12 : 4;
    unsigned char pad0a[0x1c - 0x0a];
    unsigned char f1c;
    unsigned char f1d;
    unsigned char pad1e[0x26 - 0x1e];
    unsigned char f26;
    unsigned char f27;
    struct W16 { unsigned char pad[0x16]; unsigned char f16; } *f28;
};

struct Sprite9 {
    unsigned char pad00[9];
    unsigned char lo : 2;
    unsigned char prio : 2;
    unsigned char hi : 4;
};

struct Actor {
    void *f00;
    unsigned char pad04[4];
    int f08;
    int f0c;
    int f10;
    int f14;
    int f18;
    int f1c;
    unsigned char pad20[0x50 - 0x20];
    struct Sprite *f50;
    unsigned char pad54[0x55 - 0x54];
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    unsigned short f64;
    unsigned char pad66[0x68 - 0x66];
    struct Actor *f68;
    void (*f6c)(void);
};

struct HalfWord { unsigned short v; };

extern unsigned char *iwram_3001f30;
extern struct SpriteSlot gSpriteSlots[];
extern struct Actor *_CreateActor(int id, int x, int y, int z);
extern void _Sprite_SetAnim(struct Sprite *s, int n);
extern void Func_8003f3c(int n);
extern void Func_809b3d8(void);
extern void Func_809b364(void);

void Func_809b450(struct Actor *e)
{
    unsigned char *g;
    struct Actor *leader;
    struct Actor *arr[2];
    struct Actor *a;
    struct Sprite *s;
    struct HalfWord z;
    int i;

    g = iwram_3001f30;
    leader = *(struct Actor **)(g + 0x10);
    for (i = 0; i <= 1; i++) {
        a = _CreateActor(0x1a, e->f08, e->f0c + (0x80 << 15), e->f10);
        arr[i] = a;
        if (a == 0)
            continue;
        a->f14 = e->f14;
        s = a->f50;
        a->f55 = 0;
        a->f64 = 0;
        z.v = 0;
        a->f68 = e;
        a->f1c = 0x6666;
        a->f18 = 0x6666;
        if (s == 0)
            continue;
        _Sprite_SetAnim(s, 0);
        s->f26 = z.v;
        Func_8003f3c(s->f1c);
        s->f1c = *(unsigned short *)(g + 0x71a);
        *((unsigned char *)s + 0x1d) |= 1;
        s->f08 = gSpriteSlots[s->f1c].vramOffset >> 5;
        s->c5 = 0;
        s->c6 = 1;
        s->e6 = 2;
        s->f28->f16 = z.v;
    }
    arr[0]->f6c = Func_809b3d8;
    ((struct Sprite9 *)arr[0]->f50)->prio = 0;
    arr[1]->f6c = Func_809b364;
    ((struct Sprite9 *)arr[1]->f50)->prio = ((struct Sprite9 *)leader->f50)->prio;
}
