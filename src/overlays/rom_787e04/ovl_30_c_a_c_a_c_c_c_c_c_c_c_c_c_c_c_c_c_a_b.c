/* Cluster OvlFunc_887_200933c..OvlFunc_887_200933c extracted from goldensun/asm/overlays/rom_787e04/ovl_30_c_a_c_a_c_c_c_c_c_c_c_c_c_c_c_c_c_a.s.
 *
 * Slotted between ..._a_a.o and the rest of the overlay.
 *
 * A two-way talk. The second arm reaches its follow-up line with
 * `add r5, #1`, so the base is the symbol `_MSG_1c79`. __Func_80925cc is left
 * undeclared (r1 first); __Func_8093040 and __ActorMessage are declared.
 */
extern int _MSG_1c79;
extern unsigned char gScript_887__02009e6c[];
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int id);
extern void __Func_8092a1c(int slot, int a, int b);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_801776c(int id, int b);

void OvlFunc_887_200933c(void)
{
    int base;

    __CutsceneStart();
    if (__GetFlag(0x203)) {
        __Func_8092a1c(8, 0x80 << 9, (int)gScript_887__02009e6c);
        __CutsceneWait(0x14);
        __MessageID(0x1c77);
        __ActorMessage(8, 0);
    } else {
        __Func_80925cc(8, 2);
        __CutsceneWait(0x28);
        base = (int)(&_MSG_1c79);
        __MessageID(base);
        __Func_8093040(8, 0, 0x28);
        __Func_801776c(base + 1, 1);
    }
    __CutsceneEnd();
}
