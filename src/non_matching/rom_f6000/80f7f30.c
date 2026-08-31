/* Func_80f7f30 -- 0x080f7f30 -- asm/rom_f6000/rom_f6008_c.s
 *
 * BLOCKER: register-offset loads where the ROM builds addresses.
 * 10 of 31, LENGTH EXACT.
 *
 * Copies `count` bytes from a fixed source into dst at a running index held in
 * memory, bumping that index per byte and re-reading the count each pass.
 *
 * TWO ORDERING FIXES took it from 28 differing to 10, and both were read off
 * the ROM rather than guessed:
 *
 *   1. THE OFFSET IS LOADED BEFORE THE BASE. The ROM has `ldr r1, =0x4404 /
 *      ldr r2, [r3] / add r3, r2, r1`, and keeps 0x4404 live to rebuild the
 *      count pointer inside the loop body. Assigning `o = 0x4404` before the
 *      base pointer reproduces that; the other order costs the length.
 *   2. THE COUNTER IS INITIALISED BEFORE THE TEST. `mov r0, #0` precedes the
 *      `cmp`, so `i = 0;` belongs before the `if`, not inside it.
 *
 * WHAT REMAINS is the addressing form at the two `base + o` sites:
 *
 *     rom    add r3, r2, r1 / ldr r3, [r3, #0x0]     (address built, then load)
 *     ours   ldr r3, [r2, r3]                        (register-offset load)
 *
 * MEASURED, none of them move it:
 *   a named pointer local for the test site
 *     (`t = (int *)(base + o); if (*t != 0)`)     30 lines, 24 differ  WORSE
 *   index-first form at the test site
 *     (`*(int *)(o + (int)base)`)                 31 lines, 10 differ
 *   index-first at both sites                     31 lines, 10 differ
 *
 * The last two are byte-identical to the baseline, so gcc canonicalises
 * `o + (int)base` and `base + o` to one rtx here. That bounds the operand-order
 * lever, which DID decide this exact choice on Func_80b9a70 and
 * OvlFunc_916_2008be4: it works when the two operands are a base and a
 * COMPUTED index, and does nothing when the index is a loop-invariant constant
 * gcc has already hoisted into a register.
 *
 * The named-pointer result is the sharper negative -- naming the address is the
 * documented way to force `add` + immediate load, and here it costs a line and
 * fourteen differences, because the named pointer is then shared with the
 * in-loop count pointer that the ROM rebuilds separately.
 */
extern int ewram_2004c00;

void Func_80f7f30(unsigned char *dst)
{
    char *base;
    int *pos;
    int *cnt;
    unsigned char *src;
    int i;
    int o;
    int off;

    o = 0x4404;
    base = (char *)ewram_2004c00;
    i = 0;
    if (*(int *)(base + o) != 0) {
        off = 0x443c;
        pos = (int *)(base + off);
        off -= 0x34;
        cnt = (int *)(base + o);
        src = (unsigned char *)(base + off);
        do {
            dst[*pos] = *src;
            *pos = *pos + 1;
            i++;
            src++;
        } while (i != *cnt);
    }
}
