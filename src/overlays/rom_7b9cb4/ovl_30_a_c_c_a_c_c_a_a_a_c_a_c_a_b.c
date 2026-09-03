/* OvlFunc_932_200a804  --  0x0200a804
 *
 * The tail of goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_c_a_c_a.s;
 * the function ahead of it stays in _a_a.s. The file's only .word is this
 * function's own mid-function pool and travels with it.
 *
 * BUILT WITH CSE_CFLAGS. The flag id 0x908 is read twice with the first use
 * dominating the second, which is the guard/set shape the flag exists for.
 * 34 aligned regions without it.
 *
 * A POOLED SMALL CONSTANT REACHED BY strb CAN BE AN HImode TEMPORARY, NOT A
 * SYMBOL. This corrects the recorded note that byte stores have no QImode
 * analogue of the halfword exception, so a pooled zero reaching a `strb` must
 * be a linker symbol. The ROM has `ldr r5, .Ln / .word 0 / add r0, #0x55 /
 * strb r5, [r0]`, and spelling that zero as a const.sym symbol is WORSE -- 21
 * against 8 -- because an SImode symbol's pool range is 1020, which lets the
 * pool sink to the end barrier and the ROM's mid-function pool disappears.
 * What reproduces it is an UNPROMOTED HImode lvalue: a one-field struct or a
 * one-element array of `unsigned short`, stored and then read back. A plain
 * `short` local does NOT work, because PROMOTE_MODE forces every sub-word
 * integer local to SImode -- which is exactly why the struct field escapes and
 * the local does not.
 *
 * THE POOL DISTANCE IS A READOUT OF THE OPERAND'S MODE, and it identifies the
 * class before any spelling is tried. The halfword move's constant alternative
 * carries a pool range of 64; the byte move has no constant alternative at all;
 * a word move's is 1020. Measured from the ROM's own labels, this load sits 56
 * bytes from its pool -- inside 64, far outside the byte range, and nowhere
 * near 1020. So a mid-function pool at a short distance says HImode, and the
 * error in the old note was concluding the VALUE must be QImode because the
 * STORE is `strb`.
 *
 * Two confirmed levers carried the rest. The stack-argument pair is worth 6
 * regions. Naming the pointer to move a load's base and offset is worth the
 * last 6: with the pointer written inline, sched2 hoists the constant build
 * into the slot the ROM uses for the pointer load, and naming it -- loaded
 * BEFORE the neighbouring store -- restores the ROM's order.
 *
 * The `int` intermediate for each halfword constant is required; written inline
 * the shift still pools, so the recorded "unsigned short destination with the
 * shift inline" note does not hold for this shape.
 */
struct HalfWord { unsigned short v; };

extern unsigned char gState[];

extern int __GetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern void __WaitFrames(int n);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80933f8(int a, int b, int c, int d);

void OvlFunc_932_200a804(void)
{
    unsigned char *a;
    unsigned char *g;
    unsigned int off;
    short v;
    int h;
    int e5, e6;
    struct HalfWord z;
    unsigned char *q;

    a = __MapActor_GetActor(10);
    __MapActor_SetPos(8, 0, 0);
    __MapActor_SetPos(9, 0, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
    a[0x55] = 0;
    *(int *)(a + 0x18) = 0xe666;
    q = *(unsigned char **)(a + 0x50);
    *(int *)(a + 0x1c) = 0x9999;
    h = 0x80 << 8;
    *(unsigned short *)(q + 0x1e) = h;
    z.v = 0;
    __MapActor_GetActor(12)[0x55] = z.v;
    *(int *)(__MapActor_GetActor(12) + 0xc) = 0xffe40000;
    if (__GetFlag(0x908)) {
        *(int *)(a + 8) += 0xe0 << 12;
        *(int *)(a + 0xc) += 0xfff80000;
        q = *(unsigned char **)(a + 0x50);
        h = 0xc0 << 8;
        *(unsigned short *)(q + 0x1e) = h;
    }
    if (__GetFlag(0x908)) {
        e5 = 0xb;
        e6 = 9;
        __CopyMapTiles(0x19, 0x24, 0x2b, 0x24, e5, e6);
        e5 = 0x2b;
        e6 = 0x23;
        __Func_8010704(0x19, 0x23, 0xa, 5, e5, e6);
        __Func_800fe9c();
        __WaitFrames(1);
    }
    off = 0xe1;
    off <<= 1;
    g = gState + off;
    off = 0;
    v = *(short *)(g + off);
    if (v == 6 && !__GetFlag(0x109)) {
        __CutsceneStart();
        *(int *)(__MapActor_GetActor(0) + 0xc) = 0xffa80000;
        __Func_80933f8(0xc6 << 18, 0xffa80000, 0x2410000, 0);
        __Func_800fe9c();
        __WaitFrames(1);
        __CutsceneEnd();
    }
}
