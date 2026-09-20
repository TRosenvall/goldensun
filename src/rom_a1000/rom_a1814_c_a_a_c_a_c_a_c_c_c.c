/* Cluster Func_80a2324..Func_80a2324 extracted from goldensun/asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a_c_c.s.
 *
 * Total .text for this TU = 156 bytes (= 0x9c).
 * Preserves the original ROM layout when slotted between
 * asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a_c_c_b.o and asm/rom_a1000/rom_a1814_c_a_a_c_a_c_b.o in
 * goldensun/stage1.ld.
 *
 * Never attempted before batch 273. No pins, no flags. Batch 271 elevated its
 * immediate neighbour Func_80a22f4 out of the same parent, so the .c beside this one
 * supplied the area's idioms.
 *
 * FIVE LEVERS, each measured, and four of them are about WHERE A VALUE IS BORN rather
 * than what it says.
 *
 * 1. LOOP 1 IS AN ASCENDING `for`, NOT THE DESCENDING SHAPE IT LOOKS LIKE. The ROM's
 *    `sub r6,#1 / cmp r6,#0 / bge` with an ascending `ldmia r3!` cursor is
 *    check_dbra_loop REVERSING a plain `for (i = 0; i <= 0x1f; i++)` whose `i` is
 *    unused in the body. Transcribing the ROM's direction (as the sibling
 *    Func_80a3d24 is written) is 44 of 79. The control is in the same file stem:
 *    Func_80a3480 has `i % 5` in its body and is NOT reversed.
 * 2. THE LOOP BOUND MUST NOT BE A VARIABLE. `end = i + count; while (i < end)` puts
 *    `mov r10, r0` before the entry guard, 55 differing. Inlined as
 *    `while (i < count + first)`, loop.c hoists the invariant add to the preheader and
 *    cse-after-loop rewrites it reusing the guard's register -- the ROM's position.
 *    Operand order matters too: `first + count` gives `add r0, r6, r0` and
 *    `count + first` gives the ROM's `add r0, r6`.
 * 3. THE 0x48 MUST SIT IN THE INDEX, NOT THE BASE. Every parenthesisation of
 *    `state + 0x48 + i*4` reassociates to `(state + i*4) + 0x48`. A named
 *    `off = i*4 + 0x48` inside the loop holds the grouping, gives `ldr r5, [r1, r3]`,
 *    and leaves `i*4` for cse-after-loop to reuse in the giv initialiser. 44 to 29.
 * 4. THE y CURSOR IS A GIV, NOT A VARIABLE. `cy = y; ... cy += 0x10` loads the stack
 *    argument before the entry guard; `y + (i - first) * 0x10` makes strength
 *    reduction build it, so the `ldr r7, [sp, #0x20]` init lands in the preheader.
 *    29 to 3.
 * 5. THE LAST THREE WERE A SPURIOUS ALIAS DEPENDENCE. .23.sched2 showed the byte
 *    store to +5 with a successor reading the spill slot, which gave `mov r3, #1`
 *    priority 3 against `add r6, #1`'s 2, so `i++` scheduled last. Replacing the cast
 *    dereferences with a typed struct killed the store-to-spill-slot dependence and
 *    the tie then broke on LUID in `i++`'s favour. SOURCE REORDERING DID NOTHING --
 *    only the type did, which is the recorded alias/anti-dependence lever again.
 */
struct Node {
    unsigned char pad_00[5];
    unsigned char flags;
    unsigned short x;
    unsigned short y;
};

extern unsigned char *iwram_3001f2c;
extern void Func_80a17c4(struct Node *p);

void Func_80a2324(int count, int first, int unused, int x, int y)
{
    unsigned char *state;
    struct Node **slot;
    struct Node *p;
    int i;
    int off;

    state = iwram_3001f2c;
    slot = (struct Node **)(state + 0x48);
    for (i = 0; i <= 0x1f; i++) {
        p = slot[i];
        if (p != 0)
            p->flags = 0xd;
    }

    i = first;
    while (i < count + first) {
        off = i * 4 + 0x48;
        p = *(struct Node **)((int)state + off);
        if (p == 0)
            break;
        if (i > (int)state[0x218] - 1)
            break;
        p->x = x;
        p->y = y + (i - first) * 0x10;
        Func_80a17c4(p);
        p->flags = 1;
        i++;
    }
}
