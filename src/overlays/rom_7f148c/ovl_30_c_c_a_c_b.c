/* Cluster OvlFunc_966_20080c4..OvlFunc_966_20080c4 extracted from goldensun/asm/overlays/rom_7f148c/ovl_30_c_c_a_c.s.
 *
 * Split out of that .s; the _a and _c parts stay as assembly.
 *
 * A three-message prompt: says the opening line, runs a check, and delivers
 * one of two follow-ups at base+1 or base+2. One of seven identical stubs
 * differing only in the message base.
 *
 * TWO THINGS ARE LOAD-BEARING.
 *
 * 1. The base is a SYMBOL, not a literal. The ROM holds it in r5 across the
 *    whole function and computes the follow-ups with `add r0, r5, #1`. Written
 *    as a plain 0x28be gcc folds each use into its own pool load and the
 *    function comes out an instruction shorter. Only a link-time address
 *    survives as a register value gcc will add to.
 *
 * 2. __Func_8092c40 is deliberately NOT declared. The ROM fills its r0 LAST,
 *    which is what gcc does for an implicitly declared callee -- see
 *    docs/elevation.md. Declaring it flips the pair and costs the match.
 */
extern void __MessageID(int id);
extern void __CutsceneWait(int frames);
extern void __ActorMessage(int slot, int b);
extern int __Func_8091c7c(int a, int b);
extern int _MSG_28be;

void OvlFunc_966_20080c4(int slot)
{
    int base = (int)(&_MSG_28be);

    __MessageID(base);
    __Func_8092c40(slot, 0);
    if (!__Func_8091c7c(0, 0)) {
        __CutsceneWait(0xa);
        __MessageID(base + 1);
    } else {
        __MessageID(base + 2);
    }
    __ActorMessage(slot, 0);
}
