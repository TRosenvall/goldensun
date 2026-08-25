/* Func_80173ac  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_15000/rom_15e8c_c_a_c_a_a.s
 * Best screen: 21 instructions in disagreeing regions, of 23 (streams same length).
 *
 * BLOCKER CLASS: pool-loads-first.
 *
 * Five straight-line halfword stores through pooled offsets. The ROM
 * materialises each offset immediately before its use:
 *
 *      ldr r2, [r3] / ldr r3, =0xeae / add r1, r2, r3 / mov r3, #0xf / strh
 *
 * gcc hoists the pool loads ahead of the global dereference and ahead of each
 * other:
 *
 *      ldr r0, =0xeae / ldr r3, [r3] / mov r1, #0xf / add r2, r3, r0 / strh
 *      ldr r1, =0xea8 / ldr r0, =0x12b0 / ...        <- two offsets in flight
 *
 * Once two pool loads are live at once every register downstream is renamed,
 * which is why 21 of 23 differ on a function whose control flow is a straight
 * line and whose store order is already exact.
 *
 * The source below is already written in the ROM's statement order, including
 * the fourth store's value-before-pointer ordering (`v = 0;` then `p = b + o;`,
 * matching `mov r0, #0 / add r3, r2, r1`) and the fifth store's destructive
 * `b += o` matching `add r2, r3`. None of that reaches the hoisting.
 *
 * Same class as the other pool-loads-first parks. It is a scheduling decision
 * about literal pool references and there is no source-level handle on it; the
 * scheduler flags were not tried here because on every function in this corpus
 * where a pool-load order was the defect, disabling the scheduler made the
 * result worse rather than better.
 */
extern unsigned char *iwram_3001e8c;

void Func_80173ac(void)
{
    unsigned char *b;
    unsigned char *p;
    unsigned int o;
    int v;

    b = iwram_3001e8c;
    o = 0xeae;
    p = b + o;
    v = 0xf;
    *(short *)p = v;
    o = 0xea8;
    p = b + o;
    v = 0xa;
    *(short *)p = v;
    o = 0x12b0;
    p = b + o;
    v = 9;
    *(short *)p = v;
    o = 0xeac;
    v = 0;
    p = b + o;
    *(short *)p = v;
    o = 0xeaa;
    b += o;
    v = 1;
    *(short *)b = v;
}
