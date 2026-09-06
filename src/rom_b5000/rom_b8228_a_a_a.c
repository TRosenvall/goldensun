/* SetBattleActorKnockback  --  0x080b8228.
 *
 * THE WHOLE of goldensun/asm/rom_b5000/rom_b8228_a_a_a.s: one function, no
 * data.  DESTINATION src/rom_b5000/rom_b8228_a_a_a.c.
 *
 * NO LINKER EDIT.  stage1.ld already names the .o, once, on one line:
 *
 *     asm/rom_b5000/rom_b8228_a_a_a.o(.text)
 *
 * (between rom_b7410_c_c_c_c_c.o(.text) and rom_b8228_a_a_b.o(.text)).  The
 * .o keeps its name and its slot; the .s is deleted and rebuilt from this .c
 * through the `asm/%.o: src/%.c` rule -- the same shape as the already-landed
 * src/rom_b5000/rom_b8228_c_a_a.c.
 *
 * NO FLAG GROUP.  The only explicit rule anywhere under rom_b5000 is for
 * asm/rom_b5000/rom_b8228_c_a_c_c_a_c_b.o (STRENGTH_CFLAGS), a different file,
 * and every non-default PATTERN rule in the Makefile targets asm/overlays/,
 * src/lib/m4a/ or tools/.  This TU takes stock GCC296_CFLAGS, which is what it
 * was screened and verified under.
 *
 * WHAT IT DOES.  Puts a combatant into a reaction pose: four parallel tables
 * indexed by the animation number supply the two motion words (+0x34, +0x30),
 * an optional third (+0x28) and a travel scale.  The whole body is SKIPPED --
 * only _Actor_SetAnim(actor, 5) survives -- when the unit's class byte at
 * +0x128 is 0x94.
 *
 * LEVERS, with the mechanism:
 *
 *  - THE FOUR TABLES ARE REACHED WITH gcc's ASM-LABEL EXTENSION.  `.Lc59a4`
 *    and friends are file-local labels defined in asm/rom_b5000/rom_b8228_c_c.s,
 *    which already carries the four `.global` directives they need, so nothing
 *    outside this file changes (§"Technique: reaching a file-local `.L` symbol
 *    from C").  Declaring them `extern int tbl[]` and subscripting by `anim`
 *    is what produces the ROM's one-shot `lsl r5, r2, #2` reused as the index
 *    register in all four `ldr r3, [r3, r5]` -- an explicit `*(int *)((char *)
 *    tbl + (anim << 2))` would not (§"`(idx << 1)` and `idx * 2` are not
 *    interchangeable in a register-offset store").
 *
 *  - THE CLASS BYTE IS A STRUCT FIELD AT 0x128, NOT AN INDEX.  The ROM's
 *    `mov r2, #0x94 / lsl r2, #1 / add r0, r2 / ldrb r3, [r0]` is gcc's OWN
 *    split of the offset 296: thumb's `add rN, #imm` stops at 255, and the
 *    mov+lsl pair takes the smallest shift that reaches it.  It is not a
 *    source-level shift and must not be written as one.
 *
 *  - THE GUARD IS INVERTED.  `if (u->cls != 0x94) { body }` puts the body on
 *    the fall-through and the ROM's `beq` on the skip (§"Lever: invert the
 *    guard so the BODY is the taken branch").
 *
 *  - THE COMPOUND CONDITION IS LEFT COMPOUND.  `if (actor->fc == 0 || anim > 4)`
 *    compiles straight to the ROM's `beq .Lb8266 / cmp r2, #4 / ble .Lb826c`
 *    fall-through pair.  §"A COMPOUND CONDITION FUSES; SPLIT IT INTO
 *    STATEMENTS" did NOT apply here and splitting it was not needed --
 *    a reminder that the section is a remedy, not a rule.
 *
 *  - THE MULTIPLY'S OPERAND ORDER IS THE WHOLE REMAINING DIFFERENCE.
 *    `mul` copies its SECOND operand into the destination, so
 *    `rec->fc * tblSpd[anim]` gives the ROM's `mov r0, r3(table) / mul r0,
 *    r2(fc)` AND the ROM's load order (`ldr r2, [r7, #0xc]` interleaved
 *    between the pool load and the indexed load).  Written the other way
 *    round, `tblSpd[anim] * rec->fc`, the two loads swap and the operands with
 *    them: 64 lines against 64, 2 differing.  Naming rec->fc in a local first
 *    (`c = rec->fc;`) does NOT fix it -- also 64 / 2.
 *
 * MEASURED AND WORSE:
 *
 *     spelling                                     lines / differing
 *     `rec->fc * tblSpd[anim]`      (this file)       64 / 0   EXACT
 *     `tblSpd[anim] * rec->fc`                        64 / 2
 *     `c = rec->fc;` then `tblSpd[anim] * c`          64 / 2
 *
 * VERDICT
 *   OK SetBattleActorKnockback -- 156 bytes, 67 encodings and 10 relocations
 *   identical
 * (tools/objcmp.py against asm/rom_b5000/rom_b8228_a_a_a.s, a single-function
 * reference, so the SIZE check engaged as well; and, before the .s was split,
 * against the original asm/rom_b5000/rom_b8228_a_a.s with --func.)
 */
struct A {
    unsigned char pad00[0xc];
    int fc;
    unsigned char pad10[0x18];
    int f28;
    unsigned char pad2c[4];
    int f30;
    int f34;
    unsigned char pad38[0xc];
    int f44;
    int f48;
    unsigned char pad4c[0xe];
    unsigned char f5a;
};

struct C {
    struct A *f0;
    unsigned char pad04[8];
    int fc;
    int f10;
};

struct U {
    unsigned char pad00[0x128];
    unsigned char cls;
};

extern struct C *GetBattleActor(int id);
extern struct U *_GetUnit(int id);
extern void _Actor_Stop(struct A *a);
extern void _Actor_TravelTo(struct A *a, int x, int y, int z);
extern void _Actor_SetAnim(struct A *a, int anim);
extern int tbl34[] __asm__(".Lc59a4");
extern int tbl30[] __asm__(".Lc59c4");
extern int tbl28[] __asm__(".Lc59e4");
extern int tblSpd[] __asm__(".Lc5a04");

void SetBattleActorKnockback(int id, int anim)
{
    struct C *rec;
    struct A *actor;
    struct U *u;

    rec = GetBattleActor(id);
    actor = rec->f0;
    u = _GetUnit(id);
    if (u->cls != 0x94) {
        actor->f34 = tbl34[anim];
        actor->f30 = tbl30[anim];
        if (actor->fc == 0 || anim > 4) {
            actor->f28 = tbl28[anim];
        }
        actor->f48 = 0x9999;
        actor->f44 = 0;
        actor->f5a = 0;
        _Actor_Stop(actor);
        _Actor_TravelTo(actor, rec->fc * tblSpd[anim] / 100, 0, rec->f10);
    }
    _Actor_SetAnim(actor, 5);
}
