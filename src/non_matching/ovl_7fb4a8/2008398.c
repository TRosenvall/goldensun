/* OvlFunc_971_2008398 -- NON-MATCHING, 172 of 192 encodings differ.
 * Unattempted before batch 298.  Reference asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_a.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_7fb4a8/2008398.c \
 *       asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_a.s --func OvlFunc_971_2008398
 *
 * NOT a distance: size 408 against 420 and count 186 against 192.  aligncmp 60.9%.
 * SHIMS: ZERO of every class, and NO SPLIT NEEDED -- one function, pure text,
 * datacheck silent.  The cleanest landing shape of this brief's five.
 * A named `size` local was worth 12 encodings here (196 -> 184) by keeping the
 * budget in low r7 rather than r8 -- see the CSE note in docs/elevation.md.
 * BLOCKER found through the RELOCATION SEQUENCE, not the offsets: the symbols are
 * REORDERED, which means block layout -- the ROM puts the whole wait/poll group
 * ABOVE the loop head.  Loop spelling is inert here (while, for(;;) and for are
 * byte-identical).
 */
/* OvlFunc_971_2008398  --  0x02008398   [PARK DRAFT -- 6 encodings short]
 *   [asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_a.s, 1st of 1 -- NO SPLIT NEEDED]
 *
 * REFERENCE: 189 instructions / 192 encodings / 420 bytes.  Anchored
 * thumb_func_start count = 1, and THE .s IS PURE TEXT -- no `.section .data`,
 * no `.incbin`, and tools/datacheck.py prints nothing and exits 0.  So this is
 * the CLEANEST LANDING SHAPE in the batch: whole-file replacement, no text/data
 * split, no new exports, no linker edit.  SHIMS: 0 of every class.
 *
 * WHAT IT DOES -- replaces the .s header's "CALL TRACE rather than a
 * description".  This is a TWO-PHASE LINK TRANSFER WITH A SHARED FRAME BUDGET,
 * and it returns how many of the three received units carried a flag, or -1 on
 * any failure:
 *   * phase 1: allocate a 0x154-byte scratch, then for each of party slots
 *     0x80..0x82 open a transfer (__Func_8006408), poll __Func_80064f4 until it
 *     returns 0, require ewram_2002238 == 0x154, count the unit if its byte at
 *     0x12a is set, then decompress string 0x80c into a 0x30-byte stack buffer
 *     and SPLICE it into the front of the unit's 15-byte name: shift the name
 *     right by the string's halfword length and write the string's halfwords
 *     down into bytes, NUL-terminating at [0xe];
 *   * phase 2: free, allocate 0x140, __Func_8077330(1), and run the SAME poll
 *     loop once for a 0x140-byte transfer.
 * THE BUDGET IS THE INTERESTING PART AND IT IS SHARED ACROSS BOTH PHASES: r7
 * starts at 0xe1 << 2 = 900 frames and is decremented once per polled frame in
 * both loops.  The retry counter r5 is per-transfer (reset to 0 at each
 * __Func_8006408) and caps at 0x18, but IT ONLY ADVANCES when the budget has
 * run out OR (iwram_3001f64 & 3) != 3 -- i.e. while both those bits are set and
 * frames remain, the poll can spin without consuming a retry.  Two timeouts,
 * one global and one local, and the local one is gated on the global.
 *
 * STATE: objcmp  XX SIZE ref 420 bytes, ours 408
 *                XX ENCODINGS differ in 172 place(s) (ref 192, ours 186)
 *                first at index 19: ref e058 ours 4650
 * SIZE AND INSTRUCTION COUNT BOTH DIFFER, so 172 is NOT a distance.
 * tools/aligncmp.py: aligned-equal 117 of 192 (60.9%), 100 differing/ins/del
 * in 24 hunks.  tools/tryc.py --full: "rom 211 lines, ours 203, first diff at
 * 19" -- the first 19 encodings, the whole prologue and the allocation, are
 * byte-identical, and so is essentially every line of both poll loops, both
 * copy loops and the epilogue, REGISTER FOR REGISTER.
 *
 * THE LEVER THAT DID THE WORK -- A NAMED `size` LOCAL, AND IT IS WORTH 12
 * ENCODINGS.  The transfer size appears three times per phase (the allocation
 * and the two ewram_2002238 comparisons).  Written as the literal `0xaa << 1`
 * at all three sites, gcc REMATERIALISES it (`mov r5,#0xaa / lsl r5,#1`) at
 * every use, which costs a low callee-saved register and PUSHES `budget` INTO
 * r8 -- and a decrement-and-test of a high register is
 * `mov r3,#1 / neg r3,r3 / add r8,r3 / mov r2,r8 / cmp r2,#0` where the ROM has
 * `sub r7,#1 / cmp r7,#0`.  Three extra encodings at each of the two poll
 * loops.  Assigning the value to an `int size` first gives gcc ONE pseudo, and
 * the allocation then lands exactly the ROM's
 * `mov r2,#0xaa / lsl r2,#1 / mov r8,r2 / mov r0,r8` with budget back in r7:
 * 196 encodings -> 184.  This is the mirror image of the blocker recorded in
 * src/non_matching/ovl_7fa4ec/20092ac.c (same overlay family): there the ROM
 * rematerialises a repeated constant and gcc CSEs it; here the ROM CSEs and gcc
 * rematerialises.  BOTH DIRECTIONS ARE REACHABLE FROM SOURCE -- a named local
 * forces the CSE -- so that park's "no known source-level lever" applies to
 * defeating a CSE, not to creating one.
 *
 * AND THE ASYMMETRY IS REAL, MEASURED, NOT A GUESS.  In phase 1 the ROM keeps
 * 0x154 in r8 for all three uses; in phase 2 it keeps 0x140 in r8 only for the
 * allocation and REMATERIALISES `mov r2,#0xa0 / lsl r2,#1` at both comparisons.
 * Using `size` in phase 2's comparisons as well overshoots to 184 encodings
 * (8 short); reverting just those two comparisons to the literal while leaving
 * the allocation on `size` reads 186 (6 short), which is the best measured.
 * The shipped spelling therefore mixes the two deliberately -- that is not
 * sloppiness, it is what the ROM shows.
 *
 * THE BLOCKER, NAMED BY PASS: LOOP ROTATION in the jump/loop pass, plus two
 * live-range splits the allocator declines to make.
 *   (1) THE OUTER LOOP IS UNROTATED IN THE ROM: `b .L474` into a test at the
 *       TOP (`mov r3,r10 / cmp r3,#2 / bgt`), with the body below it.  gcc
 *       proves 0 <= 2 and moves the test to the BOTTOM, saving the entry
 *       branch.  MEASURED, three spellings -- `for (i = 0; i <= 2; i++)`, an
 *       explicit `i = 0; while (i <= 2) { ... i++; }`, and both combined with
 *       (2) below -- ALL produce BYTE-IDENTICAL output at 186 encodings.  gcc
 *       normalises for and while to the same RTL before the loop pass runs, so
 *       there is no source spelling of this loop that keeps the entry test.
 *       AND THE RESIDUE IS BIGGER THAN AN ENTRY BRANCH -- THE RELOCATION ORDER
 *       PROVES IT.  Both objects carry the SAME 17 relocations, but not in the
 *       same order, and objcmp's `XX RELOCATIONS differ` here is NOT merely the
 *       offset shift it looks like.  The ROM's call order through the object is
 *         Func_8004970, WaitFrames, Func_80064f4, WaitFrames,
 *         DecompressString2, GetUnit, Func_8006408, ...
 *       and ours is
 *         Func_8004970, GetUnit, Func_8006408, WaitFrames, Func_80064f4,
 *         WaitFrames, DecompressString2, ...
 *       i.e. THE ROM PLACES THE WHOLE WAIT/POLL BLOCK GROUP *ABOVE* THE LOOP
 *       HEAD, with the `b` at the top jumping forward past it to a test at the
 *       bottom of the body; gcc places the body first.  So this is the outer
 *       loop's entire basic-block ORDER, not one branch, and that is why no
 *       loop spelling moved it.  Worth knowing generally: when the instruction
 *       counts are close but `RELOCATIONS differ`, CHECK THE SYMBOL SEQUENCE --
 *       a reordered sequence is a block-layout finding, while a same-sequence
 *       different-offsets diff really is just the size shift.
 *   (2) THE SCAN INDEX IS SPLIT IN THE ROM AND NOT HERE.  The ROM runs the
 *       string-length scan in r0 and then copies it (`mov r4, r0`) into the
 *       register the two copy loops use; gcc keeps one register throughout.
 *       Writing the scan into a separate `n` and then `len = n` -- the source
 *       shape that split would come from -- changes NOTHING (186, identical).
 *       Related and from the same cause: the ROM's first scan read is
 *       `ldrh r3, [r5, r0]` with the index in a register, where gcc folds
 *       `sbuf[0]` to `ldrh r3, [r5, #0]`.
 *   (3) The final halfword-to-byte copy loop is a pure register PERMUTATION:
 *       the ROM fills dst/src/count as r1/r2/r0, gcc as r0/r1/r5.  Same
 *       instruction count, so it contributes to the 172 but not to the 6.
 *
 * ALSO CONFIRMED HERE: passing the stack array DIRECTLY to
 * __DecompressString2 and only then letting the scans index it reproduces the
 * ROM's `mov r1, sp` ... `mov r5, sp` pair; introducing an `unsigned short *s`
 * before the call instead costs a `mov r1, r5` copy.  This stem has NO Makefile
 * rule, so the tree default -O2 with strict aliasing is the production build
 * and every figure above is a production-flag figure.
 *
 * NEXT IDEA IF PICKED UP: all six remaining encodings are in the jump/loop and
 * allocator passes, and none of the three responded to source shape.  The one
 * untried mechanism is whether the outer loop's `goto done` exits are what let
 * gcc rotate at all -- restructure the four failure exits as a flag tested
 * after the loop (no goto out of the loop body) and see whether the entry test
 * survives.  That changes real control flow, so measure before believing it.
 */
extern void *__Func_8004970(int size);
extern void __free(void *p);
extern void *__GetUnit(int party);
extern int __Func_8006408(void);
extern int __Func_80064f4(void);
extern void __WaitFrames(int n);
extern void __DecompressString2(int id, unsigned short *dst);
extern void __Func_8077330(int n);
extern unsigned short ewram_2002238;
extern unsigned short iwram_3001f64;

int OvlFunc_971_2008398(void)
{
    unsigned short sbuf[24];
    void *buf;
    unsigned char *u;
    int size;
    int budget;
    int ret;
    int i;
    int tries;
    int rc;
    int len;
    int k;
    int j;

    size = 0xaa << 1;
    buf = __Func_8004970(size);
    ret = 0;
    budget = 0xe1 << 2;
    for (i = 0; i <= 2; i++) {
        u = __GetUnit(i + 0x80);
        rc = __Func_8006408();
        tries = 0;
        if (rc == -1) {
            ret = rc;
            goto done;
        }
        for (;;) {
            if (__Func_80064f4() == 0)
                break;
            if (ewram_2002238 > size) {
                ret = -1;
                goto done;
            }
            __WaitFrames(1);
            budget--;
            if (budget < 0 || (iwram_3001f64 & 3) != 3) {
                tries++;
                if (tries > 0x18) {
                    ret = -1;
                    goto done;
                }
            }
        }
        if (ewram_2002238 != size) {
            ret = -1;
            goto done;
        }
        if (u[0x95 << 1] != 0)
            ret++;
        __WaitFrames(2);
        __DecompressString2(0x80c, sbuf);
        for (len = 0; len <= 4; len++)
            if (sbuf[len] == 0)
                break;
        for (k = 0xe; k >= len; k--)
            u[k] = u[k - len];
        for (j = 0; j < len; j++)
            u[j] = sbuf[j];
        u[0xe] = 0;
    }
    __free(buf);
    size = 0xa0 << 1;
    buf = __Func_8004970(size);
    __Func_8077330(1);
    rc = __Func_8006408();
    tries = 0;
    if (rc == -1) {
        ret = rc;
        goto done;
    }
    for (;;) {
        if (__Func_80064f4() == 0)
            break;
        if (ewram_2002238 > (0xa0 << 1)) {
            ret = -1;
            goto done;
        }
        __WaitFrames(1);
        budget--;
        if (budget < 0 || (iwram_3001f64 & 3) != 3) {
            tries++;
            if (tries > 0x18) {
                ret = -1;
                goto done;
            }
        }
    }
    if (ewram_2002238 != (0xa0 << 1)) {
        ret = -1;
        goto done;
    }
    __WaitFrames(2);
done:
    __free(buf);
    return ret;
}
