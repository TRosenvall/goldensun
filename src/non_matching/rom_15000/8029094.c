/* Debug_WarpMenu_UI -- 0x08029094, the only function in
 * asm/rom_15000/rom_23178_a_c_c_a.s (grep -c func_start = 1), so it converts
 * WHOLE-FILE; no data section (datacheck.py exits 0), NO SPLIT needed.
 *
 * STILL A PARK. 17 of 163 PIN-FREE, re-measured in batch 323 at production
 * flags (163 / 163 instructions, +0 bytes, so 17 IS A TRUE DISTANCE).
 *
 *   *** 0 of 163 WITH ONE REGISTER PIN -- byte-identical, 163/163, +0 bytes.
 *   *** TWO INDEPENDENT ONE-PIN ROUTES, below. Parked under the pin policy
 *   *** (owner decision 3): prefer a pin-free body; record the pinned figure
 *   *** beside it and leave the landing for pass 3. This is the same shape as
 *   *** Func_80979a4 (owner decision 3, DEFERRED TO PASS 3) -- see
 *   *** reports/pass3-depin.md.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/8029094.c \
 *     asm/rom_15000/rom_23178_a_c_c_a.s --whole
 *
 * ON THE SYMBOL AND THE RECIPE, because batch 323's scan flagged this park as
 * having a malformed recipe. IT IS NOT MALFORMED: there is no `--func` to
 * extract because this is a `--whole` recipe. The subject is confirmed from
 * the .s and not from the filename --
 *   asm/rom_15000/rom_23178_a_c_c_a.s:6:
 *     .thumb_func_start Debug_WarpMenu_UI  @ 0x08029094
 * -- so `Func_8029094` IS NOT A SYMBOL in this tree; the park's filename is an
 * address. The neighbours Func_8029274 and Func_80292c4 are unrelated.
 *
 * ========================== WHAT THE RESIDUE IS ==========================
 *
 * ALL 17 ARE ONE CAUSE: pseudo 38 (`d`) and pseudo 39 (`&gKeyRepeat`) hold
 * each other's hard register. The ROM puts `d` in r0 and &gKeyRepeat in r6; we
 * do the reverse. Printed with operands:
 *     idx 1   ldr r6,=gKeyRepeat | mov r7,r0
 *     idx 2   mov r7,r0          | ldr r0,=gKeyRepeat
 *     idx 3   mov r0,r3          | mov r6,r3
 *     idx 4,15,23,28,38,62,87,125   ldr r3,[r6] | ldr r3,[r0]
 *     idx 33,36                     ldrh/strh [r0] | [r6]
 *     idx 44,68,94,132              ldrsh [r0,r2] | [r6,r2]
 * The idx 1/2 order difference is a CONSEQUENCE: with the pool load targeting
 * r0 it cannot be scheduled before `mov r7,r0`.
 *
 * ================= BATCH 323: A SECOND ROUTE, FROM .18.greg =================
 *
 * The park framed this purely as an allocation ORDER problem and reduced it to
 * a priority inequality. That framing is CORRECT AND INCOMPLETE. `.18.greg`'s
 * conflict rows say there is a second, independent route the park never named:
 *
 *     ;; 38 conflicts: ... 1 2 3 13      <- hard regs 1, 2, 3
 *     ;; 39 conflicts: ... 2 3 13        <- hard regs 2, 3 ONLY
 *
 * **p39 has NO CONFLICT WITH r0 AND NO CONFLICT WITH r1.** p38's only
 * call-used option is r0 (it conflicts with r1, r2, r3). So p39 takes r0
 * simply because it is allocated first AND r0 is open to it, and p38 is then
 * pushed to r6. EITHER of these closes the function:
 *
 *   ROUTE A (the park's): re-rank, so p38 is allocated first. Needs
 *            pri(38) >= pri(39) = 4500. allocno_compare (global.c:597) is
 *            `(floor_log2(n_refs) * n_refs / live_length) * 10000 * size`,
 *            truncated to int, tie-broken BY ALLOCNO NUMBER ASCENDING
 *            (`return v1 - v2`), and allocno numbers follow pseudo numbers --
 *            so a TIE GOES TO p38. With R38 = 7, L38 = 44, L39 = 60, R39 = 9:
 *                R38 = 7 and L38 <= 31     (14/L * 10000 >= 4500)
 *                R38 = 8 and L38 <= 53     (the floor_log2 step at 8 does it)
 *                R39 <= 7                  (ruled out: the ROM genuinely reads
 *                                           gKeyRepeat eight times and loads
 *                                           its address once)
 *                L39 >= 85                 (nothing extends it; its last use
 *                                           is the 0x200 test)
 *   ROUTE B (new): leave the order alone and MANUFACTURE A CONFLICT BETWEEN
 *            p39 AND r0. p39 is born at the first gKeyRepeat read, which
 *            expand places AFTER all four parameter copies, so r0 (holding `a`)
 *            is already dead. In the ROM `ldr r6,=gKeyRepeat` is the FIRST body
 *            insn -- the address pseudo is born while r0..r3 still hold
 *            parameters, which forces it off every call-used register and onto
 *            r6, and p38 then takes r0 unopposed. No source form reached that
 *            ordering: expand_function_start emits the parameter copies before
 *            any body statement, and four named-pointer forms (below) are
 *            exactly inert.
 *
 * WHICH DUMP TO READ, because this cost a round. `.12.life` prints
 * `Register 39 used 9 times across 30 insns`, i.e. L39 = 30 -- and those
 * figures DO NOT reproduce `.18.greg`'s printed allocno order (they put 39
 * third, ahead of 33 and 37). The park's L38 = 44 / L39 = 60 DO reproduce it
 * exactly, on all ten allocnos, so `reg_live_length` is recomputed between
 * `.12.life` and global-alloc. **Screen allocation edits on the printed
 * `;; N regs to allocate:` ORDER, which is the observable, not on `.12.life`.**
 *
 * ============== THE TWO ONE-PIN BODIES, both 0 of 163, 163/163 ==============
 *
 * 1. `register short *d __asm__("r0")` assigned from a renamed 4th parameter.
 * 2. `register volatile unsigned int *k __asm__("r6"); k = &gKeyRepeat;` with
 *    all eight tests through `*k`. This one is arguably the better pass-3
 *    candidate: r6 is callee-saved and the ROM genuinely keeps the pointer
 *    there across the whole function, so the pin asserts something the bytes
 *    show, where pinning `d` to an argument register does not.
 * Each is ONE pin. Neither ships here.
 *
 * ===================== MEASURED INERT / WORSE (batch 323) =====================
 *   volatile unsigned int *k named, no pin, assigned at the top     17, R/L unmoved
 *   same, initialised in its declaration                            17, R/L unmoved
 *   same, declared before the other locals                          17, R/L unmoved
 *   `extern volatile unsigned int gKeyRepeat[]` with `[0]`          17, R/L unmoved
 *   `register short *d` (no asm register)                           17, R/L unmoved
 *   `if (!*d)` for the four `*d == 0` tests                         17, R/L unmoved
 *   `short t; if ((t = *d) == 0)` for all four tests                17  (R39 and the
 *       printed order BOTH move -- 39 goes to the head of the list -- and the
 *       figure does not, which is the clearest proof the order is not the cause)
 *   the xor through a short local (`t = *d; *d = t ^ 1;`)           31, L38 44 -> 46
 *   `*c = *d` for the two arms where the ROM stores the known zero 125, 167 insns
 *   `unsigned short *d` with `(short)` casts on the tests          134, 157 insns
 *
 * The naming results are the park's own "copy propagation deletes the split"
 * finding again, in a new place: the pseudo count changes and the allocno list
 * renumbers, and p38/p39 do not move.
 *
 * ===================== STILL TRUE FROM THE OLD PARK =====================
 *   - Inert this way too: `*d = *d ^ 1`; `d[0]` throughout; return type `int`;
 *     `*d = *d;` as a free eighth reference (DELETED as a no-op store); five
 *     placements of `short *e = d;` (copy propagation deletes the split before
 *     flow measures it).
 *   - `mov r2,#0 / ldrsh r3,[r0,r2]` is the tell that `d` is `short *`:
 *     thumb-1 has no `ldrsh` with an immediate offset.
 *   - `ldrh r3,[r4] / lsl r3,#16 / cmp r3,r2` with r2 = 0x63<<16 is a SIGNED
 *     halfword compare in the shifted domain -> `c` is `short *`.
 *   - `ldr r2,=1` and `ldr r3,=0x63` are pooled because they are HImode
 *     constants. This is now settled from the compiler source:
 *     arm.md:4318 `*thumb_movhi_insn` constrains operand 1 as
 *     "l,mn,l,*h,*r,I" -- alternative 1's `n` matches any const_int and sits
 *     BEFORE alternative 5's `I`, and recog takes the first match, so the
 *     8-bit `mov` alternative is UNREACHABLE for a HImode const_int. The
 *     internal control is in this very function: `gKeyRepeat & 1` emits
 *     `mov r2, #1` (SImode) four instructions from the `*d ^= 1` that emits
 *     `ldr r2, .L290f8` with `.word 1` (HImode). Same value, one each way.
 *   - The wrap fixups must be `*c = *c - 0x63` / `*c = *c + 0x63`, NOT 0x59:
 *     gcc CSEs the pre-store load and folds the constants outermost
 *     (0xa - 0x63 = -0x59), which is the ROM's `mov r3,r2 / sub r3,#0x59`.
 *     Writing 0x59 directly is wrong arithmetic AND wrong code (19 of 163).
 *   - NO FLAG CLOSES IT, re-measured: -fno-gcse, -fno-rerun-cse-after-loop,
 *     -fno-strict-aliasing, -fno-caller-saves, -fomit-frame-pointer all 17;
 *     -fno-force-mem 19, -fno-schedule-insns2 31, -fno-cse-follow-jumps 90,
 *     -fno-expensive-optimizations 177. NOT a per-file flag row.
 *
 * No pins, no barriers, no .equ, no DMA3_SET in the body below.
 * NO fakematch row needed.
 */
extern volatile unsigned int gKeyRepeat;

extern int Func_8028ef0(int a, int b, short *c);

short Debug_WarpMenu_UI(int a, short b, short *c, short *d)
{
    if (gKeyRepeat & 1)
        return -1;
    if (gKeyRepeat & 2)
        return -2;
    if ((gKeyRepeat & 0x80) || (gKeyRepeat & 0x40)) {
        *d ^= 1;
    } else if (gKeyRepeat & 0x10) {
        if (*d == 0) {
            b = b + 1;
        } else {
            *c = *c + 1;
            if (*c > 0x63)
                *c = 0;
        }
        if (b > 0xc8)
            b = 0;
        Func_8028ef0(a, b, c);
    } else if (gKeyRepeat & 0x20) {
        if (*d == 0) {
            b = b - 1;
        } else {
            *c = *c - 1;
            if (*c < 0)
                *c = 0x63;
        }
        if (b < 0)
            b = 0xc8;
        Func_8028ef0(a, b, c);
    } else if (gKeyRepeat & 0x100) {
        if (*d == 0) {
            *c = 0;
            b = b + 0xa;
        } else {
            *c = *c + 0xa;
            if (*c > 0x63)
                *c = *c - 0x63;
        }
        if (b > 0xc8)
            b = 0;
        Func_8028ef0(a, b, c);
    } else if (gKeyRepeat & 0x200) {
        if (*d == 0) {
            *c = 0;
            b = b - 0xa;
        } else {
            *c = *c - 0xa;
            if (*c < 0)
                *c = *c + 0x63;
        }
        if (b < 0)
            b = 0xc8;
        Func_8028ef0(a, b, c);
    }
    return b;
}
