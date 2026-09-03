/* OvlFunc_901_20088a8  --  0x020088a8
 *
 * The remaining function from
 * goldensun/asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a_a.s; the two
 * ahead of it are ovl_314_c_c_a_a_c_c_a_c_c_a_a_b.c.
 *
 * BUILT WITH CSE_CFLAGS, for the reason that file already records: this is the
 * read-a-flag-then-set-the-same-flag shape, here 0x308, and at plain -O2 gcc
 * hoists the id into a callee-saved register across the call to save one pool
 * load. The ROM builds `mov r0, #0xc2 / lsl r0, #2` twice.
 * -fno-rerun-cse-after-loop is what undoes it. 27 aligned regions to 4.
 *
 * THE STORED ZERO IS THE FLAG VARIABLE, REASSIGNED. The tell is that the ROM
 * spends a CALLEE-SAVED register on it -- `mov r5, #0` for a value used by the
 * very next store but one, where a fresh local would take a caller-saved r3.
 * A callee-saved register means a live range that crosses a call, and the only
 * such range here is the flag's. So `f` is read from __GetFlag, tested, stored
 * as-is on the path where the branch has proved it zero (`strb r5` with no
 * preceding `mov` -- gcc knows the compared register holds 0 there), and
 * reassigned `f = 0;` on the path where it does not.
 *
 * THE ACTOR POINTER IS NOT A VARIABLE. Named, each use costs `mov r2, r0 /
 * add r2, #0x5b`; written as `__MapActor_GetActor(0x10)[0x5b] = ...` gcc
 * destroys r0 in place with `add r0, #0x5b`, which is the ROM.
 *
 * __Func_8092c40 RETURNS A VALUE, and that is the last two instructions. Its
 * arguments are (0x10, 0) and the ROM fills r1 before r0 -- the same order it
 * uses at every other two-argument call here, all of which we already matched.
 * Only this one call inverted, and only because it directly follows a `bl`.
 * Declaring the callee `int` marks r0 live out of the previous call, which
 * changes what precompute_register_parameters may reorder across, and the pair
 * lands the ROM's way round. Four unrelated spellings of the zero and of the
 * slot all measured EXACTLY 2 first, which is the notebook's own rule that the
 * residue was not reachable through those variables -- it was in a signature.
 */
extern unsigned short *iwram_3001ebc;
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, int b);
extern void __ActorMessage(int slot, int b);
extern void __Func_80925cc(int slot, int b);
extern void __Func_8092848(int slot, int b, int c);
extern int __Func_8092c40(int slot, int b);
extern int __Func_8091c7c(int a, int b);
extern void OvlFunc_901_20084b4(int slot);

void OvlFunc_901_20088a8(void)
{
    int f;

    f = __GetFlag(0x308);
    if (!f) {
        __CutsceneStart();
        __MapActor_GetActor(0x10)[0x5b] = 1;
        __MapActor_SetAnim(0x10, 1);
        __Func_80925cc(0x10, 1);
        __CutsceneWait(0x14);
        __MessageID(0x1cb5);
        __Func_8092848(0x10, 0, 2);
        __Func_8092c40(0x10, 0);
        if (__Func_8091c7c(0, 0))
            iwram_3001ebc[0xec]++;
        __ActorMessage(0x10, 0);
        __MapActor_GetActor(0x10)[0x5b] = f;
        __MapActor_SetBehavior(0x10, 2);
        __CutsceneEnd();
        __SetFlag(0x308);
    } else {
        __MessageID(0x1cc2);
        __MapActor_GetActor(0x10)[0x5b] = 1;
        OvlFunc_901_20084b4(0x10);
        f = 0;
        __MapActor_GetActor(0x10)[0x5b] = f;
    }
}
