/* OvlFunc_945_200837c  --  0x0200837c, was
 * asm/overlays/rom_7cb2c0/ovl_30_c_c_a_a_a_a_a_a.s (this function alone), so it
 * converts whole. Matched from scratch.
 *
 * A DEAD STORE INSIDE A SKIPPED BLOCK KILLS cse's CONSTANT REUSE. The plain C is
 * exact under -fno-rerun-cse-after-loop OR -fno-cse-skip-blocks: cse2 reaches the
 * second __GetFlag(0x8a0) along a skip-blocks path (cse_end_of_basic_block,
 * "branch around a block"). Naming the id `f = 0x8a0`, passing f, and assigning
 * `f = 1` inside the conditionally skipped store block makes
 * invalidate_skipped_block drop f's equivalence, so the constant is rebuilt; flow
 * deletes the dead store and nothing is emitted. A source route for what would
 * otherwise want a per-file CSE_CFLAGS row. (tryc's "pool 39 vs 21" warning counts
 * the jump table; objcmp is byte-exact.)
 */
extern unsigned char gState[];
extern unsigned char L6da8[] __asm__(".L6da8");
extern unsigned char L6eb0[] __asm__(".L6eb0");
extern unsigned char L6d78[] __asm__(".L6d78");
extern unsigned char L6fe8[] __asm__(".L6fe8");
extern unsigned char L6d48[] __asm__(".L6d48");
extern unsigned char L6c58[] __asm__(".L6c58");
extern unsigned char L6bf8[] __asm__(".L6bf8");
extern unsigned char L6be0[] __asm__(".L6be0");

extern int __GetFlag(int id);

unsigned char *OvlFunc_945_200837c(void)
{
    unsigned char *g;
    int f;

    g = gState;
    switch (*(short *)(g + (0xe1 << 1))) {
    case 1:
    case 2:
    case 11:
        if (__GetFlag(0x93e))
            return L6da8;
        if (__GetFlag(0x928)) {
            if (__GetFlag(0x8a0)) {
                L6eb0[0x16] = 2;
                L6eb0[0x46] = 2;
                L6eb0[0x76] = 2;
                L6eb0[0x8e] = 2;
                L6eb0[0xd6] = 2;
                L6eb0[0xbe] = 2;
                L6eb0[0xa6] = 1;
                L6eb0[0x5e] = 2;
            }
            return L6eb0;
        }
        if (__GetFlag(0x911)) {
            if (__GetFlag(0x925)) {
                L6da8[0x16] = 2;
                L6da8[0x76] = 2;
                L6da8[0x2e] = 2;
                L6da8[0x5e] = 2;
            }
            return L6da8;
        }
        return L6d78;
    case 4:
    case 12:
    case 16:
    case 18:
    case 20:
    case 21:
    case 23:
    case 24:
        return L6fe8;
    case 15:
    case 17:
    case 19:
        L6fe8[0x16] = 2;
        L6fe8[0x2e] = 2;
        L6fe8[0x5e] = 1;
        L6fe8[0x76] = 2;
        L6fe8[0x8e] = 2;
        L6fe8[0xa6] = 2;
        L6fe8[0xbe] = 2;
        L6fe8[0xd6] = 1;
        L6fe8[0xee] = 2;
        return L6fe8;
    case 5:
        if (__GetFlag(0x93e))
            return L6d48;
        if (__GetFlag(0x911) == 0)
            goto bf8;
        if (__GetFlag(0x922) == 0)
            break;
        f = 0x8a0;
        if (__GetFlag(f)) {
            L6c58[0x2e] = 1;
            f = 1;
        }
        if (__GetFlag(0x925)) {
            if (__GetFlag(0x8a0) == 0)
                L6c58[0x16] = 0;
        }
        return L6c58;
    case 10:
    case 13:
    case 14:
    case 22:
    bf8:
        return L6bf8;
    }
    return L6be0;
}
