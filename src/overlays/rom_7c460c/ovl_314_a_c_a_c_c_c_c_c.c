/* Cluster OvlFunc_939_20088ec..OvlFunc_939_20088ec extracted from goldensun/asm/overlays/rom_7c460c/ovl_314_a_c_a_c_c_c_c_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * A three-way talk on a gState halfword and two save flags.
 *
 * THREE LEVERS, all previously established, and worth listing because this is
 * a good example of them stacking in one 50-instruction function:
 *
 *   * `e = 0x101;` at the top is the BASIC-BLOCK LEVER. The __MapActor_Emote
 *     call is two branches deep, so gcc rematerialises the pooled constant
 *     there and it lands after `mov r2, #0x3c` instead of before it.
 *   * the message base is a SYMBOL, `_MSG_24db`, because the ROM reaches the
 *     second line with `add r5, #1` -- gcc only emits that for a symbol
 *     address, and folds `0x24db + 1` into a second pool entry otherwise.
 *   * the gState offset is a destructive `add` on a walked pointer (`g +=`),
 *     not an index, which is what the ROM's `add r3, r2` needs.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _MSG_24db;

extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MapActor_Emote(int a, int b, int c);
extern void __ActorMessage(int a, int b);
extern void __Func_809259c(int a, int b);

void OvlFunc_939_20088ec(void)
{
    unsigned char *g;
    int base;
    int e;

    e = 0x101;
    g = (unsigned char *)&gState;
    g += 0x93 << 2;
    if (*(short *)g != 0) {
        __MessageID(0x2411);
    } else if (__GetFlag(0x941) && !__GetFlag(0x94d)) {
        __MapActor_Emote(8, e, 0x3c);
        base = (int)(&_MSG_24db);
        __MessageID(base);
        __ActorMessage(8, 0);
        __Func_809259c(8, 1);
        __MessageID(base + 1);
        __SetFlag(0x9af);
    } else {
        __MessageID(0x1bb5);
    }
    __ActorMessage(8, 0);
}
