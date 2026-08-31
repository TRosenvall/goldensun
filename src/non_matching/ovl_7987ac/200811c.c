/* OvlFunc_902_200811c -- 0x0200811c  (asm/overlays/rom_7987ac/ovl_30_c_c_a_a_c_c_c_a_a.s)
 *
 * BLOCKER: argument interleave at a STRAIGHT-LINE site. 2 of 62, exact length.
 * This is the documented out-of-reach case, not a new shape -- see "Argument-
 * setup order: the zero interleaved into a shifted build" in docs/elevation.md,
 * which sizes 98 remaining functions as straight-line at every site.
 *
 *     rom   mov r1, #0x81 / mov r0, #0x10 / lsl r1, #0x1 / mov r2, #0x3c
 *     ours  mov r1, #0x81 / lsl r1, #0x1  / mov r0, #0x10 / mov r2, #0x3c
 *
 * The single-instruction argument 0x10 belongs INSIDE the split build of
 * 0x81 << 1. The lever for this needs named locals assigned at the top with a
 * BRANCH between the assignments and the call, so gcc declines to keep them
 * live and rematerialises at the use. This function's only conditional branch
 * -- the __Func_8091c7c guard -- sits AFTER the __MapActor_Emote call, so there
 * is no block to dominate from and the precondition simply is not met.
 *
 * Four spellings of the argument list, all inert, all still 2 differing:
 *
 *     name the split build, slot left bare      2   (the 6942 recipe)
 *     name the other two arguments              2
 *     name all three                            2
 *     write 0x102 instead of 0x81 << 1          2
 *
 * and dropping __MapActor_Emote's prototype is WORSE at 3. That the "name the
 * OTHER arguments" recipe is inert here is the useful datum: that recipe was
 * recorded from OvlFunc_932_200a9dc, where the naming happened in a DOMINATING
 * BLOCK. Reading it as "name the other arguments" alone drops the load-bearing
 * half. The block is the lever; the naming is just how you reach across it.
 *
 * THE OTHER HALF OF THE RESIDUE WAS REACHABLE, and is worth recording because
 * it is a second confirmation of a lever the docs call narrow. The function
 * started at 4 differing; two of those were a plain two-argument fill order:
 *
 *     rom   mov r1, #0x0  / mov r0, #0x10      __Func_8092c40(0x10, 0)
 *     ours  mov r0, #0x10 / mov r1, #0x0
 *
 * DELETING the `extern void __Func_8092c40(int, int);` declaration -- letting
 * it be implicitly declared -- gives the ROM's order exactly. 4 -> 2 on that
 * one edit. Note this is deletion, not weakening: docs/elevation.md records
 * that `extern int f();` behaves identically to a full prototype, so the lever
 * is the absence of a declaration and nothing less than that. The kin file
 * beside this one calls __MessageID with no declaration, so the convention is
 * already in the overlay.
 *
 * NEXT: nothing at this site. If the interleave class is ever reached for
 * straight-line sites, this function is a two-instruction test case.
 */
extern int iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __MapActor_SetAnim(int a, int n);
extern void __MapActor_DoAnim(int a, int n);
extern void __MapActor_Emote(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_902_200811c(void)
{
    char *p;

    __CutsceneStart();
    __MessageID(0x1cd4);
    __Func_8092848(0x10, 0, 2);
    __MapActor_SetAnim(0x10, 1);
    __Func_8093040(0x10, 0, 0x14);
    __MapActor_DoAnim(0x10, 4);
    __CutsceneWait(0x14);
    __Func_8093040(0x10, 0, 0x14);
    __MapActor_Emote(0x10, 0x81 << 1, 0x3c);
    __Func_8093040(0x10, 0, 0x1e);
    __Func_8092c40(0x10, 0);
    if (__Func_8091c7c(0, 0) != 0) {
        p = (char *)iwram_3001ebc;
        *(unsigned short *)(p + (0xec << 1)) += 1;
    }
    __Func_8093040(0x10, 0, 0x14);
    __SetFlag(0xc0 << 2);
    __SetFlag(0x868);
    __CutsceneEnd();
}
