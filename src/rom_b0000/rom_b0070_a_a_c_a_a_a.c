/* Cluster Func_80b010c..Func_80b010c extracted from
 * goldensun/asm/rom_b0000/rom_b0070_a_a_c_a_a_a.s.
 *
 * OpenShopState: allocate the module's 0xa70-byte block, DMA-clear it, reserve
 * six OBJ tile slots and register the per-frame task.  Matched on the first
 * screen; the only things that needed looking up were already in the tree.
 *
 *   THE DMA BLOCK IS DMA3_CLEAR FROM include/dma.h.  `mov r3, #0 / mov r0, sp /
 *   str r3, [r0] / mov r1, r5 / ldr r3, =REG_DMA3SAD / ldr r2, =0x8500029c /
 *   stmia r3!, {r0, r1, r2} / sub r3, #0xc` is that inline's expansion exactly,
 *   including the dead `sub` that restores the base.  The control word carries
 *   the size: 0x85000000 | (0xa70 / 4) = 0x8500029c, and 0xa70 is the same
 *   number the allocation asks for.
 *
 *   THE SPRITE TABLES ARE `.LN` EXTERNS AND THESE ONES ARE SAFE.  gcc numbers
 *   its own local labels .L1, .L2, ... from one, so a short name like .L3 gets
 *   captured (see the _TBL_L aliases in the overlay linker scripts).  These are
 *   .Lb3940 and friends -- five hex digits, well past anything gcc will emit for
 *   a function this size -- so the plain `__asm__(".Lb3940")` extern is fine.
 *   They are `.global` in asm/rom_b0000/rom_b0070_c_c_c.s.
 *
 *   EACH SLOT NEEDS A TEMPORARY.  The ROM stores the AllocSpriteSlot result to
 *   the state block and then passes THE SAME REGISTER to UploadSpriteGFX; the
 *   store is a halfword, so reading it back would need an `ldrh` the ROM does
 *   not have.  `t = AllocSpriteSlot(); *(short *)(p + K) = t;
 *   UploadSpriteGFX(t, ...)` is what keeps it in r0.
 *
 * The six field offsets are written as plain literals and gcc picks how to
 * build each: 0x3a8, 0x390, 0x394 and 0x398 factor as imm8 << 2 and come out
 * `mov`/`lsl`, while 0x36e, 0x3a7, 0x392, 0x396 and 0x39a do not and are pooled.
 * That split is the compiler's, not a spelling choice.
 */
#include "dma.h"

extern unsigned char Lb3940[] __asm__(".Lb3940");
extern unsigned char Lb39c0[] __asm__(".Lb39c0");
extern unsigned char Lb3a40[] __asm__(".Lb3a40");
extern unsigned char Lb3ac0[] __asm__(".Lb3ac0");
extern unsigned char Lb3b40[] __asm__(".Lb3b40");
extern unsigned char Lb3bc0[] __asm__(".Lb3bc0");

extern unsigned char *galloc_iwram(int tag, int size);
extern void _Func_808e118(void);
extern int _Func_80796c4(unsigned char *p);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int size, unsigned char *gfx);
extern int StartTask(void *f, int prio);
extern void Func_80b00f4(void);

void Func_80b010c(void)
{
    unsigned char *p;
    int t;

    p = galloc_iwram(0x37, 0xa7 << 4);
    _Func_808e118();
    DMA3_CLEAR(p, 0xa7 << 4);
    p[0xea << 2] = 0xc;
    p[0x3a7] = _Func_80796c4(p + 0x36e);
    t = AllocSpriteSlot();
    *(short *)(p + (0xe4 << 2)) = t;
    UploadSpriteGFX(t, 0x80, Lb3940);
    t = AllocSpriteSlot();
    *(short *)(p + 0x392) = t;
    UploadSpriteGFX(t, 0x80, Lb3b40);
    t = AllocSpriteSlot();
    *(short *)(p + (0xe5 << 2)) = t;
    UploadSpriteGFX(t, 0x80, Lb3bc0);
    t = AllocSpriteSlot();
    *(short *)(p + 0x396) = t;
    UploadSpriteGFX(t, 0x80, Lb39c0);
    t = AllocSpriteSlot();
    *(short *)(p + 0x39a) = t;
    UploadSpriteGFX(t, 0x80, Lb3a40);
    t = AllocSpriteSlot();
    *(short *)(p + (0xe6 << 2)) = t;
    UploadSpriteGFX(t, 0x80, Lb3ac0);
    StartTask(Func_80b00f4, 0xc8 << 4);
}
