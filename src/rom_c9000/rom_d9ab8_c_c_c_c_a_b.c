/* Anim_Flare -- MATCHING.  644 bytes, 290 encodings and 24 relocations
 * identical to the ROM.  Split out of asm/rom_c9000/rom_d9ab8_c_c_c_c_a.s,
 * where it is the SECOND of TWO functions (BaseAnim_StatDown is the first and
 * is still parked as src/non_matching/rom_c9000/d9ab8_StatDown.c).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py scratch_elev/b300a/flare_j.c \
 *     asm/rom_c9000/rom_d9ab8_c_c_c_c_a.s --func Anim_Flare
 *
 * SPLIT SHAPE: TEXT-ONLY, NO NEW EXPORTS.  tools/datacheck.py prints nothing
 * for rom_d9ab8_c_c_c_c_a.s -- the file has no data section at all.  The four
 * tables it reads (Data_edeb2 u16[], Data_ede9f/Data_edea5/Data_edeab u8[])
 * are already .global in another TU and are already spelled as externs by
 * src/non_matching/rom_c9000/rom_dc968.c and e47b8_SpecialAttack.c.  NO SHIMS,
 * NO PINS (tools/shimcount.py is clean), no per-file Makefile flag override.
 *
 * ================================================================
 * THE ONE THAT MATTERS: THE WIDTH LOAD MUST BE AN EMBEDDED ASSIGNMENT INSIDE
 * ARGUMENT 3, BECAUSE THE LITERAL POOL RECORDS ARGUMENT ORDER
 * ================================================================
 *
 * The blit is called three times with the same six subexpressions, and the ROM
 * reaches them as
 *
 *     ldr r0,=Data_edeb2 / ldr r2,=Data_ede9f / lsl r4,r5,#1 / ldrh r1,[r0,r4]
 *
 * i.e. Data_edeb2 IS THE FIRST OF THE TWO SYMBOLS TO GET AN `ldr`, so the
 * assembler pools it at +0x270 and Data_ede9f at +0x274.  Any candidate that
 * loads the width first gets the pool the other way round: the instruction
 * stream still has the same 290 encodings and the same 644 bytes, but every
 * `ldr [pc,#N]` in the three call sites is wrong and objcmp reports a
 * RELOCATIONS difference as well.  THIS IS INVISIBLE TO tryc.py.
 *
 * Three spellings were measured (frame/fn slot order already correct, see
 * below), all with size and count exact:
 *
 *   w as its own statement before the call         42 -> 23 -> 16 of 290
 *   w inline TWICE in the argument list            298 instructions (+8):
 *       gcc stops CSEing the two `ldrb`s, hoists &Data_ede9f and &Data_edeab
 *       into registers that must survive the call, spills both, and the call
 *       site degrades to `bl _call_via_r7`.
 *   w assigned INSIDE argument 3                   0 of 290  <-- this file
 *
 *     fn(ctx, base + Data_edeb2[idx],
 *        q->x - ((w = Data_ede9f[idx]) >> 1),
 *        q->y + Data_edeab[idx], w, Data_edea5[idx]);
 *
 * The embedded assignment is what orders the two symbol loads: argument 2 is
 * expanded before argument 3, so Data_edeb2 gets its `ldr` first, while `w`
 * still exists as ONE pseudo shared by arguments 3 and 5 -- which is what
 * keeps the instruction count at 290.  `unsigned char w` (not int) is required
 * for the ROM's `lsr r3,r4,#1`; a signed int gives `asr`.
 *
 * ================================================================
 * THREE MORE LEVERS, in the order they paid
 * ================================================================
 *
 * TWO SEPARATE `Part *` FOR THE TWO LOOPS, worth 42 -> 23.  The ROM keeps the
 * sin/cos setup loop's walker in r5 and the frame loop's walker in r6.  One
 * source variable cannot do that: gcc-2.96 does not split live ranges, so a
 * single `q` gets ONE hard register for both loops (r6 in both) and the whole
 * setup loop reads with the wrong register.  This is the recorded
 * one-variable-per-region rule, and it is worth more here than anything else.
 *
 * `ang += 0x1000` BEFORE `i++` IN THE LOOP BODY, worth 21 -> 16.  Both are
 * constant pseudos competing for r3 (first callee-clobbered entry in
 * REG_ALLOC_ORDER); whichever the source mentions first gets it.  The ROM puts
 * 0x1000 in r3 and the 1 in r4, so the angle step must be written first.
 *
 * `i = 0;` BEFORE `t = 0;` in the setup preheader, worth 23 -> 21.
 *
 * ALSO LOAD-BEARING, found on the first candidate and not moved since:
 *   - `int frame;` DECLARED BEFORE `DrawFn fn;`.  The three spilled scalars land
 *     at sp+0x8/0xc/0x10 in DESCENDING declaration order (ctx 0x10, frame 0xc,
 *     fn 0x8), so the declaration list IS the frame map.
 *   - `pp = tbl; base = *pp++; ctx = *pp;` for the ROM's `ldmia r3!, {r1}`
 *     (the recorded iwram_3001eec pointer-table lever).
 *   - `if ((unsigned)life <= 0x17)` -- an UNSIGNED guard (`cmp #0x17 / bhi`)
 *     around a SIGNED `life / 4`.  gcc-2.96 has no value-range propagation, so
 *     the signed-division correction `if (life < 0) life += 3` survives the
 *     unsigned test, exactly as the ROM has it.
 *   - `q->t = q->t + 1;` -- a RE-READ, not `life + 1`.  The calls clobber
 *     memory, so the ROM reloads; on the skipped path cse reuses the value
 *     loaded at the top of the body, which is the ROM's `.Lda192/.Lda194` pair.
 *   - TWO SEPARATE STORES for the 0x7784 value, not a `?:` and not a value
 *     variable.  The ROM emits `ldr r2,=0x7784` in BOTH arms and cross-jumps
 *     only the shared `add r2,r8 / str r3,[r2]` tail; a single store with a
 *     conditional value computes the address once.
 *   - `while (i != (*(State **)(base + 0x7828))->f14)` for the Func_80d6888
 *     loop -- the `while` form, so duplicate_loop_exit_test manufactures the
 *     guard AFTER gcse and the address is re-derived with its own pool word in
 *     the loop, as the ROM does.  Do not cache base + 0x7828 here.
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
extern unsigned char Data_ede9f[], Data_edea5[], Data_edeab[];
extern unsigned short Data_edeb2[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern int  BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern int  sin(int a);
extern int  cos(int a);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void Func_80cd52c(void);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_Flare(void *context)
{
    char **tbl;
    char **pp;
    u8 *base;
    void *ctx;
    int frame;
    DrawFn fn;
    Part *p;
    Part *q;
    int i;
    int ang, t, xb, sign;

    tbl = (char **)iwram_3001eec;
    pp = tbl;
    base = (u8 *)*pp++;
    ctx = (void *)*pp;
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0);
    REG_BLDCNT = 0x3f46;
    REG_BLDALPHA = 0x100e;
    LoadVFXFile(FILE_b4, base, 1, 1);
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 3);
    fn = (DrawFn)tbl[7];
    if ((*(State **)(base + 0x7828))->ids[0] > 0x7f) {
        xb = 0;
        sign = 1;
    } else {
        xb = 0x40;
        sign = -1;
    }
    ang = -0x4000;
    i = 0;
    t = 0;
    p = (Part *)(base + (0xe1 << 7));
    do {
        p->x = sign * ((sin(ang) << 5) >> 16) + xb + 0x14;
        p->y = ((cos(ang) << 4) >> 16) + 0x28;
        p->t = t;
        ang += 0x1000;
        i++;
        t -= 4;
        p++;
    } while (i != 9);
    *(int *)(base + (0xef << 7)) = 2;
    if ((*(State **)(base + 0x7828))->f18 == 2) {
        *(int *)(base + 0x7784) = 0x4b;
    } else {
        *(int *)(base + 0x7784) = 0x32;
    }
    StartTask(Task_BlitAnim, 0x90 << 3);
    _PlaySound(0x88);
    frame = 0;
    do {
        if (frame == 0x18) {
            _Func_80bd7dc(0x85);
        }
        i = 0;
        q = (Part *)(base + (0xe1 << 7));
        do {
            int life = q->t;
            if ((unsigned)life <= 0x17) {
                int idx = life / 4;
                unsigned char w;
                fn(ctx, base + Data_edeb2[idx],
                   q->x - ((w = Data_ede9f[idx]) >> 1),
                   q->y + Data_edeab[idx], w, Data_edea5[idx]);
                if ((*(State **)(base + 0x7828))->f18 != 0) {
                    fn(ctx, base + Data_edeb2[idx],
                       q->x - ((w = Data_ede9f[idx]) >> 1),
                       (q->y + Data_edeab[idx]) - 0x10, w, Data_edea5[idx]);
                }
                if ((*(State **)(base + 0x7828))->f18 == 2) {
                    fn(ctx, base + Data_edeb2[idx],
                       q->x - ((w = Data_ede9f[idx]) >> 1),
                       (q->y + Data_edeab[idx]) - 0x20, w, Data_edea5[idx]);
                }
            }
            q->t = q->t + 1;
            i++;
            q++;
        } while (i != 9);
        i = 0;
        while (i != (*(State **)(base + 0x7828))->f14) {
            if (frame == i * 8 + 0x10) {
                Func_80d6888((*(State **)(base + 0x7828))->ids[i], 0xa, 5, i, 0xc);
            }
            i++;
        }
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x50);
    gfree(0x2e);
    StopTask(Task_BlitAnim);
    AnimEnd();
}
