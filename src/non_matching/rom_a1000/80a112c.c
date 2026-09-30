/* Func_80a112c -- 0x080a112c, asm/rom_a1000/rom_a1050_c_c_a.s (1 function, no data).
 *
 * NON-MATCHING, 18 of 411 encodings differ.  SIZE MATCHES (964 bytes) and the
 * INSTRUCTION COUNT MATCHES (411 = 411), so the count IS a distance here.
 * Relocation sequence is identical, symbol for symbol.  Zero shims.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a112c.c \
 *     asm/rom_a1000/rom_a1050_c_c_a.s --func Func_80a112c
 *   -> XX ENCODINGS differ in 20 place(s) (ref 411, ours 411)
 *      first at index 182
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py src/non_matching/rom_a1000/80a112c.c \
 *     asm/rom_a1000/rom_a1050_c_c_a.s Func_80a112c
 *   -> aligned-equal 394  (95.9% of ref)   differing/ins/del 19 in 15 hunks
 *
 * (objcmp reads 20 and aligncmp 19 because one hunk is a one-instruction
 * SCHEDULE swap -- an insert plus a delete -- which objcmp counts twice.)
 *
 * SINGLE-FUNCTION FILE: converting it deletes asm/rom_a1000/rom_a1050_c_c_a.s
 * outright, no split.  tools/datacheck.py reports no data; the `ldr r0, =.Laf20c`
 * is a REFERENCE to a .global label defined in rom_a1050_c_c_c_c_b.s, reached from
 * C with an __asm__ label alias (the src/rom_15000 house idiom).
 *
 * WHAT THIS IS.  The panel drawn beside every list in the item module.  r0 IS
 * DEAD -- the ROM never reads it; the box it draws into is re-read from
 * state->[0x24] instead.  r1 = character id, r2 = equipment slot, r3 = mode.
 * Bit 8 of the mode (0x100) suppresses the window; the low byte selects one of
 * five layouts through a 9-entry jump table (cases 0, 2, 3, 4, 6, 8; 1, 5 and 7
 * fall to the shared exit).  Returns nothing -- the epilogue is `pop {r0} / bx r0`.
 *
 * ==================== THE FOUR LEVERS THAT GOT IT HERE ====================
 * Measured from a first draft at 165 of 411 differing (aligncmp 64.7%):
 *
 *  1. A NAMED OFFSET VARIABLE IS WHAT PRODUCES THE REGISTER-OFFSET LOAD.
 *     `o = s2 + 0xd8; raw = *(unsigned short *)((int)u + o);` gives
 *     `add r3,#0xd8 / ldrh r3,[r0,r3]`.  Written inline as
 *     `*(unsigned short *)((int)u + (s2 + 0xd8))` gcc reassociates to
 *     `(u + s2) + 0xd8` and emits `add r3,r8 / add r3,#0xd8 / ldrh r3,[r3]`
 *     -- two extra instructions AND it stores the wrong value in the spill slot.
 *     Same lever fixed case 4's `u + (i*4 + 0x58)` loop load.  This is the
 *     80a355c park's `(int)r7 + ofs` idiom, confirmed on a second function.
 *
 *  2. NEW, AND IT IS THE BIG ONE: WHEN THE ROM RE-DERIVES AN OFFSET FROM A
 *     SPILLED BASE, THE SECOND OCCURRENCE MUST BE WRITTEN FROM THE *ORIGINAL*
 *     OPERAND, NOT FROM THE INTERMEDIATE.  The ROM stores `slot * 2` at sp+8 and
 *     adds 0xd8 to it twice, in two different blocks.  Spelling both as
 *     `s2 + 0xd8` lets gcse hoist the whole sum: the slot then holds
 *     `slot*2 + 0xd8`, the second `add #0xd8` disappears, and -- because the
 *     spilled value changed -- reload renumbers the prologue as well.  Spelling
 *     the second one `slot * 2 + 0xd8` instead leaves gcse only the `slot * 2`
 *     subexpression to common, which is exactly what the ROM shares.  This ONE
 *     EDIT moved the first divergence from index 8 to index 50 and took 76
 *     differing to 66.  A rename of the destination variable (o -> o2) does
 *     nothing; it is the RHS that has to differ.
 *
 *  3. WRITE THE DUPLICATED CALL OUT IN BOTH ARMS AND LET GCC CROSS-JUMP ONLY
 *     THE TAIL.  Case 4 ends in two arms that differ in one string id.  Hoisting
 *     the shared `_Func_801e7c0(item + 0x333, ...)` above the `if`, or routing
 *     the differing id through a variable, both make gcc share MORE than the ROM
 *     does (the ROM duplicates all six instructions of the first call and shares
 *     only the four of the second).  Writing both calls in both arms is what
 *     reproduces it.  Compare src/rom_b5000/rom_b6eb4_a.c's "the work goes INSIDE
 *     each case".
 *
 *  4. LEVERS 1 AND 3 ARE NOT SEPARABLE, AND EITHER ONE ALONE LOOKS WORSE.
 *     Case-4 loop offset alone: 44 differing.  Case-4 duplication alone: 42.
 *     BOTH: 20.  Applied to the pre-lever-2 base they each made things WORSE
 *     (118 and 98).  A probe that regresses is not evidence the lever is wrong
 *     -- it can be evidence the base is.  Re-run the probe after the next
 *     structural fix lands.
 *
 * ==================== THE RESIDUE, THREE CLUSTERS ====================
 *
 * (A) CASES 0 AND 6, 5 lines.  ROM: `ldrb r2,[r3] / ldr r3,=0x741 /
 *     adds r6,r2,r3` -- the byte goes to a register OTHER than the address
 *     register and the pool constant reuses the address register.  Ours keeps
 *     the byte in the address register.  FOUR STRUCTURALLY DIFFERENT SPELLINGS
 *     OF THE SAME ARM ALL READ EXACTLY 19/20: `b + 0x741`, `0x741 + b`,
 *     `t = b; v = t + 0x741`, and a named `base = 0x741`.  A four-way tie
 *     across different shapes; treat this one as a wall, not a spelling.
 *
 * (B) CASE 4's r0/r4 SWAP, 8 lines.  The ROM puts `item` in r4 and `found` in
 *     r0; we put `item` in r0 and `found` in r4.  Nothing else about the arm
 *     differs.  global.c's allocno_compare is
 *     `floor_log2(n_refs) * freq / live_length`: `item` has 4 refs
 *     (floor_log2 = 2) with one of them inside the loop, `found` has 3
 *     (floor_log2 = 1), so ours processes `item` first and it takes r0 -- r4
 *     going to whatever comes next, r4 being usable at all only because neither
 *     value crosses a call.  FOR THE ROM'S ORDER `found` NEEDS FOUR REFERENCES
 *     OR `item` THREE, and the arm has no fourth use of `found` to give it.
 *     Measured and REJECTED: declaration-order swaps of i/found/item (no
 *     change at all), reversing the loop compare (no change), indexing info as
 *     `((unsigned short *)info)[0x14]` (no change), reusing the outer `n` for
 *     `found` (55), reusing the outer `v` for `item` (52), both (394, and one
 *     instruction long).  The last two are the informative ones: a variable
 *     with an unrelated earlier life is strictly worse, so the ROM's `found`
 *     and `item` really are arm-local.
 *
 * (C) ONE SCHEDULE SWAP, 3 lines (2 by objcmp's double count).  In the second
 *     `_call_via_r3` the ROM issues `ldr r3,=Func_8001af8` BEFORE `mov r0,r8`;
 *     we issue it after.  Measured: dropping the second `copy = Func_8001af8;`
 *     and reusing the pointer from the first call -> 52 (much worse, the ROM
 *     reloads the pool constant at both sites).  Hoisting the assignment above
 *     `_CalcStats` -> 413 encodings, one instruction long.  The elevation.md
 *     lever "naming a call's ARGUMENT fixes the _call_via_rN register" does not
 *     reach this one: the register is already r3, it is only the ORDER.
 *
 * NOT A RESIDUE, and worth saying because it cost a pass to be sure: `box` sits
 * in r7 and the `buf` address in r6, matching the ROM, and that FELL OUT of
 * lever 1 -- the first draft had them swapped and no register-directed edit was
 * ever made.  Forty-plus hunks of `adds r1,r6` vs `adds r1,r7` vanished with a
 * change to an addressing expression.  Do not chase a callee-saved register
 * assignment until the addressing modes are right.
  *
 * BODY REPLACED IN BATCH 305e: 20 -> 18 of 411, count exact, relocations identical,
 * aligncmp 395 of 411 (96.1%), 18 in 14 hunks (was 19 in 15).  Shimcount clean.  The change
 * halves cluster C by reusing `n`, which already holds `0xa6 * 2` from Func_8004938(n), as
 * the second copy()'s length -- the register-inheritance lever, reading as natural C.
 *
 * WHY THAT LEVER CANNOT REACH CLUSTER B -- a reason, not two failed probes.  The register
 * map read off the asm shows every early local SPILLED TO THE STACK (`win` sp+20, `raw`
 * sp+12, `info` sp+16, `s2` sp+8); the only register-resident locals are hi->r5, u->r8,
 * id->r9, st->r10, mode->r11.  THERE IS NO r0 OR r4 DONOR IN THE FUNCTION, so the lever's
 * precondition is unsatisfiable -- which is why this header's `n` (55) and `v` (52) attempts
 * failed.  Do not retry donor hunting on cluster B.
 *
 * AND ONE PROBE THAT LOOKED LIKE A TEST IS NOT ONE.  Re-reading `item` from `info` in both
 * arms reads 20 with a residue IDENTICAL LINE-FOR-LINE to the baseline, because gcse commons
 * the re-read back -- an inert spelling, not evidence.  The genuine hoists all regress (fresh
 * local 150 and 2 insns short, into `v` 34, into `n` 30), and hoisting the `+ 0x333` triggers
 * the very call-sharing this park's lever 3 rejects.  So the "give `item` three references"
 * route is BLOCKED by lever 3, not merely unmeasured.
 *
 * CLUSTER A IS A WALL, WITH A MECHANISM: the ROM needs the byte in a scratch register
 * distinct from the sum (3-operand `add r6,r2,r3`); gcc coalesces the load into the
 * destination (2-operand `add r6,r3`) unless the loaded pseudo OUTLIVES the add, and the ROM
 * gives it no later use.  Three further spellings regress: shared byte temp 143, reusing
 * `raw` 270 (2 long), named offset local plus temp 151.
 * Remaining residue: cluster A 5, cluster B 11, cluster C 2.
*/

extern int iwram_3001f2c;
extern unsigned char Laf20c[] __asm__(".Laf20c");

extern char *_GetUnit(int id);
extern short *_GetItemInfo(int item);
extern int Func_80a10d0(void *p, int b, int c, int d, int e, int f);
extern void WaitFrames(int n);
extern void _Func_80164d4(void *box, int a, int b, int c, int d);
extern void _Func_801e8b0(void *s, void *box, int x, int y);
extern void Func_80a8b10(void *buf, int a, int id);
extern void _Func_801e7c0(int id, void *box, int x, int y);
extern void _Func_801ea08(int v, int n, void *box, int x, int y);
extern void _Func_80164ac(void *box);
extern void *_Func_801ec6c(int a, int b, int c, void *box, int e, int f);
extern void Func_80a153c(char *u, void *box);
extern void *Func_8004938(int size);
extern void Func_8001af8(void *dst, void *src, int len);
extern int _CanEquipItem(int unit, int item);
extern void _EquipItem(int unit, int slot);
extern void _CalcStats(int unit);
extern void Func_80a15f0(char *u, void *old, void *box);
extern void free(void *p);

void Func_80a112c(int arg0, int id, int slot, int mode)
{
    unsigned char buf[8];
    int win;
    short *info;
    int raw;
    int s2;
    char *st;
    void *box;
    char *u;
    int hi;
    int hi2;
    int n;
    int v;
    int base;
    int o;
    int o2;
    int o3;
    int i;
    int found;
    int item;
    void *old;
    void (*copy)(void *, void *, int);

    win = 0;
    st = (char *)iwram_3001f2c;
    u = _GetUnit(id);
    s2 = slot * 2;
    o = s2 + 0xd8;
    raw = *(unsigned short *)((int)u + o);
    info = _GetItemInfo(raw & 0x1ff);
    hi = 0x80 * 2;
    hi &= mode;
    if (hi == 0)
        win = Func_80a10d0(st + 0x24, 0, 5, 0xd, 0xc, 0x102);
    box = *(void **)(st + 0x24);
    if (hi == 0) {
        if (win == 0) {
            WaitFrames(1);
            _Func_80164d4(*(void **)(st + 0x24), 0, 0, 0x58, 0x20);
        }
        _Func_801e8b0(u, box, 0x20, 0);
        Func_80a8b10(buf, 1, id);
        n = 0;
        if (buf[1] != 0) {
            _Func_801e7c0(0xbd6, box, 0x20, 8);
            n = 1;
        }
        if (buf[2] != 0) {
            _Func_801e7c0(0xbd7, box, 0x20, n * 8 + 8);
            n++;
        }
        if (buf[3] != 0) {
            _Func_801e7c0(0xbd8, box, 0x20, n * 8 + 8);
            n++;
        }
        if (buf[4] != 0) {
            _Func_801e7c0(0xbd9, box, 0x20, n * 8 + 8);
            n++;
        }
        if (n <= 1) {
            v = *(unsigned char *)(u + 0xf);
            _Func_801e8b0(Laf20c, box, 0x28, 0x10);
            _Func_801ea08(v, 4, box, 0x38, 0x10);
        }
    }
    if (win == 0) {
        WaitFrames(1);
        _Func_80164d4(*(void **)(st + 0x24), 0, 0x20, 0x58, 0x50);
    }
    _Func_80164ac(box);
    hi2 = 0x80 * 2;
    hi2 &= mode;
    if (hi2 == 0)
        *(void **)(st + 0xbe * 2) = _Func_801ec6c(id, 0, 0, box, hi2, hi2);
    switch (mode & 0xff) {
    case 0:
        v = *(unsigned char *)(u + 0x129);
        v += 0x741;
        _Func_801e7c0(v, box, 0, 0x20);
        Func_80a153c(u, box);
        v = *(int *)(u + 0x92 * 2);
        _Func_801e7c0(0xb0e, box, 0, 0x40);
        _Func_801ea08(v, 8, box, 0x18, 0x48);
        break;
    case 6:
        v = *(unsigned char *)(u + 0x129);
        v += 0x741;
        _Func_801e7c0(v, box, 0, 0x20);
        Func_80a153c(u, box);
        break;
    case 2:
    case 3:
        if (_CanEquipItem(id, raw) == 0) {
            _Func_801e7c0(0xb21, box, 0, 0x30);
        } else {
            n = 0xa6 * 2;
            old = Func_8004938(n);
            copy = Func_8001af8;
            copy(old, u, n);
            if (*(signed char *)(st + 0x97 * 4) != 0) {
                o2 = slot * 2 + 0xd8;
                *(unsigned short *)((int)u + o2) &= 0xfdff;
            } else {
                _EquipItem(id, slot);
            }
            _CalcStats(id);
            Func_80a15f0(u, old, box);
            copy = Func_8001af8;
            copy(u, old, n);
            free(old);
        }
        break;
    case 4:
        item = *(unsigned short *)((char *)info + 0x28);
        found = 0;
        for (i = 0; i <= 0x1f; i++) {
            o3 = i * 4 + 0x58;
            if ((*(unsigned short *)((int)u + o3) & 0x3fff) == item) {
                found = 1;
                break;
            }
        }
        if (found != 0) {
            _Func_801e7c0(item + 0x333, box, 0, 0x30);
            _Func_801e7c0(0xb23, box, 0, 0x38);
        } else {
            _Func_801e7c0(item + 0x333, box, 0, 0x30);
            _Func_801e7c0(0xb22, box, 0, 0x38);
        }
        break;
    case 8:
        base = 0xb1c;
        _Func_801e7c0(base, box, 0, 0x28);
        v = *(unsigned short *)(u + 0x3c);
        _Func_801ea08(v, 3, box, 0x40, 0x28);
        _Func_801e7c0(base + 1, box, 0, 0x30);
        v = *(unsigned short *)(u + 0x3e);
        _Func_801ea08(v, 3, box, 0x40, 0x30);
        _Func_801e7c0(base + 4, box, 0, 0x38);
        v = *(unsigned short *)(u + 0x40);
        _Func_801ea08(v, 3, box, 0x40, 0x38);
        base += 3;
        _Func_801e7c0(base, box, 0, 0x40);
        v = *(unsigned char *)(u + 0x42);
        _Func_801ea08(v, 3, box, 0x40, 0x40);
        break;
    }
}
