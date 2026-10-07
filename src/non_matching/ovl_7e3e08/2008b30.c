/* OvlFunc_957_2008b30 -- PARKED AT 0 of 62 encodings.  PIN-FREE, SHIM-FREE, NO asm.
 *
 * THE FUNCTION ITSELF IS BYTE-EXACT.  objcmp on this body reports
 * "152 bytes, 62 encodings and 5 relocations identical", and reports
 * "ok OvlFunc_957_2008b30" inside the whole translation unit too.
 *
 * IT IS PARKED BECAUSE ITS PIECE CANNOT CONVERT.  The reference
 * asm/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_a_a.s holds TWO functions,
 * and the other one -- OvlFunc_957_2008bc8,
 * src/non_matching/ovl_7e3e08/2008bc8.c -- is parked with a RELOCATION SYMBOL
 * difference (_AREA_92 and _AREA_97 appear in ours and not in the ROM), which
 * `make compare` cannot pass.  A piece converts whole or not at all.  THIS
 * FUNCTION'S BLOCKER IS NOW ENTIRELY IN ITS PIECE-MATE: solve 2008bc8 and the
 * piece lands.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7e3e08/2008b30.c \
 *     asm/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_a_a.s --func OvlFunc_957_2008b30
 *
 * Sets the fade parameters for one room, differently depending on a flag.
 *
 * Earlier bodies measured forty-three, and before that fifty-six instructions
 * against the ROM's fifty-four with seventeen then eleven differing.  Batch 334
 * took it from forty-three to zero.
 *
 * ===== THE LENGTH GAP WAS A SECOND LITERAL POOL AND THE BRANCH OVER IT =====
 *
 * DIRECTION: forty-eight instructions for the reference against FORTY-NINE for
 * us -- WE EMITTED AN EXTRA ONE.  gcc's own assembly said what it was:
 *
 *     .L3:  ... strh r2, [r3]
 *           b    .L8          <-- THE EXTRA INSTRUCTION
 *     .L9:  .align 2, 0
 *     .L7:  .word 16191 / .word 16194 / .word 3076 / ...
 *     .L8:  .L4:  pop {r5} / pop {r0} / bx r0
 *
 * We dumped the constant pool TWICE -- once free after the then-arm's `b`, and
 * once before the epilogue, where gcc had to emit a branch to jump over it.
 * The ROM has ONE pool, after `bx r0`.
 *
 * MECHANISM, AND IT IS THE HImode RULE DOING SOMETHING THIS TREE HAD NOT
 * RECORDED.  All FIVE of this function's stored constants were HImode, not just
 * the 5 the previous header named:
 *
 *     ldrh r3, .L5     5          ldrh r3, .L5+4   0x1d1d     ldrh r3, .L7   0x3f3f
 *     ldrh r2, .L7+4   0x3f42     ldrh r2, .L7+8   0xc04
 *
 * `*thumb_movhi_insn` (arm.md:4318-4320) takes alternative 1, `mn`, for any
 * `const_int`, and that alternative's `pool_range` is SIXTY-FOUR (arm.md:4353).
 * `*thumb_movsi_insn`'s pool alternative has ONE THOUSAND AND TWENTY
 * (arm.md:3856).  Sixty-four bytes is shorter than this function, so ONE HImode
 * entry is enough to force the second dump and its branch.  Every entry has to
 * be SImode for a single end-of-function pool, which means the original
 * materialised ALL FIVE constants through `int`s.
 *
 * THE MODE IS INVISIBLE IN THE INSTRUCTION TEXT.  gas assembles gcc's
 * `ldrh rX, .L5` as a sixteen-bit `ldr rX, [pc, #imm]`, because Thumb-1 has no
 * PC-relative `ldrh`.  So the two modes produce IDENTICAL ENCODINGS and differ
 * only in WHERE THE POOL LANDS.  That is why the previous header, reading the
 * instruction stream, saw only the one site where the value was small enough for
 * the mode to change the mnemonic.
 *
 * > GENERALISABLE LEVER, new: if the reference has ONE literal pool and you have
 * > TWO -- or you are one instruction long and the extra one is a branch over a
 * > pool -- COUNT YOUR HImode POOL ENTRIES.  Every halfword store of a literal
 * > makes one at `pool_range` sixty-four, and one is enough.  The fix is an `int`
 * > carrier per constant.  arm.md:4318-4320, :4353, :3856.
 *
 * ===== THE PREVIOUS HEADER'S "NO-BOUNDARY WALL" DOES NOT EXIST =====
 *
 * It said the HImode rule "needs an int-typed value rematerialised at the store,
 * which needs a dominating block that is not the store's own", and that
 * "declared beside the store it is in the store's own block and the pool load
 * comes back".  THAT DOES NOT REPRODUCE.  `{ int c0 = 5; *(short *)(q+0x52a) =
 * c0; }` -- the store's own block -- gives `mov r2, #5`.
 *
 * It cannot come back, because combine cannot re-form the HImode store of a
 * constant: `(set (mem:HI) (const_int))` has no `*thumb_movhi_insn` alternative
 * with `m` on operand 0 and `n` on operand 1, AND the insn's own condition
 * requires one of the two operands to be a `register_operand`, so recog rejects
 * it outright.  THERE IS NO DOMINATING-BLOCK REQUIREMENT FOR THE HImode RULE AT
 * ALL.  src/non_matching/ovl_780898/2008e54.c and
 * src/non_matching/rom_7d30e0/2009838.c, which this park was cited alongside,
 * should be re-read on that basis.
 *
 * ===== THE RESIDUE WAS A COUPLING BETWEEN THREE SITES =====
 *
 * With the carriers in, three sites read
 *     ldr rA,=OFF / ldr rB,=VAL / add rC,r5,rA / strh rB,[rC]
 * against the ROM's
 *     ldr r3,=OFF / add r2,r5,r3 / ldr r3,=VAL / strh r3,[r2]
 * -- the ROM materialises the VALUE AFTER the address and reuses the offset's
 * dead register for it.  `expand_assignment` evaluates the LHS address before
 * the RHS, so a value REMATERIALISED AT THE USE (a carrier declared above the
 * `if`, which cse sinks) lands after the address, while a value ASSIGNED IN ITS
 * OWN STATEMENT lands before it.  THE SITES WANTED OPPOSITE ANSWERS:
 *
 *   the 0x534 stores            needed the rematerialised form (carrier above the if)
 *   the BLDCNT/BLDALPHA stores  needed the assigned-before form
 *   the 0x52a store             needed a NAMED ADDRESS plus an assigned-before value
 *
 * and the last instruction only fell when the assigned-before carrier for
 * BLDCNT/BLDALPHA was a SEPARATE variable from the one the 0x52a site uses.
 * Sharing one `int` across both re-couples them and costs four.
 *
 * MEASURED, and this is why one-at-a-time testing could not find it.  FOURTEEN
 * variants permuting declaration order and one-variable-two-roles were ALL inert
 * at twelve.  Crossing the three sites' spellings independently -- eighteen then
 * six variants -- went twelve, then four, then zero.  Figures for the crossed
 * grid, offsets in the order 0x52a / 0x534 / BLDCNT:
 *
 *   named address + own carriers : above-the-if : own separate carrier    ZERO
 *   named address + shared t     : above-the-if : above-the-if            four
 *   named address + shared t     : above-the-if : shared t                four
 *   carrier above the if         : above-the-if : above-the-if            eleven
 *   assigned-before t            : assigned-before : assigned-before      thirteen
 *   named address everywhere (0x536 sites too)                            twenty-three, and the SIZE changes
 *
 * KEPT FROM THE PREVIOUS BODY, all three still load-bearing: the walked register
 * base (`add r3, #2` rather than two independent pool addresses); the second
 * global reached from the first as `((char **)&iwram_3001ebc)[4]`; and the 0x3f
 * and 0x1f carriers declared above the `if`.
 *
 * NEXT: nothing here.  Work src/non_matching/ovl_7e3e08/2008bc8.c.
 */
#include "gba/types.h"
#include "gba/io.h"

extern char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __WaitFrames(int n);
extern void __Func_808fe38(int n);
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
