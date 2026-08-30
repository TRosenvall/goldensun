/*
 * OvlFunc_881_2009c08 -- asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_b.s
 * SPLIT OUT this round; byte-neutral, verified.
 *
 * BLOCKER: gcc CSEs a repeated pooled constant that the ROM rebuilds.
 * 52 lines against 49 -- THREE OVER, and the three are `push {r5, r6}` and the
 * matching pop.
 *
 * THE DIAGNOSIS, and it generalises. This is a pure straight-line call
 * sequence, 21 calls, no control flow. It calls __ClearFlag(0x16f) twice and
 * uses the flag 0x171 twice. gcc loads each pooled constant ONCE into a
 * callee-saved register so it survives the intervening calls -- our output has
 * `ldr r6, .L3+4` held across four calls -- and pays push/pop for the
 * privilege. The ROM emits `ldr r0, =0x16f` fresh at each use.
 *
 * gcc-2.96 will always CSE two identical pooled constants; there is no
 * statement order or naming that separates them, because they ARE the same
 * rtx. So a ROM that REBUILDS an identical pooled constant across calls is
 * telling you the source referenced TWO DIFFERENT SYMBOLS that happen to share
 * a value -- the same reasoning const.sym's header uses for a pooled small
 * constant, applied to repetition rather than to size.
 *
 * This tree has no flag id symbol space -- area.sym, const.sym, file_table.sym,
 * message.sym and wram.sym are the whole set -- so the symbols are not
 * available to write. Establishing one is the prerequisite for this function
 * and for any other straight-line script that reuses a flag id.
 *
 * NOT TRIED, because it would be guessing at names: inventing two flag symbols
 * at 0x16f and 0x171. The value would match and the names would assert
 * something unestablished, which is the trap const.sym's header warns about.
  *
 * ===> THE SYMBOL CONCLUSION ABOVE IS RETRACTED. <===
 *
 * 76 already-matching functions in this tree rebuild the same mov+lsl constant
 * twice across a call, so the shape is reachable from ordinary C and does not
 * require two symbols. src/overlays/rom_78b2ac/ovl_30_c_c_a_a_a.c uses the
 * literal `0x80 << 2` three times with calls between and gcc rebuilds it every
 * time -- because those three uses sit in DIFFERENT CONDITIONAL BRANCHES.
 *
 * The probe result below is still correct for a STRAIGHT-LINE sequence: there,
 * a repeated literal and a repeated single symbol both CSE, and only two
 * distinct symbols avoid it. What the probe does not establish is that this
 * function's source was straight-line. Suspect unreproduced control flow first.
*/
extern void __Func_808c4c0(void);
extern void __Func_80936a0(int a, int b);
extern void __Func_8093710(void);
extern void __Func_808c44c(void);
extern void __Func_80925cc(int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __Func_802899c(int a, int b);
extern void __ClearFlag(int id);
extern void __SetFlag(int id);
extern void __Func_80aa56c(void);
extern void __MapActor_Jump(int a, int b, int c);
extern void __Func_8091eb0(int a, int b);

void OvlFunc_881_2009c08(void)
{
    __Func_808c4c0();
    __Func_80936a0(0x80 << 9, 6);
    __Func_8093710();
    __Func_808c44c();
    __Func_80925cc(8, 2);
    __MessageID(0xc66);
    __ActorMessage(8, 0);
    __CutsceneWait(0x1e);
    __PlaySound(0x6f);
    __Func_802899c(0, 2);
    __ClearFlag(0x16f);
    __ClearFlag(0x171);
    __Func_80aa56c();
    __MapActor_Jump(8, 4, 0x1e);
    __MessageID(0xc67);
    __ActorMessage(8, 0);
    __ClearFlag(0x16f);
    __SetFlag(0x171);
    __Func_80aa56c();
    __CutsceneWait(0x1e);
    __Func_8091eb0(0xc, 6);
}
