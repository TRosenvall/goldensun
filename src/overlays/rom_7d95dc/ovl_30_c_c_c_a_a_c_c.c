/* OvlFunc_953_2009a4c  --  0x02009a4c
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * changes name in goldensun/overlays/rom_7d95dc/overlay.ld and nothing else
 * moves.
 *
 * One frame of wait, then the per-area entry hook: twelve of the sixty-six
 * areas in range do something on arrival, the rest fall straight out.
 *
 * A SIXTY-SIX SLOT TABLE WITH TWELVE LIVE TARGETS is still a table -- gcc keeps
 * it at 18% density here rather than building a comparison tree, so the twelve
 * cases can be written plainly and the fifty-four gaps left to the default.
 *
 * THE CASE ORDER IS NOT NUMERIC and is not guessable; it is read off the block
 * layout: 5, 69, 7, 70, 64, 65, 66, 12, 21, 67, 68, 31. Writing them in
 * numeric order would reorder the bodies and nothing else about the function
 * would change, so this is the one detail worth checking against the .s.
 *
 * Cases 5 and 69 open with the SAME two `__MapActor_SetAnim` calls and the ROM
 * does not merge them -- it emits both copies. Both are written out.
 *
 * The gState offset must be BUILT, not folded: `g = gState;` into a local and
 * then `g + (0xe1 << 1)`. Indexing the array directly gives
 * `ldr r3, =gState+450` and the ROM's `mov r2, #0xe1 / lsl r2, #1 / add r3, r2`
 * disappears (measured, 102 differing).
 */
extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __WaitFrames(int n);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __AddPartyMember(int n);
extern void __Func_807a664(void);
extern void OvlFunc_953_200960c(void);
extern void OvlFunc_953_2009298(void);
extern void OvlFunc_953_2009688(void);
extern void OvlFunc_953_2009cd4(void);
extern void OvlFunc_953_200a3e0(void);
extern void OvlFunc_953_200a5f0(void);
extern void OvlFunc_953_200ab1c(void);
extern void OvlFunc_953_200a4d8(void);
extern void OvlFunc_953_200a668(void);
extern void OvlFunc_953_200a820(void);
extern void OvlFunc_953_200a904(void);
extern void OvlFunc_953_200a964(void);

void OvlFunc_953_2009a4c(void)
{
    unsigned char *g;

    __WaitFrames(1);
    g = gState;
    switch (*(short *)(g + (0xe1 << 1))) {
    case 5:
        __MapActor_SetAnim(8, 2);
        __MapActor_SetAnim(9, 2);
        break;
    case 69:
        __MapActor_SetAnim(8, 2);
        __MapActor_SetAnim(9, 2);
        if (__GetFlag(0x109) == 0)
            OvlFunc_953_200960c();
        break;
    case 7:
        OvlFunc_953_2009298();
        break;
    case 70:
        OvlFunc_953_2009688();
        break;
    case 64:
        OvlFunc_953_2009cd4();
        __Func_807a664();
        break;
    case 65:
        OvlFunc_953_200a3e0();
        break;
    case 66:
        OvlFunc_953_200a5f0();
        break;
    case 12:
        __SetFlag(0xa2 << 1);
        OvlFunc_953_200ab1c();
        if (__GetFlag(0x109) == 0)
            OvlFunc_953_200a4d8();
        break;
    case 21:
        __AddPartyMember(1);
        __AddPartyMember(2);
        __AddPartyMember(3);
        __SetFlag(0x90e);
        OvlFunc_953_200a668();
        break;
    case 67:
        OvlFunc_953_200a820();
        break;
    case 68:
        OvlFunc_953_200a904();
        break;
    case 31:
        __AddPartyMember(1);
        __AddPartyMember(2);
        __AddPartyMember(3);
        __SetFlag(0x90f);
        OvlFunc_953_200a964();
        break;
    }
}
