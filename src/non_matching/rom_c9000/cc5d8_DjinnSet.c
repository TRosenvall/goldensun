/* Anim_DjinnSet -- NON-MATCHING, 308 encodings of 384, size 888 against the ROM's 904
 * (-16), 379 instructions against 384.  349 instructions.  FRAME 0x28 -- EXACT.
 * 44 instructions in disagreeing regions of 369, ALL inside the outer frame loop:
 * instructions 0-169 and 344-368 are IDENTICAL.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/cc5d8_DjinnSet.c \
 *     asm/rom_c9000/rom_cc5d8_a_a_a.s --func Anim_DjinnSet
 * TWO functions in the reference; a text split is required.  NO .rodata in that TU --
 * .Lee058, .Lee05c and .Lee060 live in rom_cc5d8_c_c.s and are already covered by
 * stage1.ld:1825, so they are plain externs.
 *
 * Its file-mate src/rom_c9000/rom_cc5d8_a_a_b.c (Anim_UnleashIntro) is a LANDED,
 * byte-exact function in the same idiom -- the closest donor any rom_c9000 target has.
 *
 * BLOCKER CLASS: scheduling and reload-register choice (about 26 of 40 label-blind), plus
 * loop-invariant motion in the `frame <= 0xe` loop (about 6) and constant-synthesis mode
 * (about 2).
 *
 * THREE FINDINGS WORTH KEEPING:
 *
 * A WRITE-THEN-NEVER-READ SPILL STORE MEANS A LOCAL *ARRAY*, and it was worth the whole
 * 0x24 -> 0x28 frame correction here.  That is the same rule BaseAnim_Spore established
 * (src/non_matching/rom_c9000/ca1e4_Spore.c) -- an array element cannot be deleted by
 * flow analysis where a scalar can -- now confirmed on a second function and worth
 * treating as the standard response to a frame four bytes short.
 *
 * HImode NARROWING AT A u16 REGISTER STORE IS A 20-INSTRUCTION LEVER AND SHOULD BE PRICED
 * GLOBALLY, NOT LOCALLY.  It reads as a two-instruction local difference at each site and
 * is worth ten times that in aggregate.
 *
 * STATEMENT ORDER DECIDES `add rd, sp, #imm` AGAINST `mov rd, sp / add rd, #imm`.  Two
 * instructions against one, and it is a source-position question rather than a spelling.
 *
 * No per-file Makefile flag override applies to this stem.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef void (*CopyFn)(volatile u16 *dst, void *src, s32 len);
typedef void (*DrawFn)(void *ctx, void *src, s32 x, s32 y, s32 w, s32 h);

typedef struct {
    int x;
    int y;
    int z;
    int p3;
    int p4;
    int p5;
    int t;
} Part;

extern Part gBuffer[];
extern unsigned char gPtrs[];
extern DrawFn iwram_3001f0c;
extern unsigned short Data_ede5c[];
extern signed char Lee058[] __asm__(".Lee058");
extern signed char Lee05c[] __asm__(".Lee05c");
extern unsigned char Lee060[] __asm__(".Lee060");

extern void *galloc_iwram(s32 tag, s32 size);
extern void gfree(s32 tag);
extern void AnimStart(s32 n);
extern void AnimEnd(void);
extern void LoadVFXFile(s32 id, void *dst, s32 a, s32 b);
extern void Func_8001af8(volatile u16 *dst, void *src, s32 len);
extern int Random(void);
extern void StartTask(void *fn, s32 arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern int BuildDraw2DFuncEx(s32 idx, s32 a, s32 b, s32 c, s32 d);
extern void _PlaySound(s32 id);
extern void GetBattleActorPos2(s32 unit, int *dest);
extern void Func_80d6888(s32 a, s32 b, s32 c, s32 d, s32 e);
extern void Func_80cd52c(void);
extern void WaitFrames(s32 n);
extern int sin(int a);
extern int cos(int a);

void Anim_DjinnSet(void *context)
{
    int pos[3];
    void *buf2;
    DrawFn fns[2];
    u8 *buf3;
    u8 *buf;
    int **slot;
    int *pv;
    int frame;
    int i;
    int id;
    int t;
    int arg;
    int off;
    u8 *data;
    u8 *pt;
    CopyFn copy;

    buf = galloc_iwram(0x27, 0x782c);
    buf2 = galloc_iwram(0x28, 0x80 << 7);
    buf3 = galloc_iwram(0x29, 0x60e);
    slot = (int **)(buf + 0x7828);
    *slot = (int *)context;
    AnimStart(0);
    *(int *)(buf + 0x77b4) = 0x18;
    *(int *)(buf + 0x77b8) = 0;
    REG_BLDALPHA = 0x100c;
    REG_BG2PA = 0x100;
    LoadVFXFile(FILE_45, buf, 1, 0);
    LoadVFXFile(FILE_76, buf3, 0, 0);
    switch (**slot) {
    case 0:
        id = FILE_48;
        break;
    case 1:
        id = FILE_57;
        break;
    case 2:
        id = FILE_47;
        break;
    default:
        id = FILE_46;
        break;
    }
    data = (u8 *)GetFile(id);
    copy = Func_8001af8;
    copy((volatile u16 *)(0xa0 << 19), data, 0x80);
    {
        Part *g = gBuffer;
        i = 0;
        do {
            g->y = 0x80 << 16;
            g->x = Random() & 0xffff;
            g->z = (Random() & 0x1ff) + (0x80 << 3);
            g->t = -i;
            i++;
            g++;
        } while (i != 0x80);
    }
    {
        Part *p;
        i = 0;
        p = (Part *)(buf + (0xe1 << 7));
        do {
            p->x = Random() & 0xffff;
            p->y = (Random() & 0x1f) + 0x10;
            p->t = (i & 0xf) + 0x10;
            i++;
            p++;
        } while (i != 0x40);
    }
    *(int *)(buf + (0xef << 7)) = 2;
    arg = 0x90;
    *(int *)(buf + 0x7784) = 0x4b;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    BuildDraw2DFuncEx(0x2e, 7, 7, 7, 3);
    pt = gPtrs;
    fns[0] = *(DrawFn *)(pt + 0xb8);
    _PlaySound(0x8c);
    frame = 0;
    pv = pos;
    do {
        volatile u32 *io;
        register int *pw __asm__("r5");
        pw = pv;
        GetBattleActorPos2(((int *)context)[2], pv);
        io = (volatile u32 *)REG_ADDR_BG2X;
        *io = (0x40 - pw[0]) << 8;
        if (frame > 0x31) {
            int v = (0x70 - frame * 2) | 0x1000;
            *(volatile u16 *)((char *)io + 0x2a) = v;
        }
        if (frame == 0x1a) {
            _PlaySound(0xd4);
            Func_80d6888(*(short *)((char *)*(void **)(buf + 0x7828) + 0x24), 7, -1, 0, 0x14);
        }
        t = frame - 0x1c;
        if ((u32)t <= 0x14) {
            u8 *dp = buf + (t / 3) * 0x900 + 0x1400;
            fns[0](buf2, dp, 0x28, pv[1] - 0x18, 0x30, 0x30);
        }
        if (frame >= 0 && frame <= 0xe) {
            off = ((frame / 3) % 5) << 10;
            i = 0;
            do {
                DrawFn *pfn = &iwram_3001f0c;
                u8 *dp;
                BuildDraw2DFuncEx(0x2f, 7, 7, Lee060[i] | 3, 2);
                fns[1] = *pfn;
                dp = buf;
                dp += off;
                fns[1](buf2, dp, Lee058[i] + 0x20, pv[1] + Lee05c[i] - 0x20, 0x20, 0x20);
                i++;
                gfree(0x2f);
            } while (i != 4);
        }
        if (frame >= 0) {
            Part *p;
            i = 0;
            p = (Part *)(buf + (0xe1 << 7));
            do {
                if (p->t >= 0 && p->y > 0) {
                    int n = (p->t >> 3) + 1;
                    int x = ((p->y * sin(p->x)) >> 16) + 0x40;
                    int y = ((p->y * cos(p->x)) >> 16) + pv[1];
                    int s;
                    int k;
                    u16 *tp;
                    if (n <= 0) {
                        n = 1;
                    }
                    s = n << 1;
                    tp = Data_ede5c;
                    k = s - 2;
                    fns[0](buf2, buf3 + *(u16 *)((char *)tp + k), x - n, y - n, s, s);
                    p->y -= 2;
                    p->t -= 1;
                }
                i++;
                p++;
            } while (i != 0x40);
        }
        Func_80cd52c();
        *(int *)(buf + 0x7824) = 1;
        frame++;
        WaitFrames(1);
    } while (frame != 0x38);
    gfree(0x2e);
    StopTask(Task_BlitAnim);
    AnimEnd();
    gfree(0x29);
    gfree(0x28);
    gfree(0x27);
}
