/* Func_80ae2f4 (0x080ae2f4) -- NON-MATCHING, 395 of 461 encodings differ.
 * Reference asm/rom_a1000/rom_ad274_c_c_a_c.s.  Intended park path:
 * src/non_matching/rom_a1000/80ae2f4.c
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_a1000/80ae2f4.c \
 *       asm/rom_a1000/rom_ad274_c_c_a_c.s --func Func_80ae2f4
 *
 * NON-MATCHING, 395 of 461 encodings differ.
 * NOT A TRUE DISTANCE: 1000 bytes against 1056 and 433 encodings against 461,
 * so 395 does not rank.  tools/aligncmp.py puts 219 of 461 aligned-equal
 * (47.5%), 298 in 103 hunks.  Reference instruction count 439 (the .s's own
 * lines) plus one pool word.  Shim count 0 (pin-free, no fakematch.txt row).
 *
 * SPLIT: asm/rom_a1000/rom_ad274_c_c_a_c.s holds THREE functions -- Func_80ad69c,
 * Func_80ad6d4 and Func_80ae2f4, in that order -- so landing this one is a
 * two-way split at the third: `rom_ad274_c_c_a_a.s` (the first two, unchanged)
 * plus `rom_ad274_c_c_a_b.s`.  AND THE ROADBLOCK IS IN THE LINKER SCRIPT, NOT
 * THE .s: tools/datacheck.py reports NO data requirement, yet stage1.ld names
 * this object TWICE --
 *     stage1.ld:1388  asm/rom_a1000/rom_ad274_c_c_a.o(.text)
 *     stage1.ld:1440  asm/rom_a1000/rom_ad274_c_c_a.o(.rodata)
 * -- so the object DOES contribute .rodata that its `.s` does not spell as a
 * section.  The source is Func_80ad6d4's 19-word jump table (`.word .Ladcc4`
 * ... at lines 709-727), which gcc emits into .rodata; Func_80ae2f4 itself has
 * no table.  The `_a` half must therefore keep BOTH linker-script lines and the
 * `_b` half gets only the .text line.  This is exactly the case the brief warns
 * about -- grep the linker scripts for the object stem, because a script can
 * name an object in a section its `.s` does not have.
 *
 * ================================================================
 * WHAT CLOSED, BY PASS (448 -> 446 -> 395)
 * ================================================================
 *
 * 1. `(*(char **)(S + 0x14))[5] = 0xd;` -- a `strb` through a POINTER MEMBER,
 *    not a store to the member.  (A transcription slip in the first pass; the
 *    only reason it is recorded is that it cost 2 encodings and nothing else
 *    moved, which is the signature of a one-store error.)
 *
 * 2. BOTH TWO-INT BLOCKS ARE REACHED THROUGH POINTER LOCALS.  The ROM holds
 *    &pA in r10 AND spills it to sp+0x14, and holds &pB in r11, then reads
 *    element [0] BOTH ways -- `ldr r3, [sp, #0x4c]` (direct) and
 *    `mov r0,#0 / ldr r3,[r0,r1]` with r1 = r11 (a zero index register,
 *    because Thumb cannot use a high register as a `ldr` base).  Writing the
 *    blocks as plain arrays keeps them in sp-relative form throughout and is
 *    51 encodings worse: 446 -> 395 came entirely from declaring `int *pAp`
 *    and `int *pBp` and routing the [1] accesses and the loop reads through
 *    them.  It also fixed the frame: 0x48 -> 0x4c against the ROM's 0x54.
 *    TELL for this class: `mov rL, #0` immediately before `ldr rD, [rL, rH]`
 *    is not an index, it is a HIGH-REGISTER BASE, and it means a pointer local.
 *
 * 3. THE TWO Func_801e7c0 MESSAGE IDS ARE A NAMED BASE.  `ldr r5, =0xbaa` ...
 *    `add r5, #2` with r5 held across the intervening call is the named-base
 *    lever: two plain literals cannot produce a constant carried in a
 *    callee-saved register.  `msg = 0xbaa; ...; msg += 2;` gives it, and the
 *    two uses are in one basic block so no flag is needed.
 *
 * 4. THE 0xf AND 2 PASSED ON THE STACK TO BOTH Func_80a10d0 CALLS ARE ONE
 *    QUANTITY EACH (r6 and r5, held across the first call), while the SAME
 *    0xf passed in r3 is rematerialised in each call.  Do not "fix" the r3
 *    one -- rematerialising a register argument and CSE-ing the stack one is
 *    what gcc does here, and it is already right.
 *
 * ================================================================
 * THE BLOCKER: 28 ENCODINGS SHORT, AND THE CALL SEQUENCE NAMES BOTH
 * ================================================================
 *
 * The call sequence is the cheap instrument here and it isolates the residue
 * to exactly two places.  Extracting every `bl` from both sides gives 46
 * against 46 with two differences:
 *
 * (a) A DEAD `__modsi3` THAT GCC WILL NOT EMIT.  The ROM does
 *         bl __modsi3 / sub r0, #5 / mov r0, #0 / ... / bl Func_80ad5b4
 *     -- it computes `frame % 0x3c - 5`, keeps the subtraction, and then
 *     overwrites r0 with the first argument of the next call.  The value is
 *     dead in the ROM's own object.  Our gcc deletes the whole thing, and it
 *     deletes it from every dead-value spelling tried: assignment to a local
 *     that is later overwritten, assignment to a local never read, and a
 *     `(void)` cast expression statement -- all three score IDENTICALLY (395),
 *     so this is not a spelling question.  gcc-2.96 cannot delete the CALL
 *     (it does not know __modsi3 is pure) which is why the ROM still has it;
 *     what our build additionally deletes is the whole STATEMENT before the
 *     call is ever emitted.  Worth about 5 of the 28 encodings.  NEXT STEP:
 *     find the live use, not a way to keep dead code -- the `- 5` says the
 *     value fed something, and Func_80ad5b4's four arguments are all
 *     constants, so the use is elsewhere in a form this reading has missed.
 *
 * (b) ONE SHARED `bl _PlaySound(0x71)` FOR TWO CANCEL KEYS.  The ROM sets
 *     `r0 = 0x71` in BOTH arms and shares only the tail
 *     `bl _PlaySound / neg r7, r7 / b end` -- i.e. gcc CROSS-JUMPED the tail
 *     while leaving the argument setup duplicated, and the `neg` says the
 *     return value is built as a positive and negated once.  Two spellings
 *     were measured:
 *       - `ret = 2 / ret = 1` in the arms, the shared tail after an `else`
 *         that `continue`s: the tail merges, but gcc lays the else-block
 *         FIRST and the merged tail LAST (397, aligned 213), which is the
 *         wrong block order.
 *       - two separate `_PlaySound(0x71); ret = -ret; break;` bodies: gcc
 *         does NOT cross-jump them at all (395, aligned 219, four
 *         `bl _PlaySound` against the ROM's three).
 *     So the residue here is a BLOCK-PLACEMENT question, not a structural one,
 *     and the ROM's order (test, test, merged tail, then the d-pad block) has
 *     not yet been produced.
 *
 * Everything else about the call sequence -- 46 calls, same symbols, same
 * order -- already agrees, so the remaining ~23 encodings are inside the
 * straight-line blocks between those calls and should be attacked block by
 * block against the frame, which is still 0x4c against the ROM's 0x54 (two
 * spill words the ROM has and we do not).
 *
 * ALSO WORTH RECORDING, because it looks like a defect and is not: the call
 *     Func_80aae14(u + 0x58, u + 0x58, b1, &v40, &v3c)
 * really does pass the SAME pointer in r0 and r1 (`mov r1,r0 / add r1,#0x58 /
 * mov r0,r1`), and the 0x14c-byte buffer from the second Func_8004970 is
 * allocated and freed WITHOUT being passed to anything.  Both are in the ROM.
 */
extern char *iwram_3001f2c;
extern char *iwram_3001e8c;
extern unsigned int gKeyPress;
extern unsigned int gKeyRepeat;
extern unsigned char Data_af26c[];

extern void Func_8001af8(void *dst, const void *src, int n);
extern void Func_80008d8(void *dst, int n, unsigned int v);
extern void Func_80a19a0(void);
extern void _Func_80164ac(void *box);
extern int WaitFrames(int n);
extern void *Func_8004970(int size);
extern void free(void *p);
extern unsigned char *_GetUnit(int id);
extern int Func_80aae14(void *a, void *b, void *c, int *d, int *e);
extern void Func_80a10d0(void *box, int a, int b, int c, int d, int e);
extern void _Func_8016498(void *box);
extern void _Func_801e7c0(int id, void *box, int a, int b);
extern void _Func_8016478(void *box);
extern void Func_80acab8(void *box, int a, int b, int c, int d, int e, int f, int g, int h);
extern void _Func_8019000(void *box, int ch, int x, int y, int z);
extern void Func_80ad5b4(int a, int b, int c, int d);
extern int Func_80aa538(int a, int b);
extern void _PlaySound(int id);
extern void Func_800352c(void);
extern int StartTask(void *f, int prio);
extern void Func_80a1114(void *box, int a);

int Func_80ae2f4(void)
{
    int pA[2];
    int pB[2];
    int v40;
    int v3c;
    char *S;
    int *pAp;
    int *pBp;
    unsigned char *ptrC;
    unsigned int rep;
    unsigned int keys;
    char *base;
    void *boxB;
    void *boxA;
    int frame;
    int redraw;
    int sel;
    void (*copy)(void *, const void *, int);
    void (*fill)(void *, int, unsigned int);
    unsigned short *q;
    unsigned char *u;
    void *b1;
    void *b2;
    void *box;
    int i, n, msg, ret;

    S = iwram_3001f2c;
    redraw = 1;
    sel = 0;
    frame = 0;
    (*(char **)(S + 0x14))[5] = 0xd;
    pAp = pA;
    pAp[0] = 0;
    pAp[1] = 0;
    q = (unsigned short *)(S + 0x14a);
    for (i = 0; i < 4; i++)
        *q-- = 0xc8;
    _Func_80164ac(*(void **)(S + 0x30));
    WaitFrames(1);
    pBp = pB;
    pBp[0] = 1;
    pBp[1] = 1;
    b1 = Func_8004970(0x60);
    b2 = Func_8004970(0xa6 * 2);
    u = _GetUnit(*(unsigned char *)(S + 0x21a));
    pBp[0] = Func_80aae14(u + 0x58, u + 0x58, b1, &v40, &v3c);
    pBp[1] = pBp[0];
    free(b2);
    free(b1);
    pB[0] = (pB[0] - 1) / 6 + 1;
    if (pB[0] == 0)
        pB[0] = 1;
    pBp[1] = (pBp[1] - 1) / 6 + 1;
    if (pBp[1] == 0)
        pBp[1] = 1;
    boxA = (void *)(S + 0x24);
    Func_80a10d0(boxA, 0, 5, 0xf, 0xf, 2);
    boxB = (void *)(S + 0x34);
    Func_80a10d0(boxB, 0xf, 5, 0xf, 0xf, 2);
    _Func_8016498(*(void **)(S + 0x86 * 2));
    _Func_8016498(*(void **)(S + 0x10));
    msg = 0xbaa;
    _Func_801e7c0(msg, *(void **)(S + 0x10), 0, 0);
    msg += 2;
    _Func_801e7c0(msg, *(void **)(S + 0x10), 0, 0x10);
    ptrC = (unsigned char *)(S + 0x96 * 4);
    for (;;) {
        base = iwram_3001e8c;
        keys = gKeyPress;
        rep = gKeyRepeat;
        if (redraw != 0) {
            base[0xea6] = 1;
            _Func_8016478(*(void **)(S + 0x24));
            _Func_8016478(*(void **)(S + 0x34));
            Func_80acab8(*(void **)(S + 0x24), 0, 0, *ptrC, 0, 0, 3, 0, 1);
            Func_80acab8(*(void **)(S + 0x34), 0, 0, *ptrC, 0, 0, 3, pA[0] + 1, 1);
            base[0xea6] = 0;
        }
        if (pBp[0] > 1) {
            box = *(void **)(S + 0x34);
            for (i = 0; i < pBp[0]; i++) {
                n = i + 0xf031;
                if (i > 9)
                    n = 0xf030;
                if (i == pA[0])
                    n -= 0x1000;
                _Func_8019000(box, n, *(unsigned short *)((char *)box + 8) - pBp[0] + i - 2, -1, 0);
            }
            _Func_8019000(box, 0xf128, *(unsigned short *)((char *)box + 8) - pBp[0] - 3, -1, 0);
            _Func_8019000(box, 0xf129, *(unsigned short *)((char *)box + 8) - 2, -1, 0);
            base[0xea3] |= 2 << (*(unsigned short *)((char *)box + 0xe) >> 2);
        }
        frame++;
        ret = frame % 0x3c - 5;
        Func_80ad5b4(0, 0x20, 0xc8, 0);
        if (redraw != 0) {
            redraw = 0;
            sel = Func_80aa538(sel, 2);
        }
        if ((frame & 3) == 0) {
            if (frame & 4) {
                copy = Func_8001af8;
                copy((void *)0x60052c0, Data_af26c, 0x20);
            } else {
                fill = Func_80008d8;
                fill((void *)0x60052c0, 0x20, 0x44444444);
            }
        }
        if (keys & 8) {
            _PlaySound(0x71);
            ret = -2;
            break;
        }
        if (keys & 0x303) {
            _PlaySound(0x71);
            ret = -1;
            break;
        }
        if (rep & 0x20) {
            pAp[0]--;
            pAp[0] = Func_80aa538(pAp[0], pBp[0]);
            _PlaySound(0x6f);
            Func_800352c();
            redraw = 1;
        } else if (rep & 0x10) {
            pAp[0]++;
            _PlaySound(0x6f);
            Func_800352c();
            redraw = 1;
            pAp[0] = Func_80aa538(pAp[0], pBp[0]);
        }
        WaitFrames(1);
    }
    StartTask(Func_80a19a0, 0xc8 << 4);
    box = (void *)(S + 0x86 * 2);
    iwram_3001e8c[0xea6] = 1;
    Func_80a1114(box, 1);
    WaitFrames(1);
    Func_80a10d0(box, 0xd, 0, 0x11, 5, 2);
    Func_80a1114(boxA, 1);
    Func_80a1114(boxB, 1);
    _Func_8016498(*(void **)(S + 0x30));
    _Func_8016498(*(void **)(S + 0x28));
    _Func_8016498(*(void **)(S + 0x10));
    iwram_3001e8c[0xea6] = 0;
    WaitFrames(1);
    return ret;
}
