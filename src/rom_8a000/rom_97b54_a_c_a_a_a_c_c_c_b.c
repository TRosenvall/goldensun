/* Func_80982dc  --  0x080982dc
 *
 * Field proximity check: measure the party leader's distance from a marker,
 * tick a hold counter while it is inside, and write a state word back when the
 * distance opens up or the hold expires.
 *
 * THIS WAS PARKED, AND THE PARK'S CONCLUSION WAS WRONG. It sat at "the
 * instructions are identical, the size is identical, and the literal pool is in
 * a different order", with nine pool words in the ROM's order ROTATED -- 0x2090
 * moved from last to first -- and it closed with "NEXT: nothing source-level".
 * Three spellings of the final store had been measured and all three produced
 * byte-identical pool orders, which was taken as proof that the order was not
 * driven by source position at all.
 *
 * THE ORDER IS NOT DRIVEN BY POSITION. IT IS DRIVEN BY THE MODE OF THE
 * REFERENCE. That is the lever OvlFunc_909_200a1bc supplied a batch later:
 * add_minipool_forward_ref keeps the pool sorted by max_address, and a NARROW
 * reference has a 64-byte range where a word reference has a long one, so a
 * narrow entry is forced to sort EARLY.
 *
 * The ROM loads this constant with a WORD `ldr r3, =0x2090` and stores only its
 * low halfword with `strh`. Written as `*(short *)(...) = 0x2090;` the constant
 * is HImode, its reference is narrow, and it sorts FIRST instead of last --
 * which rotates the whole pool. An `int` intermediate makes the reference wide
 * and the entry sorts last, exactly as the ROM has it: 18 differing encodings
 * to 2.
 *
 * THE LAST TWO WERE AN ADJACENT SWAP: the ROM forms the store's ADDRESS
 * (`lsl r1, #1`) before loading the value, and the int-intermediate spelling
 * emits the value first. Binding the address to its own pointer ahead of the
 * value orders them the ROM's way.
 *
 * THE CONTROL IS WORTH KEEPING. Binding the address first but writing the value
 * as a CAST rather than an int local is back to 18 differing -- so it is the
 * local's MODE doing the work and not the parenthesisation, and the two levers
 * are independent rather than one effect seen twice.
 *
 * What the park had already got right and is kept: the signed-division idiom
 * (`cmp / bge / ldr =0xffff / add / asr #16` is `x / 0x10000`, not `x >> 16`,
 * and it appears twice), the call_via_r3 helper taken from the elevated
 * neighbour with its load-bearing "memory" clobber, named locals for both base
 * pointers, and `K << S` spellings for every constant the ROM builds rather
 * than pools. The prologue looks wrong and is not: r4 carries the actor pointer
 * across the indirect call while the push does not save it, which is this
 * tree's -fcall-used-r4 plus a helper clobbering only r12 and memory.
 *
 * A POOL-ORDER PARK IS NOT AUTOMATICALLY A DEAD END. tryc cannot see this class
 * at all -- it normalises PC-relative loads -- so objcmp is the only screen
 * that measures it, and the mode of each pooled constant's reference is the
 * first thing to vary.
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
        { short *q = (short *)(b + (0xbf << 1)); int v = 0x2090; *q = v; }
}
