/* OvlFunc_882_200c0f0  --  0x0200c0f0
 *
 * The whole of goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_c_c_c.s:
 * that .s holds ONE function, 146 instructions, no `.word` and no data
 * directives, so this lands as a WHOLE-FILE .c and needs no linker edit. The
 * single .ld line naming the object is
 *
 *     asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_c_c_c.o(.text)
 *
 * in overlays/rom_77dd1c/overlay.ld, and it stays as written: the Makefile's
 * cross-dir `asm/%.o: src/%.c` rule builds it from here. Default flag group
 * (GCC296_CFLAGS) -- no Makefile stanza, tryc.makefile_flags returns set().
 *
 * A straight-line map-edit script: eight `__CopyMapTiles` rectangles, seven
 * `__Func_8010704` attribute rectangles, one `__Func_800fe9c` at the end. Every
 * call takes six arguments, so every call has a stack-argument pair, and the
 * whole function is the stack-arg-pair lever fifteen times over.
 *
 * NOT A LENGTH TELL, EVERY TIME. Written with all thirty stack arguments as
 * bare literals the residue is 143 instructions against 146 -- three short --
 * but that headline hides three different failures, and only ONE of them is a
 * length problem. Naming all fifteen pairs is byte-exact; the sweep below
 * shows only four names survive, and INERT SCAFFOLDING MUST NOT SHIP.
 *
 * ELEVEN OF THE FIFTEEN SITES NEED NO NAME AT ALL, and the reason is the
 * recorded discriminator in `§Stack-argument materialisation` reaching further
 * than its three bullets. Sites 3, 7 and 8 pass the SAME value in both slots
 * (7,7 and 1,1) -- one register, matches. Sites 1, 4, 5, 6, 11, 13, 14 and 15
 * pass values that are ALSO used at a later site, so gcc's own CSE has already
 * put them in two distinct callee-saved registers before the call; there is
 * nothing left to materialise and its one-register-recycle habit never shows.
 *
 * NEW -- THE "ALREADY IN A REGISTER" BULLET HAS A HIGH-REGISTER EXCEPTION.
 * `§Stack-argument materialisation` records "the first already in a register (a
 * variable) -- matches; gcc has nothing to move, so its habit never shows".
 * Sites 9, 10 and 12 are exactly that shape and they FAIL. The reason is that
 * the value is in r8/r9/r10: thumb `str` cannot take a high register, so gcc
 * must copy it DOWN to a low one first, and that copy is a materialisation like
 * any other --
 *
 *     rom    mov r2, r9 / mov r3, #0x4b / str r2, [sp] / str r3, [sp, #4]
 *     ours   mov r3, r9 / str r3, [sp]  / mov r3, #0x4b / str r3, [sp, #4]
 *
 * The cure is HALF the usual lever: name only the operand that is still a
 * literal. `n9`, `n10` and `n12` below get their own pseudo, which pushes the
 * copy-down into r2 and defers both stores past both moves. Naming the OTHER
 * half instead is worse at all three sites (3, 8, and three instructions short),
 * because the high-register copy is placed by reload and no source order
 * reaches it -- which is the boundary `§gcc-2.96 emits the two movs in source
 * order` already states for `mov r8, rN` copies.
 * (Grepped first as "already in a register", "high register ... stack slot",
 * "copy ... high register ... low", "mov from a high" -- no hit.)
 *
 * NEW -- A STACK-ARG PAIR SPLIT ACROSS ONE LOW AND ONE HIGH REGISTER IS A
 * ROLE TELL, NOT A LENGTH TELL, AND ASSIGNMENT ORDER DECIDES THE ROLES.
 * Site 2 is the only site needing both halves named, and dropping it costs
 * ZERO instructions: 146 against 146, with 0xe and 0xa swapped between r6 and
 * r8. Both values are CSEd for site 5, so both get callee-saved registers --
 * but one is LOW (r6) and one is HIGH (r8), and unnamed gcc gives the HIGH one
 * to the FIRST stack argument while the ROM gives it to the second. Writing
 * `m2 = 0xe; n2 = 0xa;` in that order reproduces the ROM's split; writing the
 * assignments in the other order does not (5 differing). `§When two locals are
 * initialised to constants in the same basic block` says source order stops
 * mattering "once the values reach an argument list (the pooled-load rule
 * wins)"; a stack-argument pair is an argument list where it still decides.
 * (Grepped as "role inver", "swapped the two", "which value gets which
 * register", "assignment order decides", "same length but" -- no hit.)
 *
 * ONE NAME SETTLED THREE DISTANT SITES. Site 12's pair is the only one whose
 * removal is a real length tell (320 bytes against 324), and what it buys is
 * not local: it fixes the 8-in-r8 / 9-in-r6 / 0x49-in-r5 assignment across
 * sites 6, 10, 11, 13 and 14 at once. Recorded as `§ONE SCRATCH-REGISTER PIN
 * CAN SETTLE TWO DISTANT CLUSTERS`; this is a third cluster on one name.
 *
 * MEASURED (objcmp, ref 146 encodings / 324 bytes):
 *
 *     all thirty stack arguments as literals        320 B, 143 enc, 107 differ
 *     all fifteen pairs named                       EXACT (11 names inert)
 *     the four below                                EXACT
 *     ... minus site 2's pair                       324 B, 146 enc,   7 differ
 *     ... site 2, only m2 named                     324 B, 146 enc,   7 differ
 *     ... site 2, only n2 named                     324 B, 146 enc,   5 differ
 *     ... site 2, n2 assigned before m2             324 B, 146 enc,   5 differ
 *     ... minus site 9's n9                         324 B, 146 enc,   3 differ
 *     ... site 9, the e-operand named instead       324 B, 146 enc,   3 differ
 *     ... minus site 10's n10                       324 B, 146 enc,   8 differ
 *     ... site 10, the e-operand named instead      324 B, 146 enc,   8 differ
 *     ... minus site 12's n12                       320 B, 143 enc,  93 differ
 *     ... site 12, the e-operand named instead      320 B, 143 enc,  93 differ
 *
 * The drop sweep was run cumulatively and restarted from site 1 after every
 * successful drop, so each of the four names below was re-tested against the
 * reduced form and is load-bearing there.
 *
 * tools/solved_twins.py: no twin (0 hits across 2082 solved shapes).
 */
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);

void OvlFunc_882_200c0f0(void)
{
    int m2, n2;
    int n9, n10, n12;

    __CopyMapTiles(0x10, 0x60, 0xb, 0x49, 6, 3);
    m2 = 0xe;
    n2 = 0xa;
    __CopyMapTiles(0x10, 0x60, 0x22, 0x44, m2, n2);
    __CopyMapTiles(0x10, 0x60, 0x40, 0x44, 7, 7);
    __CopyMapTiles(9, 0x5f, 0xb, 0x49, 6, 3);
    __CopyMapTiles(0x28, 0x5e, 0x22, 0x44, 0xe, 0xa);
    __CopyMapTiles(0x36, 0x5e, 0x40, 0x44, 8, 7);
    __CopyMapTiles(0x48, 0x4b, 0x48, 0x4c, 1, 1);
    __CopyMapTiles(0x48, 0x4b, 0x4a, 0x4c, 1, 1);
    n9 = 0x4b;
    __Func_8010704(7, 0x4b, 1, 1, 6, n9);
    n10 = 0x47;
    __Func_8010704(8, 0x46, 3, 1, 8, n10);
    __Func_8010704(8, 0x46, 2, 1, 9, 0x48);
    n12 = 0x49;
    __Func_8010704(8, 0x46, 2, 1, 9, n12);
    __Func_8010704(0xb, 0x42, 1, 1, 8, 0x49);
    __Func_8010704(0xc, 0x42, 1, 4, 0xb, 0x49);
    __Func_8010704(0x19, 0, 1, 1, 6, 0x4a);
    __Func_800fe9c();
}
