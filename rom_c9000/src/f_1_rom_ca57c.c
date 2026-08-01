/* Func_ca57c .. Func_ca600 -- PlayEffect_Variant0 .. Variant11
 *
 * Twelve one-line wrappers around Func_ca60c, each supplying a variant number.
 * The variant indexes the 7-byte parameter records at .Ledf04 that drive the
 * effect's colours, timing and sound.  They exist so the animation table can
 * hold one address per variant.
 *
 * STATUS: ALL TWELVE MATCHING.  Verify any of them with e.g.
 *     tools/asmdiff.py Func_ca57c rom_c9000/src/f_1_rom_ca57c.c \
 *         --rom-offset 0xca57c --rom-size 0xc
 *
 * Func_ca60c is declared void deliberately.  The ROM epilogue is
 * `pop {r0}; bx r0`, which would destroy a return value in r0 -- so nothing is
 * returned, and declaring it int makes agbcc emit `pop {r1}; bx r1` instead.
 * That epilogue register is a reliable tell for void vs non-void.
 */

extern void Func_ca60c(void *desc, int variant);

void Func_ca57c(void *desc) { Func_ca60c(desc, 6); }
void Func_ca588(void *desc) { Func_ca60c(desc, 3); }
void Func_ca594(void *desc) { Func_ca60c(desc, 1); }
void Func_ca5a0(void *desc) { Func_ca60c(desc, 10); }
void Func_ca5ac(void *desc) { Func_ca60c(desc, 5); }
void Func_ca5b8(void *desc) { Func_ca60c(desc, 9); }
void Func_ca5c4(void *desc) { Func_ca60c(desc, 4); }
void Func_ca5d0(void *desc) { Func_ca60c(desc, 0); }
void Func_ca5dc(void *desc) { Func_ca60c(desc, 8); }
void Func_ca5e8(void *desc) { Func_ca60c(desc, 7); }
void Func_ca5f4(void *desc) { Func_ca60c(desc, 2); }
void Func_ca600(void *desc) { Func_ca60c(desc, 11); }
