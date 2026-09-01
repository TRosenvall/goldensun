/* Func_8011f54 (0x08011f54) -- NON-MATCHING.
 * Blocker class: instruction selection -- gcc COMBINES a copy and a shift that
 * the original build kept separate.
 *
 * 53 lines against the ROM's 57. The body is right: every instruction from the
 * table lookup through the indirect call appears in the ROM's order, and the
 * push list matches. Four lines are missing, and two of them are this, twice:
 *
 *     rom    mov r5, r1 / ... / asr r5, #0x10
 *     ours   asr r5, r1, #0x10
 *
 * The parameter is copied into a callee-saved register and shifted IN PLACE.
 * gcc-2.96 has the three-operand `asr rd, rs, #imm` encoding and uses it, so
 * the copy never becomes its own insn. Writing `x = fx; x >>= 16;` as two
 * statements is byte-identical -- the fold happens well before RTL.
 *
 * MEASURED (rom 57 lines):
 *   baseline                                             55, 55
 *   `x = fx; x >>= 16;` as two statements                55, 55 (inert)
 *   named table offset for the reg+reg load              53, 55
 *   named pointer for the ewram_202c000 read             53, 55  <- kept
 *   -fno-gcse / -fno-strict-aliasing / -fno-strength-reduce /
 *     -fno-rerun-cse-after-loop / -fno-schedule-insns2   53, 55 (ALL inert)
 *
 * WHAT IS RIGHT, and is why this park is worth reading -- the file contains
 * BOTH halves of the batch-174 addressing discriminator, in one function, ten
 * instructions apart:
 *
 *   `ldr r2, [r1, r3]` for the table pointer -- base plus INDEX. Written with
 *   the offset inline, gcc computes the address first. Naming the offset
 *   (`off = ((a & 3) * 3 << 4) + (0x98 << 1); src = *(...)(base + off);`)
 *   gives the ROM's form.
 *
 *   `add r0, r1, r3 / ldrb r0, [r0, #0x0]` for the ewram read -- address
 *   COMPUTED first. Written as `ewram_202c000[t]` gcc gives reg+reg, the
 *   opposite. A named pointer (`q = ewram_202c000 + t; k = *q & 0xf;`) gives
 *   the ROM's form, AND fixes the push list from {r5, r6, lr} to
 *   {r5, r6, r7, lr}.
 *
 * So the discriminator is not a property of the function or the file -- the
 * same translation unit wants an index in one place and a pointer ten
 * instructions later, and the ROM tells you which each time. Read the load.
 *
 * Also right: `_call_via_r3` from an ordinary call through
 * `void (*L134fc[])(unsigned char *, int, int)`; the signed `/ 16` expansion
 * (`cmp / bge / add #0xf / asr #4`) from writing plain division; and the
 * `iwram_3001e70`-based table at `(a & 3) * 48 + 0x130`, the same table
 * OvlFunc_947_2008fcc uses.
 *
 * NEXT: nothing source-level. Five flag groups and two spellings of the shift
 * are inert.
 */
extern unsigned char *iwram_3001e70;
extern unsigned char gBuffer[];
extern unsigned char ewram_202c000[];
extern unsigned char ewram_202c001[];
extern void (*L134fc[])(unsigned char *p, int x, int y) __asm__(".L134fc");

void Func_8011f54(int a, int fx, int fy)
{
    unsigned char *base;
    unsigned char *src;
    int x;
    int y;
    int t;
    int k;
    int off;
    unsigned char *q;

    base = iwram_3001e70;
    x = fx >> 16;
    y = fy >> 16;
    src = gBuffer;
    if (base != 0)
        off = ((a & 3) * 3 << 4) + (0x98 << 1);
        src = *(unsigned char **)(base + off);
    t = src[((x / 16) + (y / 16) * 128) * 4 + 3] * 4;
    q = ewram_202c000 + t;
    k = *q & 0xf;
    L134fc[k](ewram_202c001 + t, x & 0xf, y & 0xf);
}
