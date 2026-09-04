/* CreateSpriteLayer (0x0800bbc0) -- NON-MATCHING.
 * Blocker class: register allocation -- gcc keeps a pointer in r8 where the
 * ROM spills it to the frame.
 *
 * 66 lines against 66, 45 differing, and almost all of the 45 are one decision
 * cascading. The value is a slot pointer initialised to 0 BEFORE the
 * `_GetSpriteInfo` call and read after it:
 *
 *     rom    mov r4, #0x0 / str r4, [sp] / ... / bl _GetSpriteInfo
 *            ... / ldr r4, [sp] / ...
 *     ours   mov r3, #0x0 / mov r8, r3   / ... / bl _GetSpriteInfo
 *
 * The ROM spills it and reloads once; gcc parks it in r8, which costs a second
 * high register (r10) for the zero the ROM keeps in r8, and a `mov r3, r8`
 * every time the value is read. Nothing in the source chooses between a spill
 * and a callee-saved register.
 *
 * MEASURED (rom 66 lines):
 *   `while (p[4] != 0) { ... }` for the search loop     66, 45  <- best
 *   an explicit `goto test;` loop-entry jump            64, 58 (worse -- gcc
 *                         already emits the ROM's `b` from the `while`, and
 *                         writing it by hand costs the guard)
 *   the two zero byte-stores UNNAMED (literal 0)        59, 63 (much worse)
 *
 * THIS FUNCTION IS WHY tools/poolblocked.py WAS CORRECTED. It was in the
 * "branch-over-pool CERTAIN" class -- 501 functions, a quarter of the corpus --
 * and it is not blocked at all. Its two `.pool_aligned` markers are EMPTY: the
 * macro flushes pending literals and there are none, so it emits no bytes, and
 * the two `b` instructions the test saw are ordinary control flow (a loop-entry
 * jump and a branch to the epilogue). A third of the class is like this.
 *
 * The other two thirds have real mid-body pools, and those are not blocked
 * either -- gcc-2.96 emits them. Compiling a plain transcription of
 * Func_80bad7c produces two, unprompted:
 *
 *     b .L10 / .L19: / .align 2, 0 / .L18: .word 256 / .word iwram_3001e74 / .L3:
 *
 * The premise in the old docstring ("old_agbcc only emits one at .func_end")
 * is about a compiler this tree uses for five m4a and agb_flash objects and for
 * nothing else.
 *
 * WHAT IS RIGHT: the search loop written as a plain `while`, which produces the
 * ROM's jump-to-test entry by itself; the two byte stores of a NAMED zero,
 * which is what puts a zero in a register at all; and the whole initialisation
 * block including the conditional `GetCachedSpriteGFX`.
 *
 * THREE MORE SPELLINGS, batch 217, all measured against the body below and all
 * WORSE. They are recorded because each one closes a direction that looks
 * obvious from the blocker description:
 *
 *     the body below (unchanged)                     66 lines, 45 differing
 *     `sel` PINNED to r4, the register the ROM        61 lines, 62 differing
 *       spills FROM
 *     a barrier on `sel` before the call              67 lines, 63 differing
 *     `sel` declared `volatile`                       65 lines, 56 differing
 *
 * THE PIN RESULT IS THE USEFUL ONE. Pinning to the very register the ROM
 * spills from does not produce a spill -- the pin is DROPPED, because it is
 * assigned before a call and used after it, which is the batch 210 hazard. So
 * that hazard also blocks the one use that would turn a pin into a spill tool:
 * A PIN CANNOT ASK FOR A SPILL.
 *
 * `volatile` does force a frame and a store/reload, but it is not a spill --
 * every later access goes through memory too, including the loop assignment
 * and the null test, so the function grows accesses the ROM does not have.
 *
 * The barrier proves the value is live across the call and gcc still satisfies
 * it in a register rather than a frame slot.
 *
 * NEXT: nothing source-level for the spill. Reading the generated assembly for
 * a NEIGHBOURING register is the lever that has twice unblocked this class and
 * is still untried here -- the question to answer is which value the ROM keeps
 * alive across that call that we are rebuilding, since that is what takes the
 * register gcc is currently handing to the slot pointer.
 */
extern unsigned char *_GetSpriteInfo(int id);
extern unsigned char *iwram_3001e5c;
extern void *GetCachedSpriteGFX(int id);

void *CreateSpriteLayer(int id)
{
    unsigned char *info;
    unsigned char *p;
    unsigned char *sel;
    unsigned char *r;
    unsigned char *t;
    void *gfx;
    int i;
    int z;

    sel = 0;
    info = _GetSpriteInfo(id);
    p = iwram_3001e5c;
    r = 0;
    if (info[0] != 0) {
        i = 0;
        while (p[4] != 0) {
            i++;
            p += 0x18;
            if (i > 0x3f)
                goto none;
        }
        sel = p;
    none:
        if (sel != 0) {
            z = 0;
            gfx = *(void **)(info + 0xc);
            r = sel;
            *(unsigned short *)r = id;
            if (gfx == 0)
                gfx = GetCachedSpriteGFX(id);
            t = *(unsigned char **)(info + 0x10);
            *(void **)(r + 8) = gfx;
            *(unsigned char **)(r + 0xc) = t;
            r[7] = info[0xa];
            r[0x16] = 0xff;
            *(int *)(r + 0x10) = *(int *)t;
            r[0x14] = z;
            r[4] = info[4];
            r[5] = z;
        }
    }
    return r;
}
