/* Func_80a7380  --  0x080a7380
 *
 * Cut from the head of goldensun/asm/rom_a1000/rom_a7380_a.s; the eight
 * functions that follow it stay in _a_c.s. No data outside in-function pools.
 *
 * Allocate a working block, blank the picker row, run the picker, tear down.
 *
 * THE [cse] MARKER DID NOT HOLD, and no flag is needed. The repeated pooled item
 * is a global's ADDRESS, dereferenced at both ends of the body with calls in
 * between, and gcc reloads it from the pool at each site by itself. That is the
 * third counter-example this batch. The sharper statement: the CSE_CFLAGS shape
 * is a repeated VALUE consumed as a register argument, not a repeated symbol
 * address that is immediately dereferenced.
 *
 * BLOCKER 1b, with the mechanism read rather than inferred. The ROM has
 * `mov r3, #1 / strh` where the plainest spelling gives `ldrh r3, .L9 / strh`,
 * and this has nothing to do with the pool or with block placement. In the
 * Thumb halfword-move pattern the source constraint list puts an alternative
 * accepting any CONST_INT -- which loads from the pool -- BEFORE the
 * alternative that would emit a `mov`. recog takes the first match, so for a
 * HImode const_int the mov alternative is UNREACHABLE and every halfword store
 * of a literal pools. Probed in isolation: storing 1 pools, storing 0 pools,
 * and an `int` local stores with `mov`. THE FIX IS THE int TYPE, which puts the
 * value in SImode so the word move sets it and the store goes through a subreg.
 * 10 differing to 3.
 *
 * THE LAST THREE WERE AN r2/r3 SWAP, AND IT IS ARITHMETIC, NOT GUESSWORK.
 * local-alloc.c ranks quantities by
 * floor_log2(refs) * refs * size / (death - birth) and says in its own comment
 * that shorter-lived quantities get higher priority; REG_ALLOC_ORDER starts
 * r3, so the shortest-lived quantity takes r3. The -da .17.lreg dump prints the
 * spans directly -- with the constant as an initialiser it lived across ten
 * insns and lost r3 to the four-insn pointer.
 *
 * The recipe needs BOTH halves and that is the part worth keeping: give the
 * operand you want in the LOWER register its own statement FIRST, then assign
 * the r3 candidate immediately before its use. Assigning the constant late on
 * its own does not work, because the pointer expression is then born after it
 * and is shorter still.
 *
 * One case deliberately left alone: the tail's halfword zero is a bare literal,
 * and cse substitutes the SImode zero already live from the argument sequence,
 * which is exactly the ROM's shape. Do not "fix" that one with an int local.
 *
 * A counting note that cost a screen: for a `sub rN, #2` down-loop the array
 * base is top - 2*(count-1), not top - 2*count.
 */
extern unsigned char *iwram_3001e68[];

unsigned char *galloc_iwram(int, int);
void _Func_80170f8(int, int, int, int);
void WaitFrames(int);
void Func_80a1090(int);
int _Func_80796c4(unsigned char *);
void Func_80a8034(int, int, int, int);
int _CreateUIBox(int, int, int, int, int);
int Func_80a7440(void);
void _Func_80164ac(int);
void Func_80a34c0(void);
void gfree(int);

int Func_80a7380(void) {
    unsigned char *p = galloc_iwram(0x37, 0xa70);
    int r;
    int i;
    unsigned char *q;
    int one;
    int three = 3;

    q = iwram_3001e68[0];
    one = 1;
    *(short *)(q + 4) = one;
    _Func_80170f8(0, 0, 0x1e, 0x14);
    WaitFrames(1);
    Func_80a1090(0);
    *(unsigned char *)(p + 0x219) = _Func_80796c4(p + 0x208);
    Func_80a8034(0, 3, 0, 7);
    *(int *)(p + 0x10c) = _CreateUIBox(0xd, 0, 0x11, 5, 2);
    for (i = 7; i >= 0; i--)
        *(short *)(p + 0x144 + i * 2) = 0x1e;
    *(short *)(p + 0x220) = three;
    r = Func_80a7440();
    _Func_80164ac(*(int *)(p + 0x24));
    Func_80a34c0();
    *(short *)(iwram_3001e68[0] + 4) = 0;
    WaitFrames(1);
    gfree(0x37);
    return r;
}
