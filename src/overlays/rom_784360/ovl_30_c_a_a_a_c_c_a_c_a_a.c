/* Cluster OvlFunc_884_2008248..OvlFunc_884_2008248 extracted from
 * goldensun/asm/overlays/rom_784360/ovl_30_c_a_a_a_c_c_a_c_a_a.s.
 *
 * Total .text for this TU = 364 bytes (= 0x16c). Never attempted before batch 278.
 * NO PINS, no flags, no volatile -- requires _MSG_1197, and see message.sym for why that is
 * the honest form rather than the pin that reaches the same bytes.
 *
 * Landed on candidate 1 at 7 differing, because the same-stem neighbour
 * ovl_30_c_a_a_a_c_c_a_c_a_c.c is a near-twin: the
 * `for (i = 0; i <= 0x27; i++) { OvlFunc_884_200a2f8(__MapActor_GetActor(slot));
 * __WaitFrames(1); }` loop transferred verbatim, as did the
 * `base = *(char **)iwram_3001ebc; (*(unsigned short *)(base + (0xec << 1)))++;`
 * save-counter idiom and the extern set. The sibling _c_a_b.c supplied the
 * "__Func_8092c40 deliberately undeclared" lever.
 *
 * AND THE TWIN WAS WRONG ABOUT ONE THING, which is the recorded trap firing exactly as
 * described: its `__StartTask` PIN is right for a function with TWO calls sharing `0xc8 << 4`,
 * and wrong here. A single call wants the callee UNDECLARED instead. Transfer the idiom, then
 * re-derive anything the listing pins down.
 *
 * THE MESSAGE BASE IS A SYMBOL, AND THE EVIDENCE IS THE FUNCTION'S LENGTH. With a plain
 * `int m = 0x1197` the function comes out a different size -- every relocation offset after the
 * first __MessageID shifts by two. With `(int)&_MSG_1197` it is 140 encodings against 140 with
 * every relocation identical and only the pool word left, which the .sym assignment fills. A
 * length difference is a stronger tell than a register difference, and worth looking for before
 * reading a count.
 *
 * One thing objcmp says that is not a defect: `_umodsi3_RAM / __umodsi3 is ONE symbol (same
 * address in the linked ELF)` -- overlays/rom_784360/overlay.ld:83 already carries that alias.
 */
extern unsigned char iwram_3001ebc[];
extern unsigned char iwram_3001e70[];
extern int _MSG_1197;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int n);
extern void __PlaySound(int id);
extern int __Func_8091c7c(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_884_200a2f8(unsigned char *p);
extern void OvlFunc_884_200a564(void);
extern void OvlFunc_884_200a574(void);
extern void OvlFunc_884_200a590(void);
/* __StartTask intentionally implicit */
extern void __StopTask(void (*f)(void));
extern void __Func_8092848(int a, int b, int c);
/* implicit: __Func_8092c40, __Func_8093054, __Func_809259c, __Func_8092950 */

void OvlFunc_884_2008248(void)
{
    char *base;
    int *p;
    int m;
    unsigned int i;
    int n;

    __CutsceneStart();
    if (__GetFlag(0x815)) {
        m = (int)&_MSG_1197;
        __MessageID(m);
        if (__GetFlag(2)) {
            base = *(char **)iwram_3001ebc;
            (*(unsigned short *)(base + (0xec << 1)))++;
        }
        if (__GetFlag(3)) {
            base = *(char **)iwram_3001ebc;
            (*(unsigned short *)(base + (0xec << 1)))++;
        }
        __Func_8092c40(0x11, 0);
        if (__Func_8091c7c(0, 0) == 0)
            __MessageID(m + 3);
        else
            __MessageID(m + 4);
        __ActorMessage(0x11, 0);
    } else {
        p = *(int **)*(char **)iwram_3001e70;
        __MessageID(0xf48);
        __Func_8092848(0x11, 0, 0);
        __Func_8093054(0x11, 0);
        __CutsceneWait(0x14);
        __Func_809259c(0x11, 2);
        __CutsceneWait(0xf);
        OvlFunc_884_200a564();
        n = 0;
        for (i = 0; i <= 0x27; i++) {
            OvlFunc_884_200a2f8(__MapActor_GetActor(0x11));
            __WaitFrames(1);
        }
        __StartTask(OvlFunc_884_200a590, 0xc8 << 4);
        __PlaySound(0x6b);
        for (i = 0; i != 0xb4; i++) {
            if (i % 10 == 0) {
                if (n & 1)
                    *p -= 0x80 << 9;
                else
                    *p += 0x80 << 9;
                n++;
            }
            __CutsceneWait(1);
        }
        __PlaySound(0x121);
        __StopTask(OvlFunc_884_200a590);
        __WaitFrames(1);
        OvlFunc_884_200a574();
        __Func_8092950(0x11, 0);
        __CutsceneWait(0x28);
        __MessageID(0xf4b);
        __ActorMessage(0x11, 0);
    }
    __CutsceneEnd();
}
