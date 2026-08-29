/* Func_8079d7c  --  0x08079d7c
 *
 * Cut out of goldensun/asm/rom_77000/rom_79460_c_c_c_c_a_c_c_a_c_c.s.
 *
 * Maps a status or class id to a numeric weight, negative for two of them and
 * for everything unlisted.
 *
 * A FIFTY-SLOT TABLE WITH FIFTEEN TARGETS, and the whole function is the
 * switch. The case groupings are read straight off the table -- 8, 9, 28 and 32
 * all return 0x3c; 18, 19 and 25 all return 0x37 -- and the block order gives
 * the source order, which is by RETURN VALUE and not by case value.
 *
 * THE `neg` IS SHARED BY THREE ARMS. `case 56: return -0x3c;` and
 * `case 57: return -0x5a;` and the trailing `return -0x64;` each compile to
 * `mov r0, #k` followed by a jump into one `neg r0, r0`. Writing the negative
 * literals is enough -- gcc cross-jumps the three tails itself.
 *
 * Matched on the first screen once the table was transcribed.
 */
int Func_8079d7c(int n)
{
    switch (n) {
    case 12:
    case 13:
        return 0x46;
    case 16:
    case 17:
        return 0x4b;
    case 22:
        return 0x1e;
    case 23:
        return 0x28;
    case 24:
        return 0x2d;
    case 18:
    case 19:
    case 25:
        return 0x37;
    case 26:
        return 0x19;
    case 27:
        return 0x14;
    case 20:
    case 31:
        return 0x41;
    case 21:
    case 34:
        return 0x23;
    case 35:
        return 0x32;
    case 56:
        return -0x3c;
    case 57:
        return -0x5a;
    case 8:
    case 9:
    case 28:
    case 32:
        return 0x3c;
    }
    return -0x64;
}
