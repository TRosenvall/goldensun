/* OvlFunc_883_2008b28 -- MATCHES with --cflags "-fno-rerun-cse-after-loop".
 * ref: asm/overlays/rom_780898/ovl_30_c_c_a_c.s
 * tryc.py: OK (43 lines) with the flag.  Byte-verified: 128 bytes of .text
 * identical to the ROM's (scratch/agent3/bytecheck.sh).
 *
 * The TU therefore needs a CSE_CFLAGS rule in the Makefile.  On the default
 * flags it is 33 of 43 for one reason: 0x806 is read by __GetFlag and then
 * written by __SetFlag with the call between, so gcc parks it in r5 and adds a
 * push -- the exact shape docs/elevation.md assigns to this flag.
 *
 * The nested test is written with the SetFlag arm as the `if` body because the
 * ROM branches AWAY from it (`bne .Lb72`), per the fall-through rule.
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
