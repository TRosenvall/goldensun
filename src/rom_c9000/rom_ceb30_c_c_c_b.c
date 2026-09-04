/* BuildDraw2DFuncs  --  0x080cef64
 *
 * Cut out of goldensun/asm/rom_c9000/rom_ceb30_c_c_c.s.
 *
 * Builds the two draw-2D entries for an effect and hands their function
 * pointers back through `out`. Entry 0x2e goes to out[0], entry 0x2f to
 * out[1], and the pointers are read from gPtrs at +0xb8 and +0xbc. The only
 * thing `mode` decides is the clip-flags argument: 3 when it is zero, 7
 * otherwise.
 *
 * CORRECTION to the .s header this replaced, which read "dispatches on r0 to
 * choose the effect style, defaulting to 2 when r0 is 0". The 2 is not a
 * default style -- it is the fifth argument of the FIRST call, and it is 2 in
 * both arms. What actually varies with r0 is the fourth argument, 3 against 7,
 * and the fifth argument is 2 on the first call and 3 on the second regardless
 * of mode.
 *
 * BOTH ARMS ARE WRITTEN OUT IN FULL, INCLUDING THE TAIL. This is the
 * "duplicated ROM code means duplicated source" rule, and the measurement shows
 * how much of the tail has to be duplicated: sharing the `out[1]` store between
 * the arms leaves 28 of 52, and writing the second call AND the out[1] store
 * inside both arms takes it to 8. gcc then cross-jumps the common suffix -- the
 * `bl`, the load and the store -- back into one block by itself, which is why
 * the ROM shows a single shared tail even though the source repeats it. The
 * `add r5, #0xbc` stays in each arm because the two arms order it differently
 * against the stack-argument store, so the merge cannot start any earlier.
 *
 * gPtrs IS A DIRECT READ, NOT A NAMED LOCAL, and this one is worth recording
 * because the instinct runs the other way. The ROM loads its address in each
 * arm and keeps it in a callee-saved register across the second call, which
 * looks exactly like the recorded "one load kept across a call is a named
 * local" shape. It is not. MEASURED: a named `void **p = gPtrs;` hoisted above
 * the branch gives 33, the same local assigned separately inside each arm gives
 * 28, and no local at all gives 8. The register really is live across the call,
 * but that is gcc's own doing once both arms need the address -- not something
 * the source asked for.
 *
 * THE LAST FOUR INSTRUCTIONS WERE THE CALLEE'S RETURN TYPE. With
 * BuildDraw2DFuncEx declared `void`, `mov r0, #0x2e` is emitted one slot too
 * early in both arms -- 8 differing, symmetric. Declaring it to return a value
 * puts it last, exactly as the ROM has it, and the function matches.
 *
 * That is the mechanism recorded on OvlFunc_884_2008780, seen from the other
 * side: an implicitly-declared, int-returning callee carries
 * `(set (reg:SI 0 r0) (call ...))`, which is the next REAL write of r0 and so
 * truncates the dependent list of the `mov r0` feeding it, changing how the
 * argument-setup scheduling tie resolves. There it was `void` that was needed;
 * here it is anything BUT void.
 *
 * MEASURED, and the indifference is the point: `int`, `unsigned int`, `char`,
 * `short`, `void *`, `long` and leaving the callee undeclared entirely all
 * match. Only `void` fails. The lever is whether the call writes r0 at all, not
 * what width it writes -- so pick the type the callee actually has rather than
 * reading anything into the choice. `int` is used here because an undeclared
 * callee defaults to it.
 */

extern void *gPtrs[];
extern int BuildDraw2DFuncEx(int idx, int a, int b, int flags, int e);

void BuildDraw2DFuncs(int mode, void **out)
{
    if (mode == 0) {
        BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
        out[0] = gPtrs[0xb8 / 4];
        BuildDraw2DFuncEx(0x2f, 7, 7, 3, 3);
        out[1] = gPtrs[0xbc / 4];
    } else {
        BuildDraw2DFuncEx(0x2e, 7, 7, 7, 2);
        out[0] = gPtrs[0xb8 / 4];
        BuildDraw2DFuncEx(0x2f, 7, 7, 7, 3);
        out[1] = gPtrs[0xbc / 4];
    }
}
