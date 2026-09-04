// fakematch
/* OvlFunc_942_2008328  --  0x02008328
 *
 * Was the whole of goldensun/asm/overlays/rom_7c6bac/ovl_30_c_c_a_c_a_c.s, so
 * no split was needed.
 *
 * A SINGLE EXIT WAS WORTH 86 INSTRUCTIONS. The ROM reaches `bl __CutsceneEnd`
 * from four places by branch and skips it entirely on one early-out. Written
 * with `__CutsceneEnd()` duplicated at the end of the two outer arms -- which
 * is the natural C -- the function came out 152 lines against 150 with 86
 * differing and the whole label structure shifted. One call at the bottom, with
 * a bare `return` for the path that skips it, is exact:
 *
 *     if (flag) { ... } else { if (test) return; ... }
 *     __CutsceneEnd();
 *
 * That is the single-exit lever from src/non_matching/ovl_7ced6c/2008f70.c,
 * which measured it at 152 to 98 on five duplicated returns. This is the same
 * thing with a call instead of a return, and it is worth knowing that the tell
 * is a LENGTH overshoot plus a label-structure diff, not a scheduling residue.
 *
 * TWO LEVERS INHERITED FROM src/overlays/rom_7d768c/ovl_30_c_a_a_a_c_b.c, which
 * opens with the identical masked-field computation:
 *
 *   - `~0x3fff` rather than `0xffffc000`, because gcc narrows an AND mask when
 *     the result is truncated and emits `ldr =0xc000` for the literal form.
 *   - The message base is a named local and the ROM derives `m + 1` and `m + 2`
 *     from it with `add r0, r5, #1`. Here NO PIN IS NEEDED -- the id is a linker
 *     symbol, spelled `extern int _MSG_1d20;` with `(int)&_MSG_1d20`, and a
 *     symbol address is not something constant propagation can fold into two
 *     separate pool entries. The sibling needed `register int m __asm__("r5")`
 *     because its base was a plain integer. THE SPELLING OF THE ID DECIDES
 *     WHETHER THE PIN IS NEEDED.
 *
 * `0x8a7` and `0x8a9` are each tested and then set, and both are rebuilt by the
 * ROM, so both take r0 pins; without them the prologue gains a register and 145
 * of 156 instructions shift.
 *
 * The two item queries are ordinary nested calls -- `__CheckItem` takes
 * `__CheckPartyItem`'s result as its first argument -- and both results are
 * named because they are live across three calls before being passed on
 * together.
 */
extern unsigned char *iwram_3001ebc;
extern int _MSG_1d20;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern int __CheckPartyItem(int item);
extern int __CheckItem(int a, int item);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8078948(int a, int b);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_942_2008328(void)
{
    unsigned char *p;
    int d;
    int m;
    register int p0 __asm__("r0");
    int c1, c2;

    d = (short)((*(unsigned short *)(__MapActor_GetActor(0) + 6) + (0x80 << 6))
                & ~0x3fff);
    __CutsceneStart();
    p0 = 0x8a7;
    if (__GetFlag(p0) != 0) {
        p0 = 0x8a9;
        if (__GetFlag(p0) != 0) {
            __MessageID(0x1d23);
            __Func_8092c40(0xc, 0);
        } else {
            m = (int)&_MSG_1d20;
            __MessageID(m);
            { PIN2; q1 = 0; q0 = 0xc; __Func_8092c40(q0, q1); }
            if (__Func_8091c7c(0, 0) == 0) {
                __CutsceneWait(0xa);
                __MessageID(m + 1);
                __ActorMessage(0xc, 0);
                { PIN3; q2 = 0xa1; q0 = 0xc; q1 = 0x58; q2 <<= 3;
                  __Func_80921c4(q0, q1, q2); }
                { PIN3; q1 = 0x80; q0 = 0xc; q1 <<= 7; q2 = 0;
                  __Func_8092adc(q0, q1, q2); }
                __CutsceneWait(0x14);
                p0 = 0x8a9;
                __SetFlag(p0);
            } else {
                __MessageID(m + 2);
                __ActorMessage(0xc, 0);
            }
        }
    } else {
        if ((d << 16) != (0x80 << 24))
            return;
        __MessageID(0x1d16);
        __ActorMessage(0xc, 0);
        if (__GetFlag(0x8a5) != 0) {
            c1 = __CheckPartyItem(0xeb);
            c2 = __CheckItem(c1, 0xeb);
            __MapActor_DoAnim(0xc, 3);
            { PIN3; q2 = 0xa1; q0 = 0xc; q1 = 0x58; q2 <<= 3;
              __Func_80921c4(q0, q1, q2); }
            { PIN3; q1 = 0x80; q0 = 0xc; q1 <<= 7; q2 = 0;
              __Func_8092adc(q0, q1, q2); }
            *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
            __ActorMessage(0xc, 0);
            { PIN2; q1 = c2; q0 = c1; __Func_8078948(q0, q1); }
            p0 = 0x8a7;
            __SetFlag(p0);
            p = __MapActor_GetActor(0);
            { PIN3; q1 = *(short *)(p + 0xa); q2 = 0xa3; q0 = 0; q2 <<= 3;
              __Func_80921c4(q0, q1, q2); }
            { PIN3; q2 = 0xa3; q0 = 0; q1 = 0x48; q2 <<= 3;
              __Func_80921c4(q0, q1, q2); }
            { PIN3; q2 = 0xa3; q0 = 0xc; q1 = 0x58; q2 <<= 3;
              __Func_80921c4(q0, q1, q2); }
            __Func_8092adc(0xc, 0, 0);
        } else {
            __ActorMessage(0xc, 0);
        }
    }
    __CutsceneEnd();
}
