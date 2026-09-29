/* LoadPortrait (0x08019ebc) -- 71 encodings, 164 bytes, exact.
 *
 * FAKEMATCH: the `__asm__("" : : "r" (id))` below emits no instruction, but the
 * match depends on it and there is no C spelling that replaces it, so the file
 * is booked in fakematch.txt.
 *
 * It works by manufacturing a REFERENCE.  global.c:598's allocno_compare ranks
 * by floor_log2(n_refs) * n_refs / live_length, and the dumps give id 7 refs /
 * 47 insns against b's 6 / 29, so b is allocated first and takes r5 while id
 * takes r6.  The barrier moves id to 9 / 48 -- 0.56 against 0.41 -- which
 * reorders the allocation and closes all twelve differences at once.
 *
 * The pin-free route is structurally closed, which is why this is a fakematch
 * and not an unfinished function: the four thresholds are n_refs(id) >= 8,
 * live_length(id) <= 33, n_refs(b) <= 4 and live_length(b) >= 41; every one of
 * id's 7 and b's 6 references already IS an operand of the ROM's own 71
 * instructions, and both live ranges are pinned at both ends by the ROM.  The
 * only free lever is a reference that costs no instruction, which C does not
 * have.  regs_may_share, the one route that merges two pseudos into one allocno
 * and sums their n_refs, is written only at loop.c:1832 and LoadPortrait has no
 * loop.  Exact also with "+r" and at two other placements (4 and 2 elsewhere).
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
    __asm__("" : : "r" (id));
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
