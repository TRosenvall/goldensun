/* OvlFunc_898_2008cfc  --  0x02008cfc
 * OvlFunc_898_2008d78  --  0x02008d78
 *
 * The whole of goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_a_c_a.s, which
 * held these two functions and nothing else, so the linker script's existing
 * line for that object now picks up this file's.
 *
 * The cutscene bookend: set the lock bit in the actor's flag halfword at +0x64,
 * speak, and clear the halfword down to bit 0 on the way out.
 *
 * THE POOLED 2 IS A SYMBOL, and this pair is where that was settled. The ROM
 * writes
 *
 *     ldrh r2, [r0] / ldr r3, =2 / orr r3, r2 / strh r3, [r0]
 *
 * and gcc never pools what `mov r3, #2` can build. Twelve literal spellings were
 * measured across the four functions in this family, and they split cleanly:
 *
 *     `*p |= 2` and every commutative rearrangement of it
 *         -> the POOL, with `orr r3, r2` the other way round (2 of 32)
 *     a named `unsigned short two = 2` used as `*p = two | *p`
 *         -> the ROM's operand order, with `mov r3, #2` (1 of 32)
 *     the same local taken from a SYMBOL address
 *         -> both, exactly
 *
 * The internal control is unusually good and is the reason this is filed as the
 * tell rather than as a compiler difference. OvlFunc_898_2008cfc below uses the
 * value 2 TWICE: once as the save-flag id in `__GetFlag(2)`, where the ROM
 * writes `mov r0, #2`, and once in this OR, where it writes `ldr r3, =2`. Same
 * value, same function, one immediate and one pooled.
 *
 * `_CONST_2` is defined in const.sym, whose header records the bar for adding
 * an entry. An absolute assignment emits no bytes, so a wrong name costs
 * nothing and a wrong value fails `make compare` at once.
 *
 * THREE OTHER READINGS, all of them rules already on the books:
 *
 *   TAKE THE ADDRESS, DO NOT KEEP THE STRUCT. `a = GetActor(); a->f64 |= 2;`
 *   gives `mov r1, r0 / add r1, #0x64` because gcc keeps the actor alive;
 *   `p = &GetActor()->f64;` gives the ROM's in-place `add r0, #0x64`.
 *
 *   THE ROM FALLS THROUGH TO THE FIRST MESSAGE. `bne .Ld30` puts the second
 *   message at the branch target, so the test is `== 0` with 0x123c in the
 *   `if`. Written the other way round it swaps both ids and the condition.
 *
 *   `*p &= 1` IS HAND-WRITTEN MASKING, NOT A BITFIELD. The ROM builds the mask
 *   with a bare `mov r3, #1` -- byte width -- where a bitfield assignment would
 *   have produced a 32-bit `mov / neg` pair. Batch 71's width rule.
 */
struct A { unsigned char pad00[0x64]; unsigned short f64; };

extern int _CONST_2;
extern unsigned char iwram_3001ebc[];
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void OvlFunc_898_2008938(int slot);

void OvlFunc_898_2008cfc(void)
{
    unsigned short *p;
    unsigned short two;
    char *base;

    p = &__MapActor_GetActor(0xe)->f64;
    two = (unsigned short)(int)&_CONST_2;
    *p = two | *p;
    __CutsceneStart();
    if (__GetFlag(0x855) == 0) {
        __MessageID(0x123c);
    } else {
        __MessageID(0x1349);
        if (__GetFlag(2)) {
            base = *(char **)iwram_3001ebc;
            (*(unsigned short *)(base + (0xec << 1)))++;
        }
    }
    OvlFunc_898_2008938(0xe);
    __CutsceneEnd();
    p = &__MapActor_GetActor(0xe)->f64;
    *p &= 1;
}

void OvlFunc_898_2008d78(void)
{
    unsigned short *p;
    unsigned short two;

    p = &__MapActor_GetActor(0xf)->f64;
    two = (unsigned short)(int)&_CONST_2;
    *p = two | *p;
    __CutsceneStart();
    if (__GetFlag(0x855) == 0)
        __MessageID(0x123d);
    else
        __MessageID(0x134b);
    OvlFunc_898_2008938(0xf);
    __CutsceneEnd();
    p = &__MapActor_GetActor(0xf)->f64;
    *p &= 1;
}
