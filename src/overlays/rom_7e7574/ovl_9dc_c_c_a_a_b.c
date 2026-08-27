/* OvlFunc_959_200c638  --  0x0200c638
 *
 * Cut out of goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a.s.
 *
 * Speaks one of eight lines depending on a counter in overlay data. Seven of
 * them are just a message id; the eighth walks the actor across the room first.
 *
 * THE ODD ARM SITS AFTER THE SHARED TAIL, AND A `goto` IS WHAT PUTS IT THERE.
 * The ROM's layout is: the seven id-selecting blocks, then the shared
 * `__MessageID / __ActorMessage` tail, then case 5's long block. Written as an
 * ordinary `case 5:` with its body inline -- even written last -- gcc places the
 * block BEFORE the tail and needs a `b` to jump over it, one instruction too
 * many.
 *
 * `case 5: goto five;` with the body after the tail reproduces the ROM exactly.
 *
 * That spelling is recorded honestly rather than confidently: it is what
 * reproduces the bytes, and the ROM's block order does say the body was
 * out of line, but a `goto` is only one of the shapes that could have put it
 * there. If a cleaner one turns up it should be preferred.
 */
extern int L5fa4[] __asm__(".L5fa4");
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int arg);
extern void __Func_8092adc(int a, int b, int c);
extern void __CutsceneWait(int n);

void OvlFunc_959_200c638(void)
{
    int id;

    switch (L5fa4[0]) {
    case 0:
        id = 0x2414;
        break;
    case 1:
        id = 0x2415;
        break;
    case 2:
        id = 0x2416;
        break;
    case 3:
        id = 0x2417;
        break;
    case 4:
        id = 0x2418;
        break;
    case 6:
        id = 0x241a;
        break;
    case 7:
        id = 0x241b;
        break;
    case 5:
        goto five;
    default:
        return;
    }
    __MessageID(id);
    __ActorMessage(0x15, 0);
    return;
five:
    __Func_8092adc(0x15, 0xd0 << 8, 0);
    __CutsceneWait(0x32);
    __Func_8092adc(0x15, 0xb0 << 8, 0);
    __CutsceneWait(0x32);
    __Func_8092adc(0x15, 0xa0 << 7, 0);
    __CutsceneWait(0x32);
    __MessageID(0x2419);
    __ActorMessage(0x15, 0);
}
