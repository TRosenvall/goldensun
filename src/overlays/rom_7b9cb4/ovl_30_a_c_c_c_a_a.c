/* Cluster OvlFunc_932_200b028..OvlFunc_932_200b028 extracted from
 * goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_c_a.s -- ONE function in its .s, no split.
 *
 * 373 instructions. Never attempted before batch 280. TWENTY-NINE PINS at fixpoint, down from 32 -- one fakematch row. THE THREE FREE DROPS WERE EACH
 * THE LAST MEMBER OF A REPEATED-CONSTANT RUN, with no later consumer left to CSE into, which is a
 * useful shape to check when trimming a ladder.
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
struct Actor {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
};

extern unsigned char *iwram_3001ebc;
extern unsigned char gScript_932__0200c054[];
extern unsigned char gScript_932__0200c084[];
extern unsigned char gScript_932__0200c0b4[];
extern unsigned char gScript_932__0200c0e4[];
extern unsigned char gScript_932__0200c12c[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __MessageID(int id);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __ActorMessage(int slot, int b);
extern void __StartTask(void (*f)(void), int n);
extern void __StopTask(void (*f)(void));
extern int __Func_8091c7c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092950(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void OvlFunc_932_200affc(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_932_200b028(void)
{
    struct Actor *p;

    __CutsceneStart();
    { PIN3; q0 = 8; q1 = 0xa4 << 17; q2 = 0xb0 << 15; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xa4 << 17; q2 = 0xb0 << 15; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetAnim(8, 0);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x100;
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x28;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    p = __MapActor_GetActor(0);
    if (p != 0) {
        __MapActor_SetPos(1, p->f8, p->f10);
    }
    p = __MapActor_GetActor(0);
    if (p != 0) {
        __MapActor_SetPos(2, p->f8, p->f10);
    }
    p = __MapActor_GetActor(0);
    if (p != 0) {
        __MapActor_SetPos(3, p->f8, p->f10);
    }
    { PIN3; q0 = 1; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetBehavior(1, gScript_932__0200c054);
    __MapActor_SetBehavior(2, gScript_932__0200c084);
    __MapActor_RunScript(3, gScript_932__0200c0b4);
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(1, 1);
    __Func_8092adc(1, 0xe0 << 8, 0xa);
    __MessageID(0x190c);
    __Func_8093040(1, 0, 0xa);
    __Func_80925cc(2, 1);
    { PIN3; q0 = 2; q1 = 0xa0 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x80 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 2; __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        __MapActor_DoAnim(2, 3);
    } else {
        __MapActor_DoAnim(2, 4);
        (*(unsigned short *)(iwram_3001ebc + (0xec << 1)))++;
    }
    __Func_8093040(2, 0, 0x14);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(0, 3);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x14);
    __Func_80925cc(3, 2);
    __MessageID(0x1910);
    __ActorMessage(3, 0);
    { PIN3; q0 = 0; q1 = 0x80 << 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 6; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    __ActorMessage(1, 0);
    { PIN3; q0 = 0; q1 = 0xc0 << 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __MapActor_Emote(3, 0x101, 0x28);
    __PlaySound(0xbe);
    __Func_8092950(8, 7);
    __CutsceneWait(0xa);
    __PlaySound(0x121);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __MapActor_Emote(3, 0x80 << 1, 0x28);
    __PlaySound(0x67);
    __StartTask(OvlFunc_932_200affc, 0xc8 << 4);
    __MapActor_SetBehavior(9, gScript_932__0200c0e4);
    __MapActor_RunScript(8, gScript_932__0200c0e4);
    __StopTask(OvlFunc_932_200affc);
    __CutsceneWait(0x3c);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    __ActorMessage(2, 0);
    { PIN3; q0 = 1; q1 = 0xe0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xa0 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    __Func_80925cc(3, 1);
    __Func_8093040(3, 0, 0x14);
    { PIN3; q0 = 1; q1 = 0xe0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 7; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(1, 3);
    __Func_8093040(1, 0, 0xa);
    __MapActor_DoAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(1, gScript_932__0200c12c);
    __MapActor_SetBehavior(2, gScript_932__0200c12c);
    __MapActor_RunScript(3, gScript_932__0200c12c);
    __CutsceneWait(0x14);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x204;
    __ClearFlag(0x12f);
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x10;
    __SetFlag(0x909);
    __CutsceneEnd();
}
