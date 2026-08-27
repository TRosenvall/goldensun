/* Func_801ebd8  --  0x0801ebd8
 *
 * The whole of goldensun/asm/rom_15000/rom_1de5c_c_c_c_a_a_c_a.s, which held
 * this function and nothing else, so the linker script's existing line for that
 * object now picks up this file's.
 *
 * Allocates a sprite slot, loads an old-style UI icon into it, and builds the
 * sprite -- returning null if no slot was free. `0x60` is the failure value
 * AllocSpriteSlot returns, not a count.
 *
 * The slot is passed to LoadOldUIIcon BY ADDRESS and then read back off the
 * stack for the next call (`add r2, sp, #8` ... `ldr r0, [sp, #8]`), so the
 * icon loader can move it. The second out-parameter at sp+4 is written and
 * never read, which is why it looks unused below -- the ROM reserves the slot
 * either way.
 *
 * Matched on the first screen.
 */
struct Y { unsigned char pad00[0xf]; unsigned char ff; };

extern int AllocSpriteSlot(void);
extern void LoadOldUIIcon(int a, int b, int *slot, int *out, int e);
extern struct Y *Func_801eadc(int slot, int m, int b, int c, int d);

struct Y *Func_801ebd8(int a, int b, int c, int d)
{
    int slot;
    int out;
    struct Y *p;

    slot = AllocSpriteSlot();
    if (slot == 0x60)
        return 0;
    LoadOldUIIcon(a, 1, &slot, &out, 1);
    p = Func_801eadc(slot, 0x80 << 23, b, c, d);
    p->ff = 0xfb;
    return p;
}
