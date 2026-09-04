/* OvlFunc_945_200812c -- 0x0200812c
 *
 * A thirteen-way state machine on the actor's step counter at +0x66: set a
 * facing word, walk somewhere, wait for the walk to finish, and in a few of
 * the states poke the party leader's flag byte at +0x63. Most states finish by
 * advancing the counter, which is why the tail is shared and cross-jumped.
 *
 * THIS TRANSLATION UNIT NEEDS -fno-gcse; see the Makefile rule beside it. At
 * plain -O2 the global pass HOISTS the counter's `ldrh` into the entry block,
 * above the switch, and then every state after it shifts by one instruction --
 * 147 of 157 lines differ for that single hoisted load. The ROM re-loads the
 * counter in each arm. Nothing in the source reaches it: separate tails per
 * case, an early exit in the +0x5b test, recomputing the pointer per arm and a
 * `"memory"` clobber before the switch were all tried and all left the hoist
 * standing, which is what identifies this as a FLAG choice in the original
 * build rather than a spelling. -O1 also removes the hoist but costs more than
 * it saves: it reorders the entry block and the jump-table setup, and settles
 * eight lines worse than -fno-gcse does.
 *
 * The rest is the crossed mov/shift class. __Actor_TravelTo's three constants
 * are materialised `mov r1 / mov r2 / mov r3` but SHIFTED `lsl r3 / lsl r1 /
 * lsl r2`, with the `mov r0, r5` wedged between the first shift and the
 * second. Pinning alone does not hold that apart -- gcc pairs each mov with its
 * own shift -- so each of the first two movs carries a barrier. Same lever for
 * `p2++` before the +0x63 store, which otherwise folds into `strb r3, [r2, #1]`,
 * and for the zero in the +0x62 stores, which otherwise sinks past the pointer.
 *
 * The one place the ROM POOLS a zero (`ldr r2, =0x0`) where every direct
 * spelling gives `mov r2, #0` is the shared tail. Twelve spellings were
 * measured -- literal, cast, `char`/`unsigned char`/`short` locals, a
 * cross-block local, a static const, a volatile store -- and only ONE reaches
 * it: a `short` local PINNED to r2. The pin is what does it, not the width; the
 * same `short` unpinned still movs. Read that with the narrow-store table in
 * docs/elevation.md, which had this site's shape recorded as out of reach.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern int OvlFunc_945_20080fc(unsigned char *a);
extern void OvlFunc_945_20080d8(unsigned char *a);

void OvlFunc_945_200812c(unsigned char *arg)
{
    unsigned char *a;
    unsigned char *b;
    short *c;
    register unsigned char *y0 __asm__("r0");
    register int y1 __asm__("r1");
    register unsigned char *p2 __asm__("r2");
    register int y2 __asm__("r2");
    register short hz __asm__("r2");
    register int y3 __asm__("r3");
    register unsigned char *p3 __asm__("r3");

    a = arg;
    b = __MapActor_GetActor(8);
    c = (short *)(a + 0x66);
    switch (*c) {
    case 0:
        y3 = 0xb0 << 8;
        *(short *)(a + 6) = y3;
        goto bump;
    case 2:
        y3 = 0;
        *(short *)(a + 6) = y3;
        goto bump;
    case 4:
        __Actor_SetAnim(a, 2);
        y1 = 0xea;
        __asm__ volatile ("" : : "r" (y1));
        y2 = 0x80;
        __asm__ volatile ("" : : "r" (y2));
        y3 = 0x9e;
        y3 <<= 18;
        y0 = a;
        y1 <<= 17;
        y2 <<= 14;
        __Actor_TravelTo(y0, y1, y2, y3);
        *(int *)(a + 0x4c) = 0x3c;
        (*c)++;
        break;
    case 5:
        if (OvlFunc_945_20080fc(a) == 0)
            break;
        __Actor_SetAnim(a, 1);
        p2 = a;
        __asm__ volatile ("" : : "r" (p2));
        y3 = 0;
        __asm__ volatile ("" : : "r" (y3));
        p2 += 0x62;
        *p2 = y3;
        if (b[0x5b] == 0) {
            p2++;
            __asm__ volatile ("" : : "r" (p2));
            y3 = 1;
            *p2 = y3;
        }
        (*c)++;
        break;
    case 7:
        if (b[0x5b] == 0) {
            __Actor_SetAnim(a, 3);
            p2 = a + 0x63;
            y3 = 2;
            *p2 = y3;
        }
    bump:
        (*c)++;
        hz = 0;
        __asm__ volatile ("" : : "r" (hz));
        p3 = a + 0x62;
        *p3 = hz;
        break;
    case 9:
        __Actor_SetAnim(a, 2);
        y1 = 0xf0;
        __asm__ volatile ("" : : "r" (y1));
        y2 = 0x80;
        __asm__ volatile ("" : : "r" (y2));
        y3 = 0x96;
        y3 <<= 18;
        y0 = a;
        y1 <<= 17;
        y2 <<= 14;
        __Actor_TravelTo(y0, y1, y2, y3);
        *(int *)(a + 0x4c) = 0x3c;
        (*c)++;
        if (b[0x5b] != 0)
            break;
        p2 = a + 0x63;
        y3 = 3;
        *p2 = y3;
        break;
    case 10:
        if (OvlFunc_945_20080fc(a) == 0)
            break;
        __Actor_SetAnim(a, 1);
        p2 = a;
        __asm__ volatile ("" : : "r" (p2));
        y3 = 0;
        __asm__ volatile ("" : : "r" (y3));
        p2 += 0x62;
        *p2 = y3;
        (*c)++;
        break;
    case 1:
    case 3:
    case 6:
    case 8:
    case 11:
        OvlFunc_945_20080d8(a);
        break;
    case 12:
        y3 = 0;
        *c = y3;
        break;
    }
}
