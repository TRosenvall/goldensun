/* Cluster OvlFunc_917_20095a0..OvlFunc_917_20095a0 extracted from goldensun/asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_c.s.
 *
 * Total .text for this TU = 396 bytes (= 0x18c) -- the largest function landed so far.
 * Never attempted before batch 275. No pins, no flags, no .sym entry, no split.
 *
 * Spawns six particle actors in a ring on every tenth tick of a 0x79-tick cycle.
 *
 * ONE LEVER WAS ALL IT NEEDED: `i = 0` AS A STATEMENT, NOT A `for`-INIT. With `i` in the
 * for-init the first compile was 144 lines and 29 differing, with `i` in r7 and `ang` in r8 --
 * so the latch needs `add r7, #1 / cmp r7, #5` where the ROM spends four high-register
 * instructions. Hoisting `i = 0` out swaps the two and is exact. A declaration-order swap
 * alone (`ang` before `i`) is INERT at 29, so it is the statement position and not the
 * declaration.
 *
 * That is the fourth function in two batches to turn on this one thing, which is worth saying
 * plainly: when a loop counter and another preheader value are in each other's registers, try
 * the counter's zero as its own statement before anything else.
 *
 * NOTHING ELSE WAS NEEDED because the infrastructure was already in place, and it is worth
 * recording that the checks were cheap: `__udivsi3 = _udivsi3_RAM;` is already in
 * overlays/rom_7a4370/overlay.ld:67, and `.L1dc0` and `.L1dcc` are already `.global` in
 * asm/overlays/rom_7a4370/ovl_30_c_c_c_c_c_c_c_c.s:44-50. Three things that would each have
 * looked like a blocker, all already solved.
 *
 * NOTE ON objcmp's OUTPUT: it prints `~~ relocation _udivsi3_RAM / __udivsi3 is ONE symbol
 * (same address in the linked ELF)` before the OK line. That is the tool recognising the
 * overlay.ld alias, not a difference.
 */
struct Sprite {
    unsigned char pad00[9];
    unsigned char b0 : 2,
                  b2 : 2,
                  b4 : 4;
};

struct Actor {
    unsigned char pad00[0x30];
    int f30;
    unsigned char pad34[0x38 - 0x34];
    int f38;
    int f3c;
    int f40;
    unsigned char pad44[0x50 - 0x44];
    struct Sprite *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    short f64;
    short f66;
    unsigned char pad68[0x6c - 0x68];
    void *f6c;
};

extern int L1dc0[3] __asm__(".L1dc0");
extern int L1dcc __asm__(".L1dcc");

extern void __PlaySound(int id);
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern int __Func_8096c48(struct Sprite *s, int prev);
extern void __Actor_SetSpriteFlags(struct Actor *a, int n);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void OvlFunc_917_200952c(struct Actor *a);

void OvlFunc_917_20095a0(void)
{
    struct Actor *a;
    unsigned int i;
    unsigned int ang;
    int prev;
    int zero;

    prev = 0;
    switch (L1dcc) {
    case 0:
    case 10:
    case 20:
    case 30:
    case 40:
        __PlaySound(0xdc);
        i = 0;
        zero = 0;
        ang = 0;
        for (; i <= 5; i++) {
            a = __CreateActor(0x11d, L1dc0[0], L1dc0[1], L1dc0[2]);
            if (a != 0) {
                prev = __Func_8096c48(a->f50, prev);
                a->f55 = zero;
                a->f50->b2 = 1;
                __Actor_SetSpriteFlags(a, 0);
                __Actor_SetAnim(a, 1);
                a->f64 = zero;
                a->f66 = ang / (0xb4 << 1);
                a->f38 = L1dc0[0];
                a->f3c = L1dc0[1];
                a->f40 = L1dc0[2];
                a->f30 = 0x19999;
                a->f6c = OvlFunc_917_200952c;
            }
            ang += 0xf0 << 14;
        }
        break;
    }
    L1dcc += 1;
    if (L1dcc > 0x78)
        L1dcc = 0;
}
