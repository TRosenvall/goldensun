/* Func_80e7338 -- 0x080e7338, and its twin
 * Func_80e73a0 -- 0x080e73a0,
 * both in asm/rom_c9000/rom_e7320_c_c.s
 *
 * Two slot claimers over parallel pools. Scan a fixed-size array of 28-byte
 * entries for one whose word at +0x18 is -1, write 0 there, then store the
 * payload. 80e7338 has 16 slots at base +0x7400 and stores three words (+0,
 * +4, +0xc); 80e73a0 has 32 slots at base +0x7080 and stores two (+0, +4). If
 * every slot is taken the call does nothing and there is no failure signal.
 *
 * 43 of 54 and untried respectively -- one solution lands both, so there is no
 * point screening the twin until the first one moves.
 *
 * BLOCKER: A TRANSCRIBED STATIC CHAIN CANNOT REPRODUCE AN UP-LEVEL READ INSIDE
 * A LOOP. These were NESTED FUNCTIONS, and unlike the batch-187 pair -- which
 * merely SAVED r9 without using it -- these two DEREFERENCE the chain, and they
 * do it inside the scan loop.
 *
 * The nesting is not inferred, it is read off the caller. At
 * rom_e7320_c_c.s:1162, inside BaseAnim_Meteor:
 *
 *     add  r2, sp, #0x11c        <- a pointer into the PARENT's own frame
 *     mov  r9, r2                <- static chain
 *     bl   Func_80e7338
 *
 * which is the documented caller signature exactly. So `*(chain - 0x88)` is
 * BaseAnim_Meteor's local at sp+0x94, and the array base is re-read from the
 * parent's frame on every iteration:
 *
 *     ldr  r2, [r1, #0x0]        <- INSIDE the loop, r1 = chain - 0x88
 *
 * gcc will not hoist a genuine up-level reference out of a loop. It WILL hoist
 * the ordinary local load that the transcription turns it into, and that single
 * difference is the whole residue: because our `*pp` is held in r1 across the
 * loop, the index and the slot pointer swap registers (ROM has i in r4 and the
 * entry in r2; we get the reverse), and the chain-copy chain shortens from the
 * ROM's three pseudos to one. Every one of the 43 disagreements traces back to
 * it.
 *
 * MEASURED, and the plateau is the point. SEVEN unrelated spellings tie at
 * EXACTLY 43 -- `i != 0x10` with a plain `char **`, with an `int *` base, with
 * the dereference cast at either end, and all four declaration orders of
 * (i, pp, e). Declaration order does not touch it, which rules out the pseudo
 * numbering route to the register swap. Per this notebook's own rule, a tie
 * that broad means the lever is not in the spelling.
 *
 * WHAT WAS LEARNED ANYWAY, all measured:
 *   - The volatile slot must be written THROUGH A POINTER, not to the bare
 *     local. `mov r3, sp / str r4, [r3]` is the byte-granular address-taken
 *     shape from Func_80c0700; a named `volatile u32 *slot = &buf;` gets it and
 *     is worth 51 -> 44. A direct `*(volatile u32 *)&buf = chain;` does not.
 *   - `cmp r4, #0x10 / beq` is an EQUALITY test, so the loop bound was written
 *     `i != 0x10`, not `i < 0x10`. Worth one instruction (44 -> 43); a `<`
 *     bound compiles to `cmp #0xf / bgt`.
 *   - MAKING THE LOAD VOLATILE IS NOT THE ESCAPE AND IS ACTIVELY WORSE.
 *     `char *volatile *pp` forces the reload but costs the prologue -- 52
 *     differing with the FIRST instruction wrong, because the extra constraint
 *     changes the push list. Do not reach for volatile to defeat LICM.
 *   - THE ALIASING ROUTE ALSO FAILS. Loading the base as `int` so that the
 *     in-loop `int` stores to +0x18 might alias it, and thus block the hoist,
 *     changes nothing (43, byte-identical to the `char **` form). gcc-2.96
 *     hoists it regardless, so this is not a strict-aliasing question.
 *
 * THE REAL FIX IS STRUCTURAL, and it is the same one batch 187 recorded for
 * Func_8015fb8: elevate the PARENT and write these as nested functions inside
 * it. BaseAnim_Meteor is in this very file, at line 208, and runs to line 1883
 * -- roughly 1670 lines. Once it is C, both of these become nested functions,
 * the up-level read becomes a real up-level read, the register binding and the
 * volatile slot both disappear, and the reload should fall out for free.
 *
 * Do not spend another round on spellings. This one is gated on its parent.
 */

typedef unsigned int u32;

struct Slot {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
};

void Func_80e7338(int a, int b, int c)
{
    register u32 _chain __asm__("r9");
    volatile u32 buf;
    volatile u32 *slot;
    u32 chain;
    int i;
    char **pp;
    struct Slot *e;

    slot = &buf;
    chain = _chain;
    *slot = chain;
    pp = (char **)(chain - 0x88);
    for (i = 0; i != 0x10; i++) {
        e = (struct Slot *)(*pp + 0x7400) + i;
        if (e->f18 == -1) {
            e->f18 = 0;
            e->f0 = a;
            e->f4 = b;
            e->fc = c;
            break;
        }
    }
}

void Func_80e73a0(int a, int b)
{
    register u32 _chain __asm__("r9");
    volatile u32 buf;
    volatile u32 *slot;
    u32 chain;
    int i;
    char **pp;
    struct Slot *e;

    slot = &buf;
    chain = _chain;
    *slot = chain;
    pp = (char **)(chain - 0x88);
    for (i = 0; i != 0x20; i++) {
        e = (struct Slot *)(*pp + 0x7080) + i;
        if (e->f18 == -1) {
            e->f18 = 0;
            e->f0 = a;
            e->f4 = b;
            break;
        }
    }
}
