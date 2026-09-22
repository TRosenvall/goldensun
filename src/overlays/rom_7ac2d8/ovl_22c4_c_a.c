/* OvlFunc_924_200a318 -- whole-file conversion of
 * asm/overlays/rom_7ac2d8/ovl_22c4_c_a.s (1 function, 179 instructions,
 * 492 bytes).  Byte-identical: 187 encodings and 53 relocations.
 *
 * TWO FINDINGS.
 *
 * SWITCH vs IF-CHAIN IS VISIBLE IN THE BRANCH POLARITY.  `cmp #0xa / beq` is
 * a switch; `cmp #0xa / bne` is an if / else-if.  Writing the two-case
 * dispatch on v.b as a switch fixed all 25 branch-structure differences at
 * once (29 -> 4).  Read the polarity before guessing the construct.
 *
 * A SUBSET PIN IS ORDER-SENSITIVE WHERE A FULL PIN IS NOT.  The two
 * OvlFunc_common0_18 sites pin q1/q2/q3 only, leaving q0 to expand from the
 * CSE'd 0xd2 << 18.  Ascending q1,q2,q3 FAILS (4 differing); q2,q1,q3 is
 * exact.  This qualifies batch 273's "write every pinned fill uniformly
 * ascending" rule -- that holds when the pin set is q0-first-and-complete,
 * not when q0 is deliberately excluded.  PIN4 here (i.e. including q0)
 * destroys the CSE the ROM has and rematerialises 0xd2 << 18 twice.
 */
struct Pk {
    int a;
    int b;
    int x;
    int y;
    int z;
    void (*arg5)(void);
};

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __SetFlag(int f);
extern void __ClearFlag(int f);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_WaitScript(int slot);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __Func_8091220(int a, int b);
extern void __Func_8091e9c(int a);
extern void OvlFunc_common0_18(int a, int b, int c, int d);
extern int OvlFunc_924_2008758(struct Pk *p);
extern void OvlFunc_924_20088ec(struct Pk arg);
extern int OvlFunc_924_200a1cc(void);
extern void OvlFunc_924_200a030(int a);
extern void OvlFunc_924_2009db4(int a);
extern void OvlFunc_924_200a304(void);
extern unsigned char *iwram_3001ebc[];

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_924_200a318(void)
{
    struct Pk v;
    int d;

    __CutsceneStart();
    if (OvlFunc_924_2008758(&v)) {
        switch (v.b) {
        case 0xa:
            OvlFunc_924_20088ec(v);
            if ((v.z >> 20) == 0x26)
                __SetFlag(0xc6 << 2);
            else
                __ClearFlag(0xc6 << 2);
            break;
        case 0xb:
            d = *(int *)(__MapActor_GetActor(0xb) + 8) >> 20;
            OvlFunc_924_20088ec(v);
            if ((v.x >> 20) == 0x2f) {
                __SetFlag(0x319);
                __ClearFlag(0x31a);
                __ClearFlag(0x31b);
                OvlFunc_924_200a1cc();
                if (d == 0x36)
                    OvlFunc_924_200a030(0);
                else if (d == 0x30)
                    OvlFunc_924_200a030(1);
                OvlFunc_924_2009db4(2);
                __CutsceneWait(0x3c);
            } else if ((v.x >> 20) == 0x30) {
                __SetFlag(0x31a);
                __ClearFlag(0x31b);
                __ClearFlag(0x319);
                if (OvlFunc_924_200a1cc()) {
                    OvlFunc_924_200a030(2);
                    OvlFunc_924_200a304();
                    OvlFunc_924_2009db4(1);
                    __MapActor_WaitScript(9);
                    { PIN4; q2 = 0x312 << 16; q1 = 0; q3 = 0xdf;
                      OvlFunc_common0_18(0xd2 << 18, q1, q2, q3); }
                    { PIN4; q2 = 0x332 << 16; q1 = 0; q3 = 0xdf;
                      OvlFunc_common0_18(0xd2 << 18, q1, q2, q3); }
                    __MapActor_TravelTo(9, 0xd2 << 2, 0xba << 2);
                    __CutsceneWait(5);
                    __PlaySound(0xbd);
                    __MapActor_WaitMovement(9);
                    __CutsceneWait(0x28);
                    __SetFlag(0x877);
                    __Func_8091220(0x80 << 9, 0);
                    *(int *)(iwram_3001ebc[0] + 0x1c0) = 0x100;
                    __MapTransitionOut();
                    __WaitMapTransition();
                    __Func_8091e9c(0xf);
                } else {
                    OvlFunc_924_200a030(2);
                    OvlFunc_924_200a304();
                    OvlFunc_924_2009db4(1);
                    __CutsceneWait(0x3c);
                }
            } else if ((v.x >> 20) == 0x35) {
                __SetFlag(0x31b);
                __ClearFlag(0x319);
                __ClearFlag(0x31a);
                OvlFunc_924_200a1cc();
                OvlFunc_924_200a030(0);
                __CutsceneWait(0x3c);
            } else {
                __ClearFlag(0x319);
                __ClearFlag(0x31a);
                __ClearFlag(0x31b);
                OvlFunc_924_200a1cc();
                if (d == 0x2f)
                    OvlFunc_924_200a030(2);
                else if (d == 0x30)
                    OvlFunc_924_200a030(1);
                OvlFunc_924_2009db4(0);
                __CutsceneWait(0x3c);
            }
            break;
        }
    }
    __CutsceneEnd();
}
