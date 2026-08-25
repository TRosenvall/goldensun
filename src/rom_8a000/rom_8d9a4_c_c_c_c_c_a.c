/* Cluster GetSpriteVoiceEntry..GetSpriteVoiceEntry extracted from goldensun/asm/rom_8a000/rom_8d9a4_c_c_c_c_c.s.
 *
 * Total .text for this TU = 32 bytes (= 0x20).
 * The .rodata stays in the original .s, which keeps its name and its .rodata
 * line in goldensun/stage1.ld; only the .text line is repointed here.
 *
 * A LEAF FUNCTION, matched on the first screen. Searches a 0x82-entry table of
 * 4-byte records for the one whose leading halfword equals the speaker id, and
 * returns a pointer to it -- or to one past the end if there is no match, which
 * is what the ROM does and is preserved here.
 *
 * The loop is entered by testing the FIRST entry before the increment, which is
 * why the entry test sits outside the do/while rather than at its head. The
 * bound check `(unsigned int)i > 0x81` is unsigned to match `bhi`.
 *
 * NOTE the `.global .L9e9f0` added to the .s: the table label is file-local, and
 * splitting the function out of that file puts the reference in a different
 * object. See docs/elevation.md -- the link error names the label and reads
 * like a missing file.
 */
extern unsigned char L9e9f0[] __asm__(".L9e9f0");

void *GetSpriteVoiceEntry(int id)
{
    unsigned short *p;
    int key;
    int i;

    key = id;
    p = (unsigned short *)L9e9f0;
    i = 0;
    if (*p == key)
        return p;
    do {
        i++;
        p = (unsigned short *)((unsigned char *)p + 4);
        if ((unsigned int)i > 0x81)
            return p;
    } while (*p != key);
    return p;
}
