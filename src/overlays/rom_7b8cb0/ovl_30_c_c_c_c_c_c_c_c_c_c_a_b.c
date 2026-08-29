/* OvlFunc_931_2008874  --  0x02008874
 *
 * Three independent flag fixups on entry, then a two-way area check.
 *
 * THE AREA IDS ARE NAMED CONSTANTS. 0x4b and 0x4c both fit in a `cmp`
 * immediate, and the ROM pools them -- `ldr r3, =0x4b` -- which only happens
 * for a linker symbol. `_AREA_4b` and `_AREA_4c` are already in area.sym and
 * `(int)&_AREA_4b` reproduces it. A literal gives `cmp r2, #0x4b` and the pool
 * entry disappears.
 *
 * The first flag pair is `||` and the second is `&&` on the same two flags,
 * read off which way each `b` jumps.
 */
extern unsigned char gState[];
extern int _AREA_4b;
extern int _AREA_4c;
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void OvlFunc_931_2008904(void);
extern void OvlFunc_931_2008b2c(void);

int OvlFunc_931_2008874(void)
{
    unsigned char *g;
    int e;

    if (__GetFlag(0x8fd))
        __SetFlag(0x90 << 2);
    if (__GetFlag(0x8fe) || __GetFlag(0x907))
        __SetFlag(0x241);
    if (__GetFlag(0x8fe) && __GetFlag(0x907))
        __SetFlag(0x242);
    g = gState;
    e = *(short *)(g + (0xe0 << 1));
    if (e == (int)&_AREA_4b)
        OvlFunc_931_2008904();
    else if (e == (int)&_AREA_4c)
        OvlFunc_931_2008b2c();
    return 0;
}
