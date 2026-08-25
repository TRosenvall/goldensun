/* LoadUIBanner -- NOT MATCHING. 3 of 29, same length, and the three are SYMBOL
 * NAMES rather than instructions.
 *
 * Source asm: goldensun/asm/rom_15000/rom_19ebc_a_c_c_c_a_c_c.s
 *
 * THE STRUCTURE IS EXACT. 29 against 29, every instruction in place, including
 * gcc's balanced-tree switch lowering. What differs is only which symbol each of
 * three arms loads.
 *
 * WHAT THE ROM'S LITERAL POOL SAYS. It holds FOUR entries and all four are the
 * same address:
 *
 *     .L1a360: .word Data_31864
 *     .L1a364: .word Data_31864
 *     .L1a368: .word Data_31864
 *     .L1a36c: .word Data_31864
 *
 * Written with ONE symbol in all four arms, gcc collapses the entire function to
 * NINE instructions: the four arms become identical basic blocks and it
 * tail-merges them, then folds the switch away entirely. That is not a subtle
 * difference -- 9 against 29.
 *
 * So the original source did NOT reference one symbol four times. gcc-2.96
 * tail-merges identical blocks (batch 56 established that independently, as the
 * cross-jumping class), and the ROM's blocks are not merged. THE FOUR NAMES WERE
 * DISTINCT and they resolve to the same address.
 *
 * THE REMEDY IS THREE SYMBOL DEFINITIONS, AND IT IS A MAINTAINER'S CALL.
 * Defining `_UIBANNER_1 = 0x08031864;` and two more, in the style of area.sym,
 * emits no bytes and makes this match -- the placeholder externs in the body
 * below are exactly that shape and produce the 29-instruction stream.
 *
 * It is left undone because the existing .sym files hold ID VALUES, not
 * addresses, and asserting that four names exist at one data address is a claim
 * about the original source rather than about the ROM. HANDOFF.md records the
 * same reasoning for the area ids, which were parked until the pool tell made
 * the inference solid. This one has a solid inference and no names.
 *
 * Data_31864 is a single `.incdata` blob spanning 0x31864..0x31e24, so all four
 * point at its start rather than at offsets within it.
 *
 * WORTH CHECKING ELSEWHERE FIRST: N identical pool entries in one function means
 * N distinct source symbols. That is a general reading and this is the first
 * function it has been needed for.
 */
extern unsigned char Data_31864[];
extern unsigned char _BANNER_1[];
extern unsigned char _BANNER_2[];
extern unsigned char _BANNER_3[];
extern void UploadSpriteGFX(void *dst, int size, void *src);

int LoadUIBanner(unsigned int which, int unused, void *dst)
{
    unsigned char *p;

    switch (which) {
    case 1:
        p = _BANNER_1;
        break;
    case 2:
        p = _BANNER_2;
        break;
    case 3:
        p = _BANNER_3;
        break;
    case 0:
    default:
        p = Data_31864;
        break;
    }
    UploadSpriteGFX(dst, 0x20, p);
    return 1;
}
