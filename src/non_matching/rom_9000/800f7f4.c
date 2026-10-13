/* ActorCmd_Player_Climb (0x0800f7f4) -- NON-MATCHING, 168 encodings of 222 differ.
 *
 * ours 220 encodings / 206 16-bit instructions against ref 222 / 207, pool
 * 14/14, 468 bytes against 472 -- ONE INSTRUCTION SHORT (it was two), so the
 * 168 is NOT a distance.  Honest figures: tryc --align 85 of 229 (was 89);
 * aligncmp raw 123 (was 127).  0 pins, 0 devices (shimcount).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_9000/800f7f4.c \
 *     asm/rom_9000/rom_ebec_c.s --func ActorCmd_Player_Climb
 * Rank variants on:
 *   ... tools/aligncmp.py src/non_matching/rom_9000/800f7f4.c \
 *       asm/rom_9000/rom_ebec_c.s ActorCmd_Player_Climb
 * AND on tryc --align.  THEY DISAGREE HERE AND aligncmp IS THE ONE TO TRUST --
 * see the gBuffer entry below: tryc's DESTRUCTIVE normaliser folds
 * `add r0, r0, r3` into `add r0, r3`, so it is STRUCTURALLY BLIND to exactly the
 * destructive-vs-three-operand difference this function turns on.
 *
 * SPLIT SHAPE (datacheck.py + split_s.py --dry-run, both run this round):
 *   asm/rom_9000/rom_ebec_c.s
 *       data sections : .rodata
 *       functions     : ActorCmd_Player_Climb
 *       EXPORTS       : .L13254, .L13274, .L13280, .L1328c  (ALREADY global)
 *       ActorCmd_Player_Climb reads .L13254 -> NO NEW EXPORT
 *   split_s.py asm/rom_9000/rom_ebec_c.s ActorCmd_Player_Climb --dry-run:
 *       would write rom_ebec_c_b.s  (the function, 243 lines)
 *       would write rom_ebec_c_c.s  (the .rodata + 4 labels, 15 lines)
 *       would REMOVE rom_ebec_c.s, rewrite stage1.ld
 *   The four labels are referenced from asm/rom_9000/rom_ebec_a.s and from the
 *   parks src/non_matching/rom_9000/800ebec.c, 800f2f8.c and this file, so the
 *   data MUST keep its own object.  `exports` in the manifest is [] because they
 *   are already `.global`; this is the CODE/DATA split, and a gate went red on
 *   exactly this class in batch 345.
 *
 * ============ BATCH 348 (brief D): THE GAP WAS TWO SITES OF 1 ===============
 * d_regions.py on the previous body, every other hunk netting zero:
 *
 *   A. ref[100:102] 2, ours 1:   `adds r3, r3, r1` + `ldr r1, [pc, #248]`
 *      the ROM's SECOND load of gBuffer's address.  We load it once and CSE.  -1
 *   B. ref[194:200] 5, ours 4:   `add r3, sp, #68` before the three
 *      `ldr r1/r2/r3, [r3, #0/4/8]` that build the Actor_TravelTo arguments.
 *      We used the live r5, which already holds sp+68.                       -1
 *
 * SITE B IS NOW CLOSED, and this body is the closure: the ROM REMATERIALISES
 * &s.v at its LAST use.  Writing that ONE use through a block-local
 *     {vec3_t *q = &s.v; Actor_TravelTo(a, q->x, q->y, q->z);}
 * brings `add r3, sp, #68` back -- 205 -> 206 instructions, aligncmp 127 -> 123,
 * tryc --align 89 -> 85, pool and size untouched.  The bare member form
 * `Actor_TravelTo(a, s.v.x, s.v.y, s.v.z)` also reaches 206 but only aligncmp
 * 127 / tryc 89, so the block-local `q` is strictly better.  (The old header
 * recorded "addressing `s.v.x` directly costs 2 instructions (224 vs 222)" --
 * that was measured at EVERY site; ONE site is a different experiment, and it
 * pays.)
 *
 * ---- SITE A: THE OLD HEADER'S RECORDED NEGATIVE WAS SCORED ON THE WRONG
 *      METRIC, AND IS THE BEST aligncmp STATE MEASURED THIS ROUND ----
 * The old header says: "`p0 = gBuffer; p0 += off;` to force the destructive form
 * made it WORSE (214 of 218)".  214 is objcmp's RAW ndiff, which saturates as
 * soon as the stream shifts.  Measured on the ALIGNED residue instead:
 *
 *   body                                   insns  pool  size  aligncmp  tryc
 *   this body (q, gBuffer as-is)            206    14    468   123       85
 *   q + `p0 = gBuffer; p0 += (ix+row)<<2;`  205    14    468    91       90
 *   member + same                           205    14    468    94       93
 *   p (old travel) + same                   204    14    464   102       93
 *   q + the same on p1 instead              206    14    468   123       85  (inert)
 *   q + the same on BOTH p0 and p1          205    14    468    91       90  (= p0 alone)
 *
 * So the destructive form is worth 32 aligned encodings and costs ONE
 * instruction, and tryc cannot see the gain because its DESTRUCTIVE regex folds
 * the two `add` forms together.  It is NOT shipped only because it reopens the
 * instruction gap to -2 and scatters the residue from two clean sites into ~10
 * +/-1 hunks; it is the first thing to re-try once site A's second gBuffer load
 * is understood, and the two may well be the same question.  Note `p1`-side is
 * INERT -- the lever is on p0 only.
 *
 * Site A still open.  The ROM's two adds are DIFFERENT FORMS (`add r0, r3`
 * destructive, then `add r7, r3, r1` three-operand), so the ROM holds TWO
 * separate gBuffer pseudos, the first dying at its add.  Brief lever 1 does NOT
 * apply: a constant-pool MEM is RTX_UNCHANGING_P, so a `bl` between the uses
 * cannot drop it (cse.c:2260, :6068-6080).  Reload rematerialising an
 * unallocated REG_EQUIV symbol per use is the remaining mechanism to aim at.
 *
 * ================== WHAT WAS SOLVED (keep all of it) ==================
 * 1. THE FRAME.  The ROM has `sub sp, #0x50` with its ONLY stack object at
 *    sp+0x44 -- 68 bytes nothing in the function touches.  The whole rom_ebec
 *    family shares the number: ActorCmd_Player 0x68 frame with vec3s at
 *    0x44/0x50/0x5c, ActorCmd_Player_World 0x5c with 0x44/0x50, Climb 0x50 with
 *    0x44 -- 68 + 12*N exactly.  So the vec3s are the LAST members of one local
 *    aggregate whose first 68 bytes this handler never reads.  Spelled
 *      struct { unsigned char pad[68]; vec3_t v; } s;  vec3_t *p;  p = &s.v;
 *    gcc emits `sub sp, #80` and `add r5, sp, #68` -- the ROM's two encodings at
 *    no instruction cost.  A bare `int pad[17]` is DELETED unless stored to, and
 *    storing to it costs 3; the aggregate is what makes the slot survive free.
 *    `p` BORN at the zero stores is right; born at the top of the function, 188.
 *    A corpus sweep found Climb is the ONLY function whose lowest stack offset
 *    is 0x44, so this is not a compile-unit artifact.
 * 2. `extern volatile unsigned int gKeyHeld;` -- the ROM reads gKeyHeld THREE
 *    times and keeps only the ADDRESS in r1 across the 0x40/0x80 branch
 *    (`ldr r1,=gKeyHeld ... ldr r3,[r1]` in BOTH arms).  gcse commons the VALUE
 *    without volatile.  This is a READING, not a pin: three reads in the ROM,
 *    three in the source, so no fakematch row is needed.  `-fno-gcse` reaches
 *    the same re-read and is the per-TU alternative if volatile is judged wrong.
 * 3. `short h; h = L13254[k];` -- the ROM's `ldrsh` says the DESTINATION is a
 *    short.  With an `int` h, combine folds the load and the zero-extension into
 *    one `ldrh` and the ROM's `lsl #16 / lsr #16` pair vanishes (216 insns, 460
 *    bytes).  `(unsigned short)h` at each use keeps both `lsr`s.
 * 4. Both halfword-facing literals need INT CARRIERS in their own blocks
 *    (`{ int f = 0xc0 << 8; *(unsigned short *)(a+6) = f; }`): as bare literals
 *    through the `unsigned short *` they POOL (`ldr r3,=0xc000`) where the ROM
 *    builds `mov #0xc0 / lsl #8`.  The recorded halfword-literal rule.
 * 5. `.L13254` is reached with the asm-label extension,
 *    `extern short L13254[] __asm__(".L13254");` -- 5 digits, so the
 *    low-numbered-label caveat does not apply.  `p[i]` on the short array gives
 *    the ROM's register-offset `ldrsh r3, [r1, r3]`.
 * 6. r4 carries 0x8000 from the +0x30 store to the `cmp r2,r4` at 0xf846 by
 *    plain constant CSE (no call between), while 0x4000 is rebuilt after
 *    vec3_translate.  Both are plain literals; the dominance rule does the rest.
 *    The `anim = 0xe / 0xf / 0xa` chain comes out as the ROM's three separate
 *    `mov r9` pairs with ordinary nested ifs -- the apparent cross-jump in the
 *    aligned diff is difflib, not codegen.
 *
 * Also still open, and count-neutral: branch polarity at the 0x40 arm (ROM
 * `cmp r3,r0 / bge tail / b set1`, ours `blt set1 / b tail`) and
 * `sub r3, r0, r3` three-operand against our destructive `sub r0, r3` at both
 * Func_8011f54 sites -- which is the SAME destructive/three-operand axis as
 * site A, and tryc is blind to all of it.
 */
#include "gba/types.h"

extern volatile unsigned int gKeyHeld;
extern unsigned char gBuffer[];
extern unsigned char *iwram_3001ebc;
extern short L13254[] __asm__(".L13254");
extern void vec3_translate(int mag, int angle, vec3_t *v);
extern int Func_801219c(vec3_t *v);
extern int Func_8011f54(int layer, int x, int z);
extern void Actor_SetAnim(void *a, int anim);
extern void Actor_TravelTo(void *a, int x, int y, int z);

int ActorCmd_Player_Climb(unsigned char *r0)
{
    unsigned char *a;
    unsigned char *p0;
    unsigned char *p1;
    unsigned char *w;
    int k;
    short h;
    int hs;
    unsigned int u;
    int q;
    int t;
    int anim;
    int flag;
    int ix;
    int iz;
    int row;
    int nx;
    int d;
    struct { unsigned char pad[68]; vec3_t v; } s;
    vec3_t *p;

    a = r0;
    *(int *)(a + 0x34) = 0x80 << 7;
    *(int *)(a + 0x30) = 0x80 << 8;
    k = (gKeyHeld >> 4) & 0xf;
    h = L13254[k];
    anim = 0xc;
    flag = 4;
    u = (unsigned short)h;
    if (u == 0xffff)
        goto tail;
    q = u & (0xf0 << 8);
    anim = 0xe;
    if (q == 0)
        goto haveanim;
    anim = 0xf;
    if (q == 0x80 << 8)
        goto haveanim;
    anim = 0xa;
haveanim:
    flag = 0;
    p = &s.v;
    p->x = 0;
    p->y = 0;
    p->z = 0;
    vec3_translate(0x80 << 12, (unsigned short)h, p);
    p->x = p->x + *(int *)(a + 8);
    if (p->z < 0) {
        int f = 0xc0 << 8;
        *(unsigned short *)(a + 6) = f;
    }
    if (p->z > 0) {
        int f = 0x80 << 7;
        *(unsigned short *)(a + 6) = f;
    }
    p->y = *(int *)(a + 0xc) - p->z;
    p->z = *(int *)(a + 0x10);
    ix = *(int *)(a + 8) / 0x100000;
    iz = *(int *)(a + 0x10) / 0x100000;
    row = iz << 7;
    p0 = gBuffer + ((ix + row) << 2);
    nx = p->x / 0x100000;
    p1 = gBuffer + ((nx + row) << 2);
    if (Func_801219c(p) != 0 || p0[2] != p1[2]) {
        flag = 4;
        anim = 0xc;
        goto tail;
    }
    if ((gKeyHeld & 0x40) != 0) {
        d = Func_8011f54(*(a + 0x22), p->x, p->z - 0x100000) - *(int *)(a + 0xc);
        if (d >= 0x80 << 13)
            goto tail;
    } else {
        if ((gKeyHeld & 0x80) == 0)
            goto tail;
        d = Func_8011f54(*(a + 0x22), p->x, p->z) - *(int *)(a + 0xc);
        if (d <= -(0x80 << 12))
            goto tail;
    }
    flag = 1;
    anim = 0xc;
tail:
    w = iwram_3001ebc;
    if (w != 0) {
        t = flag & 3;
        if (t != 0)
            *(unsigned short *)(w + (0xce << 1)) += 1;
        else
            *(unsigned short *)(w + (0xce << 1)) = t;
    }
    Actor_SetAnim(a, anim);
    if (flag != 0) {
        *(int *)(a + 0x38) = 0x80 << 24;
        *(int *)(a + 0x3c) = 0x80 << 24;
        *(int *)(a + 0x40) = 0x80 << 24;
        *(int *)(a + 0x24) = 0;
        *(int *)(a + 0x28) = 0;
        *(int *)(a + 0x2c) = 0;
    } else {
        {vec3_t *q = &s.v;
        Actor_TravelTo(a, q->x, q->y, q->z);}
    }
    *(unsigned short *)(a + 4) += 1;
    return 1;
}
