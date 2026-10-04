/* OvlFunc_948_2009838 -- NOT MATCHING, and this park covers THREE functions.
 *
 * NON-MATCHING, 47 of 51 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 51, ours 55) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *   SIZE ref 136 bytes, ours 144.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_7d30e0/2009838.c \
 *     asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_c_c_c_c_c_c.s --func OvlFunc_948_2009838
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * Source asm: goldensun/asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_c_c_c.s
 * Best screen: 53 instructions against the ROM's 50, 52 differing.
 *
 * Siblings, found by tools/prologue_families.py:
 *   OvlFunc_948_2009838  49 instructions
 *   OvlFunc_948_200938c  51
 *   OvlFunc_948_200949c  51
 *
 * BLOCKER CLASS: a pool constant built once for two calls, with NO BRANCH to
 * lever against.
 *
 *     rom    mov r0, #0 / ldr r1, =0x1b333 / ldr r2, =0xd999 / bl SetSpeed
 *            mov r0, #0xc / ldr r1, =0x1b333 / ldr r2, =0xd999 / bl SetSpeed
 *     ours   ldr r5, =0x1b333 / ldr r6, =0xd999 / ... / mov r1, r5 / mov r2, r6
 *
 * gcc builds each constant once into a callee-saved register and the function
 * grows `push {r5, r6}`; the ROM issues four pool loads.
 *
 * THIS IS THE FIRST CASE WHERE -fno-rerun-cse-after-loop DOES NOT FIX A
 * CONSTANT HOIST, and that sharpens batch 106's rule. Measured, all 52 of 53:
 *   -fno-rerun-cse-after-loop
 *   -fno-gcse
 *   -fno-cse-follow-jumps
 *   -fno-cse-skip-blocks
 *   -fno-expensive-optimizations
 *   -fno-force-mem
 *
 * The distinction: the flag reaches CSE performed ACROSS A CALL, which is what
 * batch 106's OvlFunc_890_2008150 had -- a flag id read then written with calls
 * between. It does NOT reach the hoist gcc performs when the same pool constant
 * is needed by two calls in one straight-line block; that happens at expand and
 * no flag in this tree's vocabulary disables it.
 *
 * The BASIC-BLOCK LEVER reaches that hoist -- it is what fixed Task_BlitAnim in
 * batch 105 -- and it needs a branch between the assignment and the use. The
 * first branch in this function is the `if (a != 0)` guard, four calls AFTER
 * both SetSpeed calls. There is nothing to lever against.
 *
 * Same shape and same reason as src/non_matching/ovl_780898/2008e54.c, which
 * parks six functions on a straight-line arg-interleave. Between them,
 * NINE functions are parked on "the lever is right and there is no boundary",
 * which makes a construct that works without one the highest-value open
 * question in the tree after the eighteen-member family.
 */
extern char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __PlaySound(int id);
extern void __ClearFlag(int id);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern int __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern int __Func_809228c(int a, int b, int c);

void OvlFunc_948_2009838(void)
{
    char *a;

    __CutsceneStart();
    __MapActor_SetSpeed(0, 0x1b333, 0xd999);
    __MapActor_SetSpeed(0xc, 0x1b333, 0xd999);
    __PlaySound(0xbc);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_TravelTo(0xc, *(short *)(a + 0xa), *(short *)(a + 0x12));
    __MapActor_WaitMovement(0xc);
    __Func_809228c(0, 0, 0x18);
    __PlaySound(0xbc);
    __Func_809228c(0xc, 0, 0x10);
    __MapActor_WaitMovement(0);
    __MapActor_TravelTo(0xc, 0x9c << 1, 0xe8);
    __MapActor_WaitMovement(0xc);
    __CutsceneEnd();
    __ClearFlag(0x88 << 2);
}
