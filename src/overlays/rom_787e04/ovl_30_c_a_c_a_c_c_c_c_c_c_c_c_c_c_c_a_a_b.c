/* Cluster OvlFunc_887_2008a0c..OvlFunc_887_2008a0c extracted from
 * goldensun/asm/overlays/rom_787e04/ovl_30_c_a_c_a_c_c_c_c_c_c_c_c_c_c_c_a_a.s (4 functions; the
 * target was the third, so this needed a three-way split).
 *
 * This had the highest call-family score in the corpus -- 35 of its 35 callees already elevated.
 *
 * 392 instructions. Never attempted before batch 280. THIRTEEN PINS at fixpoint, down from 30, plus a bitfield, two named blend values, a
 * `do { } while (0);` and two block-local loop constants (all load-bearing, 2 to 11 each) --
 * one fakematch row.
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

struct B {
    unsigned char pad00[9];
    unsigned char f9_0 : 2;
    unsigned char f9_2 : 2;
    unsigned char f9_4 : 4;
};

struct Actor {
    unsigned char pad00[0x23];
    unsigned char f23;
    unsigned char pad24[0x2c];
    struct B *f50;
    unsigned char f54;
    unsigned char f55;
};

extern void *iwram_3001ebc[];
extern unsigned char gScript_887__02009b78[];
extern unsigned char gScript_887__02009bb4[];
extern unsigned char gScript_887__02009c04[];
extern unsigned char gScript_887__02009c54[];
extern unsigned char gScript_887__02009ca4[];
extern unsigned char gScript_887__02009cec[];
extern unsigned char gScript_887__02009d38[];

extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __MessageID(int id);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __StartThunder(void);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __MapActor_WaitScript(int slot);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __SetCameraTarget(int slot, int a);
extern void __Func_8011ae0(void);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Func_8095240(void);
extern void __Func_8091e9c(int n);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_887_2008a0c(void)
{
    struct Actor *p;
    struct B *q;
    unsigned int i;
    int c0, c1;

    p = __MapActor_GetActor(0xa);
    q = p->f50;
    __CutsceneStart();
    __MapActor_SetPos(0xb, 0, 0);
    __MapActor_SetPos(0xc, 0, 0);
    __MapActor_SetPos(0xd, 0, 0);
    __MapActor_SetPos(0xe, 0, 0);
    __MapActor_SetPos(0xf, 0, 0);
    __MapActor_SetPos(0x10, 0, 0);
    { PIN3; q0 = 8; q1 = 0x1af0000; q2 = 0x1870000; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetPos(0xa, 0x1cf0000, 0xca << 17);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
    p->f23 &= 0xfe;
    p->f55 = 0;
    q->f9_2 = 1;
    __MapActor_SetBehavior(0xa, gScript_887__02009cec);
    *(int *)((char *)iwram_3001ebc[0] + (0xe0 << 1)) = 0x201;
    __CopyMapTiles(0x53, 0xf, 0x53, 0x13, 5, 4);
    __CopyMapTiles(0x5a, 0x10, 0x5a, 0x14, 5, 4);
    __CopyMapTiles(0x4d, 0x17, 0x52, 0x17, 5, 7);
    __CopyMapTiles(0x53, 0x21, 0x55, 0x21, 2, 2);
    __CopyMapTiles(0x5b, 0x1c, 0x5a, 0x1c, 1, 1);
    __CopyMapTiles(0x5b, 0x1c, 0x58, 0x1e, 1, 1);
    __CopyMapTiles(0x5e, 0x1b, 0x5e, 0x17, 6, 4);
    __CopyMapTiles(0x5c, 0x1c, 0x57, 0x17, 4, 4);
    __CopyMapTiles(0x41, 0x35, 0x58, 0x18, 2, 2);
    __Func_8011ae0();
    c0 = 0x3f42;
    REG_BLDCNT = c0;
    do { } while (0);
    c1 = 0x100c;
    REG_BLDALPHA = c1;
    __StartThunder();
    *(short *)((char *)iwram_3001ebc[3] + 0x1f84) = 1;
    __Func_8095240();
    __WaitFrames(0x1e);
    __SetCameraTarget(8, 1);
    { PIN3; q0 = 8; q1 = 0xc0 << 9; q2 = 0xc0 << 8; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 9; q2 = 0xc0 << 8; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xc0 << 9; q2 = 0xc0 << 8; __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetBehavior(0, gScript_887__02009bb4);
    __MapActor_SetBehavior(8, gScript_887__02009b78);
    __MapTransitionIn();
    __MapActor_WaitScript(8);
    __PlaySound(0x9e);
    { PIN3; q0 = 8; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(8, 2);
    { PIN3; q0 = 8; q1 = 0x80 << 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0x80 << 11; q1 = 0x80 << 8; __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xcf << 17; q1 = -1; q2 = 0x2120000; q3 = 1; __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 9; q1 = 0xcf << 17; q2 = 0x2120000; __MapActor_SetPos(q0, q1, q2); }
    __Func_80921c4(9, 0x1ab, 0x1e3);
    __Func_8093530();
    __MessageID(0xe5b);
    __Func_8093040(0x8009, 0, 0xa);
    __Func_80933d4(0xc0 << 9, 0xc0 << 6);
    __Func_80933f8(0xf0 << 17, -1, 0xde << 17, 1);
    __Func_8093530();
    __CutsceneWait(0x14);
    { PIN3; q0 = 8; q1 = 0x80 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(8, 3);
    __MapActor_DoAnim(0, 3);
    __MapActor_DoAnim(9, 3);
    __Func_809218c(9, 0x19f, 0x24d);
    __CutsceneWait(0xa);
    __MapActor_SetBehavior(8, gScript_887__02009c04);
    __MapActor_SetBehavior(0, gScript_887__02009c54);
    __PlaySound(0xea);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(0xa, gScript_887__02009d38);
    for (i = 0; i <= 3; i++) {
        int v = 0x100e;
        REG_BLDALPHA = v + i;
        __WaitFrames(1);
    }
    __PlaySound(0xca);
    __WaitFrames(0xa);
    for (i = 0; i <= 0xf; i++) {
        int v = 0x100f;
        REG_BLDALPHA = v - i;
        __WaitFrames(1);
    }
    __MapActor_WaitScript(0);
    __MapActor_SetAnim(8, 1);
    __Func_809259c(8, 2);
    __Func_80925cc(0, 2);
    __CutsceneWait(0xa);
    { PIN3; q0 = 8; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 8; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    __MapActor_Surprise(0, 0x81 << 1);
    __CutsceneWait(0x50);
    __MapActor_SetPos(9, 0, 0);
    __Func_8092848(8, 0, 0x14);
    __MapActor_SetAnim(8, 3);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x28);
    __Func_80933d4(0xcccc, 0x1999);
    __SetCameraTarget(8, 1);
    __MapActor_SetBehavior(8, gScript_887__02009ca4);
    __MapActor_RunScript(0, gScript_887__02009ca4);
    *(int *)((char *)iwram_3001ebc[0] + (0xe0 << 1)) = 0x100;
    *(int *)((char *)iwram_3001ebc[0] + (0xe4 << 1)) = 0x20;
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0x15);
}
