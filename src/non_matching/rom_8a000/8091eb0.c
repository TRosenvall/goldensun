/* Func_8091eb0 (0x08091eb0) -- NON-MATCHING.
 * Blocker class: a dead join branch gcc will not delete, plus value-vs-address
 * materialisation order at one store.
 *
 * 43 lines against the ROM's 41, 13 differing, and the 13 are two independent
 * residues plus the alignment shift they cause.
 *
 * RESIDUE 1 -- a branch to the next instruction (2 of the 2 extra lines):
 *
 *     rom    bl Func_808adf0 / L1: / mov r0, r5
 *     ours   bl Func_808adf0 / b L2 / L2: / L1: / mov r0, r5
 *
 * Our second `if` block exits through a jump to a label that is immediately
 * followed by the real join. gcc-2.96's jump optimiser normally deletes this.
 *
 * RESIDUE 2 -- which of the value and the address is built first:
 *
 *     rom    ldr r3, =gState / ldr r2, =0x21 / add r1, #0x5a / add r3, r1
 *            strh r2, [r3]
 *     ours   ldr r3, =gState / add r1, #0x5a / add r2, r3, r1 / ldr r3, =0x21
 *            strh r3, [r2]
 *
 * Same five instructions; the ROM materialises the stored VALUE first and then
 * advances the base register in place, ours builds the address into a second
 * register and loads the value after.
 *
 * MEASURED (rom 41 lines):
 *   baseline, `g = gState;` then `*(short *)(g + 0x1d6) = 0x21;`   43, 13
 *   `g = gState; g += 0x1d6; *(short *)g = 0x21;`                  44, 30 (worse)
 *   two separate `unsigned char *` locals, one per if-block        43, 13
 *   Func_808adf0 declared int rather than void                     43, 13
 *   `if (a == 0x62) { if (b == 0) {...} }` instead of &&           43, 13
 *   naming the nested GetFieldActor result before the outer call   43, 13
 *   -fno-strict-aliasing / -fno-gcse / -fno-strength-reduce /
 *     -fno-rerun-cse-after-loop                                    43, 13 (all inert)
 *   -fno-schedule-insns2                                           43, 16 (worse)
 *
 * The flag sweep was run EARLY this time, per batch 173's rule about aliasing,
 * and it says the residue is not any of the documented passes. Every one of the
 * six source spellings is byte-identical to the baseline, which is the same
 * "the pairing is the finding" argument recorded for OvlFunc_928_2008d0c: if a
 * source construct controlled either residue, six spellings that move the block
 * structure, the locals, the callee's return type and the call nesting would not
 * all land on exactly 43 and 13.
 *
 * WHAT IS RIGHT: everything else, including both derived offsets. gcc rebuilds
 * 0x1d6 as `0x17c + 0x5a` and 0x1f4 as `0x19e + 0x56` on its own, exactly as the
 * ROM does -- those come free from writing the plain offsets and do NOT need to
 * be spelled as derivations. The `g = gState;` named base is required (without
 * it the offset folds into a pooled `gState+470`), and the pooled `ldr r2, =0x21`
 * is NOT a `_CONST_*` symbol: gcc pools that literal by itself here, so the
 * pooled-small-constant tell has an exception when the constant is stored
 * through a halfword pointer at a large derived offset.
 *
 * NEXT: nothing source-level found in six probes.
 */
extern int iwram_3001ebc;
extern unsigned char gState[];
extern int GetEncounterGroup(int encounterID, int group);
extern int GetFieldActor(int actorID);
extern void Func_808adf0(int a);
extern void Func_808b320(int a, int b);

void Func_8091eb0(int a, int b)
{
    unsigned char *g;
    int e;

    e = iwram_3001ebc;
    *(short *)(e + (0xbe << 1)) = GetEncounterGroup(a, b);
    if (a == 0x62 && b == 0) {
        g = gState;
        *(short *)(g + 0x1d6) = 0x21;
    }
    if (*(short *)(e + (0xcf << 1)) == 3) {
        g = gState;
        Func_808adf0(GetFieldActor(*(int *)(g + 0x1f4)) + 8);
    }
    Func_808b320(a, b);
}
