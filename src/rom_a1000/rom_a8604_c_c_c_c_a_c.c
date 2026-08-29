/* Func_80aa460  --  0x080aa460
 *
 * The whole of goldensun/asm/rom_a1000/rom_a8604_c_c_c_c_a_c.s.
 *
 * Picks the sound a move makes: element 1 plays it twice, element 0xB once, and
 * anything else selects by the move's animation id through a 32-entry jump
 * table.
 *
 * THE FIRST SWITCH FALLS THROUGH, and the ROM shows it plainly: `.Laa476` runs
 * `mov r0, #0x7e / bl Func_80a2438` and then falls INTO `.Laa47c`, which does
 * the same call again before branching to the exit. Written as two separate
 * `if` arms gcc would emit one call each and cross-jump them; the fallthrough
 * is what gives element 1 two calls and element 0xB one.
 *
 * With only two case labels the outer switch comes out as an equality chain
 * rather than a decision tree -- batch 91's rule.
 *
 * THE CASE ORDER IN THE SOURCE DECIDES THE BLOCK ORDER. The inner switch's
 * table is 32 entries with most of them pointing at the default. Its two
 * interesting cases are 3 and 5, and the ROM emits case 5's block first.
 * Writing `case 3:` before `case 5:` puts them the other way round and leaves
 * four positions differing; writing `case 5:` first matches. With a jump table
 * the bodies come out in source order, so the ROM's layout reads the source's.
 *
 * The four do-nothing cases (1, 2, 31 and 32) are what point their table slots
 * at the function exit rather than at the default block, so they have to be
 * written out even though their bodies are empty.
 */
struct M {
    unsigned char pad00[1];
    unsigned char f1;
    unsigned char pad02[1];
    unsigned char f3;
};

extern struct M *_GetMoveInfo(int id);
extern void Func_80a2438(int n);

void Func_80aa460(int move)
{
    struct M *m;
    int k;

    m = _GetMoveInfo(move);
    k = m->f1 & 0xf;
    switch (k) {
    case 1:
        Func_80a2438(0x7e);
    case 0xb:
        Func_80a2438(0x7e);
        break;
    default:
        switch (m->f3) {
        case 1:
        case 2:
        case 31:
        case 32:
            break;
        case 5:
            Func_80a2438(0x52);
            break;
        case 3:
            Func_80a2438(0x54);
            break;
        default:
            Func_80a2438(0x5b);
            break;
        }
        break;
    }
}
