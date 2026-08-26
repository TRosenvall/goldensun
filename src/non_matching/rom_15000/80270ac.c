/* Func_80270ac  --  0x080270ac, asm/rom_15000/rom_23178_a_a_a_a_c_a.s
 *
 * Source asm: goldensun/asm/rom_15000/rom_23178_a_a_a_a_c_a.s
 *
 * BLOCKER CLASS: gcc turns a halfword store into a word read-modify-write.
 * Status: 19 lines against 20 and not close -- the RMW replaces one `strh` with
 * five instructions and the register assignment goes with it.
 *
 *     rom    mov r3, #0xff / strh r3, [r5]
 *     ours   ldr r3, [sp] / ldr r2, =0xffff0000 / and r3, r2 /
 *            mov r2, #0xff / orr r3, r2 / str r3, [sp]
 *
 * The object is eight bytes on the stack whose address is passed to two
 * callees: a halfword at +0 and a word at +4. Four struct layouts were measured
 * -- `{unsigned short; unsigned short; int}`, the same with `short`, the same
 * with `unsigned int`, and the two stores in the other order -- and all four
 * produce the RMW. Writing through an explicit `*(unsigned short *)&s` (17
 * lines) and using a `unsigned short h[4]` with a cast for the word (15 lines)
 * are both further away.
 *
 * NOT CAUSED BY THE UNINITIALISED READ, which was the first suspicion. The ROM
 * reads r9 without ever writing it -- the documented "uninitialised local"
 * shape -- and replacing `s.b = u` with `s.b = 0` still gives the RMW (20 lines,
 * 16 differing), so the two are independent.
 *
 * Its file-mate Func_80270d8 has the same uninitialised-r9 shape and is not
 * attempted here.
 */
struct S { unsigned short a; unsigned short pad; int b; };

extern void Func_802281c(struct S *s);
extern void _Func_80c10e8(struct S *s, int n);

void Func_80270ac(void)
{
    struct S s;
    int u;

    s.b = u;
    s.a = 0xff;
    Func_802281c(&s);
    _Func_80c10e8(&s, 1);
}
