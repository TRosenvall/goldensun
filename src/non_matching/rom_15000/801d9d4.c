/* Func_801d9d4 (DrawSubScreen) -- 0x0801d9d4, asm/rom_15000/rom_1ca1c_c_c_c.s
 *
 * NON-MATCHING: 41 encodings of 184 differ (objcmp).
 *
 * SIZE EXACT (412 bytes both), INSTRUCTION COUNT EXACT (176 = 176), and objcmp
 * reports NO relocation difference -- all 14 `bl` offsets and all 4 ABS32 pool
 * words are the ROM's, at the ROM's byte offsets.  Only ENCODINGS differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801d9d4.c \
 *     asm/rom_15000/rom_1ca1c_c_c_c.s --func Func_801d9d4
 *
 * NO SHIMS, NO PINS, NO asm.  The only non-obvious declaration is
 * `extern unsigned char L367dc[] __asm__(".L367dc");` -- the tree's established
 * spelling for a `.L` local label (src/rom_c9000/rom_dd2ac_c_c_b.c:123).
 *
 * ITS .s NEEDS A TEXT/DATA SPLIT, AND THE EXPORT LIST IS ONE LABEL SHORT.
 * datacheck.py, verbatim:
 *     data sections : .rodata
 *     functions     : Func_801d9d4, StartMenu_Main, Func_801dd28
 *     EXPORTS       : .L367c9, .L367cc, .L367ce, .L367d0, .L367d6, .L36750
 * Those six are already `.global`.  BUT `.L367dc` is NOT, and BOTH Func_801d9d4
 * and StartMenu_Main reach it (`ldr r3, =.L367dc`, lines 157 and 274).  Today it
 * resolves as a file-local label; the moment either function leaves this .s the
 * data object must also carry `.global .L367dc`.  So the split needs SEVEN
 * exports, not six.  Data is a clean tail cut: functions end at line 564, the
 * `.section .rodata` is at 566, and the seven `.incrom` ranges span
 * 0x36750-0x367e4 with nothing interleaved between functions.  stage1.ld keeps
 * the two lines far apart -- `.text` at line 468, `.rodata` at line 623 -- so the
 * repoint is two independent edits.
 *
 * FOUR LEVERS LANDED.  Drop ladder from 41, each removed alone:
 *   int carrier for the sign extension   67   (26 worse)
 *   `ty` reused for the sprite Y         89   (48 worse, and 2 instructions short)
 *   `sel = 0` after the _GetFlag call    44
 *   `t = 3` before `n = rows - 1`        43
 * Full ladder: 78 -> 64 (ty reuse + sel-after-call) -> 43 (int carrier + named
 * `bx`/`pp`) -> 41 (`t = 3` first).
 *
 * 1. AN `int` CARRIER TURNS `ldrsb` BACK INTO `ldrb / lsl #24 / asr #24`.
 *    The ROM sign-extends the option table byte with shifts.  Both
 *    `signed char *p; ... f((signed char)*p)` and
 *    `unsigned char *p; ... f((signed char)*p)` give gcc-2.96 a single
 *    `mov r0,#0 / ldrsb r0,[r6,r0]` -- combine merges the extension into the
 *    load, and on Thumb that costs an extra zero register because `ldrsb` has no
 *    immediate-offset form.  Routing the byte through `int c = *p;` and casting
 *    `(signed char)c` at the call keeps the extension a separate operation and
 *    reproduces the ROM exactly.  This is the "an `int` carrier keeps a
 *    zero-extension combine would drop" lever, in its sign-extension direction.
 *    It is also worth an INSTRUCTION, not just an encoding: it took the count
 *    from 175 to 176.
 *    NOTE THE CONTRAST WITH ITS SIBLING: StartMenu_Main, 200 lines below in the
 *    same .s, reads the SAME table and the ROM really does use `ldrsb` there
 *    (`ldrsb r0,[r6,r3]`), because there the index varies and the base is
 *    loop-invariant.  So the table is `signed char` in one function and read
 *    through an int in the other; the two spellings are not interchangeable.
 *
 * 2. THE SPRITE Y GOES INTO THE SAME LOCAL THAT CARRIES THE TEXT ROW.
 *    The ROM spends `mov r7,r3 / add r7,#0x10 / ... / mov r2,r7` where a fresh
 *    temporary gives `lsl r2,#3 / add r2,#0x10` with no copy -- two instructions
 *    fewer and 48 encodings worse.  r7 is the register that held the
 *    DrawSmallText row offset (4, 0x34, +0x18 ...) above and holds -4 below, so
 *    ONE `ty` local spans all three roles.  Assigning the y coordinate into it
 *    (`ty = bx->y * 8 + 0x10;` then passing `ty`) is what puts the copy back.
 *
 * 3. `sel = 0` IS AFTER THE _GetFlag CALL, not before.  The ROM's
 *    `bl _GetFlag / mov r2,#0 / mov r9,r0 / str r2,[sp,#8]` puts the store below
 *    the call, and gcc-2.96 will not move a store across a call, so source
 *    position is the only lever.  Worth 3.
 *
 * 4. `t = 3;` BEFORE `n = rows - 1;` lets sched2 land `mov r6,#3` between the
 *    ROM's `mov r5,r10` and `sub r5,#1`.  Worth 2.
 *
 * MEASURED AND INERT, so nobody need repeat them:
 *   declaration-initialiser vs statement form for `rows = 3` and `s`   0
 *   block-scoping `bx`/`pp`/`c` inside the `if`                        0
 *   `slot` declared first / last in the local list                     0
 *   `p` before `q` in the final loop                                   0
 *   a descending argument pin on CreateUIBox (`a2 = 0x14; a0 = 5;`)     0
 *   `rows = 1;` before `sel = 2;` in the flag arm                      +2
 *   `n = rows` assigned before q/p in the final loop                   +3
 *
 * MEASURED AND WRONG: the "one shared scratch variable" theory.  The ROM keeps
 * r5 for five separate roles -- the first loop's counter, the 0xc23 message id,
 * the 0xc27 message id, the sprite slot, and the final loop's counter -- which
 * looks like one source variable.  It is not.  Unifying all five read 62,
 * `slot` merged with `m` alone read 50, `slot` merged with `n` alone read 53,
 * all against 41 for five distinct locals.  Separate variables are correct here.
 *
 * BLOCKER: RELOAD/GLOBAL-ALLOC REGISTER ROTATION AT EQUAL COUNT.  PASS .18.greg.
 * Every one of the 41 differing encodings is the same instruction with a
 * different register number, and the shift is one coherent rotation:
 *
 *   ROM  mov r0,#3        ours  mov r2,#3      (rows, prologue)
 *   ROM  mov r2,#0        ours  mov r3,#0      (sel = 0)
 *   ROM  mov r0,#2        ours  mov r2,#2      (sel = 2)
 *   ROM  mov r0,r10       ours  mov r2,r10     (rows copy for the y/h algebra)
 *   ROM  mov r2,r10       ours  mov r3,r10     (rows copy for the loop guard)
 *   ROM  mov r3,r9        ours  mov r2,r9      (flag copy)
 *   ROM  mov r5,r0        ours  mov r6,r0      (sprite slot)
 *   ROM  ldr r2,=0x5a4    ours  ldr r5,=0x5a4  (the &s->obj address)
 *   ROM  ldr r0,[sp,#8]   ours  ldr r2,[sp,#8] (sel reload)
 *
 * `.18.greg` gives the dispositions: 39 in r6, 40 in r5, 41 in r6, 42 in r5,
 * 45 in r5, 80 in r6.  The sprite slot draws r6 where the ROM's is r5, and the
 * 0x5a4 address then draws r5 (callee-saved) where the ROM's is r2
 * (call-clobbered, correctly short-lived).  This is the corpus's
 * REG_ALLOC_ORDER class -- {3,2,1,0,12,14,4,5,6,7,...} with no Thumb override --
 * and nothing tried here moves it.  It is NOT the cse-zero class: no constant
 * zero is shared, the four `mov rN,#0` sites all rematerialise.
 */
struct SubScr {
    unsigned char pad0[0x5a4];
    void *obj;                          /* 0x5a4 */
    unsigned char pad1[0x610 - 0x5a8];
    void *items[1];                     /* 0x610 */
};
struct Box { unsigned char pad[0xc]; unsigned short x; unsigned short y; };

extern struct SubScr *iwram_3001ea0;
extern unsigned char gDebugMode;
extern unsigned char L367dc[] __asm__(".L367dc");
extern unsigned char Data_310a4[];
extern int _GetFlag(int flag);
extern unsigned char *CreateUIBox(int x, int y, int w, int h, int mode);
extern void Func_801e41c(void *box, int a, int b, int c, int d);
extern void DrawSmallText(int id, void *box, int x, int y);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int n, void *src);
extern void *Func_801eadc(int slot, unsigned int m, void *box, int d, int e);
extern void _Func_80b0a20(void *p, int x, int y);
extern void *Func_8021750(int a, int b, void *box, int c, int d);

unsigned char *Func_801d9d4(void)
{
    struct SubScr *s;
    int rows;
    int sel;
    int f;
    int y, h, ty, slot, n, t, m, c;
    struct Box *bx;
    void **pp;
    unsigned char *box;

    s = iwram_3001ea0;
    rows = 3;
    f = _GetFlag(0x17e);
    sel = 0;
    if (f != 0) {
        sel = 2;
        rows = 1;
    }
    if (gDebugMode != 0)
        rows += 3;
    y = 8 - rows;
    h = rows * 3 + 1;
    if (y + h > 0x13) {
        y = 1;
        h = 0x13;
    }
    box = CreateUIBox(5, y, 0x14, h, 2);
    if (rows > 1) {
        t = 3;
        n = rows - 1;
        do {
            Func_801e41c(box, 0, t, 0x13, t);
            t += 3;
        } while (--n != 0);
    }
    ty = 4;
    if (f == 0) {
        m = 0xc23;
        DrawSmallText(m, box, 0x30, 4);
        m++;
        DrawSmallText(m, box, 0x30, 0x1c);
        ty = 0x34;
    }
    DrawSmallText(0xc25, box, 0x30, ty);
    ty += 0x18;
    if (gDebugMode != 0) {
        m = 0xc27;
        DrawSmallText(m, box, 0x30, ty);
        ty += 0x18;
        DrawSmallText(m + 1, box, 0x30, ty);
        ty += 0x18;
        m += 2;
        DrawSmallText(m, box, 0x30, ty);
    }
    slot = AllocSpriteSlot();
    if (slot <= 0x5f) {
        UploadSpriteGFX(slot, 0x80, Data_310a4);
        pp = &s->obj;
        *pp = Func_801eadc(slot, 0x40000000, box, 0, 0);
        bx = (struct Box *)box;
        ty = bx->y * 8 + 0x10;
        _Func_80b0a20(pp, bx->x * 8, ty);
    }
    ty = -4;
    if (rows > 0) {
        void **q = &s->items[0];
        unsigned char *p = &L367dc[sel];
        n = rows;
        do {
            c = *p;
            p++;
            *q++ = Func_8021750((signed char)c, 0, box, 0xc, ty);
            ty += 0x18;
        } while (--n != 0);
    }
    return box;
}
