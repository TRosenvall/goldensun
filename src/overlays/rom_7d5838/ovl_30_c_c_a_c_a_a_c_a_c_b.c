/* Cluster OvlFunc_950_20085a8..OvlFunc_950_20085a8 -- the whole of
 * goldensun/asm/overlays/rom_7d5838/ovl_30_c_c_a_c_a_a_c_a_c_b.s, confirmed
 * data-free by split_s.py, so no split was needed.
 *
 * Total .text for this TU = 196 bytes (0xc4).
 *
 * TWO LEVERS, and the second is the more interesting one.
 *
 * The message base is a SYMBOL, `(int)(&_MSG_2389)`, as on its two siblings.
 * With that and the `int` return type on __Func_8092c40, every INSTRUCTION
 * matched -- but .text came out 0xc0 against the ROM's 0xc4.
 *
 * THE FACING TEST'S WIDTH DECIDES THE POOL PLACEMENT. Written with a raw
 * pointer the test is SImode, so gcc's first pool fix has a 1020-byte range
 * and the whole pool lands at the end of the function. Written through the
 * struct with an `unsigned short` local the mask becomes a HImode pool entry,
 * whose range is 64 bytes, which forces the early pool and reproduces the
 * ROM's mid-function `.pool_aligned` and its two alignment pads exactly.
 *
 * That is worth remembering generally: a four-byte .text difference with every
 * instruction identical is a pool-placement difference, and the operand WIDTH
 * of a nearby expression is one of the things that moves it.
 *
 * The previous park for this function claimed a symbol base "behaves
 * identically, so this is not a symbol question". That was tested against a
 * probe rather than against this function with the facing test corrected.
 */
extern int _MSG_2389;

struct A { unsigned char pad00[6]; unsigned short f6; };

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_80b0278(int a, int b);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int a, int b);
extern void __MapActor_Emote(int a, int b, int c);

void OvlFunc_950_20085a8(int slot)
{
    struct A *a;
    unsigned short d;
    int base;

    a = __MapActor_GetActor(0);
    d = (a->f6 + 0x2000) & ~0x3fff;
    if (d == 0xc000) {
        __Func_80b0278(0x1a, slot);
        return;
    }
    if (__GetFlag(0x95 << 4)) {
        base = (int)(&_MSG_2389);
        __MessageID(base);
        __Func_8092c40(slot, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __CutsceneWait(0xa);
            __MessageID(base + 1);
        } else {
            __MessageID(base + 2);
        }
        __ActorMessage(slot, 0);
        return;
    }
    if (__GetFlag(0x962)) {
        __MessageID(0x2219);
        __ActorMessage(slot, 0);
        return;
    }
    __MessageID(0x1fd2);
    __ActorMessage(slot, 0);
    __MapActor_Emote(slot, 0x83 << 1, 0);
    __CutsceneWait(0x28);
    __ActorMessage(slot, 0);
}
