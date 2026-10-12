/* OvlFunc_903_2008fc8 -- NON-MATCHING, 31 differing STREAM encodings of 87.
 *
 * RE-MEASURED batch 343 brief D.  31 of 87 CONFIRMED.  ref 87 / ours 87 stream
 * encodings, 79/79 16-bit INSTRUCTIONS, 6/6 relocations with the SAME SYMBOLS
 * IN THE SAME ORDER.
 *
 * THE RELOCATION "DIFFERENCE" IS NOT A DEFECT, AND autosweep FLAGS IT.
 * Exactly one relocation OFFSET differs -- `__EquipItem` at 0x000000a4 in the
 * ROM against 0x0000009e in ours.  Every symbol, kind and order is identical.
 * That 6-byte shift is a CONSEQUENCE of the single residue below, not a second
 * problem: no symbol is unresolved and no .sym literal is involved.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/2008fc8.c \
 *     asm/overlays/rom_798dc4/ovl_314_c_c_c_c_b.s --func OvlFunc_903_2008fc8
 *
 * PINS: 0 (tools/shimcount.py).  datacheck.py: no data under the reference.
 * tools/piece.py: the reference .s holds THIS FUNCTION ALONE.
 *
 * ================= THE 31 ARE *ONE* DECISION, NOT 31 CHOICES =================
 *
 * Index-aligned side-by-side disassembly shows the two streams are IDENTICAL
 * except that THE LITERAL POOL IS IN A DIFFERENT PLACE:
 *
 *   ROM    ...  59 b.n <top>   60 .short 0 (align fill)  61 .word 0x000008ff
 *               62 adds r6,#1  ...  86 bx r0            END
 *   ours   ...  59 b.n <top>   60 adds r6,#1  ...  84 bx r0
 *               85 .short 0    86 .word 0x000008ff      END
 *
 * The ROM dumps the pool at the BARRIER AFTER THE `found:` BLOCK'S `goto top`;
 * we dump it after the epilogue.  Of the 31 differing encodings, 27 are the
 * resulting 2-encoding rotation of the tail, and the other FOUR are its
 * arithmetic consequences: the pool-load displacement at index 42
 * (`ldr r2,[pc,#40]` vs `[pc,#92]`) and three branch displacements at indices
 * 28, 52 and 55.  FIX THE POOL POSITION AND THE FUNCTION LANDS.
 *
 * -------------------------------- THE MECHANISM, WITH LINE NUMBERS ----------
 * For Thumb the pool is NOT `output_constant_pool` (varasm.c:1101 puts that
 * AFTER the function).  It is arm_reorg's minipool: MACHINE_DEPENDENT_REORG at
 * arm.h:2524 calls arm_reorg (arm.c:5459), which emits via dump_minipool
 * (arm.c:5132) -- and dump_minipool's `gen_label_rtx / align_4 /
 * minipool_vector_label / consttable_4` is exactly the `.L17: .align 2,0 /
 * .L16: .word 2303` our .s carries.
 *
 * arm_reorg chooses the dump point at arm.c:5517-5534: it walks the fix list
 * forward and sets `last_barrier` to the LAST natural barrier whose address is
 * still below `minipool_vector_head->max_address`, then dumps there.
 * max_address is `fix->address + fix->forwards` (add_minipool_forward_ref,
 * arm.c:4825) and `fix->forwards` is the insn's `pool_range` attribute
 * (push_minipool_fix, arm.c:5372).
 *
 * MEASURED FROM THE .26.mach DUMP of this very body:
 *     ;; SImode fixup for i105; addr 100, range (0,1020): 0x8ff
 *     ;; Emitting minipool after insn 289; address 254
 *     ;;  Offset 0, min -65536, max 1120 0x8ff
 * and the pre-reorg barriers are insns 146, 159 and 289 (289 is the one after
 * the epilogue; 295 is dump_minipool's OWN barrier).  Barrier 146 is the ROM's
 * position.  With max_address 1120 in a 188-byte function, EVERY barrier is in
 * range, so the loop necessarily selects the last one -- 289.
 *
 * SO WITH ONE SImode FIX THIS PLACEMENT IS UNREACHABLE, and the reason is a
 * SINGLE NUMBER: `*thumb_movsi_insn` has pool_range 1020 (arm.md:3856).
 *
 * ------------------------------ WHAT THE CORPUS SAYS, AND IT IS DECISIVE ----
 * scratch_elev/b343/D/d_poolscan.py scans the GENERATED half of asm/ (every
 * such .s is byte-matching by construction).  There are 448 mid-function pool
 * dumps, 34 of them single-word; classified by whether the `b` in front of the
 * pool TARGETS THE LABEL JUST AFTER IT (i.e. gcc inserted it) there are
 * 244 INSERTED and 145 NATURAL.  So the ROM's shape IS producible.
 *
 * EVERY natural-barrier single-word example read by hand is driven by an
 * HImode pool load, e.g. asm/overlays/rom_7cb2c0/ovl_30_a_c_c_a_b.s
 * (OvlFunc_945_2008284, `ldrh r3, .L11` then the pool after a natural `b .L2`)
 * and asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c_c.s.  THAT IS THE QUANTIFIED
 * DIFFERENCE: `*thumb_movhi_insn` has pool_range **64** (arm.md:4353) against
 * SImode's 1020.  64 puts max_address at ~164 here, which falls between
 * barrier 146 and the next one -- precisely the window that makes 146 the last
 * barrier in range.
 *
 * THE NEXT PROBE IS THEREFORE NAMED: the ROM's load is `ldr` (encoding 4a0a,
 * SImode) so the mask itself cannot be HImode, which means the body must be
 * carrying a SECOND pool fix with a small max_address, after barrier 146.
 * arm.c:5536-5551 then decrements that fix's refcount and pushes it into the
 * NEXT pool -- so a second fix can move the 0x8ff pool to barrier 146 WITHOUT
 * adding a word to it.  Find a legitimate second pooled constant (an HImode one
 * is the shape the corpus shows) in the l2 loop or the `__EquipItem` loop.
 * DO NOT re-derive the arm_reorg walk; it is above with line numbers.
 *
 * BOTH CHEAP AXES ARE CLOSED (tools/autosweep.py, batch 343):
 *   -fno-gcse 31, -fno-strict-aliasing 31, -fno-strength-reduce 31,
 *   -fno-rerun-cse-after-loop 31, -fno-rerun-loop-opt 31,
 *   -fno-schedule-insns2 42, -O1 55, -ffixed-r7 73.
 *   All 36 top-level anchor placements inert or worse.
 *
 * MEASURED THIS ROUND, all aimed at the pool position:
 *   nested anchor at `l2:`                                         31  inert
 *   nested anchor at `found:`                                      31  inert
 *   nested anchor before the second `p = unit; mask = 0x8ff;`       31  inert
 *   `unsigned int mask` instead of `int mask`                       31  inert
 *   DUPLICATING the `found:` block into both call sites             31  inert
 *       -- gcc cross-jumps it straight back, which is the same datum the
 *          piece-mate Func_80a3ef0's park records: the merged tail is the
 *          COMPILER'S, not the source's.
 *   `info[0xc] == 1` tested before the mask test                    36  worse
 *   `mask` hoisted above the `top:` label                           75  worse
 *   the mask written inline with no local                           80  worse
 *                                                      AND 8 BYTES SHORT
 */
extern unsigned char *__GetUnit(int i);
extern int __GiveItemTo(int who, int item);
extern unsigned char *__GetItemInfo(int id);
extern void __Func_8078948(int who, int slot);
extern void __EquipItem(int who, int slot);

void OvlFunc_903_2008fc8(void)
{
    unsigned char *unit;
    unsigned char *p;
    unsigned char *info;
    int n, j, item, mask, v, z;
    unsigned char *q;

    item = 0x41;
    unit = __GetUnit(2);
    n = 0;
top:
    n = n + 1;
    if (n > 0xfa << 2) {
        q = unit;
        q += 0xf4;
        z = 0;
        *(unsigned short *)q = z;
    }
    if (__GiveItemTo(2, 0x41) == -1) {
        p = unit;
        j = 0;
        p += 0xd8;
    l1:
        info = __GetItemInfo(*(unsigned short *)p);
        p += 2;
        if (info[2] == 1)
            goto found;
        j++;
        if (j <= 0xe)
            goto l1;
        p = unit;
        mask = 0x8ff;
        j = 0;
        p += 0xd8;
    l2:
        info = __GetItemInfo(*(unsigned short *)p);
        if ((*(unsigned short *)(info + 2) & mask) == 0 && info[0xc] == 1) {
        found:
            __Func_8078948(2, j);
            goto top;
        }
        j++;
        p += 2;
        if (j > 0xe)
            goto top;
        goto l2;
    }
    p = unit;
    j = 0;
    p += 0xd8;
l3:
    v = *(unsigned short *)p;
    p += 2;
    if (v == item)
        __EquipItem(2, j);
    j++;
    if (j <= 0xe)
        goto l3;
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_903_2008fc8 -- 0x02008fc8,
 * asm/overlays/rom_798dc4/ovl_314_c_c_c_c_b.s
 *
 * FAKEMATCH, and a more alarming one than DecodeMetatileset because it passed
 * every screen available.  Candidate kept at scratch/hold_2008fc8.c.
 *
 *   - tryc.py against a single-function reference: OK, 94 lines.
 *   - The size check RAN and PASSED.
 *   - `arm-none-eabi-objdump -h` on the two objects: .text 0xbc both, .data 0
 *     both, .bss 0 both -- byte-for-byte identical section sizes.
 *   - The linked overlay differs from orig.bin in 58 bytes.
 *
 * The one warning it did carry: "the reference keeps its literal pool INSIDE
 * the function".  Pool loads normalise to `=value` in the screen, so a pool
 * holding different values at the same distance compares equal.  That is where
 * the 58 bytes are.
 *
 *   The inline-pool warning is not cosmetic.  Two of this round's screens
 *   carried it and both were fakematches.  When it appears, `make compare` is
 *   the only authority -- an equal instruction stream AND an equal .text size
 *   still do not settle it.
 *
 * A NOTE ON THE SPLIT, which is sound and has been kept.  This .s held a
 * trailing `.section .data` defining gOvl_020092f8/02009358/02009368/02009488,
 * and the overlay linker script named the object twice, once for (.text) and
 * once for (.data).  Deleting the .s to replace it with a .c broke the link on
 * those four symbols.  The fix -- text into `_b`, data into `_c`, and the two
 * linker-script lines pointed at the two new objects -- was verified
 * byte-neutral with the function still in asm BEFORE any .c was introduced,
 * which is what let the fakematch be attributed to the C rather than to the
 * layout.  Do that check in that order every time.
 *)

*/
#endif
