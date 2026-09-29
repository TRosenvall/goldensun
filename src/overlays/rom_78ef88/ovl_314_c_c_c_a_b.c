/* OvlFunc_896_200bf24 -- MATCHES.  0 of 353 encodings differ.
 *   [asm/overlays/rom_78ef88/ovl_314_c_c_c_a.s, 2nd of 2 functions]
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       scratch_elev/b300e/fbf24/MATCH_OvlFunc_896_200bf24.c \
 *       asm/overlays/rom_78ef88/ovl_314_c_c_c_a.s --func OvlFunc_896_200bf24
 *   -> OK OvlFunc_896_200bf24 -- 804 bytes, 353 encodings and 45 relocations identical
 *
 * 347 instructions of map-variant patching: a 0xf..0x18 actor reset loop, then
 * four save-bit-guarded blocks, three of which replay the same ten-call
 * __CopyMapTiles / __Func_8010704 group with different tile coordinates.
 *
 * THE NEIGHBOUR THAT CARRIED IT: src/overlays/rom_7c097c/ovl_30_c_c_c_c_a_a_b.c
 * (49 __CopyMapTiles calls) and src/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_c.c (40,
 * plus the gState-offset idiom).  Their recorded rules -- name the stack-argument
 * constants; only the FIRST assignment that creates each pseudo matters; the
 * counter is initialised before the constants and must be UNSIGNED for `bls` --
 * took the first transcription to 262 of 353 and the rest fell out in four steps.
 *
 * | step                                                    | differing of 353 |
 * |---------------------------------------------------------|------------------|
 * | first transcription (pushes r7 as well)                  | 262, 4 B short   |
 * | + GF() eviction pin on the FIRST __GetFlag(0x83b)        | 111              |
 * | + PIN3 ordering pins on the two __MapActor_SetPos calls  | 105              |
 * | + `f3 = 0x1f` named after `s3 = 0xa` (block 0x83e, F704#3)|   5             |
 * | + `fz = 0x0a` named at block 0x83c's F704#9              |   3             |
 * | + `g3 = 0x18; h3 = 9;` named (block 0x83e, F704#6)       |   0             |
 *
 * ONE VARIABLE SET PER GUARDED BLOCK.  The three save-bit blocks give 3/4/1/2
 * DIFFERENT registers (0x83c: 3->r5 4->r10 0x29->r11 0x1d->r9 1->r6 2->r8;
 * 0x83d: 3->r6 4->r8 0x1d->r11 1->r10 2->r9 0x15->r5; 0x83e: 3->r5 4->r10
 * 0xa->r9 1->r6 2->r8), so each block needs its OWN locals -- a single shared
 * set is one pseudo and cannot take three assignments.  This is the recorded
 * one-variable-per-region rule reaching guarded blocks.
 *
 * NEW, AND IT IS THE LAST 105: TWO STACK-SLOT VALUES IN TWO REGISTERS MEANS TWO
 * NAMED LOCALS; ONE REGISTER REUSED MEANS AT MOST ONE.  Read the ROM's register
 * count at the site, not the values:
 *      ref   mov r3,#0x18 / mov r2,#9    / str r3,[sp] / str r2,[sp,#4]
 *      ours  mov r3,#0x18 / str r3,[sp]  / mov r3,#9   / str r3,[sp,#4]
 * Two registers live at once = two pseudos = two named locals, assigned in the
 * ROM's creation order ([sp] value first).  One register reused = literals.  At
 * block 0x83e's F704#3 that single distinction was worth 105 -> 5, because our
 * one-register form also mis-scheduled every following (e,f) pair in the block.
 * The mirror sites in blocks 0x83c/0x83d reuse one register and take literals;
 * naming them there is WORSE.  This SHARPENS docs/elevation.md's recorded
 * discriminator for the (2,1)-style pairs -- batch 257 found literals right for
 * thirteen sites and batch 295's rom_78b2ac found named locals right for four
 * blocks; the tell that separates them is the ROM's live REGISTER COUNT at the
 * two `str [sp]`s.
 *
 * SEVEN REGISTER PINS, ALL LOAD-BEARING, so this needs a fakematch.txt row.
 * Greedy drop-and-retest of all three pin sites against the cumulative set:
 *   * GF(0x83b) -- WITHOUT IT, 324 of 353.  0x83b is read twice (the first
 *     guarded block and the closing OvlFunc_896_200a7f8 test) and gcse hoists the
 *     pooled id into r7, which adds r7 to the prologue AND the epilogue and
 *     replaces both `ldr r0,=0x83b`.  The other three ids are read once and need
 *     nothing.
 *   * PIN3 on __MapActor_SetPos, both sites -- 3 of 353 with either dropped.
 *     The ROM emits `mov r1,#0xe4 / mov r2,#0xb4 / mov r0,#9 / lsl r1,#17 /
 *     lsl r2,#17`; sched2 sinks a trivially-rematerialisable `mov r0,#9` past the
 *     shifts and only a hard-register pin holds it.  PIN-FREE WAS TRIED AND
 *     FAILS: named locals for all three arguments with the shifts as separate
 *     statements (6 of 353) and a P0-only pin on the slot (8 of 353) both leave
 *     `mov r0,#9` after the shifts.  Same conclusion as rom_78b2ac's "ordering
 *     pin on __MapActor_SetPos".
 *
 * LANDING SHAPE.  tools/datacheck.py exits 0 and prints nothing: no
 * `.section .data`, no `.incbin`.  So a FUNCTION SPLIT ONLY, no data half and NO
 * NEW EXPORTS -- `.thumb_func_start` expands through
 * `.thumb_func_start_noalign`, which emits `.global` itself.  The target is the
 * SECOND of two, so `tools/split_s.py` cuts OvlFunc_896_200a7f8 (which this
 * function calls, and which keeps its linker slot) away from it; give both pieces
 * NEW names and delete the original so the generated `.s` cannot overwrite the
 * file it was split from.  The stem `ovl_314_c_c_c_a` is named by 3 overlay.ld
 * rows in 3 overlay.ld files (6 files in overlays/ with the .map siblings);
 * rom_78ef88/overlay.ld:43 is this one, and rom_7b6668/overlay.ld:53 names a
 * DIFFERENT overlay's same-named object for `(.data)`, so do not edit that row.
 * makefile_flags() is empty for the stem, so plain -O2.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8091220(int a, int b);
extern void __Func_8092b08(int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned char gState[];
extern void OvlFunc_896_200a27c(void);
extern void OvlFunc_896_200a7f8(void);
extern void OvlFunc_896_200c78c(int a, int b);

#define P0 register int q0 __asm__("r0")
#define PIN2 register int q0 __asm__("r0"); register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define GF(x) ({ P0; q0 = (x); __GetFlag(q0); })

int OvlFunc_896_200bf24(void)
{
    unsigned int i;
    int z;
    int off;
    int p, q, s, t, u, v;
    int fz;
    int p2, q2, s2, u2, v2, w2;
    int p3, q3, s3, u3, v3;
    int f3;
    int g3, h3;

    __Func_8091220(0x80 << 9, 0);
    __SetFlag(0xa2 << 1);
    i = 0xf;
    z = 0;
    do {
        __MapActor_GetActor(i)[0x59] = z;
        __Func_8092b08(i, 1);
        i++;
    } while (i <= 0x18);
    OvlFunc_896_200c78c(0xf, 0x10);
    if (GF(0x83b) != 0) {
        { PIN3; q1 = 0xe4; q2 = 0xb4; q0 = 9; q1 <<= 17; q2 <<= 17;
          __MapActor_SetPos(q0, q1, q2); }
        { PIN3; q1 = 0xdc; q2 = 0xad; q0 = 5; q1 <<= 17; q2 <<= 17;
          __MapActor_SetPos(q0, q1, q2); }
    }
    if (__GetFlag(0x83c) != 0) {
        p = 3;
        __CopyMapTiles(0, 0x28, 0x2b, 0x42, p, p);
        q = 4;
        __CopyMapTiles(0x53, 0x28, 0x60, 0x1d, p, q);
        s = 0x29;
        t = 0x1d;
        __Func_8010704(0, 0, 1, 1, s, t);
        u = 1;
        v = 2;
        __CopyMapTiles(0x57, 0x2a, 0x29, 0x1f, u, v);
        __CopyMapTiles(0x53, 0x28, 0x4a, 0x1d, p, q);
        __Func_8010704(0, 0, 1, 1, 0x13, t);
        __CopyMapTiles(0x57, 0x2a, 0x13, 0x1f, u, v);
        __CopyMapTiles(0x53, 0x28, 0x60, 0xa, p, q);
        fz = 0xa;
        __Func_8010704(0, 0, 1, 1, s, fz);
        __CopyMapTiles(0x57, 0x2a, 0x29, 0xc, u, v);
    }
    if (__GetFlag(0x83d) != 0) {
        p2 = 3;
        __CopyMapTiles(0, 0x28, 0x2b, 0x2e, p2, p2);
        q2 = 4;
        __CopyMapTiles(0x53, 0x28, 0x54, 4, p2, q2);
        s2 = 0x1d;
        __Func_8010704(0, 0, 1, 1, s2, q2);
        u2 = 1;
        v2 = 2;
        __CopyMapTiles(0x57, 0x2a, 0x1d, 6, u2, v2);
        __CopyMapTiles(0x53, 0x28, 0x4c, 0x15, p2, q2);
        w2 = 0x15;
        __Func_8010704(0, 0, 1, 1, w2, w2);
        __CopyMapTiles(0x57, 0x2a, 0x15, 0x17, u2, v2);
        __CopyMapTiles(0x53, 0x28, 0x4c, 0x1d, p2, q2);
        __Func_8010704(0, 0, 1, 1, w2, s2);
        __CopyMapTiles(0x57, 0x2a, 0x15, 0x1f, u2, v2);
    }
    if (__GetFlag(0x83e) != 0) {
        p3 = 3;
        __CopyMapTiles(0, 0x28, 0xd, 0x42, p3, p3);
        q3 = 4;
        __CopyMapTiles(0x53, 0x28, 0x41, 0x1f, p3, q3);
        s3 = 0xa;
        f3 = 0x1f;
        __Func_8010704(0, 0, 1, 1, s3, f3);
        u3 = 1;
        v3 = 2;
        __CopyMapTiles(0x57, 0x2a, 0xa, 0x21, u3, v3);
        __CopyMapTiles(0x53, 0x28, 0x4f, 9, p3, q3);
        g3 = 0x18;
        h3 = 9;
        __Func_8010704(0, 0, 1, 1, g3, h3);
        __CopyMapTiles(0x57, 0x2a, 0x18, 0xb, u3, v3);
        __CopyMapTiles(0x53, 0x28, 0x5b, 0xa, p3, q3);
        __Func_8010704(0, 0, 1, 1, 0x24, s3);
        __CopyMapTiles(0x57, 0x2a, 0x24, 0xc, u3, v3);
        OvlFunc_896_200a27c();
    }
    if (__GetFlag(0x83b) == 0) {
        off = 0xe1 << 1;
        if (*(short *)(gState + off) == 0xa)
            OvlFunc_896_200a7f8();
    }
    return 0;
}
