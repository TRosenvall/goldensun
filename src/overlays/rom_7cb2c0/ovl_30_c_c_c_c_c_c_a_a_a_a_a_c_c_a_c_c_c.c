/* OvlFunc_945_200b51c -- MATCHES BYTE-FOR-BYTE at PRODUCTION FLAGS.
 *   OK OvlFunc_945_200b51c -- 336 bytes, 136 encodings and 26 relocations identical
 *
 * NO FLAG GROUP, NO PINS, NO DEVICE.  shimcount reports none.
 *
 * Park claimed (batch 338 recovery header): LENGTH gap, ref 103 instructions
 * against ours 104 (+1), with the preserved prose blaming a CROSS-JUMP.
 * RE-DERIVED AT THE HEAD OF BATCH 339: ref 103 / ours 104 confirmed, 82 of 136
 * stream encodings, relocation list symbol- and order-identical with only the
 * offsets shifted from the 10th entry on.
 *
 * THE FIX IS A TWO-LINE MOVE: `q = 0xf4 << 1;` goes just before the 0xc call
 * and `s = 0xd0 << 8;` just before the 0xd call -- each constant defined
 * immediately before its FIRST USE instead of all four in one block.
 *
 * WHAT THE +1 ACTUALLY WAS, and the prose named the wrong instruction.  The
 * ROM materialises 0xb000 into r3, passes that LIVE r3 as the first call's
 * fourth argument, and copies it to r8 for the two later calls.  We
 * materialised it into r5 and then needed `mov r3, r5` at that call -- one
 * instruction, and the whole +1.  Everything after it was identical content
 * shifted by one line, the shared `mov r1,#0 / mov r2,#0 / bl ...` tail
 * included: THE CROSS-JUMP THE PROSE BLAMED IS ALREADY PRESENT IN OUR BUILD
 * and was never the defect.
 *
 * THE MECHANISM, and it is why the prose's sweep could not have worked.
 * allocno_compare (global.c:598-620) scores
 *     floor_log2 (n_refs) * n_refs / live_length
 * and find_reg hands out registers in REG_ALLOC_ORDER (arm.h:988-995:
 * 3,2,1,0,12,14,4,5,6,7,8,10,...), so with r4 call-used and r7 the Thumb frame
 * pointer the usable sequence is r5, r6, r8, r10 and the LOWEST-priority
 * allocno gets r10.  The ROM wants q=r5, s=r6, r=r8, p=r10.  `r` is referenced
 * FOUR times (one def, three calls) where p, q and s are referenced three, and
 * floor_log2(4)=2 against floor_log2(3)=1 -- so r's numerator is 8 against
 * their 3, a 2.7x advantage that no REORDERING of four same-block definitions
 * can overturn, because reordering only shuffles live_length among ranges that
 * all end at the same place.  To beat 8/L_r with 3/L_s you need
 * L_r > 2.67 * L_s, and the only source handle on L_s is where s is DEFINED.
 * Moving s's definition from the top of the block to just before the 0xd call
 * cuts L_s by about two thirds and that is exactly what flips it.
 *
 * SO THE PARK'S BOUND IS REFUTED, AND THE AXIS IT NAMED WAS THE WRONG ONE.
 * The prose wrote "ALL SIX ASSIGNMENT ORDERS SCREENED ... the source-order
 * lever picks registers for two independent values; with FOUR it has no
 * purchase", and offered that as "a boundary on the lever rather than another
 * failure".  All 24 permutations are screened here, not six, and the prose is
 * RIGHT that the whole space is flat -- 82, 83 or 84, never better.  It is the
 * CONCLUSION that was wrong: the lever is not the ORDER of the definitions, it
 * is their PLACEMENT, because allocno_compare reads live_length and order
 * cannot change the end of a range while placement changes its start.
 *
 * MEASURED, all at production flags (base = 82 of 136, 104 instructions):
 *   ALL 24 permutations of the four definitions   82 / 83 / 84, never below 82
 *   literal (0xb0 << 8) at r's first, middle, last or all three uses   82
 *     (cse merges them back into one pseudo, so the ref count does not move)
 *   q moved to the 0xc call alone                 82
 *   p moved to the 0xa call alone, with s         15 of 136, 103 instructions
 *   s moved to the 0xd call alone                 13 of 136, 103 instructions
 *   s to 0xd + q to 0xc                           *** 0, byte-identical ***
 * MEASURED WORSE / DIFFERENT PROGRAM SHAPE:
 *   r hoisted above the whole if-chain            102 instructions, 93
 *     -- NOT a near-miss.  gcc rematerialises `mov r3,#0xb0 / lsl r3,#8` at two
 *        call sites, drops r10 from the frame entirely and prologues with
 *        `mov r6,r8 / push {r6}`.  A different allocation regime, not one away.
 *   all four hoisted above the if-chain           96 instructions, 130
 *   r + p hoisted to the arm top                  94
 * AUTOSWEPT on the parked body: 8 flag groups all inert or worse (-O1 84,
 *   -fno-schedule-insns2 84, the rest 82); 24 anchor placements, of which
 *   anchor12/13/15 reached 81 -- all three on a stream still one instruction
 *   LONGER than the reference, so that 81 was measuring misalignment and not
 *   distance.  Not needed by this landing.
 *
 * LANDING SHAPE -- the easiest in this batch.  tools/piece.py: the reference
 * holds ONE function, this one.  tools/datacheck.py reports no data sections.
 * tools/split_s.py --dry-run says it outright:
 *     "holds only OvlFunc_945_200b51c and no data; convert it directly,
 *      no split needed"
 * So this converts straight to src/overlays/rom_7cb2c0/ with no .global
 * exports, no split and no Makefile change.
 *
 * Verify with (INSTALLED path):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_a_c_c_c.c \
 *     asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_a_c_c_c.s --whole
 *
 * Verify NOW, from this scratch path:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py scratch_elev/b339/D/p1_candidate.c \
 *     asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_a_c_c_c.s \
 *     --func OvlFunc_945_200b51c
 */
extern unsigned char gScript_945__0200e958[];
extern unsigned char gScript_945__0200e840[];

extern int __GetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void OvlFunc_945_200c890(int a, int b, int c, int d);
extern void OvlFunc_945_200c8e8(int a, int b, int c);
extern void OvlFunc_945_200d004(void);

void OvlFunc_945_200b51c(void)
{
    int p, q, r, s;

    if (__GetFlag(0x93e)) {
        __MapActor_SetPos(8, 0, 0);
        __MapActor_SetPos(9, 0, 0);
        __MapActor_SetPos(0xa, 0, 0);
        __MapActor_SetPos(0xb, 0, 0);
        __MapActor_SetPos(0xc, 0, 0);
        OvlFunc_945_200c8e8(0xe, 0, 0);
    } else if (__GetFlag(0x8a << 4)) {
        OvlFunc_945_200c890(8, 0x98, 0xde << 1, 0xc0 << 6);
        __MapActor_SetBehavior(8, gScript_945__0200e958);
        p = 0xf0 << 1;
        r = 0xb0 << 8;
        OvlFunc_945_200c890(0xa, 0xb8, p, r);
        q = 0xf4 << 1;
        OvlFunc_945_200c890(0xc, 0xaa, q, r);
        s = 0xd0 << 8;
        OvlFunc_945_200c890(0xd, 0x88, q, s);
        OvlFunc_945_200c890(0xf, 0x78, p, s);
        OvlFunc_945_200c890(0xe, 0xb8, 0x20e, r);
        OvlFunc_945_200c890(0xb, 0x88, 0x92 << 2, 0x80 << 8);
        __MapActor_SetBehavior(0xb, gScript_945__0200e840);
    } else if (__GetFlag(0x928)) {
        OvlFunc_945_200d004();
    } else if (__GetFlag(0x925)) {
        OvlFunc_945_200c8e8(0x12, 0, 0);
    } else if (__GetFlag(0x911)) {
        if (__GetFlag(0x922)) {
            OvlFunc_945_200c8e8(0xe, 0, 0);
            __MapActor_SetPos(0xc, 0, 0);
        }
    }
}

