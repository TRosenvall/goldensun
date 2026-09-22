/* Cluster OvlFunc_923_20092e0..OvlFunc_923_20092e0 extracted from
 * goldensun/asm/overlays/rom_7aa430/ovl_1150_c_c_c_a.s -- ONE function in its .s, no split.
 *
 * Uses _AREA_39, which ALREADY EXISTS in area.sym:74. The symbol is required rather than
 * cosmetic, and that was proved: writing the value as a literal, or through a named int, is FOUR
 * BYTES SHORT because gcc uses `mov` and drops the pool word. objcmp reports one phantom encoding
 * and one extra relocation because it cannot resolve .sym; make compare is the gate.
 *
 * 368 instructions. Never attempted before batch 280. TEN SCAFFOLDING ITEMS at greedy-drop fixpoint -- one fakematch row. The two free drops were
 * a pin on a flag setter and a named zero carrier. Declaration order is load-bearing at 2.
 *
 * ===== THE RESIDUE DOES NOT SCALE WITH INSTRUCTION COUNT -- IT SCALES WITH THE NUMBER OF
 *       REPEATED EXPENSIVE CONSTANTS =====
 *
 * The four functions landed together in batch 280 (338-392 instructions) came in at 132 / 134 / 95
 * / 107 differing on the FIRST candidate, and on three of them one mechanical pass -- pin every
 * call site whose arguments are all constants -- took it to 4 / 2 / 18. The defect class is single:
 * gcse/cse1 commons a `mov`+`lsl`-built or pooled constant into a callee-saved register and feeds
 * each site `mov rN, r5`, where the ROM rebuilds it every time. At this size there are simply MORE
 * INSTANCES OF THE SAME DEFECT, not a new kind of defect.
 *
 * What IS different is that the last 2-8 differing are CROSS-BLOCK and do not respond to local
 * devices. Three findings from that:
 *
 * 1. A BASIC-BLOCK BOUNDARY IS A GLOBAL SCHEDULING LEVER. One `do { } while (0);` between two
 *    adjacent MMIO stores fixed THREE transpositions, two of them about SIXTY INSTRUCTIONS EARLIER.
 *    6 differing to 0, with nothing else moved -- and pinning the far site directly made it WORSE
 *    (6 to 29). So when a transposition will not respond to anything local, look for a block
 *    boundary to insert somewhere else entirely.
 *
 * 2. `sub sp` CAN LOSE ON PRIORITY, where no tie-break device reaches it. On one of these the last
 *    defect was `sub sp, #0x44` emitted at index 14 against the ROM's 8, and `.23.sched2` with
 *    -fsched-verbose=6 gave it plainly: that insn has priority 41 against 42/42/42/43/44/44/45/46/46
 *    and is scheduled LAST in block 0, having lost every contest on the FIRST key. Priority is
 *    1 + the priority of its best dependent, and its only dependents are the calls. The fix was
 *    again a block boundary, and the POSITION was load-bearing: between the two stores works,
 *    before the pair is 8 and after it is 5.
 *
 * 3. CROSS-JUMPING IS DRIVEN BY REGISTER IDENTITY, AND A STACK ARGUMENT'S REGISTER IS REACHABLE.
 *    Two six-argument tails got tail-merged because both ended `str r5,[sp,#4] / bl / b exit`; the
 *    ROM's differ only in that one uses r2. A single `register int a6 __asm__("r2");` on the last
 *    argument both restored the register AND dissolved the merge.
 *
 * ===== OTHER LEVERS, EACH MEASURED =====
 *
 * LICM INSIDE A LOOP IS STOPPED BY A BLOCK-LOCAL INITIALISER. Where the ROM reloads a pool
 * constant every iteration, `REG = 0x100e + i;` hoists it and permutes the callee-saved roles,
 * while `{ int v = 0x100e; REG = v + i; }` INSIDE the loop body reproduces the ROM. The do/while
 * against `for` loop form is inert here, so `for` ships.
 *
 * A DESCENDING FILL ORDER CAN BE THE LEVER. Batch 279 found `q0` assigned FIRST was worth 80
 * differing to zero on one function; here one site wants the opposite -- `{ PIN2; q1 = 0; q0 = 2; }`
 * is 0 where ascending is 2. So the rule is not "ascending" but "match the ROM's fill order", and
 * both directions occur.
 *
 * AN HImode STORE CONSTANT NEEDS A NAMED `int` CARRIER to become a WORD pool load: a direct
 * `REG = 0x1010;` gives `ldrh r3, .L+4` (a HImode pool entry) where the ROM has `ldr r2, =0x1010`.
 *
 * A BITFIELD INSERTION IS NOT AN EXPRESSION STORE, confirmed again with a number: `q->f9 =
 * (q->f9 & ~0xc) | 4;` narrows the mask to `mov r3,#0xf3`, where the ROM has `mov r3,#0xd /
 * neg r3,r3` -- full SImode, and -13 == ~12. A 2-bit bitfield at bit offset 2 is the only spelling
 * that reproduces it.
 *
 * Loop counters must be `unsigned int` for the ROM's `bls` (an `int` is 4 differing), while
 * `i <= 0x1f` against `i < 0x20` is inert, as are two shift-versus-literal spellings.
 *
 * AND ONE NON-LEVER WORTH RECORDING: the unified wide-struct tag that a 334-instruction landed
 * precedent needs (`int f18; int f1c;` in the tag, narrow being 2 differing) is a TIE at 0 here
 * with the narrow tag plus casts. The struct form ships for readability, but the lever did not
 * transfer -- site-specific, not general.
 *
 * A 31-case jump table with three fallthroughs and a cross-jumped shared tail came out of ordinary
 * C written in the ROM's case order, with no lever at all.
 */
#include "gba/types.h"
#include "gba/io.h"

struct Actor {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
    unsigned char pad14[4];
    int f18;
    unsigned char pad1c[0x28];
    int f44;
    int f48;
    unsigned char pad4c[9];
    unsigned char f55;
};

struct Cfg {
    int f00;
    int f04;
    int f08;
    int f0c;
    int f10;
    int f14;
    unsigned short f18;
    unsigned short f1a;
    int f1c;
    int f20;
    void (*f24)(void);
};

typedef struct {
    unsigned char pad00[0x1c2];
    short f1c2;
    unsigned char pad1c4[0x240 - 0x1c4];
    short f240;
    short f242;
} GlobalState;

extern GlobalState gState;
extern int _AREA_39;
extern unsigned char *iwram_3001ebc;
extern unsigned char ewram_2001000[];

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Surprise(int slot, int a);
extern void __StartTask(void (*f)(void), int prio);
extern int __cos(int a);
extern int __sin(int a);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __Func_8019aa0(int a, int b, int c);
extern void __Func_8091200(int a, int b);
extern void __Func_8091220(int a, int b);
extern void __Func_8091254(int a);
extern void __Func_8091494(int a);
extern void __Func_8091e9c(int n);
extern void __Func_8091ff0(int a);
extern void __Func_8092950(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void OvlFunc_923_2008d98(void);
extern void OvlFunc_923_2008e3c(void);
extern void OvlFunc_923_2009208(void);
extern void OvlFunc_923_2009730(void);
extern void OvlFunc_923_200996c(void);
extern void OvlFunc_923_2009a3c(int a, unsigned char *p);
extern void OvlFunc_common0_10c(int x, int y, int z, int a, int b, int c,
                                int d, struct Cfg *s);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

int OvlFunc_923_20092e0(void)
{
    struct Actor *p;
    struct Cfg s;
    int v[3];
    unsigned int i;
    int ang;
    int w;
    int c;
    int t1, t2;
    int r;

    c = 0xfd << 6;
    REG_BLDCNT = c;
    do { } while (0);
    c = 0x1010;
    REG_BLDALPHA = c;
    OvlFunc_923_2009a3c(0x15, ewram_2001000);
    __SetFlag(0x111);
    gState.f242 = 0xb;
    gState.f240 = (int)(&_AREA_39);
    __Func_8091494(0);
    if (__GetFlag(0x875)) {
        __StartTask(OvlFunc_923_2008d98, 0xc8 << 4);
    } else {
        OvlFunc_923_2008e3c();
    }
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x81 << 2;
    switch (gState.f1c2) {
    case 1:
        if (__GetFlag(0x872) == 0) {
            __Func_8091e9c(0x14);
        }
        /* fall through */
    case 2:
        w = 0xffff0000;
        __MapActor_GetActor(0xc)->f18 = w;
        __MapActor_GetActor(0xd)->f18 = w;
        __MapActor_GetActor(0xe)->f18 = w;
        __WaitFrames(1);
        break;
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        if (__GetFlag(0x875) == 0) {
            break;
        }
        t1 = 0x14; t2 = 5;
        __Func_8010704(0x54, 5, 0xa, 7, t1, t2);
        t1 = 0x25; t2 = 5;
        __Func_8010704(0x65, 5, 0xc, 7, t1, t2);
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        __StartTask(OvlFunc_923_2008d98, 0xc8 << 4);
        if (__GetFlag(0x875)) {
            { register int a5 __asm__("r3"); register int a6 __asm__("r2");
              a5 = 5; a6 = 3; __CopyMapTiles(0x25, 0x62, 0xa, 0x61, a5, a6); }
            __Func_800fe9c();
            __WaitFrames(1);
            { register int a5 __asm__("r3"); register int a6 __asm__("r2");
              a5 = 6; a6 = 0x20; __Func_8010704(0x46, 0x20, 0xd, 7, a5, a6); }
        }
        if (gState.f1c2 != 6) {
            break;
        }
        { PIN1; q0 = 0x251; r = __GetFlag(q0); }
        if (r != 0) {
            break;
        }
        __SetFlag(0x251);
        __CutsceneStart();
        { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0; __Func_80933f8(q0, q1, q2, q3); }
        __Func_800fe9c();
        __WaitFrames(1);
        __MapActor_GetActor(0)->y = 0x82 << 16;
        __MapActor_GetActor(0)->f48 = 0x80 << 8;
        __MapActor_GetActor(0)->f44 = 0;
        __MapActor_GetActor(0)->f55 = 0;
        __MapTransitionIn();
        __WaitMapTransition();
        __CutsceneWait(0x1e);
        __MapActor_GetActor(0)->f55 = 3;
        __PlaySound(0xcc);
        __CutsceneWait(0x18);
        p = __MapActor_GetActor(0);
        s.f04 = 7;
        for (i = 0; i <= 0x10; i++) {
            ang = i << 12;
            v[0] = __cos(ang);
            v[1] = 0;
            v[2] = __sin(ang);
            v[0] = v[0] - v[0] / 4;
            v[2] = v[2] - v[2] / 2;
            OvlFunc_common0_10c(p->x, p->y, p->z, v[0], v[1], v[2], 0x10001, &s);
        }
        __PlaySound(0xbc);
        { PIN2; q0 = 0; q1 = 0x101; __MapActor_Surprise(q0, q1); }
        __MapActor_SetAnim(0, 0x16);
        { PIN3; q0 = 0xa0 << 11; q1 = 0xa0 << 11; q2 = 0x80 << 9; __Func_8012330(q0, q1, q2); }
        { PIN3; q0 = -1; q1 = -1; q2 = 0xe666; __Func_8012330(q0, q1, q2); }
        __Func_8012350();
        __MapActor_Surprise(0, 0x80 << 1);
        w = 0x80 << 9;
        __MapActor_GetActor(0)->f48 = w;
        __MapActor_GetActor(0)->f44 = 0x80 << 7;
        if (__GetFlag(0x875) == 0) {
            __Func_8091220(w, 0);
            __Func_8091200(0x10003, 1);
            __Func_8091254(0x1e);
            __WaitMapTransition();
            __MapActor_SetAnim(0, 1);
            __CutsceneWait(0x1e);
            __Func_8019aa0(0x1632, 0, 0);
            __Func_8091200(w, 0);
            __Func_8091254(0x1e);
        }
        __CutsceneEnd();
        break;
    case 18:
    case 19:
    case 20:
        OvlFunc_923_2009208();
        /* fall through */
    case 17:
        __Func_8091ff0(0xaa);
        break;
    case 25:
        __Func_8092950(0, 0xf);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
        __CutsceneStart();
        __Func_800fe9c();
        __WaitFrames(1);
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x80 << 1;
        __MapTransitionIn();
        __WaitMapTransition();
        __CutsceneWait(0x78);
        __Func_8091e9c(0x32);
        __CutsceneEnd();
        break;
    case 30:
        if (__GetFlag(0x109) == 0) {
            OvlFunc_923_2009730();
            break;
        }
        { register int a5 __asm__("r3"); register int a6 __asm__("r2");
          a5 = 7; a6 = 9; __Func_8010704(0, 0, 3, 3, a5, a6); }
        break;
    case 31:
        if (__GetFlag(0x109) == 0) {
            OvlFunc_923_200996c();
        }
        break;
    }
    return 0;
}
