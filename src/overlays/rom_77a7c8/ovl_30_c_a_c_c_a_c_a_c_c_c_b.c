/* OvlFunc_881_200b1fc  --  0x0200b1fc
 *
 * Cut from the head of goldensun/asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_c_c.s;
 * the second function stays in _c.s. No data in the file, so the split is a
 * pure text cut and was verified byte-neutral before this landed.
 *
 * Spawns a decorative actor at a randomised position, then on every third frame
 * nudges the camera to one of four randomly chosen offsets.
 *
 * THE MULTIPLY/SHIFT SPLIT POINT DECIDES WHERE A SECOND CALL LANDS IN AN
 * ARGUMENT LIST, and it has to be the ROM's own boundary rather than just "some"
 * split. The ROM interleaves two randomiser-derived arguments, finishing part of
 * the first chain, calling again, and then finishing both. Written inline, gcc
 * completes argument one entirely before the second call -- 12 differing.
 * Naming only the raw call results is ALSO 12, so this is not "hoist the calls".
 * The lever is putting EXACTLY the multiply in the statement and leaving
 * EXACTLY the shift-mask-and-add in the argument expression. Leaving one shift
 * behind in the statement misplaces a single `lsl` by one slot -- 3 differing.
 * Screen the multiply fully-in against partially-in before concluding anything.
 * Function scope against block scope was byte-identical, so scope was not the
 * lever; the split point was.
 *
 * THE NARROW-MASK LEVER'S LIVE RANGE IS LOAD-BEARING, AND GETTING IT WRONG IS
 * WORSE THAN NOT USING IT. The recorded fix -- name the mask so it is not
 * narrowed -- works only if the assignment sits immediately before the use.
 * Declared with an initialiser at the top of the enclosing block, so that it
 * lives across an intervening call, the pseudo takes a third callee-saved
 * register and rotates every register after it: 14 differing becomes 36, which
 * is strictly worse than not naming the mask at all. At the point of use it is
 * 14 to 12. That is the register-rotation symptom produced by a CORRECT lever
 * applied at the wrong scope.
 *
 * A FOUR-CASE DENSE SWITCH GETS THE DECISION TREE, NOT A JUMP TABLE -- four
 * consecutive cases sit below the case-values threshold, so gcc emits a
 * balanced compare tree with an unsigned branch. The ROM's shared tails, entered
 * by branches from two different arms with the physically last arm unmerged
 * because it falls through, come out verbatim from four plain break-terminated
 * calls with literal arguments. No hand-sharing and no per-arm locals were
 * needed here -- in contrast to a sibling that needed three separate locals for
 * three identical constants, because there they sat in ONE basic block.
 *
 * The [offset] marker did not apply: the byte offsets above the store's
 * immediate limit are single-use and fall out of plain indexing, and the
 * genuinely repeated constants are pooled position words that reach a call.
 * No flag rule needed -- those repeats live in different switch arms, so cse
 * never relates them.
 *
 * The remaining diff is the modulo helper's linker alias, already present in
 * this overlay's own script.
 */
extern unsigned int __Random(void);
extern unsigned char *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_SetScript(unsigned char *a, unsigned char *s);
extern void __Func_80933f8(int a, int b, int c, int d);
extern unsigned int iwram_3001e40;
extern unsigned char gScript_881__0200d14c[];

void OvlFunc_881_200b1fc(void)
{
    unsigned char *n;
    unsigned char *p;
    unsigned int x;
    unsigned int z;
    int v;
    int m;

    x = __Random() * 40;
    z = __Random() * 30;
    n = __CreateActor(0xde, 0x17b00000 + (x >> 16 << 16), 0,
                      0xc4c0000 + (z >> 16 << 16));
    if (n != 0) {
        p = *(unsigned char **)(n + 0x50);
        v = (__Random() * 0x8000 >> 16) + 0x13333;
        p[0x26] = 0;
        m = -13;
        p[9] = (m & p[9]) | 8;
        n[0x55] = 0;
        *(int *)(n + 0x18) = v;
        *(int *)(n + 0x1c) = v;
        __Actor_SetAnim(n, 1);
        __Actor_SetScript(n, gScript_881__0200d14c);
    }
    if (iwram_3001e40 % 3 == 0) {
        switch (__Random() * 4 >> 16) {
        case 0: __Func_80933f8(0x17c70000, -1, 0xc690000, 1); break;
        case 1: __Func_80933f8(0x17c90000, -1, 0xc670000, 1); break;
        case 2: __Func_80933f8(0x17c90000, -1, 0xc690000, 1); break;
        case 3: __Func_80933f8(0x17c70000, -1, 0xc670000, 1); break;
        }
    }
}
