/* Func_807808c (0x0807808c) -- asm/rom_77000/rom_77320_a_c_c.s (3 functions).
 *
 * PARK IMPROVED, 4 -> 3 of 86 encodings, DEVICE-FREE.  SIZE EXACT (86 against
 * 86 encodings, 86 instructions both sides), relocations identical, and the
 * per-opcode memory profile is the reference's exactly:
 * ldr=2 ldrb=1 ldrh=2 ldrsh=4 strb=2 strh=6.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_77000/807808c.c \
 *     asm/rom_77000/rom_77320_a_c_c.s --func Func_807808c
 *
 * INSTALLED PATH, if it ever lands: src/rom_77000/rom_77320_a_c_c_b.c.
 * Split shape: TEXT-ONLY, tools/datacheck.py prints nothing (no data section).
 *   tools/split_s.py asm/rom_77000/rom_77320_a_c_c.s Func_807808c --dry-run:
 *     _a.s Func_8077f70 (140 lines), _b.s Func_807808c (97), _c.s Func_8078144 (115).
 * NOTE FOR THE COORDINATOR: all THREE functions in this .s are parked
 * (Func_8077f70 at 3 since batch 325, Func_807808c at 3 here, Func_8078144 at
 * 4), and the two 3-way and 2-way split shapes in their headers are ALTERNATIVE
 * splits of the same file.  Whichever lands first decides the suffixes, and
 * install_batch.py's splits phase must repoint the other two recipes.
 * PINS: 0.  No shim, no fakematch row, no flag group.  No device.
 *
 * ---------------------------------------------------------------------------
 * WHAT CHANGED, AND WHY THE PARK NEVER SAW IT
 *
 * The park's own analysis names this cure and then measures it ONLY WITH A
 * DEVICE.  Its scratch_elev/b322/F/v1/g1.c -- three single-set locals for the
 * sign extend, so local-alloc's `combine_regs` can tie the intermediate to its
 * dying source and emit the ROM's IN-PLACE `lsl r1,r1,#16 / asr r1,r1,#16` --
 * carries `*(volatile unsigned short *)` on BOTH stores, and the 3 it recorded
 * was read with those casts in place.  The park header then reports that 3 as a
 * corner of a three-cornered constraint and keeps the 4.
 *
 * MEASURED HERE: strip the two volatile casts and the SAME body still reads 3.
 * The casts were never load-bearing for this corner.
 *
 *   3   single-set sign-extend chain, plain stores        <- THIS BODY
 *   3   same, 0x38 store moved ahead of the 0x36 read
 *   3   b322's v1/g1.c with both volatile casts deleted
 *   4   the body this park shipped
 *
 * So the park's "corners 2 / 3 / 4" is right about the SHAPE and wrong about
 * which corner is device-free: it is 3.
 *
 * WHY THE SINGLE-SET CHAIN IS THE LEVER.  local-alloc refuses to combine a dest
 * with a dying source when the source "is not local to this block OR DIES MORE
 * THAN ONCE".  With `r1 = (r1 << 16) >> 16;` the variable r1 is SET TWICE in the
 * block (the 0x34 load, then the sign-extend result), so it never gets a
 * quantity and the shift pair cannot be in-place.  Three single-set names
 * (t1, s1 and r1 read-only after the load) do chain, and indices 22/23 go
 * byte-exact.
 *
 * THE REMAINING 3, stated as a defect and not as a verdict.  Indices 21,22,23:
 *
 *      ref                       ours
 *  18  ldrh r1, [r5, #0x34]      ldrh r1, [r5, #0x34]
 *  19  ldrh r3, [r5, #0x36]      ldrh r3, [r5, #0x36]
 *  20  strh r1, [r5, #0x38]      strh r1, [r5, #0x38]
 *  21  strh r3, [r5, #0x3a]      lsls r1, r1, #16
 *  22  lsls r1, r1, #16          asrs r1, r1, #16
 *  23  asrs r1, r1, #16          strh r3, [r5, #0x3a]
 *
 * The in-place pair is now EXACT and the only defect left is that
 * `strh r3,[r5,#0x3a]` sinks below it.  The park's scheduler arithmetic explains
 * why, and it is worth restating because this body makes it sharp: a store's
 * priority comes from whichever later insn OVERWRITES ITS SOURCE REGISTER (an
 * anti dependence, `arm_adjust_cost` cost 0 for REG_DEP_ANTI, so the store
 * inherits that insn's priority exactly).  The ROM's in-place `lsl r1,r1,#16`
 * overwrites r1, which is the 0x38 store's source, so the 0x38 store is lifted;
 * NOTHING overwrites r3 before the call, so the 0x3a store keeps only the call's
 * memory dependence and loses to the shifts.
 *
 * So the open question is narrow and new: **lift the 0x3a store without
 * reintroducing a second set of the sign-extend variable.**  The 4-body lifted
 * it by putting the sign-extend intermediate in r3 -- which is exactly what
 * breaks the in-place pair.  That is the trade, now priced at one encoding
 * instead of two.
 *
 * MEASURED FLAT ON TOP OF THIS BODY (all exactly 3, crossfire depth 2, 10 edits
 * and every compatible pair): swap the two stores; read 0x36 after the 0x38
 * store; the 0x3a copy as one statement; declare t1/s1 after r3; `s1 * 0x4000`
 * instead of `s1 << 14`; `(s1 << 14) / s1` as one expression; drop the unused k;
 * the real `GetUnit(unsigned int)` signature.
 *
 * FREE CORRECTNESS DIVIDEND, measured exactly inert: `GetUnit`'s real parameter
 * type is `unsigned int`, from its landed definition at
 * src/rom_77000/rom_77320_a_a_c_c_a_b.c:136.  The park declared `int`.
 */
extern int GetPartySize(void);
extern void *GetUnit(unsigned int id);
extern unsigned char gState[];

void Func_807808c(int sel)
{
    void *r5;
    int r0;
    int r1;
    int t1;
    int s1;
    int r3;
    int i;
    int n;
    int k;

    n = GetPartySize();
    for (i = 0; i < n; i++) {
        r5 = GetUnit(gState[(0xfc << 1) + i]);
        r1 = *(unsigned short *)((char *)r5 + 0x34);
        r3 = *(unsigned short *)((char *)r5 + 0x36);
        *(unsigned short *)((char *)r5 + 0x38) = r1;
        *(unsigned short *)((char *)r5 + 0x3a) = r3;
        t1 = r1 << 16;
        s1 = t1 >> 16;
        r0 = s1 << 14;
        r0 /= s1;
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
            goto label_sel;
        }
        r3 = *(short *)((char *)r5 + 0x3a);
        if (r3 == 0) {
            goto label_sel;
        }
        r3 = 1;
        *(short *)((char *)r5 + 0x16) = r3;
    label_sel:
        if (sel == 1) {
            *((char *)r5 + 0x131) = 0;
            *((char *)r5 + 0x140) = 0;
        }
    }
}
