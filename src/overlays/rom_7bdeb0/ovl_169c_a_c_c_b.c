/* OvlFunc_934_2009984 -- *** EXACT ***, 632 bytes, 261 encodings and 49
 * relocations identical, stable over three repeats.  LANDS AT 5 REGISTER PINS,
 * DOWN FROM 15.  ONE PREREQUISITE, AND IT IS NOT A CODE CHANGE (see A).
 *
 * Verify with:
 *   python3 tools/objcmp.py src/overlays/rom_7bdeb0/ovl_169c_a_c_c_b.c \
 *     asm/overlays/rom_7bdeb0/ovl_169c_a_c_c_b.s --func OvlFunc_934_2009984
 *   OK OvlFunc_934_2009984 -- 632 bytes, 261 encodings and 49 relocations identical
 *
 * ONE function in the reference and tools/datacheck.py reports no data section,
 * so NO SPLIT, no data export, and the .c installs at the mirrored path.
 *
 * ===================== A. THE asm/ PREREQUISITE =====================
 * asm/overlays/rom_7bdeb0/ovl_169c_a_c_c_b.s needs two lines respelled:
 *
 *     line  21:   ldr  r3, =0x5e   ->   ldr  r3, =_AREA_5e
 *     line 163:   ldr  r3, =0x5f   ->   ldr  r3, =_AREA_5f
 *
 * area.sym:86-87 define `_AREA_5e = 0x5e;` / `_AREA_5f = 0x5f;` as absolute
 * symbols in stage1.o, so the linker resolves them to 0x5e / 0x5f and THE
 * SUBSTITUTION EMITS IDENTICAL BYTES.  Tracked precedent:
 * asm/overlays/rom_7b4558/ovl_30_c_c_a_a_b.s lines 44/46/48 carry
 * `.word _AREA_44` / `_AREA_45` / `_AREA_46`, and 226 files under src/ already
 * use `extern int _AREA_xx;` with `(int)&_AREA_xx`.
 *
 * WITHOUT the substitution objcmp reports `4 ENCODINGS ... in 4 place(s)` plus
 * `XX RELOCATIONS differ` (two extra ABS32 at 0x250 and 0x26c).  BOTH symptoms
 * are the SAME reference-side spelling artifact and neither is a code defect:
 * TWO OF THOSE FOUR "ENCODINGS" ARE POOL WORDS.  Measured both ways, against
 * the tracked file and against a corrected copy:
 *      tracked ref    4 place(s), relocations differ
 *      corrected ref  2 place(s), relocations CLEAN
 * so the park's true residue before this batch was TWO, not four.
 *
 * ====== B. HOW THE LAST TWO WENT, AND WHY THE OLD PARK COULD NOT ======
 * The residue was ONE ADJACENT TRANSPOSITION of the argument fill at the third
 * OvlFunc_934_2008528 site:
 *
 *     [218] ref 2304 movs r3, #4  | ours 2000 movs r0, #0
 *     [219] ref 2000 movs r0, #0  | ours 2304 movs r3, #4
 *
 * THE FIX IS ONE WORD:
 *
 *     extern void OvlFunc_934_2008528(...)  ->  extern int OvlFunc_934_2008528(...)
 *
 * THE PARK NAMED THE WRONG BLOCKER AND THEREFORE BOUNDED THE WRONG SEARCH.  It
 * measured 31 spellings at the site, concluded "THE TWO DEFECTS ARE MUTUALLY
 * EXCLUSIVE UNDER PINNING, which is the real finding and bounds the search", and
 * recorded r3 as being under a two-sided constraint.  It is not a pinning
 * problem at all: EVERY PIN AT THAT SITE IS INERT (see D) and the whole
 * 31-spelling sweep was in the wrong space.
 *
 * THE DEPENDENCE TABLE SAYS IT OUTRIGHT (sched2, basic block 26):
 *
 *     insn  code  bb  dep prio cost   units      INSN_DEPEND
 *     559   173    0   0   68    1    core  :   600 578 566      <- movs r0,#0
 *     565   173    0   2   68    1    core  :   600 575 566      <- movs r3,#4
 *
 * Identical priority, identical dependent count.  rank_for_schedule falls
 * through EVERY key to its last one, `INSN_LUID (tmp) - INSN_LUID (tmp2)`, so
 * ORIGINAL ORDER decides and 559 precedes 565 -- our wrong order exactly.  The
 * trace confirms it: `Ready list (t = 5): 557 565 559` then
 * `--> scheduling insn <<<559>>>` (haifa takes ready[n_ready-1], the LAST
 * printed).  SO THE "TRANSPOSITION" WAS A RANK TIE BROKEN ONE KEY TOO LATE.
 *
 * WHY THE RETURN TYPE BREAKS THE TIE.  Insn 578 (`r0 = 14`, the NEXT call's
 * first argument) carries an OUTPUT dependence on 559 ACROSS the intervening
 * call 566, because a call only CLOBBERS r0 and sched_analyze leaves
 * `reg_last_sets[r0]` pointing at 559 -- A CLOBBER NEVER BECOMES A LAST SETTER,
 * ONLY A SET DOES.  Declare the callee `int` and the call becomes a SET of r0,
 * 578 depends on 566 instead, and 559's dependent count drops 3 -> 2.  Now 565
 * (3) outranks 559 (2) on the DEPENDENT-COUNT key, the tie never reaches LUID,
 * and 565 is emitted first.
 *
 * ========== C. THE TREE ALREADY HELD THE EVIDENCE, IN A .c FILE ==========
 * src/overlays/rom_1300... -- specifically src/overlays/rom_7bdeb0/ovl_1300_c_c_a_c.c:63,
 * a LANDED file in THIS overlay -- declares
 *
 *     extern int OvlFunc_934_2008528(int a, int b, int c, int d, int e, int f);
 *
 * The old park's `extern void` CONTRADICTED a landed sibling.  Generalise the
 * cross-check: for an OVERLAY-LOCAL callee there is no header, so THE LANDED
 * .c FILES ARE THE ONLY DECLARATION AUTHORITY THAT EXISTS.  Grep them.
 *
 * The lever is SITE-SPECIFIC and must be measured per callee, never swept --
 * three of the nine void callees here are actively HARMFUL:
 *     OvlFunc_934_2008528 -> int      0   *** EXACT ***
 *     __Func_8010704      -> int     10      __Func_8092b08    -> int   6
 *     __MapActor_SetPos   -> int      5      2008528+8010704   -> int   8
 *     2008ba4 / __SetFlag / __WaitFrames / common0_70 / 2009770 -> int   2 (inert)
 *
 * ================== D. DEPINNED 15 -> 5, FIGURE HELD ==================
 * Five of the ten removed pins were released by the new baseline, which is the
 * brief's "re-run inert lists after a structural change" earning its keep.
 * Sites grouped by the value they materialise and removed as units:
 *     site 3 (q1=0xc,q2=0x10,q3=1)    drop -> 0      }  all three inert
 *     site 4 (q1=0x10,q2=0x10,q3=1)   drop -> 0      }  singly AND as the
 *     site 5 (q1=0xd,q2=0xf)          drop -> 0      }  {3,4} {3,5} {4,5} {3,4,5} group
 *     site 1 q2 only                  drop -> 0      }  and jointly with
 *     site 2 q0 only                  drop -> 0      }  each other
 * Sites 1 and 2 are otherwise load-bearing and the survivors are minimal under
 * single drops: site1 q0 3, site1 as plain ints 3, site1 shift-chain 3,
 * site2 q1 3, site2 q2 4, site2 q3 2, site2 as plain ints 11, dropping 1+2 7.
 *
 * SHIMS: 5 register pins (tools/shimcount.py), so this needs a fakematch.txt row:
 *     OvlFunc_934_2009984  src/overlays/rom_7bdeb0/ovl_169c_a_c_c_b.c
 *
 * MEASURED INERT, so the next reader does not repeat it: -fno-gcse, -fno-strict-
 * aliasing, -fno-caller-saves, -fno-peephole, -fno-force-mem, -fno-cse-follow-
 * jumps, -fno-cse-skip-blocks, -fno-thread-jumps, -fno-function-cse, -fno-defer-
 * pop, -fomit-frame-pointer, -fno-regmove, -fno-strength-reduce and nine more all
 * read 2 on the old body.  `-fno-dce`, `-fnew-ra` and `-fno-loop-optimize` DO NOT
 * EXIST in this cc1.
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
extern int OvlFunc_934_2008528(int a, int b, int c, int d, int e, int f);
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
            { register int q0 __asm__("r0"); register int q1 __asm__("r1");
              q0 = 8; q1 = 0xc6 << 18; __MapActor_SetPos(q0, q1, 0x8c << 17); }
            *(int *)(__MapActor_GetActor(8) + 0x6c) = (int)OvlFunc_934_2008cf8;
            break;
        case 8:
        case 9:
        case 10:
            { register int q1 __asm__("r1"); register int q2 __asm__("r2"); register int q3 __asm__("r3");
              q2 = 0x8a << 18; q1 = 0; q3 = 0x14; OvlFunc_common0_70(0x2820000, q1, q2, q3); }
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
            OvlFunc_934_2008528(0, 0xc, 0x10, 1, four, t);
            OvlFunc_934_2008528(0, 0xd, 0x10, 1, four, t);
        } else {
            OvlFunc_934_2008ba4(9);
        }
        if (__GetFlag(0x203) != 0) {
            four = 4;
            t = 0;
            OvlFunc_934_2008528(2, 0x10, 0x10, 1, four, t);
            OvlFunc_934_2008528(0, 0x10, 0x10, 1, four, t);
        } else {
            OvlFunc_934_2008ba4(0xa);
        }
        t = __GetFlag(0x205);
        if (t != 0) {
            OvlFunc_934_2008528(0, 0xd, 0x13, 4, 2, 0);
        } else if (__GetFlag(0x81 << 2) != 0) {
            OvlFunc_934_2008528(0, 0xd, 0xf, 4, 2, t);
            t = 0xe;
            __Func_8010704(0xe, 0x11, 2, 1, t, 0x10);
            __Func_8010704(0xe, 0xd, 1, 1, t, 0xf);
        } else {
            OvlFunc_934_2008ba4(0xb);
            __Func_8092b08(0xb, 3);
        }
    }
}
