// fakematch
/* OvlFunc_935_2008754  --  0x02008754
 *
 * WHOLE-FILE CONVERSION of asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_c_c_a_a.s --
 * its only function, no data section.  `%` resolves through the overlay's
 * existing `__modsi3 = _modsi3_RAM;` alias; the four softfloat calls are made
 * by name, so no linker change.
 *
 * A cutscene: sets actor-scale fields, 480 frames of jitter on p->0x24
 * (p = *iwram_3001e70 + 0x164), 70 frames of a fading blend word, then a map
 * rect copy and a sound.
 *
 * THE JITTER IS SOFT-FLOAT DOUBLE MATH, WRITTEN AS EXPLICIT CALLS.
 *   OvlFunc_common2_304 = floatsidf, _254 = adddf3, _28c = subdf3,
 *   _380 = fixdfsi.  The ROM is gcc's own `(double)unsigned` expansion
 *   (floatsidf, then `if ((int)u < 0) += 2^32`) inside
 *   `x = x - (4718.592 - (double)u)`.  Real `double` arithmetic cannot be
 *   used: this gcc build pools every double literal but 0.0/1.0 as
 *   0xafafafaf (docs/elevation.md), and expand_float synthesises the 2^32
 *   itself.  So the values are `long long` bit patterns (ARM mixed-endian:
 *   0x41f00000_00000000 is the long long 0x41f00000).
 *
 * `__attribute__((const))` ON THE FOUR ROUTINES IS THE LEVER (22 lines -> 0).
 *   expand_call wraps a const call in a libcall block, as gcc does for the real
 *   __adddf3; its argument constants then stay in their own pseudos (k, c2)
 *   that local-alloc puts in r2:r3 / r0:r1 while the converted value is still
 *   live, so that value conflicts with r0-r3 and goes to r6:r7 and the first
 *   operand spills to [sp+8] -- the ROM's allocation.  Without the attribute
 *   gcse cprop folds k into `(set r2 const)`, the converted value takes its r2
 *   preference and `a` takes r6:r7 (8 bytes less frame).  With the attribute
 *   the named k/c2 are still required (literals: 22 again).
 *
 * OTHER LEVERS: `r <<= 11; r >>= 16;` as two statements (in-place, and it
 * raises r's global-alloc priority so r5 goes to r, which pushes the double
 * off r4:r5); `c = 6` shifted at the use (cse2 rewrites the hoisted 6 << 10
 * as `lsl r7, r5, #10`); k1/k2/k3 in the block dominating the first
 * __Func_8012330 (constant-CSE lever).
 *
 * FAKEMATCH: the `-1, -1, 0xe666` call is register-pinned, the same pin as
 * OvlFunc_949_200828c.  Dominating-block locals for the pair (the documented
 * non-pin route, which closes OvlFunc_939_200849c) do not work here because
 * the else-arm stores -1 too: assigned above the if, cse1 follows the taken
 * branch into the else arm and shares one hoisted -1 (10 lines); assigned
 * above __CutsceneStart it lands in r5.  Literal / named / `-1U` / `(short)`
 * spellings are all 4 lines (one `mov r0,r1` copy).
 */
#include "gba/io.h"
extern unsigned char *iwram_3001e70;
extern unsigned int iwram_3001e40;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __Func_8012330(int a, int b, int c);
extern void __PlaySound(int id);
extern unsigned int __Random(void);
extern void __SetRegAnimDest(int dest, int val);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern void __Func_8012350(void);

extern long long OvlFunc_common2_304(int x) __attribute__((const));
extern long long OvlFunc_common2_254(long long a, long long b) __attribute__((const));
extern long long OvlFunc_common2_28c(long long a, long long b) __attribute__((const));
extern int OvlFunc_common2_380(long long a) __attribute__((const));

void OvlFunc_935_2008754(void)
{
    unsigned char *p;
    int i;
    unsigned int r;
    long long a, b, k, c2;
    int x, y, c;
    int m1, m2;
    int e, f, k1, k2, k3;

    p = iwram_3001e70 + 0x164;
    __CutsceneStart();
    k1 = 0xc0 << 10;
    k2 = 0xc0 << 10;
    k3 = 0x80 << 9;
    if (iwram_3001e40 & 1) {
        *(int *)(p + 0x18) = 1;
        *(int *)(p + 0x1c) = 1;
    } else {
        *(int *)(p + 0x18) = -1;
        *(int *)(p + 0x1c) = -1;
    }
    __Func_8012330(k1, k2, k3);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q0 = 1;
        q1 = 1;
        q0 = -q0;
        q1 = -q1;
        q2 = 0xe666;
        __Func_8012330(q0, q1, q2);
    }
    __PlaySound(0xa3);
    for (i = 0x1df; i >= 0; i--) {
        r = __Random();
        a = OvlFunc_common2_304(*(int *)(p + 0x24));
        r <<= 11;
        r >>= 16;
        b = OvlFunc_common2_304(r);
        if ((int)r < 0) {
            k = 0x41f00000LL;
            b = OvlFunc_common2_254(b, k);
        }
        c2 = 0x8d4fdf3b40b26e97LL;
        *(int *)(p + 0x24) = OvlFunc_common2_380(OvlFunc_common2_28c(a, OvlFunc_common2_28c(c2, b)));
        __CutsceneWait(1);
    }
    x = 6;
    y = 6;
    c = 6;
    for (i = 0; i <= 0x45; i++) {
        __SetRegAnimDest(REG_ADDR_BLDALPHA, (c << 10) | (x << 5) | y);
        __CutsceneWait(1);
        if (i % 0x14 == 0) {
            y--;
            x--;
        }
    }
    e = 0x13;
    f = 0x5b;
    __Func_80105d4(0x13, 0x53, 0xf, 8, e, f);
    __PlaySound(0x120);
    __Func_800fe9c();
    __Func_8012350();
    __CutsceneEnd();
}
