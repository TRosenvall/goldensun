/* Cluster Func_80a1fd4..Func_80a1fd4 extracted from goldensun/asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a_a_c.s.
 *
 * Total .text for this TU = 368 bytes (= 0x170). Never attempted before batch 277.
 * No pins, no flags. 172 instructions. Three coupled levers, and the third is new.
 *
 * 1. A `ret` LOCAL WITH A SINGLE EXIT -- the `pop {r1} / bx r1` tell again -- so the two
 *    `-1`s land before their tests rather than in one merged block. 180 to 181 instructions
 *    but 140 to 119 differing.
 * 2. `*row * cols`, NOT `cols * *row`, in the up/down arms.
 * 3. `ret = 0;` placed BEFORE the two `*col = 0` stores in the right arm.
 *
 * LEVER 3 IS THE FINDING: cse1 SUBSTITUTES THE NEAREST BRANCH-PROVEN-ZERO PSEUDO FOR A
 * `const_int 0` STORE, AND THAT IS A REGISTER-ALLOCATION LEVER.
 *
 * Our `left` was landing in r6, which pushed `colptr` to r7 and `cols` to r8 and cost a
 * fourth high register -- eight extra instructions. The cause: `*col = 0;` in the right arm
 * became `str <left>`, because `left` was the nearest pseudo cse could prove zero there.
 * That extended `left`'s live range across `_PlaySound`, so `ALLOCNO_CALLS_CROSSED > 0`
 * excluded every call-used register -- r1 AND r4 under -fcall-used-r4. `.18.greg` showed
 * `39 conflicts: ... 126 127 128 129 130 133 134`, pseudos in a branch `left` has no
 * business being live in.
 *
 * THE FIX IS TO GIVE cse A NEARER ZERO, NOT TO TRY TO BLOCK IT. `ret = 0;` before the
 * stores is a zero you already need; `left` then dies at its test, takes r1, and `right`
 * takes r4 exactly as the ROM has them. A named `int z = 0;` does NOT work at ANY
 * declaration position -- cse merges it into `left`'s qty and canonicalises to `left`,
 * deleting `z`. `volatile` fails too.
 *
 * THE TELL, so this is recognisable next time: a `str <callee-saved reg>` where the ROM has
 * `str r0` after a `mov r0, #0`, together with a push list one register too wide. That is a
 * zero-substitution problem, not a spilling problem, and the lever is the ORDER of an
 * assignment you were going to write anyway.
 */
extern volatile unsigned int gKeyRepeat;
extern void _Func_80219c8(unsigned int addr);
extern void _PlaySound(int id);
extern void Func_800352c(void);

int Func_80a1fd4(int swap, int total, int cols, int *col, int *row)
{
    int rows;
    int right, left, up, down;
    int ret;

    ret = -1;
    if (total == 0)
        goto out;
    _Func_80219c8(0x6002500);
    rows = total / cols;
    if (total % cols != 0)
        rows++;
    if (swap != 0) {
        right = gKeyRepeat & 0x10;
        left = gKeyRepeat & 0x20;
        up = gKeyRepeat & 0x40;
        down = gKeyRepeat & 0x80;
    } else {
        right = gKeyRepeat & 0x80;
        left = gKeyRepeat & 0x40;
        up = gKeyRepeat & 0x20;
        down = gKeyRepeat & 0x10;
    }
    if (up != 0) {
        _PlaySound(0x6f);
        *row = *row - 1;
        if (*row < 0)
            *row = rows - 1;
        if (*col + *row * cols > total - 1) {
            *col = total - *row * cols - 1;
            if (*col > cols - 1)
                *col = cols - 1;
        }
        Func_800352c();
        ret = 1;
        goto out;
    }
    if (down != 0) {
        _PlaySound(0x6f);
        *row = *row + 1;
        if (*row > rows - 1)
            *row = 0;
        if (*col + *row * cols > total - 1) {
            *col = total - *row * cols - 1;
            if (*col > cols - 1)
                *col = cols - 1;
        }
        Func_800352c();
        ret = 1;
        goto out;
    }
    if (left != 0) {
        _PlaySound(0x6f);
        *col = *col - 1;
        if (*col < 0) {
            *col = cols - 1;
            *col = total - *row * cols - 1;
            if (*col > cols - 1)
                *col = cols - 1;
        }
    } else {
        ret = -1;
        if (right == 0)
            goto out;
        _PlaySound(0x6f);
        ret = 0;
        *col = *col + 1;
        if (*col == total - *row * cols)
            *col = 0;
        if (*col > cols - 1)
            *col = 0;
    }
    ret = 0;
out:
    return ret;
}
