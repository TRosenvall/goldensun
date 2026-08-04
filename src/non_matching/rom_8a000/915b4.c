/* GetSpriteVoice  [rom_8a000]  --  GetActiveMessagePortrait
 *
 * Source asm: goldensun/asm/rom_8a000/rom_91584_a_c_a_a.s
 *
 * Returns the portrait id for the current speaker, biased by 0x100, or 0 when
 * there is none.
 *
 * ONE THING IS SOLVED AND WORTH REUSING. gcc folds `&gState + 0x20a` into a
 * single pool entry (`ldr r3, =gState+522`) where the ROM keeps two words and
 * adds them:
 *
 *     ldr r3, =gState / ldr r2, =0x20a / add r3, r2 / ldrb r3, [r3]
 *
 * Writing the addition as its own statement over an integer base reproduces
 * it. Putting the offset in a named local does NOT -- gcc folds it back and
 * the function comes out a instruction short. This is the same shape as the
 * gState+0x1C0 arithmetic in the GetEntrances family, and it is the second
 * function where the statement form is what matters rather than the naming.
 *
 * Blocker: BLOCK ORDER. Eighteen instructions against eighteen, everything up
 * to the second compare identical, and then the two arms are emitted in the
 * opposite order:
 *
 *     rom    bne L1 / mov r0,#0 / b L2 / mov r3,#0x80 / lsl / add r0,r3
 *     ours   beq L0 / mov r3,#0x80 / lsl / add r0,r3 / b L1 / mov r0,#0
 *
 * The ROM puts the zero arm first and branches PAST it to the add; gcc puts
 * the add first and branches past it to the zero.
 *
 * TRIED, all still 18-vs-18 at instruction 10 unless noted:
 *   1. `if (p != 0xff) return add;` with a trailing `return 0`
 *   2. `if (p == 0xff) return 0;` with a trailing `return add`
 *   3. explicit if/else with the zero in the else arm
 *   4. a single exit through a result variable (19 instructions)
 *   5. a shared early `return 0` for both failure paths (19)
 *   6. a ternary (19)
 *
 * docs/elevation.md records "the if body becomes the fall-through path" as a
 * general rule, and it decided two functions in batches 17 and 19. It does not
 * reach this one: every arrangement of the two arms produces the same layout,
 * because gcc is choosing which arm to make the fall-through on its own rather
 * than following the source. That is a real limit on the rule and is recorded
 * here rather than left as an exception someone else rediscovers.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *GetSpriteVoiceEntry(void);

int GetSpriteVoice(void)
{
    unsigned char *e;
    unsigned int base;
    int p;

    base = (unsigned int)&gState;
    base += 0x20a;
    if (*(unsigned char *)base != 0) {
        e = GetSpriteVoiceEntry();
        p = e[2];
        if (p == 0xff)
            return 0;
        return p + (0x80 << 1);
    }
    return 0;
}
