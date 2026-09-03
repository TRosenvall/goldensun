/* OvlFunc_960_2008b24  --  0x02008b24
 * asm/overlays/rom_7eaf28/ovl_314_c_c_a_a.s, line 11 (first of two functions).
 *
 * PARKED at 21 aligned of 88.
 *
 * BLOCKER CLASS: 2, register birth order. The whole residue is ONE SWAP: the
 * ROM puts the second parameter in r6 and the gState base in r5, and we put
 * them the other way round. REG_ALLOC_ORDER's call-saved run is 4, 5, 6, ... so
 * r5 goes to the higher-priority (shorter-lived) quantity; the ROM therefore
 * ranks the LATE-BORN base above the parameter, and we rank the parameter above
 * the base. Every differing instruction is that swap propagating, plus the
 * `mov r5, r1` copy the ROM makes to establish the callee-saved copy and we do
 * not.
 *
 * THE PUSH LISTS ARE IDENTICAL -- both `push {r5, r6, lr}` -- so this is not
 * register pressure and not an extra live value. It is purely the ranking of
 * two quantities, which is what makes it a clean specimen of the class.
 *
 * THE LADDER WAS RUN TO THE END AND NOTHING MOVED IT.
 *
 *   base local assigned at four different points        23 / 23 / 23 / 24
 *   the parameter copied into its own local             23
 *   six callees declared int rather than void           23 (one at 25)
 *   distinct locals per block, no reuse                 21   <- best
 *
 * The first three tie at EXACTLY 23, which by this notebook's own rule means
 * the residue is not in those variables; the return-type rung of the ladder is
 * therefore also spent, and one of the six was actively worse.
 *
 * The one thing that did help was the read-count rule's third face -- the first
 * candidate reused a single `base`/`off` pair across all three address blocks,
 * and giving each block its own locals was worth 2. That is the "never reuse
 * one local for two roles" lever, and it is worth noting it fired here for only
 * 2 where it was worth 28 on Func_8021228: the difference is that there the
 * reused local was a genuine two-def pseudo feeding a priority sort, and here
 * the blocks are already separated by calls.
 *
 * WHAT IS ALREADY EXACT and should not be re-derived: the statement-form
 * runtime offset build for both gState reads and the iwram read, copied from
 * the matched sibling src/overlays/rom_7eaf28/ovl_314_a_c.c in this same
 * overlay; `_AREA_a4` and `_AREA_a5` as symbols, both already in area.sym (the
 * ROM pools 0xa4 and 0xa5, which are BELOW 256 and would otherwise be a bare
 * `mov` -- textbook pooled-small-constant tell); the `int zero` for the halfword
 * store, which is blocker 1b's SImode escape; and the whole call sequence,
 * argument order and branch structure.
 *
 * NEXT MOVE FOR WHOEVER PICKS THIS UP: the `-da` `.18.greg` dump prints
 * `;; 5 regs to allocate: 34 40 39 33 38`, which is the priority order
 * directly. Identify which pseudo is the gState base and which is the
 * parameter, then find the source change that lifts the base above it. That is
 * a bounded question and the compiler will answer it -- it was not pursued here
 * only because the round's remaining effort was better spent elsewhere.
 *
 * Screened with tools/tryc.py --align. Not built.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern int _AREA_a4;
extern int _AREA_a5;

extern void __ClearFlag(int id);
extern void __SetFlag(int id);
extern void __SetFlagByte(int id, int v);
extern void __Func_8091eb0(int a, int b);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern unsigned char *__MapActor_GetActor(int slot);

void OvlFunc_960_2008b24(int a, int slot)
{
    unsigned int wbase;
    unsigned int woff;
    unsigned int abase;
    unsigned int aoff;
    unsigned int g;
    unsigned int goff;
    short v;
    short w;
    int zero;

    wbase = (unsigned int)iwram_3001ebc;
    woff = 0xc1;
    woff <<= 1;
    wbase += woff;
    woff = 0;
    zero = 0;
    w = *(short *)((char *)wbase + woff);
    if (w == 0x63)
        *(short *)wbase = zero;
    __ClearFlag(0x20f);
    abase = (unsigned int)&gState;
    aoff = 0xe0;
    aoff <<= 1;
    abase += aoff;
    aoff = 0;
    v = *(short *)((char *)abase + aoff);
    if (v == (int)(&_AREA_a4))
        __SetFlag(slot + 0x2f9);
    else if (v == (int)(&_AREA_a5))
        __SetFlag(slot + 0x309);
    __SetFlagByte(0x84 << 2, 0);
    __Func_8091eb0(0x62, 5);
    g = (unsigned int)&gState;
    *(char *)(g + 0x22b) = 3;
    goff = 0xe0;
    goff <<= 1;
    v = *(short *)((char *)(g + goff));
    if (v == (int)(&_AREA_a5)) {
        if (slot == 0xb) {
            __Func_8091eb0(0x62, 7);
        } else if (slot == 0xc) {
            __Func_8091eb0(0x62, 6);
            __MapActor_SetIdle(0xc);
            __MapActor_SetPos(0xc, 0, 0);
        }
    }
    goff = 0xfa;
    goff <<= 1;
    *(__MapActor_GetActor(*(int *)(g + goff)) + 0x55) = 3;
}
