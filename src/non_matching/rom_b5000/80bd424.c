/* Func_80bd424 (ChooseAction)  --  0x080bd424.  NON-MATCHING.
 * IMPROVED IN BATCH 316: 405 instructions+pool words against the reference's
 * 414, 333 differing +/- lines, down from 399 / 337.  Func_80bd3e4 stays
 * BYTE-IDENTICAL, 30 of 30, emitted as the nested local `Func_80bd3e4.0` ahead
 * of its parent exactly where the ROM has it.  ZERO SHIMS.  Production flags.
 *
 * Verify with (per symbol -- objcmp cannot isolate a nested parent, see THE
 * SYMBOL CAVEAT below; this is the same recipe as the 80b9604 TU park):
 *   R=asm/rom_b5000/rom_bbb0c_a_a_c.s
 *   (head -2 $R; sed -n '3,513p' $R) > /tmp/refbd.s     # Func_80bd3e4+Func_80bd424
 *   /opt/gcc296/xgcc -B/opt/gcc296/ -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi \
 *     -fno-builtin -nostdinc -ffreestanding -fcall-used-r4 -Iinclude -S \
 *     -o /tmp/cbd.s src/non_matching/rom_b5000/80bd424.c
 *   printf '\n\t.text\n\t.align\t2, 0\n' >> /tmp/cbd.s
 *   for f in refbd cbd; do arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork \
 *     -Iinclude -o /tmp/$f.o /tmp/$f.s; arm-none-eabi-objdump -d \
 *     --no-show-raw-insn /tmp/$f.o > /tmp/$f.txt; done
 *   # then diff the two per symbol with branch targets and pool offsets normalised
 *   # (scratch_elev/b316e/score.py does exactly this; scorebd.py is it with
 *   #  FUNCS = ["Func_80bd3e4", "Func_80bd424"])
 *
 * *** RESIDUE A'S VERDICT WAS WRONG, AND IT WAS WRONG AGAINST THIS PARK'S OWN
 * *** ITEM 3.  The park read the kept sign extension at info+0x35 as
 * "not a spelling problem ... we cannot make it need one".  IT IS A SPELLING
 * PROBLEM, and the spelling is the one item 3 already uses for the packed byte
 * at unit+0x120: WRITE THE SHIFTS IN THE SOURCE.
 *
 *     ROM, at all three sites:  add r3,#0x35 / ldrb r3,[r3] / lsl r3,#24 /
 *                               asr r3,#24 / cmp r3,#0 (or #2)
 *     was:  si[0x35] == 0            -> ldrb + cmp, combine folds the extension
 *     now:  (si[0x35] << 24) >> 24 == 0
 *
 * That is +6 instructions in exactly the right opcodes -- the per-opcode
 * histogram over ENCODINGS goes `asrs` 1 -> 4 (reference 5) and `lsls` 9 -> 12
 * (reference 15) -- so it is a real gain and not a measurement artefact.  Three
 * spellings are BYTE-IDENTICAL to each other: the shifts on the existing `s8 *`,
 * the same shifts through a separate `u8 *`, and a `struct { signed int f : 8; }`
 * cast at the three sites.  The park says it tried "a `signed int f : 8`
 * bitfield"; whatever it tried, this measures 333.
 * INERT OR WORSE on this baseline: `(s8)` on a `u8` load, `*(s8 *)(info+0x35)`,
 * `(si[0x35] | 0)` -- all byte-identical to the OLD 337.
 *
 * STILL 9 ENCODINGS SHORT, and the histogram now names them.  Reference against
 * ours: ldr 32/29, ldrh 10/7, b 25/21, lsls 15/12, asrs 5/4, ldrsb 1/0, bge 1/0,
 * bne 16/15, mov 34/33, pool words 14/13; and we are OVER on adds 18/14,
 * ldrsh 13/11, ldrb 17/16, beq 27/26, bgt 4/3, movs 52/51.
 *   * `ldrsh` 13 against 11 with `ldrh` 7 against 10 says two of our halfword
 *     reads are signed where the ROM's are not.  CROSSED AND ALL NEGATIVE:
 *     `p` as `u16 *` or `unsigned short *` (375 instructions, 379), `(u16)p[0]`
 *     at the call arguments (394, 416), `(u16)p[0]` at `_GetUnit` only (383).
 *     18 variants, three axes crossed; nothing beats 333.
 *   * `int act` for `short act` is now exactly INERT (333, same instruction
 *     count).  The park's item 2 evidence for `short` -- the reference's
 *     `ldr r2, =1` pool load of an HImode constant -- still stands, so keep
 *     `short`; but the park's "much worse" figure for the carrier no longer
 *     reproduces on this baseline.
 *   * `bge` 1 against 0 with `bgt` 4 against 3 is one more instance of item 6's
 *     fused/split comparison rule, at a site item 6 did not reach.
 *   * `b` 25 against 21 is four unconditional branches, i.e. four more
 *     out-of-line arms or shared tails in item 4's shape.
 * Those three are the next work and each is independently checkable off the
 * histogram, which is the instrument this park was missing.
 * =========== THE FINDING: Func_80bd3e4 IS A NESTED FUNCTION ===========
 *
 * src/non_matching/rom_b5000/80bd3e4.c is parked at 8 of 32 with a byte-exact
 * body and the whole residue in the prologue and epilogue, and its verdict is
 * "WHY THE FRAME IS NOT REACHABLE ... a property of the ORIGINAL translation
 * unit's intermediate state, not of its C, and there is nothing in the source to
 * recover it from."  *** THAT VERDICT IS WRONG AND THIS FILE IS THE COUNTER-
 * EXAMPLE. ***  Func_80bd3e4 is a GNU C nested function inside Func_80bd424, and
 * writing it that way emits all eight of its missing instructions.
 *
 * gcc-2.96's arm.h sets STATIC_CHAIN_REGNUM to r9 under Thumb.  The tell is the
 * triple docs/elevation.md already documents -- r9 saved and restored, a
 * `mov rX, r9` nothing defines, one stack slot never read back -- and the CALLER
 * settles it.  Func_80bd424 does, in each of the three cases that call it:
 *     add r3, sp, #0x1c      @ sp + the frame size, i.e. the caller's frame base
 *     mov r9, r3
 *     bl  Func_80bd3e4
 * which is gcc handing a callee a pointer into the caller's own frame.  A
 * five-line control compiled here reproduces both sides verbatim:
 *     pick.0:  push {r5, lr} / mov r5, r9 / push {r5} / mov r3, r9 /
 *              sub sp, #4 / str r3, [sp]   ... / add sp, #4 / pop {r3} /
 *              mov r9, r3 / pop {r5} / pop {r1} / bx r1
 *     outer:   ... add r3, sp, #28 / mov r9, r3 / bl pick.0
 * The `str r3, [sp]` the park called dead is the chain slot; the frame exists to
 * hold it.  So the park's residue was never about register pressure or an
 * addressable local -- BOTH halves are explained by one construct, and the fix is
 * the whole-file job docs/elevation.md's static-chain section prescribes.
 *
 * Nothing outside this .s references Func_80bd3e4 (grep over asm/ and src/), so
 * nesting costs no external symbol.
 *
 * ============================== SPLIT SHAPE ==============================
 *
 * `python3 tools/datacheck.py asm/rom_b5000/rom_bbb0c_a_a_c.s` prints NOTHING:
 * the file has NO data section, so there is no text/data split to do.  The jump
 * table at .Lbd4a0 is `.word`s inside `.text` between two basic blocks of
 * Func_80bd424 and is not data in datacheck's sense.  `.Lc2b80/.Lc2b88/.Lc2b90`
 * live in asm/rom_b5000/rom_bbb0c_c_c.s and are ALREADY `.global` there, so no
 * export is needed either.
 *
 * What IS needed is a TEXT split, and it has a constraint no other split in the
 * tree has had: the file holds Func_80bd3e4, Func_80bd424 and Func_80bd7a4, and
 * THE FIRST TWO MUST STAY IN ONE OBJECT because one is nested inside the other.
 * So the cut is between Func_80bd424 and Func_80bd7a4 -- two pieces, not three:
 *     piece A  Func_80bd3e4 + Func_80bd424   -> becomes this .c
 *     piece B  Func_80bd7a4                  -> stays .s
 * No local label crosses that boundary (checked: every .L in Func_80bd7a4's body
 * is defined inside it, and .Lbd4a0/.Lbd414/.Lbd404 are inside piece A).
 *
 * THE SYMBOL CAVEAT.  A nested function's assembler name is the local
 * `Func_80bd3e4.0`, so the parent's call needs no relocation where the ROM has
 * `R_ARM_THM_CALL Func_80bd3e4`, and objcmp's `--func Func_80bd424` cannot find
 * our symbol table entry for the parent either -- it falls back to the WHOLE
 * object and so compares 433 encodings (both functions) against the reference's
 * isolated 417.  That is the "ours 928 bytes / 433 encodings" line above and it
 * means nothing.  Read the per-symbol figures, and when this lands read
 * `make compare`, exactly as docs/elevation.md says for this class.  The
 * relocation SEQUENCE is otherwise identical: _GetUnit, _GetEnemyInfo, the seven
 * jump-table `.text` words, _RPGRandom, _GetItemInfo, _GetMoveInfo, _GetMoveInfo,
 * Func_80b9a70, Func_80bd3c8, the `.Lbd4a0` pool word with .Lc2b80/88/90,
 * Func_80bae40, Func_80bad7c, Func_80bae40, Func_80bad7c, Func_80b9a70,
 * Func_80b9a70 -- and five of the last six are at the SAME OFFSETS as the
 * reference.
 *
 * ================== WHAT LANDED, EACH MEASURED SINGLY ==================
 *
 * The first spelling was 416 instructions against 400 and 38.1% aligned.  Four
 * changes took it to 402 and 48.9%; the aligned scale is the only one that can
 * rank them, since the count is short throughout.
 *
 * 1. AN `int` CARRIER FOR EVERY CONSTANT u16 STORE.  `p[3] = 4` written directly
 *    emits `ldrh r3, .L82 / strh r3, [r6,#6]` -- gcc-2.96 has no immediate
 *    alternative for an HImode constant, so it POOLS it, which is
 *    src/rom_b5000/rom_b9b30_a_a.c's finding in its second form: there it was
 *    `*arg0 = 0xff`, here it is nine separate stores.  `t = 4; p[3] = t;` gives
 *    the ROM's `mov r3, #4 / strh`.  This was the single largest item.
 * 2. `short act` IS CONFIRMED BY A POOL LOAD OF 1.  The reference has
 *    `ldr r2, =1` at .Lbd698 where `mov r2, #1` would do, which is the same
 *    HImode-constant rule seen from the other side: the destination is a
 *    REGISTER of HImode, so even 1 is pooled.  That is the evidence that the
 *    action code is carried in a `short` local and not an int, and it is why the
 *    reference compares it with `lsl r3, r2, #16 / cmp r3, r1` (gcc's HImode
 *    compare) and sign-extends it with `lsl #16 / asr #16` before the int
 *    comparisons at .Lbd69a.
 * 3. BITFIELD-SHAPED SHIFTS FOR THE PACKED BYTE AT unit+0x120.  `if ((w & 1) == 0)`
 *    gives `mov r3,#1 / and / cmp`; the ROM has `lsl r3, r2, #31 / cmp r3, #0`,
 *    so it is written `if ((w << 31) == 0)`.  `(w >> 1) & 7` gives `lsr / mov #7 /
 *    and`; the ROM has `lsl r3, r2, #28 / lsr r3, #29`, so it is written
 *    `(w << 28) >> 29`.  Both now match instruction for instruction.
 * 4. CASES 3 AND 4 SHARE THEIR TAIL THROUGH A LABEL INSIDE THE switch.  The ROM's
 *    case 4 falls into .Lbd508 and case 3 branches to it, with case 3 skipping
 *    case 4's re-read of the word when bit 0 was already set.  Written as two
 *    independent cases the tail is DUPLICATED (+20 instructions) because
 *    cross-jumping runs after reload and the two copies got different registers.
 *    A `goto common;` in case 3 with `common:` inside case 4 is the exact shape
 *    and is legal C.
 * 5. `int off = choice * 2 + 0x38; move = *(u16 *)(info + off);`  Written as one
 *    expression gcc folds the 0x38 into the load offset (`add r3,r3,r0 /
 *    ldrh r3,[r3,#56]`); named, the index goes into a register and the ROM's
 *    `lsl r3,r1,#1 / add r3,#0x38 / ldrh r3,[r2,r3]` comes out.
 * 6. THE RANGE TEST SPLIT.  `if (v > 2 || v < 1) hit = 0;` fuses into one
 *    comparison (`add r3,#255 / lsl r3,#24 / lsl r2,#17 / cmp r3,r2`); two
 *    separate `if`/`else if` statements give the ROM's `cmp #2 / bgt` then
 *    `cmp #1 / bge`.  This is docs/elevation.md's fused-condition rule, confirmed
 *    again.
 * 7. THE CALL RESULT NEEDS ITS OWN LOCAL.  Reusing one `int` for both the
 *    Func_80bae40/Func_80bad7c results and the byte-mask scratch costs a
 *    `mov r5, r0` after every call; a dedicated `res` keeps the value in r0 the
 *    way the reference does.
 * 8. THE RANDOM CALL'S RESULT MUST NOT BE NAMED SEPARATELY FROM ITS MASK.
 *    `t = _RPGRandom(); v = st[0];` puts the `ldrb` BEFORE the call and adds a
 *    copy.  `rnd = _RPGRandom() & 7; b = st[0];` puts the load after the call and
 *    applies the `and` in place, as the reference does.  Worth 2.6 points alone.
 * 9. `hit` IN TWO STATEMENTS.  `hit = ((mask[0] >> choice) & 1) & ok;` loses the
 *    `& 1` entirely -- gcc proves it redundant against `ok`'s value set.  Split
 *    into `hit = (mask[0] >> choice) & 1; hit &= ok;` both `and`s survive.
 *
 * MEASURED AND INERT: declaring the spilled locals in the reference's slot order
 * (info, ok, tries, again, mode, mask -- the reverse of the frame layout, per
 * rom_b9b30_a_a.c's rule); `unsigned short act` (38.1%, much worse); `p` as
 * `unsigned short *` with `(short)` casts at the four `ldrsh` sites (identical);
 * reading info[0x35] through an `int` temp, through an `s8 *`, or with a cast at
 * the use (all identical).
 *
 * ================== THE RESIDUE, BY PASS ==================
 *
 * A. A SIGN EXTENSION gcc DELETES AND THE ROM KEEPS -- 6 of the 16 missing
 *    instructions, at three sites.  The reference reads the signed byte at
 *    info+0x35 as
 *        ldr r3,[sp,#0x14] / add r3,#0x35 / ldrb r3,[r3] / lsl r3,#24 /
 *        asr r3,#24 / cmp r3,#0
 *    and we get the `ldrb` and the `cmp` with nothing between, because combine
 *    proves the extension cannot change `== 0` or `== 2` for a byte.  Every
 *    spelling tried -- `s8 *` indexing, `(s8)` on a u8 load, an `int` temp, an
 *    `s8` temp, a `signed int f : 8` bitfield -- either folds the same way or
 *    (when the address can take a register index) becomes `mov r3,#0x35 /
 *    ldrsb r3,[r0,r3]`, which is a different pair of instructions again.  The
 *    reference's form is what gcc emits when it must sign-extend AND has no spare
 *    register for a zero index; we cannot make it need one.  This is the same
 *    family as the register-pressure residues elsewhere in the batch and it is
 *    not a spelling problem.
 * B. `act` IS REMATERIALISED, NOT ALLOCATED.  The reference loads it once per
 *    branch (`ldrh r2,[r7,#6]`) and keeps it in r2 through the whole tail; we
 *    re-read it with `mov r3,#6 / ldrsh r2,[r6,r3]` at two later sites.  Worth
 *    about 4 instructions.  The HImode compares are right (item 2) -- it is the
 *    allocation that differs.
 * C. A THREE-WAY CALLEE-SAVED ROTATION, which is most of the 213 differing
 *    encodings even where the structure matches:
 *        ROM   r5 = the info pointer   r6 = hit   r7 = the order pointer
 *        ours  r5 = hit               r6 = p     r7 = mi
 *    with `u` in r11 both times and `st`/`move`/`choice` in r8/r9/r10 both times.
 *    Same `allocno_compare` tie-break as the other park in this batch.
 * D. TWO MID-FUNCTION LITERAL POOLS.  The reference emits `.pool_aligned` twice
 *    inside the body (after .Lbd674 and after .Lbd6ba); we emit one pool at the
 *    end.  That is a consequence of C and A -- the function is 32 bytes shorter,
 *    so no branch is far enough to force a pool -- and it moves the `.Lbd4a0`
 *    pool word from offset 0x258 to 0x338.  It will follow the length, not lead
 *    it.
 *
 * ========================= WHAT IT IS =========================
 *
 * The battle AI's per-turn decision.  Takes the order record (a small halfword
 * struct: id at 0, action at 6, argument at 8, target at 0xa, cost at 0xc) and a
 * flag that distinguishes a real turn from a rehearsal.  Loops -- up to 0x11
 * attempts, then forces action 3 -- over: pick a behaviour slot for this unit
 * (cases 0..2 roll the weighted table with the nested Func_80bd3e4, cases 3 and 4
 * read and rewrite a 3-bit rotation counter packed at unit+0x120, case 5 just
 * advances it); look the slot's move up in the enemy record; on a real turn check
 * the equipped item at unit+0xd8 and substitute its move; classify the move
 * (0x2e/0x2f/0x31 are special targeting), price it with Func_80bae40 or fall back
 * to Func_80bad7c and Func_80b9a70, and write the chosen action, argument, target
 * and cost back into the order.  Action 2 short-circuits the pricing, 0x1fd is
 * the "nothing usable" action, and Func_80bd3c8 decides whether the move is worth
 * an item slot.
 *
 * ===== BATCH 330: A GUARD AGAINST THIS PARK BEING BRIEFED AS A SMALL ONE =====
 * This park was handed to an agent in batch 330 as a short function with a
 * single-digit residue, on a pair of numbers lifted out of the SIBLING park
 * src/non_matching/rom_b5000/80bd3e4.c -- a pair that park itself labels wrong
 * (it measures thirty standalone) and that is moot anyway, since Func_80bd3e4 is
 * a nested function and is byte-identical when written that way.  This function
 * is nothing like that size: its residue is in the hundreds of differing lines
 * over four hundred-odd instructions and pool words, and `parkcheck' cannot
 * score it at all -- it reports this file as needing the per-symbol recipe above,
 * because a nested parent has no isolatable symbol.  So the only figure for this
 * park is the one in its own opening paragraph, reached through that recipe, and
 * the sibling's prose must not be read as this park's distance.
 * THE NEXT WORK IS STILL THE THREE HISTOGRAM ITEMS NAMED ABOVE: the two signed
 * halfword reads that should be unsigned, the fused-versus-split comparison at
 * the site item six did not reach, and the four unconditional branches that are
 * missing out-of-line arms or shared tails.  Each is checkable off the per-opcode
 * histogram on its own.
 *
 * AND THE SPLIT HAS ALREADY HAPPENED, SO THE SPLIT SHAPE SECTION ABOVE IS NOW
 * HISTORY, as is this park's opening recipe -- which names a piece that no
 * longer exists on disk, and therefore cannot be run at all:
 *   asm/rom_b5000/rom_bbb0c_a_a_c_a.s  holds exactly Func_80bd3e4 then
 *     Func_80bd424, in five hundred and thirteen lines -- the pair, alone, in one
 *     piece, which is the cut the section above prescribes.  Compare against this
 *     file WHOLE; no line range is needed any more.
 *   asm/rom_b5000/rom_bbb0c_a_a_c_b.s  carries gcc's own banner and names a .c
 *     file in its .file directive, because Func_80bd7a4 has ALREADY LANDED as
 *     src/rom_b5000/rom_bbb0c_a_a_c_b.c.
 * So the install path for a future landing of the nested pair is
 * src/rom_b5000/rom_bbb0c_a_a_c_a.c, and nothing about the layout is outstanding:
 * what is left is entirely the three histogram items.
 */
#include "gba/types.h"

extern u8 *_GetUnit(int id);
extern u8 *_GetEnemyInfo(int id);
extern u8 *_GetItemInfo(int item);
extern u8 *_GetMoveInfo(int id);
extern int _RPGRandom(void);
extern int Func_80bd3c8(int move);
extern int Func_80b9a70(int id);
extern int Func_80bae40(int id, u8 *mi);
extern int Func_80bad7c(int near);

extern u8 Lc2b80[] __asm__(".Lc2b80");
extern u8 Lc2b88[] __asm__(".Lc2b88");
extern u8 Lc2b90[] __asm__(".Lc2b90");

void Func_80bd424(short *p, int flag)
{
    u8 *u;
    u8 *info;
    s8 *si;
    u8 *st;
    s8 *mode;
    u8 *mask;
    u8 *mi;
    int tries;
    int again;
    int ok;
    int choice;
    int move;
    int hit;
    int t;
    int v;
    int off;
    int res;
    short act;
    u32 w;
    int rnd;
    int b;
    int sv;

    int Func_80bd3e4(u8 *tbl)
    {
        int rr;
        int acc;
        int i;
        int ret;

        rr = _RPGRandom() & 0xff;
        acc = tbl[0];
        ret = 0;
        i = 0;
        if (rr >= acc) {
            for (;;) {
                i++;
                if (i > 7)
                    break;
                acc += tbl[i];
                if (rr < acc) {
                    ret = i;
                    break;
                }
            }
        }
        return ret;
    }

    u = _GetUnit(p[0]);
    tries = 0;
    ok = 1;
    again = 1;
    choice = -1;
    if (u[0x129] != 0)
        return;
    if (flag != 0 && p[3] != 4)
        return;
    info = _GetEnemyInfo(u[0x94 << 1]);
    si = (s8 *)info;
    mode = (s8 *)(info + 0x36);
    mask = info + 0x37;
    st = u + (0x90 << 1);
    do {
        switch (mode[0]) {
        case 0:
            choice = Func_80bd3e4(Lc2b80);
            break;
        case 1:
            choice = Func_80bd3e4(Lc2b88);
            break;
        case 2:
            choice = Func_80bd3e4(Lc2b90);
            break;
        case 3:
            w = *(u32 *)st;
            if ((w << 31) == 0) {
                rnd = _RPGRandom() & 7;
                b = st[0];
                st[0] = (b & ~0xe) | (rnd << 1) | 1;
                w = *(u32 *)st;
            }
            goto common;
        case 4:
            w = *(u32 *)st;
        common:
            choice = (w << 28) >> 29;
            if (flag != 0) {
                t = (choice + 1) & 7;
                v = st[0];
                st[0] = (v & ~0xe) | (t << 1);
            }
            break;
        case 5:
            choice += 1;
            break;
        case 6:
            break;
        }

        hit = (mask[0] >> choice) & 1;
        hit &= ok;
        off = choice * 2 + 0x38;
        move = *(u16 *)(info + off);
        t = 4;
        p[3] = t;
        if (hit != 0 && flag != 0) {
            u16 *eq = (u16 *)(u + 0xd8);
            if ((*eq & 0x1ff) == 0) {
                hit = 0;
                if ((si[0x35] << 24) >> 24 == 0) {
                    t = 2;
                    p[3] = t;
                    t = 0x1fd;
                    p[4] = t;
                    return;
                }
            }
            if (hit != 0) {
                mi = _GetItemInfo(*eq);
                if (mi[0xc] == 1) {
                    u8 *mv = _GetMoveInfo(*(u16 *)(mi + 0x28));
                    t = 2;
                    p[3] = t;
                    move = *(u16 *)(mi + 0x28);
                    t = 0;
                    p[4] = t;
                    v = mv[1];
                    if (v > 2)
                        hit = 0;
                    else if (v < 1)
                        hit = 0;
                } else {
                    hit = 0;
                }
            }
            if (hit == 0)
                ok = 0;
        }

        if (again != 0) {
            mi = _GetMoveInfo(move);
            switch (mi[3]) {
            case 0x2e:
                t = 3;
                p[3] = t;
                p[5] = Func_80b9a70(p[0]);
                break;
            case 0x2f:
                t = 7;
                p[3] = t;
                p[5] = Func_80b9a70(p[0]);
                break;
            case 0x31:
                t = 0x63;
                p[3] = t;
                p[5] = Func_80b9a70(p[0]);
                break;
            }
            if (flag == 0) {
                act = p[3];
                if (p[3] != 3 && p[3] != 7)
                    return;
            } else {
                act = p[3];
            }
            if (act != 2) {
                if (Func_80bd3c8(move) != 0) {
                    t = 1;
                    p[3] = t;
                    p[4] = move;
                    if (mi[9] > *(short *)(u + 0x3a) && (si[0x35] << 24) >> 24 != 0)
                        goto next;
                    if (u[0x13d] != 0 && (si[0x35] << 24) >> 24 == 2)
                        goto next;
                    act = 1;
                } else {
                    act = p[3];
                }
            }
            if (act == 0x63 && u[0xa4 << 1] != 0)
                goto next;
            if (flag != 0) {
                if (act == 3)
                    goto next;
                if (act == 7)
                    goto next;
            }
            if (act == 4) {
                p[4] = move;
                if (move == 1) {
                    t = 0;
                    p[3] = t;
                }
            }
            p[6] = mi[8];
            switch (mi[0]) {
            case 2:
            case 4:
                res = Func_80bae40(p[0], mi);
                if (res == -2)
                    res = Func_80bad7c((unsigned short)p[0] <= 7);
                if (res == -1)
                    break;
                p[5] = res;
                again = 0;
                break;
            case 1:
                res = Func_80bae40(p[0], mi);
                if (res == -2)
                    res = Func_80bad7c((unsigned short)p[0] <= 7);
                if (res == -1)
                    break;
                p[5] = res;
                again = 0;
                break;
            case 3:
                p[5] = Func_80b9a70(p[0]);
                break;
            default:
                res = Func_80b9a70(p[0]);
                p[5] = res;
                again = 0;
                break;
            }
        }
        if (flag == 0)
            again = 0;
    next:
        if (again != 0 && tries > 0x10) {
            t = 3;
            p[3] = t;
            again = 0;
        }
        tries++;
    } while (again != 0);
}
