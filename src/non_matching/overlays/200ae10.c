/* OvlFunc_common1_e10 -- NON-MATCHING.
 * Blocker class: DATA SYMBOL NAMES COLLIDE WITH gcc's GENERATED LABELS.
 * 48 lines against the ROM's 47, 13 differing, and the first 24 lines are
 * exact.
 *
 * THIS ONE IS A HAZARD, NOT JUST A MISMATCH, AND THAT IS WHY IT IS PARKED
 * RATHER THAN LEFT AS A NEAR MISS.
 *
 * The function selects one of five script tables whose ROM labels are `.L2`,
 * `.L3`, `.L11`, `.L12` and `.L13`. Declared the usual way --
 * `extern unsigned char L3[] __asm__(".L3");` -- gcc compiles the reference to
 * `.word .L3` in its literal pool. But gcc ALSO generates its own local labels
 * for this function's branches, and here they are:
 *
 *     .L3:  .L4:  .L5:  .L6:  .L8:  .L9:  .L10:  .L11:  .L12:
 *
 * Three of them -- .L3, .L11, .L12 -- are the same names. The assembler
 * resolves the pool entries to the LOCAL labels, so the code loads the address
 * of a branch target instead of a script table. It compiles clean and it is
 * wrong.
 *
 * The screen shows this as `rom ldr r1, =L9 / ours ldr r1, =L3` after
 * normalisation, which reads like a label-numbering artifact. It is not; it is
 * the collision surfacing.
 *
 * NINE elevated files already use short `.LN` externs and match, so the hazard
 * is not general -- it bites only when gcc's label numbering for that
 * particular function happens to overlap. Which means it cannot be predicted
 * from the source and has to be checked in the generated .s.
 *
 * HOW TO CHECK, for anyone writing one of these: compile and grep the output
 * for the symbol both as a definition and as a pool entry.
 *
 *     grep -oE "^\.L[0-9]+:" out.s        gcc's own labels
 *     sed -n "/^\.L8:/,/^$/p" out.s       the pool it built
 *
 * If a name appears in both lists, the reference is captured.
 *
 * NOT SOLVED, and no workaround is proposed here. The obvious ones -- renaming
 * the data labels, or aliasing them in the linker script -- both change files
 * this function does not own, and the symbols are not declared .global in
 * their defining .s, so how they are meant to be reached across translation
 * units is itself unresolved. That question should be answered before anyone
 * spends a screen on this function.
 *
 * The rest of the C below is believed correct: the two halfword stores, the
 * task start, the five-way selection, and the pooled zero shared by three
 * halfword stores with a plain `mov` for the word store.
 */
extern short L33[] __asm__(".L33");
extern short L22[] __asm__(".L22");
extern short L36[] __asm__(".L36");
extern short L46[] __asm__(".L46");
extern short L24[] __asm__(".L24");
extern void *L37[] __asm__(".L37");
extern int L25[] __asm__(".L25");
extern unsigned char L11[] __asm__(".L11");
extern unsigned char L2[] __asm__(".L2");
extern unsigned char L12[] __asm__(".L12");
extern unsigned char L3[] __asm__(".L3");
extern unsigned char L13[] __asm__(".L13");
extern void OvlFunc_common1_920(void);
extern void __StartTask(void (*f)(void), int n);

void OvlFunc_common1_e10(int a, int b)
{
    unsigned char *p;

    *L33 = a;
    *L22 = b << 4;
    __StartTask(OvlFunc_common1_920, 0xc8 << 4);
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
    *L36 = 0;
    *L37 = p;
    *L46 = 0;
    *L24 = 0;
    *L25 = 0;
}
