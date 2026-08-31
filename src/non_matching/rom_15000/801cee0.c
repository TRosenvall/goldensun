/* Func_801cee0 -- asm/rom_15000/rom_1ca1c_a_c.s
 *
 * BLOCKER: gState offset folding, partly fixed, plus a per-arm register copy.
 * Best 34 of 48 at 38 lines; best LENGTH 43 of 48 at 46 differing.
 *
 * A three-case switch on a halfword: each arm reads a different gState byte,
 * bounds-checks it, and all three converge on a shared increment.
 *
 * THE DISPATCH IS EXACT. Every instruction of the comparison chain -- the
 * `cmp #1 / beq`, `cmp #1 / bgt`, `cmp #0 / beq`, the `b` to the exit, and the
 * second-level `cmp #2 / beq` -- reproduces from a plain `switch` with three
 * cases and a `default: return`. That is the switch-versus-if/else-if lever
 * from Func_80c0f98 working again, and it is the part that looked hardest.
 *
 * THE LENGTH GAP IS THE gSTATE FOLD. Written as `&gState[0x83 * 4]`, gcc
 * folds the whole thing to `ldr r2, =gState+524`; the ROM builds it at
 * runtime, `ldr r3, =gState / mov r2, #0x83 / lsl r2, #2 / add r1, r3, r2`.
 * A local `unsigned char *g = gState` restores the runtime construction and
 * takes the function from 38 lines to 43 of 48 -- the documented lever, worth
 * five lines here.
 *
 * It also RAISES the differing count from 34 to 46, because the arms then use
 * different registers than the folded version did. Both results are recorded
 * rather than picking the prettier number: the folded form is closer by count
 * and further by length, and on a function with a five-line shortfall the
 * length is the one that matters.
 *
 * WHAT REMAINS is the ROM's `ldrb r2, [r1] / mov r3, r2 / cmp r3, #N / bhi`
 * -- a copy of the loaded byte before the comparison, once per arm, which is
 * three of the five missing lines.
 *
 * MEASURED: typing the loaded value `unsigned char` so the comparison forces a
 * QImode-to-SImode promotion -- the mechanism identified on Func_8020b64 --
 * is BYTE-IDENTICAL, with and without the gState pointer. So the copy is not
 * a width promotion here; the byte is already zero-extended by `ldrb` and gcc
 * compares it directly.
 *
 * That leaves it as another instance of the unreachable-copy boundary
 * (Func_80a8b10, Func_80e38b8, HeightTile_B): the copied value never diverges
 * from the original, so any local initialised from it is coalesced.
 */
extern unsigned char gState[];

void Func_801cee0(char *p)
{
    unsigned char *g;
    unsigned char *q;
    int v;

    g = gState;
    switch (*(unsigned short *)(p + 0x574)) {
    case 0:
        q = g + 0x83 * 4;
        v = *q;
        if ((unsigned int)v > 1)
            return;
        break;
    case 1:
        q = g + 0x205;
        v = *q;
        if ((unsigned int)v > 0x17)
            return;
        break;
    case 2:
        q = g + 0x206;
        v = *q;
        if ((unsigned int)v > 0xe)
            return;
        break;
    default:
        return;
    }
    *q = v + 1;
}
