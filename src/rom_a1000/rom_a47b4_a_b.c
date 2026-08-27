/* Func_80a47b4  --  0x080a47b4
 *
 * The first function of goldensun/asm/rom_a1000/rom_a47b4_a.s; the other three
 * stay as assembly in rom_a47b4_a_c.s.
 *
 * Opens a menu on the context at [iwram_3001f2c]+0x30, runs one update, and --
 * if the halfword at index `idx` in the table at +0x178 is non-zero -- hands
 * that value to Func_80a4924 along with the pointer the context holds.
 *
 * THE CALLEE'S RETURN TYPE IS `int`, NOT `void`, and that is the match. This
 * was the last three instructions, and they are a pure rotation of the
 * argument-setup moves:
 *
 *      rom    mov r1, #0 / mov r2, #0 / mov r0, r7
 *      ours   mov r0, r7 / mov r1, #0 / mov r2, #0
 *
 * Declared `extern void Func_80a10d0(...)`, gcc emits the hard-register moves
 * in ascending order and puts r0 first. Declared `extern int` -- or not
 * declared at all, which gives the implicit `int` return -- it emits r0 last,
 * which is the ROM.
 *
 * BATCH 99 CORRECTED THIS FILE. It originally said the lever was the absence of
 * a PROTOTYPE, because deleting the whole declaration is what was tried first
 * and it worked. That changed two things at once. Isolating them shows the
 * parameter list is irrelevant and the RETURN TYPE decides it: `int f(int,int)`,
 * `int f()` and no declaration all match; `void f(int,int)` and `void f()` do
 * not. The full prototype below is therefore the honest declaration, and the
 * function still matches with it.
 *
 * Nothing else touched the rotation: the declaration-order lever, `void *`
 * parameter types, assigning inside the call expression, and
 * -fno-schedule-insns / -fno-schedule-insns2 / -fno-peephole / -fno-defer-pop /
 * -fno-caller-saves all left it exactly as it was.
 *
 * TWO OTHER READINGS that were needed to get that far:
 *
 *   THE BYTE OFFSET IS ITS OWN VARIABLE. `ldrh r3, [r6, r5]` is the
 *   register-offset form, which gcc emits only when the whole offset is in one
 *   register. Written inline, `base + 0x178 + idx * 2` associates as
 *   `(base + idx * 2) + 0x178` and folds the base into the index; naming
 *   `off = 0x178 + idx * 2` first keeps the base separate.
 *
 *   THE TABLE ENTRY IS READ TWICE, NOT NAMED. The ROM has `ldrh r3 / cmp r3 /
 *   ... / mov r1, r3` -- an extra register-to-register move that a named local
 *   does not produce, because gcc loads a named local straight into the
 *   argument register. Repeating the subscript in the test and in the call and
 *   letting CSE share it is what puts the value in r3 and copies it out.
 */
extern int Func_80a10d0(void *p, int a, int b, int c, int d, int e);
extern char *iwram_3001f2c;
extern void Func_80a22f4(void);
extern void Func_80a4924(void *p, int v);

int Func_80a47b4(int idx)
{
    char *base;
    char *p;
    int off;

    base = iwram_3001f2c;
    p = base + 0x30;
    Func_80a10d0(p, 0, 0, 0xd, 0xa, 2);
    Func_80a22f4();
    off = 0x178 + idx * 2;
    if (*(unsigned short *)(base + off) != 0)
        Func_80a4924(*(void **)p, *(unsigned short *)(base + off));
    return 1;
}
