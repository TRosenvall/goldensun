/* ActorCmd_Player_Climb (0x0800f7f4) -- NON-MATCHING: 168 encodings of 222 differ (objcmp).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_9000/800f7f4.c asm/rom_9000/rom_ebec_c.s --func ActorCmd_Player_Climb
 *
 * SIZE 468 vs the ROM's 472 and 220 instructions vs 222, so 168 is NOT a
 * distance -- TWO INSTRUCTIONS ARE STILL MISSING.  tryc --full --align reports
 * 94 instructions in disagreeing regions of 229 lines, which is the honest
 * figure; the headline 168 is difflib mis-alignment.
 *
 * THE SPLIT this needs: rom_ebec_c.s holds ONE function plus a .rodata tail of
 * four `.incrom` blobs.  The function reads ONLY .L13254, which the file ALREADY
 * exports (`.global .L13254`), so the split needs NO NEW EXPORTS.  Two files:
 * the function alone, and the `.include` + `.section .rodata` + four labels.
 *
 * ================== WHAT WAS SOLVED (keep all of it) ==================
 * 1. THE FRAME, which is the thing to read first.  The ROM has `sub sp, #0x50`
 *    with its ONLY stack object at sp+0x44 -- 68 bytes of frame that nothing in
 *    the function touches.  The whole rom_ebec family shares the number:
 *      ActorCmd_Player        0x68 frame, vec3s at 0x44/0x50/0x5c  (3)
 *      ActorCmd_Player_World  0x5c frame, vec3s at 0x44/0x50       (2)
 *      ActorCmd_Player_Climb  0x50 frame, vec3  at 0x44            (1)
 *    68 + 12*N exactly, every time.  So the vec3s are the LAST members of one
 *    local aggregate whose first 68 bytes this handler never reads.  Spelled
 *      struct { unsigned char pad[68]; vec3_t v; } s;  vec3_t *p;  p = &s.v;
 *    gcc emits `sub sp, #80` and `add r5, sp, #68` -- the ROM's two encodings,
 *    at no instruction cost.  A bare `int pad[17]` is DELETED unless it is
 *    stored to, and storing to it costs 3 instructions; the aggregate is what
 *    makes the slot survive for free.  The named `p` is required: addressing
 *    `s.v.x` directly costs 2 instructions (224 vs 222) and loses `add r5,sp,#68`.
 *    p BORN at the zero stores is 169; born at the top of the function, 188.
 *    A corpus sweep (every function still in asm) found Climb is the ONLY one
 *    whose lowest stack offset is 0x44, so this is not a compile-unit artifact.
 * 2. `extern volatile unsigned int gKeyHeld;` -- the ROM reads gKeyHeld THREE
 *    times and keeps only the ADDRESS in r1 across the 0x40/0x80 branch
 *    (`ldr r1,=gKeyHeld ... ldr r3,[r1]` in BOTH arms).  gcse commons the VALUE
 *    without volatile.  Single drop 218 -> 220 instructions (the missing re-read
 *    comes back), 169 -> 168.  `-fno-gcse` reaches the same re-read (228 lines,
 *    156 dirty) and is the per-TU alternative if volatile is judged wrong.
 * 3. `short h; h = L13254[k];` -- the ROM's `ldrsh` says the DESTINATION is a
 *    short (docs "ldrh versus ldrsh: the type of the DESTINATION decides").
 *    With an `int` h, combine folds the load and the zero-extension into one
 *    `ldrh` and the ROM's `lsl #16 / lsr #16` pair vanishes: 216 instructions,
 *    460 bytes.  `(unsigned short)h` at each use site keeps both `lsr`s.
 * 4. Both halfword facing literals need INT CARRIERS in their own blocks
 *    (`{ int f = 0xc0 << 8; *(unsigned short *)(a+6) = f; }`): written as bare
 *    literals through the `unsigned short *` they POOL (`ldr r3,=0xc000`) where
 *    the ROM builds `mov #0xc0 / lsl #8`.  The recorded halfword-literal rule.
 * 5. `.L13254` is reached with the asm-label extension,
 *    `extern short L13254[] __asm__(".L13254");` -- 5 digits, so the
 *    low-numbered-label caveat does not apply.  `p[i]` on the short array gives
 *    the ROM's register-offset `ldrsh r3, [r1, r3]`.
 * 6. r4 carries 0x8000 from the +0x30 store to the `cmp r2,r4` at 0xf846 by
 *    plain constant CSE (no call between), while 0x4000 is rebuilt after
 *    vec3_translate.  Both are written as plain literals; the dominance rule
 *    does the rest.  The `anim = 0xe / 0xf / 0xa` chain comes out as the ROM's
 *    three separate `mov r9` pairs with ordinary nested ifs -- the apparent
 *    cross-jump in the aligned diff is difflib, not codegen.
 *
 * ================== THE RESIDUE ==================
 * Two instructions short, and the identified half of that is:
 *   - gBuffer's address is loaded TWICE in the ROM (`ldr r0,=gBuffer ... add r0,r3`
 *     destructive on the symbol register, then `ldr r1,=gBuffer ... add r7,r3,r1`
 *     three-operand).  We load it once and CSE.  `p0 = gBuffer; p0 += off;` to
 *     force the destructive form made it WORSE (214 of 218) -- the two adds are
 *     different forms in the ROM and that pairing has not been found.
 *   - branch polarity at the 0x40 arm: ROM `cmp r3,r0 / bge tail / b set1`,
 *     ours `blt set1 / b tail`.  Two encodings, same count.
 *   - `sub r3, r0, r3` (three-operand, destination = the pos.y register) against
 *     our destructive `sub r0, r3`, at both Func_8011f54 sites.
 * Everything else is low-register scratch rotation (r0/r1/r2/r3 permuted) in the
 * two `/ 0x100000` divisions and the two Func_8011f54 arms.
 *
 * NEXT: find what makes gBuffer rematerialise at the second index; that is the
 * one clearly-missing instruction, and the r0/r1 rotation in the divisions
 * almost certainly follows it.
 *
 * `volatile` on gKeyHeld is a READING (three reads in the ROM, three in the
 * source), not a pin -- if this lands it needs no fakematch row for it.  There
 * are no register pins, no barriers and no .equ in this draft.
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
        Actor_TravelTo(a, p->x, p->y, p->z);
    }
    *(unsigned short *)(a + 4) += 1;
    return 1;
}
