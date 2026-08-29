/* OvlFunc_common1_e10  --  asm/overlays/common/common1_a_a_a_a_c_c.s
 *
 * SCREENS OK ONLY WITH THE LABELS RENAMED.  The .L9xx names below are STAND-INS
 * for the overlay's real symbols .L33 .L22 .L36 .L46 .L24 .L25 .L37 .L11 .L2
 * .L12 .L3 .L13.  Spelt with the real names via the asm-label extension the
 * function is 13 differing of 47, because gcc emits its OWN .L3, .L11 and .L12
 * in this compile and the references bind to those instead.  See the report:
 * the fix is a rename in asm/overlays/common/common1_c_c_b.s (definitions and
 * .global lines) plus the references in common1_a_a_a_a_c_c.s -- two files,
 * nothing else in the tree names these symbols.
 *
 * Otherwise straightforward: a chain of independent `if`s selecting a script
 * pointer (NOT an else-if chain -- the ROM re-tests r5 each time), and the
 * halfword zeros come from a POOL (`ldr r2, .Le7c`) while the word zero is a
 * `mov r3, #0`, which is docs/elevation.md's HImode rule visible in one
 * function.  Writing `L36 = 0; L37 = p; L46 = 0; L24 = 0; L25 = 0;` in that
 * order is what shares the one pooled zero across the three strh.
 * No --cflags.
 */
extern short L33 __asm__(".L933");
extern short L22 __asm__(".L922");
extern short L36 __asm__(".L936");
extern short L46 __asm__(".L946");
extern short L24 __asm__(".L924");
extern int L25 __asm__(".L925");
extern unsigned char *L37 __asm__(".L937");
extern unsigned char L11[] __asm__(".L911");
extern unsigned char L2[] __asm__(".L902");
extern unsigned char L12[] __asm__(".L912");
extern unsigned char L3[] __asm__(".L903");
extern unsigned char L13[] __asm__(".L913");

extern void OvlFunc_common1_920(void);
extern void __StartTask(void (*f)(void), int a);

void OvlFunc_common1_e10(int a, int b)
{
    unsigned char *p;

    L33 = a;
    L22 = b << 4;
    __StartTask(OvlFunc_common1_920, 0xc80);
    p = L11;
    if (a == 2)
        p = L2;
    if (a == 4)
        p = L12;
    if (a == 3) {
        if (b != 0)
            p = L3;
        else
            p = L13;
    }
    L36 = 0;
    L37 = p;
    L46 = 0;
    L24 = 0;
    L25 = 0;
}
