/* Func_808f28c  --  0x0808f28c
 *
 * The last function of goldensun/asm/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c.s; see
 * rom_8d9a4_c_a_c_c_c_c_c_a.c for how that file was divided.
 *
 * A one-in-ten sparkle: roll, and on a low roll spawn a particle actor at the
 * caller's position jittered by two more rolls.
 *
 * THE ROLL IS A SCALED UNSIGNED MULTIPLY. `mov r3, #0x64 / mul r3, r0 /
 * lsr r3, #16 / cmp r3, #9 / bhi` is `Random() * 100 >> 16`, and the `lsr` and
 * the `bhi` between them say Random returns UNSIGNED -- the same reading as
 * src/overlays/rom_7ca63c/ovl_30_c_c_a_c_c_b.c, where it showed up as lsr
 * against asr.
 *
 * THE MASK IS A NAMED `int`. The ROM builds 0xfffffff3 as
 * `mov r3, #0xd / neg r3, r3` and then does `and r3, r2`:
 *
 *      mov r3, #0xd / ldrb r2, [r1, #9] / neg r3, r3 / and r3, r2
 *      mov r2, #4 / orr r3, r2 / strb r3, [r1, #9]
 *
 * Written inline, `(p->f9 & ~0xc) | 4` narrows to byte width and comes out at
 * 49 instructions against 50 with 12 differing. Assigning `~0xc` to a named
 * `int` first forces 32 bits and matches.
 *
 * OPERAND ORDER IS NOT PART OF IT -- measured, because the `and r3, r2` puts
 * the MASK in the destination and that looks like it should be reachable by
 * writing the mask on the left. It is not: `mask & p->f9` and `p->f9 & mask`
 * compile to the same fifty instructions. gcc picks the destination register
 * itself, which is the same conclusion src/non_matching/ovl_7ed0a0/2009458.c
 * reached the hard way. Only the TYPE of the mask is reachable from the C.
 *
 * Worth contrasting with src/rom_8a000/rom_944ec_c_c_a.c, which wants the same
 * ~0xc at the same width but reaches it as `sub r3, #0x11` from a 4 already in
 * the register. Here there is no nearby constant to derive from, so gcc spends
 * a `neg`. The C is the same in both; only the surroundings differ.
 *
 * The three coordinates go through a scratch array because vec3_translate takes
 * its destination by pointer -- `mov r6, sp` and three stores, then the same
 * three slots read back as arguments to CreateParticleActor.
 */
struct Spr { unsigned char pad00[9]; unsigned char f9; };

struct Actor {
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x28 - 0x14];
    int f28;
    unsigned char pad2c[4];
    int f30;
    int f34;
    unsigned char pad38[0x48 - 0x38];
    int f48;
    unsigned char pad4c[4];
    struct Spr *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[0x6c - 0x56];
    void *f6c;
};

extern unsigned char gState[];
extern unsigned char L9e75c[] __asm__(".L9e75c");
extern unsigned char L9e6c0[] __asm__(".L9e6c0");
extern unsigned char L9e87c[] __asm__(".L9e87c");
extern void Func_808eee4(void);
extern struct Actor *GetFieldActor(int id);
extern void _Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void WaitFrames(int n);
extern void _Actor_SetAnim(struct Actor *a, int anim);
extern void _Actor_SetSpriteFlags(struct Actor *a, int f);
extern void _Actor_SetScript(struct Actor *a, unsigned char *s);
extern unsigned int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern struct Actor *CreateParticleActor(int a, int b, int c, int d);

void Func_808f28c(struct Actor *a)
{
    int v[3];
    struct Actor *n;
    int r;
    int mask;

    if (Random() * 0x64 >> 16 <= 9) {
        v[0] = a->f8;
        v[1] = a->fc;
        v[2] = a->f10;
        r = Random();
        vec3_translate(r << 4, Random(), v);
        n = CreateParticleActor(0x11d, v[0], v[1], v[2]);
        if (n != 0) {
            _Actor_SetScript(n, L9e87c);
            _Actor_SetAnim(n, 0);
            mask = ~0xc;
            n->f50->f9 = (mask & n->f50->f9) | 4;
        }
    }
}
