/* OvlFunc_926_2008afc  --  0x02008afc,
 * asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_a.s (SINGLE-function file)
 *
 * BYTE-IDENTICAL, --func AND --whole.  216 bytes, 82 encodings and 20
 * relocations identical; instructions 54/54.  Pin-free, device-free, no
 * per-file flag, NO SPLIT -- the .s holds one function and
 * tools/datacheck.py prints nothing, so it converts WHOLE and the stage1.ld
 * line stays verbatim on the asm/ path.
 *
 * Verify with (the INSTALLED path):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_a.c \
 *     asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_a.s --whole
 *
 * THE PARK'S BLOCKER WAS WRONG, AND THE FIX IS ONE CHARACTER CLASS.
 * The park (recovered from scratch/N8afc_best.c in batch 338) declared:
 *   "BLOCKER: RELOAD REMATERIALISATION ... That is a decision of the register
 *    allocator with a register to spare, and no spelling reaches it."
 * One spelling reaches it: write the LAST byte store as the literal `0` and
 * keep the named `z` for the two SImode stores only.
 *
 *     PARK:  __MapActor_GetActor(0xc)[0x5b] = z;
 *     THIS:  __MapActor_GetActor(0xc)[0x5b] = 0;
 *
 * Re-derived figures before the change: ref 54 instructions / 216 bytes
 * against ours 52 / 204, i.e. -2 instructions AND one missing 32-bit entry
 * (26 ref against 25 ours); 34 differing of 82 with the first at index 36.
 * The -2 is ONE site, at the tail, and it is TWO instructions plus a pool
 * word, not two independent -1s:
 *     prologue .. __ActorMessage (indices 0-35)   exact, both sides
 *     the `strh` of 0x4000                        exact
 *     the final byte store   ROM `ldr r5, .Lbc0 @ 0` + `strb r5,[r0]`
 *                            ours                   `strb r5,[r0]`      -1
 *     epilogue               ROM `b .Lbcc` over the pool, then pop/pop/bx
 *                            ours pop/pop/bx and the pool past the end  -1
 * and the index-36 first difference is PHASE: ref 4809 against ours 4816 is
 * the same `ldr r0,[pc,#N]` with the pool 52 bytes further away.
 *
 * WHY THE LITERAL WORKS -- the mechanism is the MINIPOOL, not reload.
 * With `z` at that site the value is one pseudo in r5 for the whole tail, so
 * nothing is pooled there and gcc's pool has 6 words.  With the literal, the
 * QImode store's value is built in the tail instead, gcc's pool takes a 7th
 * word, and arm_reorg's pool placement changes with it: `arm.c:5518-5536`
 * scans fixes forward and records `last_barrier` only while
 * `ftmp->address < minipool_vector_head->max_address`, then
 * `arm.c:5572` calls `create_fix_barrier` when no natural barrier is in
 * range -- which is where the ROM's `b .Lbcc` over the in-function pool comes
 * from, and why the pool splits in two (`.pool_aligned` mid-function in the
 * reference).  Both of the missing instructions and the missing word are that
 * one decision, which is why one site accounts for -2.
 *
 * MEASURED, on the current baseline (ndiff of 82; the park's own figures were
 * against a different body and have expired):
 *   literal 0 at the LAST byte store only           **0, byte-identical**
 *   park body (named `z` at all three)              34, 52 insns, 204 bytes
 *   a second zero local `z2` used only for that
 *     store (the park's own first negative, re-run) 34, 52 insns -- INERT,
 *     byte-identical to the park body, so cse unifies the two names
 *   DEVICE INSTRUMENT, a `.set` symbol of value 0
 *     referenced by address (the const.sym pool-tell
 *     route, as rom_28e54.c uses)                   36, 53 insns, 212 bytes
 *     -- gets the pool word but NOT the second instruction, so the pool tell
 *        is NOT the answer here and nothing need go in const.sym
 *   autosweep FLAG axis, all eight groups: -O1 43, -fno-gcse 34,
 *     -fno-schedule-insns2 43, -ffixed-r7 34, -fno-strict-aliasing 34,
 *     -fno-strength-reduce 34, -fno-rerun-cse-after-loop 34,
 *     -fno-rerun-loop-opt 34.  CLOSED.
 *   autosweep ANCHOR axis, 15 of 15 boundaries: all inert.  CLOSED.
 *
 * WHAT THE PARK GOT RIGHT, kept because it is still the reason the body looks
 * like this: the halfword store of 0x4000 goes through the pool unless the
 * DESTINATION is a typed field, so `((struct A *)...)->f6 = 0x80 << 7` is
 * load-bearing -- casting the address does not do it, and naming the value
 * does it at the price of a callee-saved register.  Same lever as
 * src/non_matching/ovl_77a7c8/200b57c.c.  And the zero stored to the two
 * SImode fields IS one named local, which is the exception to "do not name
 * zeros": the ROM keeps it in r5 across those two __MapActor_GetActor calls
 * and three literal zeros give three movs.
 */
struct A {
    unsigned char pad00[6];
    short f6;
};

extern unsigned char gScript_926__0200c638[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_809280c(int a, int b, int c);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapActor_SetBehavior(int slot, void *s);

void OvlFunc_926_2008afc(void)
{
    int z;

    __CutsceneStart();
    __MapActor_GetActor(0xc)[0x5b] = 0;
    while (*(int *)(__MapActor_GetActor(0xc) + 0xc) > 0)
        __WaitFrames(1);
    z = 0;
    *(int *)(__MapActor_GetActor(0xc) + 0xc) = z;
    *(int *)(__MapActor_GetActor(0xc) + 0x3c) = 0x80 << 24;
    *(int *)(__MapActor_GetActor(0xc) + 0x28) = z;
    __MapActor_GetActor(0xc)[0x5b] = 1;
    __Func_809280c(0xc, 0, 0);
    if (__GetFlag(0x895))
        __MessageID(0x1a5b);
    else if (__GetFlag(0x89b))
        __MessageID(0x189e);
    else
        __MessageID(0x182a);
    __ActorMessage(0xc, 0);
    ((struct A *)__MapActor_GetActor(0xc))->f6 = 0x80 << 7;
    __MapActor_GetActor(0xc)[0x5b] = 0;
    __MapActor_SetBehavior(0xc, gScript_926__0200c638);
    __CutsceneEnd();
}
