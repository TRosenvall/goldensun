/* Func_80b0574  --  0x080b0574
 *
 * Whole of goldensun/asm/rom_b0000/rom_b0070_a_a_c_a_c_a_c.s (ONE function, no
 * data of any kind -- see LANDING below).
 *
 * The .s header calls it "As Func_b04dc but returning the player's selection".
 * The first half of that is exactly right and is what solved this in one
 * spelling; the second half is WRONG and the epilogue says so.
 *
 * THIS IS A FAMILY MEMBER, NOT A NEW READING. src/rom_b0000/rom_b0070_a_a_c_a_c_a_b.c
 * (Func_80b04dc) is the sibling cut from the same parent .s, and its body is
 * this body minus a save/restore. Everything below that could have been a
 * blocker -- the runtime constant subtraction, the two different signed-byte
 * reads, the loop rotation -- is already settled there and was transplanted
 * verbatim. tools/solved_twins.py reports 0 twins because it matches whole
 * shapes and this function has four extra instructions; the sibling was found
 * by grepping the corpus for the IDIOM (`0xc9b`) instead. Worth remembering
 * that a twin miss is not a family miss.
 *
 * THE RUNTIME SUBTRACTION, restated because it is the whole reading. The ROM
 * does, three times:
 *
 *     ldr r3, =0xcc6 / ldr r2, =0xc9b / sub r3, r2 / add r5, r3
 *
 * two pool loads and a subtract to compute 0x2b, a value that fits `add rN,
 * #imm8`. gcc folds literal arithmetic, so these cannot be integers; they are
 * the difference of two absolute SYMBOL addresses, which gcc has no way to
 * fold. See docs/elevation.md "Technique: stopping a constant fold with symbol
 * addresses". All four ids were already in message.sym -- NO .sym ADDITION IS
 * NEEDED for this function, unlike its sibling which had to add _MSG_cf1.
 *
 * The three `ldr r2, =0xc9b` are three separate loads, not a CSE failure worth
 * chasing: each subtraction sits in its own conditional block, so the value is
 * not available on all paths into the next one.
 *
 * FIVE CALLEE-SAVED REGISTERS, FIVE VARIABLES. The prologue reads, by CONTENT:
 * push {r5,r6,r7,lr} plus r8 and r10 through the r6/r7 shuttle. That is exactly
 * the five values whose live ranges cross a call -- msg (r5), s (r6), the
 * s+0x380 slot address (r7), voice (r8), old (r10) -- and it is what confirmed
 * `old` has to be a named local rather than a re-read.
 *
 * THE EPILOGUE POPS INTO r0, SO THE FUNCTION IS `void`. This is the oracle from
 * the template neighbour src/rom_8a000/rom_92950_c_c_c_c_a.c, and here it
 * directly contradicts the disassembly's own header comment about "returning
 * the player's selection". The comment is a human note on an unlinked listing;
 * the pop is the compiler. Trust the pop. (Contrast docs/elevation.md "Tell:
 * `pop {r1}` in a function that looks void names a RETURN VALUE" -- r0 is the
 * void case, r1 the value case.)
 *
 * NOTE ON THE TEMPLATE. rom_8a000 is not an overlay and the brief warned to
 * treat its cutscene-script idiom with suspicion. Correctly so: only the `void`
 * oracle above survived the crossing. The template's message-id `++` idiom, its
 * unprototyped `GetSpriteVoice()`, and its `iwram_3001ebc` base are all absent
 * here -- this bank calls `_GetSpriteVoice` (leading underscore, a different
 * symbol) off `iwram_3001f2c`.
 *
 * WHY THE 0x3a4 OFFSET IS SPELT `0xe9 << 2`. The ROM builds it as 0x380 + 0x24:
 *
 *     mov r2, #0xe0 / lsl r2, #2   ; 0x380, used for s + 0x380
 *     add r2, #0x24                ; 0x3a4, reused for s + 0x3a4
 *
 * That `add r2, #0x24` is gcc's OWN constant arithmetic (docs/elevation.md "An
 * add/sub chain on a constant may be gcc's OWN arithmetic"), not a source-level
 * split. Both offsets are written as the natural `<< 2` forms and gcc finds the
 * reuse itself; nothing had to be hoisted or named to get it.
 *
 * TWO SIGNED BYTE READS, TWO DIFFERENT SEQUENCES, from the sibling. +0x3a9 is
 * `ldrsb` through a register offset (ldrsb has no immediate-offset form, hence
 * the `mov r1, #0`) because it is read straight into an `int`; +0x3ac is `ldrb`
 * plus an explicit `lsl #24 / asr #24` because it lands in a `signed char`
 * LOCAL first. Swapping the two spellings costs two instructions and nothing
 * else, which is what makes it easy to misread as noise.
 *
 * THE SAVE/RESTORE IS THE ONLY THING THIS ADDS TO THE SIBLING: the byte at
 * (*(s + 0x380))[5] is read out, forced to 0xd across the call, and put back.
 * The address expression is written THREE TIMES verbatim. gcc CSEs the first
 * two into r7 and REBUILDS the third after the loop -- the loop's join point
 * ends the extended basic block cse_main was working in, so the address is no
 * longer available. Do not try to "help" that by naming a pointer local; the
 * ROM's rebuild is the compiler's, not the source's.
 *
 * Also note `_Func_8019a54()` moves: the sibling calls it immediately after
 * _GetSpriteVoice, this one calls it AFTER the three message adjustments and
 * after the 0xd store. Position is load-bearing, so it was read off the ROM
 * rather than carried over with the rest of the body.
 *
 * MATCHED ON THE FIRST SPELLING -- there is no measured-worse table for this
 * function because no lever was ever swept. Everything above was read off the
 * reference and off the sibling before compiling once.
 *
 * VERDICT / HOW IT WAS VERIFIED -- READ THIS BEFORE RE-SCREENING.
 *
 *   objcmp does NOT print its OK line for this function, and it is still
 *   byte-identical. It reports:
 *
 *     XX ENCODINGS differ in 4 place(s) (ref 84, ours 80)
 *     XX RELOCATIONS differ        [4 extra R_ARM_ABS32 on the _MSG_* words]
 *
 *   The recorded objcmp false negative (docs/elevation.md "objcmp CAN REPORT A
 *   BYTE-IDENTICAL MATCH AS FAILING") says the tell is SIZE *and* ENCODINGS
 *   silent. That tell is too strict and does not cover this case -- see the NEW
 *   FINDING below. Proof of identity, symbols resolved at ASSEMBLY time so
 *   codegen is untouched (scratch_elev/b237/f80b0574/verify.sh):
 *
 *     ref  bytes: 192      cand bytes: 192      BYTE-IDENTICAL
 *     sha1 835dde61df7d237f77c55854555ed6d3fe718afe  (both)
 *
 * NEW FINDING -- objcmp's ENCODINGS line fires on the SYMBOL-ADDRESS TECHNIQUE,
 * as a pure LENGTH delta, and the recorded tell does not cover it.
 *
 *   Grepped first as "objcmp", "false negative", "ENCODINGS", "relocation",
 *   "R_ARM_ABS32": the corpus records that an extra relocation refutes nothing
 *   ("THE RELOCATION CHECK CANNOT REFUTE A SYMBOL, ONLY CONFIRM A LITERAL") and
 *   that relocation-only complaints are false negatives. Neither says ENCODINGS
 *   can fire too. Mechanism, from objdump on both objects:
 *
 *     ref .o    a8: .word 0  ac: .word 0x3a9  b0..bc: .word 0xcc6/0xc9b/cf1/d4c
 *     cand .o   a8: .word 0  ac: .word 0x3a9  ...
 *
 *   Our four _MSG_* pool words are relocation PLACEHOLDERS, so they are all
 *   zero, and objdump ELIDES a run of identical words as a single `...` line.
 *   objcmp's ENC regex does not match `...`, so our list is 4 entries short and
 *   it reports 4 differing "places" that do not exist.
 *
 *   THE DIAGNOSTIC IS THE MISSING "first at index" LINE. objcmp prints that
 *   line only when a ZIPPED PAIR differs; a pure length delta prints the count
 *   and nothing else. So:
 *
 *   > "ENCODINGS differ in N place(s)" with NO "first at index" line means the
 *   > streams agree everywhere they overlap and the whole delta is length. On a
 *   > candidate using symbol addresses, check for an elided zero run before
 *   > believing it.
 *
 *   It needs a run objdump will elide, so it bites functions with THREE OR MORE
 *   symbol pool words and not the one- and two-symbol cases -- which is
 *   probably why it has not been hit before now.
 *
 * LANDING. The .s holds ONE function (`grep -c thumb_func_start` = 1) and no
 * `.section`, `.global`, `.incrom`, `.incbin`, `.word`, `.byte` or `.hword` --
 * checked, because the template neighbour's own header records a link failure
 * from converting a whole .s that carried a rodata block. So this is a
 * WHOLE-FILE .c, it needs NO split and NO linker edit. stage1.ld already names
 * the object on exactly ONE line, cited by content:
 *
 *     asm/rom_b0000/rom_b0070_a_a_c_a_c_a_c.o(.text)
 *
 * which sits directly after `asm/rom_b0000/rom_b0070_a_a_c_a_c_a_b.o(.text)`,
 * the already-elevated sibling -- proof the cross-dir rule already builds
 * asm/<bank>/X.o from src/<bank>/X.c for this exact family.
 *
 * FLAG GROUP: NONE. This is the DEFAULT `%.o: %.c` rule with GCC296_CFLAGS
 * (-O2, -fcall-used-r4). objcmp printed no "(built with: ...)" line and neither
 * this file nor its sibling has a Makefile rule. So there is no green-screen /
 * red-build hazard here: no rule needs to be added.
 */

extern unsigned char *iwram_3001f2c;
extern int _MSG_c9b;
extern int _MSG_cc6;
extern int _MSG_cf1;
extern int _MSG_d4c;
extern int _GetSpriteVoice(int id);
extern void _Func_8019a54(void);
extern void _Func_8017658(int a, int b, int c, int d);
extern int _Func_8017364(void);
extern void WaitFrames(int n);

void Func_80b0574(int msg)
{
    unsigned char *s;
    int voice;
    int k;
    signed char t;
    int old;

    s = iwram_3001f2c;
    old = (*(unsigned char **)(s + (0xe0 << 2)))[5];
    voice = _GetSpriteVoice(*(unsigned short *)(s + (0xe9 << 2)));
    k = (signed char)s[0x3a9];
    if (k == 2)
        msg += (int)(&_MSG_cc6) - (int)(&_MSG_c9b);
    if (k == 0)
        msg += (int)(&_MSG_cf1) - (int)(&_MSG_c9b);
    t = s[0xeb << 2];
    if (t != 0)
        msg += (int)(&_MSG_d4c) - (int)(&_MSG_c9b);
    (*(unsigned char **)(s + (0xe0 << 2)))[5] = 0xd;
    _Func_8019a54();
    _Func_8017658(msg, 5, 0, (voice << 16) | 0x22);
    while (_Func_8017364() == 0)
        WaitFrames(1);
    WaitFrames(1);
    (*(unsigned char **)(s + (0xe0 << 2)))[5] = old;
}
