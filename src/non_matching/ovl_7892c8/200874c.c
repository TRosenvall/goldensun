/* OvlFunc_888_200874c -- NOT MATCHING, a SIZE mismatch: ref 252 bytes against ours 240.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/N874c_best.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * objcmp stops at the size check, so no encoding figure exists yet; the
 * body must reach the reference's LENGTH before a distance can be read.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7892c8/200874c.c \
 *     asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_a_c_a_a_a.s --func OvlFunc_888_200874c
 *
 * The original prose park is preserved below.
 */
extern unsigned char *iwram_3001ed0;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int n);
extern void __Func_8019aa0(int a, int b, int c);
extern void __Func_8091e9c(int n);
extern int OvlFunc_888_200a7d4(void);

void OvlFunc_888_200874c(void)
{
    unsigned char *p;

    __CutsceneStart();
    __MapActor_SetAnim(0, 0);
    __MapActor_SetAnim(1, 0);
    __MapActor_SetAnim(0xb, 0);
    __MapActor_SetAnim(0xc, 0);
    __MapActor_SetAnim(8, 0);
    __MapActor_SetAnim(9, 0);
    __MapActor_SetAnim(0xa, 0);
    __Func_8091200(0x10002, 0);
    __Func_8091254(0x78);
    __CutsceneWait(0xb4);
    p = iwram_3001ed0;
    *(short *)(p + 0xe5a) = 0xf8 << 7;
    *(short *)(p + 0xe5c) = 0xf8 << 7;
    *(short *)(p + 0xe5e) = 0xf8 << 7;
    p[0xa8 << 6] = 0;
    p[0x2a01] = 1;
    p[0x2a02] = 1;
    p[0x2a03] = 1;
    __CutsceneWait(1);
    __Func_8019aa0(0x116d, 1, 0);
    __Func_8091200(0, 0);
    __Func_8091254(0x78);
    __CutsceneWait(0x78);
    __CutsceneWait(0x3c);
    if (OvlFunc_888_200a7d4() == 0) {
        __CutsceneEnd();
        __Func_8091e9c(0x14);
    } else {
        __CutsceneEnd();
        __Func_8091e9c(0x32);
    }
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_888_200874c -- 0x0200874c,
 * asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_a_c_a.s
 *
 * 87 lines against the ROM's 89, 47 differing, all of it in ONE run of seven
 * stores through a base pointer.  Everything before and after -- seven
 * __MapActor_SetAnim calls, the fade pair, the choice prompt, the two-armed
 * tail -- is exact.  Candidate at scratch/N874c_best.c.
 *
 * THE STORE BLOCK, and three separate residues in it:
 *
 *   ROM   ldr r0, =0xe5a / ldr r4, =0xe5c / mov r1, #0xf8 / lsl r1, #7
 *         add r3, r2, r0 / strh r1, [r3] / add r3, r2, r4 / add r4, #2 ...
 *   ours  ldr r0, =0xe5a / ldr r1, =0x7c00
 *         add r3, r2, r0 / add r0, #2 / strh r1, [r3] ...
 *
 *   1. THE HALFWORD VALUE IS POOLED.  We emit `ldr r1, =0x7c00`; the ROM builds
 *      it `mov r1, #0xf8 / lsl r1, #7`.  Same shape as
 *      src/non_matching/ovl_7b2078/2008afc.c, where naming the value in an int
 *      local fixes it -- and here that fix costs more than it buys: 47
 *      differing -> 52, because it changes which register every offset lands in.
 *   2. THE OFFSETS.  The ROM keeps 0xe5a and 0xe5c in TWO registers and derives
 *      only the third (`add r4, #2`); we keep one and mutate it twice.  The
 *      same at the byte stores: the ROM pool-loads 0x2a01, 0x2a02 and 0x2a03
 *      separately where we derive each from the last with `add r0, #1`.
 *      Writing them as separate literals is what the candidate already does --
 *      gcc CSEs them into a chain anyway.  A named mutated offset for the pair
 *      (the batch-141 lever) goes the wrong way: 85 lines, 55 differing.
 *   3. THE ZERO BYTE STORE.  The ROM has `ldr r0, =0x0` -- a pooled zero -- and
 *      gcc notices the address register for 0x2a00 already has a zero low byte
 *      and emits `strb r3, [r1]` off the ADDRESS.  That is correct code and one
 *      instruction shorter, and there is nothing to write in C that forbids it.
 *
 * Residues 1 and 3 are both the narrow-store constant question that
 * 2008afc and src/non_matching/ovl_77a7c8/200b57c.c also park on, and the three
 * of them disagree about which way gcc goes: 200b57c pools a zero the ROM movs,
 * this one movs a value the ROM pools, and 2008afc needs one of each.  The
 * typed-field lever settles it when the destination is a struct member; here
 * the destinations are seven different offsets off one iwram pointer and there
 * is no struct to hang them on without inventing one.
 *
 * NEXT THING TO TRY: build the struct.  If `iwram_3001ed0` gets a declared type
 * with `short` at 0xe5a/0xe5c/0xe5e and `unsigned char` at 0x2a00..0x2a03, all
 * three residues are in scope for the one lever, and docs/structs.md says no
 * such struct exists yet.  That is a bigger change than a spelling and it is
 * the reason this is parked rather than pushed further today.
 *)

*/
#endif
