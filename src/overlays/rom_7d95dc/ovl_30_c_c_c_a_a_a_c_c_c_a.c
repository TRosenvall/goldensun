// fakematch
/* ovl_30_c_c_c_a_a_a_c_c_c_a.c  --  OvlFunc_953_2008710
 *   [the WHOLE of asm/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_a_c_c_c_a.s -- ONE
 *    `.thumb_func_start`, so NO SPLIT is required and overlay.ld:32 stays
 *    VERBATIM as `asm/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_a_c_c_c_a.o(.text)`]
 *
 *   OK OvlFunc_953_2008710 -- 1724 bytes, 665 encodings and 184 relocations
 *      identical.  Re-measured thirteen times.
 *
 * objcmp prints no `(built with: ...)` line: adjust = set(), the tree default
 * -O2 -mthumb -mthumb-interwork -fcall-used-r4.  `makefile_flags()` on this
 * path is the EMPTY set and no rule in the Makefile names any rom_7d95dc file
 * whose stem could match this one, so the rule that fires is `asm/%.o: src/%.c`.
 * The .s carries NO .section/.data/.bss/.lcomm/.word/.byte; all nine `.L`
 * symbols are branch targets DEFINED IN THIS FILE, so nothing needs exporting.
 * FAKEMATCH: the name goes in fakematch.txt (register-pin idiom).
 *
 * A cutscene: ~643 instructions of straight-line script under a save-bit guard,
 * with a three-way tail that gcc cross-jumps two of three ways into one block.
 *
 * hi = 0, hiv = 0 AND GCC SPENDS FOUR HIGH REGISTERS THE ROM NEVER TOUCHES.
 * The plain transcription is 600 of 665 differing, +12 bytes, and the FIRST
 * differing encoding is `mov r7, fp` -- the high-register save sequence itself.
 * That is the recorded "you-have-more-registers is a COMMONING tell" exactly:
 * gcse commons SEVEN values (0x3000, 0x8000, &iwram_3001ebc, 0xcccc, 0x6666,
 * 0xc000, 0x102) where the ROM holds THREE.
 *
 * WHAT CLOSED IT, in the order the mechanism sizes said to try them.  The first
 * five rows are objcmp's own encodings-differing count, which on a function this
 * size is DOMINATED by literal-pool offsets; the last four are a pool- and
 * branch-normalised count, and the two coincide from the row where the size
 * matches onward (objcmp reads 6, 4, 2, 0 on those four rows too):
 *
 *   plain C                                                    600  (+12 bytes)
 *   + 21 eviction pins on the four constants the ROM rebuilds   618  (+24 bytes)
 *   + pin every site EXCEPT the r5/r6 HELD-value uses           529   (-4 bytes)
 *   + each run of script-pointer uses held in a local `s`       (pool noise)
 *   + the GetActor/SetPos fill with r0 SEEDED LAST               11  (exact size)
 *   + __Func_8093554()'s store written `b = ... + 0x55; *b = 0`   6
 *   + one fill order at __Func_80933d4 and at OvlFunc_953_2009c5c 4
 *   - the __MapActor_RunScript(3, s) pin DROPPED                  2
 *   + a pin ADDED at __Func_8092adc(0x11, 0xc0 << 6, 0) in .Laec  0
 *
 * ONE PIN HAD TO BE *ADDED* LAST, AND IT WAS THE LAST LEVER.  With everything
 * else right the residue was two encodings: `lsls r1, r1, #1` and `movs r0, #3`
 * transposed at one __MapActor_Emote.  THAT SITE IS COMPLETELY INERT -- all six
 * fill orders, both narrower widths and no pin at all measure the same 2, and
 * `do { } while (0)` before / after measures 8 / 10, `__asm__ volatile("")` 4.
 * What closes it is a pin on a DIFFERENT site twenty instructions earlier, the
 * first of three adjacent __Func_8092adc calls that pass the held 0x3000.  NEW:
 * the recorded minimisation discipline is all about DROPPING pins; this is the
 * addition side reached at the END of the ladder rather than the start, and a
 * pure drop-to-fixpoint search would never have found it.
 *
 * THE SCRIPT POINTERS MUST BE *LOCALS*, NOT PINS.  The ROM does
 * `ldr r5, =Sym / mov r0, #1 / mov r1, r5` three times per run; a pin on those
 * sites emits `ldr r1, =Sym` at each use and pinning is not the cure -- dropping
 * the pins is not either (586).  Writing each run as `s = Sym; f(1, s); f(2, s);
 * g(3, s);` with `unsigned char *s` reassigned per run is what puts the symbol
 * in r5 and the copies in r1.  Three runs, three reassignments, one local.
 * This is the recorded "a link-time constant evicts nothing when pinned" seen
 * from the other side: the pointer is not a commoning HAZARD, it is a commoning
 * TARGET, and the source has to offer it a variable.
 *
 * THE HELD VALUES ARE FOUND BY EVICTING THE *EARLIEST* OCCURRENCE, NOT BY
 * NAMING THEM.  The ROM builds 0x3000 in r5 at the fourth call and first USES
 * it at the fifth -- but the SECOND call also wants 0x3000 and the ROM builds
 * that one inline.  Pin the second call and leave the remaining eight bare: the
 * cse web now starts where the ROM's does and gcc picks r5 by itself.  Same for
 * 0x8000/r6, which the ROM never rebuilds at all and which therefore needs no
 * eviction site.  No `int v = ...` local is involved in either.
 *
 * THE ONE-COMPILE DIAGNOSTIC SAID "NOT ALIAS" AND IT WAS RIGHT.
 * -fno-schedule-insns2 on the 6-differing candidate measures 199 -- a clean
 * REGRESSION, so sched2 was already right and the alias axis was never opened.
 * (-fno-schedule-insns is inert: there is no sched1 at -O2 on thumb.)
 *
 * THE PATH IS NOT MONOTONE AND THE PIN SET IS ONLY MINIMAL W.R.T. ITS BASE.
 * The __Func_80933d4 fill order was worth 3 differing when it was found and is
 * INERT in the shipped set; the OvlFunc_953_2009c5c fill order went from -2 to
 * +2 to -2 as the base changed under it; the first 80-site pin set minimised to
 * 36 in one set-wise drop (all 31 singly-droppable pins drop together) and then
 * to 35 by width, with a strict re-drop finding nothing further.  Three of the
 * six fill orders that fix __Func_8092adc(0x14, 0xc0 << 6, 0) tie exactly, and
 * the common feature is that q2 is assigned before q1 -- plain ascending is
 * wrong there and the ROM's own descending order is one of the three winners.
 *
 * MEASURED WORSE / INERT (against 665 encodings / 1724 bytes):
 *
 *   spelling                                            differing
 *   -------------------------------------------------  ---------
 *   plain C, no pins                                         600  (+12 bytes)
 *   21 eviction pins on the four rebuilt constants           618  (+24 bytes)
 *   every site pinned, held values included                  611  (push too NARROW)
 *   every site pinned, held-value uses bare                  529   (-4 bytes)
 *   ... and all script-pointer pins DROPPED instead of        586
 *     held in a local
 *   do { } while (0) before / after the Emote                   8 / 10
 *   __asm__ volatile("") at five places around it               4 (INERT)
 *   -fno-schedule-insns2 on the 6-differing candidate          199
 *   INERT: the 0x19999/0x3333 fill order in the final set;
 *     every fill order and width at the residual Emote;
 *     31 of the 66 pins, dropped as one set, then 25 of the
 *     35 survivors narrowed by width to a fixpoint.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, void *s);
extern void __MapActor_RunScript(int slot, void *s);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8091e9c(int a);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern int __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern unsigned char *__Func_8093554(void);
extern void OvlFunc_953_2009c48(int slot);
extern void OvlFunc_953_2009c5c(int slot, int a);
extern unsigned char ActorCmd_ARRAY_953__0200ad3c[];
extern unsigned char gScript_953__0200ade4[];
extern unsigned char gScript_884__0200ad74[];
extern unsigned char *iwram_3001ebc;

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_953_2008710(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *s;

    if (__GetFlag(0x962) == 0)
        return;
    __CutsceneStart();
    __Func_80925cc(0x11, 1);
    { PIN2; q0 = 0x11; q1 = 0xc0 << 6; __Func_8092adc(q0, q1, 0x14); }  /*0*/
    __Func_8092adc(0x11, 0, 0x3c);
    { PIN2; q0 = 0x11; q1 = 0x80 << 1; __MapActor_Emote(q0, q1, 0x28); }  /*1*/
    OvlFunc_953_2009c5c(0x11, 0xc0 << 6);
    __MessageID(0x2267);
    __Func_809259c(0x11, 2);
    OvlFunc_953_2009c48(0x11);
    __Func_8092adc(0x12, 0xc0 << 6, 0);
    __Func_8092adc(0x13, 0xc0 << 6, 0);
    __Func_8092adc(0x14, 0xc0 << 6, 0);
    b = __Func_8093554() + 0x55;
    *b = 0;
    __Func_80933d4(0x19999, 0x3333);
    { PIN2; q0 = 0x80 << 17; q1 = -1; __Func_80933f8(q0, q1, 0xac << 16, 1); }  /*8*/
    __Func_8093530();
    __CutsceneWait(0x14);
    { PIN2; q0 = 0x12; q1 = 0x81 << 1; __MapActor_Emote(q0, q1, 0x28); }  /*9*/
    OvlFunc_953_2009c48(0x12);
    __MapActor_DoAnim(0x11, 3);
    OvlFunc_953_2009c48(0x11);
    { PIN2; q0 = 0x13; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }  /*10*/
    __CutsceneWait(0x28);
    OvlFunc_953_2009c5c(0x13, 0);
    OvlFunc_953_2009c48(0x13);
    { PIN1; q0 = 0x14; __MapActor_Emote(q0, 0x103, 0x28); }  /*11*/
    OvlFunc_953_2009c5c(0x14, 0);
    __Func_809259c(0x14, 2);
    __Func_8093040(0x14, 0, 0x14);
    __Func_80925cc(0x11, 1);
    __CutsceneWait(0xa);
    { PIN2; q0 = 0x11; q1 = 0xb0 << 8; __Func_8092adc(q0, q1, 0x14); }  /*12*/
    __Func_8093040(0x11, 0, 0x14);
    __Func_80925cc(0x12, 1);
    __MapActor_DoAnim(0x12, 4);
    __CutsceneWait(0x14);
    OvlFunc_953_2009c5c(0x11, 0x80 << 8);
    __Func_8093040(0x11, 0, 0x14);
    __Func_80925cc(0x13, 1);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0x13, 4);
    __Func_8093040(0x11, 0, 0x14);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0x14, 3);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0x11, 3);
    __MapActor_SetAnim(0x12, 3);
    __MapActor_SetAnim(0x13, 3);
    __MapActor_DoAnim(0x14, 3);
    __Func_8092adc(0x11, 0xc0 << 6, 0);
    __Func_8092adc(0x12, 0xc0 << 6, 0);
    __Func_8092adc(0x13, 0xc0 << 6, 0);
    OvlFunc_953_2009c5c(0x14, 0xc0 << 6);
    { PIN2; q0 = 0x11; q1 = 0x9999; __MapActor_SetSpeed(q0, q1, 0x4ccc); }  /*18*/
    { PIN2; q0 = 0x11; q1 = 0x81 << 1; __Func_80921c4(q0, q1, 0xac); }  /*19*/
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }  /*20*/
    { PIN2; q0 = 0; q1 = 0x83 << 1; __Func_80921c4(q0, q1, 0xbc); }  /*21*/
    { PIN2; q0 = 0; q1 = 0xc0 << 8; __Func_8092adc(q0, q1, 0); }  /*22*/
    a = __MapActor_GetActor(0);
    if (a != 0)
        { PIN3; q1 = *(int *)(a + 8); q2 = *(int *)(a + 0x10); q0 = 1;
          __MapActor_SetPos(q0, q1, q2); }
    a = __MapActor_GetActor(0);
    if (a != 0)
        { PIN3; q1 = *(int *)(a + 8); q2 = *(int *)(a + 0x10); q0 = 2;
          __MapActor_SetPos(q0, q1, q2); }
    a = __MapActor_GetActor(0);
    if (a != 0)
        { PIN3; q1 = *(int *)(a + 8); q2 = *(int *)(a + 0x10); q0 = 3;
          __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }  /*23*/
    { PIN3; q0 = 2; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }  /*24*/
    { PIN3; q0 = 3; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }  /*25*/
    __Func_809218c(1, 0xf6, 0xc8);
    { PIN2; q0 = 2; q1 = 0x83 << 1; __Func_809218c(q0, q1, 0xc8); }  /*26*/
    { PIN1; q0 = 3; __Func_80921c4(q0, 0x8b << 1, 0xc8); }  /*27*/
    __MapActor_SetAnim(2, 1);
    __MapActor_SetAnim(1, 1);
    { PIN2; q0 = 1; q1 = 0xc0 << 8; __Func_8092adc(q0, q1, 0); }  /*28*/
    { PIN2; q0 = 2; q1 = 0xc0 << 8; __Func_8092adc(q0, q1, 0); }  /*29*/
    { PIN2; q0 = 3; q1 = 0xc0 << 8; __Func_8092adc(q0, q1, 0x28); }  /*30*/
    __Func_80925cc(0x11, 1);
    OvlFunc_953_2009c48(0x11);
    __Func_809259c(0, 2);
    __Func_809259c(1, 2);
    __Func_809259c(2, 2);
    __Func_80925cc(3, 2);
    __MapActor_DoAnim(0x11, 3);
    __Func_8092c40(0x11, 0);
    { PIN1; q0 = 1; __Func_8092adc(q0, 0xe0 << 8, 0); }  /*31*/
    { PIN2; q0 = 3; q1 = 0xa0 << 8; __Func_8092adc(q0, q1, 0); }  /*32*/
    if (__Func_8091c7c(0, 0) == 0) {
        __MapActor_DoAnim(0x11, 3);
        OvlFunc_953_2009c48(0x11);
        s = ActorCmd_ARRAY_953__0200ad3c;
        __MapActor_SetBehavior(1, s);
        __MapActor_SetBehavior(2, s);
        __MapActor_RunScript(3, s);
        __Func_80933d4(0x6666, 0xccc);
        __Func_80933f8(0x80 << 17, -1, 0xc8 << 15, 1);
        { PIN3; q0 = 0x11; q1 = 0x80 << 9; q2 = 0x80 << 8; __MapActor_SetSpeed(q0, q1, q2); }  /*38*/
        s = gScript_953__0200ade4;
        __MapActor_SetBehavior(0x11, s);
        __CutsceneWait(0xa);
        { PIN1; q0 = 0; __MapActor_SetSpeed(q0, 0x80 << 9, 0x80 << 8); }  /*40*/
        __MapActor_SetBehavior(0, s);
        __CutsceneWait(0x50);
        *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x28;
        __MapTransitionOut();
        __WaitMapTransition();
    } else {
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        { PIN3; q0 = 0x11; q1 = 0x81 << 1; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }  /*42*/
        OvlFunc_953_2009c48(0x11);
        { PIN2; q0 = 0x12; q1 = 0x81 << 1; __MapActor_Emote(q0, q1, 0x28); }  /*43*/
        OvlFunc_953_2009c48(0x12);
        { PIN3; q0 = 0x13; q1 = 0x81 << 1; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }  /*44*/
        OvlFunc_953_2009c48(0x13);
        { PIN2; q0 = 0x14; q1 = 0x81 << 1; __MapActor_Emote(q0, q1, 0x28); }  /*45*/
        OvlFunc_953_2009c48(0x13);
        __MapActor_DoAnim(0x11, 4);
        OvlFunc_953_2009c48(0x11);
        __Func_80925cc(2, 1);
        __CutsceneWait(0x14);
        OvlFunc_953_2009c48(2);
        { PIN2; q0 = 3; q1 = 0x81 << 1; __MapActor_Emote(q0, q1, 0x28); }  /*46*/
        OvlFunc_953_2009c48(3);
        { PIN1; q0 = 0x11; __Func_8092adc(q0, 0xb0 << 8, 0); }  /*47*/
        __Func_8092adc(0x13, 0x80 << 8, 0);
        __Func_8092adc(0x14, 0, 0x3c);
        __Func_8092adc(0x11, 0xc0 << 6, 0);
        __Func_8092adc(0x13, 0xc0 << 6, 0);
        __Func_8092adc(0x14, 0xc0 << 6, 0x14);
        __MapActor_SetAnim(0x11, 3);
        __MapActor_SetAnim(0x12, 3);
        __MapActor_SetAnim(0x13, 3);
        __MapActor_DoAnim(0x14, 3);
        { PIN1; q0 = 3; __MapActor_Emote(q0, 0x80 << 1, 0x3c); }  /*52*/
        OvlFunc_953_2009c5c(3, 0xa0 << 8);
        OvlFunc_953_2009c48(3);
        __MapActor_SetAnim(1, 4);
        __CutsceneWait(0x14);
        __Func_8092c40(1, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __MapActor_DoAnim(0x11, 3);
            OvlFunc_953_2009c48(0x11);
            s = ActorCmd_ARRAY_953__0200ad3c;
            __MapActor_SetBehavior(1, s);
            __MapActor_SetBehavior(2, s);
            __MapActor_RunScript(3, s);
            __Func_80933d4(0x6666, 0xccc);
            __Func_80933f8(0x80 << 17, -1, 0xc8 << 15, 1);
            { PIN3; q0 = 0x11; q1 = 0x80 << 9; q2 = 0x80 << 8; __MapActor_SetSpeed(q0, q1, q2); }  /*59*/
            s = gScript_953__0200ade4;
            __MapActor_SetBehavior(0x11, s);
            __CutsceneWait(0xa);
            { PIN1; q0 = 0; __MapActor_SetSpeed(q0, 0x80 << 9, 0x80 << 8); }  /*61*/
            __MapActor_SetBehavior(0, s);
            __CutsceneWait(0x50);
            *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x28;
            __MapTransitionOut();
            __WaitMapTransition();
        } else {
            *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
            __CutsceneWait(0x14);
            __Func_809259c(1, 2);
            __Func_8093040(1, 0, 0x14);
            __MapActor_Emote(2, 0x81 << 1, 0x3c);
            OvlFunc_953_2009c48(2);
            OvlFunc_953_2009c5c(3, 0x80 << 8);
            __MapActor_SetAnim(3, 3);
            OvlFunc_953_2009c48(3);
            s = gScript_884__0200ad74;
            __MapActor_SetBehavior(2, s);
            __MapActor_RunScript(3, s);
            __CutsceneWait(0x14);
            __MapActor_RunScript(0, s);
            { PIN1; q0 = 1; __Func_80921c4(q0, 0x83 << 1, 0xbc); }  /*68*/
            OvlFunc_953_2009c5c(1, 0xc0 << 8);
            __MapActor_DoAnim(1, 3);
            OvlFunc_953_2009c48(1);
            __Func_80933d4(0x6666, 0xccc);
            __Func_80933f8(0x80 << 17, -1, 0xc8 << 15, 1);
            { PIN2; q0 = 0x11; q1 = 0x80 << 9; __MapActor_SetSpeed(q0, q1, 0x80 << 8); }  /*72*/
            s = gScript_953__0200ade4;
            __MapActor_SetBehavior(0x11, s);
            __CutsceneWait(0xa);
            { PIN1; q0 = 1; __MapActor_SetSpeed(q0, 0x80 << 9, 0x80 << 8); }  /*74*/
            __MapActor_SetBehavior(1, s);
            __CutsceneWait(0x50);
            *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x28;
            __MapTransitionOut();
            __WaitMapTransition();
        }
    }
    __Func_8091e9c(2);
    __SetFlag(0x93f);
    __CutsceneEnd();
}
