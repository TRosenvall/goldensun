/* OvlFunc_883_20091d8  --  0x020091d8,
 * asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_a.s
 *
 * Source asm: goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_a.s
 *
 * BLOCKER CLASS: a pool load scheduled one call too early.
 * Status: 38 lines against 38, 9 differing, and all 9 are one instruction
 * sliding across a call boundary.
 *
 *     rom    ... bl __MapActor_SetSpeed / ldr r5, =0xf4d / mov r0, r5
 *     ours   ldr r5, =0xf4d / ... / bl __MapActor_SetSpeed / mov r0, r5
 *
 * TWO THINGS WERE SOLVED GETTING HERE.
 *
 *   THE SECOND MESSAGE ID IS DERIVED FROM THE FIRST. The ROM keeps 0xf4d in r5
 *   and does `add r5, #2` for 0xf4f. Written as two literals gcc emits two pool
 *   entries and the function comes out THREE INSTRUCTIONS SHORT (35 against
 *   38), which is the shorter-stream signature. A named `int id = 0xf4d` used
 *   as `id` and `id + 2` reproduces the ROM's shape exactly.
 *
 *   THE LAST CALL'S ARGUMENT ORDER. `__Func_80921c4(0, 0x45, 0x366)` with a
 *   prototype precomputes the pooled 0x366 ahead of r0 and r1; the ROM emits it
 *   last. Leaving that call implicit -- the second declaration lever -- fixes
 *   it, 10 differing down to 9.
 *
 * WHAT IS LEFT is the `ldr r5, =0xf4d` placement. gcc hoists it above
 * __MapActor_SetSpeed because nothing stops it: `id` is a plain constant with
 * no dependencies and r5 is callee-saved either way. Measured:
 * -fno-schedule-insns 9, -fno-gcse 9, -fno-rerun-cse-after-loop 9,
 * -fno-schedule-insns2 12 (worse), -O1 12 (worse). Turning the schedulers off
 * makes it worse rather than exposing the ROM's order, which puts this with
 * the thirteen-member family's blocker rather than with anything spellable.
 */
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_801776c(int a, int b);
/* __Func_80921c4 intentionally implicit -- see the note above. */

void OvlFunc_883_20091d8(void)
{
    int id;

    if (__GetFlag(0x808) == 0) {
        __CutsceneStart();
        __MapActor_SetSpeed(0, 0x80 << 9, 0x80 << 8);
        id = 0xf4d;
        __MessageID(id);
        __Func_8093040(0xf, 0, 2);
        __Func_8093040(0x10, 0, 2);
        __Func_801776c(id + 2, 1);
        __CutsceneWait(6);
        __Func_80921c4(0, 0x45, 0x366);
        __CutsceneEnd();
    }
}
