/* Func_80a93a4 (DrawAbilityDetailPage, 0x080a93a4) -- 229 instructions.
 * NON-MATCHING: 121 encodings of 232 differ (objcmp --whole).
 * 121 IS A TRUE DISTANCE: ours 232 encodings against ref 232, and objcmp
 * reports no SIZE line, so byte count and instruction count both match.
 * tryc.py --align says 103 instructions in disagreeing regions of 244.
 *
 * Reference: asm/rom_a1000/rom_a8604_c_a_a_a.s -- ONE function, datacheck
 * clean, so this is a WHOLE-FILE conversion and needs no split.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a93a4.c \
 *     asm/rom_a1000/rom_a8604_c_a_a_a.s --whole
 *
 * objcmp also reports ONE EXTRA RELOCATION, R_ARM_ABS32 `_MSG_75` at 0x1f0,
 * because the hand-written reference spells the word as the literal `=0x75`.
 * That is expected and not a difference in the linked bytes: `_MSG_75 = 0x0075`
 * is already admitted at message.sym:220, named by the neighbour Func_80a5614
 * feeding the SAME _Func_801e7c0 sink, and the landed twin below carries the
 * identical shape with _MSG_53a.  make compare is the authority here, not the
 * relocation list.
 *
 * THE FIND: this is a longer sibling of the LANDED, byte-exact Func_80a6a98 in
 * src/rom_a1000/rom_a5534_c_c_c_a_c_b.c -- same opening
 * (`d[6] = d[2]*5 + d[4]`, _Func_8016498(*(unsigned int *)(state + 0x2c)),
 * WaitFrames(1), `_Func_801e7c0((v & 0x1ff) + (int)&_MSG_k, ..., 0, 0)`), same
 * `for (i = 0; i <= 4; i++)` loop testing `i == d[4]`, same `return 1`.
 * Copying that neighbour's idioms wholesale is what took the first candidate
 * from 235 to 118.
 *
 * DROP LADDER (--align of 244; ours length in brackets):
 *   first candidate ....................................... 235   (258)
 *   + `n5 = d[2] * 5` NAMED so the two uses share one
 *     pseudo (written `d[2]*5*2` it folds to `d[2]*10`
 *     and nothing is shared) .............................. 118   (247)
 *   + a named `ofs` inside the `i == d[4]` arm, which keeps
 *     the ROM's two-register `ldrh rD,[rB,rO]` instead of
 *     folding state into the address
 *   + the arms spelled `if (t != 4) ... else ...`, from the
 *     ROM's `beq` to the ==4 block
 *   + the else arm's halfword read through an explicit
 *     POINTER incremented at the loop bottom, not `row[i]`  103   (244)
 * The last step is the one that made length and count exact: with `row[i]` gcc
 * keeps i*2 as one biv and SPILLS the row base (`sub sp,#0x10`, two extra
 * words); with an incremented pointer it is a single biv and the frame is the
 * ROM's `sub sp,#8`.
 *
 * Also load-bearing, each read off the .s rather than guessed:
 *  - the second loop is 4 iterations counting DOWN in the ROM (`mov r7,#3 /
 *    sub r7,#1 / cmp r7,#0 / bge`) with y a separate biv, which is
 *    check_dbra_loop reversing a count-up `for` whose counter is otherwise
 *    unused -- so it is written counting UP and gcc reverses it.
 *  - the four Func_80a2268 tails in the i-loop are one shared block at .La948e
 *    entered with r2/r3 preset: find_cross_jump merging identical call tails,
 *    so each arm gets its OWN call in the source and the merge is automatic.
 *  - the trailing dispatch is a 4-way switch on `info[2]` emitted as a
 *    comparison TREE, and its case BODIES are laid out 1, 4, 3, <shared tail>,
 *    2, so source order is 1, 4, 3, 2 -- with case 2's tail duplicated because
 *    cross-jumping merged only the first three.
 *  - `mov r3,#0x80 / lsl r3,#2` is the shiftable-constant expansion of 0x200.
 *
 * BLOCKER -- global.c `allocno_compare`, and the .18.greg dump names it
 * exactly.  That dump's `;; 17 regs to allocate:` line IS the sorted priority
 * order, and `Register dispositions` gives the result.  REG_ALLOC_ORDER is
 * {3,2,1,0,12,14,4,5,6,7,8,10,9,11}, and with -fcall-used-r4 the call-crossing
 * candidates take r5, r6, r7, r8, r10, r9, r11 in that order.  So the priority
 * order is readable straight off the assignment:
 *     ROM   y  >  win  >  i  >  state  >  d  >  n5/rowptr  >  one
 *     ours  n5 >  i    >  win >  state  >  y  >  d         >  one
 * Two adjacent swaps, y<->n5 and win<->i.  Nothing in the source moved them:
 *   n5 declared first instead of last ...................... 103 (no change)
 *   an explicit `y` local incremented by 2 ................. 102 (length 243)
 *   both ................................................... 102 (length 243)
 *   a local copy of `win` assigned first ................... 103 (no change)
 *   `y` declared immediately after `state` ................. 102 (length 243)
 * Every explicit-`y` variant trades one instruction of length for one of
 * distance, so none of them is progress by the batch-292 rule.
 *
 * WHAT IS RIGHT: everything except register names.  All 16 relocations are the
 * ROM's, in the ROM's order, with the single extra `_MSG_75` word noted above;
 * size and instruction count are exact; the frame, both loop shapes, the
 * cross-jumped tail, the switch tree and the epilogue all line up.  This is the
 * closest of the four targets in this brief and the one to reopen first.
 */
extern unsigned char *iwram_3001f2c;
extern int _MSG_75;
extern void _Func_8016498(unsigned int win);
extern void WaitFrames(int n);
extern void _Func_801e7c0(int msg, unsigned int win, int x, int y);
extern unsigned char *_GetItemInfo(int item);
extern void _Func_8019000(unsigned int win, int a, int b, int c, int d);
extern void Func_80a2268(unsigned int win, int a, int b, int c, int d, int e);

int Func_80a93a4(unsigned int win, int a1, int *d)
{
    unsigned char *state;
    unsigned short *p;
    int ofs;
    int i;
    int t;
    int v;
    int n5;

    state = iwram_3001f2c;
    n5 = d[2] * 5;
    d[6] = n5 + d[4];
    _Func_8016498(*(unsigned int *)(state + 0x2c));
    WaitFrames(1);
    ofs = d[6] * 2 + 0xe4 * 2;
    if (*(unsigned short *)((int)state + ofs) != 0)
        _Func_801e7c0((*(unsigned short *)((int)state + ofs) & 0x1ff) + (int)&_MSG_75,
                      *(unsigned int *)(state + 0x2c), 0, 0);
    p = (unsigned short *)((int)state + n5 * 2 + 0xe4 * 2);
    for (i = 0; i <= 4; i++) {
        if (i == d[4]) {
            ofs = d[6] * 2 + 0xe4 * 2;
            t = *(int *)(_GetItemInfo(*(unsigned short *)((int)state + ofs) & 0x1ff) + 0x14);
            if (t != 4) {
                _Func_8019000(win, t + 1, 0x1b, i * 2 + 1, 0);
                Func_80a2268(win, 0xe, i * 2 + 1, 0xd, 1, 0xe);
            } else {
                Func_80a2268(win, 0xe, i * 2 + 1, 0xe, 1, 0xe);
            }
        } else {
            t = *(int *)(_GetItemInfo(*p & 0x1ff) + 0x14);
            if (t != 4) {
                _Func_8019000(win, t + 1, 0x1b, i * 2 + 1, 4);
                Func_80a2268(win, 0xe, i * 2 + 1, 0xd, 1, 0xf);
            } else {
                Func_80a2268(win, 0xe, i * 2 + 1, 0xe, 1, 0xf);
            }
        }
        p++;
    }
    for (i = 0; i < 4; i++)
        Func_80a2268(*(unsigned int *)(state + 0x30), 1, i * 2 + 1, 0xc, 1, 0xf);
    ofs = d[6] * 2 + 0xe4 * 2;
    v = *(unsigned short *)((int)state + ofs);
    if ((v & 0x200) != 0) {
        switch (_GetItemInfo(v & 0x1ff)[2]) {
        case 1:
            Func_80a2268(*(unsigned int *)(state + 0x30), 1, 1, 0xc, 1, 0xe);
            break;
        case 4:
            Func_80a2268(*(unsigned int *)(state + 0x30), 1, 3, 0xc, 1, 0xe);
            break;
        case 3:
            Func_80a2268(*(unsigned int *)(state + 0x30), 1, 5, 0xc, 1, 0xe);
            break;
        case 2:
            Func_80a2268(*(unsigned int *)(state + 0x30), 1, 7, 0xc, 1, 0xe);
            break;
        }
    }
    WaitFrames(1);
    return 1;
}
