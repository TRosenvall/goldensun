/* OvlFunc_924_200adcc -- 0x0200adcc, 60 bytes, 26 encodings and 1 relocation
 * identical.  Cut out of asm/overlays/rom_7ac2d8/ovl_2dcc_a.s, the file's only
 * function; tools/datacheck.py prints nothing, export list empty, no split.
 * ZERO pins, no devices, no per-file flag group.
 *
 * TWIN: src/overlays/rom_7ac2d8/ovl_22c4_c_c_c_a_a.c (OvlFunc_924_200a648) is
 * the same routine at 0x5000050/0x500005e/0x5000052 with a bound of six.  The
 * body below ports to it unchanged except the three addresses and the bound,
 * and it landed there on the first try.  One edit, two functions.
 *
 * ================= HOW THIS CLOSED, AFTER SIX BATCHES OF PARK =================
 *
 * The park stood at nine differing of twenty-six (size exact, count exact, so a
 * true distance) and had decomposed the residue correctly into two independent
 * runs.  Both of its runs were real; both of its "no C spelling exists"
 * conclusions were wrong, and each fell to a dimension the park had never
 * varied.  The decomposition is worth keeping because it is what made the two
 * edits findable:
 *
 *   RUN B, five encodings -- the save pointer and the halfword temp swap r2 and
 *   r3.  Both pseudos are block-local, so local-alloc decides, and the order is
 *   QTY_CMP_PRI (local-alloc.c:1496-1498):
 *       floor_log2 (n_refs) * n_refs * size / (death - birth), times 10000.
 *   With REG_ALLOC_ORDER (config/arm/arm.h:989) starting 3,2,1,0 the qty
 *   allocated FIRST takes r3, and the ROM needs that to be the save pointer, so
 *   the ROM needs the temp to carry the LONGER span -- the load materialised
 *   before the save address.  Writing the store directly gives the reverse,
 *   because expand_assignment is LHS-first (expr.c:3402).  Naming the temp does
 *   invert the RTL order, but the park's named temps were all INTEGER_TYPE:
 *     - `int` / `unsigned int` / `unsigned long` leave a live subreg copy
 *       (a volatile mem load does not propagate through combine), which lifts
 *       the combined qty's n_refs to four, and floor_log2(4) = 2 puts it back
 *       above the save pointer;
 *     - `unsigned short` / `short` trip PROMOTE_MODE and emit a real lsl/asr
 *       pair, which is why every narrow spelling measured longer than the
 *       reference.
 *   THE UNVARIED DIMENSION IS THE TYPE CONSTRUCTOR.  promote_mode's switch
 *   (explow.c:897-901) promotes INTEGER_TYPE, ENUMERAL_TYPE, BOOLEAN_TYPE,
 *   CHAR_TYPE, REAL_TYPE and OFFSET_TYPE; RECORD_TYPE, UNION_TYPE and
 *   ARRAY_TYPE fall out of the `default:` at :911 with the mode UNCHANGED.  So
 *   a one-member aggregate of `unsigned short` is a genuine HImode pseudo with
 *   n_refs two and no promotion and no subreg copy, and the save pointer wins
 *   r3.  MEASURED, each alone against the park's nine: `struct { unsigned short
 *   v; }`, `union { unsigned short v; }`, `struct { unsigned short v : 16; }`
 *   and `unsigned short t[1]` all read SEVEN, first differing index ten, i.e.
 *   each closes run B entirely and leaves run A.  Making the member `volatile`
 *   destroys it (twenty-eight differing and twelve bytes longer).
 *
 *   RUN A, four encodings -- `s` and `i` swap r2 and r0, a global-alloc defect:
 *   .18.greg's allocno order line reads `32 34 33` where the ROM needs
 *   `32 33 34`, the second-ranked allocno taking r2 and the third r0.  The park
 *   had measured ALL SIX declaration orders of d/s/i inert, and concluded the
 *   tie-break rung (global.c:617) was dead here.  That reading was right and
 *   irrelevant: the lever is not the DECLARATION order, it is the position of
 *   `i = 0`.  HOISTING `i = 0` TO THE TOP OF THE GUARDED ARM, above the pointer
 *   setup, is what fixes the ranking -- it shortens nothing and lengthens `i`'s
 *   live_length, which is the DENOMINATOR of allocno_compare, so `i` sinks
 *   below `s` instead of overtaking it.  Declaration order is inert because it
 *   does not move an assignment; placement is directional and is not guessable
 *   from the inert list.
 *
 * SO THE PARK'S FINAL SENTENCE -- that run B needs a shape no C spelling
 * produces -- was false in the type-constructor dimension, and its run-A bound
 * was false in the placement dimension.  The two levers are INDEPENDENT and
 * COMPOSE: struct alone seven, hoist alone was never isolated on this body, the
 * pair zero.
 *
 * ALSO RETIRED BY THIS: the park's own earlier claim that the blocker was
 * constant derivation (a `sub r2, #0xc` from reload_cse_move2add,
 * reload1.c:8840).  Batch 327 had already shown that derivation to be a
 * CONSEQUENCE of reload putting the save pointer and `s` in one hard register,
 * not a cause; fixing the allocation removes it with nothing aimed at it.  The
 * three-absolute-symbol proposal that was aimed at it stays withdrawn.
 *
 * ONE ARTIFACT, FOR PASS 3/4.  `struct H` exists only to keep the halfword out
 * of promote_mode's switch; a human would have written a plain `unsigned short`
 * here, which measures twenty-seven instructions against twenty-six.  Same
 * class as Func_80bf574 (batch 329).
 */
extern int iwram_3001e40;

struct H { unsigned short v; };

void OvlFunc_924_200adcc(void)
{
    volatile unsigned short *d;
    volatile unsigned short *s;
    unsigned int i;

    if ((iwram_3001e40 & 7) == 0) {
        struct H t;
        i = 0;
        d = (volatile unsigned short *)0x50000c2;
        t.v = *d;
        *(volatile unsigned short *)0x50000ce = t.v;
        s = (volatile unsigned short *)0x50000c4;
        do {
            *d = *s;
            i++;
            s++;
            d++;
        } while (i <= 5);
    }
}
