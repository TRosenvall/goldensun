/* CreateBattleSpriteOverlays (0x080b7b6c) -- NON-MATCHING, 260 of 282 encodings differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_b5000/b7b6c.c asm/rom_b5000/rom_b7410_a_a_c_c_c_a.s \
 *       --func CreateBattleSpriteOverlays
 *
 * SIZE 612 == 612 AND INSTRUCTIONS 282 == 282, so 260 IS A TRUE DISTANCE.
 * tools/aligncmp.py: aligned-equal 146 of 282 (51.8%), 166 differing in 58 hunks.
 * SHIMS: 3 register pins -- 2 inside the shared `call_via` helper (precedent
 * src/rom_c9000/rom_e3958_c_c_c_a.c) and 1 for `i` in r11, which is discussed
 * below and is the single most valuable thing in this draft.  Needs a
 * fakematch.txt row.
 *
 * SPLIT: none needed.  asm/rom_b5000/rom_b7410_a_a_c_c_c_a.s holds ONE function;
 * tools/datacheck.py reports NO required data exports.
 *
 * ================== THE PROSE COMMENT, CHECKED ================================
 * The reference's `@` block reads "RunActorSequence / r0 = combatant list,
 * r1 = mode. Drives the combatants' actors a frame at a time through WaitFrames,
 * submitting sprites with Func_b7aac, cleaning up with .gcc2_compiled., and
 * reaching into the overlay at _Func_185008. ... 303 lines".  Scored:
 *   - "r0 = combatant list, r1 = mode" is RIGHT, and is the useful half.
 *   - "cleaning up with .gcc2_compiled." is not a claim about this function at
 *     all.  `.gcc2_compiled.` is the assembler symbol every gcc output carries;
 *     it is not a callee and this function does not reference it.
 *   - "reaching into the overlay at _Func_185008" -- no such reference exists in
 *     the body.  The callees are Func_80b770c, Func_80b7b30, WaitFrames,
 *     GetBattleActor, Func_80b78e4, _CreateSprite, Func_80008d4, Func_8000888,
 *     _GetSpriteInfo, _Sprite_AddLayer, _SpriteLayer_SetAnim and Func_80b7aac.
 *   - "303 lines" is a .s LINE count.  The function is 282 instructions, and the
 *     batch brief's "266" is a line count too.  Per docs/elevation.md, never take
 *     a figure out of an `@` block as an instruction count.
 * What it actually does: three passes over a 14-entry short list terminated by
 * 0xff, with 0xfe as a skip sentinel.  Pass 1 tears down any combatant
 * Func_80b770c reports absent, remapping ids above 7 by +0x78 (the inverse of
 * GetBattleActor's own `if (unit > 7) unit -= 0x78`, src/rom_b5000/
 * rom_b7410_a_a_c_c_c_b.c).  Pass 2 builds each live combatant's sprites --
 * either ONE sprite with up to three added layers, or TWO sprites written into a
 * shared slot block, chosen on the low 12 bits of the actor's id.  Pass 3, only
 * when `mode` is non-zero, re-submits every live combatant through Func_80b7aac.
 *
 * ================== WHAT CLOSED IT TO EXACT SIZE AND COUNT ====================
 * First draft was 284 instructions / 620 bytes.  Each step measured alone:
 *
 * 1. `Func_80b7b30(i > 7 ? i + 0x78 : i)` AS A TERNARY, not as an if/else with
 *    two calls.  The ROM computes `i + 0x78` UNCONDITIONALLY and then overwrites
 *    it with `i` on the not-taken side (`mov r0,fp / mov r2,fp / add r0,#0x78 /
 *    cmp r2,#7 / bgt / mov r0,fp`).  The if/else form gets the sense the other way
 *    round and costs 2.  284/620 -> 281/612, and the SIZE matched from here on.
 * 2. PASS 1 IS A do-while, not a for.  `for (i = 0; i <= 0xd; i++)` emits an entry
 *    guard (`cmp r1,#13 / bgt`) the ROM does not have; the ROM's only test is at
 *    the bottom (`add fp,r3 / mov r1,fp / cmp r1,#13 / ble`).  Note `add fp, r3`
 *    with r3 built by `mov r3,#1` -- Thumb has no `add hi,#imm`, which is a small
 *    confirmation that the counter really is in a high register.
 * 3. `kind` IS AN unsigned char, NOT AN int.  The tell is unmistakable once seen:
 *    the ROM spills it with a WORD store (`str r2,[sp,#0xc]`) and reads it back
 *    with `add r1,sp,#0xc / ldrb r1,[r1]` -- a byte load off a computed stack
 *    address, which is what gcc does for a spilled QImode pseudo.  An `int` local
 *    reads back with `ldr`.  Worth adding to the tell list: A BYTE LOAD FROM A
 *    STACK SLOT NAMES THE LOCAL'S TYPE.
 * 4. SEPARATE VARIABLES FOR THE THREE THINGS THE DRAFT CALLED `id`: the masked
 *    12-bit selector (`id`), the full sprite resource (`res`, which the ROM keeps
 *    in r5 and re-reads from actor+4 rather than reusing the masked value), and
 *    the layer resource (`lid`).  Reusing one name folds three pseudos into one
 *    and loses the pressure the ROM is under.  281/612 -> 282/612, both exact.
 *
 * 5. `i` PINNED TO r11, AND WHY THE PIN IS IN THIS DRAFT.  Pin-free, gcc gives
 *    r11 to `cur` and spills `i` to a stack slot, so all of pass 1 comes out as
 *    `ldr r3,[sp,#12] / ... / str r3,[sp,#12]` where the ROM has `mov r1,fp`.
 *    That is ~10 encodings in pass 1 alone and it propagates.  With the pin,
 *    aligned-equal goes 52.5% -> 57.4% and pass 1 becomes shape-exact.
 *    MEASURED, pin-free, all leaving r11 with `cur`:
 *      `cur` declared last in the block                     no change
 *      a SEPARATE `cur2` for pass 3 (shorter live ranges)   277/604, 53.5%, worse
 *      declaring `i` first                                  no change
 *    `i` and `cur` have almost the same use count (~12 vs ~10), which is why this
 *    tips the wrong way; it is the REG_ALLOC_ORDER class (HANDOFF batch 295) seen
 *    from the tie-break side rather than the ordering side.
 *
 * Also load-bearing:
 *  - `spr[0x26] = kind` sits OUTSIDE the `if (spr != 0)` guard at both two-sprite
 *    sites.  That is a null dereference when _CreateSprite fails and it is what
 *    the ROM does (`mov r3,r6 / add r3,#0x26 / strb`, with r6 the unchecked
 *    result).  Do not "fix" it; it changes the instruction count.
 *  - pass 2 walks a POINTER `p` (spilled to [sp,#8], stepped `+= 2`) while ALSO
 *    counting `i`; pass 3 indexes `list[i]` instead.  Two different idioms in one
 *    function, and both are in the ROM.
 *  - pass 3 reads the list TWICE per iteration -- once for the 0xfe test and once
 *    for the value -- so they are two separate source reads, not one cached one.
 *  - `base = iwram_3001e68; base = base + *(int *)(base + 0x18) * 4;` then
 *    `slot = base + 8` and later `slot = base + 0xc`, which is why `base` has to
 *    survive as its own variable.
 *
 * ================== AN UNRESOLVED CONSTANT: A NAMED BASE AT 0x1dc =============
 * The selector test is `id == 0x1dc || id == 0x1e3` and the ROM builds the pair as
 *      mov r2, #0xee / lsl r2, #1     @ 0x1dc
 *      cmp r3, r2 / beq
 *      add r2, #7                     @ 0x1e3
 *      cmp r3, r2 / bne
 * -- it DERIVES the second from the first.  Per batch 298 (HANDOFF), gcc-2.96
 * never chains plain CONST_INTs, so a ROM deriving one constant from another
 * identifies a NAMED BASE.  Every spelling tried folds to two independent
 * constants, with 0x1e3 POOLED (it is not imm8-shiftable) where the ROM has
 * `add r2,#7`:
 *      id == 0x1dc || id == 0x1dc + 7         283/616
 *      id == 0x1dc || id == 0x1e3             283/616   identical output
 *      id == 0xee * 2 || id == 0xee * 2 + 7   283/616   identical output
 * A `.sym` entry will NOT fix this: 0x1dc is BUILT here (`mov / lsl`), not pooled,
 * so it is not a symbol address -- a symbol would pool.  What the evidence points
 * at is a base held in something gcc-2.96 cannot constant-propagate, and the
 * honest statement is that the namespace is unidentified.  The pool word
 * `.word 0x000001e3` in our object is the visible residue; it is 1 of the 22.
 *
 * ================== THE RESIDUE ==================
 * With size, instruction count, branch structure and pass-1 shape exact, what is
 * left is register assignment and one stack slot:
 *  - `kind` lands at [sp,#8] and `cur` at [sp,#0xc]; the ROM has them at 0xc and
 *    0x10, i.e. EVERY slot shifted down by one word.  The word we are missing is
 *    the ROM's [sp,#4], where it SPILLS the `rec + 0x54` pointer across
 *    _CreateSprite in the one-sprite path (`str r1,[sp,#4] / bl / ldr r1,[sp,#4]`).
 *    We keep it in a callee-saved register instead -- the same "the ROM is under
 *    more pressure than gcc gives us" fault as everywhere else in this batch.
 *    Fixing that one spill should carry every slot with it, and it is the next
 *    thing to try.  A pad local will NOT do it: [sp,#4] is genuinely read and
 *    written, so the two missing instructions have to be generated, not reserved.
 *  - `base`/`slot` take r9/sl where the ROM takes sl/r9.  Swapping their
 *    declaration order changes nothing (measured).
 *  - the two `ldr r5,[r6,#0x28]` / `strb` orderings, and the register holding
 *    `kind` at the merged `spr[0x26] =` tail.
 *
 * AN ALTERNATIVE DRAFT WORTH KEEPING: without step 4 (one `id` for all three
 * uses) the function is 283/616 -- size and count BOTH wrong -- but aligncmp reads
 * 57.4% against this draft's 51.8%.  The two metrics disagree, which is exactly
 * the trap docs/elevation.md warns about; this draft is the one parked because its
 * count is a true distance, but a future worker should re-derive from the 57.4%
 * variant if the slot fix above does not land.  Its only difference from this file
 * is that `res` and `lid` are spelled `id`.
 */
extern int Func_8000888(int, int);

static inline int call_via(int (*f)(int, int), int a, int b)
{
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\t%1"
        : "=r" (_a)
        : "r" (f), "0" (_a), "r" (_b)
        : "memory", "lr", "r12"
    );
    return _a;
}

extern void Func_80008d4(void *dst, int len);
extern int Func_80b770c(short *list, int i);
extern void Func_80b7b30(int unit);
extern unsigned char *GetBattleActor(int unit);
extern void Func_80b78e4(int unit, void *actor);
extern void Func_80b7aac(int unit);
extern void WaitFrames(int n);
extern unsigned char *_CreateSprite(int resource);
extern unsigned char *_Sprite_AddLayer(void *s, int n);
extern void _SpriteLayer_SetAnim(unsigned char *p, int b);
extern unsigned char *_GetSpriteInfo(int id);
extern unsigned char iwram_3001a10;
extern unsigned char *iwram_3001e68;

void CreateBattleSpriteOverlays(short *list, int mode)
{
    unsigned char *actor;
    unsigned char *rec;
    unsigned char *spr;
    unsigned char *lay;
    unsigned char *state;
    unsigned char *base;
    unsigned char **slot;
    short *p;
    register int i __asm__("r11");
    int cur;
    int id;
    int res;
    int lid;
    unsigned char kind;
    void (*fp2)(void *, int);

    i = 0;
    do {
        if (Func_80b770c(list, i) == 0) {
            Func_80b7b30(i > 7 ? i + 0x78 : i);
        }
        i++;
    } while (i <= 0xd);
    if (iwram_3001a10 == 0)
        WaitFrames(1);
    i = 0;
    cur = list[0];
    if (cur == 0xff)
        return;
    p = list;
    do {
        if (cur != 0xfe) {
            actor = GetBattleActor(cur);
            if (actor != 0) {
                Func_80b78e4(cur, actor);
                rec = *(unsigned char **)actor;
                if (rec != 0) {
                    state = rec + 0x54;
                    kind = *state;
                    if (kind == 0) {
                        id = *(unsigned short *)(actor + 4) & 0xfff;
                        if (id == 0x1dc || id == 0x1dc + 7) {
                            base = iwram_3001e68;
                            base = base + *(int *)(base + 0x18) * 4;
                            slot = (unsigned char **)(base + 8);
                            *state = 2;
                            res = *(unsigned short *)(actor + 4);
                            *(unsigned char ***)(rec + 0x50) = slot;
                            fp2 = Func_80008d4;
                            fp2(slot, 0x10);
                            spr = _CreateSprite(res);
                            if (spr != 0) {
                                *(int *)(spr + 0x18) =
                                    call_via(Func_8000888,
                                             *(int *)(spr + 0x18),
                                             *(int *)(actor + 0x18));
                                *(unsigned short *)(rec + 0x20) =
                                    _GetSpriteInfo(res)[9] >> 1;
                                *slot = spr;
                                slot = (unsigned char **)(base + 0xc);
                            }
                            spr[0x26] = kind;
                            spr = _CreateSprite(res + 0x2001);
                            if (spr != 0) {
                                *(int *)(spr + 0x18) =
                                    call_via(Func_8000888,
                                             *(int *)(spr + 0x18),
                                             *(int *)(actor + 0x18));
                                *slot = spr;
                            }
                            spr[0x26] = kind;
                        } else {
                            spr = _CreateSprite(*(unsigned short *)(actor + 4));
                            if (spr != 0) {
                                *state = 1;
                                *(unsigned char **)(rec + 0x50) = spr;
                                *(int *)(spr + 0x18) =
                                    call_via(Func_8000888,
                                             *(int *)(spr + 0x18),
                                             *(int *)(actor + 0x18));
                                lay = *(unsigned char **)(spr + 0x28);
                                lay[6] = 1;
                                lay[5] = *(int *)(actor + 0x14);
                                lid = *(unsigned short *)(actor + 6);
                                if (lid != 0) {
                                    lay = _Sprite_AddLayer(spr, lid);
                                    lay[6] = 1;
                                }
                                lid = *(unsigned short *)(actor + 8);
                                if (lid != 0) {
                                    lay = _Sprite_AddLayer(spr, lid);
                                    *(unsigned char **)(actor + 0x20) = lay;
                                    _SpriteLayer_SetAnim(lay, 0);
                                    lay[6] = 3;
                                }
                                lid = *(unsigned short *)(actor + 0xa);
                                if (lid != 0) {
                                    if (spr[0x20] == 0x20)
                                        lid = 0x1ff;
                                    lay = _Sprite_AddLayer(spr, lid);
                                    *(unsigned char **)(actor + 0x24) = lay;
                                    lay[6] = kind;
                                    spr[0x26] = kind;
                                }
                            }
                        }
                        Func_80b7aac(cur);
                    }
                }
            }
        }
        i++;
        p++;
        if (i > 0xd)
            break;
        cur = *p;
    } while (cur != 0xff);

    if (mode != 0) {
        cur = list[0];
        i = 0;
        while (cur != 0xff) {
            if (list[i] != 0xfe) {
                actor = GetBattleActor(cur);
                if (actor != 0) {
                    rec = *(unsigned char **)actor;
                    if (rec != 0)
                        Func_80b7aac(cur);
                }
            }
            i++;
            if (i > 0xd)
                break;
            cur = list[i];
        }
    }
}
