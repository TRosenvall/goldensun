/* Func_a1f74  --  0x080a1f74, was asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a_a_c_c_c_a.s
 * (this function and its pool), so it converts whole. Matched from scratch.
 *
 * The byte compare comes out as `lsl #24` on both sides only when the test is
 * on the ASSIGNMENT expression, `(*dst = *src) != (char)0xff`; a loaded local
 * gives `cmp #255`, a signed char local `asr`. The four tables are exported by
 * rom_a1814_c_c_c_c.s. (The ROM's .s spelled `.thumb_Func_start`.)
 */
extern char Laf2a6[] __asm__(".Laf2a6");
extern char Laf2b1[] __asm__(".Laf2b1");
extern char Laf2bc[] __asm__(".Laf2bc");
extern char Laf2d0[] __asm__(".Laf2d0");

void Func_a1f74(int sel, char *dst)
{
    char *src = Laf2a6;
    int i;

    switch (sel) {
    case 0: src = Laf2d0; break;
    case 1: src = Laf2bc; break;
    case 2: src = Laf2b1; break;
    }
    i = 0;
    if ((*dst = *src) != (char)0xff) {
        do {
            i++;
            if (i > 31)
                break;
            src++;
            dst++;
        } while ((*dst = *src) != (char)0xff);
    }
}
