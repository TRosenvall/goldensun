/* OvlFunc_common2_618 extracted from
 * goldensun/asm/overlays/common/common2_c_c_c_c_c_c_c_c_c_c.s.  The .text of
 * this TU is ONE function and it matches, so the linker lines
 *
 *     asm/overlays/common/common2_c_c_c_c_c_c_c_c_c_c.o(.text)
 *
 * stay verbatim in overlays/rom_7bf5a8/overlay.ld and
 * overlays/rom_7e7574/overlay.ld.  212 bytes, 100 encodings.  Built with
 * COMMON2_CFLAGS via the existing asm/overlays/common/common2_%.o pattern --
 * no new Makefile rule.
 *
 * !! THIS TU ALSO HAS A .data SECTION AND IT CANNOT LAND IN PLACE. !!
 * The .s ends with `.section .data / .global .L1 / .incbin
 * "overlays/rom_7bf5a8/orig.bin", 0x1888, (0x189c-0x1888)` -- twenty zero
 * bytes, which is the quiet NaN the library hands back and is referenced from
 * another object (`.word .L1` in asm/overlays/common/common2_b.s).  Both
 * overlay.ld files also name `common2_c_c_c_c_c_c_c_c_c_c.o(.data)`.  The
 * `asm/%.o: src/%.c` rule builds from the .c and drops the .data, so the blob
 * must move to its own stem with `.L1` kept `.global` and the two `.data`
 * linker lines repointed at it FIRST, gated green alone, before this .c lands.
 * (batch 260's text+data rule.)
 *
 * WHAT IT IS.  IEEE-754 double decomposition, the front half of the overlay's
 * software-float library.  It reads a double as two words -- word 0 is the
 * HIGH word, ARM's legacy mixed-endian layout -- and fills the five-word
 * record (kind, sign, exp, lo, hi) that OvlFunc_common2_0 and
 * OvlFunc_common2_44c consume.  kind is 2 zero, 4 infinity, 1/0 quiet/
 * signalling NaN, 3 finite.  The mantissa is delivered left-aligned by 8,
 * with the implicit bit at 1<<60 for a normal.
 *
 * Parked since batch 152 at "38 of 102, three gcc-is-cleverer residues that
 * pull against each other".  Re-measured after the COMMON2_CFLAGS screen fix:
 * objcmp puts the park at 48 differing of 100.  Route from there, by objcmp
 * differing encodings:
 *
 *     the park as written                                        48
 *     + quiet-NaN test as an if/else, not `!= 0`                 42
 *     + the finite arm gets its OWN 64-bit temporary `t`         40
 *     + the shared store tail moved INTO the 0x7ff arm,
 *       reached from the denormal arm by `goto`                   8
 *     + `out->kind = 3` written BEFORE the finite arm's shift     4
 *     + `p` pinned to r2                                       EXACT
 *
 * Each of those five is load-bearing: removing any one from the matching
 * source gives 4, 4, 44, 36 and 36 differing respectively.
 *
 * FOUR THINGS WORTH KEEPING.
 *
 * 1. THE BLOCK ORDER IS CROSS-JUMPING, NOT SOURCE ORDER, AND `goto` IS THE
 *    LEVER.  The ROM lays the arms out denormal / NaN / shared-tail / finite,
 *    with the finite arm falling into the epilogue.  No if/else-if chain
 *    produces that: gcc emits the arms in source order and the fall-through
 *    tail last, so writing the tail after the chain puts it BELOW the finite
 *    arm and the other two arms jump down to it.  Duplicating the tail in all
 *    three arms instead lets jump2 cross-jump the first two together -- which
 *    is the ROM's shape -- but it also let loop.c's load_mems promote
 *    `out->exp` into a register across the normalising loop, costing the ROM's
 *    per-iteration ldr/sub/str: 21 disagreeing instead of 15.  ONE copy of the
 *    tail, written inside the 0x7ff arm and entered from the denormal arm by a
 *    `goto` into that block, gives the ROM's layout with the ROM's loop and
 *    took 15 to 7.  C permits the jump into the block; it is the only spelling
 *    of the three that reaches both.
 *
 * 2. A SHARED TAIL IS CROSS-JUMPED BY HARD REGISTER, SO A SEPARATE LOCAL IS
 *    WHAT SPLITS IT.  The ROM does NOT merge the finite arm's `str/str` with
 *    the other two: it stores from r3/r4 where they store from r5/r6.  The
 *    park had the finite arm reuse the mantissa union `m` and concluded a
 *    separate variable "does not stop it".  It does -- a block-scoped
 *    `union M t` inside the else arm, which is never live at the same time as
 *    the loop's `m` and so is allocated the scratch pair instead.  It took the
 *    if/else NaN source from 42 differing encodings to 40, and from 27
 *    disagreeing instructions to 15; removing it from the finished source
 *    costs 36.
 *
 * 3. A 64-BIT `& CONST` TEST NARROWS TO A BITFIELD EXTRACT UNLESS THE BRANCH
 *    IS WRITTEN OUT.  The quiet-NaN mask is bit 51, so the constant's LOW word
 *    is zero and combine folds `(m.q & MASK) != 0` to `(hi >> 19) & 1` -- one
 *    lsr/and where the ROM has the full DImode `mov rlo,#0 / and rhi,mask /
 *    orr / cmp / beq / mov #1`.  Spelling it `if (m.q & MASK) k = 1; else
 *    k = 0;` keeps the DImode compare, and cse then deletes the `mov #0` arm
 *    because record_jump_equiv already knows the or-of-halves is zero there --
 *    which is exactly the ROM's reuse of the or result as the boolean 0.
 *    Six instructions, and it is the only one of the three residues the park
 *    had diagnosed correctly.
 *
 * 4. THE OFFSET-0 STORE THROUGH A FRAME POINTER NEEDS A PIN.  Batch 151
 *    recorded that gcc folds the offset-0 store back to `sp` while keeping the
 *    pointer for offset 4, and called it unfixable by more pointer naming.
 *    The mechanism is now read off the dumps: `.00.rtl` has
 *    `(set (reg 35) (addressof:SI (reg/v:DI 40)))`, purge_addressof turns that
 *    into a plain `(set (reg 35) sp)` copy, and cse2's canon_reg then replaces
 *    the BARE base of `(mem (reg 35))` with sp -- while `(plus (reg 35) 4)`,
 *    `+6` and `+7` keep the pseudo.  It costs two instructions, because the
 *    sp-based store is provably frame memory and sched2 then hoists
 *    `ldr r3,[r0]` above it, where the ROM's unknown-base `[r2]` keeps the
 *    dependence and the load stays between the two stores.  Measured inert
 *    against it, all four still 4 differing: a wrapper struct, `*(unsigned
 *    int *)p`, `p = (union Bits *)u.b` / `u.h` / `&u.w`, a plain `register`
 *    keyword, and a `volatile` store.  Storing the high word first is 7.
 *    `register union Bits *p __asm__("r2")` is a user hard register, which
 *    canon_reg will not substitute, and it closes the function.  No barrier,
 *    no `__asm__ volatile` -- this is a pin, not a fakematch.
 */
struct Dec {
    int kind;
    int sign;
    int exp;
    unsigned int lo;
    unsigned int hi;
};

struct Pair {
    unsigned int hi;
    unsigned int lo;
};

union Bits {
    struct { unsigned int lo; unsigned int hi; } w;
    unsigned short h[4];
    unsigned char b[8];
};

union M {
    unsigned long long q;
    struct { unsigned int lo; unsigned int hi; } w;
};

void OvlFunc_common2_618(struct Pair *in, struct Dec *out)
{
    union Bits u;
    register union Bits *p __asm__("r2");
    union M m;
    unsigned int low;
    unsigned int hi;
    unsigned int e;

    p = &u;
    low = in->lo;
    p->w.lo = low;
    hi = in->hi;
    p->w.hi = hi;
    m.w.hi = (hi << 12) >> 12;
    e = (p->h[3] >> 4) & 0x7ff;
    m.w.lo = low;
    out->sign = p->b[7] >> 7;
    if (e == 0) {
        if ((low | m.w.hi) == 0) {
            out->kind = 2;
            return;
        }
        out->exp = -1022;
        m.q = m.q << 8;
        out->kind = 3;
        while (m.w.hi <= 0xfffffff) {
            m.q = m.q << 1;
            out->exp = out->exp - 1;
        }
        goto tail;
    } else if (e == 0x7ff) {
        if ((low | m.w.hi) == 0) {
            out->kind = 4;
            return;
        }
        if (m.q & 0x0008000000000000ULL) {
            out->kind = 1;
        } else {
            out->kind = 0;
        }
tail:
        out->lo = m.w.lo;
        out->hi = m.w.hi;
        return;
    } else {
        union M t;

        out->exp = e - 1023;
        out->kind = 3;
        t.q = (m.q << 8) | 0x1000000000000000ULL;
        out->lo = t.w.lo;
        out->hi = t.w.hi;
        return;
    }
}
