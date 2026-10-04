/* Func_801219c (IsPositionOnMap) -- 0x0801219c
 *                                  (asm/rom_9000/rom_1219c_a_a_a.s)
 *
 * NON-MATCHING, 4 of 50 encodings.  MEASURED in batch 323, brief I.
 * Previous park figure: 17 of 50.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_9000/801219c.c \
 *     asm/rom_9000/rom_1219c_a_a_a.s --func Func_801219c
 *
 * SPLIT SHAPE: none.  asm/rom_9000/rom_1219c_a_a_a.s holds exactly one
 * .thumb_func_start (Func_801219c), so this would convert WHOLE.
 * PINS: 0.  No register pin, no inline asm, no device.
 *
 * THE PARK'S 17 WAS THREE CAUSES, AND THE COUNT HID A LENGTH DIFFERENCE.
 * objcmp reported 50 against 50, which reads like a distance.  It is not: the
 * ROM's 50 slots are 47 real instructions + one `.short 0x0000` alignment pad
 * + 2 pool words, and the park's 50 were 48 real instructions + 2 pool words.
 * THE PARK WAS ONE INSTRUCTION LONG and the equal count concealed it, because
 * the ROM's pad happened to occupy the slot the extra instruction took.
 *
 *   cause                                                    indices        n
 *   A  register exchange: x and layer, and x/16's register    6 24 25 29 35  7
 *                                                             37 38
 *   B  the `^ 0xff` idiom costs one extra instruction         39 40 41       3
 *   C  pure misalignment downstream of B (incl. one branch    20 42..47      7
 *      displacement at idx 20)
 *
 * B AND C ARE CLOSED.  B in full:
 *
 *     rom    movs r2, #255 / eors r3, r2                      (2 insns)
 *     park   mvns r3, r3 / lsls r3, #24 / lsrs r3, #24         (3 insns)
 *
 * The park named the tile byte into `int t = layer[...] ^ 0xff;` and returned
 * `(t != 0) - 1`.  Because the operand is a zero-extended QI, combine rewrites
 * `xor x 255` into `not x`, and then has to re-narrow with `lsl #24 / lsr #24`
 * -- `~x` is never zero, so the consumer IS sensitive to the high bits.  Net
 * +1 instruction, and that one instruction is the whole of C.
 *
 * The ROM's two-insn form is emit_store_flag's own reduction: for EQ/NE against
 * a nonzero constant gcc rewrites `x != 255` as `(x ^ 255) != 0` so that the
 * neg/orr/lsr#31 zero-test applies, and thumb has no EOR immediate, so the 255
 * must be materialised in a register.  That requires the COMPARISON to be the
 * expression, not a named value.  Writing the whole thing as
 * `(layer[...] != 0xff) - 1` takes 17 -> 7 in one edit.
 *
 * A IS HALF CLOSED, by declaration order.  Global alloc decides it --
 * .18.greg says `;; 10 regs to allocate: 38 43 52 54 32 35 51 34 33 36`, so
 * the denominator is allocno[v].live_length, NOT local-alloc's death-birth.
 * Pseudo 33 is x (set at insn 22, REG_EQUAL `div 65536`); pseudo 36 is layer
 * (set at insn 61 from `mem(reg 49)`).  They are the LAST TWO allocated, 33
 * then 36, and 33's conflict set already excludes hard r0/r2/r3 -- so 33 took
 * r1 and 36 was left r4, the exact reverse of the ROM.  Declaring `layer`
 * BEFORE `x` reverses the pair and takes 7 -> 4.
 *
 * THE REMAINING 4, which is ONE cause in two halves:
 *
 *     idx 29   rom  asrs r4, r3, #4     ours  asrs r2, r3, #4
 *     idx 35   rom  adds r3, r4, r3     ours  adds r3, r3, r2
 *     idx 37   rom  adds r1, r1, r3     ours  adds r3, r3, r1
 *     idx 38   rom  ldrb r3, [r1, #2]   ours  ldrb r3, [r3, #2]
 *
 *   (a) idx 29/35: the ROM REUSES x's register for x/16 (x dies at the copy
 *       into the divide temp, so r4 is free); we take a fresh r2.
 *   (b) idx 37/38: the final `layer + index` accumulates into layer's register
 *       in the ROM and into the index's register in ours.
 *
 * MEASURED AND INERT (exactly tie this body at 4 -- candidate prerequisites,
 * not dead ends):
 *   declaring a spare `unsigned char *q`
 *   separating `int x, z;` into two declarations, in either order
 *   loading layer through `(unsigned char *)*(int *)(...)`
 * MEASURED AND WORSE:
 *   `x = x / 16;` / `x /= 16;` as a statement            8   (was 8 on the
 *                                                            7-body too, so
 *                                                            it is worse on
 *                                                            BOTH bases)
 *   `q = layer + (...)*4; return (q[2] != 0xff) - 1;`    7
 *   hoisting `m = iwram_3001e70;` above x                22  RELOCDIFF
 *   computing z before x                                 40  RELOCDIFF
 *   naming the 0xc8<<1 offset                            35  RELOCDIFF
 *   `(z / 16) * 128 + x / 16` operand order              12
 * Crossed to depth 2 with tools/crossfire.py; no pair beats 4, and
 * crossfire's memory screen reports the reference profile ldr=8 ldrb=1 with no
 * MEM flag on this body.
 *
 * ALSO CORRECTED: the park's prose has the semantics backwards.  `(t != 0) - 1`
 * with `t = b ^ 0xff` returns -1 when the tile byte IS 0xff and 0 otherwise,
 * not "zero when the tile byte is 0xFF and -1 otherwise".
 */
extern char *iwram_3001e70;

int Func_801219c(int *pos)
{
    unsigned char *layer;
    char *m;
    int x, z;

    x = pos[0] / 0x10000;
    z = (pos[2] - pos[1]) / 0x10000;
    m = iwram_3001e70;
    if (m == 0)
        return 0;
    layer = *(unsigned char **)(m + (0xc8 << 1));
    return (layer[(x / 16 + (z / 16) * 128) * 4 + 2] != 0xff) - 1;
}
