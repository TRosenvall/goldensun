/* Anim_Hail -- MATCHING.  956 bytes, 422 encodings and 45 relocations identical.
 * FRAME `sub sp, #0x30`, the ROM's exactly.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/rom_c9000/rom_e0564_a_b.c \
 *     asm/rom_c9000/rom_e0564_a.s --func Anim_Hail
 *
 * objcmp prints one advisory twice and it is not a defect:
 *   `~~ relocation _call_via_r12 / _call_via_ip is ONE symbol`.
 *
 * SPLIT SHAPE.  asm/rom_c9000/rom_e0564_a.s holds FOUR functions --
 * Anim_Venus, Anim_Mars, Anim_Hail, Anim_Ground.  Anim_Hail is the THIRD, so
 * landing it is a three-way split of that stem:
 *
 *   asm/rom_c9000/rom_e0564_a_a.s   Anim_Venus + Anim_Mars   (unchanged asm)
 *   src/rom_c9000/rom_e0564_a_b.c   THIS FILE
 *   asm/rom_c9000/rom_e0564_a_c.s   Anim_Ground              (unchanged asm)
 *
 * and stage1.ld's two `asm/rom_c9000/rom_e0564_a.o(...)` lines (the .text line
 * and the .rodata line directly under it) each become three, in that order.
 * `python3 tools/datacheck.py asm/rom_c9000/rom_e0564_a.s` is SILENT -- the file
 * carries no data section of its own, so nothing is lost by the split; the
 * .rodata lines stay only to keep the stem's shape.
 * Per "COPY THE `.include` LINES, NOT THE FILE HEAD": rom_e0564_a.s opens with
 * `macros.inc` + `gba.inc` and each piece needs both, plus its own leading
 * comment block (the head's block describes Anim_Venus only).
 *
 * EXPORTS: none new.  `.Leec5f`, `.Leec63` and `.Leec68` are read as externs and
 * are ALREADY `.global` in asm/rom_c9000/rom_e0564_c.s from the earlier split of
 * this stem, so no asm edit is required and objcmp's SIZE line stays clean (no
 * .rodata-from-C false positive).  `Data_ede48` lives in rom_eda78.s.
 *
 * ================================================================
 * WHAT CLOSED IT, BY PASS
 * ================================================================
 *
 * Pass 1 (h1.c) read 5 of 422 with SIZE and COUNT both already exact, from three
 * oracles taken off the landed siblings before writing a line:
 *   - src/rom_c9000/rom_dd2ac_c_c_b.c (Anim_Vine) for the Anim_Djinni /
 *     BuildDraw2DFuncs / `d[2]` idiom and for `slot` named at the top but
 *     `*(State **)(base + 0x7828)` written INLINE inside the frame loop;
 *   - src/rom_c9000/rom_d82b0_b.c (Anim_Break) for the reused-counter rule
 *     (ONE variable across all three of this function's outer loops -- i for the
 *     0x40-particle init, the 0x40-particle draw, and the five-lane tail);
 *   - the ROM's own two lane loops, which must stay TWO do-whiles: the family
 *     warning about cross-jumping is real and the arms differ only in the sign
 *     of the `(frame / 4) & 0x1f` term and the `- 0x20`.
 *
 * Pass 2 closed the last 5 with ONE edit -- the strength-reduced-giv lever from
 * Anim_Break.  The particle-init loop was written `g = gBuffer; do { g->x = ...;
 * g++; } while (i != 0x40);`, which makes `g = gBuffer` a SOURCE preheader
 * statement; loop.c then inserts its hoisted invariants (0xf and 0x7f) AFTER it,
 * so the pool load is born first and wins r3, and the ROM's order
 * `ldr r4,=gBuffer / movs r3,#15 / ... / mov r9,r3 / mov r8,r4` comes out
 * transposed.  Indexing instead -- `gBuffer[i].x = ...` with no walking pointer --
 * makes the base a strength_reduce giv init, created AFTER move_movables, so
 * `movs r3,#15` is born first and the five encodings land.
 *
 * > A PREHEADER TRANSPOSITION BETWEEN A POOL LOAD AND A HOISTED CONSTANT IS THE
 * > SIGNATURE OF A SOURCE-LEVEL POINTER INIT WHERE THE ROM HAS A giv.  The
 * > register roles are already right; only the birth order is wrong.
 *
 * The biv survives the rewrite here (nine giv references off one index keep it),
 * so Anim_Vine's "the index must be used twice or loop.c deletes the counter"
 * caution does not bite.
 *
 * OTHER FACTS READ OFF THE REFERENCE, all confirmed by the match:
 *   - The two `fns[...]` call sites go through the ARRAY, and the spilled
 *     address pseudo at sp+0xc is gcc's, not a source-level `DrawFn *fp`:
 *     `fns[1]` reads it (`ldr r0,[sp,#0xc] / ldr r4,[r0,#4]`) while the lane
 *     loop's `fns[0]` reads sp+0x14 directly.  No pointer local is needed.
 *   - `.Leec5f` and `.Leec63` are UNSIGNED chars: the ROM halves them with
 *     `lsr`, and each is written twice per iteration (once for the /2, once as
 *     an argument) with cse commoning the `ldrb`.
 *   - `(-sin(ang) * 4) >> 16` and `(cos(ang) * 2) >> 16`; `mag` is
 *     `(Random() & 0x1ff) + 0x80` with the ROM's `add r5,#0x80` merely scheduled
 *     past the sin call.
 *   - `0x14`, `0x28`, `0x78` and `0x20` stay BARE LITERALS so loop.c hoists them
 *     into r8/r7/r6; naming any of them would make them cheap to rebuild.
 *
 * SHIMS: none.  `python3 tools/shimcount.py` reports zero, so no fakematch.txt
 * row is needed.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern int *iwram_3001eec[];
extern Part gBuffer[];
extern unsigned char Leec5f[] __asm__(".Leec5f");
extern unsigned char Leec63[] __asm__(".Leec63");
extern unsigned short Leec68[] __asm__(".Leec68");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void Anim_Djinni(void *context, int a, int b, int c, int *p1, int *p2);
extern void BuildDraw2DFuncs(int a, void **fns);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void GetBattleActorPos3(int id, vec3_t *out);
extern int Random(void);
extern int sin(int a);
extern int cos(int a);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _SetBattleActorKnockback(int id, int a);
extern void Func_80e38b8(Part *g, int a, int b);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void Anim_Hail(void *context)
{
    vec3_t pos;
    int cx;
    int cy;
    DrawFn fns[2];
    void *ctx;
    char **tbl;
    char **pp;
    u8 *base;
    State **slot;
    Part *g;
    int i;
    int frame;
    int arg;
    int s;

    tbl = (char **)iwram_3001eec;
    pp = tbl;
    base = (u8 *)*pp++;
    ctx = (void *)*pp;
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(0);
    Anim_Djinni(context, 1, (*slot)->f4, 2, &cx, &cy);
    BuildDraw2DFuncs((*slot)->f4, (void **)fns);
    LoadVFXFile(FILE_6e, base, 1, 1);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    GetBattleActorPos3((*slot)->ids[0], &pos);
    i = 0;
    do {
        int a2 = (Random() & 0x7fff) + 0x4000;
        int mag = (Random() & 0x1ff) + 0x80;
        gBuffer[i].x = (pos.x / 2 + (Random() & 0xf) - 8) << 16;
        gBuffer[i].y = (pos.y + 8) << 16;
        gBuffer[i].vx = (sin(a2) * mag) >> 9;
        gBuffer[i].vy = (cos(a2) * mag) >> 6;
        gBuffer[i].z = Random() & 0x7f;
        gBuffer[i].vz = Random() & 0x7f;
        gBuffer[i].t = (Random() & 0xf) + 0x20;
        i++;
    } while (i != 0x40);
    frame = 0;
    do {
        if (frame > 0x2f) {
            REG_BLDALPHA = (0x40 - frame) | 0x1000;
        }
        if (frame == 1) {
            LoadVFXFile(FILE_b8, base + (0x80 << 3), 1, 1);
            LoadVFXFile(FILE_92, base + 0x65c0, 1, 0);
        }
        if ((*(State **)(base + 0x7828))->f1c == 1) {
            int ang = frame << 11;
            int X = ((-sin(ang) * 4) >> 16) + cx / 2 - 0xa;
            int Y = ((cos(ang) * 2) >> 16) + cy - 0x16;
            if (frame > 0x45) {
                Y = Y - frame * 2 + 0x8a;
            }
            fns[1](ctx, base + 0x65c0, X, Y, 0x14, 0x28);
            if (frame <= 3) {
                fns[1](ctx, base + 0x65c0, X, Y, 0x14, 0x28);
            }
        }
        g = gBuffer;
        i = 0;
        do {
            if (frame >= i / 4 + 4) {
                int k = (g->z / 0x80) & 3;
                fns[i & 1](ctx, base + (0x80 << 3) + Leec68[k],
                           *(short *)((char *)g + 2) - Leec5f[k] / 2,
                           *(short *)((char *)g + 6) - Leec63[k] / 2,
                           Leec5f[k], Leec63[k]);
                Func_80e38b8(g, 0x3f, 0x80 << 5);
            }
            i++;
            g++;
        } while (i != 0x40);
        if (frame == 8) {
            *(int *)(base + 0x77a8) = 8;
            _Func_80bd7dc(0x86);
            Func_80d6888((*(State **)(base + 0x7828))->ids[0], 7, 5, 0, 0x10);
            _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[0], 3);
        }
        s = frame * 4;
        if (s > 0x20) {
            s = 0x20;
        }
        if ((*(State **)(base + 0x7828))->f4 == 0) {
            i = 0;
            do {
                fns[0](ctx, base, i * 32 - ((frame / 4) & 0x1f), 0x78 - s, 0x20, 0x20);
                i++;
            } while (i != 5);
        } else {
            i = 0;
            do {
                fns[0](ctx, base, i * 32 + ((frame / 4) & 0x1f) - 0x20, 0x78 - s, 0x20, 0x20);
                i++;
            } while (i != 5);
        }
        UpdateScreenShake(4, 8);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x40);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
