/* OvlFunc_common1_5e4 (common1 + 0x5e4) -- NON-MATCHING, 232 encodings of 263,
 * size 624 against the ROM's 620 (+4).
 *
 * Blocker class: not yet isolated.  This is an EARLY park and says so.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_common/common1_5e4.c \
 *     asm/overlays/common/common1_a_a_a_a_c_a.s --func OvlFunc_common1_5e4
 *
 * READ THE NUMBER CAREFULLY.  The agent reported "202 of 277"; objcmp against
 * the tree reports 232 of 263.  The reference instruction count itself
 * disagrees (263 here against the 277 reported), which means the agent was not
 * measuring against this reference as the tree now holds it -- so the 202 is
 * not comparable and should not be quoted.  The 232 is.  Same lesson as the
 * sibling park src/non_matching/ovl_7f6e64/200bbc8.c from this batch: quote
 * objcmp, and quote the reference count alongside the difference count so a
 * mismatched baseline is visible immediately.
 *
 * WHAT IS KNOWN: size is +4, i.e. one word long, and the instruction counts are
 * 263 against 265 -- so unlike 200bbc8 this is NOT a pure pool displacement;
 * there are two genuinely extra instructions plus a large real residue.
 *
 * ONE CAUTION SPECIFIC TO common1.  This is a shared overlay TU and its stems
 * carry per-file flag history (see the COMMON2_CFLAGS note in the Makefile and
 * tryc.makefile_flags, which substitutes -fcall-used-r4 -> -fcall-saved-r4 and
 * drops -mthumb-interwork for common2).  Before reading any prologue difference
 * here as a source problem, confirm which flags this stem is actually built
 * with -- an epilogue or a pushed r4 that looks unreachable is the classic
 * symptom of reading a function against the wrong flag set.
 *
 * NEXT: re-derive from the disassembly rather than continuing from this
 * candidate.  With a 232/263 residue and a baseline disagreement, the candidate
 * is not a useful starting point; the structure should be re-read first.
 */
extern unsigned char gState[];
extern unsigned char *__MapActor_GetActor(int slot);
extern int __GetPartySize(void);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __WaitFrames(int n);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8079664(int m);
extern void __AddPartyMember(int m);
extern int __Func_80a7380(void);
extern void __Func_8019908(int a, int b);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_80921c4(int slot, int a, int b);
extern void __Func_809218c(int slot, int a, int b);
extern void __Func_8092848(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetExtra(int slot, int v);
extern void __Func_8092adc(int slot, int a, int b);
extern void __SetFlagByte(int id, int v);

void OvlFunc_common1_5e4(int mode, int slot, int fb)
{
    signed char buf[8];
    unsigned char *a;
    unsigned char *p;
    signed char *q;
    int n, sel, i, who, m, id;
    int x, y, t, u;

    a = __MapActor_GetActor(slot);
    x = *(short *)(a + 0xa);
    y = *(short *)(a + 0x12);
    if (mode == 3) {
        goto m207e;
    }
    n = __GetPartySize();
    if (n > 0) {
        p = gState;
        p += 0xfc << 1;
        q = buf;
        i = n;
        do {
            *q = *p;
            i--;
            p++;
            q++;
        } while (i != 0);
    }
    if (n <= 1) {
        id = 0x2083;
        goto say;
    }
    if (__GetFlag(fb + (0x80 << 2)) != 0) {
        id = 0x2084;
        goto say;
    }
    if (mode == 2) {
        sel = 0;
        __WaitFrames(6);
    } else {
        __MessageID(0x207d);
        __Func_8092c40(slot, 0);
        sel = __Func_8091c7c(0, 0);
    }
    if (sel != 0) {
        goto m207e;
    }
    if (sel < n) {
        q = buf;
        i = n;
        do {
            __Func_8079664(*q);
            i--;
            q++;
        } while (i != 0);
    }
    if (n > 0) {
        q = buf;
        i = n;
        do {
            m = *q;
            q++;
            if (m != 0) {
                __AddPartyMember(m);
            }
            i--;
        } while (i != 0);
    }
    who = __Func_80a7380();
    if (n > 0) {
        q = buf;
        i = n;
        do {
            __Func_8079664(*q);
            i--;
            q++;
        } while (i != 0);
    }
    if (n > 0) {
        q = buf;
        i = n;
        do {
            __AddPartyMember(*q);
            i--;
            q++;
        } while (i != 0);
    }
    if (who == -1) {
        goto m207e;
    }
    __Func_8019908(who, 1);
    __MessageID(0x207f);
    __ActorMessage(slot, 0);
    __MapActor_SetSpeed(0, 0x80 << 9, 0x80 << 8);
    __MapActor_SetSpeed(who, 0x80 << 9, 0x80 << 8);
    __MapActor_SetSpeed(slot, 0x80 << 9, 0x80 << 8);
    a = __MapActor_GetActor(0);
    if (a != 0) {
        __MapActor_SetPos(who, *(int *)(a + 8), *(int *)(a + 0x10));
    }
    t = y + 0x10;
    u = x + 0x10;
    __Func_80921c4(who, x, t);
    __Func_80921c4(0, u, t);
    __Func_8092848(who, 0, 0x1e);
    __MapActor_SetAnim(who, 3);
    t = t - 0x20;
    __MapActor_DoAnim(0, 3);
    __Func_80921c4(slot, x, t);
    __Func_809218c(slot, u, t);
    __MapActor_SetExtra(0, who);
    __Func_80921c4(who, x, t);
    __MapActor_SetAnim(slot, 1);
    __Func_8092adc(slot, 0x80 << 8, 0);
    __Func_80921c4(who, x, y - 0x30);
    __Func_80921c4(slot, x, t);
    __Func_80921c4(slot, x, y);
    __Func_8079664(who);
    __SetFlag(fb + (0x80 << 2));
    a = __MapActor_GetActor(who);
    u = who << 4;
    __SetFlagByte(u + (0xdc << 2), *(int *)(a + 8) >> 20);
    __SetFlagByte(u + (0xde << 2), *(int *)(a + 0x10) >> 20);
    return;
m207e:
    id = 0x207e;
say:
    __MessageID(id);
    __ActorMessage(slot, 0);
}
