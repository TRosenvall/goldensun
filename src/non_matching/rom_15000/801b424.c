/* Func_801b424  --  0x0801b424
 * asm/rom_15000/rom_1aeec_a_a_c_a_c_c.s, line 6 (first of seven functions).
 *
 * PARKED at 12 aligned of 91, ours 89 lines.
 *
 * BLOCKER CLASS: addressing-mode selection, with a register-numbering tail.
 *
 * THE TWO-INSTRUCTION GAP IS AN ADDRESSING MODE gcc will not give up. The ROM
 * materialises each halfword address into its own register and loads with a
 * zero displacement:
 *
 *      rom   add r3, r5, r1 / add r1, #2 / ldrh r2, [r3] / add r3, r5, r1 /
 *            ldrh r3, [r3]
 *      ours  ldrh r2, [r5, r3] / add r3, #2 / ldrh r3, [r5, r3]
 *
 * Unsigned halfword loads DO have a register-offset form in Thumb, and gcc
 * takes it whenever the address is a PLUS of two live pseudos -- which is the
 * exact mirror of the ldrsh finding recorded for OvlFunc_899_200a564, where a
 * REG+REG address is what SAVES two instructions. Here it costs them. Naming the
 * address in its own pointer variable does not help: gcc folds `q = p + off;
 * *(unsigned short *)q` straight back into the register-offset mode. Three
 * spellings -- inline offsets, a pointer assigned before the increment, and a
 * pointer assigned after -- all measure EXACTLY 12, which by this notebook's own
 * rule means the residue is not in those variables.
 *
 * The rest is caller-saved register NUMBERING: the ROM uses r1 for the loop
 * offset and r2 for the 0x3e7 compare, ours uses r2 and r3. REG_ALLOC_ORDER
 * starts 3, 2, 1, 0, so ours are ranking HIGHER than the ROM's -- consistent
 * with ours being two instructions short, since the missing address pseudos are
 * exactly the competitors that would push the others down. Close the addressing
 * mode and this tail should close with it; it is not an independent problem.
 *
 * WHAT IS ALREADY EXACT, and the lever that got it there:
 *
 * THE LOOP HEAD MUST BE A goto LABEL, NOT A LOOP. Written as `for (;;)` around
 * a `do/while`, gcc hoists the poll address out of the loop -- `add r7, r5, r2`
 * before the loop, a seventh callee-saved register in the push list, and the
 * ROM's per-iteration `mov / lsl / add` gone. The ROM rebuilds it every pass.
 * A backward `goto` is not a natural loop to loop.c and gets no invariant
 * motion at all, which is what the ROM shows. 17 aligned to 12, and the push
 * list matches exactly afterwards.
 *
 * That the label is branched to from THREE places is the structural tell: two
 * of them are the `continue` paths at the bottom, which is not a shape a single
 * `for`/`while` produces. When a backward target has more than one predecessor
 * besides the loop latch, write the goto.
 *
 * Also already right: `volatile` on both polled key globals, which is what
 * makes the ROM's separate reloads of each appear; the literal `0xe8 << 2` and
 * `0xd2 << 2` offsets built with mov/lsl from bare literals; and the parameter
 * being REUSED as the return value in the one arm that returns it, which the
 * ROM shows by writing the sum into the same register the parameter arrived in.
 *
 * Screened with tools/tryc.py --align; 7 spellings measured. Not built.
 */
extern unsigned char *iwram_3001e98;
extern volatile int gKeyRepeat;
extern volatile int gKeyPress;

extern void WaitFrames(int n);
extern void _PlaySound(int id);
extern void Func_801b664(unsigned char *p);
extern void Func_801b810(unsigned char *p);

int Func_801b424(int v)
{
    unsigned char *p;
    unsigned int off;
    int lo;

    p = iwram_3001e98;
top:
    WaitFrames(1);
    if (*(unsigned short *)(p + (0xe8 << 2)) != 0)
        goto top;
    if (v != 0x3e7) {
        if (gKeyRepeat & 0x10) {
            _PlaySound(0x6f);
            Func_801b664(p);
        } else if (gKeyRepeat & 0x20) {
            _PlaySound(0x6f);
            Func_801b810(p);
        }
        if (gKeyPress & 1) {
            off = 0xe7 << 2;
            lo = *(unsigned short *)(p + off);
            off += 2;
            v = lo + *(unsigned short *)(p + off);
            if (*(unsigned short *)(*(int *)(p + (0xd2 << 2)) + 0xa) == 6) {
                if (v == 0)
                    _PlaySound(0x70);
                else
                    _PlaySound(0x71);
            } else {
                _PlaySound(0x70);
            }
            return v;
        }
    }
    if (v == 0)
        goto top;
    if (!(gKeyPress & 2))
        goto top;
    _PlaySound(0x71);
    return -1;
}
