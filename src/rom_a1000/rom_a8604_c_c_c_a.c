/* Cluster Func_80a9e48..Func_80a9e48 extracted from goldensun/asm/rom_a1000/rom_a8604_c_c_c.s.
 *
 * Total .text for this TU = 196 bytes (= 0xc4).
 * Preserves the original ROM layout when slotted between
 * asm/rom_a1000/rom_a8604_c_c_b.o and asm/rom_a1000/rom_a8604_c_c_c_b.o in
 * goldensun/stage1.ld.
 *
 * Never attempted before batch 274. EXACT ON THE FIRST SCREEN, no pins, no flags.
 *
 * Uses a held item: look up the unit's slot, run the item's effect, and if it resolved,
 * consume a consumable or downgrade a one-shot.
 *
 * IT MATCHED FIRST TIME BECAUSE THE FIELD IS DEREFERENCED TWICE, NOT CACHED, and that
 * is the reusable part. The ROM reads `info[0xc]` twice -- once against 1 and once
 * against 4 -- and each read carries a redundant `mov r3, r2` before its `cmp`:
 *
 *     ldrb r2, [r6, #0xc] / mov r3, r2 / cmp r3, #1     ... and again for #4
 *
 * A cached `kind = info[0xc];` collapses that to one load and no copy. TWO TEXTUAL
 * DEREFERENCES of the same field is what produces the ROM's shape.
 *
 * The same tell, measured on this function's file-mate Func_80a5614, is worth 54
 * instructions: `v = *p; if (v != 0) f((v & 0x1ff) + ...)` is 55 differing and
 * `if (*p != 0) f((*p & 0x1ff) + ...)` is 1. Named-temp, separate-mask, operand-order
 * and `unsigned short`-temp spellings all measured identically at 55.
 *
 * SO: a redundant `mov rX, rY` sitting between a load and its `cmp` is a DOUBLE
 * DEREFERENCE in the source, not a scheduling artefact and not something a pin should
 * be spent on. The instinct to cache a repeated field read is exactly wrong here.
 */
extern unsigned char *iwram_3001f2c;
extern unsigned char *_GetUnit(int id);
extern unsigned char *_GetItemInfo(int id);
extern int Func_80a9f10(int a, int b, int c, int d);
extern int _Func_80788c4(int a, int b);
extern int Func_80a3ddc(unsigned char *unit, unsigned short *p, int f);

int Func_80a9e48(int slot, int user, int target)
{
    unsigned char *state;
    unsigned char *unit;
    unsigned char *info;
    int off;
    int id;
    int ret;

    unit = _GetUnit(user);
    state = iwram_3001f2c;
    off = slot * 2 + 0xd8;
    id = *(unsigned short *)(unit + off) & 0x1ff;
    info = _GetItemInfo(id);
    ret = Func_80a9f10(*(unsigned short *)(info + 0x28) & 0x3fff, user, target, 1);
    if (ret != -1) {
        info = _GetItemInfo(*(unsigned short *)(unit + off));
        if (info[0xc] == 1) {
            _Func_80788c4(user, slot);
            state[0x86 * 4] = Func_80a3ddc(unit, (unsigned short *)(state + 0xe4 * 2), 0);
        }
        if (info[0xc] == 4) {
            if (id == 0xb8)
                id = 0xb9;
            *(unsigned short *)(unit + off) = id;
        }
        ret = 0;
    }
    return ret;
}
