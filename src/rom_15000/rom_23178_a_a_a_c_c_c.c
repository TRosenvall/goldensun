/* Cluster DataTransferMenu..DataTransferMenu extracted from
 * goldensun/asm/rom_15000/rom_23178_a_a_a_c_c.s.
 *
 * Never attempted before batch 279. No pins, no volatile, no flags, no split.
 * Requires _MSG_c76 -- and that entry rests on a NEW FORM OF THE SYMBOL TELL, which is the reason
 * to read this file. See message.sym for the full decode.
 *
 * A DUPLICATE POOL WORD IS A SYMBOL TELL. This function's pool is SEVEN words holding SIX distinct
 * values, and the repeated one is 0xc76 -- verified by decoding baserom.gba directly rather than
 * taken on report. Eleven `ldr rN,[pc,#imm]` references resolve to those seven words, so every
 * other value is deduplicated across its references. `force_const_mem` deduplicates SImode
 * const_int per function, so TWO WORDS FOR ONE VALUE MEANS TWO DIFFERENT rtx OBJECTS -- one a
 * const_int, one a SYMBOL_REF.
 *
 * That is a structural impossibility argument of the same kind as the `byte << n` one, and it
 * reaches a case that one cannot: 0xc76 is UNSHIFTABLE, so a pooled literal and a pooled symbol
 * look identical at the instruction. tools/pool.py and its relatives key on shiftability and would
 * never flag it.
 *
 * WHICH OF THE TWO IS THE SYMBOL IS SETTLED INDEPENDENTLY: the two StartTask priority sites share
 * one word, and src/rom_15000/rom_23178_a_a_a_a_c_b.c already lands `StartTask(Func_8028194,
 * 0xc76)` as a PLAIN LITERAL. So the DrawSmallText id is the symbol.
 *
 * THREE MORE LEVERS, all from source:
 *   * `StartTask` LEFT UNDECLARED. The file-mate rom_23178_a_a_a_a_c_b.c documents it -- declaring
 *     it moves r0 ahead of r1 -- and with the prototype `ldr r0, =<task>` came one insn early,
 *     exactly as that note predicts.
 *   * `*(unsigned short *)(g + 0x96) = c;` WITH A NAMED `int c`, not a bare 0xffff. Bare gives a
 *     HImode pool entry at pool_range 64, which forces two mid-function minipool dumps and a `b`
 *     over each -- five extra lines. And `*(short *)` gives `.word 0xffffffff`, since the sign
 *     follows the C type.
 *   * A SEPARATE `int` PER ARM rather than one shared variable. One variable set in both arms is a
 *     GLOBAL allocno (`.17.lreg`: "used 4 times across 24 insns; set 2 times; user var") and loses
 *     r2 to a block-local address temp; two per-arm variables are block-local and the r2/r3 pair
 *     comes out the ROM's way. Eight of the thirteen residual instructions.
 *
 * MEASURED AND REJECTED on the 0xc76 CSE before reaching the symbol: -fno-rerun-cse-after-loop,
 * -fno-gcse and -fno-cse-follow-jumps (13 differing each, unchanged); separate locals per use site,
 * both at the arm top and immediately before the use -- this is the recorded MEASURED LIMIT of that
 * lever, "does not work when the uses are far apart", and here they are ~18 insns and two calls
 * apart; StartTask prototyped with short and unsigned short priorities (23 each); and a five-way
 * isolated probe in which int, short, unsigned short locals and a (short) cast all CSE identically.
 */
extern unsigned char *iwram_3001f38;
extern void Func_80284dc(void);
extern void AddMenuBarOption(int n);
extern void Func_8028808(int y, int w, int h);
extern void StopTask(void *task);
extern void *CreateUIBox(int a, int b, int c, int d, int e);
extern void DrawSmallText(int id, void *w, int x, int y);
extern int Func_8028574(int n);
extern void Func_8016478(void *w);
extern void CloseUIBox(void *box, int mode);
extern void WaitFrames(int n);
extern void Func_802851c(void);
extern void Func_8028aa8(void);
extern void Func_8028b80(void);
#include "message.h"

int DataTransferMenu(int mode)
{
    unsigned char *g;
    void **p;
    void *w;
    int m;
    int v;
    int c1;
    int c2;

    Func_80284dc();
    g = iwram_3001f38;
    if (mode == 0) {
        AddMenuBarOption(0x2c);
        AddMenuBarOption(0x2d);
    } else {
        AddMenuBarOption(0x2e);
        AddMenuBarOption(0x2f);
        AddMenuBarOption(0x30);
    }
    Func_8028808(0x11, 7, 0);
    if (mode != 0) {
        StartTask(Func_8028aa8, 0xc76);
        c1 = 0xffff;
        *(unsigned short *)(g + 0x96) = c1;
        w = CreateUIBox(7, 0, 0x11, 4, 2);
        p = (void **)(g + 0x80);
        *p = w;
        m = 0xc77;
        DrawSmallText(m, w, 0, 4);
        w = CreateUIBox(3, 4, 0x19, 0xc, 2);
        *(void **)(g + 0x7c) = w;
        DrawSmallText(m + 1, w, 8, 0);
        DrawSmallText(m + 2, *(void **)(g + 0x7c), 8, 0xb);
        m += 3;
        DrawSmallText(m, *(void **)(g + 0x7c), 8, 0x16);
    } else {
        StartTask(Func_8028b80, 0xc76);
        c2 = 0xffff;
        *(unsigned short *)(g + 0x96) = c2;
        w = CreateUIBox(6, 0, 0x12, 4, 2);
        p = (void **)(g + 0x80);
        *p = w;
        DrawSmallText((int)&_MSG_c76, w, 2, 4);
        *(void **)(g + 0x7c) = CreateUIBox(1, 5, 0x1c, 7, 2);
    }
    v = Func_8028574(0);
    if (mode != 0) {
        StopTask(Func_8028aa8);
    } else {
        StopTask(Func_8028b80);
    }
    Func_8016478(*p);
    Func_8016478(*(void **)(g + 0x7c));
    CloseUIBox(*p, 2);
    CloseUIBox(*(void **)(g + 0x7c), 2);
    WaitFrames(1);
    Func_802851c();
    return v;
}
