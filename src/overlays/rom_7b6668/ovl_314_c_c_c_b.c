/* OvlFunc_928_2009148  --  0x02009148, cut from
 * goldensun/asm/overlays/rom_7b6668/ovl_314_c_c_c.s.
 *
 * EXACT.  Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this file> \
 *     asm/overlays/rom_7b6668/ovl_314_c_c_c.s --func OvlFunc_928_2009148
 * 488 bytes, 202 encodings, 34 relocations identical.  No shims, no pins.
 *
 * NEEDS A TEXT/DATA SPLIT.  The reference carries a `.data` section of nine
 * `.incbin` blobs; `datacheck` reports the function reads no data label, so the
 * split needs NO new export -- the data keeps its own object as it stands.
 *
 * THE "DEAD CALLEE-SAVED REGISTER" READING OF THIS FUNCTION WAS WRONG, and that
 * is the most reusable thing here.  Batch 298 read `r8` as written once
 * (`mov r8, r2` with r2 = 0) and never read, and filed the function under the
 * class where the ROM spends four prologue/epilogue instructions we cannot
 * emit.  gcc emits all five by itself: the zero is one of THREE zero pseudos
 * gcc's constant CSE creates here (r7 from a `mov`, r8 from the same `mov`, and
 * a third that is spilled to a `.word 0` in the function's own pool and loaded
 * back with `ldr`), and it only needs `r8` at this exact register pressure.  The
 * evidence is a probe in the wrong direction: retyping the loop actor from
 * `struct Actor *` to `unsigned char *` shortens the body by six instructions
 * AND drops the `r8` push, so the first differing encoding moves to index 1.
 * **A register written from a CSEd constant and never read is not the dead-
 * register class** -- count the constant's other uses before filing it.
 *
 * FOUR LEVERS, all already in the notebook:
 *
 * 1. THE gState BASE IS A LOCAL.  `*(short *)(gState + 0x1c2)` folds to a pool
 *    word `=gState+450`; assigning `g = gState` first keeps the ROM's
 *    `mov r2, #0xe1 / lsl r2, #1 / add r3, r2`.  Six instructions.
 *
 * 2. A 32-BIT `mov` + `neg` MEANS A BITFIELD.  The ROM masks byte 9 of the
 *    sprite with `mov r0, #0xd / neg r0, r0` -- 0xfffffff3, which is ~0xc, not
 *    ~0xd.  Written as explicit arithmetic gcc narrows the mask to a byte
 *    (`mov r3, #0xf2`) and the pair disappears; a real 2-bit bitfield at bit 2
 *    reproduces the SImode mask with the QImode `ldrb`/`strb` around it.
 *
 * 3. ONE VARIABLE PER REGION.  Reusing the loop's actor local for the actors
 *    fetched after the loop puts the later one in a callee-saved register and
 *    costs a copy; a separate local leaves it in r0, where the ROM has it.
 *
 * 4. A DOMINATING-BLOCK LOCAL FOR EACH REPEATED CONSTANT, which is what took
 *    this from 8 of 202 to exact.  Three argument setups had the ROM's cheap
 *    `mov r0, #K` landing INSIDE the `mov` + `lsl` of a later argument, which
 *    the recorded table calls unreachable for a literal.  Naming `0x80 << 8`
 *    and the two `0x92 << 16` / `0x9c << 17` position constants in the block
 *    that dominates both call sites drops all three: a rematerialised pseudo
 *    has low `rtx_cost` and falls out of `precompute_register_parameters`.
 *    Measured separately: naming only the first is 6 differing, only the
 *    position pair is 2, both is exact.  The pooled `.word 0` fixed ITSELF at
 *    the same time -- our version had it as a HImode `ldrh`, and the changed
 *    allocation made it the ROM's SImode `ldr`.  It was never a blocker.
 */
struct Sprite {
    unsigned char pad00[9];
    unsigned char f9_0 : 2;
    unsigned char f9_2 : 2;
    unsigned char f9_4 : 4;
    unsigned char pad0a[0x14];
    unsigned short f1e;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[4];
    int f18;
    int f1c;
    unsigned char pad20[3];
    unsigned char f23;
    unsigned char pad24[0x18];
    int f3c;
    unsigned char pad40[0x10];
    struct Sprite *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[3];
    unsigned char f59;
    unsigned char pad5a[0x12];
    int f6c;
};

extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];

extern void __Func_8091ff0(int a);
extern void __ClearFlag(int id);
extern int __GetFlag(int id);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_SetPos(int slot, int x, int z);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Func_80929d8(struct Actor *a, int n);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __StartTask(void (*fn)(void), int n);
extern void __Func_8092950(int a, int b);
extern void OvlFunc_928_2008324(void);
extern void OvlFunc_928_2008500(void);

int OvlFunc_928_2009148(void)
{
    unsigned int i;
    struct Actor *a;
    struct Sprite *s;
    unsigned char *g;
    struct Actor *b;
    int y, z;
    int k;
    int px, pz;

    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x80 << 1;
    __Func_8091ff0(0xa9);
    g = gState;
    if (*(short *)(g + (0xe1 << 1)) > 9)
        __ClearFlag(0x12f);
    k = 0x80 << 8;
    px = 0x92 << 16;
    pz = 0x9c << 17;
    if (__GetFlag(0x895)) {
        __Func_8092adc(0xd, k, 0);
        __MapActor_SetPos(0xe, px, pz);
        __Func_8092adc(0xe, 0, 0);
        if (__GetFlag(0x89a))
            __MapActor_SetPos(0x11, 0, 0);
    }
    if (__GetFlag(0x8b << 4))
        __MapActor_SetPos(0x11, 0, 0);
    i = 0;
    do {
        a = __MapActor_GetActor(i + 0x17);
        s = a->f50;
        s->f9_2 = 1;
        a->f55 = 0;
        a->f59 = 8;
        __Actor_SetSpriteFlags(a, 0);
        __Func_80929d8(a, 0xf);
        a->f23 = (a->f23 & 0xfe) | 2;
        i++;
    } while (i <= 2);
    if (__GetFlag(0x202)) {
        __MapActor_SetPos(0xe, px, pz);
        __Func_8092adc(0xe, 0, 0);
    }
    if (__GetFlag(0x201)) {
        __MapActor_SetAnim(0x14, 5);
        y = __MapActor_GetActor(0x14)->f8;
        z = __MapActor_GetActor(0x14)->f10;
        __Func_8010704(3, 0x11, 1, 1, y >> 20, z >> 20);
        __StartTask(OvlFunc_928_2008324, 0xc8 << 4);
    }
    __Func_8092950(0x12, 2);
    __MapActor_GetActor(0x12)->f6c = (int)OvlFunc_928_2008500;
    b = __MapActor_GetActor(0x13);
    b->f55 = 0;
    b->fc = 0x80 << 13;
    b->f3c = 0x80 << 13;
    b->f18 = 0x8ccc;
    b->f1c = 0x6666;
    b->f50->f1e = 0x80 << 8;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x15), 0);
    __MapActor_GetActor(0x15)->f55 = 0;
    __MapActor_GetActor(0x15)->fc = 0;
    __MapActor_GetActor(0x15)->f3c = 0x80 << 24;
    return 0;
}
