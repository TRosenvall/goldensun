/* Func_801ef08  --  0x0801ef08
 *
 * Cut out of goldensun/asm/rom_15000/rom_1de5c_c_c_c_c_a_a_a_c.s.
 *
 * Builds one UI box: allocate the descriptor, raise a re-entrancy flag at
 * [iwram_3001e8c]+0xea6, run the caller's setup, create the box from the four
 * halfwords the setup filled in, run the caller's teardown, drop the flag.
 *
 * THE ZERO MUST BE A NAMED LOCAL, and this one IS forced. The ROM keeps it in
 * r10 across three calls (`mov r3, #0 / mov r10, r3` at the top, `mov r3, r10 /
 * strb r3, [r6]` at the bottom). Written as a bare `*flag = 0;` the function
 * comes out at 35 instructions against 39, diverging at the first -- gcc has to
 * rebuild it after the calls and the register pressure changes around it.
 *
 * The discriminator is that the value CROSSES CALLS. That is what separates
 * this from the several cases now on record where a value sitting in a
 * callee-saved register proved nothing about the source: see batch 94's note in
 * docs/elevation.md, and src/overlays/common/common1_c_a_c_c_a_b.c, where three
 * different spellings of a shared zero all give identical code because none of
 * them has to survive anything.
 *
 * The 1 is written inline below, but that is not forced -- naming it as well
 * also matches. Only the zero is.
 *
 * The four box dimensions are read back out of the descriptor the setup call
 * filled in -- `ldrh r3, [r5, #0xa]` and friends, in reverse argument order --
 * and the fifth argument goes on the stack.
 *
 * Matched on the first screen.
 */
struct Box {
    int f0;
    unsigned short f4;
    unsigned short f6;
    unsigned short f8;
    unsigned short fa;
};

extern unsigned char *iwram_3001e8c;
extern struct Box *galloc_ewram(int a, int b);
extern void Func_801eea0(int a);
extern void Func_801f200(int a);
extern int CreateUIBox(int a, int b, int c, int d, int e);

void Func_801ef08(int arg)
{
    struct Box *p;
    unsigned char *flag;
    int zero;

    p = galloc_ewram(0x10, 0x10);
    flag = iwram_3001e8c + 0xea6;
    zero = 0;
    *flag = 1;
    Func_801eea0(arg);
    p->f0 = CreateUIBox(p->f4, p->f6, p->f8, p->fa, 6);
    Func_801f200(arg);
    *flag = zero;
}
