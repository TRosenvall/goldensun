/* OvlFunc_969_200d6a0 -- PARK, first-candidate draft.
 * NON-MATCHING, 270 encodings of 316.  NOT a distance -- 318 encodings / 852 bytes against 316 / 848, two instructions long.
 * READ `--align`: 52 of 318, first difference at index 8.  Its 49-entry jump table materialised
 * on the first try.  Blocker: register allocation -- the ROM holds the actor in r6 and shares r7
 * between the spawn flag and the created actor, while this gets r10 for the actor so every field
 * access pays a `mov`.  Carries 6 register pins and one volatile cast (a dead-store blocker).
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7f6e64/200d6a0.c \
 *     asm/overlays/rom_7f6e64/ovl_314_c_c_a.s --func OvlFunc_969_200d6a0
 * asm/overlays/rom_7f6e64/ovl_314_c_c_a.s (grep -ci func_start = 1, datacheck:
 * no data -- the 49-word table at .L56c4 is gcc's own casesi table).
 *
 * NOT a true distance: objcmp --whole 270 of 316 differ, ours 318 encodings /
 * 852 bytes against 316 / 848 -- TWO INSTRUCTIONS AND FOUR BYTES LONG.
 * tryc --align 52 of 318.
 *
 * WHAT IS ALREADY RIGHT: the first diff is at index 8.  The 49-entry jump table
 * materialised as a table on the first try with cases 0 / 8 / 16 / 24 / 25 / 26 /
 * 27..34 / 36 / 48 in ascending source order; cases 0 and 8 share their tail
 * through a `goto` into case 8's body, which is what the ROM's
 * `b .L57a6` is; cases 25 and 26 share the `drop` tail, and the `a->f0c` load
 * made for each case's compare is reused by the shared `+= 0x90 << 10` on the
 * path that does not call __PlaySound -- exactly the ROM's r1 reuse.
 *
 * THREE FIXES WORTH RECORDING, each single-dropped (tryc --align / 318):
 *
 * 1. SIX r0/r1/r2 PINS ON THE TWO `__Func_8012330` CALLS, 189 -> 75 and the
 *    length from 315 to 318.  Same identical-argument commoning as target 3:
 *    case 0 passes 0xc0<<11 twice and case 16 passes 0x80<<9 three times, and
 *    cse2 collapses each group into one `mov` plus register copies.
 * 2. A STRUCT-OFFSET BUG, 75 -> 65.  `int f30; short f32;` puts the short at
 *    0x34, not 0x32, and shifted every later field by four -- visible as
 *    `add r0, #0x66` against the ROM's `#0x62` at six call sites.  0x32 is the
 *    UPPER HALFWORD of the word at 0x30 and both are used, on two different
 *    objects, so it cannot be a struct member: it is reached with
 *    `*(short *)((unsigned char *)a + 0x32)`.
 * 3. AN `int` CARRIER FOR THE 0xffff000 MASK, 65 -> 52.  `n->f64 = __Random() &
 *    0xffff000;` with f64 a `short` narrows the AND to HImode and gcc pools
 *    `0xf000`, not `0xffff000`.  Masking into an int first keeps it SImode.
 *    The `*(volatile int *)` cast on the `__sin(...) * 24` store is in the same
 *    class of problem and is a SHIM: the ROM stores to +0x30 twice in a row and
 *    gcc deletes the first as dead.  A non-volatile spelling of that store has
 *    not been found; it may be that the original is not a dead store at all and
 *    the second write belongs to a different object.
 *
 * THE BLOCKER IS REGISTER ALLOCATION, the corpus's largest parked class.  The
 * ROM keeps the actor in r6 and shares r7 between the "spawn" flag and the
 * created actor -- one pseudo doing both jobs, `mov r7,#0` then `mov r7,r0`.
 * The candidate puts the actor in r10, a HIGH register, so every `a->` access
 * costs a `mov rX, r10` first; that is where nearly all 52 sit.  Spelling the
 * flag and the actor as ONE variable (t2_f.c) does make them share a register
 * and improves the aligned count to 56 -- but it moves the actor to r7 and the
 * flag to r10, and objcmp gets WORSE (298 of 316, 856 bytes).  Retry after the
 * allocation order is understood, not before.
 *
 * SHIMS -- EIGHT:
 *   register class:  6  -- q0/q1/q2 at each of the two __Func_8012330 calls
 *   .equ class:      0
 *   other __asm__:   1  -- extern int L6764 __asm__(".L6764"), the asm-NAME
 *                          alias for the dot-prefixed .lcomm global, copied
 *                          from src/non_matching/ovl_7f6e64/200c23c.c
 *   volatile cast:   1  -- *(volatile int *)(n + 0x30), a DSE blocker
 */
struct Sub {
    unsigned char pad00[9];
    unsigned char f09;
    unsigned char pad0a[0x26 - 0x0a];
    unsigned char f26;
};

struct Actor {
    unsigned char pad00[8];
    int f08;
    int f0c;
    int f10;
    unsigned char pad14[4];
    int f18;
    int f1c;
    unsigned char pad20[0x30 - 0x20];
    int f30;
    unsigned char pad34[0x50 - 0x34];
    struct Sub *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[0x62 - 0x56];
    unsigned char f62;
    unsigned char pad63[1];
    short f64;
    short f66;
    unsigned char pad68[0x6c - 0x68];
    void (*f6c)(void);
};

extern int L6764 __asm__(".L6764");
extern unsigned int iwram_3001e40;
extern unsigned char gScript_969__0200e2d0[];
extern unsigned char gScript_969__0200e1cc[];

extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_SetIdle(int slot);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __Func_8012330(int x, int y, int z);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern unsigned int __Random(void);
extern struct Actor *__CreateActor(int kind, int x, int y, int z);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);
extern void __Func_80929d8(struct Actor *a, int n);
extern int __sin(int a);

extern void OvlFunc_969_200d688(struct Actor *a);
extern void OvlFunc_969_200d9f0(struct Actor *a);
extern void OvlFunc_969_200b660(void);

void OvlFunc_969_200d6a0(void)
{
    struct Actor *a;
    struct Actor *n;
    struct Sub *sb;
    int f;
    int y;
    int c;
    int w;

    a = __MapActor_GetActor(0x17);
    f = 0;
    switch (L6764) {
    case 0:
        __PlaySound(0xdc);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");

            q0 = 0xc0 << 11;
            q1 = 0xc0 << 11;
            q2 = 0x80 << 9;
            __Func_8012330(q0, q1, q2);
        }
        c = 0x2063ff;
        goto fade;
    case 8:
        c = 0x80 << 9;
    fade:
        __Func_8091200(c, 1);
        __Func_8091254(8);
        break;
    case 16:
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");

            q0 = 0x80 << 9;
            q1 = 0x80 << 9;
            q2 = 0x80 << 9;
            __Func_8012330(q0, q1, q2);
        }
        break;
    case 24:
        a->f08 = 0x98 << 17;
        a->f0c = 0xfe980000;
        a->f10 = 0xa4 << 16;
        a->f18 = 0x80 << 9;
        a->f1c = 0x80 << 9;
        OvlFunc_969_200d688(a);
        __MapActor_SetBehavior(0x17, gScript_969__0200e2d0);
        break;
    case 25:
        L6764--;
        if (a->f0c <= 0)
            goto drop;
        __Func_8091200(0x203210, 0);
        __Func_8091254(0x10);
        L6764++;
        __MapActor_GetActor(0)->f62 = 1;
        __MapActor_GetActor(1)->f62 = 1;
        __MapActor_GetActor(2)->f62 = 1;
        __MapActor_GetActor(3)->f62 = 1;
        __MapActor_GetActor(0x15)->f62 = 1;
        __MapActor_GetActor(6)->f62 = 1;
        break;
    case 26:
        L6764--;
        if (a->f0c <= (0xa0 << 14))
            goto drop;
        __Func_8091200(0x80 << 9, 0);
        __Func_8091254(0x28);
        L6764++;
        break;
    drop:
        if ((iwram_3001e40 & 7) == 0)
            __PlaySound(0xf6);
        a->f0c += 0x90 << 10;
        f = 1;
        break;
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
        f = 1;
        break;
    case 36:
        __PlaySound(0xbb);
        __Func_8091200(0x7fff, 0);
        __Func_8091254(0xc);
        break;
    case 48:
        __MapActor_SetIdle(0x17);
        __SetFlag(0x237);
        break;
    }
    if (f != 0) {
        y = a->f0c - (int)(((__Random() * 80) >> 16) << 16) + 0xfff80000;
        n = __CreateActor(0x8e << 1, a->f08, y, a->f10);
        if (n != 0) {
            sb = n->f50;
            __Actor_SetScript(n, gScript_969__0200e1cc);
            __Func_80929d8(n, 1);
            n->f55 = 0;
            w = __Random() & 0xffff000;
            n->f64 = w;
            n->f66 = 0;
            n->f62 = __Random() >> 13;
            n->f6c = OvlFunc_969_200b660;
            *(volatile int *)((unsigned char *)n + 0x30) =
                __sin((__Random() * 0xffff) >> 20) * 24;
            n->f30 = *(short *)((unsigned char *)a + 0x32);
            sb->f26 = 0;
            sb->f09 = (sb->f09 & ~0xc) | 4;
        }
    }
    L6764++;
    OvlFunc_969_200d9f0(__MapActor_GetActor(0));
    OvlFunc_969_200d9f0(__MapActor_GetActor(1));
    OvlFunc_969_200d9f0(__MapActor_GetActor(2));
    OvlFunc_969_200d9f0(__MapActor_GetActor(3));
    OvlFunc_969_200d9f0(__MapActor_GetActor(0x15));
    OvlFunc_969_200d9f0(__MapActor_GetActor(6));
}
