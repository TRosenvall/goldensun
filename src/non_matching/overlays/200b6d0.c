/* OvlFunc_969_200b6d0 -- 0x0200b6d0,
 * asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_a_a_c.s (1 function, so landing needs NO split).
 *
 * NOT MATCHING: ONE INSTRUCTION SHORT, and that one instruction is now the ONLY difference --
 * 46 encodings, down from 49 in batch 275. It MUST be built with -fno-rerun-cse-after-loop
 * (CSE_CFLAGS). Candidate below.
 *
 * WITHOUT THE FLAG it is 89 differing; with it, 49; with the new lever below, 46.
 *
 * THE FLAG IS NOT A NOVEL COST. This is the established flag-id constant-CSE class that the
 * CSE_CFLAGS group exists for (Makefile:414-470). Both `__GetFlag(0x236)` calls are bare literals;
 * at -O2 the second CSE pass hoists 0x236 into r6 and keeps it live across three calls where the
 * ROM re-loads the pool word. Confirmed PASS-level rather than source-level, exactly as
 * Makefile:424-437 records: five structural spellings of the guard (nested `if`, `&&`, `goto`,
 * `else if`) all give IDENTICAL output at 55 differing. -fno-cse-skip-blocks also fixes it;
 * -fno-gcse is inert; -fno-expensive-optimizations and -fno-cse-follow-jumps do not.
 *
 * ===== BATCH 276: RESIDUE 2 IS SOLVED, AND THE PARK'S OWN PRESCRIPTION IS RULED OUT. =====
 *
 * RESIDUE 2 FELL TO A LEVER THIS PARK DID NOT LIST: a named `int id = 0x8e << 1;` assigned
 * BEFORE THE FIRST GUARD. 49 -> 46, and the ROM's `ldr r1, [r5, #8] / ldr r3, [r5, #0x10] /
 * lsl r0, #1` now lands. THE POSITION IS LOAD-BEARING: the identical assignment placed
 * immediately before the call is back at 49. So this is the assignment-position lever, not a
 * naming one. The park's old note that "`0x11c` as a plain literal instead of `0x8e << 1` is
 * inert" is confirmed (49) but was testing the wrong axis -- the variable's BIRTHPLACE, not its
 * spelling, is what moved it.
 *
 * THE SINGLE-EXIT RESTRUCTURE WITH A `ret` LOCAL IS A MEASURED NEGATIVE HERE. It is what fixed
 * Func_80b153c's version of this class in batch 274, and this park named it as the thing to try
 * next. It does not reach this one, and it is much worse:
 *   `ok = 1; if (GetFlag == 0 && ...) ok = 0; if (ok) { ... }`      97 differing, 101 lines
 *   the nested-if form of the same                                   97 differing, 101 lines
 *   `ok` set only around the `q != 0` test                           59 differing,  99 lines
 * THE REASON IS THAT THIS FUNCTION IS `void`, confirmed from the epilogue: it does
 * `pop {r0} / bx r0`, not `pop {r1} / bx r1`, so there is no return value for the local to
 * carry, and every flag-variable form instead pays for itself in the top guard -- which
 * currently matches exactly. The lever needs a `ret` value to work on; Func_80b153c had one.
 *
 * RESIDUE 1 IS NOW THE WHOLE RESIDUE, AND IT IS PRICED OUT: the missing `mov r0, r7` before
 * `ldr r6, [r7, #0x50] / bl __Actor_SetScript`. Reload's find_equiv_reg deletes the copy because
 * r0 provably still holds the __CreateActor result at that point, so deleting it is CORRECT, and
 * every spelling that keeps `q` in a callee-saved register hits it. The 4-byte deficit is what
 * forces the ROM's extra `.short 0x0000` pool pad, so it is the only size difference too.
 *
 * TWENTY-FIVE SPELLINGS NOW MEASURED IDENTICAL -- the original nine, plus: early return 46;
 * `goto out` 46; two pointer variables 46; `e = q;` 46; `if (q)` 46; __CreateActor returning
 * `int` with a cast 46; __Actor_SetScript taking `void *` 46 and returning `int` 46; `s` via a
 * cast-through-`char *` 46; `&gScript[0]` 46; `zero` moved before the call 46; __Actor_SetScript
 * UNDECLARED so it is an implicit `int` call 46; BOTH callees undeclared 46.
 *
 * NINE FURTHER FLAGS MEASURED, none reaching it: -fno-cse-skip-blocks, -fno-cse-follow-jumps,
 * -fno-thread-jumps, -fno-delayed-branch, -fno-peephole, -fno-caller-saves and
 * -fno-function-cse all 46 -- and -fno-cse-skip-blocks was checked as a FULL DIFF, not just a
 * count, and is byte-identical. -fno-gcse 55/98, -fno-expensive-optimizations 53/95,
 * -fno-schedule-insns2 50: all worse.
 *
 * NEXT: nothing source-level and no flag in the existing groups. The class statement holds ("a
 * one-instruction residue at a call-result copy is a pressure symptom") but this instance has no
 * source-level pressure knob. Do not spend another round on spellings -- 25 are on file.
 *
 * TWO THINGS WORTH CARRYING:
 *   - the `.L37a4: .word 0` is NOT a symbol. `unsigned short z = 0;` byte-stored gives gcc's
 *     `ldrh r2, .L10 / .word 0` and reproduces it exactly -- the
 *     src/overlays/rom_7d6418/ovl_30_c_c_c_a_c_c.c mechanism, second specimen. Here it COEXISTS
 *     with a plain `int zero = 0` used for another byte store and a halfword store in the same
 *     function, which is the clearest demonstration yet that the two forms are different tools.
 *   - THE STATEMENT-BOUNDARY LEVER (docs/elevation.md:12587) produces the ROM's
 *     `mov r2, r0 / lsl r2, #8`: `t = __Random(); t <<= 8;` as two statements, then
 *     `t >>= 16; t <<= 16; t += e->f0c; t += 0xffe40000;`. One expression is 55 differing;
 *     all-compound is 49 and every shift and add in the chain lands.
 *
 * A TOOLING NOTE FROM THE RE-SCREEN: objcmp derives its cflags from the Makefile rule for the .c
 * path, so it CANNOT screen a park that needs CSE_CFLAGS and is not in the Makefile yet. Batch
 * 276 worked around it with a 7-line wrapper that monkeypatches tryc.makefile_flags to return
 * {"no-rerun-cse"} and then execs tools/objcmp.py unmodified. Worth building properly if another
 * CSE_CFLAGS park needs measuring.
 */
struct Sprite {
    unsigned char pad00[9];
    unsigned char b0 : 2,
                  b2 : 2,
                  b4 : 4;
    unsigned char pad0a[0x26 - 0xa];
    unsigned char f26;
};

struct Actor {
    unsigned char pad00[8];
    int f08;
    int f0c;
    int f10;
    unsigned char pad14[0x30 - 0x14];
    int f30;
    unsigned char pad34[0x50 - 0x34];
    struct Sprite *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    short f64;
    short f66;
    unsigned char pad68[0x6c - 0x68];
    void *f6c;
};

extern unsigned int iwram_3001e40;
extern unsigned char gScript_969__0200e16c[];

extern int __GetFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern int __Random(void);
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);
extern void __Func_80929d8(struct Actor *a, int n);
extern int __sin(int a);
extern void OvlFunc_969_200b600(struct Actor *a);

void OvlFunc_969_200b6d0(void)
{
    struct Actor *e;
    struct Actor *q;
    struct Sprite *s;
    unsigned int t;
    unsigned int r;
    int v;
    int zero;
    unsigned short z;
    int id;

    id = 0x8e << 1;
    if (__GetFlag(0x236) == 0) {
        if (iwram_3001e40 % 3 != 0)
            return;
    }
    e = __MapActor_GetActor(0x18);
    if (__GetFlag(0x236) != 0) {
        t = __Random();
        t <<= 8;
    } else {
        t = __Random();
        t <<= 6;
    }
    t >>= 16;
    t <<= 16;
    t += e->f0c;
    t += 0xffe40000;
    q = __CreateActor(id, e->f08, t, e->f10);
    if (q != 0) {
        s = q->f50;
        __Actor_SetScript(q, gScript_969__0200e16c);
        __Func_80929d8(q, 1);
        zero = 0;
        q->f55 = zero;
        v = 0xffff000 & __Random();
        *(short *)((char *)q + 0x64) = v;
        *(short *)((char *)q + 0x66) = zero;
        q->f6c = OvlFunc_969_200b600;
        z = 0;
        r = __Random();
        q->f30 = (__sin((r * 0xffff) >> 20) * 24) >> 16;
        s->f26 = z;
        s->b2 = 1;
    }
}
