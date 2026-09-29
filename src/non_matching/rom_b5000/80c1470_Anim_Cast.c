/* Anim_Cast (0x080c1470) -- NON-MATCHING, 93 encodings of 267 differ.
 * 234 ROM instructions.  Frontier target: neither this project nor the parallel
 * decompilation had attempted it (docs/parallel-coverage.tsv).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_b5000/80c1470_Anim_Cast.c asm/rom_b5000/rom_c10e8_a_a_a_c.s \
 *       --func Anim_Cast
 *
 * 93 IS NOT A DISTANCE.  Size 600 against 608 and count 263 against 267, so both
 * disagree and the figure is pool-offset inflation.  The navigation figure is
 * `tryc --align`: 18 instructions in disagreeing regions of 256, with every loop,
 * the frame, the prologue and the epilogue already exact.
 *
 * 158 -> 18 came from five things, largest first:
 *   - an UNREFERENCED `s32 v[7]`, reproducing the ROM's user-less `sub sp,#0x24`
 *     (158 -> 89, by far the biggest step)
 *   - `pop {r1} / bx r1` in the reference means the function RETURNS A VALUE
 *   - declaring three callees int-returning so gcc fills r0 last, which closed
 *     three transpositions at once
 *   - one shared loop counter, where the ROM reuses r9
 *   - 0x100 routed through an `s32` local rather than written at the `vu16`
 *
 * BLOCKER, named by the pass: `.14.ce` -- ifcvt's `find_if_case_2` speculates
 * case 1's single-insn switch arm above the `cmp #1` branch and deletes
 * code_label 493.  Traced insn 496 across every `-da` dump: present through
 * `.13.combine`, gone at `.14.ce`.  It accounts for 6 of the 18 (4 the arm
 * itself, 2 the `mov r5,r0` / `mov r0,r5` collateral).  gcc-2.96 has NO
 * `-fno-if-conversion` -- cc1 rejects it -- so this class has no flag escape,
 * which is the if_convert blocker class already recorded in docs/elevation.md.
 * Three source spellings measured, all identical at 18.
 *
 * SHIMS: 8 register pins (shimcount.py) -- 2 from the in-tree call_via inline,
 * 3 the published Func_8001af8 PIN block, and 3 load-bearing (`base`=r11,
 * `m`=r8, `data`=r5).  A landing needs a fakematch.txt row.
 *
 * SPLIT: none needed.  Anchored grep gives exactly ONE function in the
 * reference, and datacheck.py reports nothing -- no data section, no exports.
 *
 * SYMBOL PROPOSAL, WITHHELD pending the owner: `_FILE_c9` is absent from both
 * include/file_table.h and file_table.sym, while _FILE_c8, _FILE_ca and
 * _FILE_cb all exist.  That absence is why the ROM's `ldr r6, =0xc9`
 * disassembles as a raw literal even though 0xc9 fits an imm8 -- and by the
 * batch-297 audit's corpus check (zero SImode pool loads of a plain 1-255
 * constant across all 4,342 generated .s; all 93 such loads are ldrh) a pooled
 * sub-256 value IS the tell that the original named it.  So the evidence here is
 * the strong verified class, not positional.  By the c8/ca/cb rows the name is
 * FILE_PAL_CAST_MERCURY and `element` is the elemental index.  Two rows would be
 * needed.  It is recorded rather than added because it does not complete this
 * function either way, and the resulting objcmp relocation difference would be
 * benign: .sym ids are absolute, so a zero-addend R_ARM_ABS32 links to the same
 * word.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"
#include "file_table.h"

extern int _FILE_c9;
#define FILE_c9 ((int)&_FILE_c9)

typedef int (*MulFn)(int, int);
typedef int (*ClearFn)(void *dst, s32 len);
typedef void (*CopyFn)(volatile u16 *dst, void *src, s32 len);

extern int Func_8000888(int, int);
extern int Func_80008d4(void *dst, s32 len);
extern void Func_8001af8(volatile u16 *dst, void *src, s32 len);
extern void *galloc_ewram(s32 tag, s32 size);
extern void *galloc_iwram(s32 tag, s32 size);
extern int Random(void);
extern int sin(int a);
extern int cos(int a);
extern int DecompressLZ(void *src, void *dst);
extern int _BuildDraw2DFuncEx(s32 idx, s32 a, s32 b, s32 c, s32 d);
extern s32 StartTask(void *fn, s32 arg);
extern void Func_80c11ec(void);
extern void Task_BlitPreAnim(void);
extern u8 iwram_3001f00[];
extern u8 gPtrs[];

static inline int call_via(int (*f)(int, int), int a, int b)
{
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\t%1"
        : "=r" (_a)
        : "r" (f), "0" (_a), "r" (_b)
        : "memory", "lr", "r12"
    );
    return _a;
}

typedef struct {
    s32 vx;
    s32 vy;
    s32 f8;
    s32 fc;
    s32 f10;
    s32 f14;
    s32 f18;
} Spark;

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fc;
    s32 f10;
} Ring;

s32 Anim_Cast(s32 element)
{
    register u8 *base __asm__("r11");
    s32 *flag;
    register u8 *data __asm__("r5");
    CopyFn copy;
    s32 id;
    s32 pal;
    s32 arg;
    s32 n;
    s32 idm;
    s32 v[7];

    flag = *(s32 **)iwram_3001f00;
    flag[2] = 1;
    base = galloc_ewram(0x27, 0x13d0);
    galloc_iwram(0x28, 0x80 << 7);

    {
        MulFn h;
        Spark *p;

        h = Func_8000888;
        p = (Spark *)(base + (0x8e << 5));
        n = 0xf;
        do {
            s32 ang;
            u32 t;
            u32 mag;

            ang = Random();
            t = Random() + 0x10000;
            mag = t >> 1;
            p->vx = call_via(h, cos(ang), mag);
            p->vy = call_via(h, sin(ang), mag);
            if (p->vx & 1) {
                p->vx = -p->vx;
            }
            if (p->vy & 1) {
                p->vy = -p->vy;
            }
            p->f8 = (u32)(Random() + 0x8000) >> 2;
            p->fc = (-p->vx >> 7) + (p->vy >> 8);
            p->f10 = (-p->vy >> 7) + (-p->vx >> 8);
            p->f14 = 0;
            p->f18 = (t >> 13) + 1;
            n--;
            p++;
        } while (n >= 0);
    }

    {
        MulFn h;
        Ring *q;
        s32 ang;
        register s32 m __asm__("r8");

        m = 0x80 << 5;
        h = Func_8000888;
        ang = 0;
        q = (Ring *)(base + (0x9c << 5));
        n = 2;
        do {
            q->f0 = call_via(h, cos(ang), m);
            q->f4 = call_via(h, sin(ang), m);
            q->f8 = call_via(h, cos(ang), 0x80 << 2);
            q->fc = call_via(h, sin(ang), 0x80 << 2);
            q->f10 = 0;
            ang += 0x5555;
            n--;
            q++;
        } while (n >= 0);
    }

    *(s32 *)(base + 0x13bc) = 0;
    *(s32 *)(base + (0x9e << 5)) = 0;
    *(s32 *)(base + 0x13cc) = 0;

    {
        u8 *g;
        void *d;
        ClearFn clr;
        s32 off;

        g = gPtrs;
        off = 0xa0;
        g += off;
        d = *(void **)g;
        clr = Func_80008d4;
        clr(d, 0x80 << 7);
    }

    id = FILE_c9;
    data = GetFile(id);
    {
        register u32 p0 __asm__("r0");
        register s32 p1 __asm__("r1");
        register s32 p2 __asm__("r2");
        p0 = 0xa0;
        p1 = (s32)data;
        copy = Func_8001af8;
        p2 = 0x80;
        p0 <<= 19;
        data += 0x80;
        copy((volatile u16 *)p0, (void *)p1, p2);
    }
    DecompressLZ(data, base);

    switch (element) {
    case 0:
        pal = FILE_c8;
        break;
    case 1:
        pal = id;
        break;
    case 2:
        pal = FILE_ca;
        break;
    default:
        pal = FILE_cb;
        break;
    }
    data = GetFile(pal);
    DMA3_SET(data, (void *)(0xa0 << 19), 0x84000020);
    REG_BG2X = 0;
    REG_BG2Y = 0;
    idm = 0x100;
    REG_BG2PA = idm;
    REG_BG2PB = 0;
    REG_BG2PC = 0;
    REG_BG2PD = idm;
    _BuildDraw2DFuncEx(0x2e, 7, 7, 3, 3);
    arg = 0xc8;
    arg <<= 4;
    _BuildDraw2DFuncEx(0x2f, 7, 7, 3, 2);
    StartTask(Func_80c11ec, arg);
    return StartTask(Task_BlitPreAnim, arg);
}
