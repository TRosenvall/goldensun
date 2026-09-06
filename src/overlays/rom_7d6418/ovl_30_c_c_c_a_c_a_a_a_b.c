/* OvlFunc_951_20084bc  --  0x020084bc
 *   [asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c_a_a_a.s, 2nd of 3 -- SPLIT REQUIRED]
 *
 * 384 instructions of straight-line cutscene script: one basic block, no
 * labels, no branches, 97 calls.  Built at the tree default -O2 -- no Makefile
 * pattern rule matches rom_7d6418/*, so `asm/%.o: src/%.c` applies and objcmp
 * prints no `(built with: ...)` line from either path.  THE MATCH DOES NOT
 * DEPEND ON A FLAG GROUP.
 *
 * THE WHOLE FUNCTION IS THREE VALUES AND ONE OF THEM IS IN A HIGH REGISTER.
 * 29 __CopyMapTiles(x, y, 0x46, 0, 3, 8) sites carry two STACK arguments, so
 * both constants need a register to be stored FROM.  The ROM holds them for the
 * whole function -- 3 in r6, 8 in r8 -- and the parameter in r5.  r8 is not
 * free in Thumb: every one of the 29 sites pays `mov r3, r8` before its
 * `str r3, [sp, #4]`, and entry/exit pay `mov r6, r8 / push {r6}` and
 * `pop {r3} / mov r8, r3`.  Those 29 copies plus the one `mov r8, r3` setup ARE
 * the entire length gap of the plain spelling.
 *
 * READ THE PROLOGUE BY CONTENT.  `push {r5, r6, lr}` + `mov r6, r8 / push {r6}`
 * + `sub sp, #8` is the same shape the rom_7f148c/ovl_30_c_c_c_a_a_c_b
 * template landed on, and it means the same thing: r5/r6 hold nothing but the
 * call-script's held values and `sub sp, #8` is the outgoing-argument area for
 * six-argument calls.  The template's 8-symbol score bought the IDIOM -- named
 * stack arguments, uninitialised `PIN` blocks, "do not transcribe the ROM's
 * emitted order" -- and nothing else; the residue here is a different class.
 *
 * THE HIGH-REGISTER ASSIGNMENT IS NOT REACHABLE FROM PLAIN C, AND THE
 * ARITHMETIC SAYS WHY.  gcc puts the same THREE values in the same THREE
 * registers -- it just deals them the other way round:
 *
 *      value            ROM     gcc (any plain spelling)
 *      slot (2 uses)    r5      r8
 *      3    (29 uses)   r6      r5
 *      8    (29 uses)   r8      r6
 *
 * Callee-saved allocation order is r5, r6, r8, ... here: r4 is call-USED under
 * the tree's `-fcall-used-r4`, and r7 is already out of the pool (`-ffixed-r7`
 * is measured INERT below, which is the proof).  So the deal is decided purely
 * by `allocno_compare`'s priority, floor_log2(n_refs)*n_refs/live_length.  The
 * two constants are born at the 7th call and die at the last (~350 insns) with
 * ~30 refs each: 4*30/350.  `slot` is born at insn 4 and dies at the
 * __Func_8091a58 call (~210 insns) with 3 refs: 1*3/210 -- eighty times
 * smaller, so it is dealt LAST and takes r8.  For `8` to be dealt last instead
 * it would need n_refs <= 3, and it is stored at 29 sites.  Nothing the source
 * can say moves it.
 *
 * MEASURED, so the search is bounded.  Every one of these is 355 lines against
 * the ROM's 384 -- exactly 29 short -- and 336 differing at the object level:
 *
 *   both constants as bare literals                     355   (349 differing)
 *   `int three = 3; int eight = 8;`                      355
 *   the same two declared in the other order             355
 *   only one of the two named (either one)               355
 *   the parameter copied to a local first                355
 *   -fno-gcse / -fno-rerun-cse-after-loop                355
 *   -fno-expensive-optimizations / -fno-caller-saves     355
 *   -fno-schedule-insns2 / -O1                           355
 *   -ffixed-r7                                           355  <- ENTIRELY INERT
 *   -fcall-saved-r4                                      351  (r4 taken, no
 *                                                              high reg at all)
 *   -ffixed-r5  /  -ffixed-r6                            388  (TWO high regs:
 *                                                              r8 AND r10)
 *
 * `-ffixed-r7` being inert is the load-bearing measurement: the recorded
 * `-ffixed-r7` lever ("the ROM saves a high register and your version does
 * not, and the gap is about four") does NOT apply, because our version already
 * saves r8.  This is that lever's near neighbour and its counter-example --
 * SAME register set, WRONG deal -- and no flag reshuffles a deal.
 *
 * SO THE PIN IS THE LEVER, AND IT IS NOT SCAFFOLDING.
 *
 *     register int eight __asm__("r8");      -- declared, NOT initialised
 *
 * takes the screen from 355 lines / 336 differing to 384 / 0.  Removing it is
 * the whole gap again (m8: 355 lines, 336 differing).  The tree's standing
 * warning that "a pin can be a symptom of a different defect" -- the alias-set
 * entry where r8 and r10 pins evaporated -- was checked and does not fire here:
 * there is no aliasing in this function at all, and the twelve spellings and
 * eleven flag settings above all tie at the identical wrong number.
 *
 * BOTH KNOBS OF THE PIN ARE USED, AND THAT IS THE RECORDED RULE VERBATIM.
 * "An uninitialised pin moves its assignment": the declaration's position sets
 * the register, the assignment's position sets when the value is materialised.
 * The ROM materialises 8 at the SEVENTH call, not at entry:
 *
 *   `register int eight __asm__("r8") = 8;`          385 lines, 377 differing
 *   pin split, both assignments hoisted to entry     384 lines, 377 differing
 *   pin split, assigned before the first CopyMapTiles  EXACT
 *
 * The initialiser form is one instruction LONG, and the extra instruction is
 * `mov r3, #8` hoisted above `sub sp, #8` -- the pin's `mov` pinned to the
 * declaration point, exactly as the rule predicts.
 *
 * `three` NEEDS THE SAME SPLIT, AND THE TWO ASSIGNMENTS ARE ORDERED.
 * This is the counterpart measurement and it is worth stating separately,
 * because `three` is NOT pinned -- it is an ordinary `int` and the split still
 * decides 33 encodings:
 *
 *   `int three = 3;` at the declaration               33 differing
 *   `int three;` + `three = 3;` before the first site  EXACT
 *   no `three` at all, literal 3 at all 29 sites       2 differing
 *   `eight = 8; three = 3;`  (order swapped)           2 differing
 *
 * So the ROM's `mov r6, #3` sits where the source's assignment sits, and 3 is
 * born BEFORE 8 -- which is also what the ROM's first block reads as
 * (`mov r3, #8 / str r3, [sp, #4] / mov r6, #3 / mov r8, r3`: the 8 is stored
 * straight from r3 and only THEN copied to r8, so its pseudo is the later one).
 * Declaration order of the two locals is INERT -- both orders are exact.
 *
 * ONE ARGUMENT PIN, AND THE TEARDOWN SAYS WHICH HALF.  The last two encodings
 * are __Func_8091a58(slot, 0), where the ROM fills DESCENDING:
 *
 *   rom   mov r1, #0 / mov r0, r5      ours  mov r0, r5 / mov r1, #0
 *
 *   unpinned                                           2 differing
 *   `int z = 0;` named, at the site or at the top      2 differing
 *   r0 pinned alone                                    2 differing
 *   r0 + r1 pinned (PIN2, descending)                  EXACT
 *   **r1 pinned alone**                                **EXACT** -- shipped
 *
 * "Anchor the argument that participates, and let the teardown find which one
 * that is": the r0 pin is inert and does not ship.  The r1 pin is equally exact
 * with or without its initialiser, so it keeps the initialiser and reads as one
 * line.  __Func_808f1c0(slot, 3) four lines earlier fills ASCENDING and needs
 * nothing -- same function, adjacent sites, opposite requirements, which is the
 * recorded "the rule belongs to the site, not the callee".
 *
 * DO NOT TRANSCRIBE THE ROM'S EMITTED ORDER -- confirmed again.  The 15th
 * __CopyMapTiles is the one block of the 29 whose ROM order is transposed
 * (`mov r2, #0x46 / mov r3, #0 / mov r1, #0x1d / mov r0, #0x64` against the
 * other 28 blocks' `r1, r2, r3, r0`).  It is written identically to the other
 * 28 and sched2 produces the transposition on its own.  Uniform ascending is
 * CORRECT, not merely cheaper: it is simultaneously right at 29 of 29.
 *
 * NOT A LOOP.  The 29 sites walk 0x4f..0x64 by 3 up two rows and back down, and
 * the temptation is to write two `for`s.  The ROM has NO branch and NO label in
 * 384 instructions, so the source is longhand; a loop cannot produce this.
 *
 * `tools/solved_twins.py` reports ZERO twins over 2133 solved shapes, and the
 * 31 r8-r11 uses put this at the worst end of the recorded tractability table
 * (27 uses = abandoned, 17 = parked, 0 = elevated first try).  BOTH SIGNALS ARE
 * WRONG HERE, and for one reason worth recording: that table counts high-register
 * REFERENCES, and 30 of these 31 are the SAME reference -- one pseudo's reload
 * copy, repeated once per call site.  A count of distinct high-register VALUES
 * would have said 1.  High-register traffic predicts trouble when it means
 * PRESSURE; here it means one deliberately-placed constant, and the function
 * fell in twelve screens.
 *
 * LANDING.  The `.s` holds THREE functions and this elevation takes the SECOND,
 * so the file must be split three ways before the .c can land.
 * `asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c_a_a_a.o` is 0x720 bytes at
 * 0x020081d8:
 *
 *   OvlFunc_951_20081d8   0x020081d8   0x2e4
 *   OvlFunc_951_20084bc   0x020084bc   0x3c4   (964, the size objcmp matches)
 *   OvlFunc_951_2008880   0x02008880   0x078
 *
 * No suffix of the stem is taken in either asm/ or src/ under rom_7d6418 (the
 * directory holds ovl_30_c_c_c_a_c_a_a_a, _a_a_b, _a_b and _a_c, none of which
 * collide), so the pieces are `_a` (residual .s), `_b` (this .c) and `_c`
 * (residual .s).  ONE tracked linker line names the .o, by full path:
 *
 *   overlays/rom_7d6418/overlay.ld:28
 *     asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c_a_a_a.o(.text)
 *
 * and it becomes three, in address order: `..._a.o(.text)`, `..._b.o(.text)`,
 * `..._c.o(.text)`.  That is the ONLY edit.  This overlay's `.data` and `.bss`
 * output sections (overlay.ld:41-42 and :45-46) name exactly one object,
 * `asm/overlays/rom_7d6418/ovl_30_c_c_c_c_a.o`, a DIFFERENT file -- our .o is
 * named in NO other section, so there is no zero-size `.data` line to remap.
 * The six further hits in `overlays/rom_7d6418/overlay.map` -- the
 * `.data`/`.bss`/`.ARM.attributes` triplet at lines 52-55, the
 * `.text 0x020081d8 0x720` placement at 155-156, and `LOAD` at 298 -- are NOT
 * edits: `git ls-files overlays/rom_7d6418/` lists only exports.s, imports.s,
 * overlay.ld and ovl_30.s, so overlay.map is an untracked build output
 * (Makefile `-Map $(<:.ld=.map)`) and regenerates with three of each triplet.
 * The basename `ovl_30_c_c_c_a_c_a_a_a` ALSO exists under overlays/rom_7a37f0/
 * with several `_a_*`/`_b`/`_c_*` children; every match above was taken on FULL
 * PATH for that reason.
 *
 * All three functions keep external linkage (`.thumb_func_start` emits
 * `.global`), so the split is link-neutral: OvlFunc_951_20084bc is called from
 * asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c_a_c.s:288 and resolves inside the
 * overlay.  OvlFunc_951_2008880 -- the third piece -- is already PARKED at
 * src/non_matching/ovl_7d6418/2008880.c and is untouched by this split; when it
 * is elevated it replaces `..._c.s` with `..._c.c` and the overlay.ld line
 * needs no further change.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_808f1c0(int a, int b);
extern void __Func_8091a58(int a, int b);

void OvlFunc_951_20084bc(int slot)
{
    register int eight __asm__("r8");
    int three;

    __CutsceneStart();
    __CutsceneWait(0x1e);
    __PlaySound(0x94);
    __CutsceneWait(0x64);
    __Func_8092adc(0, 0xc0 << 8, 0);
    __CutsceneWait(0x28);
    three = 3;
    eight = 8;
    __CopyMapTiles(0x52, 0x14, 0x46, 0, three, eight);
    __CutsceneWait(3);
    __CopyMapTiles(0x55, 0x14, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x58, 0x14, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x5b, 0x14, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x5e, 0x14, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x61, 0x14, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x64, 0x14, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x4f, 0x1d, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x52, 0x1d, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x55, 0x1d, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x58, 0x1d, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x5b, 0x1d, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x5e, 0x1d, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x61, 0x1d, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x64, 0x1d, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(0x46);
    __PlaySound(0x7e);
    __Func_808f1c0(slot, 3);
    { register int q1 __asm__("r1") = 0;
      __Func_8091a58(slot, q1); }
    __CutsceneWait(0x14);
    __CopyMapTiles(0x61, 0x1d, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x5e, 0x1d, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x5b, 0x1d, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x58, 0x1d, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x55, 0x1d, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x52, 0x1d, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x64, 0x14, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x61, 0x14, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x5e, 0x14, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x5b, 0x14, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x58, 0x14, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x55, 0x14, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x52, 0x14, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CopyMapTiles(0x4f, 0x14, 0x46, 0, three, eight);
    __PlaySound(0x9a);
    __CutsceneWait(8);
    __CutsceneEnd();
}
