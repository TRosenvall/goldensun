/* OvlFunc_946_2008ab0  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7ced6c/ovl_30_a_a_c_c_c.s
 * Best screen: 2 instructions in disagreeing regions, of 27 (streams same length).
 *
 * THIS PARK COVERS THREE FUNCTIONS. Byte-for-byte identical copies exist as
 *
 *      OvlFunc_946_2008ab0   ovl_7ced6c/ovl_30_a_a_c_c_c.s
 *      OvlFunc_964_2008ab0   ovl_7ed0a0/ovl_30_a_a_a_c_c_c_c.s
 *      OvlFunc_965_2008ab0   ovl_7ef4f4/ovl_30_a_a_a_c_c_c_c.s
 *
 * verified by comparing the instruction lists directly, not just the lengths.
 * Solving this one elevates all three. (tools/twin_finder.py did not report
 * them because it only matches unelevated functions against SOLVED ones; a
 * cluster of unelevated twins is invisible to it. Worth extending if another
 * such cluster turns up.)
 *
 * BLOCKER CLASS: scheduling of one load.
 *
 * Five int accumulate-adds followed by a halfword one. The whole difference is
 * that gcc issues the pointer load for the halfword part two instructions
 * early, inside the last int pair:
 *
 *      rom   ... str r3, [r0, #0x1c] / ldr r1, [r0, #0x50]
 *      ours  ... ldr r1, [r0, #0x50] / ... / str r3, [r0, #0x1c]
 *
 * WHAT WAS TRIED
 *   1. The five accumulates as `*(int *)(a + X) += *(int *)(a + Y);` and the
 *      halfword spelled out (kept below). 2 of 27.
 *   2. The LAST accumulate also spelled out with temporaries, to give gcc a
 *      longer statement sequence it would not interleave. WORSE, 10 of 27 --
 *      the temporaries perturb the register assignment of the whole tail.
 *   3. `--no-sched2` and `--O1`: both 8 of 27, so the scheduler is wanted here
 *      and is not the thing to disable. `--no-rerun-cse` is identical to the
 *      default at 2.
 *
 * The accumulate order is already right and should not be re-derived: the ROM
 * loads the DESTINATION first in the first pair and the ADDEND first in the
 * other four, which is exactly what `dst += src` produces once the first pair
 * has set the pattern.
 */
void OvlFunc_946_2008ab0(unsigned char *a)
{
    unsigned char *q;
    int v;
    int w;

    *(int *)(a + 8) += *(int *)(a + 0x44);
    *(int *)(a + 0xc) += *(int *)(a + 0x48);
    *(int *)(a + 0x10) += *(int *)(a + 0x4c);
    *(int *)(a + 0x18) += *(int *)(a + 0x30);
    *(int *)(a + 0x1c) += *(int *)(a + 0x34);
    q = *(unsigned char **)(a + 0x50);
    a += 0x64;
    v = *(unsigned short *)(q + 0x1e);
    w = *(unsigned short *)a;
    v += w;
    *(unsigned short *)(q + 0x1e) = v;
}
