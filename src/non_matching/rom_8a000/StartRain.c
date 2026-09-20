/* StartRain -- asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a.s (7 functions).
 *
 * NOT MATCHING: 4 differing of 95 encodings, LENGTH IDENTICAL, relocations identical.
 * Candidate below.
 *
 * THE RESIDUE IS ONE INSTRUCTION, a sched2 hoist at the tail:
 *
 *   rom   add r3, #0x2 / strh r4, [r3] / ldr r0, =Task_Rain / mov r1, #0xc8
 *   ours  mov r1, #0xc8 / add r3, #0x2 / strh r4, [r3] / ldr r0, =Task_Rain
 *
 * AND IT IS PRICED OUT, not unswept. The PRE-schedule RTL order (.20.ce2) is ALREADY the
 * ROM's -- insn 224 (`ldr r0, =Task_Rain`) precedes insn 281 (`mov r1, #0xc8`) -- and sched2
 * hoists the mov because the dependence dump gives 281 priority 67 against 224's and 221's 66:
 * 281 -> 282 -> call is one cycle longer than 224 -> call. rank_for_schedule never reaches the
 * LUID tie-break, so no statement order can decide it.
 *
 * ================ THE LEVER THAT GOT IT HERE, AND IT IS NEW ================
 *
 * TWO POINTER ROLES CAN BE ONE SOURCE VARIABLE. The second allocation's buffer pointer and the
 * per-entry walk pointer had to be the SAME local: `q` as its own `int *` is 46 differing,
 * reusing one variable is 9, reusing the other is 16.
 *
 * Batch 274 recorded "DISTINCT call results want DISTINCT variables". This is the other side of
 * it and was worth 37 differing in one edit. THE TELL is a callee-saved register serving two
 * unrelated roles either side of a `gfree`.
 *
 * Making them one variable is what makes the walk pointer a global allocno holding r6, which
 * forces the shared literal 0 onto call-clobbered r4 and produces the ROM's `sub sp, #8` -- the
 * DMA fill word at sp+4, a caller-save slot at sp+0 -- plus the `str r4, [sp]` / `ldr r4, [sp]`
 * pair.
 *
 * WHICH GIVES A SECOND GENERAL READING: a `sub sp, #N` LARGER THAN THE ADDRESS-TAKEN LOCALS IS A
 * CALLER-SAVE SLOT, not a spill slot, when -fcall-used-r4 is in the flags. `str r4, [sp]` before a
 * call plus `ldr r4, [sp]` after it, with r4 ABSENT from the push list, is caller-save.c, and it
 * means that value's allocno lost every callee-saved register. Read the push list and the frame
 * size TOGETHER before touching spellings.
 *
 * MEASURED: `DMA3_FILL(p, z, ...)` with `z` a named local assigned right after the first
 * galloc_ewram took 9 to 4 -- the fill value then wants a callee-saved register, matching the
 * ROM's r6. DMA3_CLEAR is 9; `z` assigned just before the fill is 8; one `zero` shared by the
 * fill AND the loop AND REG_BLDY is 97.
 *
 * `c1 = 0xfc << 6; REG_BLDCNT = c1;` is REQUIRED -- the bare literal pools as `ldr r3, =0x3f00`,
 * the HImode-store class.
 *
 * INERT at 4: 0xc80 spelled directly, `&Task_Rain`, `unsigned short c1`, declaration-order swaps,
 * a named `vu16 *` for the three BLD registers, StartTask returning unsigned. WORSE: StartTask
 * returning int/void*/undeclared 5; a named `pri` local 5-10; `c1` hoisted before the loop 10;
 * any reordering of the w[0]/w[2] reads relative to the walk 56-57; no `zero`-shaped local at all 46.
 */
#include "dma.h"

struct Ent {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0x1c - 0x18];
    short f1c;
    unsigned char pad1e[0x20 - 0x1e];
};

extern int **iwram_3001e70;
extern unsigned char Data_9ff58[];

extern unsigned char *galloc_ewram(int tag, int size);
extern void Func_8091ff0(int a);
extern void DecompressLZ1(void *src, void *dst);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int size, void *gfx);
extern void gfree(int tag);
extern int _Func_8011f54(int a, int b, int c);
extern void StartTask(void *f, int pri);
extern void Task_Rain(void);

void StartRain(void)
{
    unsigned char *p;
    int *g;
    struct Ent *e;
    unsigned int i;
    int t;
    int *w;
    int x;
    int y;
    int c1;
    int z;
    int c2;

    p = galloc_ewram(0x1d, 0x82 << 3);
    z = 0;
    w = *iwram_3001e70;
    Func_8091ff0(0xaa);
    e = (struct Ent *)(p + 8);
    DMA3_FILL(p, z, 0x82 << 3);
    g = (int *)galloc_ewram(0xe, 0x80 << 3);
    DecompressLZ1(Data_9ff58, g);
    t = AllocSpriteSlot();
    *(int *)p = t;
    *(int *)(p + 4) = UploadSpriteGFX(t, 0xc0 << 2, g);
    gfree(0xe);
    i = 0;
    do {
        g = (int *)e;
        *g++ = 0;
        *g++ = 0x40000400;
        *g = 0xd4 << 8;
        x = w[0];
        y = w[2];
        e->fc = x;
        e->f14 = y;
        e->f10 = _Func_8011f54(0, x >> 16, y >> 16) << 16;
        e->f1c = (i & 0xf) + 1;
        i += 1;
        e = e + 1;
    } while (i <= 0x1f);
    c1 = 0xfc << 6;
    REG_BLDCNT = c1;
    c2 = 0x1008;
    REG_BLDALPHA = c2;
    REG_BLDY = 0;
    StartTask(Task_Rain, 0xc8 << 4);
}
