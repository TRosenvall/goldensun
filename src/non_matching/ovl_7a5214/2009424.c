/* OvlFunc_918_2009424 (0x02009424) -- NON-MATCHING.
 * Blocker class: argument-setup order, TWO SITES of eleven, 4 lines of 154.
 *
 * A thirteen-case jump table whose arms are all __MapActor_SetAnim(8, N) with
 * __WaitFrames(6) between, including two arms that fall through into shorter
 * ones. The table, the fallthroughs, the shared exit and nine of the eleven
 * call sites are exact on the first screen.
 *
 * The residue is two sites, both the same shape and both the THIRD call of a
 * four-call arm:
 *
 *     rom    mov r1, #0x3 / mov r0, #0x8
 *     ours   mov r0, #0x8 / mov r1, #0x3
 *
 * WHAT MAKES IT ODD, and worth the file: the ROM uses r1-first at ALL FOUR
 * calls in each long arm, and we match three of the four. The same source
 * statement, repeated four times with only the anim number changing, comes out
 * r1-first three times and r0-first once. So this is not the return-type or
 * prototype question that normally governs two-argument order -- it is
 * position-dependent scheduling inside one basic block.
 *
 * MEASURED, both 154 lines and 4 differing:
 *   the callee's prototype withheld (the recorded lever for two-argument
 *     order, and the one this function's own exemplar file singles out)
 *   the callee declared to return `int` (the return-type lever)
 *
 * Those are the two recorded controls over r0's position and neither moves it.
 *
 * WHAT IS RIGHT AND SHOULD BE KEPT: the switch. Thirteen cases over a dense
 * 0..0xc range give the ROM's jump table directly; case 6 is DELIBERATELY
 * ABSENT because its table entry is the default target, and omitting it
 * produces the same table. The two fallthrough arms (case 4 into case 1, case
 * 11 into case 8) are written as fallthroughs and come out as the ROM's
 * `b`-less joins.
 *
 * NEXT: nothing source-level.
  *
 * BATCH 205 -- THE PIN IS INERT HERE, MEASURED AT EIGHT SITES.
 *
 * The two differing pairs are, by instruction number rather than by prose:
 *
 *     70/71    rom  mov r1, #0x3 / mov r0, #0x8      ours the reverse
 *    136/137   rom  mov r1, #0x8 / mov r0, #0x8      ours the reverse
 *
 * Pinning r0 and r1 and assigning them in the ROM's order changes NOTHING --
 * not at the two sites alone, and not with all EIGHT calls of both long arms
 * pinned. Byte-identical to the unpinned form in both cases, same count, same
 * first difference.
 *
 * THIS BOUNDS THE PIN FURTHER THAN BATCH 197 DID. That batch ordered three
 * argument registers by pinning, on a call whose fills the ROM ran backwards,
 * and concluded a pin orders independent movs. It does not order these. The
 * difference is not yet identified; what is established is that "a pin orders
 * independent movs" is too strong as stated, and the 200a1c4 case should be
 * read as three-register argument setup rather than as a general rule.
 *
 * Note the second pair: both registers receive 0x8, so this is also the
 * same-value case from batch 192, where nothing in the source orders them.
 * The first pair is NOT -- 3 and 8 are distinct -- so the two sites fail for
 * what may be two different reasons.
 *
 * A CORRECTION TO THIS PARK'S OWN TEXT: it describes the residue as "the THIRD
 * call of a four-call arm". The differing instructions carry anims 3 and 8,
 * which are the SECOND and FOURTH calls of those arms, not the third. Picking
 * sites from that description rather than from the instruction numbers wasted
 * the first attempt here.
*/
extern void __MapActor_SetAnim(int slot, int anim);
extern void __WaitFrames(int n);

void OvlFunc_918_2009424(int n)
{
    switch (n) {
    case 0:
        __MapActor_SetAnim(8, 1);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 3);
        break;
    case 2:
        __MapActor_SetAnim(8, 1);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 5);
        break;
    case 3:
        __MapActor_SetAnim(8, 1);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 4);
        break;
    case 4:
        __MapActor_SetAnim(8, 1);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 3);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 1);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 3);
        __WaitFrames(6);
    case 1:
        __MapActor_SetAnim(8, 1);
        break;
    case 5:
        __MapActor_SetAnim(8, 1);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 2);
        break;
    case 7:
        __MapActor_SetAnim(8, 6);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 8);
        break;
    case 9:
        __MapActor_SetAnim(8, 6);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 9);
        break;
    case 10:
        __MapActor_SetAnim(8, 6);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 0xa);
        break;
    case 11:
        __MapActor_SetAnim(8, 6);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 8);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 6);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 8);
        __WaitFrames(6);
    case 8:
        __MapActor_SetAnim(8, 6);
        break;
    case 12:
        __MapActor_SetAnim(8, 6);
        break;
    }
    __WaitFrames(0xc);
}
