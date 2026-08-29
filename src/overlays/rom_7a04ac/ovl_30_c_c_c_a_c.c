/* OvlFunc_913_200a7c8  --  0x0200a7c8
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and the linker script is unchanged.
 *
 * One frame in eight, spawn a falling-debris actor: play the cue if the scene
 * flag is set, create it, clear one sprite bit, set another, and send it down.
 *
 * CSE_CFLAGS PLUS THE BASIC-BLOCK LEVER, in that order, which is the batch-106
 * rule and the batch-107 rule applied in sequence:
 *
 *     plain literals, default flags               44 differing of 66
 *     plain literals, -fno-rerun-cse-after-loop    8
 *     four levered constants, with the flag        0
 *
 * The eight the flag leaves are both calls' shifted coordinates. `0xe7 << 16`
 * is passed to BOTH `__CreateActor` and `__Actor_TravelTo` and the ROM rebuilds
 * it at each -- REBUILT, so it gets its own local per site rather than one
 * shared. Four locals for four sites.
 *
 * THE MASKED VALUE IS THE STORED ZERO, and that falls out for free. The ROM
 * writes `strb r6, [..]` twice where r6 holds `iwram_3001e40 & 7` -- a value
 * the enclosing `if` has already proved is zero. Writing the stores as plain
 * `= z` is not what does it; writing them as `= 0` gives the same code, because
 * gcc's value numbering substitutes the register it already knows holds zero.
 * That is the "provably constant inside its branch is NOT evidence" note in
 * docs/elevation.md, seen from the producing side for once.
 *
 * Two masks on two different bytes, and they are NOT the same kind:
 *   `q->f23 &= 0xfe`  is hand-written masking -- a bare byte-width `mov r3,
 *   #0xfe`, which is batch 71's width rule saying "not a bitfield".
 *   `s->b2 = 1`       is a bitfield -- `mov r3, #0xd / neg r3, r3` is ~0xc at
 *   32-bit width, a two-bit field at offset 2. Same function, both rules.
 */
struct Sprite {
    unsigned char pad00[9];
    unsigned char b0 : 2,
                  b2 : 2,
                  b4 : 4;
    unsigned char pad0a[0x26 - 0xa];
    unsigned char f26;
};

struct Actor {
    unsigned char pad00[0x18];
    int f18;
    unsigned char pad1c[0x23 - 0x1c];
    unsigned char f23;
    unsigned char pad24[0x30 - 0x24];
    int f30;
    int f34;
    unsigned char pad38[0x50 - 0x38];
    struct Sprite *f50;
    unsigned char pad54;
    unsigned char f55;
};

extern unsigned int iwram_3001e40;
extern int L3398 __asm__(".L3398");
extern unsigned char gScript_913__0200b2d0[];
extern void __PlaySound(int id);
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);

void OvlFunc_913_200a7c8(void)
{
    struct Actor *q;
    struct Sprite *s;
    int z;
    int c1;
    int c2;
    int t1;
    int t2;

    c1 = 0xe7 << 16;
    c2 = 0xe6 << 17;
    t1 = 0xe7 << 16;
    t2 = 0x9c << 18;
    z = iwram_3001e40 & 7;
    if (z == 0) {
        if (L3398 != 0)
            __PlaySound(0xc8);
        q = __CreateActor(0x1a, c1, 0, c2);
        if (q != 0) {
            s = q->f50;
            s->f26 = z;
            q->f23 &= 0xfe;
            s->b2 = 1;
            q->f18 = 0x1999;
            q->f30 = 0x80 << 12;
            q->f34 = 0x80 << 12;
            q->f55 = z;
            __Actor_SetAnim(q, 2);
            __Actor_TravelTo(q, t1, 0, t2);
            __Actor_SetScript(q, gScript_913__0200b2d0);
        }
    }
}
