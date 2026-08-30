/*
 * OvlFunc_882_200bc48 -- asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_c_c_b.s
 * SPLIT OUT this round; byte-neutral, verified.
 *
 * BLOCKER: gcc CSEs a constant the ROM rebuilds. 54 lines against 53 -- ONE
 * OVER, and the one is `push {r5}` with its pop, because gcc hoists
 * `0xb3 << 1` into r5 to survive the calls between the __SetFlag and the
 * __ClearFlag twenty instructions later. The ROM emits `mov r0,#0xb3 /
 * lsl r0,#1` at both sites.
 *
 * SEE the corrected section in docs/elevation.md before attacking this. The
 * shape IS reachable -- 76 matching functions in the tree do it -- but in every
 * one checked the repeated uses sit in different conditional branches, and this
 * function is straight-line in the ROM with no branch at all. So either the
 * original had control flow that the call trace does not reveal, or the
 * register pressure differs. It is NOT established that two symbols are
 * required, and one symbol used twice CSEs exactly like a literal.
 *
 * The rest of the function is exact: 22 calls, the six-times-repeated channel
 * loops written out rather than looped (a loop gives an induction variable the
 * ROM does not have), and 0x166 built as `0xb3 << 1` rather than pooled.
 */
extern void __WaitFrames(int n);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_80118c0(int n);
extern void __Func_80118a8(int n);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int n);

void OvlFunc_882_200bc48(void)
{
    __WaitFrames(0x14);
    __SetFlag(0xb3 << 1);
    __Func_80118c0(0);
    __Func_80118c0(1);
    __Func_80118c0(2);
    __Func_80118c0(3);
    __Func_80118c0(4);
    __Func_80118c0(5);
    __Func_8091200(0x10003, 1);
    __Func_8091200(0x80 << 9, 2);
    __Func_8091254(1);
    __WaitFrames(0x78);
    __Func_8091200(0, 0);
    __Func_8091254(0x3c);
    __WaitFrames(0x3c);
    __ClearFlag(0xb3 << 1);
    __Func_80118a8(0);
    __Func_80118a8(1);
    __Func_80118a8(2);
    __Func_80118a8(3);
    __Func_80118a8(4);
    __Func_80118a8(5);
}
