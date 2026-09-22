/* OvlFunc_881_20086ec -- whole-file conversion of
 * asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_a_a.s (1 function,
 * 210 instructions, 928 bytes).  Byte-identical: 312 encodings and 138
 * relocations against the reference with its two pooled area ids spelled as
 * symbols.
 *
 * SYMBOL TELL, ALREADY PROVISIONED.  The ROM pools 0x3a and 0xbb as
 * __SetDestMap destinations where `movs r0,#0x3a` would fit -- area.sym's own
 * criterion verbatim.  _AREA_3a and _AREA_bb were already in area.sym (added
 * for __Func_8091f90); nothing new was needed here, and that prior entry is
 * what this function independently confirms.
 *
 * THE DOMINANT LEVER WAS POOL PLACEMENT, NOT ALLOCATION OR ORDER, and it was
 * reachable only through two named locals.  Plain C gave 940 bytes against
 * 928 with 230 of 316 differing -- and the WHOLE cascade was two extra pool
 * words.  `*(short *)(a + 6) = 0xc0 << 6;` folds 0x3000 into the pool (the
 * documented halfword-store lever, 16 bytes here) and
 * `*(unsigned char *)&gState[0xf9] = 2;` folds =gState+498 where the ROM
 * builds mov/lsl/add.  Those two words pushed gcc's minipool out of the
 * function tail and into the middle of the body; that moved the .La2c join
 * 0x144 bytes away from twelve `bne`s, each of which then had to become
 * beq/b.  TWELVE TWO-INSTRUCTION PAIRS, ALL REPORTED AS REAL DIFFERENCES,
 * ALL CAUSED BY TWO CONSTANTS.  Naming both values (`h = 0xc0 << 6;` and
 * `gs = (unsigned char *)gState; gs[0xf9 << 1] = 2;`) put the pool back at
 * the tail and took it to 3 differing in one step.
 *
 * THE DIAGNOSTIC IS GENERAL: a reference whose pool sits after .func_end and
 * a candidate whose pool sits mid-body is a LENGTH-OF-CONDITIONAL-BRANCH
 * symptom.  Read the pool address in the `ldr [pc, #N]` comment before you
 * read the branches; the branch differences are downstream and counting them
 * tells you nothing.
 *
 * Two idioms were copied, not re-derived: `extern short gState[]` with
 * `gState[0xe1]` (not unsigned char[] + byte offset) per
 * src/non_matching/ovl_786f0c/2008368.c, and `extern int __StartTask(...)`
 * non-void per src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_c_c.c.
 */
extern short gState[];
extern unsigned char iwram_3001ebc[];
extern int L679c __asm__(".L679c");
extern int _AREA_3a;
extern int _AREA_bb;

extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern int __GetFlag(int id);
extern int __GetFlagByte(int id);
extern void __SetDestMap(int map, int entrance);
extern void __CutsceneStart(void);
extern void __WaitFrames(int n);
extern void __Func_8078a08(int a);
extern void __Func_8091e9c(int n);
extern void __Func_80933d4(int a, int b);
extern void __Func_8010d48(int a, int b, int c, int d);
extern int __StartTask(void (*fn)(void), int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);

extern void OvlFunc_881_200b9fc(int a);
extern void OvlFunc_881_2008598(void);
extern void OvlFunc_881_200b678(void);
extern void OvlFunc_881_200a768(int a);
extern void OvlFunc_881_200a4a8(void);
extern void OvlFunc_881_2008a8c(void);
extern void OvlFunc_881_2008c28(void);
extern void OvlFunc_881_20097fc(void);
extern void OvlFunc_881_2009888(void);
extern void OvlFunc_881_2009938(void);
extern void OvlFunc_881_20099e8(void);
extern void OvlFunc_881_2009a98(void);
extern void OvlFunc_881_2009b5c(void);
extern void OvlFunc_881_200a274(void);
extern void OvlFunc_881_200b57c(void);
extern void OvlFunc_881_200b130(void);
extern void OvlFunc_881_200b2f0(void);
extern void OvlFunc_881_200acb4(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

int OvlFunc_881_20086ec(void)
{
    unsigned char *b;
    unsigned char *a;
    unsigned char *gs;
    int h;
    int v;

    if (gState[0xe1] == 0x63) {
        __SetFlag(0xb0 << 1);
        __SetFlag(0x161);
        __SetFlag(0x163);
    }
    v = gState[0xe1];
    if (v == 0x5a) {
        OvlFunc_881_200b9fc(0);
        __SetDestMap((int)&_AREA_3a, 1);
    } else if (v == 0x5b) {
        OvlFunc_881_200b9fc(1);
        __SetDestMap((int)&_AREA_bb, 0x5d);
    } else if (v == 0x4e) {
        __CutsceneStart();
        __Func_8078a08(0xf2);
        __Func_8091e9c(0x70);
    } else {
        __SetFlag(0xa2 << 1);
        b = *(unsigned char **)iwram_3001ebc;
        *(int *)(b + 0x1c0) = 0x80 << 3;
        *(int *)(b + 0x1c8) = 0x10;
        __WaitFrames(1);
        __Func_80933d4(0x80 << 12, 0x80 << 9);
        __ClearFlag(0x12f);
        __StartTask(OvlFunc_881_2008598, 0xc8 << 4);
        if (__GetFlag(0x90a) == 0)
            { PIN1; q0 = 0x80;
              __Func_8010d48(q0, 0x80 << 1, 0xb0, 0x38); }
        switch (gState[0xe1]) {
        case 1:
            { PIN1; q0 = 0x815;
              if (__GetFlag(q0) == 0) {
                  __SetFlag(0x815);
                  __SetFlag(0x85c);
              } }
            break;
        case 0x21:
            if (__GetFlag(0x109)) {
                if (__GetFlag(0x85d) == 0 && __GetFlag(0x8d << 2)) {
                    L679c = 0x37;
                    { PIN2; q0 = 0x37; q1 = 0x17940000;
                      __MapActor_SetPos(q0, q1, 0xd480000); }
                    a = __MapActor_GetActor(L679c);
                    h = 0xc0 << 6;
                    *(short *)(a + 6) = h;
                    OvlFunc_881_200a768(L679c);
                }
            } else if (__GetFlag(0x85d) == 0 && __GetFlag(0x9b8) == 0) {
                OvlFunc_881_200a4a8();
            }
            break;
        case 0x31:
            if (__GetFlag(0x94f) == 0 && __GetFlag(0x941))
                OvlFunc_881_2008a8c();
            break;
        case 0x40:
            if (__GetFlag(0x85a) == 0)
                OvlFunc_881_2008c28();
            break;
        case 0x41:
            OvlFunc_881_20097fc();
            break;
        case 0x42:
            OvlFunc_881_2009888();
            break;
        case 0x43:
            OvlFunc_881_2009938();
            break;
        case 0x44:
            OvlFunc_881_20099e8();
            break;
        case 0x45:
            OvlFunc_881_2009a98();
            break;
        case 0x46:
            OvlFunc_881_2009b5c();
            break;
        case 0x47:
            OvlFunc_881_200a274();
            break;
        case 0x48:
            OvlFunc_881_200b57c();
            break;
        case 0x49:
            OvlFunc_881_200b130();
            break;
        case 0x4a:
        case 0x4c:
        case 0x4d:
            __SetFlag(0x8e << 1);
            if (__GetFlagByte(0xbe << 2)) {
                gs = (unsigned char *)gState;
                gs[0xf9 << 1] = 2;
                __StartTask(OvlFunc_881_200b678, 0xc8 << 4);
            }
            break;
        case 0x4b:
            OvlFunc_881_200b2f0();
            break;
        case 0x50:
            OvlFunc_881_200acb4();
            break;
        default:
            a = __MapActor_GetActor(0x35);
            *(int *)(a + 0x18) = 0xa0 << 9;
            a = __MapActor_GetActor(0x35);
            *(int *)(a + 0x1c) = 0xa0 << 9;
            break;
        }
    }
    return 0;
}
