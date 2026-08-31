/* Func_80f7df0 -- asm/rom_f6000/rom_f6008_c.s
 *
 * BLOCKER: base-plus-offset addressing that gcc folds into pointers.
 * 27 of 30, two lines short.
 *
 * A doubly-linked list head insertion: node at base + i*12, list head at
 * base + v*4 + 0x3000 where v is read from a table at base + i*4 + 0x3404.
 * The arithmetic is all correct -- i*3 then shifted, the 0xc0<<6 head base,
 * the guarded back-link -- and the two-line shortfall is the addressing.
 *
 * THE ROM KEEPS THE BASE IN r4 THROUGHOUT and reaches every field with a
 * register-offset access carrying a COMPLETE byte offset:
 *
 *     rom    ldr r2, [r4, r0]  /  str r3, [r4, r5]  /  ldr r3, [r4, r2]
 *     ours   add r3, r1 (base into the offset) ... ldr r2, [r3, #0x0]
 *
 * It also pushes r5, which we do not: holding three complete offsets live at
 * once costs a callee-saved register that gcc has no reason to spend when it
 * can fold each base-plus-offset into a pointer instead.
 *
 * MEASURED:
 *   offsets written inline in each access          28 lines, 29 differ
 *   EVERY offset named as a complete byte offset
 *     (`n4 = no + 4`, not `b + no + 4`)            28 lines, 27 differ
 *
 * The second applies the name-the-COMPLETE-offset rule from Func_80b6cdc
 * exactly, and it is worth only two differences here. That bounds the lever:
 * it decides addressing FORM when the base register is already right, and it
 * does not stop gcc folding the base itself when doing so saves a register.
 *
 * On Func_80b6cdc the ROM's base was already in the register gcc chose, so
 * naming the offset was the whole fix. Here the fold and the register spend
 * are the same decision, and the source cannot ask for the more expensive one.
 */
extern int ewram_2004c00;

void Func_80f7df0(int i)
{
    char *b;
    int v;
    int vo;
    int no;
    int n4;
    int ho;
    char *nx;

    b = (char *)ewram_2004c00;
    vo = i * 4 + 0x3404;
    v = *(int *)(b + vo);
    no = i * 12;
    n4 = no + 4;
    ho = v * 4 + 0xc0 * 64;
    *(int *)(b + n4) = (int)(b + ho);
    *(int *)(b + no) = *(int *)(b + ho);
    *(int *)(b + ho) = (int)(b + no);
    nx = *(char **)(b + no);
    if (nx != 0)
        *(int *)(nx + 4) = (int)(b + no);
}
