/* Func_80a46b4  --  0x080a46b4   [rom_a1000]
 *
 * Source asm: goldensun/asm/rom_a1000/rom_a1814_c_c_c_a_c.s -- ONE function,
 * so this lands as a WHOLE-FILE .c at src/rom_a1000/rom_a1814_c_c_c_a_c.c.
 * No split, no linker edit: the object path asm/rom_a1000/rom_a1814_c_c_c_a_c.o
 * does not change.
 *
 * stage1.ld names that object on TWO lines and both stay exactly as they are:
 *   - in the rom_a1000 .text list, between
 *     `asm/rom_a1000/rom_a1814_c_c_c_a_b.o(.text)` and
 *     `asm/rom_a1000/rom_a1814_c_c_c_b.o(.text)`;
 *   - in the matching .rodata list, between
 *     `asm/rom_a1000/rom_a1814_c_c_c_a_b.o(.rodata)` and
 *     `asm/rom_a1000/rom_a1814_c_c_c_b.o(.rodata)`.
 * The .rodata line names a section this object does NOT have -- the .s carries
 * no .section/.data/.rodata/.word/.incbin, and the compiled .c puts its two
 * pool words (0x1ff, 0x3fff) in .text.  An unmatched .ld entry is not an error
 * and, because nothing is being split, nothing needs re-aiming.
 *
 * NO FLAG GROUP.  No explicit and no wildcard Makefile rule matches
 * src/rom_a1000/rom_a1814_c_c_c_a_c.c, so the generic `asm/%.o: src/%.c` rule
 * with plain GCC296_CFLAGS builds it -- which is what objcmp used.
 *
 * VERDICT
 *   OK Func_80a46b4 -- 160 bytes, 74 encodings and 4 relocations identical
 * (objcmp against asm/rom_a1000/rom_a1814_c_c_c_a_c.s, run twice.)
 *
 * WHAT IT DOES.  Classifies an item for a character: -1 "cannot be aimed at
 * this character", else 2/1 from the move record when bit 0x40 is set and 0/-1
 * when it is not.  Item 0-gates through _Func_808e990 (non-zero there returns
 * 0, a DIFFERENT value from the -1 default), the +0x28 move id must be
 * non-zero, and the equip gate is: +0x02 zero passes outright, otherwise +0x0c
 * must not be 3 AND _CanEquipItem must approve.
 *
 * ---------------------------------------------------------------------------
 * TWO LEVERS.  The template supplied neither.
 *
 * Template: src/rom_15000/rom_23178_a_a_a_a_a_b.c (Func_8025180).  It is a real
 * family member -- same struct Item, same three accessors, same short-circuit
 * equip test -- and the struct declaration and call shapes transplant free.
 * Its own headline is ACTIVELY MISLEADING here: it tests `info->f28 == 0`
 * BEFORE calling _GetMoveInfo and reaches the move through
 * `_GetMoveInfo(info->f28)->f1` in one expression.  This ROM calls
 * _GetMoveInfo FIRST, with `info->f28 & 0x3fff`, and then RE-LOADS info->f28
 * for the zero test (`ldrh r3, [r5, #0x28]` on both sides of the `bl`).  The
 * reload is not CSE failing; it is the call clobbering memory, and it is what
 * the statement order has to say.
 *
 * LEVER 1 -- `1 + (x == 0xff)`, not `2 - (x != 0xff)`.
 *
 * The ROM wants gcc to expand the comparison as a VALUE:
 *
 *   rom  mov r3,#0xff / eor r2,r3 / neg r3,r2 / orr r3,r2 / lsr r7,r3,#0x1f
 *        / mov r3,#0x2 ... sub r7,r3,r7
 *
 * Written `2 - ((f8 ^ 0xff) != 0)` gcc knows f8 is a byte, folds the xor to
 * `mvn` and truncates with `lsl #24`, then BRANCHES -- because at that point it
 * also knows `ret == 1` from the dominating `cmp r7,#1`, so falling through
 * already leaves the answer in place.  Written `2 - (f8 != 0xff)` it uses a
 * plain `cmp r3,#0xff` and branches for the same reason.  Written
 * `1 + (f8 == 0xff)` gcc reassociates to `2 - ((f8 ^ 0xff) != 0)` ITSELF, in
 * SImode, and emits the branchless sequence above.
 *
 * NOT NEW, and it was found by grepping the corpus rather than the docs: the
 * `neg/orr/lsr #31` idiom appears in 23 generated TUs, and
 * src/rom_b5000/rom_b7410_a_a_c_c_a_b.c (Func_80b7aac) already records it as
 * "LEVER 1 -- `4 + (x == 1)`, not `5 - (x != 1)`".  This is the same lever with
 * a byte constant instead of 1.  The sibling arm here, `0 - ((f1 & 0x80) != 0)`,
 * needs NO such treatment and matched from the plain spelling -- the asymmetry
 * is the tell that the branch in the other arm was opportunistic, not
 * structural.
 *
 * LEVER 2 -- `ret = -1;` is written BEFORE `item &= 0x1ff;`, and that is the
 * whole difference between 74 encodings and a match.
 *
 *   rom   ldr r3, =0x1ff / mov r6, r1 / and r6, r3      the VALUE is rd
 *   ours  ldr r6, =0x1ff / and r6, r1                   the CONSTANT is rd
 *
 * Twelve spellings of the mask are EXACTLY INERT -- byte for byte identical,
 * first diff at the same index: `item &= 0x1ff`, `item = item & 0x1ff`,
 * `id = item & 0x1ff`, `id = 0x1ff & item`, `id = item; id &= 0x1ff`,
 * `int m = 0x1ff` with the local on either side, `unsigned int m`,
 * `id = 0x1ff; id &= item`, `item % 0x200` on an unsigned parameter, and
 * `unsigned int` / `unsigned short` / `long` parameter types.  That is exactly
 * what docs/elevation.md predicts under "The source-order lever does not reach
 * commutative operands", whose advice is to confirm and park.
 *
 * THAT ADVICE IS TOO STRONG, and this is the NEW finding.  The lever that
 * reaches it is not a spelling of the expression at all; it is an UNRELATED
 * STATEMENT PLACED BEFORE IT.  Mechanism, read from the RTL dumps and then
 * from the compiler source:
 *
 *   - With the mask first, `.13.combine` already has
 *       (set (reg/v:SI 33) (and:SI (reg:SI 1 r1) (reg:SI 37)))
 *     -- combine has forward-propagated the incoming parameter copy, so
 *     operand 1 is the HARD REG r1.  Operand 1 is tied to the destination and
 *     a hard reg cannot take the tie, so the tie falls to the constant and the
 *     pool load becomes the destination.  (regmove then rewrites the operand
 *     order to match; `-fno-regmove` produces the ROM shape here but breaks the
 *     0x3fff mask and rotates r6/r7 -- 19 differing -- so it is not the answer.)
 *   - With `ret = -1;` first, `.13.combine` has
 *       (set (reg/v:SI 33) (and:SI (reg/v:SI 33) (reg:SI 37)))
 *     -- the parameter copy survives, the tie lands on the value, and the ROM
 *     three-instruction form comes out.  regmove leaves it alone.
 *
 * WHY the statement changes it: combine.c/can_combine_p refuses to substitute a
 * hard register into I3 under SMALL_REGISTER_CLASSES unless `all_adjacent`
 * ("Do not extend the life of a hard register unless it is user variable ...").
 * `ret = -1;` is a `mov`+`neg` pair, so writing it first puts two RTL insns
 * between the parameter copy and the `and`, `all_adjacent` fails, and r1 is
 * never propagated.
 *
 * So the boundary in the existing entry should read: source ORDER OF THE
 * OPERANDS cannot reach a commutative role, but source ORDER OF THE STATEMENTS
 * can, whenever one of the operands is a live-in parameter -- because the thing
 * being decided is not which operand is first, it is whether combine gets to
 * delete the parameter copy.  Look for this wherever the ROM spends an extra
 * `mov rN, rPARAM` in front of a two-operand data-processing instruction.
 *
 * MEASURED.  Tail spellings (rom 78 lines), on the ret-variable body:
 *
 *   `2 - ((f8 ^ 0xff) != 0)`                          75 lines, 74 differ
 *   `2 - (f8 != 0xff)`                                75, 74
 *   `int ff = 0xff;` then `2 - ((f8 ^ ff) != 0)`      73, 74
 *   `unsigned char ff = 0xff;`, constant written first 75, 74
 *   boolean into its own local, then `2 - b`          72, 73
 *   `ret = b; ret = 2 - ret;`                         72, 73
 *   both arms as `k - b` with a shared join, b first  72, 74
 *   the same, k first                                 71, 74
 *   the same with `^`                                 73, 73
 *   explicit early returns instead of the variable    76, 64  (branchless at
 *                                                      last, wrong join)
 *   a separate local `v` then `ret = v`               78, 64
 *   `goto out` with the arms assigning ret            75, 74
 *   `1 + (f8 == 0xff)`                                77, 74  (BODY EXACT --
 *                                                      only the mask shifted)
 *
 * Mask spellings, on the `1 + (f8 == 0xff)` body (all first-diff at index 4):
 *
 *   twelve spellings listed above                     77 lines, 74 differ
 *   -fno-regmove                                      78, 19
 *   `ret = -1;` written before the mask               MATCH
 *   the same plus a separate `id` local               MATCH
 */
struct Item {
    unsigned char pad00[2];
    unsigned char f2;
    unsigned char pad03[0xc - 3];
    unsigned char fc;
    unsigned char pad0d[0x28 - 0xd];
    unsigned short f28;
};

struct Move {
    unsigned char pad00[1];
    unsigned char f1;
    unsigned char pad02[8 - 2];
    unsigned char f8;
};

extern struct Item *_GetItemInfo(int item);
extern struct Move *_GetMoveInfo(int id);
extern int _Func_808e990(int item);
extern int _CanEquipItem(int unit, int item);

int Func_80a46b4(int unit, int item)
{
    struct Item *info;
    struct Move *mv;
    int ret;

    /* This assignment is FIRST on purpose.  It puts two RTL insns between the
     * incoming copy of `item` and the `and` below, which is what stops combine
     * from folding r1 into the `and` and losing the ROM `mov r6, r1`.  See the
     * header. */
    ret = -1;
    item &= 0x1ff;
    info = _GetItemInfo(item);
    if (_Func_808e990(item) != 0)
        return 0;
    mv = _GetMoveInfo(info->f28 & 0x3fff);
    if (info->f28 != 0) {
        if (info->f2 == 0 || (info->fc != 3 && _CanEquipItem(unit, item) != 0))
            ret = 1;
        if (ret == 1) {
            if ((mv->f1 & 0x40) != 0)
                ret = 1 + (mv->f8 == 0xff);
            else
                ret = 0 - ((mv->f1 & 0x80) != 0);
        }
    }
    return ret;
}
