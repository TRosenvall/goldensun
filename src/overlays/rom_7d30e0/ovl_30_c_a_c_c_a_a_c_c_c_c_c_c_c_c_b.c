/* OvlFunc_948_20097ac -- 0x020097ac
 *
 * A tile gate: if the story counter has not reached its limit, the save bit is
 * still clear, a second counter is zero, and the player stands on one of two
 * specific tiles, set the bit and poke a value into the map struct.
 *
 * THIS FILE NEEDS -fno-rerun-cse-after-loop; see the CSE_CFLAGS rule beside it
 * in the Makefile. The ROM rebuilds the flag id `mov r0, #0x88 / lsl r0, #2` at
 * BOTH the test and the set. At -O2 the second CSE pass hoists it into a
 * callee-saved register and keeps it live across the __GetFlag call, which
 * costs a fifth callee-saved register -- so the actor coordinate the ROM parks
 * in r8 is pushed out to r10 and the prologue grows from `push {r7}` to
 * `push {r6, r7}`. 63 differing of 62; exact with the flag.
 *
 * WHY NO SPELLING REACHES IT, which is what justifies the rule: three were
 * measured -- the cast store, the typed field, and the id written as a plain
 * `0x220` at both sites -- and all three produce IDENTICAL output. The constant
 * is folded before CSE ever runs, so the ROM's rematerialisation is a
 * pass-level property, not a source-level one. The named template
 * src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_c_c.c carries the same rule for
 * the same reason.
 *
 * THE SAME FUNCTION SHOWS BOTH CONSTANT SPELLINGS, which makes the rule
 * readable here: gState+0x24c is written `0x93 << 2` and the map offset
 * `0xc1 << 1` because the ROM builds those with `mov`/`lsl`, while gState+0x24a
 * -- which no eight-bit `mov` plus shift can build -- is POOLED and so is
 * written as a plain `0x24a`. Measure the operand; do not guess from magnitude.
 *
 * Three more on file: both scalings are SIGNED DIVISIONS (`cmp / bge /
 * ldr =0xfffff / add / asr #20` is `/ 0x100000`, not a shift); the gState base
 * is a NAMED LOCAL or gcc folds symbol and offset into one `=gState+586` pool
 * word; and the narrow store of 0x60 goes through a TYPED FIELD, since the cast
 * form pools it and this function has no spare register for a named local.
 *
 * Verified with tools/objcmp.py under the file's own flags: 140 bytes, 63
 * encodings and 5 relocations identical.
 */
typedef struct {
    unsigned char pad[0xc1 << 1];
    short f182;
} Obj;

extern unsigned char gState[];
extern Obj *iwram_3001ebc;

extern unsigned char *__MapActor_GetActor(int slot);
extern int  __GetFlag(int id);
extern void __SetFlag(int id);

void OvlFunc_948_20097ac(void)
{
    unsigned char *a;
    unsigned char *g;
    Obj *p;
    int x;
    int z;

    a = __MapActor_GetActor(0);
    x = *(int *)(a + 8) / 0x100000;
    z = *(int *)(a + 0x10) / 0x100000;
    p = iwram_3001ebc;
    g = gState;
    if (*(short *)(g + 0x24a) == 0xc)
        return;
    if (__GetFlag(0x88 << 2) != 0)
        return;
    if (*(short *)(g + (0x93 << 2)) != 0)
        return;
    if (x != 0x13)
        return;
    if (z != 0xf && z != 0x10)
        return;
    __SetFlag(0x88 << 2);
    p->f182 = 0x60;
}
