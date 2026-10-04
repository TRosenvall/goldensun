/* Func_80ab21c (FillTilemapRect, 0x080ab21c) -- NON-MATCHING, 2 of 101 encodings.
 *
 * Figure RE-MEASURED this batch (brief 321-B), not inherited.  107 instructions
 * against the ROM's 107, every register identical.  PIN COUNT 0
 * (tools/shimcount.py reports nothing of either class).
 *
 * Verify with -- NAMES THE INSTALLED PATH:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80ab21c.c \
 *     asm/rom_a1000/rom_aa538_c_c_a_c_c.s --func Func_80ab21c
 *     -> XX ENCODINGS differ in 2 place(s) (ref 101, ours 101)
 *           first at index 82: ref 3d01  ours 9b00
 *
 * `--whole` IS MEANINGLESS UNTIL THE SPLIT, and saying so is the point: the
 * reference holds THREE functions, so --whole reports
 * "FUNCTIONS/ORDER differ, ref [Func_80aafb8, Func_80ab1f4, Func_80ab21c]" and a
 * SIZE line of 820 against 208.  Per-function it still says
 * "Func_80ab21c  2 of 101 differ (ours 101), first at index 82".
 *
 * SPLIT SHAPE, from the tool and not by counting by hand.  tools/datacheck.py is
 * silent on the reference (no data section), and
 *   tools/split_s.py asm/rom_a1000/rom_aa538_c_c_a_c_c.s Func_80ab21c --dry-run
 *     [dry-run] would write asm/rom_a1000/rom_aa538_c_c_a_c_c_a.s  (2 functions, 309 lines)
 *     [dry-run] would write asm/rom_a1000/rom_aa538_c_c_a_c_c_b.s  (1 function, 114 lines)
 *     [dry-run] would REMOVE asm/rom_a1000/rom_aa538_c_c_a_c_c.s
 *     [dry-run] would rewrite stage1.ld
 * so a landing installs to src/rom_a1000/rom_aa538_c_c_a_c_c_b.c.  NO exports
 * are needed -- it is a text-only split.
 *
 * THE RESIDUE, ONE ADJACENT SWAP AT THE OUTER-LOOP TAIL:
 *   rom   strb r2,[r1] / sub r5,#1     / ldr r3,[sp]
 *   ours  strb r2,[r1] / ldr r3,[sp]   / sub r5,#1
 *
 * ================================================================
 * WHICH RUNG DECIDES, READ OFF -fsched-verbose=6
 * ================================================================
 * .23.sched2, basic block 16 (`-- basic block 16 from 211 to 177 --`):
 *
 *   ;;      insn  code    bb   dep  prio  cost   blockage units
 *   ;;      164   189     0     3     5     2    1 - 32   core : 177 294 291     <- strb r2,[r1]
 *   ;;      167     5     0     0     2     1    1 - 32   core : 177             <- sub r5,#1   (h--)
 *   ;;      291   173     0     3     4     2    1 - 32   core : 177 294 170     <- ldr r3,[sp] (off reload)
 *   ;;      170     5     0     1     2     1    1 - 32   core : 177 294         <- add r3,#0x40
 *   ;;      294   173     0     4     1     2    1 - 32   core : 177             <- str r3,[sp]
 *   ;;      173     5     0     1     1     1    1 - 32   core : 177             <- add r0,#1   (y++)
 *   ;;      177   203     0    12     1     1    1 - 32   core :                 <- bne
 *   ;;      Ready list (t =  9):    173  167  291
 *   ;;              --> scheduling insn <<<291>>>          (ROM picks 167)
 *
 *   rung 1 PRIORITY   priority(291) = 4,  priority(167) = 2.
 *          ***THIS RUNG DECIDES, AND IT RETURNS BEFORE ANY TIE-BREAK.***
 *          No class, no dependent count and no LUID is consulted at all, so no
 *          lever aimed at those rungs can reach this window.
 *
 * WHERE THOSE TWO NUMBERS COME FROM, AND WHY BOTH ARE FORCED BY THE ROM:
 *   priority(167) = cost(167->177) + priority(177) = 1 + 1 = 2.  `h--` has
 *     EXACTLY ONE forward dependence in the block, the loop branch, and the
 *     branch is the block's last insn so its priority is 1.  Raising this needs
 *     `h--` to head a true-dependence chain of length >= 4, which needs
 *     instructions the ROM's 107 does not have.
 *   priority(291) = cost(291->170) + priority(170) = 2 + 2 = 4, over the chain
 *     `ldr r3,[sp] -> add r3,#0x40 -> str r3,[sp]`.  That chain is reload's
 *     load/update/store of a SPILLED `off`, and THE ROM SPILLS IT TOO -- the ROM
 *     has the same `ldr r3,[sp]`, `add r3,#0x40` and `str r3,[sp]` at indices
 *     83-86, which is why only two encodings differ.  Lowering it to <= 1 would
 *     need priority(170) <= -1.
 *   The RTL confirms the shapes:
 *     (insn 291 164 167 (set (reg:SI 3 r3) (mem:SI (reg:SI 13 sp) 0)) ...
 *          (insn_list 164 (insn_list:REG_DEP_OUTPUT 157 (insn_list:REG_DEP_ANTI 161 (nil)))))
 *     (insn 167 291 170 (set (reg/v:SI 5 r5) (plus:SI (reg/v:SI 5 r5) (const_int -1))) ... (nil))
 *
 * THE REMAINING ESCAPE, AND WHY THE BLOCK HAS NO ROOM FOR IT.  291 is ready at
 * t = 9 because 164 (the `strb`) is scheduled at t = 7-8 and a store's result is
 * available after 2.  If 164 landed one cycle later, 291 would not be ready at
 * t = 9 and 167 would be taken -- the ROM's order.  But the visualisation shows
 * the block running 0..16 with ONE free slot and `total time = 16` in BOTH
 * orders, so there is nothing to spend: 0 lsr, 1 mov r1, 2 mov r2, 3 lsl,
 * 4-5 ldrb, 6 orr, 7-8 strb, 9-10 ldr, 11 sub, 12 add, 13-14 str, 15 add r0,
 * 16 bne.  Two attempts to lengthen the front of the block measured WORSE (below).
 *
 * *** AND A BOUND WITH ITS EVIDENCE ATTACHED, because the brief's retracted
 * *** alias-set bound makes this worth stating carefully.  The edge that holds
 * *** 291 after 164 is a TRUE memory dependence from a byte store in alias set 0
 * *** to a stack load -- exactly the class the brief says to attack.  IT CANNOT
 * *** BE ATTACKED HERE, FOR TWO SEPARATE REASONS, AND NEITHER IS "every one-byte
 * *** C type is a character type":
 * ***   (a) the other MEM is a RELOAD SPILL SLOT and it prints its alias set in
 * ***       the dump: `(mem:SI (reg:SI 13 sp) 0)`.  DIFFERENT_ALIAS_SETS_P needs
 * ***       BOTH sets non-zero, so no declaration on the stored-to object can
 * ***       make it fire against a spill slot.
 * ***   (b) even if the edge went away it would make things WORSE, not better:
 * ***       291 would become ready EARLIER, not later, and the decision is on
 * ***       priority anyway.
 * *** So: alias-set work is live in general (it landed three functions in batch
 * *** 318) and is simply not the lever for a spill-slot competitor.
 *
 * MEASURED THIS BATCH (tools/crossfire.py, 7 edits to depth 2, 101 encodings):
 *   base .................................................... 2
 *   `d` taken as a real STRUCT MEMBER (see below) ........... 2   exactly inert
 *   `} while (--h != 0);` with `h--` removed ................ 2   exactly inert
 *   an extra unused `unsigned int` local .................... 2   exactly inert
 *   those three crossed pairwise ............................ 2   exactly inert
 *   `*d |= ...` moved AFTER `h--; off += 0x40;` ............. 11
 *   the shift value named in a local, `*d |= n;` ............ 14
 *   both of those together .................................. 23
 *
 * A DECLARATION DIVIDEND, EXACTLY INERT, AND WORTH TAKING ANYWAY.  The brief's
 * pattern 4 (declare the object with its real type instead of doing raw-offset
 * arithmetic) applies to `d = base + 0xea3`, and it is byte-identical:
 *     struct Map { u8 pad0000[0xea3]; u8 rowFlags; };
 *     d = &((struct Map *)base)->rowFlags;
 * measures 2 of 101, unchanged.  It is better code and it takes the store out of
 * alias set 0, so it is the right shape to carry forward -- but it is NOT shipped
 * in the body below, because 0xea3 is an ODD offset into a tilemap whose other
 * accesses in this same function are u16, so the struct as written is a guess
 * about a layout nobody has established.  Record it, do not invent it.
 *
 * Blocker class: sched2 PRIORITY (rank_for_schedule rung 1), which returns before
 * any tie-break.  The park's verdict is CORRECT and is now backed by the dump
 * numbers rather than by eight statement orders.  Do not spend another round on
 * the tail; if anything is ever to move here it is the FRONT of block 16, and the
 * two shapes that lengthen it measure 11 and 14.
 *
 * Unchanged from the batch-276 park, and still the four levers that got it here:
 * a REASSIGNED PARAMETER IS A GLOBAL ALLOCNO (`fill <<= 12;`); a HImode store
 * target TRUNCATES A MASK IN THE RHS, so the mask is assigned into an SImode
 * local first; `unsigned` for `lsr` and mask preservation are COUPLED; and
 * NAMING THE DIRTY POINTER BEFORE THE OFFSET (`d = base + 0xea3;` written before
 * `off = ...` is 2, after it 5) because LICM appends rather than prepends.
 * Read the last one against Func_8016f2c, the same lever with the opposite sign.
 */
typedef unsigned char u8;
typedef unsigned short u16;

extern u8 *iwram_3001e8c;

void Func_80ab21c(int x, int y, int w, int h, int fill)
{
    u8 *base;
    u16 *p;
    int off;
    u8 *d;
    int n;
    unsigned int v;

    base = iwram_3001e8c;
    fill <<= 12;
    if (x < 0) {
        w += x;
        x = 0;
    }
    if (x + w > 0x1d)
        w = 0x1e - x;
    if (y < 0) {
        h += y;
        y = 0;
    }
    if (y + h > 0x1d)
        h = 0x14 - y;
    if (w > 0 && h > 0) {
        d = base + 0xea3;
        off = (y << 6) + (x << 1);
        do {
            p = (u16 *)(base + off);
            n = w;
            while (n != 0) {
                v = *p;
                if (((v >> 12) & 0xf) == 0xf) {
                    v = v & 0xffff0fff;
                    v = v | fill;
                    *p = v;
                }
                n--;
                p++;
            }
            *d |= 2 << ((unsigned)y >> 2);
            h--;
            off += 0x40;
            y++;
        } while (h != 0);
    }
}
