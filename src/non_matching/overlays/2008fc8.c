/* OvlFunc_903_2008fc8 -- NON-MATCHING, 31 differing STREAM encodings of 87.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/hold_2008fc8.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 87 / ours 87 stream encodings; 79/79 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/2008fc8.c \
 *     asm/overlays/rom_798dc4/ovl_314_c_c_c_c_b.s --func OvlFunc_903_2008fc8
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
extern unsigned char *__GetUnit(int i);
extern int __GiveItemTo(int who, int item);
extern unsigned char *__GetItemInfo(int id);
extern void __Func_8078948(int who, int slot);
extern void __EquipItem(int who, int slot);

void OvlFunc_903_2008fc8(void)
{
    unsigned char *unit;
    unsigned char *p;
    unsigned char *info;
    int n, j, item, mask, v, z;
    unsigned char *q;

    item = 0x41;
    unit = __GetUnit(2);
    n = 0;
top:
    n = n + 1;
    if (n > 0xfa << 2) {
        q = unit;
        q += 0xf4;
        z = 0;
        *(unsigned short *)q = z;
    }
    if (__GiveItemTo(2, 0x41) == -1) {
        p = unit;
        j = 0;
        p += 0xd8;
    l1:
        info = __GetItemInfo(*(unsigned short *)p);
        p += 2;
        if (info[2] == 1)
            goto found;
        j++;
        if (j <= 0xe)
            goto l1;
        p = unit;
        mask = 0x8ff;
        j = 0;
        p += 0xd8;
    l2:
        info = __GetItemInfo(*(unsigned short *)p);
        if ((*(unsigned short *)(info + 2) & mask) == 0 && info[0xc] == 1) {
        found:
            __Func_8078948(2, j);
            goto top;
        }
        j++;
        p += 2;
        if (j > 0xe)
            goto top;
        goto l2;
    }
    p = unit;
    j = 0;
    p += 0xd8;
l3:
    v = *(unsigned short *)p;
    p += 2;
    if (v == item)
        __EquipItem(2, j);
    j++;
    if (j <= 0xe)
        goto l3;
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_903_2008fc8 -- 0x02008fc8,
 * asm/overlays/rom_798dc4/ovl_314_c_c_c_c_b.s
 *
 * FAKEMATCH, and a more alarming one than DecodeMetatileset because it passed
 * every screen available.  Candidate kept at scratch/hold_2008fc8.c.
 *
 *   - tryc.py against a single-function reference: OK, 94 lines.
 *   - The size check RAN and PASSED.
 *   - `arm-none-eabi-objdump -h` on the two objects: .text 0xbc both, .data 0
 *     both, .bss 0 both -- byte-for-byte identical section sizes.
 *   - The linked overlay differs from orig.bin in 58 bytes.
 *
 * The one warning it did carry: "the reference keeps its literal pool INSIDE
 * the function".  Pool loads normalise to `=value` in the screen, so a pool
 * holding different values at the same distance compares equal.  That is where
 * the 58 bytes are.
 *
 *   The inline-pool warning is not cosmetic.  Two of this round's screens
 *   carried it and both were fakematches.  When it appears, `make compare` is
 *   the only authority -- an equal instruction stream AND an equal .text size
 *   still do not settle it.
 *
 * A NOTE ON THE SPLIT, which is sound and has been kept.  This .s held a
 * trailing `.section .data` defining gOvl_020092f8/02009358/02009368/02009488,
 * and the overlay linker script named the object twice, once for (.text) and
 * once for (.data).  Deleting the .s to replace it with a .c broke the link on
 * those four symbols.  The fix -- text into `_b`, data into `_c`, and the two
 * linker-script lines pointed at the two new objects -- was verified
 * byte-neutral with the function still in asm BEFORE any .c was introduced,
 * which is what let the fakematch be attributed to the C rather than to the
 * layout.  Do that check in that order every time.
 *)

*/
#endif
