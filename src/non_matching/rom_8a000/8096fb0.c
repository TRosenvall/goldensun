/* Func_8096fb0 -- NON-MATCHING, 79 encodings of 145 (objcmp: ENCODINGS differ in 79
 * place(s), ref 145 / ours 141).  The .s (asm/rom_8a000/rom_96cdc_c_a_a.s) holds only this
 * function -- whole-file, no split.  The count is inflated by a 4-instruction length
 * shift: tryc shows the first 57 of 139 lines exact, and everything after the area
 * compares is the same code displaced.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_8a000/8096fb0.c asm/rom_8a000/rom_96cdc_c_a_a.s --func Func_8096fb0
 *
 * What is right: the iwram_3001ebc-relative globals (-0x4c for the caster, +0x74 for the
 * reuse case), DMA3_CLEAR (its value store comes out as `str r6,[sp]` off the k==0 path),
 * the HImode 0x200 mid-function pool, and the pooled compares, which are _AREA_35 / _AREA_37
 * from area.sym (gState+0x1da is the current area) -- literals give `cmp #imm`.
 *
 * BLOCKER: the two area compares.  The ROM loads the area ONCE as `ldrh r1` (a zero-extended
 * value -- a promoted short or an unsigned short), compares the first time through a
 * combine-made `ldrsh r2,[r3,r4]` (combine merging the load and the sign extension while
 * keeping the ldrh because the value is still needed), and the second time through
 * `lsl/asr #16` of r1 AFTER the first if's strb.  So at combine time there were TWO separate
 * sign extensions.  Every spelling tried gets ONE: cse1 (-fcse-skip-blocks: the first if's
 * body is a skippable block, its join label has LABEL_NUSES == 1) re-uses the first
 * extension for the second compare, and cse2 would do the same even if cse1 didn't.
 * Downstream this frees a register, so gState lands in r0 instead of the ROM's r12 and the
 * constant 1 in r1 instead of r0.
 *
 * Tried, all still merged (79): `short h`; `unsigned short h` / `int h` from a u16 load with
 * `(short)h` compares; the first compare reading memory directly (cse replaces it through
 * LOAD_EXTEND_OP); `do { } while (0)` between the ifs or inside the first body; `&& k == 0`
 * or `&& one` on the first compare (to give the join label a second use; cse folds the extra
 * jump and cse2 merges anyway).  `*(volatile short *)` on the first compare separates the
 * loads but blocks combine, so no ldrsh (66 by count, 1 longer).  Merging to
 * `if (A35 || A37)` fixes the r0/r1 choice but emits one strb where the ROM has two.
 */
#include "dma.h"
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern unsigned char L9c410[] __asm__(".L9c410");
extern void *galloc_iwram(int tag, int size);
extern unsigned char *_GetMoveInfo(int id);
extern int Func_8096c24(void);
extern void Func_80970f8(int a, int b);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int n, void *src);
extern void StartTask(void *fn, int prio);
extern void Func_8096f8c(void);
extern int _AREA_35;
extern int _AREA_37;

void Func_8096fb0(int kind, int arg)
{
    unsigned char *st;
    unsigned char *src;
    unsigned char *p;
    unsigned char *g;
    unsigned char *info;
    short h;
    int k;
    int slot;

    st = iwram_3001ebc;
    src = *(unsigned char **)((unsigned char *)&iwram_3001ebc - 0x4c);
    k = *(signed char *)(st + 0xcc6);
    if (k == 0) {
        p = galloc_iwram(0x38, 0xe4 << 3);
        DMA3_CLEAR(p, 0x720);
    } else {
        p = *(unsigned char **)((unsigned char *)&iwram_3001ebc + 0x74);
    }
    *(short *)(p + 0x1c) = kind;
    info = _GetMoveInfo(kind);
    *(short *)(p + 0x1e) = info[0xc];
    k = *(signed char *)(st + 0xcc6);
    if (k != 0)
        return;
    *(short *)(p + 0x4a) = 0x200 - Func_8096c24();
    *(p + 0x21) = arg;
    *(p + 0x22) = 1;
    *(p + 0x20) = 1;
    *(p + 0x23) = 1;
    *(p + 0x71c) = 1;
    *(int *)(p + 0x4c) = *(int *)(src + 4);
    *(int *)(p + 0x50) = *(int *)(src + 8);
    *(int *)(p + 0x54) = *(int *)(src + 0xc);
    g = gState;
    h = *(short *)(g + 0x1da);
    if (h == (int)&_AREA_35)
        *(p + 0x45) = 1;
    if (h == (int)&_AREA_37)
        *(p + 0x45) = 1;
    Func_80970f8(*(int *)(g + 0x1f4), -1);
    if (*(short *)(p + 0x1e) != 8)
        *(short *)(st + 0xcc0) = k;
    slot = AllocSpriteSlot();
    *(short *)(p + 0x46) = slot;
    UploadSpriteGFX((short)slot, 0x100, L9c410);
    StartTask(Func_8096f8c, 0xc80);
}
