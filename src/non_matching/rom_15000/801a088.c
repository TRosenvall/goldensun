/* DrawInventoryIcon -- 0x0801a088, sole function of
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/801a088.c \
 *     asm/rom_15000/rom_19ebc_a_c_c_c_a_a_c.s --whole
 * asm/rom_15000/rom_19ebc_a_c_c_c_a_a_c.s (grep -ci func_start = 1), so it
 * converts WHOLE-FILE; no data section (datacheck), no split needed.
 *
 * NON-MATCHING: 105 encodings of 252 differ (objcmp).
 *
 * 105 IS A TRUE DISTANCE, and the draft's own header said otherwise -- corrected on
 * re-measurement when parkcheck flagged the claim against the body.  objcmp reports
 * `105 of 252 differ (ours 252)` with NO SIZE line: the total size and the instruction
 * count are both identical.
 *
 * What the draft read as a 4-byte shortfall is a 4-byte INTERNAL shift that cancels.
 * The relocation list is the evidence: the first six calls and every R_ARM_ABS32 pool
 * entry sit at identical offsets, `__modsi3` is 4 bytes late (0x194 against 0x190) and
 * the following LoadIcon 2 bytes early (0x1ba against 0x1bc), and the tail re-syncs.
 * So the residue is localised between those two calls rather than being a length
 * deficit, which makes this park closer than it was written up as.
 * "RELOCATIONS differ" because the sizes differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801a088.c \
 *     asm/rom_15000/rom_19ebc_a_c_c_c_a_a_c.s --whole
 *
 * 107 IS NOT A DISTANCE.  Ours is 252 instructions against the ROM's 253, so
 * the streams misalign at the shortfall and everything past it is counted.
 * The shortfall is exactly 4 BYTES and it is LOCALISED -- objcmp's relocation
 * list proves it:
 *   the first six calls (_GetItemInfo at 0x20, LoadIcon at 0x5c, 0x90, 0xbc,
 *   0xf4, 0x140) are at IDENTICAL offsets in both, and so is every R_ARM_ABS32
 *   pool entry (iwram_3001e94 at 0x200, .L29a10 0x208, .L29ee4 0x20c,
 *   .L29acc 0x214, .L29b68 0x218).  Only __modsi3 (ref 0x190 / ours 0x194),
 *   the LoadIcon after it, and __divsi3 sit 4 bytes apart.
 * So the first 0x140 bytes and the whole literal pool are already in the right
 * place; the entire residue lives in the last two blocks.
 *
 * BLOCKER: which three of the ten long-lived values go to memory, decided in
 * global.c (global alloc, -da .18.greg).  Both sides use r5-r11 plus three
 * stack slots (sub sp,#0xc) for the same ten values:
 *   id, flags, info, slot, k, p, and the three cached pointers p+0x604 /
 *   p+0x600 / p+0x602, plus the halfword-2 carrier.
 *   ROM  memory = {id, info, slot};  registers = flags:r11 k:r10 p:r5
 *                                                ptr604:r9 ptr600:r8 ptr602:r7
 *   ours memory = {info, slot, k};   registers = id:r11 flags:r9 p:r5
 *                                                ptr604:r10 ptr600:r8 ptr602:r7
 * The ROM spills `id` and keeps `k` in r10; we spill `k` and keep `id`.
 * THAT ONE CHOICE IS THE WHOLE 4 BYTES.  Because the ROM's `k` lives in a HIGH
 * register, thumb-1 cannot do `add r10,#1` (no immediate form for Hi regs), so
 * the ROM must spend `mov r3,#1 / add r10,r3` where we spend a single
 * `add r3,#1`.  That happens in BOTH the `flags & 2` and the `flags & 4` block
 * -- 2 instructions x 2 sites = the 4 bytes, and nothing else differs in
 * length anywhere in the function.
 *
 * WHAT WOULD CLOSE IT: anything that makes `k` outrank `id` in
 * allocno_compare.  Declaration order is NOT enough (see INERT) -- the two are
 * not an exact tie, so the pseudo-number tie-break never runs.
 *
 * LOAD-BEARING CONSTRUCTS (each measured by dropping it singly):
 *  - THE BLOCK-LOCAL int CARRIER FOR THE HALFWORD 2 IS THE BIGGEST LEVER HERE:
 *    writing `*(short *)(p + 0x600) = 2;` pools the constant
 *    (`ldr r2,=0x2`, because gcc-2.96 has no immediate alternative for an
 *    HImode constant) where the ROM has `mov r2,#2`.  Wrapping each pair of
 *    halfword stores in its own `{ int t = 2; ... }` block gives the ROM's
 *    `mov`.  Measured: literal 2 -> 208 of 258; ONE shared `int v = 2;` for all
 *    six sites -> 221 of 254 (gcc hoists the single carrier into r8 and reuses
 *    it, where the ROM re-materialises 2 per block); block-local carriers ->
 *    111 of 252.  The scope is the whole lever, not the carrier.
 *  - `unsigned int id`, not `int id`: the ROM shifts with `lsr r3,#0xb` and a
 *    signed operand gives `asr`.  Measured: `int id` -> 111, `unsigned` -> 110.
 *  - The cached-vs-recomputed pointer pattern needs NO source distinction.  The
 *    three stores are written identically in all six blocks; cse.c works on
 *    extended basic blocks, so the `flags & 8` block (single predecessor, in
 *    the unconditional block's extended BB) reuses r9/r8/r7 while the
 *    `flags & 0x10` and `flags & 0x20` blocks (reached through a join) recompute
 *    -- exactly the ROM.  Do not try to spell this difference.
 *
 * CONTRADICTS docs/web-session-286-289.md section 5: that handoff says
 * "A libcall with a constant divisor means the divisor was a variable at
 * expand."  NOT TRUE ON THUMB-1.  Here `k % 10` and `k / 10` written with
 * PLAIN LITERAL 10 emit `mov r1,#0xa / bl __modsi3` and `mov r1,#0xa /
 * bl __divsi3` byte-for-byte the same as routing them through a variable
 * (`int ten = 10;`): both spellings give 111 of 252, identical output.
 * thumb-1 has no high-part multiply, so expand_divmod has no shift/multiply
 * path to take and falls back to the libcall even for a constant divisor.
 * The note should be scoped to targets that HAVE a widening multiply.
 *
 * INERT: all 24 permutations of the four local declarations (107..113, best
 * 107 -- this file's order; it never flips the id/k spill choice); routing the
 * divisor through a variable; naming `k / 10` in a local.
 *
 * The four .L tables live in asm/rom_15000/rom_19ebc_c_c.s and are ALREADY
 * .global there (lines 5-9), so NO new export and no .s change is needed --
 * only gcc's asm-label extension, which is a name binding, not a shim.
 * No pins, no "+r" barriers, no volatile, no DMA3_SET, no .equ.
 * NO fakematch row needed.
 */
extern unsigned char *iwram_3001e94;

extern unsigned char *_GetItemInfo(int id);
extern int LoadIcon(unsigned char *p, int n);

extern int L29a10[] __asm__(".L29a10");
extern int L29ee4[] __asm__(".L29ee4");
extern int L29acc[] __asm__(".L29acc");
extern int L29b68[] __asm__(".L29b68");

int DrawInventoryIcon(unsigned int id, int flags)
{
    unsigned char *p;
    int k;
    int slot;
    unsigned char *info;

    slot = 0;
    k = 0;
    info = _GetItemInfo(id & 0x1ff);
    p = iwram_3001e94;
    if (p == 0)
        return -1;
    if (flags & 1) {
        *(int *)(p + 0x604) = L29a10[2];
        {
            int t = 2;
            *(short *)(p + 0x600) = t;
            *(short *)(p + 0x602) = t;
        }
        LoadIcon(p, 0);
        slot = 1;
    }
    *(int *)(p + 0x604) = L29ee4[*(unsigned short *)(info + 6)];
    {
        int t = 2;
        *(short *)(p + 0x600) = t;
        *(short *)(p + 0x602) = t;
    }
    LoadIcon(p, slot);
    if ((flags & 8) && (id & 0x400)) {
        *(int *)(p + 0x604) = L29acc[1];
        {
            int t = 2;
            *(short *)(p + 0x600) = t;
            *(short *)(p + 0x602) = t;
        }
        LoadIcon(p, 1);
    }
    if ((flags & 0x10) && (id & 0x200)) {
        *(int *)(p + 0x604) = L29acc[0];
        {
            int t = 2;
            *(short *)(p + 0x600) = t;
            *(short *)(p + 0x602) = t;
        }
        LoadIcon(p, 1);
    }
    if ((flags & 0x20) && (id & 0x200) && (info[3] & 1) && (info[3] & 2)) {
        *(int *)(p + 0x604) = L29acc[2];
        {
            int t = 2;
            *(short *)(p + 0x600) = t;
            *(short *)(p + 0x602) = t;
        }
        LoadIcon(p, 1);
    }
    if (flags & 2) {
        k = ((id & 0xf800) >> 11) + 1;
        if (k <= 1)
            k = 0;
    }
    if (flags & 4) {
        k = ((id & 0xf800) >> 11) + 1;
    }
    if (k != 0 && k <= 0x1e) {

        *(int *)(p + 0x604) = L29b68[k % 10];
        {
            int t = 2;
            *(short *)(p + 0x600) = t;
            *(short *)(p + 0x602) = t;
        }
        LoadIcon(p, 1);

        if (k / 10 != 0) {
            *(int *)(p + 0x604) = L29b68[k / 10 + 9];
            {
                int t = 2;
                *(short *)(p + 0x600) = t;
                *(short *)(p + 0x602) = t;
            }
            LoadIcon(p, 1);
        }
    }
    return 0x100;
}
