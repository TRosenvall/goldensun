/* Func_80a8578 -- 0x080a8578, asm/rom_a1000/rom_a7380_c_c.s (single-function
 * file, so it would convert WHOLE with no split).
 *
 * BLOCKER CLASS: ALLOCNO PRIORITY TIE between the parameter and a pointer.
 * SIZE EXACT -- 140 bytes, 60 instructions against 60 -- with 11 encodings
 * differing, all of them one register swap and its cascade.
 *
 * THE READING IS COMPLETE. Every instruction is present, in the ROM's order,
 * with the ROM's operands. The whole residue is that the second argument lives
 * in r7 and the `base + 0x21a` pointer in r6, and we get them the other way:
 *
 *     rom    mov r7, r1        ours   mov r6, r1
 *            add r6, r3, r2           add r7, r3, r2
 *
 * Allocation order hands out r5, r6, r7 in that sequence, so the ROM gives the
 * POINTER the earlier register even though the parameter is born first. The
 * pointer is three refs over a short range inside the `if`; the parameter is
 * six refs over the whole function. local-alloc's
 * floor_log2(n_refs) * n_refs * size / (death - birth) puts the short range
 * ahead, and ours comes out the other way by a margin no spelling has moved.
 *
 * MEASURED AND INERT, all 11:
 *   three declaration orders (pointer first, pointer last, pointer second)
 *   dropping the named pointer and writing base[0x21a] twice as a CSE
 *   naming the Func_8079008 result in its own local before the subtraction
 *   naming `b + 0xbe6` AFTER the allocation call
 *
 * MEASURED AND WORSE:
 *   naming `b + 0xbe6` BEFORE the allocation call   144 bytes, 62 instructions
 *   the parameter copied into a `register ... __asm__("r7")` local
 *                                                   124 bytes, 53 instructions
 *
 * PINNING THE POINTER TO r6 TAKES IT 11 -> 3, which confirms the diagnosis but
 * does not close the function, so this is NOT a one-pin fakematch away.
 *
 * THE REMAINING 3 UNDER THE PIN ARE A SEPARATE, SMALLER DEFECT and are recorded
 * here because they will still be there after the tie is solved:
 *
 *     rom    bl Func_8004938 / mov r5, r0 / ldr r0, =0xbe6 / mov r1, r5
 *                            / add r0, r7, r0
 *     ours   bl Func_8004938 / ldr r3, .L7+8 / mov r5, r0 / mov r1, r5
 *                            / add r0, r7, r3
 *
 * The ROM finishes the assignment statement, which frees r0, and then loads the
 * pool constant INTO r0. We hoist the pool load above the assignment and it
 * therefore has to use r3. Same shape as the batch-265 sched2 result on
 * Func_80270d8 -- a bare load with no dependence floating up into a gap -- and
 * the same lever (move the arithmetic, not the pointer) does not apply, because
 * there is no arithmetic to move.
 *
 * NEXT: this is the third function this session to end on a local-alloc
 * priority tie that no source spelling moves (with Func_942e0 and
 * Func_80cd52c). All three have .18.greg saying `;; 0 regs to allocate`. The
 * useful next step is not another spelling sweep -- it is reading
 * local-alloc.c's qty_compare_1 to find what else feeds the ordering besides
 * the published formula, since three independent functions now sit on it.
 */
#include "gba/types.h"

extern unsigned char *iwram_3001f2c;
extern u8 *_GetUnit(s32 id);
extern int _Func_8079008(int a, int b);
extern void _Func_8019908(int a, int b);
extern void *Func_8004938(unsigned int size);
extern void _Func_801965c(int a, void *buf, int n);
extern void _Func_8017aa4(void *buf, int b, int c, int d);
extern void free(void *p);

void Func_80a8578(int a, int b, int c)
{
    u8 *base;
    u8 *p;
    u8 *unit;
    void *buf;

    base = iwram_3001f2c;
    if (c == 0 && b > 3)
        b++;
    if (b == 1) {
        p = base + 0x21a;
        unit = _GetUnit(*p);
        if (unit[0xf] == 0x63) {
            b = 8;
        } else {
            _Func_8019908(_Func_8079008(*p, unit[0xf] + 1) - *(int *)(unit + 0x124), 5);
        }
    }
    buf = Func_8004938(0x100);
    _Func_801965c(b + 0xbe6, buf, 0x80);
    _Func_8017aa4(buf, a, 0, -1);
    free(buf);
}
