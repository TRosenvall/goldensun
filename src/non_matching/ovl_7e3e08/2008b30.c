/* OvlFunc_957_2008b30 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_a_a.s
 * Best screen: 56 instructions against the ROM's 54, 11 differing.
 *
 * Sets the fade parameters for one room, differently depending on a flag.
 *
 * THREE THINGS WERE SOLVED, 17 differing to 11:
 *
 *   THE TWO IN-ARM HALFWORD STORES NEED int LOCALS IN A DOMINATING BLOCK.
 *   0x3f and 0x1f are stored through a short *, so a literal gives
 *   `ldr r3, =0x1f` where the ROM has `mov r3, #0x1f` -- the HImode rule. The
 *   `if` supplies the boundary and they are declared above it.
 *
 *   THE REGISTER BASE IS A WALKED POINTER. `REG_BLDCNT = ...; REG_BLDALPHA =
 *   ...;` gives two independent pool addresses; the ROM has one address and
 *   `add r3, #2`, which is the destructive-add walk form.
 *
 *   THE SECOND GLOBAL IS REACHED FROM THE FIRST. `ldr r5, =iwram_3001ebc` then
 *   `ldr r5, [r5, #0x10]` is `((char **)&iwram_3001ebc)[4]`, not a separate
 *   symbol.
 *
 * BLOCKER, and it is the HImode rule with nowhere to stand. The remaining
 * `mov r3, #5` is a third short store, and it happens BEFORE the `if`:
 *
 *     rom    mov r3, #0x5 / strh r3, [r2]
 *     ours   ldr r3, =0x5 / strh r3, [r2]
 *
 * The rule needs an int-typed value rematerialised at the store, which needs a
 * dominating block that is not the store's own. There is no branch before this
 * store, so the two available placements both fail in opposite directions:
 * declared at the top of the function the value is live across two calls and
 * gcc gives it a callee-saved register (`mov r6, #5`, `push {r5, r6, lr}` --
 * 18 differing); declared beside the store it is in the store's own block and
 * the pool load comes back.
 *
 * That is the same no-boundary wall as src/non_matching/ovl_780898/2008e54.c
 * and src/non_matching/rom_7d30e0/2009838.c, reached from the HImode side
 * rather than the constant-CSE side -- which is worth knowing, because it says
 * the missing construct would unpark more than the nine already catalogued.
 *
 * The other four differing lines are an r2/r3 exchange on the two register
 * stores: the ROM puts the VALUE in r2 and the ADDRESS in r3, we do the
 * reverse. Same instructions, same order.
 */
#include "gba/types.h"
#include "gba/io.h"

extern char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __WaitFrames(int n);
extern void __Func_808fe38(int n);
extern void OvlFunc_957_2008a54(void);

void OvlFunc_957_2008b30(void)
{
    char *p;
    char *q;
    vu16 *reg;
    int a3f;
    int a1f;

    a3f = 0x3f;
    a1f = 0x1f;
    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x100;
    *(int *)(p + 0x1c8) = 0x18;
    __WaitFrames(1);
    __Func_808fe38(0x4d);
    q = ((char **)&iwram_3001ebc)[4];
    *(short *)(q + 0x52a) = 5;
    if (__GetFlag(0x201)) {
        *(short *)(q + 0x534) = 0x1d1d;
        *(short *)(q + 0x536) = a3f;
        OvlFunc_957_2008a54();
    } else {
        *(short *)(q + 0x534) = 0x3f3f;
        *(short *)(q + 0x536) = a1f;
        reg = &REG_BLDCNT;
        *reg = 0x3f42;
        reg = (vu16 *)((char *)reg + 2);
        *reg = 0xc04;
    }
}
