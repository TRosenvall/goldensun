/* OvlFunc_895_20097c0  --  0x020097c0   "RestoreBlockPuzzle"
 * [asm/overlays/rom_78dee8/ovl_30_c_c_c_a_c_c_c.s, the only function in the TU]
 *
 * Byte-exact: 748 bytes, 323 encodings and 42 relocations identical.
 *
 * Repaints the six-block puzzle from its save bits: six Func_8010704 calls wipe
 * the marker columns at x 0x7a, then six flag PAIRS each repaint one marker at
 * x 0x79 and, when the argument is set, teleport the matching slot.
 *
 * THE PROLOGUE IS `push {r5, r6, r7, lr}` -- a wide push, so the ROM does keep
 * constants, and the three levers below are all about WHICH ones.
 *
 *   1. THE SIX WIPE CALLS AND THE FIRST PAIR STAY BARE LITERALS.  cse_main
 *      commons 0x64 and 0x20 out of the straight-line wipe block into r7 and
 *      r5, and the extended basic block reaches into both arms of the first
 *      flag pair, which is why those two `__Func_8010704(0x79, ...)` sites also
 *      read `str r7, [sp]` / `str r5, [sp, #4]`.  Naming them defeats the hoist:
 *      spelling all twelve marker calls with locals narrows the push to
 *      {r5, r6, lr} and shifts the whole function -- 282 of 329 differing.
 *
 *   2. FROM THE SECOND PAIR ON, BOTH STACK ARGUMENTS MUST BE NAMED, s BEFORE t.
 *      Past the first join cse's table is invalidated, so the ROM rebuilds the
 *      values -- and it materialises BOTH before storing either
 *      (`mov r3,#0x68 / mov r2,#0x20 / str r3,[sp] / str r2,[sp,#4]`).  Bare
 *      literals push one argument at a time and interleave the movs with the
 *      stores.  Two named locals, declared and assigned in argument order, put
 *      two pseudos live at the call and reproduce the ROM: 30 differing -> 0.
 *      Either one alone leaves 30 (no t) or 20 (no s); assigning t first, 20.
 *
 *   3. __MapActor_SetPos's TWO SPLIT-BUILD ARGUMENTS ARE NAMED IN THE BLOCK
 *      THAT DOMINATES THE GUARDED CALL, not inside the `if` arm.  That is what
 *      lets gcc land `mov r0, #9` between the two seed movs and the two lsls.
 *      This is the lever the same directory's ovl_30_c_c_c_a_c_b.c records; the
 *      dominance precondition, inert there, is load-bearing here -- moving the
 *      two assignments inside the arm costs 36 differing, and naming only x
 *      costs 60.
 *
 * The flag ids are written the way the ROM builds them: 0x310, 0x314 and 0x318
 * as `0xc4 << 2`, `0xc5 << 2`, `0xc6 << 2`, which gcc emits as mov+lsl, and the
 * odd ones as plain literals, which pool.  No pins, no barriers.
 *
 * No wildcard in the Makefile captures rom_78dee8/ovl_30_c_c_c_a_c_c_c; the one
 * rule naming this directory is an explicit rule for a different object, so the
 * TU builds at the tree default -O2.
 */
extern int __GetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_895_20097c0(int reposition)
{
    int s, t;
    int x, y;

    __Func_8010704(0x7a, 0x14, 1, 1, 0x64, 0x20);
    __Func_8010704(0x7a, 0x14, 1, 1, 0x68, 0x20);
    __Func_8010704(0x7a, 0x14, 1, 1, 0x6c, 0x20);
    __Func_8010704(0x7a, 0x14, 1, 1, 0x70, 0x20);
    __Func_8010704(0x7a, 0x14, 1, 1, 0x74, 0x20);
    __Func_8010704(0x7a, 0x14, 1, 1, 0x78, 0x20);

    if (__GetFlag(0x311)) {
        __Func_8010704(0x79, 0x14, 1, 1, 0x64, 0x20);
        x = 0xc7 << 19;
        y = 0x82 << 18;
        if (reposition)
            __MapActor_SetPos(9, x, y);
    } else if (__GetFlag(0xc4 << 2)) {
        __Func_8010704(0x79, 0x14, 1, 1, 0x64, 0x20);
        x = 0xcb << 19;
        y = 0x82 << 18;
        if (reposition)
            __MapActor_SetPos(9, x, y);
    }

    if (__GetFlag(0x313)) {
        s = 0x68;
        t = 0x20;
        __Func_8010704(0x79, 0x14, 1, 1, s, t);
        x = 0xcf << 19;
        y = 0x82 << 18;
        if (reposition)
            __MapActor_SetPos(0xa, x, y);
    } else if (__GetFlag(0x312)) {
        s = 0x68;
        t = 0x20;
        __Func_8010704(0x79, 0x14, 1, 1, s, t);
        x = 0xd3 << 19;
        y = 0x82 << 18;
        if (reposition)
            __MapActor_SetPos(0xa, x, y);
    }

    if (__GetFlag(0x315)) {
        s = 0x6c;
        t = 0x20;
        __Func_8010704(0x79, 0x14, 1, 1, s, t);
        x = 0xd7 << 19;
        y = 0x82 << 18;
        if (reposition)
            __MapActor_SetPos(0xb, x, y);
    } else if (__GetFlag(0xc5 << 2)) {
        s = 0x6c;
        t = 0x20;
        __Func_8010704(0x79, 0x14, 1, 1, s, t);
        x = 0xdb << 19;
        y = 0x82 << 18;
        if (reposition)
            __MapActor_SetPos(0xb, x, y);
    }

    if (__GetFlag(0x317)) {
        s = 0x70;
        t = 0x20;
        __Func_8010704(0x79, 0x14, 1, 1, s, t);
        x = 0xdf << 19;
        y = 0x82 << 18;
        if (reposition)
            __MapActor_SetPos(0xc, x, y);
    } else if (__GetFlag(0x316)) {
        s = 0x70;
        t = 0x20;
        __Func_8010704(0x79, 0x14, 1, 1, s, t);
        x = 0xe3 << 19;
        y = 0x82 << 18;
        if (reposition)
            __MapActor_SetPos(0xc, x, y);
    }

    if (__GetFlag(0x319)) {
        s = 0x74;
        t = 0x20;
        __Func_8010704(0x79, 0x14, 1, 1, s, t);
        x = 0xe7 << 19;
        y = 0x82 << 18;
        if (reposition)
            __MapActor_SetPos(0xd, x, y);
    } else if (__GetFlag(0xc6 << 2)) {
        s = 0x74;
        t = 0x20;
        __Func_8010704(0x79, 0x14, 1, 1, s, t);
        x = 0xeb << 19;
        y = 0x82 << 18;
        if (reposition)
            __MapActor_SetPos(0xd, x, y);
    }

    if (__GetFlag(0x31b)) {
        s = 0x78;
        t = 0x20;
        __Func_8010704(0x79, 0x14, 1, 1, s, t);
        x = 0xef << 19;
        y = 0x82 << 18;
        if (reposition)
            __MapActor_SetPos(0xe, x, y);
    } else if (__GetFlag(0x31a)) {
        s = 0x78;
        t = 0x20;
        __Func_8010704(0x79, 0x14, 1, 1, s, t);
        x = 0xf3 << 19;
        y = 0x82 << 18;
        if (reposition)
            __MapActor_SetPos(0xe, x, y);
    }
}
