/* Cluster OvlFunc_944_2008af8..OvlFunc_944_2008af8 extracted from
 * goldensun/asm/overlays/rom_7ca63c/ovl_30_c_c_a_c_c_c_a.s -- ONE function in its .s, no split.
 *
 * 338 instructions. Never attempted before batch 280. FIFTEEN PIN3 SITES, ALL FIFTEEN LOAD-BEARING (cost 2 to 34 each) -- one fakematch row.
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
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
    unsigned char pad14[4];
    int f18;
    int f1c;
    unsigned char pad20[8];
    int f28;
    unsigned char pad2c[0x18];
    int f44;
    int f48;
};

extern unsigned char *iwram_3001ebc;
extern unsigned char gOvl_0200976c[];
extern unsigned char L1844[] __asm__(".L1844");
extern unsigned char gScript_944__0200939c[];
extern unsigned char gScript_944__020093ac[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __LoadFieldActors(unsigned char *p);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __Func_8092950(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8091e9c(int n);
extern void OvlFunc_944_2008a84(int slot);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_944_2008af8(void)
{
    struct Actor *p;
    unsigned int i;

    __CutsceneStart();
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __WaitFrames(1);
    __LoadFieldActors(gOvl_0200976c);
    __WaitFrames(1);
    __LoadFieldActors(L1844);
    __WaitFrames(1);
    OvlFunc_944_2008a84(9);
    OvlFunc_944_2008a84(0xa);
    OvlFunc_944_2008a84(0xb);
    OvlFunc_944_2008a84(0xc);
    OvlFunc_944_2008a84(0xd);
    OvlFunc_944_2008a84(0xe);
    OvlFunc_944_2008a84(0xf);
    __MapActor_SetBehavior(8, gScript_944__0200939c);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x203;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x96 << 1);
    __PlaySound(0x93);
    __CutsceneWait(0x64);
    __MapActor_SetIdle(9);
    __MapActor_SetIdle(0xa);
    __MapActor_SetIdle(0xb);
    __MapActor_SetIdle(0xc);
    __MapActor_SetIdle(0xd);
    __MapActor_SetIdle(0xe);
    __MapActor_SetIdle(0xf);
    { PIN3; q0 = 9;   q1 = 0xc0 << 10; q2 = 0xc0 << 9; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xc0 << 10; q2 = 0xc0 << 9; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xc0 << 10; q2 = 0xc0 << 9; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xc0 << 10; q2 = 0xc0 << 9; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0xc0 << 10; q2 = 0xc0 << 9; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0xc0 << 10; q2 = 0xc0 << 9; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xf; q1 = 0xc0 << 10; q2 = 0xc0 << 9; __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_TravelTo(9, 0, 0x64);
    __MapActor_TravelTo(0xa, 0x3c, 0x64);
    __MapActor_TravelTo(0xb, 0x78, 0x64);
    __MapActor_TravelTo(0xc, 0xb4, 0x64);
    __MapActor_TravelTo(0xd, 0xf0, 0x64);
    { PIN3; q0 = 0xe; q1 = 0xa0 << 1; q2 = 0x64; __MapActor_TravelTo(q0, q1, q2); }
    __MapActor_TravelTo(0xf, 0xbe << 1, 0x64);
    __CutsceneWait(0x28);
    __MapActor_Emote(8, 0x101, 0);
    __CutsceneWait(0x14);
    __MapActor_SetPos(9, 0, 0);
    __MapActor_SetPos(0xa, 0, 0);
    __MapActor_SetPos(0xb, 0, 0);
    __MapActor_SetPos(0xc, 0, 0);
    __MapActor_SetPos(0xd, 0, 0);
    __MapActor_SetPos(0xe, 0, 0);
    __MapActor_SetPos(0xf, 0, 0);
    __CutsceneWait(0x64);
    p = (struct Actor *)__MapActor_GetActor(0x12);
    p->f18 = 0x1999;
    p->f1c = 0x1999;
    __MapActor_SetPos(0x12, 0xac << 16, 0xaa << 17);
    __MapActor_SetIdle(8);
    __WaitFrames(1);
    __Func_80925cc(8, 1);
    __Func_8092adc(8, 0xc0 << 6, 0);
    __PlaySound(0x1d);
    __SetFlag(0x8f << 4);
    for (i = 0; i <= 0x1f; i++) {
        p->f18 += 0xccc;
        p->f1c += 0xccc;
        __WaitFrames(1);
    }
    { PIN3; q0 = 8; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(8, 2);
    { PIN3; q0 = 8; q1 = 0xa8; q2 = 0xaa << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0xc8; q2 = 0xaa << 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0x80 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    p = (struct Actor *)__MapActor_GetActor(0x11);
    p->f18 = 0x12666;
    p->f1c = 0x12666;
    p->f8 = 0xac << 16;
    p->fc = 0xa0 << 16;
    p->f10 = 0xaa << 17;
    p->f6 = 0;
    p->f44 = 0x6666;
    p->f48 = 0xc0 << 10;
    __CutsceneWait(0x14);
    __MapActor_Jump(8, 6, 0x14);
    __PlaySound(0x93);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(8, gScript_944__020093ac);
    __CutsceneWait(0x50);
    __Func_8092b08(0x11, 1);
    { PIN3; q0 = 0x11; q1 = 0x80 << 9; q2 = 0x80 << 8; __MapActor_SetSpeed(q0, q1, q2); }
    p->f44 = 0x1999;
    p->f48 = 0xb333;
    __PlaySound(0x99);
    p->f28 = 0x80 << 12;
    { PIN3; q0 = 0x11; q1 = 0x84; q2 = 0xb4 << 1; __MapActor_TravelTo(q0, q1, q2); }
    { PIN3; q0 = 0x12; q1 = 0x84; q2 = 0xb4 << 1; __MapActor_TravelTo(q0, q1, q2); }
    __CutsceneWait(0x28);
    __MapActor_SetPos(0x11, 0, 0);
    p = (struct Actor *)__MapActor_GetActor(8);
    p->f18 = 0x80 << 9;
    p->f1c = 0x80 << 9;
    p->f6 = 0xa0 << 7;
    __CutsceneWait(0x28);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x202;
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0xd);
    __CutsceneEnd();
}
