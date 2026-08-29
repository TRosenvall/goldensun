/* OvlFunc_943_2009920 -- asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c.s
 *
 * BLOCKER: r0 IN THE MIDDLE OF A THREE-ARGUMENT CALL
 *
 * 5 of 50 differing.
 *
 * Twice:
 *   rom  mov r1,#0xee / mov r0,#0x17 / lsl r1,#0x10 / ldr r2,=0x2720000
 *   rom  ldr r1,=0xcccc / mov r0,#0x15 / ldr r2,=0x6666
 * The complete argument-order table in docs/elevation.md has NO row producing r0
 * second.  Two more members for that class.
 * * MEASURED: __MapActor_SetSpeed declared void, int, void(), int(), and withheld
 * (all 5; withheld = 18); the shifted argument as a named local 5;
 * -fno-rerun-cse-after-loop 5; -fno-schedule-insns 5; -fno-schedule-insns2 11.
 * Everything else matches, including the .L5160 asm-label extern, the block-scoped
 * int zero = 0 for the HImode strh, and the narrow unsigned char m = 0x80 for the orr.
 */
