/* Cluster Func_80a32b8..Func_80a32b8 extracted from goldensun/asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c.s.
 *
 * Total .text for this TU = 156 bytes (= 0x9c).
 * Preserves the original ROM layout when slotted between
 * asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_a.o and asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_c.o in
 * goldensun/stage1.ld.
 *
 * Never attempted before batch 272. No pins, no flags.
 *
 * ONE LEVER, 3 differing to exact: the constant 1 is a NAMED LOCAL DECLARED AND
 * INITIALISED AT THE TOP. As a bare literal gcc emits `ldr r3, =0x1` where the ROM
 * has `mov r3, #0x1` -- the recorded HImode-immediate rule, the value meeting a
 * halfword store.
 *
 * THE SPLIT DECLARATION IS NOT EQUIVALENT, and this is the useful half: `int one;`
 * at the top with `one = 1;` inside the guarded block is 22 differing, far WORSE
 * than the bare literal's 3, because the extra pseudo shifts register numbering
 * from line 4 onward and rewrites the whole offset chain. `int one = 1;` at the top
 * is exact. That confirms docs/elevation.md's "declared at the top, with its
 * initialiser" form as load-bearing rather than stylistic, and adds the measurement
 * that splitting it is not merely inert but harmful.
 *
 * The ROM's offset chain (`mov r2,#0xba / lsl #1` for 0x174, `ldr r3,=0x21a`,
 * `add r2,#0xa7` for 0x21b, `mov r2,#0xbc / lsl #1` for 0x178) is gcc's own and is
 * written as plain literals, per the batch-56 note in the file-mate
 * src/rom_a1000/rom_a1814_c_c_c_b.c, which also supplied the `(0xbc << 1)` and
 * `unsigned char *iwram_3001f2c` spellings.
 *
 * A CORRECTION TO THE DISASSEMBLY COMMENT: the .s says the target byte is at
 * state+0x261. It is at state+0x21b (0x174 + 0xa7), adjacent to the user byte at
 * 0x21a.
 */
extern unsigned char *iwram_3001f2c;
extern int Func_80a9e48(int item, int a, int b);
extern void _PlaySound(int id);
extern void _Func_80164ac(int x);
extern void Func_80a1d08(int a, int b, int c);
extern void Func_80aa448(int id);
extern void _CalcStats(int id);

int Func_80a32b8(void)
{
    unsigned char *p;
    int r;
    int one = 1;

    p = iwram_3001f2c;
    r = Func_80a9e48(*(unsigned short *)(p + (0xba << 1)),
                     *(unsigned char *)(p + 0x21a),
                     *(unsigned char *)(p + 0x21b));
    if (r == -1) {
        _PlaySound(0x72);
        _Func_80164ac(*(int *)(p + 0x2c));
        Func_80a1d08(0xbef + *(short *)(p + 0x25a), r, r);
        *(unsigned short *)(p + 0x222) = one;
        return r;
    }
    Func_80aa448(*(unsigned short *)(p + (0xbc << 1)) & 0x1ff);
    _CalcStats(*(unsigned char *)(p + 0x21a));
    _CalcStats(*(unsigned char *)(p + 0x21b));
    return 1;
}
