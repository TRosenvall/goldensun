/* Func_8015fb8  --  0x08015fb8
 *
 * Cut from goldensun/asm/rom_15000/rom_15e8c_a_c_a_a.s (second of five); the
 * first function stays in _a.s and the last three in _c.s. No data anywhere in
 * the file, so the split is a pure text cut, verified byte-neutral first.
 *
 * DMA-copies one tile and clears the trailing bytes of its destination.
 *
 * THE r9 TRAFFIC IS A STATIC CHAIN: THE ORIGINAL WAS A NESTED FUNCTION.
 * gcc-2.96's arm.h sets STATIC_CHAIN_REGNUM to r8 under ARM and r9 under Thumb,
 * so a Thumb function that READS r9 without ever defining it is reading a static
 * chain pointer. The tell is a triple: r9 saved in the prologue and restored in
 * the epilogue, a `mov rX, r9` with no defining write anywhere in the function,
 * and a lone stack slot that nothing ever reads back.
 *
 * The caller settles it. Func_8016018 -- still in _c.s -- does
 * `add rN, sp, #4 / mov r9, rN` immediately before each of its three calls
 * here, which is gcc handing a pointer to its own frame to a nested function.
 * That is the corpus-wide grep for this class: `add rN, sp, #K / mov r9, rN`
 * before a `bl`.
 *
 * A STANDALONE TU CANNOT DECLARE A NESTED FUNCTION, so the chain is transcribed
 * rather than expressed: an uninitialised `register` bound to r9, copied into a
 * volatile stack slot as the first statement. The `volatile` is load-bearing --
 * without it gcc dead-store-eliminates the slot AND the whole r9 save/restore
 * with it, and pointer indirection does not substitute for it.
 *
 * THIS IS A TRANSCRIPTION, NOT THE ORIGINAL SHAPE, AND IT IS IMPROVABLE. The
 * caller lives in the SAME parent .s. Once _c.s is elevated, the pair can be
 * written the way the original almost certainly was -- Func_8016018 with this
 * function nested inside it -- and the register binding and volatile slot can
 * both go. Whoever elevates _c.s should try that before keeping this file as
 * it stands.
 *
 * Three smaller results, all measured. Naming the call's argument in its own
 * local BEFORE assigning the function pointer is what puts the pointer load
 * after the argument move, leaving a register free for the address constant.
 * The library clear macro is the wrong spelling here because it orders its
 * three statements differently and reinstates the early pointer load -- where
 * the ROM shows the pointer load after the argument, hand-write the statements
 * in the ROM's order. And the last two instructions were the return type: the
 * recorded `pop {rN} / bx rN` rule held even though an r9 restore had already
 * consumed the usual scratch register.
 *
 * One correction to a recorded sweep: this function's pool MIXES a symbol with
 * integer constants, which the notebook says never occurs in gcc output. It
 * occurs, and reference order reproduced the ROM's pool exactly.
 */
#include "gba/types.h"
#include "dma.h"

extern void Func_80008d4(void *dst, int size);

int Func_8015fb8(int src, int dst)
{
    volatile u32 chain;
    register u32 _chain __asm__("r9");
    void (*fp)(void *, int);
    void *p;

    chain = _chain;
    DMA3_COPY16((void *)(0x6000010 + (src & 0x3ff) * 32),
                (void *)(0x6000000 + (dst & 0x3ff) * 32), 0x20);
    p = (void *)(0x600000c + (dst & 0x3ff) * 32);
    fp = Func_80008d4;
    fp(p, 0x14);
}
