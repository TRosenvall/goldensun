/* PARKED -- OvlFunc_934_2009984.  NON-MATCHING, 4 ENCODINGS OF 261 against the
 * tracked reference, OF WHICH ONLY 2 ARE INSTRUCTIONS.  TWO POOL WORDS AND THE
 * RELOCATION SET ARE A REFERENCE-SIDE SPELLING ARTIFACT, NOT A CODE DEFECT.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7bdeb0/2009984.c \
 *     asm/overlays/rom_7bdeb0/ovl_169c_a_c_c_b.s --func OvlFunc_934_2009984
 *   XX ENCODINGS differ in 4 place(s) (ref 261, ours 261)
 *      first at index 218: ref 2304  ours 2000
 *   XX RELOCATIONS differ          <-- ours has 2 extra ABS32, at 0x250 and 0x26c
 *
 * BATCH 314 CORRECTED THIS PARK'S HEADLINE FIGURE.  The previous revision claimed
 * "4 of 261" and reported no relocation problem at all, while its own prose said
 * "RESIDUE: TWO ENCODINGS, ONE INSTRUCTION".  THE PROSE WAS RIGHT AND THE FIGURE
 * WAS WRONG, and one cause explains both symptoms.  The reference spells the area
 * test's two pooled constants as LITERALS --
 *
 *     line  21:   ldr  r3, =0x5e
 *     line 163:   ldr  r3, =0x5f
 *
 * -- whereas this file spells them, correctly and per this park's own area.sym
 * reasoning, as the symbols `_AREA_5e` / `_AREA_5f` (area.sym:86-87 defines
 * `_AREA_5e = 0x5e;` and `_AREA_5f = 0x5f;`).  So our pool words are 0x00000000
 * plus an R_ARM_ABS32 and the reference's are 0x0000005e / 0x0000005f with no
 * relocation.  That accounts for the relocation line AND for 2 of the 4
 * "differing encodings", which are POOL WORDS, not instructions.
 *
 * ONE asm/ LINE PER CONSTANT IS NEEDED BEFORE THIS CAN LAND, AND IT IS NOT A CODE
 * CHANGE.  asm/overlays/rom_7bdeb0/ovl_169c_a_c_c_b.s needs
 *
 *     line  21:   ldr  r3, =0x5e   ->   ldr  r3, =_AREA_5e
 *     line 163:   ldr  r3, =0x5f   ->   ldr  r3, =_AREA_5f
 *
 * which is the tree's OWN settled practice with tracked precedent --
 * asm/overlays/rom_7b4558/ovl_30_c_c_a_a_b.s lines 44/46/48 carry
 * `.word _AREA_44` / `_AREA_45` / `_AREA_46`, and 226 files under src/ already use
 * `extern int _AREA_xx;` with `(int)&_AREA_xx`.  area.sym's assignments become
 * absolute symbols in stage1.o, so the linker resolves `_AREA_5e` to 0x5e and THE
 * SUBSTITUTION EMITS IDENTICAL BYTES.  Same class as the `.global .L2430` export
 * that src/non_matching/ovl_7a7298/2009fa4.c needs.
 *
 * WITH THAT SUBSTITUTION MADE, THE FIGURE IS 2 OF 261 AND THE RELOCATIONS ARE
 * CLEAN (measured against a workspace copy of the reference):
 *
 *   XX ENCODINGS differ in 2 place(s) (ref 261, ours 261)
 *      first at index 218: ref 2304  ours 2000
 *
 * ========================= WHAT IS SETTLED =========================
 * (1) the area test is two pooled area.sym symbols; (2) the sub-state selector is
 * a `switch`, cases 1..10 in three arms, emitted as gcc's dense table; (3)
 * `__SetFlag(0x201)` must reach its constant by a delta from the 0x1c2 byte
 * offset already in a register, which needs a FRESH base pointer local inside
 * that switch arm; (4) ONE variable carries four roles in r5 (22 -> 8); (5) the
 * argument fill is r1,r2,r3 THEN r0 (8 -> 4); (6) `cmp r3,#1 / blt` is not
 * `sub < 1` and needs the constant behind a do{}while(0) barrier (4 -> 2).
 * tools/datacheck.py reports NO data section, so NO SPLIT and no data export;
 * the 10-word table at .L19b8 is gcc's own switch table, not ROM data.
 *
 * ========================== WHAT IS OPEN ===========================
 * THE RESIDUE IS ONE ADJACENT TRANSPOSITION of the argument fill at the third
 * OvlFunc_934_2008528 site:
 *
 *     [218] ref 2304 movs r3, #4  | ours 2000 movs r0, #0
 *     [219] ref 2000 movs r0, #0  | ours 2304 movs r3, #4
 *
 * BATCH 314 MEASURED 31 NEW SPELLINGS AT THAT SITE AND NONE REACHES 0.
 * The park's suspected coupling IS DISPROVED: the {t, four} DECLARATION ORDER IS
 * INERT ON EVERY PIN SHAPE, so it was not waiting on the pin width to be fixed.
 *   base / swapped decls                               2
 *   q0 pinned and assigned last                        2
 *   pinned r3 carries the STACK argument, 5 orders      2
 *   q1,q2,q3 ascending; q3 via `q3=2; q3<<=1`           2, BUT AT INDEX 214
 *   q1,q2,q3 with q3 first / middle                     5 / 4
 *   q0..q3 four-wide, r0 last / r0 before r3            7 / 6
 *   no pins at the site / q3 only / q2+q3 / q0 only     4 / 6 / 6 / 4
 *   `t` pinned to r5                                   44, relocations differ
 *
 * THE TWO DEFECTS ARE MUTUALLY EXCLUSIVE UNDER PINNING, which is the real finding
 * and bounds the search.  `q1,q2,q3` ascending also reads 2, but its residue is a
 * different pair AND a different KIND -- the transposition is FIXED and the
 * carrier register for the stack argument breaks instead:
 *
 *     [214] ref 2302 movs r3, #2 | ours 2002 movs r0, #2
 *     [215] ref 9300 str r3,[sp] | ours 9000 str r0,[sp]
 *
 * Reserving r3 for argument 4 is exactly what stops gcc materialising the stack
 * argument in r3.  So r3 is under a two-sided constraint at this site and no
 * pure pin shape can satisfy both ends; the next thing to try is a spelling that
 * changes the ORDER WITHOUT RESERVING r3.
 *
 * THE BRIEF'S ALIAS-SET DEPENDENT-COUNT LEVER DOES NOT REACH THIS SITE.  It needs
 * a MEM as one of the two COMPETING insns; here both are constant register sets
 * (`movs r3,#4`, `movs r0,#0`).  The block's two stack stores are their common
 * SUCCESSOR, not a competitor, so widening any alias set cannot separate them.
 *
 * SHIMS: 15 register pins (tools/shimcount.py), no fakematch.txt row yet.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;   /* GlobalState @ 0x02000240 */
extern int _AREA_5e;
extern int _AREA_5f;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __WaitFrames(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8092b08(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_common0_70(int a, int b, int c, int d);
extern void OvlFunc_934_2008528(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_934_2008ba4(int a);
extern void OvlFunc_934_2009770(void);
extern void OvlFunc_934_2008cf8(void);

void OvlFunc_934_2009984(void)
{
    unsigned char *g;
    int area;
    int sub;
    int t;
    int four;

    g = (unsigned char *)&gState;
    area = *(short *)(g + (0xe0 << 1));
    if (area == (int)&_AREA_5e) {
        switch (*(short *)(g + (0xe1 << 1))) {
        case 1:
        case 2:
        case 3:
        case 4:
            __Func_8092b08(0xf, 3);
            __Func_8092b08(0xd, 3);
            OvlFunc_common0_70(0xf0 << 15, 0, 0xe8 << 16, 0xdf);
            break;
        case 5:
        case 6:
        case 7:
            if (__GetFlag(0x70) != 0)
                break;
            if (__GetFlag(0x302) == 0)
                break;
            __SetFlag(0x80 << 2);
            { unsigned char *g2 = (unsigned char *)&gState;
              if (*(short *)(g2 + (0xe1 << 1)) == 5)
                  __SetFlag(0x201); }
            __WaitFrames(1);
            if (__GetFlag(0x109) != 0)
                break;
            { register int q0 __asm__("r0"); register int q1 __asm__("r1"); register int q2 __asm__("r2");
              q0 = 8; q1 = 0xc6 << 18; q2 = 0x8c << 17; __MapActor_SetPos(q0, q1, q2); }
            *(int *)(__MapActor_GetActor(8) + 0x6c) = (int)OvlFunc_934_2008cf8;
            break;
        case 8:
        case 9:
        case 10:
            { register int q0 __asm__("r0"); register int q1 __asm__("r1"); register int q2 __asm__("r2"); register int q3 __asm__("r3");
              q2 = 0x8a << 18; q1 = 0; q3 = 0x14; q0 = 0x2820000; OvlFunc_common0_70(q0, q1, q2, q3); }
            { int e5 = 0; int e6 = 0x22; __Func_8010704(0x17, 0x22, 0xd, 3, e5, e6); }
            OvlFunc_934_2009770();
            if (__GetFlag(0x80 << 2) != 0)
                { int e5 = 0x17; int e6 = 0x27; __Func_8010704(0x17, 0x29, 1, 1, e5, e6); }
            if (__GetFlag(0x201) == 0)
                break;
            { int e5 = 0x1b; int e6 = 0x29; __Func_8010704(0x1f, 0x27, 2, 1, e5, e6); }
            break;
        }
    } else if (area == (int)&_AREA_5f) {
        sub = *(short *)(g + (0xe1 << 1));
        if (sub > 3)
            return;
        { int one = 1;
          do { one = (int) one; } while (0);
          if (sub < one)
              return; }
        if (__GetFlag(0x202) != 0) {
            four = 4;
            t = 0;
            { register int q1 __asm__("r1"); register int q2 __asm__("r2"); register int q3 __asm__("r3");
              q1 = 0xc; q2 = 0x10; q3 = 1; OvlFunc_934_2008528(0, q1, q2, q3, four, t); }
            OvlFunc_934_2008528(0, 0xd, 0x10, 1, four, t);
        } else {
            OvlFunc_934_2008ba4(9);
        }
        if (__GetFlag(0x203) != 0) {
            four = 4;
            t = 0;
            { register int q1 __asm__("r1"); register int q2 __asm__("r2"); register int q3 __asm__("r3");
              q1 = 0x10; q2 = 0x10; q3 = 1; OvlFunc_934_2008528(2, q1, q2, q3, four, t); }
            OvlFunc_934_2008528(0, 0x10, 0x10, 1, four, t);
        } else {
            OvlFunc_934_2008ba4(0xa);
        }
        t = __GetFlag(0x205);
        if (t != 0) {
            OvlFunc_934_2008528(0, 0xd, 0x13, 4, 2, 0);
        } else if (__GetFlag(0x81 << 2) != 0) {
            { register int q1 __asm__("r1"); register int q2 __asm__("r2");
              q1 = 0xd; q2 = 0xf; OvlFunc_934_2008528(0, q1, q2, 4, 2, t); }
            t = 0xe;
            __Func_8010704(0xe, 0x11, 2, 1, t, 0x10);
            __Func_8010704(0xe, 0xd, 1, 1, t, 0xf);
        } else {
            OvlFunc_934_2008ba4(0xb);
            __Func_8092b08(0xb, 3);
        }
    }
}
