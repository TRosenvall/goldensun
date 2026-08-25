/* Cluster OvlFunc_888_2008070..OvlFunc_888_2008070 extracted from
 * goldensun/asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_a_a_a.s.
 *
 * Total .text for this TU = 190 bytes (= 0xbe), of which 140 are the jump table.
 * First in the run, ahead of the _b piece, in goldensun/overlays/rom_7892c8/overlay.ld.
 *
 * Picks one of five script tables by the sub-area word at gState+0x1c2.
 *
 * A PLAIN `switch` REPRODUCES THE ROM'S JUMP TABLE EXACTLY, including the
 * `sub r3, #1 / cmp r3, #0x22 / bhi` range check, the `.align 2, 0`, and all
 * thirty-five `.word` entries in order. Nothing had to be done to provoke it --
 * the case values are dense enough that gcc-2.96 chooses a tablejump on its
 * own. Worth knowing before hand-writing an if-chain to imitate one: the table
 * in the .s is not a hand-written artifact.
 *
 * The area word is read as a struct member rather than by pointer arithmetic,
 * which is what gives the ROM's `add r3, r2 / mov r2, #0 / ldrsh r3, [r3, r2]`
 * -- Thumb `ldrsh` has no immediate form, and written as arithmetic gcc folds
 * the offset into the index register instead. See OvlFunc_922_2009a34.
 *
 * NOTE FOR RESCREENING: tryc.py warns that the reference keeps its pool inside
 * the function and that it cannot see PC-relative offsets. That warning is
 * correct and was cleared here by `make compare`, not by the screen.
 */

typedef struct {
    unsigned char pad[0x1c0];
    short area;
    short sub;
} GlobalState;

extern GlobalState gState;
extern unsigned char L3bf4[] __asm__(".L3bf4");
extern unsigned char L3c0c[] __asm__(".L3c0c");
extern unsigned char L3ccc[] __asm__(".L3ccc");
extern unsigned char L3d2c[] __asm__(".L3d2c");
extern unsigned char L3e04[] __asm__(".L3e04");

void *OvlFunc_888_2008070(void)
{
    switch (gState.sub) {
    case 1:
    case 2:
        return L3c0c;
    case 10:
    case 11:
    case 12:
    case 35:
        return L3ccc;
    case 20:
    case 21:
        return L3d2c;
    case 29:
    case 32:
        return L3e04;
    }
    return L3bf4;
}
