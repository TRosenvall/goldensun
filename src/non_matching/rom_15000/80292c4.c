/* Func_80292c4 (DrawDjinnRow) -- NON-MATCHING, 36 encodings of 91 against the tree
 * reference.  SIZE EXACT (200 bytes both), ENCODING COUNT EXACT (91 = 91), and ALL
 * NINE RELOCATIONS IDENTICAL (objcmp prints no RELOCATIONS line).  NO SHIMS.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_15000/80292c4.c \
 *     asm/rom_15000/rom_23178_a_c_c_c.s --func Func_80292c4
 * SPLIT: rom_23178_a_c_c_c.s holds Func_8029274, Func_80292c4 and Func_802938c.
 *
 * Indices 0-15 are identical -- prologue, frame (0x24), both stack arrays at the
 * ROM's offsets (buf[0x11] at sp+8, num[5] at sp+0x1c), and the first UIDrawText.
 *
 * THREE LEVERS, ALL LOAD-BEARING, each measured:
 *
 * 1. THE FLAG LOOP IS AN INDEX `for`, NOT A POINTER WALK.  The ROM's exit test is
 *    `cmp r5,r6 / ble` -- SIGNED -- on what are plainly two pointers.  Pointer
 *    comparisons are unsigned in gcc, so `p <= buf+0xf` gives `bls` and is wrong.
 *    `for (j = 0; j < 0x10; j++) buf[j] = ...` gives `ble`: loop strength reduction
 *    replaces the biv test with the giv but KEEPS THE ORIGINAL COMPARISON'S
 *    SIGNEDNESS.  95 -> 89 instructions and the whole loop lines up.
 *    **A signed `cmp/ble` between two pointers is a tell for an index loop.**
 * 2. THE NUL TERMINATOR IS WRITTEN THROUGH THE LOOP INDEX.  The ROM's
 *    `mov r3,#0x10 / mov r2,#0 / mov r1,r8 / strb r2,[r1,r3]` is a REGISTER-OFFSET
 *    store; `buf[0x10] = 0` gives `add r2,sp,#24 / strb`.  Writing `buf[j] = 0`
 *    after the loop (j == 0x10 there) produces the ROM's form exactly.  40 -> 36.
 * 3. THE num CLEAR LOOP RUNS BACKWARD.  Forward (`p = num; do {*p=0;p++;} while
 *    (p != num+5);`) gives the ROM's 7-instruction shape but puts the BASE in the
 *    spill slot and the END pointer in a register; the ROM does the opposite.
 *    Backward (`p = num+5; do {p--; *p=0;} while (p != num);`) gets the ROM's
 *    split -- sp[0] = num+5 spilled, num in a hard register -- for one instruction
 *    less in the loop.  Whichever pointer is the COMPARISON operand wins the
 *    register, because its ref sits at loop depth 2.
 *    Measured and worse: pointer `<` (97), `while (p - num != 5)` (91/75 diffs),
 *    an explicit `e = num + 5` hoisted or in-loop (93), index `for (u=0;u!=5;u++)`
 *    (91/45), index `for (j=0;j<5;j++)` (89 -- gcc REVERSES it and rematerialises
 *    num inside the loop).
 *
 * BLOCKER: register allocation.  `buf` and `num` get r8/r11 swapped -- the ROM has
 * buf in r8 and num in r11, gcc the reverse -- and that is the whole residue
 * (box r10, y r9, f r7, num+5 and i spilled are all already the ROM's).
 * The cause is measurable: the backward clear loop puts `num` in the INNER loop's
 * compare, which lifts its priority above `buf`'s.  The cure and its cost are both
 * known: naming BOTH arrays through pointer locals assigned before the outer loop
 * (`bp = buf; np = num;`) gives ALL FIVE of the ROM's registers -- buf r8, num r11,
 * box r10, y r9, f r7, num_end spilled -- but costs ONE instruction, because
 * `bp = buf` materialises as `mov r1,#8 / add r1,r1,sp / mov r8,r1` where the ROM
 * spends `add r2,sp,#8 / mov r8,r2`.  That variant is scratch_elev/b284/C/g1.c at
 * 92 instructions.  So the two spellings trade one instruction against five
 * registers and NEITHER is exact; the missing piece is why the ROM reaches the
 * Thumb `add Rd,SP,#imm` form for one frame address and the generic
 * `mov Rd,#imm / add Rd,SP` form for another in the same block.
 */
extern void Func_8016478(void *box);
extern void UIDrawText(unsigned char *text, void *box, int x, int y);
extern void Func_8029274(unsigned int v, int n, unsigned char *out);
extern int _GetFlag(unsigned int id);
extern unsigned char L37428[] __asm__(".L37428");
extern unsigned char L3742c[] __asm__(".L3742c");

void Func_80292c4(void *box, int row)
{
    unsigned char num[5];
    unsigned char buf[0x11];
    int i;
    int j;
    int y;
    unsigned int f;
    unsigned char *p;

    Func_8016478(box);
    UIDrawText(L3742c, box, 0x30, 0);
    f = row << 8;
    y = 0x10;
    i = 0;
    do {
        p = num + 5;
        do { p--; *p = 0; } while (p != num);
        Func_8029274(f, 3, num);
        UIDrawText(num, box, 0, y);
        UIDrawText(L37428, box, 0x20, y);
        for (j = 0; j < 0x10; j++) {
            buf[j] = (_GetFlag(f) != 0) + 0x30;
            f++;
        }
        buf[j] = 0;
        UIDrawText(buf, box, 0x30, y);
        i++;
        y += 8;
    } while (i != 0x10);
}
