// fakematch
/*
 * InitMenuLayer -- asm/rom_15000/rom_1908c_c_c_b_b.s
 *
 * fakematch: the sentinel value is pinned to r2.
 *
 * Two separate readings hold this together.
 *
 * 1. The value is HImode, and that is what the POOL ORDER says.  The ROM's
 *    pool is 0x3e7, &iwram_3001e8c, 0x12ec -- 0x3e7 FIRST although its `ldr` is
 *    the fourth instruction.  add_minipool_forward_ref sorts by
 *    address + pool_range; SImode is 1020, so a word constant loaded at
 *    address 6 could never sort ahead of the symbol loaded at address 0.
 *    *thumb_movhi_insn has pool_range 64, which puts it first.  A `short`
 *    LOCAL does not do it (PROMOTE_MODE widens it to SImode); a `register`
 *    declaration keeps the declared mode, and so does a bare literal at the
 *    store.
 *
 * 2. The address is materialised into a register because each store goes
 *    through a named pointer recomputed from the base; cse2 would otherwise
 *    re-form `[base, off]`.  (`volatile` at the store does the same job and is
 *    NOT needed once the pin is in -- measured byte-identical either way.)
 *
 * Without the pin this is 6 encodings out: r2 and r3 are exchanged between the
 * base pointer and the value.  That is an allocno-priority tie and the numbers
 * do not move -- the base is 3 refs / 5 insns and the value 3 refs / 8 insns in
 * every one of nineteen spellings measured.
 */
extern unsigned char *iwram_3001e8c;

void Func_8019d0c(void)
{
    unsigned char *p;
    int off;
    register short v __asm__("r2");
    unsigned char *q;

    off = 0x12ec;
    p = iwram_3001e8c;
    v = 0x3e7;
    q = p + off;
    *(short *)q = v;
    q = p + off + 2;
    *(short *)q = v;
}
