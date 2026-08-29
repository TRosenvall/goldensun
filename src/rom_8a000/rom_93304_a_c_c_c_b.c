/* Actor_SetBehavior  --  0x08093a6c
 *
 * Cut out of goldensun/asm/rom_8a000/rom_93304_a_c_c_c.s.
 *
 * Gives an actor one of seven stock behaviour scripts by id.
 *
 * THE SECOND PARAMETER IS BOTH THE ID AND THE FALLBACK. The switch has no
 * default block: the range check `sub r3, r1, #1 / cmp r3, #6 / bhi` jumps
 * straight to the call site with r1 UNTOUCHED, so anything outside 1..7 is
 * handed to _Actor_SetScript as-is. That only makes sense if the parameter is
 * a script pointer that small values overload as ids, and it is why the C
 * writes into the parameter rather than into a separate local -- a local would
 * need a default arm to initialise it and would cost a block.
 *
 * Case 6 is the only one that does more than pick a script, and its extra work
 * (stashing the followed actor at +0x68) sits inside the case, which is where
 * the jump table's sixth slot points.
 *
 * The gState base is a local; written inline gcc folds the 0x1f4 offset into
 * the pool.
 */
struct A { unsigned char pad00[0x68]; void *f68; };

extern unsigned char gState[];
extern unsigned char L9fd44[] __asm__(".L9fd44");
extern unsigned char L9fe00[] __asm__(".L9fe00");
extern unsigned char L9fe04[] __asm__(".L9fe04");
extern unsigned char L9fe10[] __asm__(".L9fe10");
extern unsigned char L9fecc[] __asm__(".L9fecc");
extern unsigned char L9ff18[] __asm__(".L9ff18");
extern unsigned char L9ff2c[] __asm__(".L9ff2c");

extern void *MapActor_GetActor(int slot);
extern void _Actor_SetScript(struct A *a, unsigned char *s);

void Actor_SetBehavior(struct A *a, unsigned char *s)
{
    unsigned char *g;

    switch ((int)s) {
    case 1:
        s = L9fe00;
        break;
    case 2:
        s = L9fd44;
        break;
    case 3:
        s = L9fe10;
        break;
    case 4:
        s = L9fecc;
        break;
    case 5:
        s = L9ff18;
        break;
    case 6:
        g = gState;
        a->f68 = MapActor_GetActor(*(int *)(g + (0xfa << 1)));
        s = L9ff2c;
        break;
    case 7:
        s = L9fe04;
        break;
    }
    _Actor_SetScript(a, s);
}
