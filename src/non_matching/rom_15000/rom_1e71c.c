/* SetTextColor @ 0x0801e71c -- asm/rom_15000/rom_1de5c_c_a.s
 *
 * Source asm: goldensun/asm/rom_15000/rom_1de5c_c_a.s
 *
 * Eight instructions against eight, and the divergence is the SMALL-CONSTANT
 * POOL TELL documented in docs/elevation.md:
 *
 *     rom    ldr r2, =0xf     (pooled)
 *     ours   mov r2, #0xf     (immediate)
 *
 * gcc never pools what it can `mov`, and it always pools the address of a
 * symbol. So the mask was a symbol reference in the original source, and the
 * disassembler resolved it back to 0xf. Writing it as a literal cannot
 * reproduce that, whatever the surrounding C looks like.
 *
 * The ordering also differs -- the ROM masks before computing the destination
 * address, gcc after -- but that is downstream of the same thing: with the
 * mask coming from the pool rather than an immediate, it is available earlier.
 *
 * Two formulations tried: the mask inline in the store, and via a named int.
 * The inline form additionally pools 0xf as a HALFWORD (`ldrh`), the
 * documented class-1 blocker, because the result reaches a strh; the named-int
 * form gets it into an `and` on an int but then uses a `mov` for the constant.
 * Neither can produce a word-sized pool load of 0xf, because gcc has no reason
 * to emit one.
 *
 * BLOCKED ON NAMING, not on technique. What symbol has the value 0xf here?
 * message.sym and file_table.sym are the two id namespaces in the tree and
 * neither is obviously right for a text-ink mask.
 *
 * BATCH 93: THE const.sym ROUTE WAS TRIED AND IS A NET LOSS. Batch 83
 * established `_CONST_2` for exactly this shape -- a value the ROM pools that
 * an eight-bit `mov` could build -- and this function meets that bar on paper.
 * A hypothetical `_CONST_F` taken as `(int)&_CONST_F` DOES put the pool load
 * where the ROM has it, which is the part the literal cannot do. But it costs
 * a register: gcc then keeps the mask in r2 and the address in r1 and lands
 * the `and` in r2, where the ROM reuses r2 for both and lands the `and` in r0.
 *
 *     rom          ldr r2, =0xf / and r0, r2 / ldr r2, =0xeae / add r3, r2
 *     _CONST_F     ldr r2, =sym / ldr r1, =0xeae / and r2, r0 / add r3, r1
 *
 * Five spellings of it were measured -- named int, inline, `mask & colour`,
 * `colour &= mask`, and a separate pointer local -- and all five give the same
 * five differing positions. The plain literal gives FOUR. So the symbol is not
 * simply the missing piece; something else also has to move, and until that is
 * known adding `_CONST_F` to const.sym would be adding an entry that does not
 * pay for itself. No entry was added.
 *
 * The literal form is what is kept below, because it is the closer of the two.
 * -O1, -fno-rerun-cse-after-loop and -fno-expensive-optimizations were also
 * screened against it and none improves on four.
 */
extern unsigned char *iwram_3001e8c;

/* r0 = colour, masked to four bits and stored as the ink field at +0xEAE --
 * the one Func_173ac defaults to 0x0F.
 */
void SetTextColor(int colour)
{
    *(unsigned short *)(iwram_3001e8c + 0xeae) = colour & 0xf;
}
