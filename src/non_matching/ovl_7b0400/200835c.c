/* OvlFunc_925_200835c -- NON-MATCHING, 143 of 206 encodings differ.
 * Unattempted before batch 298.  Reference
 * asm/overlays/rom_7b0400/ovl_314_c_c_c_a_a_a.s -- ONE function, so this converts
 * WHOLE with no split at all, and datacheck is silent (no data section).
 * SHIMS: ZERO.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_7b0400/200835c.c \
 *       asm/overlays/rom_7b0400/ovl_314_c_c_c_a_a_a.s --func OvlFunc_925_200835c
 *
 * 143 IS SATURATED, NOT A DISTANCE: size 524 against 528 and count 204 against 206.
 * The real view is tools/aligncmp.py -- 181 of 380 aligned-equal, 87.9%, 26
 * differing in 18 hunks.  The very first naive draft was already 206/206 with size
 * exact at 108 differing, i.e. a true distance on the first screen; two spellings
 * then removed both real gaps, and what is left is ONE INSTRUCTION.
 *
 * THE BLOCKER IS A DEAD CALLEE-SAVED REGISTER, and it is the class now documented
 * in docs/elevation.md.  The reference mentions r6 exactly three times:
 * `push {r5, r6, lr}`, `mov r6, #0`, `pop {r5, r6}`.  IT IS WRITTEN ONCE AND NEVER
 * READ.  gcc-2.96 will not emit that -- constant propagation finds the zero and
 * deletes the def in all EIGHT source shapes measured (a returned variable; a "+r"
 * barrier after the loop; the zero consumed at one site; consumed at all nine
 * post-loop zero sites; a two-biv `for (n=0xe, i=0; ...)`; `for (i=0;i<6;i++)` with
 * 0xe+i, which keeps i as the biv and costs 2; and a hard-register declaration at
 * both block and function scope).  Every one gives 204 encodings and
 * `push {r5, lr}`.
 *
 * Note the cost is NOT one instruction: the dead mov owns the push/pop mask, which
 * shifts the literal pool by 4 bytes and turns nine `ldr rX,[pc,#N]` into
 * differences.  Discount those and the only real hunks left are the iwram-store
 * r1/r2 transposition (5) and one `str r5,[r0,#0x18]` / ldrsh-scratch swap (3).
 *
 * WHAT CLOSED THE REST: the iwram_3001e70 store needed a NAMED OFFSET LOCAL so 0xc
 * stays a `str` displacement (`mov r2,#0xb2` + `str r2,[r3,#0xc]`, not the folded
 * `mov r2,#0xb8` + `str r2,[r3]`); and __CopyMapTiles's 3rd and 6th arguments
 * needed ONE SHARED named local so gcc reuses `mov r2,#4` for both the argument and
 * the stack slot -- that block is now byte-identical.  The compared 0x3a is
 * _AREA_3a (pooled though it fits imm8), and its extra relocation is the same
 * benign absolute-symbol class.
 *
 * A NEAR-MISS DELIBERATELY NOT SHIPPED, kept as scratch_elev/b298b/c925N.c:
 * `for (i = 6, n = 0xe; i != 0; i--, n++)` gets `push {r5,r6,lr}` and reads
 * 206/206 with size exact and 30 differing -- the best number on this function, and
 * a true distance.  It is knowingly WRONG: it puts the loop control on i
 * (`sub r6,#1 / cmp r6,#0 / bne`) where the ROM has `cmp r5,#0x13 / bls`, and lets
 * the exit value 0 feed `strb r6,[r3]` where the ROM rematerialises `mov r3,#0`.
 * Recorded because a better figure from a wrong structure is exactly the trap this
 * batch also hit with an odd-offset union.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern char *iwram_3001e70;
extern int _AREA_3a;
extern int __SetFlag(int id);
extern int __GetFlag(int id);
extern int __StartTask(void (*fn)(void), int prio);
extern void __Func_8092b08(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern void __WaitFrames(int n);
extern int OvlFunc_925_20088cc(void);
extern void OvlFunc_925_200b4bc(void);
extern void OvlFunc_925_200856c(void);
extern void OvlFunc_925_2009af0(void);

int OvlFunc_925_200835c(void)
{
    unsigned int off;
    unsigned int o2;
    unsigned int n;
    unsigned char *a;
    int t;
    int v;
    int c4;

    __SetFlag(0x111);
    off = 0xe0;
    off <<= 1;
    *(int *)(iwram_3001ebc + off) = 0x81 << 2;
    if (*(short *)((char *)&gState + off) == (int)(&_AREA_3a)) {
        __SetFlag(0xa2 << 1);
        __StartTask(OvlFunc_925_200b4bc, 0xc8 << 4);
        __Func_8092b08(0, 1);
        __Func_8092b08(1, 1);
        __Func_8092b08(2, 1);
        __Func_8092b08(3, 1);
        __Func_8092b08(5, 1);
        __Func_8092b08(0x14, 1);
        __Func_8092b08(0x15, 1);
        __Func_8092b08(0x16, 1);
        __Func_8092b08(0x17, 1);
        __Func_8092b08(0x18, 1);
        __Func_8092b08(8, 1);
        __Func_8092b08(9, 1);
        __Func_8092b08(0xa, 1);
        __Func_8092b08(0xb, 1);
        __Func_8092b08(0xc, 1);
        __Func_8092b08(0xd, 1);
        for (n = 0xe; n <= 0x13; n++) {
            __Func_8092b08(n, 1);
            __MapActor_GetActor(n)[0x55] = 4;
            __MapActor_GetActor(n)[0x23] |= 2;
            *(int *)(__MapActor_GetActor(n) + 0xc) = 0xffcd8000;
        }
        if (__GetFlag(0x109) != 0) {
            t = OvlFunc_925_20088cc();
            if (t != 0) {
                a = __MapActor_GetActor(t);
                if (a != 0)
                    a[0x55] = 0;
            }
        }
        __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xb), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xc), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xd), 0);
        *(int *)(__MapActor_GetActor(0xc) + 0x18) = 0xffff0000;
        *(int *)(__MapActor_GetActor(0xd) + 0x18) = 0xffff0000;
        off = 0xe1;
        off <<= 1;
        v = *(short *)((char *)&gState + off);
        if (v == 1) {
            if (__GetFlag(0x109) == 0)
                OvlFunc_925_200856c();
        } else if (v == 2) {
            if (__GetFlag(0x251) == 0) {
                o2 = 0xb2;
                o2 <<= 1;
                *(int *)(iwram_3001e70 + o2 + 0xc) = 0x80 << 19;
                __Func_800fe9c();
                __WaitFrames(1);
                c4 = 4;
                __CopyMapTiles(4, 0x46, c4, 0x4a, 5, c4);
                __MapActor_SetPos(9, 0, 0);
                if (__GetFlag(0x109) == 0)
                    OvlFunc_925_2009af0();
            }
        } else if (v == 5) {
            __SetFlag(0x251);
        }
    }
    return 0;
}
