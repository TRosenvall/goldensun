/* Func_80a7f44 -- 0x080a7f44, asm/rom_a1000/rom_a7380_a_c.s (8 functions, no
 * .section .data; datacheck.py clean, so the split is pure text).
 *
 * EXACT: objcmp "OK Func_80a7f44 -- 240 bytes, 115 encodings and 4 relocations
 * identical", measured three times.
 *
 * Reorders the party list: copies the 15-halfword roster at base+0x208 into a
 * 14-int local, swaps the selected slot with its neighbour, re-registers every
 * member and rewrites the count at base+0x219.
 *
 * FOUR LEVERS, 111 differing to 0, and the two that matter are new here.
 *
 * 1. THE ZERO LOOP IS A do/while WITH AN int-CAST POINTER COMPARE. The ROM has
 *    no loop-entry guard and a SIGNED `cmp r3, r12 / bge`. A `for` over a
 *    pointer gives both wrong: gcc cannot prove the loop runs once, so it emits
 *    `cmp r3, r7 / bcc` over the top, and a pointer comparison is UNSIGNED, so
 *    the back edge is `bcs`. `do { *p = z; p--; } while ((int)p >= (int)list);`
 *    gives the missing entry-guard removal and the signed `bge` together, and
 *    the `(int)` on both sides is what produces the separate `mov r12, r7` copy
 *    of the base. A counter loop (`for (i = 13; i >= 0; i--) list[i] = 0;`)
 *    does NOT get there -- gcc keeps the counter, and the whole function's
 *    allocation moves with it: the global pointer loses r8 (124 lines/111
 *    differing against 126/43).
 *
 * 2. THE SWAP IS WRITTEN OUT TWICE AND CROSS-JUMPING MERGES IT. The ROM
 *    computes `lsl r3, r0, #2` in BOTH arms of the `b == 1` test and then runs
 *    one shared four-instruction ldr/ldr/str/str tail. That is not a scheduling
 *    artifact: it is jump.c cross-jumping a swap that the SOURCE spells twice,
 *    once per arm. Computing `j = a +/- 1` in the arms and doing the swap once
 *    after the join puts both `lsl`s in the join block and is 43 differing;
 *    duplicating the swap is 5.
 *
 * 3. THE SWAP TEMP IS THE LOOP COUNTER. The ROM holds it in r6 -- callee-saved,
 *    across no call -- and r6 is also the `i` of the three loops that follow.
 *    One variable, two disjoint live ranges, exactly the recorded lever; a
 *    separate `int t` gets r0 and costs 2.
 *
 * 4. NAME THE STORED ZERO BEFORE THE POINTER. With `*p = 0` gcc emits
 *    `add r3, sp, #0x34` before `mov r2, #0`; `z = 0;` as its own statement
 *    ahead of `p = &list[13];` swaps them. Last 2 differing to 0.
 *
 * base[0x219] is read afresh at every loop test because it is a u8 through a
 * pointer and the loops call out; nothing is needed to keep those reloads.
 */
extern unsigned char *iwram_3001f2c;
extern void _Func_8079664(int id);
extern void _AddPartyMember(int id);
extern int _Func_80796c4(unsigned char *p);

int Func_80a7f44(int a, int b)
{
    unsigned char *base;
    int list[14];
    int i;
    int *p;
    int z;

    base = iwram_3001f2c;
    if (base[0x219] <= 1)
        return 0;
    if (b == 1) {
        if (a == base[0x219] - 1)
            return 0;
    } else if (a == 0) {
        return 0;
    }
    z = 0;
    p = &list[13];
    do {
        *p = z;
        p--;
    } while ((int)p >= (int)list);
    for (i = 0; i < base[0x219]; i++)
        list[i] = *(unsigned short *)(base + 0x208 + i * 2);
    if (b == 1) {
        i = list[a];
        list[a] = list[a + 1];
        list[a + 1] = i;
    } else {
        i = list[a];
        list[a] = list[a - 1];
        list[a - 1] = i;
    }
    for (i = 0; i < base[0x219]; i++)
        _Func_8079664(*(unsigned short *)(base + 0x208 + i * 2));
    for (i = 0; i < base[0x219]; i++)
        _AddPartyMember(list[i]);
    base[0x219] = _Func_80796c4(base + 0x208);
    return 1;
}
