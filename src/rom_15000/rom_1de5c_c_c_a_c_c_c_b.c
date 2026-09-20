/* Cluster Func_801ea3c..Func_801ea3c extracted from goldensun/asm/rom_15000/rom_1de5c_c_c_a_c_c.s.
 *
 * Total .text for this TU = 160 bytes (= 0xa0). Never attempted before batch 273.
 * No pins, no flags. The idx idiom came verbatim from the file-mate
 * src/rom_15000/rom_1de5c_c_c_a_a_c_b.c.
 *
 * TWO LEVERS, and both are about which PASS gets to act.
 *
 * 1. THE PREFIX TILE IS STORED INSIDE BOTH IF-ARMS. A merged variable assigned in the
 *    arms and stored after the join lets `.14.ce` IF-CONVERT the else-load, hoisting it
 *    above the branch and dropping the ROM's `b`. With the STORE inside each arm, ce
 *    declines -- it if-converts single-insn register sets, not arms containing a memory
 *    store -- and `jump2` then CROSS-JUMPS the two identical `strh` tails into the join,
 *    which is the ROM's shape. The pass attribution came straight out of the dumps: the
 *    load visibly moves between `.13.combine` and `.14.ce`.
 *
 *    So WHERE AN IF/ELSE'S STORE LIVES DECIDES WHETHER `ce` FIRES, and the
 *    duplicate-store-in-both-arms trick costs nothing because cross-jumping undoes it --
 *    the same mechanism as the batch-271 "duplicate a shared store into both arms"
 *    lever, reached from the opposite direction.
 *
 * 2. THE DIGIT COPY MUST BE INDEX-BASED (`out[i + 2] = p[i]`), not a running pointer.
 *    An index lets loop strength-reduction build the destination pointer and keep its
 *    base in a register, giving the ROM's `add r2, r4, #4`; a hand-written running
 *    pointer lets cse fold the second frame-address pseudo and reload emits
 *    `add r2, sp, #4`. SEVENTEEN pointer spellings and five flags failed on that one
 *    instruction.
 *
 *    `add rX, r4, #imm` against `add rX, sp, #imm` is the readable tell for this, and
 *    loop spelling -- pointer against index -- is the lever. No `-fno-*` substitutes.
 */
/* Func_801ea3c  --  0x0801ea3c
 *
 * DrawNumberToBuffer.  Formats `value` into a 16-byte char scratch at sp+0x10
 * with PrintNum, builds an eight-halfword tile string at sp+0 (prefix tile,
 * 0xF01E, five digit tiles, terminator), then renders it with Func_801de5c at
 * the window-relative tile index -- same idx idiom as Func_801e8b0 next door.
 *
 * Two spellings are load-bearing:
 *  - the prefix tile is STORED INSIDE both if-arms.  Assigning a merged
 *    variable and storing once at the join lets pass .14.ce hoist the else-arm
 *    load above the branch and drop the `b`, costing one instruction.  With the
 *    store in the arms the arms are not simple register sets, ce leaves them
 *    alone, and jump2 cross-jumps the identical `strh r3, [r4]` tails into the
 *    join -- which is the ROM's shape.
 *  - the digit copy is INDEX-based (`out[i + 2] = p[i]`), not a running
 *    pointer.  With `q = out + 2; ... *q++ = *p++` cse folds the second frame
 *    address pseudo into the frame register and reload emits `add r2, sp, #4`;
 *    the ROM has `add r2, r4, #4`.  Letting loop's strength reduction build the
 *    destination pointer keeps the base in a register.  That one instruction was
 *    the whole residue: seventeen other spellings of `q` (casts through
 *    unsigned int / char *, a struct member array, an explicit `u16 *o = out`,
 *    q before or after the stores) all left it, as did -fno-gcse,
 *    -fno-rerun-cse-after-loop, -fno-strength-reduce, -fno-cse-follow-jumps
 *    and -fno-schedule-insns2.
 */
typedef unsigned char u8;
typedef unsigned short u16;

struct W { unsigned char pad0[0xc]; u16 fc; u16 fe; };

extern u16 *iwram_3001e8c;
extern char *PrintNum(char *dest, int num, unsigned int width);
extern int Func_801de5c(u16 *buf, u16 *a, u16 *b, int c);

void Func_801ea3c(int value, struct W *w, unsigned int x, unsigned int y, int flag)
{
    char digits[16];
    u16 out[8];
    u16 *vram;
    char *p;
    u16 *v2;
    u16 *vv;
    unsigned int a, b, n;
    int i, j, m;

    vram = iwram_3001e8c;
    p = PrintNum(digits, value, 4);
    if (flag == 0)
        out[0] = 0xf01d;
    else
        out[0] = 0xf01f;
    out[1] = 0xf01e;
    for (i = 0; i < 5; i++) {
        out[i + 2] = p[i];
    }
    out[6] = 0;
    a = w->fe + (y >> 3);
    a += 1;
    b = w->fc + (x >> 3);
    a <<= 5;
    a += b;
    n = a + 1;
    if (n < 0xa0 * 4) {
        vv = (u16 *)0x6002000 + n;
        v2 = vram + n;
        m = 7;
        m &= x;
        Func_801de5c(out, v2, vv, m);
    }
}
