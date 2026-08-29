/* Anim_Summon  --  0x080d6578
 *
 * Cut out of goldensun/asm/rom_c9000/rom_d6504_a_c.s.
 *
 * Allocates the three summon-animation buffers, dispatches on the class in
 * descriptor word 0, and frees all three on the way out -- so every summon
 * handler runs with the same scratch already in place.
 *
 * CASE 0 IS PEELED, AND THAT IS A STATEMENT ABOUT THE SOURCE. The ROM tests
 * `cmp r3, #0 / beq` before the table, then subtracts 1 and dispatches on
 * 1..12. Written as one switch with `case 0:` grouped alongside `case 11:`,
 * gcc builds a thirteen-slot table from 0 and the peel disappears (measured:
 * 90 lines against 92, 77 differing). Written as
 *
 *     if (c == 0) { Anim_Meteor(desc); } else switch (c) { ... }
 *
 * it reproduces exactly -- and gcc then cross-jumps the if-branch into case
 * 11's body on its own, which is why one `beq` reaches a table target.
 *
 * `else switch` and `if (...) { ...; return; } switch` are NOT the same here:
 * the early-return form loses the shared epilogue and screens at 97 lines,
 * five long. The three `gfree` calls have to be on the common path.
 *
 * The twelve handler names come from the branch targets, not from the header
 * comment on the .s, which is stale for slots 7 and 11 (Anim_Tiamat and
 * Anim_Meteor, not Func_d2458 and Func_e7320).
 */
extern void *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern void Anim_Ramses(int *desc);
extern void Anim_Nereid(int *desc);
extern void Anim_Kirin(int *desc);
extern void Anim_Atalanta(int *desc);
extern void Anim_Cybele(int *desc);
extern void Anim_Neptune(int *desc);
extern void Anim_Tiamat(int *desc);
extern void Anim_Procne(int *desc);
extern void Anim_Judgment(int *desc);
extern void Anim_Boreas(int *desc);
extern void Anim_Meteor(int *desc);
extern void Anim_Thor(int *desc);

void Anim_Summon(int *desc)
{
    int c;

    galloc_iwram(0x29, 0x302);
    galloc_iwram(0x27, 0x782c);
    galloc_iwram(0x28, 0x80 << 7);
    c = desc[0];
    if (c == 0) {
        Anim_Meteor(desc);
    } else switch (c) {
    case 1:
        Anim_Ramses(desc);
        break;
    case 2:
        Anim_Nereid(desc);
        break;
    case 3:
        Anim_Kirin(desc);
        break;
    case 4:
        Anim_Atalanta(desc);
        break;
    case 5:
        Anim_Cybele(desc);
        break;
    case 6:
        Anim_Neptune(desc);
        break;
    case 7:
        Anim_Tiamat(desc);
        break;
    case 8:
        Anim_Procne(desc);
        break;
    case 9:
        Anim_Judgment(desc);
        break;
    case 10:
        Anim_Boreas(desc);
        break;
    case 11:
        Anim_Meteor(desc);
        break;
    case 12:
        Anim_Thor(desc);
        break;
    }
    gfree(0x28);
    gfree(0x27);
    gfree(0x29);
}
