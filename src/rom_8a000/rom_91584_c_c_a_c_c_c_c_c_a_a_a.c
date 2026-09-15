/* Func_809233c -- 0x0809233c, asm/rom_8a000/rom_91584_c_c_a_c_c_c_c_c_a_a_a.s.
 *
 * Puts a map actor into the follow state: fixes its speed, snaps it to the
 * party leader, resets a flag, and hands it a script.
 *
 * WHOLE-FILE CONVERSION -- one function in the .s, no split, stage1.ld:870
 * verbatim with its asm/ prefix, no Makefile rule, no pins.
 *
 * EXACT ON THE FIRST CANDIDATE, entirely from levers already written down:
 *
 *   * THE gState BASE/OFFSET SPLIT, again, and this time with the symbol used
 *     ONLY ONCE. `*(int *)(gState + (0xfa << 1))` folds the offset into the
 *     pool word and emits one instruction; the ROM spends four --
 *     `ldr r3, =gState / mov r2, #0xfa / lsl r2, #1 / add r3, r2`. Two
 *     statements stop the fold. Worth recording that a SINGLE use is enough:
 *     the earlier cases had the base used twice, which left open whether the
 *     split was really about commoning. It is not.
 *   * THE SCRIPT LABEL is an asm-named extern, `unsigned char L9fbcc[]
 *     __asm__(".L9fbcc")`, the same idiom 550+ landed files use for `.L`
 *     symbols.
 *
 * EXACT: 136 bytes, 57 encodings, 9 relocations, measured three times, clean on
 * tools/tryc.py with no pool warning.
 */
#include "gba/types.h"

extern unsigned char gState[];
extern unsigned char L9fbcc[] __asm__(".L9fbcc");
extern u8 *GetFieldActor(int id);
extern void MapActor_SetSpeed(int slot, int a, int b);
extern u8 *MapActor_GetActor(int slot);
extern void MapActor_SetPos(int slot, int x, int z);
extern void _Actor_SetAnim(u8 *a, int n);
extern void Func_809228c(int a, int b, int c);
extern void _Actor_SetScript(u8 *a, unsigned char *s);

void Func_809233c(int slot, int b, int c, int d)
{
    u8 *a;
    u8 *m;
    u8 *gs;

    a = GetFieldActor(slot);
    if (a != NULL) {
        MapActor_SetSpeed(slot, 0x9999, 0x4ccc);
        gs = gState;
        gs += 0xfa << 1;
        m = MapActor_GetActor(*(int *)gs);
        if (m != NULL)
            MapActor_SetPos(slot, *(int *)(m + 8), *(int *)(m + 0x10));
        a[0x5b] = 0;
        _Actor_SetAnim(a, 2);
        Func_809228c(slot, b, c);
        _Actor_SetScript(a, L9fbcc);
        *(u16 *)(a + 0x64) = d;
    }
}
