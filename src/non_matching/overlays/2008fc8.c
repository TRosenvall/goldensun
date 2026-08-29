/* OvlFunc_903_2008fc8 -- 0x02008fc8,
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
 */
