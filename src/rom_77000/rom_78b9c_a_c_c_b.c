/* GiveInnateMove  --  0x08078e28, split out of asm/rom_77000/rom_78b9c_a_c_c.s;
 * Func_8078bf0 stays in _a.s. Matched from scratch.
 *
 * EXIT-TEST DUPLICATION TIMING DECIDES BOTH STRENGTH REDUCTION AND THE PRE-CHECK.
 * jump.c's duplicate_loop_exit_test copies an exit block of 20 insns or fewer.
 * If the loop's exit code is <= 20 at JUMP1 it is copied BEFORE loop.c, the loop
 * is "ignored due to multiple entry points" and never reduced. If it is over 20
 * at jump1 but shrinks under 20 after loop.c, the post-loop jump pass copies it,
 * giving the ROM's pre-check plus duplicated found-block on a REDUCED loop. The
 * switch here is the element type: an HImode struct-field store expands to
 * and/ior subreg ops (27 insns -- kept, reduced) while a plain u16[32][2] view
 * is 17 (duplicated early, unreduced). One union carries both views, which is
 * how loop 1 comes out reduced and loop 2 not, as in the ROM.
 *
 * `if ((unsigned short)v == move)` makes cse record the equivalence on the
 * zero-extend temp, so the found-store keeps `v` rather than being rewritten to
 * `move` (the longer-lived pseudo wins); combine drops the extension.
 */
struct MoveSlot { unsigned short id; unsigned short pp; };
struct Unit { unsigned char pad[0x58]; union { struct MoveSlot s[32]; unsigned short h[32][2]; } moves; };
extern struct Unit *GetUnit(int unit);
extern void Func_8078bf0(int unit);

int GiveInnateMove(int unit, int move)
{
    struct Unit *u;
    int r;
    int i;
    int v;

    u = GetUnit(unit);
    move &= 0x3fff;
    r = -1;
    for (i = 0; i <= 0x1e; i++) {
        v = u->moves.s[i].id & 0x3fff;
        if ((unsigned short)v == move) {
            u->moves.s[i].id = v;
            r = i;
            break;
        }
    }
    if (r < 0) {
        for (i = 0; i <= 0x1e; i++) {
            if (u->moves.h[i][0] == 0) {
                u->moves.h[i][0] = move;
                r = i;
                break;
            }
        }
        if (r < 0)
            return -1;
    }
    Func_8078bf0(unit);
    for (i = 0; i <= 0x1f; i++)
        if (u->moves.s[i].id == move)
            break;
    return i;
}
