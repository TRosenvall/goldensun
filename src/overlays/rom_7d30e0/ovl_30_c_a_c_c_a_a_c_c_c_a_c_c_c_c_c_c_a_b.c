// fakematch
/* OvlFunc_948_200941c  --  0x0200941c
 *
 * Cut out of goldensun/asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_a_c_c_c_c_c_c_a.s.
 *
 * A guard: read the player's tile coordinates, and if three save-state
 * conditions hold and the player is standing in a particular three-tile strip,
 * set a flag and write a scene id. Fifty-six instructions and four levers.
 *
 * THE COORDINATES ARE SIGNED DIVISIONS, NOT SHIFTS. The ROM spends five
 * instructions on each -- `cmp r3,#0 / bge / ldr r1,=0xfffff / add r3,r1 /
 * asr r3,#20` -- which is exactly gcc's expansion of `x / 0x100000` for a
 * signed int: bias by 2^20-1 when negative, then arithmetic-shift. Writing
 * `>> 20` gives the three-instruction unbiased form and cannot match. The bias
 * constant in the pool is the tell.
 *
 * A RANGE TEST TAKES THE UNSIGNED-OFFSET IDIOM. `ty >= 0x10 && ty <= 0x12`
 * emits two compares and two branches; `(unsigned)(ty - 0x10) <= 2` gives the
 * ROM's `mov / sub #0x10 / cmp #2 / bhi`. Its twin OvlFunc_948_2009308 has BOTH
 * forms in one function -- a three-value range as the subtract and a two-value
 * range as two compares -- so neither spelling is "the" translation and the
 * listing decides.
 *
 * ONE VARIABLE CARRIES THE OFFSET AND THEN THE VALUE. The final store is
 * `mov r3,#0xc1 / lsl r3,#1 / add r2,r6,r3 / mov r3,#0x5c / strh r3,[r2]`:
 * r3 is refilled one instruction after the `add` consumes it. Two source
 * variables put the value's `mov` BEFORE the add and score 4; reusing one puts
 * it after and is exact. Same shape as the one-variable-two-ranges lever in
 * src/overlays/rom_7d768c/ovl_30_c_a_a_a_c_b.c, here inside a single statement
 * pair rather than across a branch.
 *
 * The stored constant is named as its own statement because otherwise it goes
 * to the pool despite fitting an 8-bit immediate -- store width, not magnitude.
 *
 * ITS TWIN IS PARKED at src/non_matching/ovl_7d30e0/2009308.c, 4 of 58 with the
 * length exact, on nothing but which callee-saved register two locals get. The
 * source there is this source with five constants changed.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];

extern unsigned char *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);

void OvlFunc_948_200941c(void)
{
    unsigned char *p;
    unsigned char *g;
    unsigned char *w;
    int tx, ty, v;
    short *q;
    register int p0 __asm__("r0");

    p = __MapActor_GetActor(0);
    tx = *(int *)(p + 8) / 0x100000;
    ty = *(int *)(p + 0x10) / 0x100000;
    w = iwram_3001ebc;
    p0 = 0x88; p0 <<= 2;
    if (__GetFlag(p0) == 0) {
        g = gState;
        if (*(short *)(g + (0x93 << 2)) == 0
            && *(short *)(g + 0x24a) != 9
            && tx == 0xa
            && (unsigned)(ty - 0x10) <= 2) {
            p0 = 0x88; p0 <<= 2;
            __SetFlag(p0);
            v = 0xc1;
            v <<= 1;
            q = (short *)(w + v);
            v = 0x5c;
            *q = v;
        }
    }
}
