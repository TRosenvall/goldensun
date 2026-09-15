/* Func_80108c4 @ 0x080108c4  --  SetMapLayerBits
 *
 * Replaces bits 9-11 of the map control halfword at [iwram_3001e70]+0x14 with
 * bits 9-11 of the argument, leaving every other bit untouched.
 *
 * NO PINS.  The residue was 7 of 14 at the ROM's exact length, and every one of
 * the seven was the same defect: the two constants held OPPOSITE registers,
 * because gcc hoisted the 0xf1ff pool load above the `mov #0xe0`.
 *
 *     rom   mov r2,#0xe0 / ldrh r1 / ldr r3,=0xf1ff / lsl r2,#4
 *     ours  ldr r2,=0xf1ff / ldrh r1 / mov r3,#224  / lsl r3,#4
 *
 * WRITING THE SHIFT BEFORE THE SECOND CONSTANT IS THE WHOLE FIX.  Moving
 * `m <<= 4` ahead of `k = 0xf1ff` -- one statement earlier -- is exact.  So is
 * putting the shift immediately after `m = 0xe0`.  Note the source order is NOT
 * the ROM's order: the ROM emits the pool load BETWEEN the mov and the lsl, and
 * writing it that way is what gives 7.  gcc reorders either way; what the source
 * controls is which constant is still unfinished when the pool load is placed.
 *
 * Pinning either constant also reaches exact (m to r2, k to r3, both measured),
 * but is not needed and is not shipped -- an honest statement order does it.
 *
 * Both ANDs accumulate into the CONSTANT register (`and r2, r0` and
 * `and r3, r1`), which is why the masks are the named accumulators here rather
 * than the loaded value.  That is the recorded "name whichever operand the ROM
 * puts in the second register" rule; here the ROM puts the loaded value second
 * at BOTH sites.
 *
 * The file-mate Func_8010788 is ~150 instructions and was not attempted, so
 * this landed by split: _a keeps it in asm, _b is this.
 */
extern unsigned char *iwram_3001e70;

void Func_80108c4(int value)
{
    unsigned char *p;
    int m, v, k;

    p = iwram_3001e70;
    m = 0xe0;
    v = *(unsigned short *)(p + 0x14);
    m <<= 4;
    k = 0xf1ff;
    m &= value;
    k &= v;
    k |= m;
    *(unsigned short *)(p + 0x14) = k;
}
