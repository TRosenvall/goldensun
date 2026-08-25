/* Cluster OvlFunc_939_20087f4..OvlFunc_939_20087f4 extracted from goldensun/asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7c460c/ovl_314_a_c_a_c_a_a.o and the rest of the overlay in
 * goldensun/overlays/rom_7c460c/overlay.ld.
 *
 * UNPARKED, BUILT AT -O1, AND THE C DID NOT CHANGE.
 *
 * This was parked in batch 32 on SPECULATIVE LITERAL HOIST: for a two-way pick
 * between two pooled constants feeding one call, gcc at -O2 loads the
 * fall-through constant BEFORE the compare and conditionally overwrites it,
 * which inverts the branch and costs a `mov` to get the merge into r0. The park
 * recorded four source formulations and five optimisation flags, and named the
 * one to start from next.
 *
 * None of that was the answer. The body below is formulation 2 from the park,
 * unaltered; -O1 alone matches it.
 *
 * WHY IT WAS MISSED FOR TEN BATCHES is the useful part. `--O1` WAS among the
 * flags tried -- on formulations 1, 3 and 4. It was never tried on 2, because
 * by the time 2 was written the flags had already been ruled out on the others.
 * Flags and source forms were swept as two separate axes and the product was
 * never covered.
 *
 * tools/rank_parks.py --flags now screens every park under every per-file build
 * setting the tree uses, which is what found this. It is worth re-running
 * whenever a new per-file rule is added: a park written before a rule existed is
 * otherwise never revisited by anything.
 */
extern int __GetFlag(int flag);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int b);

void OvlFunc_939_20087f4(void)
{
    int id;

    if (__GetFlag(0x941) == 0) {
        if (__GetFlag(0x85a) == 0)
            id = 0x1be2;
        else
            id = 0x1ba5;
        __MessageID(id);
        __ActorMessage(0x12, 0);
    } else {
        __MessageID(0x250c);
        __ActorMessage(0x12, 0);
    }
}
