/* Cluster OvlFunc_950_2008500..OvlFunc_950_2008500 extracted from
 * goldensun/asm/overlays/rom_7d5838/ovl_30_c_c_a_c_a_a_c_a_c_a.s.
 *
 * Total .text for this TU = 168 bytes.
 *
 * Same three-line-exchange shape as OvlFunc_962_200806c: the message base is a
 * SYMBOL, `(int)(&_MSG_1fd5)`, which is what makes gcc spend the callee-saved
 * register. Matched on the first attempt with that and the `int` return type
 * on __Func_8092c40.
 *
 * The join the ROM shares between its two earlier message arms is produced by
 * gcc's own cross-jumping; no goto or single-variable routing is needed.
 */
extern int _MSG_1fd5;

struct A { unsigned char pad00[6]; unsigned short f6; };

extern struct A *__MapActor_GetActor(int slot);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int arg);
extern void __CutsceneWait(int n);
extern void __Func_80b0278(int a, int b);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_950_2008500(int slot)
{
    struct A *a;
    unsigned short d;
    int base;

    a = __MapActor_GetActor(0);
    d = (a->f6 + 0x2000) & ~0x3fff;
    if (d == 0x8000) {
        __Func_80b0278(0x1c, slot);
    } else if (__GetFlag(0x95 << 4)) {
        __MessageID(0x238d);
        __ActorMessage(slot, 0);
    } else if (__GetFlag(0x962)) {
        __MessageID(0x221b);
        __ActorMessage(slot, 0);
    } else {
        base = (int)(&_MSG_1fd5);
        __MessageID(base);
        __Func_8092c40(slot, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __CutsceneWait(0xa);
            __MessageID(base + 1);
        } else {
            __MessageID(base + 2);
        }
        __ActorMessage(slot, 0);
    }
}
