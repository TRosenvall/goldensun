/* Cluster LoadUIBanner..LoadUIBanner extracted from goldensun/asm/rom_15000/rom_19ebc_a_c_c_c_a_c_c.s.
 *
 * The four-way switch needs FOUR DISTINCT SYMBOLS at one address -- see
 * label.sym, where _UIBANNER_1..3 are defined and the evidence is recorded.
 * With one symbol in all four arms gcc tail-merges the identical blocks and this
 * function compiles to nine instructions instead of twenty-nine.
 */
extern unsigned char Data_31864[];
extern unsigned char _UIBANNER_1[];
extern unsigned char _UIBANNER_2[];
extern unsigned char _UIBANNER_3[];
extern void UploadSpriteGFX(void *dst, int size, void *src);

int LoadUIBanner(unsigned int which, int unused, void *dst)
{
    unsigned char *p;

    switch (which) {
    case 1:
        p = _UIBANNER_1;
        break;
    case 2:
        p = _UIBANNER_2;
        break;
    case 3:
        p = _UIBANNER_3;
        break;
    case 0:
    default:
        p = Data_31864;
        break;
    }
    UploadSpriteGFX(dst, 0x20, p);
    return 1;
}
