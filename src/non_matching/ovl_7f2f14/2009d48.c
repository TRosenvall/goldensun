/* OvlFunc_968_2009d48 -- 0x02009d48,
 * asm/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_a.s
 *
 * 149 differing encodings out of 206 -- but read the shape before the number.
 * SIZE IS EXACT (480 bytes), the instruction count is exact, and the RELOCATION
 * LIST IS THE ROM'S EXACTLY: same 31 entries, same symbols, same order, with
 * only the offsets shifted. Every call site and both pool symbols
 * (gScript_968__0200d3c4, .L5ce8) are already correct.
 *      first at index 7: ref b083  ours 2200
 * Candidate at scratch_elev/b248/f2009d48/final.c.
 *
 * ONE BLOCKER, AND IT IS THE FIRST DIFFERING ENCODING. The ROM emits
 * `sub sp, #0xc` and SPILLS the OvlFunc_968_2008098 result to [sp, #8]; gcc
 * keeps it in r11 and emits `sub sp, #8`. All 149 are the resulting rename --
 * ROM a=r6 t=r7 p=r11 against ours a=r5 t=r6 p=r7 e=r11.
 *
 * BLOCKER CLASS: INTERFERENCE-GRAPH COLOURING. Nine long-lived pseudos (a, the
 * shared 0x2d, q=&a->f23, p, t, x, z, r=&a->f55, and the constant 2) all
 * conflict with e. gcc colours them with SIX registers because p/r do not
 * overlap (they share r7) and 0x2d/t do not overlap (they share r6), which
 * leaves r11 free for e. The ROM's build did not share p with r, needed seven,
 * and spilled e.
 *
 * THE DISCRIMINATOR IS CONFIRMED, AND THE SPILL IS REACHABLE. Defining
 * `r = &a->f55;` anywhere before the `p->f10` test makes p and r conflict and
 * reproduces `sub sp, #0xc` EXACTLY -- verified at four different placements.
 * It costs +2 encodings, because gcc then puts r in r11
 * (`mov r2,#0x55 / add r2,r5 / mov r11,r2`) where the ROM has
 * `mov r5,r6 / add r5,#0x55`.
 *
 * So this is NOT "allocation looks different". The spill is producible on
 * demand; the residue is narrowed to WHICH OF p/r TAKES THE HIGH REGISTER, and
 * that is where the next round should start. Anyone re-screening should begin
 * from final.c plus the r-definition, at 2 encodings, not from scratch.
 *
 * MEASURED INERT: two locals before the store at the else-arm __Func_8010704;
 * declaration order; `void *e[1]` (gcc scalarises it into r11); `&e`; `int e`;
 * `struct Actor *e`; hoisting x/z above the if/else; a named variable for the
 * shared 0x2d; an explicit copy of p before its dereference.
 * MEASURED WORSE: any `*q |= m` spelling other than `*q = *q | m`, +/-2.
 *
 * LANDING SHAPE IF CLOSED: one src/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_a.c
 * holding BOTH functions in the .s (it has two and no data sections).
 * overlay.ld:71 already names the object and MUST STAY NAMING asm/.
 * makefile_flags() is empty, so default -O2.
 *
 * THE SIBLING WAS SIZED AND DELIBERATELY NOT ATTEMPTED. OvlFunc_968_2009af0 is
 * 248 instructions -- a 480-iteration particle loop with three __Random
 * multiply chains, four distinct high registers live through the loop, a
 * mid-loop __CopyMapTiles fork and a Func_8012330 fade. With 2009d48 unclosed
 * the file cannot land whole regardless, so the larger one was left.
 */
