/* asm/rom_77000/rom_78414_c_c_a_a_c_a_b.s -- Func_8078550 (0x08078550).
 *
 * EXACT.  objcmp --func Func_8078550: 56 bytes, 26 encodings and 2 relocations
 * identical.  PINS: 0.  SHIMS: 0.  No device, no flag group -- production flags.
 * tools/datacheck.py prints nothing for the parent .s (text only, no data
 * section), so no export is needed, but the parent holds a SECOND function
 * (GiveItemTo, 0x08078588, still parked) so the landing DOES need a split.
 *
 * Split shape (tools/split_s.py --dry-run on asm/rom_77000/rom_78414_c_c_a_a_c_a.s
 * with Func_8078550):
 *     writes asm/rom_77000/rom_78414_c_c_a_a_c_a_b.s   (1 function, 32 lines)
 *     writes asm/rom_77000/rom_78414_c_c_a_a_c_a_c.s   (1 function, 80 lines)
 *     removes asm/rom_77000/rom_78414_c_c_a_a_c_a.s, rewrites stage1.ld
 * The target is the FIRST function in the piece, so no _a part is written; this
 * .c installs as src/rom_77000/rom_78414_c_c_a_a_c_a_b.c.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_77000/rom_78414_c_c_a_a_c_a_b.c asm/rom_77000/rom_78414_c_c_a_a_c_a_b.s --func Func_8078550
 *
 * ===========================================================================
 * THE PARK'S OBSERVATIONS WERE RIGHT AND ITS VERDICT POINTED AT THE WRONG HALF
 *
 * The park sat at twenty differing of twenty-six for four batches, correctly
 * noting that it measured MISALIGNMENT: we were TWO INSTRUCTIONS SHORT (ref 23,
 * ours 21) and the relocation delta was nothing but the four-byte shortfall.
 * It also named both missing instructions correctly -- `mov r6, r5`, a copy of
 * the buffer pointer so the walker is a second register, and `mov r7, r0`, the
 * running total computed with `sub r0, r7, r0` into a fresh register and then
 * assigned.  Its verdict was that the lever was on the WALKING POINTER and
 * needed an initialiser that is "an EXPRESSION, not an alias".
 *
 * Both copies are bought on the ACCUMULATOR, and the pointer needed nothing but
 * having its assignment written ONCE.  Decomposed, with figures:
 *
 *   1. THE WALKER COPY.  The park wrote `p = buf; n = Func_80796c4(p); acc = 0;
 *      p = buf;` -- passing the walker to the call and then re-assigning it.
 *      Pass the ARRAY and assign the walker once and `mov r6, r5` appears:
 *      instruction count 21 -> 22, figure twenty -> eighteen.  The park's own
 *      attempt 2 was this idea with one assignment too many.
 *   2. THE ACCUMULATOR COPY.  `acc = acc + (0xf - r)` -- the constant INSIDE the
 *      parenthesised operand -- reassociates to `sub r0, r7, r0 / mov r7, r0 /
 *      ... / add r7, #0xf`, THREE instructions and the ROM's registers exactly,
 *      where the park's `acc = acc - r; ... acc = acc + 0xf` fuses to two.
 *      22 -> 23 instructions, equal length, figure eighteen -> TWO.
 *      `acc = acc - (r - 0xf)` does the same.
 *   3. THE LAST TWO were the ORDER of `add r6, #2` (the walker step) and
 *      `add r7, #0xf`.  The increment ahead of the accumulator statement closes
 *      it -- which `*p++` inside the call argument gives for free.
 *
 * WHY STEP 2 IS NOT A SPELLING TRICK.  `fold-const.c`'s reassociation turns
 * `acc + (15 - r)` into `(acc - r) + 15` as a TREE, so expand sees two separate
 * statements and emits a three-address `minus` into a fresh pseudo plus an add;
 * written as two statements over one variable, `acc = acc - r` is already a
 * two-address update of acc's pseudo and the `+ 15` folds into it.  The
 * difference is not the arithmetic, it is WHICH PSEUDO HOLDS THE INTERMEDIATE --
 * and a fresh pseudo is what gcc then copies back.  Same class as step 1: in
 * both, the ROM is ONE INSTRUCTION WORSE than gcc's default because the original
 * source gave a value its own name.
 *
 * THE SPELLING IS CORROBORATED TWICE IN THE TREE, which is why it ships as one
 * line rather than as the three-statement form that also matches:
 *   - src/overlays/rom_78ef88/ovl_314_c_c_c_c_b.c:54 (landed) writes
 *     `n = 0x1e - __FindEmptyInventorySlot(0); n -= __FindEmptyInventorySlot(1);`
 *     -- a constant minus this very callee, accumulated;
 *   - src/rom_77000/rom_78414_c_c_a_a_b.c (landed) is Func_8078500, the
 *     immediately preceding function, and it is the same loop over the same
 *     buffer: `short buf[10]; short *p; n = Func_80796c4(buf); p = buf;
 *     for (i = 0; i < n; i++) ... *p++ ...`.  Its `extern int
 *     Func_80796c4(short *buf);` is adopted here verbatim.
 * So `0xf` is the free-slot ceiling and the loop sums free slots per party
 * member; the .s annotation's name for this function, CountPartyInventory-
 * Filtered, is consistent with that.
 *
 * ALSO MEASURED.  Four spellings reach EXACT: `*p++` inline in the call argument
 * (shipped); `h = *p++;` on its own line; the park's register-offset load
 * (`o = 0; h = *(short *)((unsigned char *)p + o);`) with `p++` next; and
 * `o = 0; h = p[o]; p++;`.  So the `mov r3, #0 / ldrsh r0, [r6, r3]`
 * register-offset load falls out of plain `*p++` here and does NOT need the
 * named-offset device the GiveItemTo park reaches for.
 * Equal length and TWO differing, the add/add order only: the accumulator
 * statement BEFORE the increment, in five arrangements including both for-step
 * comma forms.
 * Twenty-two of twenty-three, the walker copy only: plain `acc = acc - r; p++;
 * acc = acc + 0xf`; the same with a named `base` between `buf` and `p`; the same
 * with `&buf[i]` indexing and no walker; `t = acc - r; acc = t; ...`;
 * `t = acc - r; p++; acc = t; acc = acc + 0xf`; `acc -= r; p++; acc += 0xf`.
 * Twenty-one of twenty-three, neither copy: the park's body unchanged, and the
 * park's body with the counter decrement moved last.
 */
extern int FindEmptyInventorySlot(int id);
extern int Func_80796c4(short *buf);

int Func_8078550(void)
{
    short buf[10];
    short *p;
    int n;
    int i;
    int acc;

    n = Func_80796c4(buf);
    acc = 0;
    p = buf;
    for (i = 0; i < n; i++)
        acc += 0xf - FindEmptyInventorySlot(*p++);
    return acc;
}
