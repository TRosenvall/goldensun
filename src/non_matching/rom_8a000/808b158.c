/* GetLocationName -- 0x0808b158, asm/rom_8a000/rom_8ace0_a_a_c_a_c_a.s
 * (two functions; tools/datacheck.py confirms no data section).
 *
 * BLOCKER CLASS: ONE CALLEE-SAVED REGISTER TOO MANY. 63 instructions against
 * the ROM's 61, and the two extra are a push/pop pair -- we save r8 AND r10
 * where the ROM saves only r8 and fits its fifth value in r12 (ip), which needs
 * no saving because nothing crosses a call after `bl GetMapArea`.
 *
 *     rom    mov r12, r0    (the map area)      push {r7}
 *     ours   mov r6,  r0                        push {r6, r7}
 *
 * Both hold five values: the table pointer, the two parameters, the map area
 * and the result. The ROM's allocation is r5/r7/r6/r12/r8; ours is
 * r5/r8/r7/r6/r10. REG_ALLOC_ORDER is {3,2,1,0,12,14,4,5,...}, so r12 comes
 * BEFORE r5 for a value that does not cross a call -- which the map area does
 * not, being born at the call's return. gcc still put it in r6.
 *
 * THE RESIDUE IS BRACKETED, which is the useful part of this park:
 *
 *     with the result variable (below)        63 instructions, 2 LONG
 *     with early `return`s and no variable    59 instructions, 2 SHORT
 *     the ROM                                 61
 *
 * So the variable is real -- dropping it removes exactly the extra high
 * register AND two more instructions -- and the gap is purely which register
 * the map area lands in. A middle form does not exist at the statement level;
 * what is needed is for gcc to choose ip.
 *
 * WHAT IS SETTLED, and it took the most work:
 *
 *   THE TABLE ENTRY IS A BITFIELD. The ROM tests the flag with a BYTE load of
 *   the high half (`ldrb r2, [r5, #3] / and r3, #0x80`) and reads the value two
 *   different ways -- masked against 0x7fff for the wildcard test, and
 *   sign-extended with `lsl #17 / asr #17` for the comparison. Written as a
 *   plain `u16 f2` with explicit masks that is 65 instructions; as
 *
 *       s16 id;  s16 sub : 15;  u16 flag : 1;  void *name;
 *
 *   with `e->flag`, `e->sub == -1` and `e->sub == b`, it is 63. gcc narrows the
 *   flag test to the byte load on its own, and compares the SIGNED bitfield
 *   against -1 by masking against 0x7fff WITHOUT sign-extending -- which is
 *   exactly the ROM's asymmetry between the two tests, and the thing that reads
 *   like two different fields until you see it.
 *
 *   THE GUARDS MUST BRANCH, NOT PRODUCE A VALUE. Written as
 *   `ok = (e->id == a);` followed by `if (ok)`, gcc materialises 0/1 with
 *   `mov r1,#0 / mov r1,#1 / cmp r1,#0`: 73 instructions. Direct `continue`s
 *   are 65.
 *
 *   `lsr r3, r2, #31 / add r3, r2, r3 / asr r3, #1` in the caller's idiom is a
 *   signed `/ 2`; here the equivalent appears as the plain field compares.
 *
 * MEASURED AND INERT, all 63: three declaration orders, and assigning the table
 * pointer after the call instead of before.
 *
 * NEXT: this is an ip-allocation question, not a spelling one. It belongs with
 * the global_alloc parks (Func_80a8578, Func_80cd52c, Func_80919d8) -- read
 * global.c's find_reg and see what excludes r12 there, rather than sweeping
 * more source forms here.
 */
#include "gba/types.h"

struct Loc {
    s16 id;
    s16 sub : 15;
    u16 flag : 1;
    void *name;
};

extern struct Loc L9ddd8[] __asm__(".L9ddd8");
extern int GetMapArea(void);

void *GetLocationName(int a, int b)
{
    struct Loc *e;
    int area;
    void *r;

    r = NULL;
    e = L9ddd8;
    area = GetMapArea();
    while (e->id != -1) {
        if (e->flag) {
            if (e->id != a) {
                e++;
                continue;
            }
        } else if (e->id != area) {
            e++;
            continue;
        }
        if (e->sub == -1 || e->sub == b) {
            r = e->name;
            break;
        }
        e++;
    }
    return r;
}
