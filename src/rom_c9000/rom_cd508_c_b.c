/* Anim_Confuse -- MATCHING.  724 bytes, 323 encodings and 30 relocations
 * identical to the ROM.  Split out of asm/rom_c9000/rom_cd508_c.s, where it is
 * the FIFTH and LAST of five functions (InitRenderTilemapBG1, DrawLine,
 * Anim_PlanetDiver, Anim_Haunt, Anim_Confuse); InitRenderTilemapBG1 and
 * Anim_Haunt are the existing parks 80cdd58.c and cd508_Haunt.c.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py scratch_elev/b300a/confuse_z1.c \
 *     asm/rom_c9000/rom_cd508_c.s --func Anim_Confuse
 *
 * SPLIT SHAPE: TEXT-ONLY, NO NEW EXPORTS.  tools/datacheck.py prints nothing
 * for rom_cd508_c.s; it has no data section.  Every symbol used already exists
 * (iwram_3001eec, iwram_3001e80, _FILE_af, Func_8001af8, GetFile,
 * DecompressLZ, MatrixSetLook, Func_80e3944, Func_80dbb9c).  NO SHIMS, NO PINS
 * -- tools/shimcount.py is clean.  No per-file Makefile flag override.
 *
 * NOTE FOR THE Anim_Haunt PARK NEXT DOOR: `base __asm__("r10")` IS NOT NEEDED
 * HERE.  cd508_Haunt.c tells whoever writes this bank to pin base from the
 * first candidate; on Anim_Confuse the very first candidate put base in r10 on
 * its own and read 323 of 323 with size and instruction count already exact.
 * The pin advice is a lead, not a fact, for this function.
 *
 * ================================================================
 * THE TWO THAT MATTER
 * ================================================================
 *
 * (1) ONE COUNTER SERVES THE TWO SIN LOOPS *AND* THE FOUR-STEP DRAW LOOP.
 *     Worth 56 -> 22 of 323, the single largest step here.  The ROM keeps r7
 *     as the counter in the 0xa0-iteration sin loop of BOTH arms and in the
 *     inner draw loop, and keeps r11 for `&out`.  With separate `n` and `j` the
 *     draw counter takes r11 (a high register), so every `j++` becomes
 *     `movs r2,#1 / add fp,r2 / mov r1,fp` and `&out` drops to a low register
 *     -- the instruction COUNT still matches, which is exactly the trap the
 *     brief warns about.  Unifying them gives r7 to the counter and r11 to the
 *     pointer, with no change in what is computed.  This is the CONVERSE of the
 *     one-variable-per-region rule and the two must be measured, not assumed:
 *     Anim_Flare in this same batch needed the pointers SPLIT.
 *
 * (2) THE INNER-LOOP ANGLE MUST BE A GIV, NOT A STEPPED LOCAL.  The last two
 *     encodings.  `int b = (frame << 9) + (n << 14);` INSIDE the body, with no
 *     `b += 0x4000` at the end, is worth 2 -> 0; loop.c strength-reduces it to
 *     the ROM's `b` and inserts the preheader initialiser AFTER the invariant
 *     it hoisted for `&out`, so `mov fp,r5` lands BEFORE `lsl r6,r2,#9`.  Every
 *     statement-order permutation of the stepped form (b before n, n before b,
 *     n hoisted above the Func_80e3944 call) is stuck one position out, and an
 *     explicit `vec3_t *o = &out;` costs TWO INSTRUCTIONS.
 *
 * ================================================================
 * THREE MORE LEVERS
 * ================================================================
 *
 * THE g[8] LOAD MUST HAPPEN BEFORE `fp = fns`, worth 20 -> 6, and the mechanism
 * is RELOAD INHERITANCE.  `fp` has no hard register, so `fp = fns` becomes
 * `mov rX,sp / add rX,#0x20 / str rX,[sp,#0x10]` and `fp[1] = ...` needs the
 * value again.  If the g[8] load is still ahead of it in the RTL, it clobbers
 * the reload register and reload emits a second `ldr r1,[sp,#0x10]` -- one
 * instruction the ROM does not have.  Loading g[8] into its own local FIRST
 * leaves rX alive and reload inherits it, which is the ROM's
 * `mov r1,sp / add r1,#0x20 / str r1,[sp,#0x10] / str r3,[r1,#4]`.
 *
 * THE 0x80000 IN THE FIRST SIN LOOP MUST BE A SOURCE LOCAL, worth 6 -> 4, and
 * `n = 0;` MUST PRECEDE ITS ASSIGNMENT, worth 4 -> 2.  This is the
 * InitRenderTilemapBG1 park's own finding used the other way round: a source
 * local gets its materialisation emitted where the source puts it instead of
 * where loop.c inserts a hoisted invariant (last in the preheader).  Here that
 * is what the ROM wants, and the local still ends up in a LOW register spilled
 * around the `sin` call, which is the ROM's `str r3,[sp,#8] / ldr r3,[sp,#8]`.
 * C89 forces the split declaration: `int c; n = 0; c = 0x80 << 12;`.
 *
 * `p = data; data += 0x80;` AS TWO STATEMENTS BEFORE THE Func_8001af8 CALL,
 * worth 2 encodings.  The ROM finishes the destination constant
 * (`lsl r0,#19`) AFTER `add r5,#0x80`, and sched2 breaks that tie on INSN_LUID,
 * so the pointer bump has to come first in the source.  cd508_Haunt.c reaches
 * the same shape with a THREE-REGISTER PIN (r0/r1/r2, its PIN3 macro); the pin
 * set was walked down here and IT IS NOT NEEDED -- a plain local for the
 * destination value and a plain local for the source pointer are exact.  Six
 * variants measured: 3 pins, 2 pins, 1 pin (r0), 1 pin (r1) and both pin-free
 * forms all read 0 of 323.
 *
 * ALSO LOAD-BEARING: `DrawFn fns[2]` plus `DrawFn *fp = fns` with fns[0]
 * written through the ARRAY and fns[1] read through the POINTER (the StatDown
 * park device); one shared `int two = 2` for both BuildDraw2DFuncEx stack
 * arguments and the base+0x7780 store; one shared `arg = 0x90; arg <<= 3;` for
 * both StartTask calls; and `while (k != (*(State **)(base+0x7828))->f14)` for
 * the target loop.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

extern void *iwram_3001eec[];
extern void *iwram_3001e80;

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int  DecompressLZ(void *src, void *dst);
extern int  BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void Func_80dbb9c(void);
extern int  sin(int a);
extern int  cos(int a);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern void *_GetBattleActor(int id);
extern int  Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_Confuse(void *context)
{
    vec3_t v;
    vec3_t out;
    DrawFn fns[2];
    void *ctx;
    int k;
    int xoff;
    DrawFn *fp;
    DrawFn f1;
    void **g;
    void **pp;
    unsigned char *base;
    unsigned char *data;
    CopyFn copy;
    void *view;
    int *q;
    int two;
    int arg;
    int frame;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0);
    REG_BG2PA = 0x100;
    data = GetFile(FILE_af);
    {
        unsigned char *p;
        int d0;
        d0 = 0xa0;
        copy = Func_8001af8;
        p = data;
        data += 0x80;
        d0 <<= 19;
        copy((volatile u16 *)d0, p, 0x80);
    }
    DecompressLZ(data, base);
    two = 2;
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, two);
    fns[0] = (DrawFn)g[7];
    BuildDraw2DFuncEx(0x2f, 7, 7, 0xf, two);
    f1 = (DrawFn)g[8];
    fp = fns;
    fp[1] = f1;
    arg = 0x90;
    arg <<= 3;
    StartTask(Func_80dbb9c, arg);
    *(int *)(base + (0xef << 7)) = two;
    *(int *)(base + 0x7784) = 0x32;
    StartTask(Task_BlitAnim, arg);
    if ((*(State **)(base + 0x7828))->f4 == 1) {
        REG_BG2X = 0xffff9800;
        xoff = -0x70;
    } else {
        xoff = 0;
    }
    frame = 0;
    while (frame != (*(State **)(base + 0x7828))->f14 * 0x10 + 0x30) {
        int n;
        int a;
        view = iwram_3001e80;
        q = (int *)(base + (0xd3 << 7));
        if ((*(State **)(base + 0x7828))->f4 == 0) {
            int c;
            n = 0;
            c = 0x80 << 12;
            a = frame << 10;
            do {
                *q++ = (c - (sin(a) << 3)) >> 10;
                n++;
                a += 0x80 << 3;
            } while (n != 0xa0);
        } else {
            n = 0;
            a = frame << 10;
            do {
                *q++ = ((sin(a) << 3) >> 10) + (-0x7000);
                n++;
                a += 0x80 << 3;
            } while (n != 0xa0);
        }
        InitMatrixStack();
        MatrixSetLook(view, (char *)view + 0xc);
        k = 0;
        while (k != (*(State **)(base + 0x7828))->f14) {
            State **slot = (State **)(base + 0x7828);
            int *actor = (int *)_GetBattleActor((*slot)->ids[k]);
            int ab = *actor;
            int ioff = k * 0x10;
            if (frame > ioff && frame < ioff + 0x3c) {
                if (frame == ioff + 0x20) {
                    Func_80d6888((*slot)->ids[k], 0, 5, -1, 0);
                }
                v.x = *(int *)(ab + 8);
                v.y = 0xa0 << 14;
                v.z = *(int *)(ab + 0x10);
                Func_80e3944(&v, &out);
                n = 0;
                do {
                    int b = (frame << 9) + (n << 14);
                    int sx = out.x + ((sin(b) << 4) >> 16) + xoff;
                    int sy = out.y + ((cos(b) << 4) >> 16);
                    int u = frame / 0x10;
                    fp[u & 1](ctx, base + ((frame / 4 - u * 4) << 10),
                              sx - 0x10, sy - 0x10, 0x20, 0x20);
                    n++;
                } while (n != 4);
            }
            k++;
        }
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    }
    StopTask(Task_BlitAnim);
    StopTask(Func_80dbb9c);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
