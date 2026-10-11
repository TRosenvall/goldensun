/* Cluster OvlFunc_957_2008b30..OvlFunc_957_2008bc8 extracted from
 * goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_a.s.
 *
 * Slotted between the _a_* pieces in goldensun/overlays/rom_7e3e08/overlay.ld;
 * this is the WHOLE reference file, both functions, so no split is needed.
 *
 * LANDED, batch 336, 0 of 41 encodings for OvlFunc_957_2008bc8 and 0 of 62 for
 * OvlFunc_957_2008b30; `objcmp --whole` is clean.  PIN-FREE, SHIM-FREE, NO asm,
 * NO per-file flags.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_a_a.c \
 *     asm/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_a_a.s --whole
 *
 * Against the reference `.s` objcmp reports TWO differing stream entries and a
 * relocation count of 8 against 6.  BOTH are the same phantom and neither is a
 * defect: the reference spells the area ids as POOL LITERALS (`ldr r3, =0x92`)
 * and we name the absolute symbols, so our two pool words are relocation
 * placeholders that the linker fills with 0x92 and 0x97.  All 32 instructions
 * and every other stream entry are identical.  `area.sym` defines both as
 * absolute, and THREE LANDED FILES IN THIS SAME BANK already carry
 * `.word _AREA_97` against a reference that did not --
 * src/overlays/rom_7e3e08/ovl_30_c_c_a_a_a_b.c, ovl_30_c_c_c_b.c and
 * ovl_30_c_c_a_c_c_c_c_c_c_a_b.c.  tools/symrefs.py lists them.
 *
 * The symbols are also FORCED here, which is the `owner-decisions.md`
 * structural test: 0x92 and 0x97 are both 8-bit values and Thumb `cmp` takes
 * an 8-bit immediate, so a literal would have been `cmp r2, #0x92`.  The ROM
 * pools them instead, which only happens for a `symbol_ref`.
 */

/* ===================================================================== */
/* OvlFunc_957_2008b30 -- carried forward from its park, which reached 0  */
/* in batch 334 and could not ship because its piece-mate blocked the TU. */
/* ALL THREE OF THESE ARE STILL LOAD-BEARING.                             */
/*
 * Sets the fade parameters for one room, differently depending on a flag.
 *
 * THE LENGTH GAP WAS A SECOND LITERAL POOL AND THE BRANCH OVER IT.  All five
 * stored constants are HImode, and `*thumb_movhi_insn` (arm.md:4318-4320) takes
 * alternative 1, `mn`, for any `const_int`; that alternative's `pool_range` is
 * SIXTY-FOUR (arm.md:4353) against `*thumb_movsi_insn`'s ONE THOUSAND AND
 * TWENTY (arm.md:3856).  Sixty-four bytes is shorter than this function, so ONE
 * HImode entry forces a second pool dump plus a branch over it.  Every entry
 * has to be SImode for a single end-of-function pool, which is why all five
 * constants are materialised through `int` carriers.  THE MODE IS INVISIBLE IN
 * THE INSTRUCTION TEXT -- gas assembles `ldrh rX, .L5` as a sixteen-bit
 * `ldr rX, [pc, #imm]`, so the two modes produce IDENTICAL ENCODINGS and differ
 * only in WHERE THE POOL LANDS.
 *
 * There is NO "no-boundary wall" for the HImode rule, contrary to an earlier
 * header: combine cannot re-form the HImode store of a constant, because
 * `(set (mem:HI) (const_int))` has no `*thumb_movhi_insn` alternative pairing
 * `m` on operand 0 with `n` on operand 1, and the insn's own condition demands
 * a `register_operand`.  A carrier declared in the store's own block is fine.
 *
 * THE RESIDUE WAS A COUPLING BETWEEN THREE SITES.  The ROM materialises the
 * VALUE AFTER the address and reuses the offset's dead register for it.
 * `expand_assignment` evaluates the LHS address before the RHS, so a value
 * REMATERIALISED AT THE USE (a carrier declared above the `if`, which cse
 * sinks) lands after the address, while a value ASSIGNED IN ITS OWN STATEMENT
 * lands before it -- and the three sites want opposite answers:
 *
 *   the 0x534 stores            the rematerialised form (carrier above the if)
 *   the BLDCNT/BLDALPHA stores  the assigned-before form
 *   the 0x52a store             a NAMED ADDRESS plus an assigned-before value
 *
 * and the last instruction only fell when the assigned-before carrier for
 * BLDCNT/BLDALPHA was a SEPARATE variable from the one the 0x52a site uses.
 * Sharing one `int` across both re-couples them and costs four.  FOURTEEN
 * variants permuting declaration order were ALL inert at twelve; crossing the
 * three sites' spellings independently went twelve, then four, then zero.
 *
 * Also load-bearing: the walked register base (`add r3, #2` rather than two
 * independent pool addresses), and the second global reached from the first as
 * `((char **)&iwram_3001ebc)[4]`.
 */

/* ===================================================================== */
/* OvlFunc_957_2008bc8 -- the gate, closed in batch 336.                  */
/*
 * Guards five palette calls on the AREA ID at gState+0x1C0, and sets BLDALPHA
 * for one other area.
 *
 * The park read 36 of 41 with COUNT 32/29 -- THREE MISSING INSTRUCTIONS, not a
 * distance.  The ROM reads the halfword TWICE:
 *
 *      ldrh  r1, [r3]        <- unsigned, sign-extended later by lsl/asr
 *      mov   r0, #0
 *      ldrsh r2, [r3, r0]    <- signed, compared immediately
 *
 * and gcc kept only the `ldrsh`, dropping the `ldrh` and the `lsl #16 / asr #16`
 * that sign-extends it.  The park called this "gcc CSEs two loads of the same
 * address" and bounded it: "No source spelling separates them".  THAT VERDICT
 * IS REFUTED.  Its observations were right; the diagnosis named the wrong pass
 * and the wrong precondition.
 *
 * ===== THE PASS IS cse1, THE SITE IS cse.c:5048-5092, AND IT IS NOT A
 * ===== COMMONING OF TWO LOADS AT ALL
 *
 * Under `-da` the park's body loses nothing in cse at all -- it arrives at
 * `expand` already collapsed, because `arm.h:597-606`'s PROMOTE_MODE widens an
 * HImode local to SImode with `UNSIGNEDP = TARGET_MMU_TRAPS != 0`, which is
 * ZERO here.  So **an `unsigned short` local is held SIGN-extended**, `(short)h`
 * on it is a no-op, and the park's two reads were one value before any
 * optimiser ran.  An `unsigned int` carrier instead gives `expand` exactly the
 * ROM's shape -- `(zero_extend:SI (mem:HI))` for the unsigned read and an
 * independent signed read -- and then **cse1 (pass 03) collapses it**:
 *
 *   cse.c:5048-5092, "See if a MEM has already been loaded with a widening
 *   operation; if it has, we can use a subreg of that."
 *
 * `extendhisi2` (arm.md:3187-3236) ALWAYS forces a Thumb operand through
 * `copy_to_mode_reg (HImode, ...)`, so a signed halfword read is expanded as a
 * BARE `(set (reg:HI) (mem:HI))` plus two shifts -- and a bare MEM src is
 * precisely what cse.c:5057's `GET_CODE (src) == MEM` keys on.  cse then looks
 * up `(zero_extend:SI (mem:HI addr))`, finds the unsigned read's register
 * (`LOAD_EXTEND_OP` is unconditionally ZERO_EXTEND for Thumb, arm.h:2330-2333),
 * and replaces the signed load with a subreg of it.  Combine then folds the
 * remaining pair into one `ldrsh` and the `ldrh` dies.
 *
 * PROOF THE BOUND IS THAT ONE BLOCK AND NOTHING ELSE: the `unsigned int`
 * carrier body is BYTE-IDENTICAL under `-fno-expensive-optimizations`, which is
 * the only gate on cse.c:5055.  Recorded as a figure about the blocker; the
 * shipped body needs no flag.
 *
 * ===== WHAT ACTUALLY CLOSED IT: NO CARRIER VARIABLE AT ALL
 *
 * cse.c:5048 can only fire if the table already holds the wider load when the
 * narrow one is hashed.  Deleting the carrier puts the unsigned read in the
 * SECOND `if`, in a different basic block, where two things happen:
 *
 *   - it expands as a plain `(set (reg:HI) (mem:HI))` + shifts with NO
 *     `zero_extend` anywhere, so the cse.c:5048 lookup has nothing to find; and
 *   - the BLDALPHA store in the first `if` invalidates the MEM on the path
 *     between them, so the ordinary lookup misses too.
 *
 * and then **gcse (pass 07) hoists the block-2 halfword load up into block 0**,
 * which is what puts `ldrh` AHEAD of `ldrsh` -- the ROM's order.  `-fno-gcse`
 * on this very body loses the hoist and reads 31 instructions against 32.
 *
 * > GENERALISABLE, new: when gcc merges a SIGNED and an UNSIGNED read of one
 * > halfword, the pass is cse1 at cse.c:5048-5092 and the precondition is that
 * > the WIDER (zero_extend) load is already in the hash table.  Do not try to
 * > separate the two reads -- REMOVE THE CARRIER so the unsigned read is a bare
 * > HImode move in another block, and let gcse hoist it back.  And check
 * > PROMOTE_MODE first: with `TARGET_MMU_TRAPS` zero an `unsigned short` local
 * > is SIGN-extended, so `(short)h` on it is a no-op and the two reads were
 * > never distinct.
 *
 * MEASURED, crossed rather than one-at-a-time.  Source order and carrier mode
 * were crossed against the read spelling; figures are stream lengths, ref 41:
 *
 *   no carrier, unsigned re-read inside the second if                 ZERO
 *   unsigned-int carrier, SIGNED read first then unsigned             six (ref 41, ours 41 -- only the two loads' ORDER and k's register)
 *   unsigned-int carrier, unsigned read first (ROM source order)      36, count 32/29
 *   unsigned-int carrier + a copy of it for the second compare        six
 *   `int` carrier                                                     36
 *   `unsigned short` carrier (the park)                               36
 *   second compare as `(int)(h << 16) >> 16`                          36
 *   second compare as `(int)(short)h`                                 36
 *   own pointer variable for either read, or for both                 36
 *   absolute-constant address for either read                         36
 *   `const short *` for the signed read                               36
 *   a union of the two views (the park already recorded this)         36
 *   signed read spelled `((short *)g)[0]` / `(g + (unsigned int)0)`   36
 *   a `short` temp for the first compare instead of an `int`          36
 *
 * TWO DEVICES ALSO REACH ZERO and are recorded as figures about the blocker,
 * not shipped: `__asm__ volatile ("" : : "r" (g))` between the two reads, and
 * `__asm__ volatile ("" : : : "memory")`.  Both work for the stated reason --
 * cse.c:5742-5745 calls `flush_hash_table()` when `GET_CODE (PATTERN (insn))`
 * is `ASM_OPERANDS` with `MEM_VOLATILE_P`.  A BARE `__asm__ volatile ("")` does
 * NOT work and reads 36: with no operands the pattern is an `ASM_INPUT`, not an
 * `ASM_OPERANDS`, so the flush never runs.  **That is a real limit on the
 * batch-335 empty-asm barrier: it is a scheduling barrier, not a cse flush.**
 */
#include "gba/types.h"
#include "gba/io.h"

typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern char *iwram_3001ebc;
extern int _AREA_92;
extern int _AREA_97;
extern int __GetFlag(int id);
extern void __WaitFrames(int n);
extern void __Func_808fe38(int n);
extern void __Func_8092950(int a, int b);
extern void OvlFunc_957_2008a54(void);

void OvlFunc_957_2008b30(void)
{
    char *p;
    char *q;
    vu16 *reg;
    short *s;
    int t;
    int u;
    int a3f;
    int a1f;
    int c1;
    int c2;

    a3f = 0x3f;
    a1f = 0x1f;
    c1 = 0x1d1d;
    c2 = 0x3f3f;
    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x100;
    *(int *)(p + 0x1c8) = 0x18;
    __WaitFrames(1);
    __Func_808fe38(0x4d);
    q = ((char **)&iwram_3001ebc)[4];
    s = (short *)(q + 0x52a);
    t = 5;
    *s = t;
    if (__GetFlag(0x201)) {
        *(short *)(q + 0x534) = c1;
        *(short *)(q + 0x536) = a3f;
        OvlFunc_957_2008a54();
    } else {
        *(short *)(q + 0x534) = c2;
        *(short *)(q + 0x536) = a1f;
        u = 0x3f42;
        reg = &REG_BLDCNT;
        *reg = u;
        u = 0xc04;
        reg = (vu16 *)((char *)reg + 2);
        *reg = u;
    }
}

void OvlFunc_957_2008bc8(void)
{
    unsigned char *g;
    unsigned int k;
    int t;
    int v;

    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    v = *(short *)g;
    if (v == (int)(&_AREA_92)) {
        t = 0x80 << 5;
        *(unsigned short *)REG_ADDR_BLDALPHA = t;
    }
    if ((short)*(unsigned short *)g == (int)(&_AREA_97)) {
        __Func_8092950(0x10, 1);
        __Func_8092950(0x11, 4);
        __Func_8092950(0x12, 0xb);
        __Func_8092950(0x13, 2);
        __Func_8092950(0x14, 3);
    }
}
