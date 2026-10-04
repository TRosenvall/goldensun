/* Func_8078144 -- p3, batch 321 bucket C.  PARK, NOT A LANDING.
 *
 * FIGURE 4 of 103 encodings, SIZE EXACT (ref 103 / ours 103, no COUNT, no MEM).
 *   Measured, confirming the batch-319 backfill figure.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_77000/8078144.c \
 *     asm/rom_77000/rom_77320_a_c_c.s --func Func_8078144
 *
 * THE RELOCATION WARNING IN THE PARK HEADER IS NOT A DIRTY FIGURE.  All nine
 * relocations sit at identical offsets; the only delta is `.L7a828` against
 * `_TBL_7a828` at 0xd8, an ALIAS for the same address.  The figure IS a distance.
 *
 * SPLIT, if it ever lands: three functions, this one is the LAST.
 *   python3 tools/split_s.py asm/rom_77000/rom_77320_a_c_c.s Func_8078144
 *   [dry-run] would write ..._a.s (2 functions, 237 lines) and ..._b.s (1 function, 115 lines)
 *   -> src/rom_77000/rom_77320_a_c_c_b.c.  datacheck.py: CLEAN.
 * PINS: 0.  Devices: none.  Flags: none.
 *
 * ================ _TBL_7a828 IS WITHHELD, AND THAT IS THE CORRECT RESULT ================
 *
 * TEST 1 (evidence): PASSES, on label.sym's own bar rather than on a pool tell.
 *   The ROM has `ldr r3, =.L7a828`, an assembler-local label at
 *   asm/rom_77000/rom_77320_c_c_c_b.s:13 that is unreachable from C, and it is
 *   already `.global` in that .s -- which is label.sym's stated bar.  Re-verified.
 *   (aliases.txt:537 has BYTE_ARRAY_0807a828, but aliases.txt is NOT INCLUDEd by
 *   stage1.ld, so it does not link and is not a substitute.  Do not reach for it.)
 *
 * TEST 2 (completion): FAILS.  With the entry, FOUR encodings still differ.  That
 *   is the _FILE_e4 shape exactly -- an accepted argument that buys a relocation
 *   line and leaves real differences -- so the entry stays out.  A build input is
 *   worth adding when it COMPLETES a function, not when it improves one.
 *
 * ================ THE PARK'S "THE TWO RESIDUES TRADE" DIAGNOSIS IS REFUTED ================
 *
 * The park says source order A keeps the scratch registers right but sinks
 * `strh [r5, #0x3a]` below the first `ldrsh` (6 differing), and order B keeps the
 * store put but swaps the scratch registers (4 differing), so "no source order can
 * satisfy both".  REPRODUCED: swap-ldrsh-order does measure 6.  REFUTED as a
 * characterisation of the 4, because in the body below BOTH the store position AND
 * the load order are ALREADY the ROM's.  Ours against ref:
 *
 *     ldrh  r3, [r5, #0x36]      ldrh  r3, [r5, #0x36]
 *     strh  r3, [r5, #0x3a]      strh  r3, [r5, #0x3a]
 *     mov   r3, #0x38            mov   r2, #0x38       <-- only difference
 *     ldrsh r0, [r5, r3]         ldrsh r0, [r5, r2]
 *     mov   r2, #0x34            mov   r3, #0x34
 *     ldrsh r1, [r5, r2]         ldrsh r1, [r5, r3]
 *
 * SO THE WHOLE RESIDUE IS ONE REGISTER-ALLOCATION CHOICE: which of r2/r3 holds
 * which `ldrsh` offset.  (Thumb `ldrsh` has only the register-offset form, which
 * is why the two scratch `mov`s exist at all.)  gcc reuses r3 the instant `strh`
 * frees it; the ROM's allocator took r2 first and r3 second.  There is no trade
 * and no scheduling question left -- 4 is a pure allocation residue, and that is a
 * better map for pass 3 than "the residues trade".
 *
 * MEASURED HERE (crossfire, depth 2, device-free): base 4, and FLAT at 4 across
 * drop-unused-k, decl-r0-after-r1, decl-r3-first, short-ptr-subscript,
 * short-ptr-local and every pair of them -- 16 rows, all exactly inert.
 * swap-ldrsh-order 6 (and 6 crossed with everything).  separate-copy-local 7.
 * swap + separate-copy 9.  Plus the park's own list: inline index 29, offset
 * locals 29, signed-short store 75, one-statement copy 29, separate store pointer
 * 29, store between loads 29, store after both loads 28, offset locals either
 * order 27, --no-sched2 27.
 *
 * THE SWEEP IS FLAT, so per the brief the lever is not in any dimension swept.
 *
 * ================ A FREE DIVIDEND: THE REAL STRUCT, AND IT IS BYTE-IDENTICAL ================
 *
 * The brief's humanization pattern 4 was tried properly: all ELEVEN raw-offset
 * accesses `*(short *)((char *)r5 + N)` were replaced by named fields of a
 * `struct Unit *` with 0x14/0x16/0x34/0x36/0x38/0x3a declared as `short`
 * (scratch_elev/b321/C/v_78144_struct.c, zero `(char *)r5` left).  Result:
 *   4 of 103, first at index 42 -- IDENTICAL OUTPUT, down to the index.
 * So the typed struct is NOT a lever here, but it is free: same bytes, much better
 * C.  Worth adopting on code-quality grounds whenever this function is revisited,
 * and worth recording as a measured negative for pattern 4 on an allocation residue.
 *
 * WHAT PASS 3 SHOULD TRY: this is a two-register allocation swap in local-alloc,
 * not a source-shape problem.  The levers left are the ones the brief names when a
 * sweep goes flat -- the SIGNATURE of GetUnit (its return type reaching this body),
 * or the TU shape (this function's two file-mates, which a split would separate).
 */
extern int GetPartySize(void);
extern void *GetUnit(int unit);
extern int GetFlag(int id);
extern unsigned char gState[];
extern unsigned char L7a828[] __asm__("_TBL_7a828");

void Func_8078144(void)
{
    void *r5;
    int r0;
    int r1;
    int r3;
    int i;
    int n;
    int k;
    int id;
    int t;
    int ok;

    n = GetPartySize();
    for (i = 0; i < n; i++) {
        id = gState[(0xfc << 1) + i];
        t = L7a828[id];
        ok = 0;
        if (t == 0) {
            if (GetFlag(0x88 << 1) != 0) {
                ok = 1;
            } else if (GetFlag(0x89 << 1) != 0) {
                ok = 1;
            }
        } else {
            if (GetFlag(0x111) != 0) {
                ok = 1;
            } else if (GetFlag(0x113) != 0) {
                ok = 1;
            }
        }
        if (ok != 0) {
            r5 = GetUnit(id);
            r3 = *(unsigned short *)((char *)r5 + 0x36);
            *(unsigned short *)((char *)r5 + 0x3a) = r3;
            r1 = *(short *)((char *)r5 + 0x34);
            r0 = *(short *)((char *)r5 + 0x38);
            r0 <<= 14;
            r0 /= r1;
            r3 = 0x80;
            r3 <<= 7;
            if (r0 > r3) {
                r3 = 0x80 << 7;
            } else {
                if (r0 < 0) {
                    r3 = 0;
                } else {
                    r3 = r0;
                }
            }
            *(short *)((char *)r5 + 0x14) = r3;
            if ((r3 << 16) != 0) {
                goto label_0x3a;
            }
            r3 = *(short *)((char *)r5 + 0x38);
            if (r3 == 0) {
                goto label_0x3a;
            }
            r3 = 1;
            *(short *)((char *)r5 + 0x14) = r3;
        label_0x3a:
            r0 = *(short *)((char *)r5 + 0x3a);
            r1 = *(short *)((char *)r5 + 0x36);
            r0 <<= 14;
            r0 /= r1;
            r3 = 0x80;
            r3 <<= 7;
            if (r0 > r3) {
                r3 = 0x80 << 7;
            } else {
                if (r0 < 0) {
                    r3 = 0;
                } else {
                    r3 = r0;
                }
            }
            *(short *)((char *)r5 + 0x16) = r3;
            if ((r3 << 16) != 0) {
                goto label_next;
            }
            r3 = *(short *)((char *)r5 + 0x3a);
            if (r3 == 0) {
                goto label_next;
            }
            r3 = 1;
            *(short *)((char *)r5 + 0x16) = r3;
        }
    label_next:
        ;
    }
}
