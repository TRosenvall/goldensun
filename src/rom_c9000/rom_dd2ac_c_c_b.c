/* Anim_Vine -- MATCHING.  580 bytes, 258 encodings and 23 relocations identical,
 * frame `sub sp, #0x30`, the ROM's exactly.  Split out of
 * asm/rom_c9000/rom_dd2ac_c_c.s, where it is the SECOND of five functions
 * (BaseAnim_Growth, Anim_Vine, Anim_Thorn, Anim_Bolt, Anim_Djinni).
 *
 * LANDING NOTE -- ONE ASM EDIT IS REQUIRED AND IT IS THE TREE'S OWN CONVENTION.
 * This function reads the 16-byte .rodata blob `.Leeb96`
 * (`.incrom 0xeeb96, 0xeeba6`, signed bytes
 *  {0,14,-32,8,32,-48,4,12,-24,24,-12,-40,-4,-16,-32,12}).
 * DO NOT emit it from C: it sits in the MIDDLE of a 14-blob run
 * (.Leeb71 ... .Leebe9) in that file's .rodata, and moving one blob out of the
 * middle of the run would move its address.  Instead add
 *
 *     .global .Leeb96
 *
 * to the remaining .s beside its `.section .rodata`, exactly as
 * asm/rom_c9000/rom_cc5d8_c_c.s already does for .Lee058/.Lee05c/.Lee060
 * (consumed as externs by src/non_matching/rom_c9000/cc5d8_DjinnSet.c).  The
 * `extern signed char Leeb96[] __asm__(".Leeb96")` below is then the whole story
 * and objcmp's SIZE line stays clean -- no .rodata-from-C false positive here.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/rom_c9000/rom_dd2ac_c_b.c \
 *     asm/rom_c9000/rom_dd2ac_c_c.s --func Anim_Vine
 *
 * ================================================================
 * THE ONE THAT MATTERS: `sym[i]` AND A WALKING POINTER ARE DIFFERENT CODE FOR A
 * SIGNED-CHAR LOAD, AND THUMB'S extendqisi2 IS WHY
 * ================================================================
 *
 * The .Leeb96 loop is `ldrb r3,[r1,#0] / lsl r3,#24 / asr r3,#24`, and reaching
 * it took 161 of 258 down to 4.  gcc-2.96's thumb `extendqisi2` chooses at
 * EXPAND time, off the shape of the MEM address:
 *
 *   (mem (reg))            -> reload manufactures a zero register and emits the
 *                             register-offset `ldrsb rd,[rn,rm]` -- 1 insn in
 *                             the loop plus a hoisted `movs r3,#0`
 *   (plus symbol_ref reg)  -> not an ldrsb-able address, so the three-instruction
 *                             `ldrb` + `lsl #24` + `asr #24` sequence is emitted,
 *                             AND IT SURVIVES strength reduction rewriting the
 *                             address into a single walking register
 *
 * So a WALKING POINTER (`signed char *t = Leeb96; ... *t ...; t++;`) gets ldrsb
 * and an ARRAY INDEX OFF THE SYMBOL (`Leeb96[i]`) gets the ROM's form -- with
 * IDENTICAL final addressing, because loop.c turns the index into the same
 * walking pointer afterwards.  Every arithmetic dodge fails because combine
 * folds it straight back to a sign_extend: `(signed char)*t`, `((int)(*t<<24))>>24`,
 * the same shifts as three separate statements on a local, an `unsigned char`
 * array with a cast, and a `signed int v : 8` bitfield ALL produce ldrsb.
 * `volatile unsigned char` DOES produce the ROM's form (it stops combine
 * changing the MEM's mode) and is worth recording as the diagnostic that
 * identified the pass -- 162 -> 136 -- but it is not the answer and is not used.
 *
 * AND THE INDEX MUST BE USED TWICE OR loop.c DELETES THE COUNTER.  With the
 * index on .Leeb96 only and `q` walking, `maybe_eliminate_biv` replaces
 * `cmp r6,#0x10` with a pointer compare against a precomputed end value and the
 * counter disappears (158 of 258, count still exact).  Indexing BOTH sides --
 * `q[i].x = Leeb96[i] + 0x40;` -- gives the biv two givs to serve, keeps it, and
 * reproduces the ROM's `movs r6,#0 / adds r6,#1 / cmp r6,#16`: 158 -> 4.
 *
 * ================================================================
 * THE OTHER THREE LEVERS, in the order they paid
 * ================================================================
 *
 * `while (i != (*(State **)(base + 0x7828))->f14)` FOR THE Func_80d6888 LOOP,
 * NOT `if (...) { do ... while (...); }`.  Worth 161 -> 14 -- the single
 * largest step here.  The mechanism is the recorded one read the other way
 * round: `duplicate_loop_exit_test` runs AFTER gcse, so with the `while` form
 * gcse sees the address expression ONLY inside the loop and does what the ROM
 * does -- puts `ldr r5,=0x7828 / add r5,r10` in the loop PREHEADER and leaves
 * the guard, manufactured later, to reach the state with the register-offset
 * `mov r0,sl / ldr r3,[r0,r3]`.  With the explicit `if` guard the address exists
 * at gcse time, gcse unifies guard and body into ONE add, the preheader copy
 * collapses to `adds r5,r2,#0`, and the whole stream is one instruction short.
 * ANIM_BREAK NEEDED THE `if` FORM AND THIS LOOP NEEDS `while`: the deciding
 * question is whether the ROM RE-COMPUTES the guard's address or shares it.
 *
 * `i = 0;` BEFORE `q = (Part *)(base + (0xe1 << 7));` -- the "assign the
 * base + K pointer LAST" lever, 4 -> 2, reproducing `movs r6,#0 / add r7,sl`.
 *
 * THE r0 ARGUMENT AS ITS OWN STATEMENT IN A PINNED REGISTER, 2 -> 0:
 *
 *     arg = 0x90;
 *     { register void *tf __asm__("r0");
 *       tf = (void *)Task_BlitAnim;
 *       arg <<= 3;
 *       StartTask(tf, arg); }
 *
 * The ROM emits `ldr r0,=Task_BlitAnim` BEFORE the `lsl r1,#3` that finishes the
 * second argument; gcc's "fill r0 last" puts it after.  A pinned r0 assigned as
 * its own statement ahead of the shift gives that move a LOWER INSN UID, and
 * sched2's ready-list tie-break is insn order -- so the ROM's order comes back.
 * THIS IS A GENERAL sched2 LEVER, not a Vine detail: the identical edit closed
 * BaseAnim_Tackle's @75 window in the same batch (12 of 402 -> 10).
 *
 * ALSO LOAD-BEARING: `pp = tbl; base = *pp++; ctx = *pp;` for the ROM's
 * `ldmia r3!, {r2}` (the recorded iwram_3001eec pointer-table lever);
 * `DrawFn d[2]` as a real two-element LOCAL ARRAY with `&d[0]` naturally kept in
 * r9, called as `d[i & 1](...)` for the ROM's `lsl r4,#2 / ldr r4,[r4,r0]`;
 * `REG_BLDCNT = 0` (0x4000050) and NOT REG_BLDALPHA (0x4000052 in this tree);
 * `a.x = a.x + (b.x - a.x) / 2;` for the ROM's signed halving; and the two
 * write-only int outputs `&p1`/`&p2` passed to Anim_Djinni, which are what make
 * the frame 0x30 rather than 0x28.
 *
 * No .sym entry is warranted.  No per-file Makefile flag override applies.
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
extern signed char Leeb96[] __asm__(".Leeb96");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void GetBattleActorPos2(int id, vec3_t *out);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);
extern void Anim_Djinni(void *context, int a, int b, int c, int *p1, int *p2);

void Anim_Vine(void *context)
{
    vec3_t a;
    vec3_t b;
    int p1;
    int p2;
    DrawFn d[2];
    char **tbl;
    char **pp;
    u8 *base;
    void *ctx;
    State **slot;
    Part *q;
    int i;
    int frame;
    int arg;

    tbl = (char **)iwram_3001eec;
    pp = tbl;
    base = (u8 *)*pp++;
    ctx = (void *)*pp;
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    Anim_Djinni(context, 4, ((State *)context)->f4, 4, &p1, &p2);
    AnimStart(1);
    REG_BG2PA = 0x100;
    REG_BLDCNT = 0;
    LoadVFXFile(FILE_a6, base, 1, 1);
    GetBattleActorPos2((*slot)->ids[0], &a);
    GetBattleActorPos2((*slot)->ids[(*slot)->f14 - 1], &b);
    a.x = a.x + (b.x - a.x) / 2;
    REG_BG2X = (0x40 - a.x) << 8;
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 1);
    d[0] = (DrawFn)tbl[7];
    BuildDraw2DFuncEx(0x2f, 7, 7, 7, 1);
    d[1] = (DrawFn)tbl[8];
    {
        q = (Part *)(base + (0xe1 << 7));
        i = 0;
        do {
            q[i].x = Leeb96[i] + 0x40;
            i++;
        } while (i != 0x10);
    }
    *(int *)(base + (0xef << 7)) = 1;
    *(int *)(base + 0x7784) = 0;
    arg = 0x90;
    {
        register void *tf __asm__("r0");
        tf = (void *)Task_BlitAnim;
        arg <<= 3;
        StartTask(tf, arg);
    }
    frame = 0;
    do {
        if (frame == 0x20) {
            _PlaySound(0x8f);
            i = 0;
            while (i != (*(State **)(base + 0x7828))->f14) {
                Func_80d6888((*(State **)(base + 0x7828))->ids[i], 7, 5, i, 0x10);
                i++;
            }
        }
        i = 0;
        q = (Part *)(base + (0xe1 << 7));
        do {
            if (frame == i * 4 + 5) {
                *(int *)(base + 0x77a8) = 2;
            }
            if (frame > i * 2 + 4) {
                int n = (frame / 4 + i) % 5;
                int h;
                if (frame < i * 2 + 0x20) {
                    h = (frame - i * 2) * 4 - 0x10;
                    if (h > 0x20) {
                        h = 0x20;
                    }
                } else {
                    h = 0xa0 - (frame - i * 2) * 4;
                }
                if (h > 0) {
                    d[i & 1](ctx, base + (n << 10), q->x - 0x10,
                             (i & 7) - h + 0x68, 0x20, h);
                }
            }
            i++;
            q = (Part *)((char *)q + 0x1c);
        } while (i != 0x10);
        UpdateScreenShake(4, 4);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x46);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
