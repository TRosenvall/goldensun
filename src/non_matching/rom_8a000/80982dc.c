/* Func_80982dc -- 0x080982dc  [asm/rom_8a000/rom_97b54_a_c_a_a_a_c_c_c_b.s]
 *
 * NOT MATCHING, and it is a class this tree had not seen: THE INSTRUCTIONS ARE
 * IDENTICAL, THE SIZE IS IDENTICAL, AND THE LITERAL POOL IS IN A DIFFERENT
 * ORDER. 86 instructions against 86, 196 bytes against 196, zero differing
 * mnemonics -- and `make compare` fails.
 *
 * Field proximity check: measure the party leader's distance from a marker,
 * tick a hold counter while it is inside, and write a state word back when the
 * distance opens up or the hold expires.
 *
 * WHY tools/tryc.py CANNOT SEE THIS. It normalises every PC-relative load to
 * `=value`, which is exactly what makes it robust against pool PLACEMENT -- and
 * blind to pool ORDER. Both objects disassemble to the same instruction stream;
 * every `ldr rX, [pc, #N]` simply points four bytes further in ours:
 *
 *     ROM   ... 0xcba, 0xcbc, 0xffff, Func_8000888, 0xd105, 0xcbe, 0x2090
 *     ours  0x2090, iwram_3001ebc, gState, 0xcba, 0xcbc, 0xffff, Func_8000888,
 *           0xd105, 0xcbe
 *
 * Same nine words. Ours is the ROM's order ROTATED: 0x2090 moved from last to
 * first, everything else in the same relative order.
 *
 * THE CAUSE, and it is why no spelling fixes it. The ROM's .s reaches its
 * constants with `ldr rX, =value`, so the ASSEMBLER builds the pool in the
 * order the instructions reference them -- and 0x2090 is referenced by the very
 * last store. gcc emits its own explicit `.word` list in ITS OWN order, which
 * is not reference order. Since our instruction stream already matches the
 * ROM's exactly, there is no statement left to move.
 *
 * MEASURED -- three spellings of the final store, chosen to change when gcc
 * expands that constant, ALL produce byte-identical pool orders:
 *
 *     the `||` split into two ifs with a shared `goto` target
 *     the two `||` operands exchanged
 *     the store's address bound to a named `short *` first
 *
 * That the three agree exactly is the evidence that the order is not driven by
 * source position at all.
 *
 * WHAT IS ALREADY RIGHT, and worth keeping: the signed-division idiom
 * (`cmp / bge / ldr =0xffff / add / asr #16` is `x / 0x10000`, not `x >> 16` --
 * it appears twice), the `call_via_r3` inline-asm helper taken verbatim from
 * the elevated neighbour src/rom_8a000/rom_97b54_a_c_c_a_c_c_c_c_b.c (its
 * "memory" clobber is load-bearing -- it forces the post-call re-read of the
 * hold counter), named locals for both base pointers, and `K << S` spellings
 * for every constant the ROM builds rather than pools.
 *
 * The prologue looks wrong and is not: r4 carries the actor pointer across the
 * indirect call while `push {r5, r6, lr}` does not save it. That is this tree's
 * -fcall-used-r4 plus a helper that clobbers only r12 and memory.
 *
 * NEXT: nothing source-level, unless gcc's pool order can be moved by changing
 * WHICH constants exist -- e.g. if one of the nine could be built rather than
 * pooled, the rotation might close. 0x2090 itself is not buildable by
 * `mov`/`lsl`. Screening this class needs an OBJECT-LEVEL check; see
 * tools/objcmp.py, added for it.
 */
struct Actor {
    unsigned char pad0[8];
    int f8;
    int fc;
    int f10;
};

extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern struct Actor *GetFieldActor(int id);
extern int Func_8000888(int a, int b);

static inline int call_via_r3(int a, int b)
{
    register int (*_f)(int, int) __asm__("r3") = Func_8000888;
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\tr3"
        : "=r" (_a)
        : "r" (_f), "0" (_a), "r" (_b)
        : "memory", "r12"
    );
    return _a;
}

void Func_80982dc(void)
{
    unsigned char *b;
    unsigned char *g;
    struct Actor *a;
    int dx;
    int dz;

    b = iwram_3001ebc;
    g = gState;
    a = GetFieldActor(*(int *)(g + (0xfa << 1)));
    if (*(short *)(b + (0xcc << 4)) != 0 && *(short *)(b + 0xcba) != 0)
        *(short *)(b + 0xcba) -= 1;
    dx = *(short *)(b + 0xcbc) - a->f8 / 0x10000;
    dx = call_via_r3(dx, 0xd105);
    dz = *(short *)(b + 0xcbe) - (a->f10 - a->fc) / 0x10000;
    if (dx * dx + dz * dz >= 0xe1 << 4 || *(short *)(b + 0xcba) == 0)
        *(short *)(b + (0xbf << 1)) = 0x2090;
}
