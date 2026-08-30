/* Cluster OvlFunc_962_200806c..OvlFunc_962_200806c -- the whole of
 * goldensun/asm/overlays/rom_7ec19c/ovl_30_c_a_c_a_a.s, confirmed data-free by
 * split_s.py, so no split was needed.
 *
 * Total .text for this TU = 148 bytes.
 *
 * THE MESSAGE BASE IS A SYMBOL. The ROM holds it in a callee-saved register
 * and reaches the other two lines with `add r0, r5, #1` and `#2`. A plain
 * `int base = 0x261c` is constant-propagated wherever it is assigned and gcc
 * never spends the register; `(int)(&_MSG_261c)` is what makes it derive.
 * That is the `push {r5, r6, lr}` versus `{r5, lr}` difference.
 *
 * The second lever is the RETURN TYPE of __Func_8092c40: declared `void` it is
 * 3 differing, declared `int` it is 1, and the symbol closes the last line.
 */
extern int _MSG_261c;

struct A { unsigned char pad00[6]; unsigned short f6; };

extern struct A *__MapActor_GetActor(int slot);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int arg);
extern void __CutsceneWait(int n);
extern void __Func_80b0278(int a, int b);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_962_200806c(int slot)
{
    struct A *a;
    unsigned short d;
    int base;

    a = __MapActor_GetActor(0);
    d = (a->f6 + 0x2000) & ~0x3fff;
    if (d == 0xc000) {
        __Func_80b0278(0x1f, slot);
    } else if (__GetFlag(0x96f)) {
        base = (int)(&_MSG_261c);
        __MessageID(base);
        __Func_8092c40(slot, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __CutsceneWait(0xa);
            __MessageID(base + 1);
        } else {
            __MessageID(base + 2);
        }
        __ActorMessage(slot, 0);
    } else {
        __MessageID(0x25cf);
        __ActorMessage(slot, 0);
    }
}
