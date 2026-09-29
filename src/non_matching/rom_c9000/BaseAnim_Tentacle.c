/* BaseAnim_Tentacle -- 0x080ccc38, 256 ROM instructions.
 * NON-MATCHING, 241 of 279 encodings differ.
 * Size 636 against the ROM's 644 (-8) and 274 encodings against 279 (-5), so
 * 241 is NOT a true distance.  tools/aligncmp.py reads 191 aligned-equal of
 * 279, 101 differing in 60 hunks, and the first SEVEN encodings are identical.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c9000/ccc38_Tentacle.c \
 *     asm/rom_c9000/rom_cc5d8_c_c.s --func BaseAnim_Tentacle
 *
 * THE SPLIT.  asm/rom_c9000/rom_cc5d8_c_c.s holds THREE functions
 * (BaseAnim_Tentacle, Anim_SpiderWeb at 0x080ccebc, AnimTransitionOut at
 * 0x080cd104 -- the last already parked as src/non_matching/rom_c9000/80cd104.c)
 * AND a `.rodata` section, so this needs a TEXT/DATA SPLIT with the data keeping
 * its own object.  `.Lee058 / .Lee05c / .Lee060` are already `.global`;
 * THE SPLIT MUST ADD FIVE MORE -- `.global .Lee064`, `.Lee06a`, `.Lee070`,
 * `.Lee07c`, `.Lee088` -- because BaseAnim_Tentacle is the only function in the
 * file that reads them (datacheck confirms the other two need none).  They come
 * into C as `extern unsigned char Lee064[] __asm__(".Lee064");` and friends, the
 * idiom already landed in src/rom_c9000/rom_dd2ac_c_c_b.c for `.Leeb96`.
 *
 * SHIMS: ZERO.
 *
 * STRUCTURE, all reproduced: the `ldmia r3!, {r7}` table walk
 * (`pp = tbl; base = *pp++; ctx = *pp;`); the frame 0x20 with the vec3_t at
 * sp+0x14 and the drawing function pointer spilled to sp+0xc; the two
 * BuildDraw2DFuncEx arms; the variant-0 palette copy through a CopyFn local
 * (the ROM's `ldr r3,=Func_8001af8 / bl _call_via_r3`); StartTask(Task_BlitAnim,
 * 0x90<<3); `REG_BG2X = (k - pos.x) << 8` with k selected by f4; the signed
 * `frame / 4`; both six-argument indirect draw calls with their five table
 * lookups; the frame==8 / frame==0xd / frame==0x41 blocks; the 0x4a-vs-0x30
 * frame count.
 *
 * THE LEVER THAT MOVED IT MOST: DO NOT CACHE THE BATTLE-STATE SLOT.  The ROM
 * derives `base + 0x7828` THREE separate times, with its own pool word each
 * time (function head, after StartTask, and again in the frame-loop preheader).
 * A cached `State **slot` local collapses all three; writing
 * `(*(State **)(base + 0x7828))->f4` out at every use -- the idiom already
 * landed in src/rom_c9000/rom_dd2ac_c_c_b.c -- restores them.  177 -> 191
 * aligned, and it is what recovered the loop preheader.
 *
 * ============================================================
 * THE RESIDUE: -5 INSTRUCTIONS IN TWO PLACES.
 *
 *  (1) THE `.Lee070` ADDRESS IS NOT HOISTED (-2).  The ROM's frame-loop
 *      preheader is `ldr r3,=0x7828 / ldr r4,=.Lee070 / add r6,r7,r3 /
 *      mov r11,r4`: ONE pseudo holds the table address in r11 and BOTH draw arms
 *      read it (`mov r0,r11 / ldrh r1,[r0,r3]`).  Ours loads it from the pool
 *      separately in each arm, so there is no single pseudo to hoist -- the two
 *      arms are different basic blocks and cse's per-block table never commons
 *      them.
 *      MEASURED: a named `unsigned short *t70 = Lee070;` assigned just before
 *      the loop is WORSE, 181 aligned (it gets a low register and costs copies
 *      at both sites).  So the ROM's r11 is gcc's own LICM decision, not a
 *      source-level hoist, and the handle is whatever made gcc create the one
 *      pseudo -- not naming it.
 *  (2) THE BuildDraw2DFuncEx `bl` IS SHARED IN THE ROM AND NOT IN OURS (+1).
 *      The ROM duplicates all five argument movs into both arms and CROSS-JUMPS
 *      the single trailing `bl`; ours emits a `bl` in each arm.  Note the ROM's
 *      first arm reuses the register that already holds f4 for the fifth
 *      argument (`str r3,[sp]` with f4 == 1 known equal) -- record_jump_equiv --
 *      and THIS CANDIDATE REPRODUCES THAT, so only the tail merge is missing.
 *      Remaining -4 is the same cross-jumping asymmetry inside the two draw
 *      arms, which is the FAMILY WARNING working in the documented direction:
 *      our arms end up MORE identical than the ROM's and gcc merges what the
 *      ROM keeps.  Do NOT tidy the two arms to look alike.
 *
 * NEXT, if reopened: (2) is the tractable half.  The ROM's two draw arms differ
 * only in `base +` vs `gBuffer +` and in WHERE `str r0,[sp,#4]` sits relative to
 * `add r3,#0x20`; find the statement order that reproduces that asymmetry and
 * the merge stops.  (1) is a loop.c/LICM question and is probably downstream of
 * it.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);

typedef struct {
    int f0;
    int f4;
    unsigned char pad8[0x24 - 8];
    short f24;
} State;

typedef struct {
    unsigned char pad00[0x28];
    int f28;
    unsigned char pad2c[0x44 - 0x2c];
    int f44;
    int f48;
} Ent;

extern int *iwram_3001eec[];
extern unsigned char gBuffer[];
extern unsigned char Lee064[] __asm__(".Lee064");
extern unsigned char Lee06a[] __asm__(".Lee06a");
extern unsigned short Lee070[] __asm__(".Lee070");
extern unsigned char Lee07c[] __asm__(".Lee07c");
extern signed char Lee088[] __asm__(".Lee088");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern Ent **_GetBattleActor(int id);
extern void GetBattleActorPos2(int id, vec3_t *out);
extern void _Func_80bd7dc(int id);
extern void _PlaySound(int id);
extern void _SetBattleActorKnockback(int id, int n);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(int n);
extern void gfree(int tag);

void BaseAnim_Tentacle(void *context, int variant)
{
    vec3_t pos;
    DrawFn d;
    int nframes;
    char **tbl;
    char **pp;
    unsigned char *base;
    void *ctx;
    Ent *act;
    CopyFn copy;
    int frame;
    int n;
    int k;

    tbl = (char **)iwram_3001eec;
    pp = tbl;
    base = (unsigned char *)*pp++;
    ctx = (void *)*pp;
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0);
    REG_BG2PA = 0x100;
    if ((*(State **)(base + 0x7828))->f4 == 1)
        BuildDraw2DFuncEx(0x2e, 7, 7, 3, 1);
    else
        BuildDraw2DFuncEx(0x2e, 7, 7, 7, 1);
    d = (DrawFn)tbl[7];
    LoadVFXFile(FILE_71, base, 1, 1);
    LoadVFXFile(FILE_72, gBuffer, 1, 0);
    if (variant == 0) {
        copy = Func_8001af8;
        copy((volatile u16 *)(0xa0 << 19), GetFile(FILE_a0), 0x80);
    }
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    StartTask(Task_BlitAnim, 0x90 << 3);
    act = *_GetBattleActor((*(State **)(base + 0x7828))->f24);
    GetBattleActorPos2((*(State **)(base + 0x7828))->f24, &pos);
    if ((*(State **)(base + 0x7828))->f4 == 0)
        k = 0x10;
    else
        k = 0x70;
    REG_BG2X = (k - pos.x) << 8;
    nframes = 0x4a;
    if (variant != 1)
        nframes = 0x30;
    for (frame = 0; frame != nframes; frame++) {
        n = frame / 4;
        if (n <= 5) {
            if (n <= 3)
                d(ctx, base + Lee070[n], Lee07c[(*(State **)(base + 0x7828))->f4 * 6 + n],
                  Lee088[n] + 0x20, Lee064[n], Lee06a[n]);
            else
                d(ctx, gBuffer + Lee070[n], Lee07c[(*(State **)(base + 0x7828))->f4 * 6 + n],
                  Lee088[n] + 0x20, Lee064[n], Lee06a[n]);
        }
        if (frame == 8) {
            if (variant == 0) {
                _Func_80bd7dc(0x85);
                _SetBattleActorKnockback((*(State **)(base + 0x7828))->f24, 1);
            } else {
                _PlaySound(0x86);
                Func_80d6888((*(State **)(base + 0x7828))->f24, 7, 5, 0, 4);
            }
            *(int *)(base + 0x77a8) = 8;
        }
        if (variant == 1) {
            if (frame == 0xd) {
                act->f28 = 0xc0 << 12;
                act->f48 = 0x7851;
                act->f44 = 0x80 << 7;
            }
            if (frame == 0x41) {
                *(int *)(base + 0x77a8) = 4;
                _Func_80bd7dc(0x86);
            }
        }
        UpdateScreenShake(8, 8);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
    }
    StopTask(Task_BlitAnim);
    gfree(0x2e);
    AnimEnd();
}
