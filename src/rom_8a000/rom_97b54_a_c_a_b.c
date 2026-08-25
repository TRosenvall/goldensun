/* Cluster Field_Growth_Target..Field_Growth_Target extracted from goldensun/asm/rom_8a000/rom_97b54_a_c_a.s.
 *
 * Slotted between rom_97b54_a_c_a_a.o and the rest of stage1.ld.
 *
 * THE READ-MODIFY-WRITE AT +0x23 HAS TO BE SPELLED OUT COMPLETELY -- pointer,
 * loaded value, constant and result each in their own local and their own
 * statement:
 *
 *     r = a;  r += 0x23;
 *     t = *r;
 *     v = 2;  v |= t;
 *     *r = v;
 *
 * `a[0x23] = a[0x23] | 2;` is 2 of 34, with the loaded byte and the constant in
 * each other's registers: the ROM has `ldrb r2, [r1] / mov r3, #2 / orr r3, r2`
 * and gcc emits `ldrb r3 / mov r2, #2 / orr r2, r3`. Naming ONLY the loaded
 * byte, or only reordering the operands, changes nothing -- it takes all four.
 *
 * THIS DOES NOT GENERALISE, and the check is recorded so nobody assumes it
 * does. src/non_matching/ovl_7ed0a0/2009458.c has the identical two-instruction
 * residual and the same spelling makes it WORSE, 3 of 36 to 7. Whatever decides
 * the register pair here is not simply the statement structure.
 */
extern unsigned char *iwram_3001f30;
extern void Func_8098698(void);
extern void _Actor_SetAnim(void *a, int anim);
extern void _Actor_SetSpriteFlags(void *a, int f);
extern void WaitFrames(int n);
extern void _PlaySound(int id);
extern void Func_809748c(void);

void Field_Growth_Target(void)
{
    unsigned char *p;
    unsigned char *a;
    unsigned char *r;
    int t;
    int v;

    p = iwram_3001f30;
    a = *(unsigned char **)(p + 0x14);
    if (a == 0)
        return;
    Func_8098698();
    _Actor_SetAnim(a, 2);
    a[0x59] = 0;
    _Actor_SetSpriteFlags(a, 0);
    r = a;
    r += 0x23;
    t = *r;
    v = 2;
    v |= t;
    *r = v;
    WaitFrames(0xa);
    _PlaySound(0x7e);
    WaitFrames(0x28);
    Func_809748c();
}
