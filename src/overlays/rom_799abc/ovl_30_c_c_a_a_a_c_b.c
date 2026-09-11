/* OvlFunc_905_2008ecc -- 0x02008ecc, the THIRD and LAST function of
 * asm/overlays/rom_799abc/ovl_30_c_c_a_a_a_c.s.
 *
 * LANDING SHAPE: the .s holds THREE .thumb_func_start blocks --
 * OvlFunc_905_2008bd0, OvlFunc_905_2008ce0, OvlFunc_905_2008ecc -- and no
 * .data/.rodata/.word/.byte tail.  tools/asmfacts.py says
 *     3 functions  split first
 * so this is NOT a whole-file convert: the .s must be SPLIT, the first two
 * functions staying in asm/ and this one moving to src/.  Every `.L` symbol
 * used by this function (.Leec .Lefe .Lf06 .Lf0c .L105c) is DEFINED INSIDE it,
 * and the other two functions likewise reference only their own labels, so the
 * split needs no `.global` on any `.L`.  (OvlFunc_905_2008ce0 takes the address
 * of OvlFunc_905_2008bd0, but both stay on the asm side, so nothing crosses.)
 *
 * The one line naming the object, overlays/rom_799abc/overlay.ld:34,
 *     asm/overlays/rom_799abc/ovl_30_c_c_a_a_a_c.o(.text)
 * stays VERBATIM -- it is never rewritten to src/.
 *
 * FLAG GROUP: NONE.  tryc.makefile_flags() on
 * src/overlays/rom_799abc/ovl_30_c_c_a_a_a_c.c is the empty set and no explicit
 * or wildcard Makefile rule names rom_799abc, so the default
 * `asm/%.o: src/%.c` rule fires at plain GCC296_CFLAGS (-O2 -mthumb
 * -mthumb-interwork -fcall-used-r4).  NO PINS and NO SCAFFOLDING, so no
 * `// fakematch`.
 *
 * VERIFIED with tools/objcmp.py against asm/overlays/rom_799abc/ovl_30_c_c_a_a_a_c.s,
 * measured THREE times, identical each time:
 *
 *   OK OvlFunc_905_2008ecc -- 444 bytes, 185 encodings and 31 relocations identical
 *
 * `make compare` was NOT run (out of scope for this session) and remains the gate.
 *
 * WHAT IT DOES: if actor 0xa stands on tile (0x26, 0xe), drop its Y to
 * -0x20000, copy that into its targetY, play 0xbc and fire six
 * OvlFunc_905_2008a68 bursts from the actor's position, one per direction
 * (the 0x8000 / 0x6666 / 0xffff999a / 0xffff8000 pairs are a half-circle of
 * 16.16 unit vectors), then set save bit 0x301.
 *
 * TWO SCREENS, and only ONE spelling decision separated them.
 *
 *  1. THE TILE TEST IS A SIGNED DIVIDE WRITTEN LONGHAND, not `>> 20`.  The ROM
 *     has `cmp r3,#0 / bge .Leec / ldr r0,=0xfffff / add r3,r0` before the
 *     `asr #20`, which is gcc's `/ 0x100000` expansion.  The file-mate
 *     src/overlays/rom_799abc/ovl_30_c_c_a_a_a_b.c (OvlFunc_905_2008b6c, the
 *     SAME overlay directory) already writes that longhand and it transferred
 *     verbatim -- except for its guard spelling.  Its `if (x <= (0 - 1))`
 *     compiles HERE to `mov r6,#1 / neg r6,r6 / cmp r3,r6 / bgt`, four
 *     instructions and a callee-saved register instead of two.  `if (x < 0)`
 *     is the spelling, and swapping it took 170 of 187 (two instructions long)
 *     straight to EXACT.  That was the whole delta.
 *
 *  2. Everything else fell out of the neighbour idiom: repeat
 *     `__MapActor_GetActor(0xa)` at EVERY field read rather than naming a
 *     pointer.  The ROM calls it three times per burst -- one per coordinate
 *     argument -- and one named pointer would have collapsed all three.
 *
 * The four held constants (0 in r8, 1 in r11, 0x6666 in r9, 0xffff999a in r10)
 * need no source help at all: plain literals at every site, and cse commons
 * them into the callee-saved registers by itself.  r11 being pushed is the tell
 * that FOUR values are carried, and the bare literals reproduce it.
 */
#include "actor.h"

extern struct Actor *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void OvlFunc_905_2008a68(int a, int b, int c, int d, int e, int f, int g);

void OvlFunc_905_2008ecc(void)
{
    int x;
    int z;
    int vx;
    int vz;

    x = __MapActor_GetActor(0xa)->pos.x;
    if (x < 0)
        x += 0xfffff;
    vx = x >> 20;
    z = __MapActor_GetActor(0xa)->pos.z;
    if (z < 0)
        z += 0xfffff;
    vz = z >> 20;
    if (vx == 0x26) {
        if (vz == 0xe) {
            __MapActor_GetActor(0xa)->pos.y = 0xfffe0000;
            __MapActor_GetActor(0xa)->targetY =
                __MapActor_GetActor(0xa)->pos.y;
            __PlaySound(0xbc);
            OvlFunc_905_2008a68(__MapActor_GetActor(0xa)->pos.x,
                                __MapActor_GetActor(0xa)->pos.y,
                                __MapActor_GetActor(0xa)->pos.z,
                                0x80 << 8, 0, 0, 1);
            OvlFunc_905_2008a68(__MapActor_GetActor(0xa)->pos.x,
                                __MapActor_GetActor(0xa)->pos.y,
                                __MapActor_GetActor(0xa)->pos.z,
                                0x6666, 0x6666, 0, 1);
            OvlFunc_905_2008a68(__MapActor_GetActor(0xa)->pos.x,
                                __MapActor_GetActor(0xa)->pos.y,
                                __MapActor_GetActor(0xa)->pos.z,
                                0xffff999a, 0x6666, 0, 1);
            OvlFunc_905_2008a68(__MapActor_GetActor(0xa)->pos.x,
                                __MapActor_GetActor(0xa)->pos.y,
                                __MapActor_GetActor(0xa)->pos.z,
                                0xffff8000, 0, 0, 1);
            OvlFunc_905_2008a68(__MapActor_GetActor(0xa)->pos.x,
                                __MapActor_GetActor(0xa)->pos.y,
                                __MapActor_GetActor(0xa)->pos.z,
                                0x6666, 0xffff999a, 0, 1);
            OvlFunc_905_2008a68(__MapActor_GetActor(0xa)->pos.x,
                                __MapActor_GetActor(0xa)->pos.y,
                                __MapActor_GetActor(0xa)->pos.z,
                                0xffff999a, 0xffff999a, 0, 1);
            __SetFlag(0x301);
        }
    }
}
