/* LoadPortrait -- NON-MATCHING, 12 encodings of 71 against the tree reference.
 * SIZE EXACT (164 bytes both), INSTRUCTION/ENCODING COUNT EXACT (71 = 71), and all
 * SEVEN RELOCATIONS IDENTICAL (objcmp prints no RELOCATIONS line).  NO SHIMS.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_15000/801a4fc.c \
 *     asm/rom_15000/rom_19ebc_a_c_c_c_c_c.s
 * ONE function, no data sections -- converts whole when it lands, no split.
 *
 * ALL TWELVE DIFFERENCES ARE ONE SWAP: r5 <-> r6.  The ROM keeps the reused `id`
 * parameter (which becomes the source pointer) in r5 and the `Blk *` in r6; gcc
 * reverses them.  Nothing else in the function differs -- every instruction, in
 * order, is otherwise the ROM's.
 *
 * LEVER THAT GOT IT HERE (worth copying): REUSE THE PARAMETER AS THE DERIVED
 * POINTER.  The ROM keeps `mov r3, r5` before `cmp r5,#0x7f / bls / sub r3,#0x70`
 * -- a copy gcc coalesces away in every two-variable spelling, because after the
 * compare `id` is dead.  Seven spellings of the adjustment were measured and ALL
 * of them coalesce (55 of 71, one instruction short):
 *
 *   idx = id; if (id > 0x7f) idx = id - 0x70;      sub r5,#0x70 in place
 *   idx = id; if (id > 0x7f) idx -= 0x70;          same
 *   idx = id; if (idx > 0x7f) idx -= 0x70;         same
 *   idx = (id > 0x7f) ? id - 0x70 : id;            same
 *   idx = id; if (id >= 0x80) idx = id - 0x70;     same
 *   idx = id; if (!(id <= 0x7f)) idx -= 0x70;      same
 *   two-armed else                                 +2 instructions (73), copy doubled
 *
 * What works is ASSIGNING THE LATER POINTER BACK INTO THE PARAMETER
 * (`id = (unsigned int)file + *(unsigned short *)(file + idx * 2);`).  That gives
 * the parameter's pseudo ONE live range spanning the whole function, so it conflicts
 * with `idx` at the `sub` and the copy can no longer be coalesced.  55 -> 12, and it
 * also fixed the prologue's `mov r8,r3 / mov r7,r2` order for free.
 *
 * This is the same shape as docs/elevation.md's "a DERIVED initialiser forces the
 * pointer copy gcc coalesces away", reached from the other end: not a derived
 * initialiser for the SECOND name, but a later redefinition of the FIRST.
 *
 * BLOCKER: register allocation (global.c allocno priority).  The two allocnos are
 *   id/src  6 refs, live entry..end  (~54 insns)
 *   b       5 refs, live galloc..UploadSpriteGFX (~34 insns)
 * and priority ~ floor_log2(n_refs) * n_refs / live_length makes `b` win, so gcc
 * hands it r5 (REG_ALLOC_ORDER gives 4,5,6,7,8,10,9,11).  The ROM ranks id/src
 * first.  Measured inert: three local declaration orders, signed `id`, the table
 * access as `((unsigned short *)file)[idx]`, a `raw` pointer for the three field
 * stores (that one costs +3), `idx = id - 0x70` vs `idx -= 0x70` under the reuse.
 * This is the REG_ALLOC_ORDER class HANDOFF.md names.
 *
 * The Blk struct and its field order are the sibling's -- see
 * src/rom_15000/rom_19ebc_a_c_c_c_c_a.c and ..._c_c_b.c.
 */
#include "dma.h"

typedef struct {
    unsigned char pad[0x600];
    short f600;
    short f602;
    int f604;
} Blk;

extern Blk *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern unsigned char *GetFile(int id);
extern void LoadIcon(Blk *b, int n);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int size, void *gfx);
extern int _FILE_f0;

void LoadPortrait(unsigned int id, int a1, int *p2, int *p3, int a4, int a5)
{
    Blk *b;
    unsigned char *file;
    unsigned int idx;

    b = galloc_iwram(0x11, 0x608);
    file = GetFile((int)&_FILE_f0);
    idx = id;
    if (id > 0x7f)
        idx -= 0x70;
    id = (unsigned int)file + *(unsigned short *)(file + idx * 2);
    b->f604 = (int)(id + 0x20);
    b->f600 = 4;
    b->f602 = 4;
    LoadIcon(b, 0);
    if (a5 == 0)
        *p2 = AllocSpriteSlot();
    *p3 = UploadSpriteGFX(*p2, 0x200, (unsigned char *)b + 0x400);
    gfree(0x11);
    DMA3_COPY16((void *)id, (void *)(0x5000200 + (a4 << 5)), 0x40);
}
