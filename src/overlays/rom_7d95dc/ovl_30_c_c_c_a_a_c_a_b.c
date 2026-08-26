/* OvlFunc_953_200960c  --  0x0200960c, cut from
 * goldensun/asm/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_c_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_c_a_a.o and
 * asm/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_c_a_c.o in
 * goldensun/overlays/rom_7d95dc/overlay.ld.
 *
 * A cutscene script: arm the next map, fade in, wait, speak, and on a save bit
 * bump a counter before finishing.
 *
 * A NAMED LOCAL USED ONCE COSTS THE PREFERRED REGISTER. Written as
 *
 *     base = *(char **)iwram_3001ebc;
 *     *(int *)(base + (0xe0 << 1)) = 0x201;
 *
 * the whole first block comes out as a clean r2/r3 transposition -- six lines
 * of 43, with the ROM holding the base in r3 and the offset in r2 and ours the
 * other way round. REG_ALLOC_ORDER (arm.h:989) starts {3, 2, 1, 0, ...}, so r3
 * goes to whichever pseudo the allocator ranks first, and giving the base its
 * own named local raises the constant's rank instead. Inlining the dereference,
 *
 *     *(int *)(*(char **)iwram_3001ebc + (0xe0 << 1)) = 0x201;
 *
 * puts them back the ROM's way round and the function matches.
 *
 * Four other spellings of the same store -- operand order swapped, the
 * destination named as an `int *`, the offset named, the offset written as the
 * folded literal 0x1c0, an `int *` base indexed [0x70] -- all give the same six
 * differences, and so do -fno-gcse, -fno-rerun-cse-after-loop,
 * -fno-schedule-insns2 and -O1. Only removing the local moves it.
 *
 * The SECOND read of the global keeps its local, because there the ROM does the
 * same thing: it holds `&iwram_3001ebc` in r5 across every call and re-loads
 * the pointer, which is two separate reads of a global rather than one cached
 * value.
 *
 * `mov r2, #0xe0 / lsl r2, #1 / add r3, r2 / add r2, #0x41` is gcc noticing
 * that the stored value 0x201 is the offset 0x1c0 plus 0x41 and building it
 * from the register it already has. Both constants are written as plain
 * literals; nothing in the source suggests the relationship.
 */
extern unsigned char iwram_3001ebc[];
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void OvlFunc_953_2009c5c(int slot, int v);
extern void OvlFunc_953_2009c48(int slot);

void OvlFunc_953_200960c(void)
{
    char *base;

    __CutsceneStart();
    *(int *)(*(char **)iwram_3001ebc + (0xe0 << 1)) = 0x201;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    OvlFunc_953_2009c5c(0x11, 0xa0 << 7);
    __MessageID(0x206e);
    if (__GetFlag(0x8a4)) {
        base = *(char **)iwram_3001ebc;
        (*(unsigned short *)(base + (0xec << 1)))++;
    }
    OvlFunc_953_2009c48(0x11);
    OvlFunc_953_2009c5c(0x11, 0xc0 << 6);
    __SetFlag(0x8a3);
    __CutsceneEnd();
}
