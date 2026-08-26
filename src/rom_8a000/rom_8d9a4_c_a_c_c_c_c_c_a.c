/* Func_808f0d8  --  0x0808f0d8
 * Func_808f140  --  0x0808f140
 *
 * The first two functions of goldensun/asm/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c.s.
 * Func_808f1c0 stays as assembly in rom_8d9a4_c_a_c_c_c_c_c_b.s and
 * Func_808f28c is elevated beside it in rom_8d9a4_c_a_c_c_c_c_c_c.c, so
 * stage1.ld lists three objects where it listed one.
 *
 * A summon entrance. Func_808f140 is the entry point and takes a bitmask:
 * bit 0 dresses the actor and gives it a script, bit 1 walks it to the party
 * leader (that is Func_808f0d8), and the two `== 3` tests add pauses when both
 * happen, so the full sequence is paced differently from either half alone.
 *
 * THE gState OFFSET MUST BE BUILT, NOT FOLDED, and that was the only lever
 * either function needed. Written as `*(int *)(gState + 0x1f4)` gcc folds the
 * whole thing into one pool entry:
 *
 *      rom    ldr r3, =gState / mov r2, #0xfa / lsl r2, #1 / add r3, r2 / ldr r0, [r3]
 *      ours   ldr r3, =gState+500 / ldr r0, [r3]
 *
 * Assigning `gState` to a local `unsigned char *` first blocks the fold -- the
 * local holds the symbol's address as a value, so the `+ 0x1f4` has to be real
 * arithmetic. Both functions matched immediately afterwards. This is the same
 * reading as src/rom_8a000/rom_91584_a_c_a_c_c_b.c, where the local was needed
 * because the base was used twice; here it is needed even for a single use, so
 * the rule is about the FOLD and not about reuse.
 *
 * Note that 0x1f4 is reachable as `mov` + `lsl` (0xfa << 1) and 500 is not
 * reachable by `mov` at all, which is why the folded form has to pool it. The
 * pool entry is the tell.
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

void Func_808f0d8(struct Actor *a)
{
    struct Actor *b;
    unsigned char *g;

    if (a != 0) {
        g = gState;
        b = GetFieldActor(*(int *)(g + 0x1f4));
        a->f34 = 0x80 << 9;
        a->f30 = 0x80 << 10;
        a->f55 = 0;
        _Actor_TravelTo(a, b->f8, b->fc + (0x90 << 14), b->f10);
        WaitFrames(3);
        _Actor_SetAnim(b, 0x1c);
        _Actor_SetScript(a, L9e75c);
        b->f6 = 0x80 << 7;
    }
}

void Func_808f140(struct Actor *a, int flags)
{
    struct Actor *b;
    unsigned char *g;

    if (a != 0) {
        g = gState;
        b = GetFieldActor(*(int *)(g + 0x1f4));
        if (flags & 1) {
            _Actor_SetSpriteFlags(a, 0);
            _Actor_SetScript(a, L9e6c0);
            a->f28 = 0x80 << 10;
            a->f48 = 0x80 << 7;
            a->f6c = Func_808eee4;
        }
        if (flags == 3)
            WaitFrames(0x3c);
        if (flags & 2)
            Func_808f0d8(a);
        if (flags == 3)
            WaitFrames(0x50);
        _Actor_SetAnim(b, 1);
    }
}
