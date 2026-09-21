/* OvlFunc_969_200a200 (0x0200a200) -- NON-MATCHING, 90 of 138, with -fno-gcse.
 * Blocker class: TWO EXTRA ALLOCNOS -- a second frame base and a hoisted zero.
 *
 * asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_a_a_c_c_c.s (1 function, so landing needs NO split).
 * Best candidate below, built with -fno-gcse (the existing GCSE_CFLAGS group).
 *
 * THE BLOCKER IS REGISTER PRESSURE, MEASURED NOT GUESSED. `.17.lreg` shows EIGHT allocnos
 * crossing calls against the ROM's SEVEN, for seven callee-saved registers:
 *
 *     Register  32  13 refs /  98 insns; crosses 4 calls; pointer   <- src           (ROM r6)
 *     Register  35  32 refs /  49 insns; crosses 2 calls; pointer   <- s             (ROM r5)
 *     Register  36   3 refs /  73 insns; crosses 4 calls; pref STACK_REG <- array base (ROM r10)
 *     Register  38   9 refs / 140 insns; crosses 3 calls            <- i             (ROM r7)
 *     Register  39   5 refs / 134 insns; crosses 3 calls            <- SImode zero   EXTRA
 *     Register  40   5 refs / 140 insns; crosses 3 calls            <- m = 0x3f      (ROM r9)
 *     Register  59   6 refs /  92 insns; crosses 2 calls; 2 bytes   <- HImode zero   (ROM r8)
 *     Register 142   4 refs /  84 insns; crosses 3 calls; pointer   <- 2nd frame base EXTRA
 *
 * The two extras are a SECOND frame-base pseudo,
 * `(insn 365 (set (reg:SI 142) (plus (reg:SI 25 sfp) (const_int -8))))`, and an SImode zero
 * LICM-HOISTED out of the loop to block 0, `(insn 367 (set (reg/v:SI 39) (const_int 0)))`.
 * The consequence is that `i` is forced into r8, so every index costs
 * `mov r1, r8 / lsl r3, r1, #2` where the ROM has `lsl r3, r7, #2`, and the frame grows to
 * `sub sp, #0x10` against the ROM's #0x8.
 *
 * SO THIS IS THE FRAME-SIZE CASE, NOT THE PRIORITY CASE -- the batch-278 triage rule. The
 * formula does not apply; the target is REMOVING one of the two extra allocnos, not permuting
 * the ones that remain. The hoisted SImode zero is the more tractable: every naming spelling
 * was inert because cse1 merges the zeros before LICM sees them.
 *
 * CONFIRMED ALONG THE WAY, so it is not mistaken for a defect: the ROM's odd
 * `ldr r1, =0x0 / mov r8, r1` IS genuine gcc -- *thumb_movhi_insn pools a HImode zero, and this
 * build reproduces it.
 *
 * MEASURED (plain / -fno-strength-reduce / -fno-gcse):
 *     unsigned char * with offsets, one named zero      112 / 101 / 93
 *     masks named before the loop                       116 /  98 /  -
 *     masks named inside the `s != 0` block             110 / 107 / 99
 *     two named int zeros                               110 / 107 /  -
 *     zero assigned inside `s != 0`, literals elsewhere 116 /  98 / 115
 *     do/while instead of for                           110 / 107 /  -
 *     `i` pinned to r7                                  120 / 120 /  -
 *     `i`->r7 and `m`->r9 pinned                        117 / 117 /  -
 *     STRUCT-WITH-PADDING rewrite (park idiom)          106 /   - /  90  <- best
 *     -fno-schedule-insns2 on the first                   - /   - / 115
 * PINNING MAKES IT WORSE -- the pins fight the spill rather than remove it.
 * -fno-strength-reduce does remove the `stmia r1!, {r2}` giv and restore the ROM's
 * `str r0, [r3, r2]`, but is inert once -fno-gcse is on.
 *
 * THE IDIOM SOURCE WAS A PARK, NOT THE STEM NEIGHBOUR: src/non_matching/ovl_7f6e64/200b600.c's
 * struct-with-explicit-padding form was worth 6 differing when nothing else had moved in five
 * rounds. The same-stem neighbour (12 shared components) is twelve lines of sine arithmetic with
 * no structural overlap and gave only the identity of the two +0x6c hooks.
 *
 * NEXT: remove the hoisted SImode zero. Not spelling.
 */
struct SpriteSlot { unsigned short size; unsigned short vramOffset; };
extern struct SpriteSlot gSpriteSlots[];

struct S {
    unsigned char pad00[5];
    unsigned char f05;
    unsigned char pad06;
    unsigned char f07;
    unsigned short f08;
    unsigned char pad0a[0x1c - 0x0a];
    unsigned char f1c;
    unsigned char f1d;
    unsigned char pad1e[0x26 - 0x1e];
    unsigned char f26;
    unsigned char pad27;
    unsigned char *f28;
};

struct A {
    unsigned char pad00[8];
    int f08;
    int f0c;
    int f10;
    int f14;
    unsigned char pad18[0x50 - 0x18];
    struct S *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    unsigned short f64;
    unsigned char pad66[2];
    struct A *f68;
    void (*f6c)(struct A *);
};

extern unsigned char *iwram_3001f30;
extern struct A *__CreateActor(int a, int b, int c, int d);
extern void __Sprite_SetAnim(struct S *s, int anim);
extern void __PlaySound(int id);
extern void __Func_8003f3c(int a);
extern void OvlFunc_969_200a1ac(struct A *a);
extern void OvlFunc_969_200a15c(struct A *a);

void OvlFunc_969_200a200(struct A *src)
{
    struct A *v[2];
    struct A *p;
    struct S *s;
    struct S *b;
    unsigned char *g;
    int i;
    int zero;
    int m;
    int m2;
    int mk;
    int m1;

    g = iwram_3001f30;
    __PlaySound(0x83);
    m = 0x3f;
    for (i = 0; i <= 1; i++) {
        p = __CreateActor(0x1a, src->f08, src->f0c, src->f10);
        v[i] = p;
        if (p != 0) {
            p->f14 = src->f14;
            s = p->f50;
            zero = 0;
            p->f55 = zero;
            p->f64 = zero;
            p->f68 = src;
            if (s != 0) {
                __Sprite_SetAnim(s, 0);
                s->f26 = zero;
                __Func_8003f3c(s->f1c);
                s->f1c = *(unsigned short *)(g + 0x46);
                s->f1d |= 1;
                mk = ~0x3ff;
                s->f08 = (s->f08 & mk)
                       | ((gSpriteSlots[s->f1c].vramOffset << 17) >> 22);
                m1 = ~0x20;
                s->f05 = ((s->f05 & m1) & m) | 0x40;
                s->f07 = (s->f07 & m) | 0x80;
                s->f28[0x16] = zero;
            }
        }
    }
    m2 = ~0xc;
    p = v[0];
    p->f6c = OvlFunc_969_200a1ac;
    b = p->f50;
    b->f28[-0x1f] = (b->f28[-0x1f] & m2) | (src->f50->f28[-0x1f] & 0xc);
    p = v[1];
    b = p->f50;
    p->f6c = OvlFunc_969_200a15c;
    b->f28[-0x1f] = (b->f28[-0x1f] & m2) | (src->f50->f28[-0x1f] & 0xc);
    ((unsigned char *)p)[0x23] = 2;
}
