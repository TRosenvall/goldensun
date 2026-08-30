/* Cluster OvlFunc_952_20085a4..OvlFunc_952_20085a4 extracted from
 * goldensun/asm/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c.s.
 *
 * PARKED FOR SEVERAL BATCHES AT 3 DIFFERING; TWO SEPARATE LEVERS CLOSED IT.
 *
 * 1. THE MESSAGE BASE IS A SYMBOL, NOT AN INTEGER.  The ROM keeps 0x2352 in r5
 *    for the whole cutscene and derives the other two ids from it with
 *    `add r0, r5, #2` and `add r0, r5, #3`.  Written `int m = 0x2352;` gcc
 *    pools all three ids independently and never spends a callee-saved
 *    register -- rematerialising a pool constant is cheaper than a push/pop
 *    pair.  `extern int _MSG_2352; m = (int)(&_MSG_2352);` makes it derive.
 *    That was already known; what was missing is that _MSG_2352 was never
 *    added to message.sym, so the screen ran with the symbol unresolved.  It
 *    is there now, and the entry emits no bytes.
 *
 * 2. THE ARGUMENT ORDER IN THE TWO ARMS IS FIXED BY AN ABSENT PROTOTYPE.
 *    The old park note blamed __ActorMessage(0xe, 0), which the ROM emits r0-
 *    first in the then-arm and r1-first in the else-arm while we emitted r0
 *    first in both, and called it the blocker.  The lever is not at that call:
 *    deleting the declaration of __Func_8092c40 -- the call immediately BEFORE
 *    the branch -- puts both arms right.  3 differing -> 1, and the last one
 *    was only the unresolved symbol.  DO NOT ADD A PROTOTYPE FOR __Func_8092c40.
 *
 * The second lever is the same one that un-parked OvlFunc_955_20092f0 and
 * closed OvlFunc_956_200a4d0 in this batch, and this function is the case that
 * shows it need not be applied at the call site that looks wrong.  Both of
 * those had the residue and the fix at the same call; here they are one call
 * apart.  When an argument order is wrong, try dropping the prototype of the
 * NEIGHBOURING calls too, not only the offending one.
 */
extern int _MSG_2352;
extern void __CutsceneStart(void);
extern void __Func_808e118(void);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __CutsceneWait(int n);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern int __Func_8091c7c(int a, int b);
extern void __MapActor_DoAnim(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_808f1c0(int a, int b);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_8091a58(int a, int b);
extern void __SetFlag(int id);

void OvlFunc_952_20085a4(void)
{
    int m;
    int n;

    __CutsceneStart();
    __Func_808e118();
    m = (int)(&_MSG_2352);
    __MessageID(m);
    n = 1;
    n = -n;
    __ActorMessage(n, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x1e);
    __Func_809280c(0, 0xe, 0x1e);
    __Func_8092c40(0xe, 0);
    if (__Func_8091c7c(0, 0) != 0) {
        __MessageID(m + 2);
        __ActorMessage(0xe, 0);
    } else {
        __CutsceneWait(0x14);
        __MessageID(m + 3);
        __ActorMessage(0xe, 0);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0, 3);
        __CutsceneWait(0x1e);
        __Func_8092adc(0, 0x80 << 7, 0);
        __CutsceneWait(0x1e);
        __MapActor_SetPos(0x10, 0, 0);
        __Func_808f1c0(0xcd, 3);
        __MapActor_SetAnim(0, 1);
        __Func_8091a58(0xcd, 0);
        __SetFlag(0xf31);
    }
}
