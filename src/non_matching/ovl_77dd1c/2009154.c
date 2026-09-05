/* OvlFunc_882_2009154  --  0x02009154   PARKED
 *   [asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_a_c.s, 2nd of 2]
 *
 * BLOCKER CLASS: AN INSTRUCTION gcc-2.96 CANNOT EMIT.
 *
 * Best result 2 differing encodings of 160. Size 412/412, pool order identical,
 * all 33 relocations identical in symbol AND offset. The single defect:
 *
 *     rom    movs r2,#35 / ldr r3,[r0,#0x50] / mov r8,r0     / add r8,r2
 *     ours   movs r2,#35 / ldr r3,[r0,#0x50] / adds r2,r2,r0 / mov r8,r2
 *
 * The ROM does the add IN PLACE INTO A HIGH REGISTER. This compiler, as
 * configured here, never emits that form. Measured over the whole tree rather
 * than argued:
 *
 *     `add rHIGH, rN` (two-operand, high destination)
 *        244 of the ROM-disassembly .s files contain it
 *          0 of the 3729 gcc-GENERATED .s files contain it
 *
 * Zero out of 3729 is the same kind of corpus evidence as the recorded "two
 * consecutive neg" test, and it should be CHECKED BEFORE spending screens on
 * high-register address arithmetic. Read from the .00.rtl dump, the cause is
 * that gcc-2.96's expander never produces an in-place add -- it routes through
 * a temp pseudo, combine folds the temp and the preceding copy, and the
 * surviving `(set r8 (plus p 35))` needs two reloads under either alternative,
 * with reload taking the earlier low-register one.
 *
 * THE RESIDUE IS NOT AN ARTIFACT OF THE `__asm__` PIN, and that was controlled
 * for: under `-ffixed-r7`, with `t` left as a naturally allocated pseudo and no
 * pin at all, the add STILL comes out `adds r2,r2,r0`. That flag reproduces the
 * ROM's register SET from pin-free C (160 encodings, 412 bytes) but permutes it,
 * at 127 differing, and on top of the shipping pins it changes nothing -- so no
 * Makefile rule is wanted here either.
 *
 * ALSO REFUTED HERE: the recorded "the ROM's `add` says which form to write"
 * table does NOT extend to a high destination. For r8-r11 the walk form and the
 * single-expression form are indistinguishable, because the destructive
 * `add r8, r2` is a RELOAD ARTIFACT rather than a source form at all.
 *
 * WHAT GOT IT FROM 127 TO 2 (all of it kept in the body below, since a later
 * attempt should start here rather than re-derive it):
 *   - THE BASE POINTER MUST BE LIVE AT THE ADD, which is statement ORDER only:
 *     `t = p + 0x23;` must come BEFORE `r = *(unsigned char **)(p + 0x50);`.
 *     Dead `p` lets gcc destroy r0 in place (`adds r0,#35`, no `movs r2,#35` at
 *     all); live `p` forces 35 into a register, which is the ROM's `movs r2,#35`.
 *     The obvious order costs 122 positions.
 *   - The r8 pin on `t` is load-bearing (without it gcc takes r7: 147 of 160).
 *   - NAME THE STORED BYTE IN A LOW REGISTER, 5 -> 2. With the address in r8 and
 *     the value in r10 both need a low register; reload emits the INPUT reload
 *     before the OUTPUT-ADDRESS reload, so the value grabs r2. `sv = saved;
 *     *t = sv;` with `sv` pinned r3 makes the value a real move in source
 *     position, leaving the address as the only reload -- it takes r2 and goes
 *     first, as the ROM has it.
 *
 * MEASURED INERT AND THEREFORE NOT SHIPPED: pins on `p`->r0, `r`->r3, `a`->r6,
 * `saved`->r10, individually and combined. The `OvlFunc_882_2009a64` calls want
 * NO pin -- plain C is byte-identical to the descending pin, and the ascending
 * one is wrong. Both `__Func_8092b08` pins are inert. Greedy minimisation to a
 * fixpoint from a confirmed candidate dropped 8 of 20 levers.
 *
 * AND THE TEMPLATE NEIGHBOUR'S CURE IS WRONG HERE: ovl_30_c_c_c_a_c_c_c_c_a_b.c
 * records `v = 0xc0; v <<= 9;` as its lever for the five actor-offset stores;
 * all three splits measure byte-identical inline on this function, so inline
 * ships. Another instance of a sibling's cure not transferring.
 *
 * FLAGS TRIED: -fno-schedule-insns2 and -O1 both move relocations (28
 * differing); fifteen other -fno-* flags change nothing (still 2).
 *
 * NEXT: nothing at the spelling level -- the remaining difference is an
 * instruction form this compiler does not generate. It would need either a flag
 * that changes reload's ordering without disturbing anything else, or evidence
 * that the ROM's function was built from different source than assumed. Do NOT
 * re-derive the 127 -> 2 path; it is above.
 */
/* OvlFunc_882_2009154  --  0x02009154   *** PARKED at 2 of 160 ***
 *
 * Cut out of asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_a_c.s, which holds
 * TWO functions -- OvlFunc_882_20090a4 (line 6) then OvlFunc_882_2009154
 * (line 94).  The target is the LAST one, so LANDING NEEDS A TAIL SPLIT:
 * overlays/rom_77dd1c/overlay.ld line 41
 *
 *     asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_a_c.o(.text)
 *
 * becomes _c (keeping 20090a4 in asm) followed by a new _d holding this C.
 * No Makefile pattern rule reaches this TU: objcmp against the ORIGINAL asm
 * path prints no "(built with: ...)" line, so it builds at plain -O2, and the
 * extracted scratch reference is byte-identical to the in-tree one.
 *
 * STATUS.  412 bytes against 412, 160 encodings against 160, all 33
 * relocations identical in symbol AND offset, pool order identical.  TWO
 * instructions differ, and they are one address computation:
 *
 *     rom    movs r2,#35 / ldr r3,[r0,#0x50] / mov r8,r0    / add r8,r2
 *     ours   movs r2,#35 / ldr r3,[r0,#0x50] / adds r2,r2,r0 / mov r8,r2
 *
 *   XX ENCODINGS differ in 2 place(s) (ref 160, ours 160)
 *      first at index 21: ref 4680  ours 1812
 *
 * ---------------------------------------------------------------- LEVERS ---
 *
 * 1. THE BASE POINTER MUST STILL BE LIVE AT THE ADD.  127 of 160 -> 5.  This
 *    is the single largest lever in the function and it is pure statement
 *    ORDER.  Write the `+ 0x23` BEFORE the load that consumes `p`:
 *
 *        t = p + 0x23;                              <- p still live here
 *        r = *(unsigned char **)(p + 0x50);         <- p dies here
 *
 *    Mechanism: with `p` dead at the add, gcc owns r0 and destroys it in
 *    place -- `adds r0,#35 / mov r8,r0`, the 8-bit immediate form, and no
 *    `movs r2,#35` at all.  With `p` live it must materialise 35 in a
 *    register first, which is the ROM's `movs r2,#35`.  Putting the load
 *    first (the "obvious" order, and what every earlier candidate here had)
 *    costs 122 positions.  The reverse -- `r` first -- is variants a1..a8 and
 *    g5..g6 in this directory, all 127.
 *
 * 2. THE r8 PIN IS LOad-BEARING; r5/r6/r10 ARE NOT.  `register unsigned char
 *    *t __asm__("r8")` is required: without it gcc allocates r7 (147 of 160,
 *    404 bytes, prologue `push {r5,r6,r7,lr}`).  gcc's Thumb call-saved order
 *    is r5, r6, r7, r8, r10 and the ROM skips r7 -- the standard shape.
 *    Pinning `a` to r6 or `saved` to r10 measures BYTE-IDENTICAL to not
 *    pinning them and MUST NOT SHIP.  The y/r5 pin does earn its place (13 of
 *    160 without it) -- it is not interchangeable with the a/r6 pin even
 *    though either alone reached 5 before lever 3 landed.
 *
 * 3. NAME THE STORED BYTE IN A LOW REGISTER TO ORDER THE STORE'S TWO COPIES.
 *    5 of 160 -> 2.  With the address in r8 and the value in r10, both need a
 *    low register and reload picks the order:
 *
 *        rom    mov r2,r8  / mov r3,sl / strb r3,[r2]   address copied first
 *        ours   mov r2,sl  / mov r3,r8 / strb r2,[r3]   value copied first
 *
 *    Reload emits the INPUT reload (the value) before the OUTPUT-ADDRESS
 *    reload, so the value takes the first spill register.  Turning the value
 *    into a real pinned move takes it out of reload's hands entirely:
 *
 *        sv = saved;        -- sv is register int __asm__("r3")
 *        *t = sv;
 *
 *    the pin emits `mov r3,sl` in source position and the only reload left is
 *    the address, which then takes r2 and is emitted first.  Three spellings
 *    of this all reach 2 (z1 pins the ADDRESS to r2, z2 pins both, z3 pins
 *    only the value); z3 is the smallest and is what ships.
 *
 * 4. THE OvlFunc_882_2009a64 CALLS WANT NO PIN AT ALL.  The ROM emits
 *    `ldr r1,=0x29d` before `movs r0,#0x3e` -- argument 1 before argument 0.
 *    An ASCENDING PIN2 gets this wrong; a DESCENDING PIN2 (`q1 = ...; q0 =
 *    ...;`) gets it right; and PLAIN C `OvlFunc_882_2009a64(0x3e, 0x29d)` is
 *    byte-identical to the descending pin.  The pin was scaffolding that
 *    measured inert once it was spelled correctly, so it is gone.  Both
 *    __Func_8092b08 pins went the same way.
 *
 * 5. THE NEIGHBOUR'S "NAMED SHIFTED LOCAL" LEVER IS INERT HERE.  The template
 *    src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_a_b.c records `v = 0xc0;
 *    v <<= 9;` as a lever for the five actor-offset stores.  Measured on this
 *    function all three of them (v, w/u, x) are byte-identical written
 *    inline, so the inline form ships.  A sibling's cure is a hypothesis, not
 *    a result.
 *
 * The greedy minimiser (mini.py, run to a fixpoint from a CONFIRMED-exact-
 * count candidate) dropped 8 of 20 levers: both 8092b08 pins, both 2009a64
 * pins, all three shifted-store splits, and the split `t = p; t += 0x23;`
 * walk form.  12 survive.
 *
 * ------------------------------------------------------------- BLOCKER ---
 * *** NEW ***  gcc-2.96 AS CONFIGURED HERE NEVER EMITS A HIGH-DESTINATION
 * TWO-OPERAND REGISTER ADD.  Corpus test over the whole tree:
 *
 *     `add rHIGH, rN`   ROM disassembly:  1213 in 244 of 1083 .s files
 *                       gcc-generated:       0 in   0 of 3729 .s files
 *
 * So `add r8, r2` is not a spelling we have failed to find -- it is outside
 * this compiler's output set, the same way "two consecutive `neg rN,rN`" is.
 * Check that count before spending screens on any high-register address
 * arithmetic.
 *
 * WHY.  gcc's `*thumb_addsi3` reaches the high-destination alternative only
 * when operand 1 already equals operand 0 -- an IN-PLACE add.  gcc-2.96's
 * expander never produces one: `t += 0x23` on a hard-register variable
 * expands as `(set (reg 49) (plus (reg 8) 35))` + `(set (reg 8) (reg 49))`
 * (verified in the .00.rtl dump), and combine then folds BOTH the temporary
 * and the preceding copy into `(set (reg 8) (plus (reg p) 35))`.  From there
 * the low-register alternative and the high-register alternative each cost
 * two reloads, reload takes the earlier alternative, and out comes
 * `adds r2,r2,r0 / mov r8,r2`.
 *
 * WHAT DOES NOT REACH IT, all measured on this function:
 *
 *   | spelling                                             | differing |
 *   |------------------------------------------------------|-----------|
 *   | `t = p + 0x23;`                        (ships)        |     2     |
 *   | `t = p; t += 0x23;`                                   |     2     |
 *   | `t = p; t = t + 0x23;`                                |     2     |
 *   | `t = 0x23 + p;`                                       |     2     |
 *   | `t = &p[0x23];`                                       |     2     |
 *   | `t = (unsigned char *)((int)p + 0x23);`               |     2     |
 *   | `register int c23 __asm__("r2"); c23=0x23; t += c23;` |     2     |
 *   | `p` pinned to r0                                      |     2     |
 *   | `r` pinned to r3                                      |     2     |
 *   | `a` pinned r6 / `saved` pinned r10 / both             |     2     |
 *   | load through `t` instead of `p` (t = call; *(t+0x50)) |   127     |
 *   | load BEFORE the add (p dead at the add)               |   127     |
 *
 * The walk form vs. the single expression is the documented "the ROM's `add`
 * says which form to write" table (elevation.md).  *** NEW ***: that table
 * does NOT extend to a HIGH destination.  For r8-r11 the two source forms are
 * indistinguishable -- both funnel through the same combined RTL -- and the
 * destructive two-operand `add r8, r2` is a RELOAD artifact, not a source
 * form.  Do not read it as a walk.
 *
 * FLAGS.  Fifteen probed, all at 2 or worse: -fno-schedule-insns,
 * -fno-rerun-cse-after-loop, -fno-gcse, -fno-expensive-optimizations,
 * -fno-force-mem, -fno-caller-saves, -fno-peephole, -fno-cse-follow-jumps,
 * -fno-cse-skip-blocks, -fno-thread-jumps, -fno-delayed-branch,
 * -fno-strength-reduce, -fno-defer-pop, -fno-function-cse, -fno-inline.
 * -fno-schedule-insns2 and -O1 are both 28 with moved relocations.
 *
 * -ffixed-r7 WAS TRIED, and it is the right tell (the ROM saves r8 and r10
 * and skips r7).  It DOES give gcc the ROM's register SET from plain C with
 * no pins at all -- 160 encodings, 412 bytes -- but the assignment is a
 * permutation (a in r5 not r6, y in r6 not r5, t in r10 not r8, saved in r8
 * not r10) at 127 differing, and *** NEW ***: even with `t` a NATURALLY
 * ALLOCATED pseudo rather than a forced hard register, the add still comes
 * out `adds r2,r2,r0 / mov sl,r2`.  That refutes "the residue is an artifact
 * of the __asm__ pin" and confirms the blocker is reload's alternative
 * choice.  On top of the shipping pins -ffixed-r7 changes nothing (2 either
 * way), so no Makefile rule is wanted.
 */
extern void OvlFunc_882_20092f0(void);
extern void OvlFunc_882_2009a64(int a, int b);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __Func_809202c(void);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092b08(int a, int b);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_882_2009154(void)
{
    unsigned char *a;
    unsigned char *p;
    unsigned char *r;
    register unsigned char *t __asm__("r8");
    register int p0 __asm__("r0");
    unsigned char *q;
    register int sv __asm__("r3");
    register unsigned int y __asm__("r5");
    int saved, v, w, u, x, k, b;

    p0 = 0x312;
    if (__GetFlag(p0) == 0) {
        __CutsceneStart();
        p0 = 0x832;
        if (__GetFlag(p0) == 0) {
            a = __MapActor_GetActor(0xd);
            p = __MapActor_GetActor(0);
            t = p + 0x23;
            r = *(unsigned char **)(p + 0x50);
            y = r[9];
            saved = *t;
            { PIN3; q0 = 0x80 << 11; q1 = 0x80 << 11; q2 = 0x80 << 9;
              __Func_8012330(q0, q1, q2); }
            __PlaySound(0x8d);
            __WaitFrames(0x28);
            __PlaySound(0x91);
            __Func_8092b08(0, 3);
            q = __MapActor_GetActor(0) + 0x23;
            b = *q; k = 2; k |= b; *q = k;
            { PIN3; q0 = 0xd; q1 = 0; q2 = 0x2bf0000;
              __MapActor_SetPos(q0, q1, q2); }
            *(int *)(a + 0x30) = 0xc0 << 9;
            *(int *)(a + 0x34) = 0xc0 << 9;
            *(int *)(a + 0xc) += 0xa0 << 15;
            *(int *)(a + 0x3c) = *(int *)(a + 0xc);
            *(int *)(a + 0x44) = 0x80 << 8;
            { PIN3; q0 = 0xd; q1 = 0x40; q2 = 0x2bf;
              __Func_80921c4(q0, q1, q2); }
            __CutsceneWait(0x28);
            __PlaySound(0x121);
            { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
              __Func_8012330(q0, q1, q2); }
            y <<= 28;
            y >>= 30;
            __Func_8012350();
            __Func_809202c();
            p0 = 0x832;
            __SetFlag(p0);
            __Func_8092b08(0, y);
            q = __MapActor_GetActor(0) + 0x23;
            b = *q; k = 1; k |= b; *q = k;
            sv = saved;
            *t = sv;
        }
        OvlFunc_882_20092f0();
        p0 = 0x312;
        __SetFlag(p0);
        p0 = 0x837;
        if (__GetFlag(p0) != 0) {
            p0 = 0x841;
            if (__GetFlag(p0) == 0) {
                p0 = 0xc3; p0 <<= 2;
                if (__GetFlag(p0) == 0) {
                    p = __MapActor_GetActor(0);
                    if (*(int *)(p + 0x10) <= 0x2b4ffff) {
                        OvlFunc_882_2009a64(0x3e, 0x29d);
                        { PIN3; q0 = 0; q1 = 0x1b; q2 = 0x273;
                          __Func_80921c4(q0, q1, q2); }
                    } else {
                        OvlFunc_882_2009a64(0x4b, 0x2cb);
                        { PIN3; q0 = 0; q1 = 0x43; q2 = 0x2f5;
                          __Func_80921c4(q0, q1, q2); }
                    }
                    p0 = 0xc3; p0 <<= 2;
                    __SetFlag(p0);
                }
            }
        }
        __CutsceneEnd();
    }
}
