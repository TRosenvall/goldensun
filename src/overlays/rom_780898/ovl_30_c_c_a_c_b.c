/* OvlFunc_883_2008b28
 *
 * Cut out of goldensun/asm//overlays/rom_780898/ovl_30_c_c_a_c_b.s.
 *
 * A three-way conversation on two flags.
 *
 * BUILT WITH CSE_CFLAGS. 0x806 is read by __GetFlag and written by __SetFlag
 * with the call between, so gcc parks it in a callee-saved register and adds a
 * push -- 33 differing of 43 on the default flags, exact with the flag.
 *
 * BUILT WITH CSE_CFLAGS.
 *
 * Screened by a parallel agent; re-verified here before wiring.
 */
extern void __CutsceneStart(void);
extern int __GetFlag(int);
extern void __SetFlag(int);
extern void __MessageID(int);
extern void __ActorMessage(int, int);
extern void __Func_8092848(int, int, int);
extern void __Func_8093054(int, int);
extern void __CutsceneEnd(void);

void OvlFunc_883_2008b28(void) {
    __CutsceneStart();
    if (__GetFlag(0x815)) {
        __MessageID(0x11c9);
        __ActorMessage(0xe, 0);
    } else if (__GetFlag(0x806) == 0) {
        __SetFlag(0x806);
        __MessageID(0xf7c);
        __Func_8092848(0xe, 0, 4);
        __Func_8093054(0xe, 0);
    } else {
        __MessageID(0xf7e);
        __Func_8092848(0xe, 0, 4);
        __ActorMessage(0xe, 0);
    }
    __CutsceneEnd();
}
