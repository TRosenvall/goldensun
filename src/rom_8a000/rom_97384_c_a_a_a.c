/* asm/rom_8a000/rom_97384_c_a_a_a.s -- the whole file, one function, exact.
 *
 *   OK Func_80974d8 -- 104 bytes, 49 encodings and 2 relocations identical
 *
 * The park's residue was two adjacent instructions at equal length:
 *
 *      rom   ldr r2, [r6, #0x4] / str r3, [r6, #0x0]
 *      ours  str r3, [r6, #0x0] / ldr r2, [r6, #0x4]
 *
 * and it was recorded as "instruction scheduling", exhausted.  The class was
 * right but the axis was not.  `-fsched-verbose=5` on the `.23.sched2` dump
 * prints the whole decision: at t = 18 the ready list is {store, load}, the two
 * TIE on priority (6/6), TIE on class (both 3 -- the store's link from the
 * subtract costs 1, which `rank_for_schedule` already treats as independent),
 * TIE on dependent count (1 each), and fall through to `INSN_LUID`, where the
 * store was emitted first and wins.
 *
 * THE LEVER IS THE DEPENDENT COUNT, and it is bought by SINKING `out[1] = 0`
 * INTO BOTH ARMS.  That store is a memory ANTI-dependence on the `out[1]` load,
 * so `INSN_DEPEND(load)` becomes {zero-store, subtract} = 2 against the
 * `out[0]` store's 1.  The load wins one step before LUID is consulted, and
 * cross-jumping (which runs in `.25.jump2`, AFTER scheduling) merges the two
 * duplicated `mov r3,#0 / str r3,[r6,#4]` tails back into the single shared
 * tail the ROM has.  Same 49 encodings, no extra instruction.
 *
 * The alias-set device does NOT apply here and the park was right to find
 * struct retyping inert: out[0] and out[1] are constant offsets off ONE base,
 * which gcc disambiguates by offset arithmetic, so no type spelling can reach
 * them (docs/elevation.md, "ALIASING IS THE WRONG LEVER WHEN TWO ACCESSES
 * SHARE A BASE").
 *
 * Kept from the park, and still the reusable half: the second global is reached
 * by DERIVING its address -- the ROM loads &iwram_3001ebc, reads through it,
 * then `sub r3, #0x4c` and reads again.  Two separate externs cannot produce
 * that; `*(unsigned char **)((char *)&iwram_3001ebc - 0x4c)` reproduces it.
 */
extern unsigned char *iwram_3001ebc;
extern void PhysMove(int *out, int *buf);

void Func_80974d8(int *out)
{
    int buf[3];
    unsigned char *p;
    unsigned char *q;
    int a;
    int b;

    p = iwram_3001ebc;
    if (*(short *)(p + (0xcf << 1)) == 3) {
        PhysMove(out, buf);
        out[0] = buf[0] << 16;
        out[2] = buf[1] << 16;
        out[1] = 0;
    } else {
        q = *(unsigned char **)((char *)&iwram_3001ebc - 0x4c);
        a = *(int *)(q + 0xe4) & 0xffff0000;
        b = *(int *)(q + 0xe8) & 0xffff0000;
        out[0] = out[0] - a;
        out[2] = out[2] - out[1] - b;
        out[1] = 0;
    }
}
