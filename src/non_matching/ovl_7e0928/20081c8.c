/* OvlFunc_956_20081c8  [ovl_7e0928]
 *
 * Source asm: goldensun/asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c.s
 *
 * NOT SPLIT. The .s still holds all seven of its functions and the overlay
 * linker script is untouched.
 *
 * Waits ten frames, then spins until two overlay globals reach 3 and 1, giving
 * up after 0x78 frames.
 *
 * Blocker: THE PRE-HEADER LOAD MERGE. 25 of 26, short by exactly one
 * instruction. See src/non_matching/preheader_load_merge.c for the class.
 *
 * This is the overlay member of the family, and it confirms the class is not
 * specific to the main ROM or to any one symbol kind -- here both loaded values
 * are `.L` data labels in the overlay, bound with gcc asm-labels, and the shape
 * of the residue is identical to the two main-ROM members.
 */
extern int L5480 __asm__(".L5480");
extern int L5484 __asm__(".L5484");
extern void __WaitFrames(int n);

void OvlFunc_956_20081c8(void)
{
    int i;
    int v;

    __WaitFrames(0xa);
    v = L5480;
    i = 0;
    goto check;
loop:
    i++;
    __WaitFrames(1);
    if (i > 0x77)
        return;
    v = L5480;
check:
    if (v != 3)
        goto loop;
    if (L5484 != 1)
        goto loop;
}
