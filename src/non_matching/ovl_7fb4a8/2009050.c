/* OvlFunc_971_2009050  [ovl_7fb4a8]
 * Source asm: goldensun/asm/overlays/rom_7fb4a8/ovl_30_c_a_a_a.s
 *
 * ONE INSTRUCTION, and it is the symbol-address class again -- but this time
 * the symbol namespace is unknown, so it is parked rather than guessed at.
 *
 *     rom    ldr r0, =1        (a real pool load: 4800 + .word 1)
 *     ours   mov r0, #1        (2001)
 *
 * Confirmed by assembling both: gas does NOT fold `ldr r0, =1` into a mov, so
 * these are genuinely different bytes. And gcc will never pool a constant that
 * fits in eight bits -- unless the operand is the ADDRESS OF A SYMBOL whose
 * value happens to be 1, which always pools.
 *
 * That is exactly the mechanism behind the OvlFunc_974 family (see
 * docs/elevation.md). What is missing here is which symbol. The tree has two
 * id namespaces -- message.sym for message ids, file_table.sym for file ids
 * (_FILE_13 = 0x13, _FILE_BUILD_DATE = 2) -- and neither currently defines
 * anything with value 1. The argument is the first parameter of
 * __SetDestMap, so it is plausibly a MAP id, which would want a third
 * namespace rather than borrowing one of these.
 *
 * Adding _FILE_1 on a guess would put a wrongly-named symbol into a shared
 * linker fragment that goes upstream, to save one instruction. Better to
 * leave it until someone can say what the id space is.
 */
extern void __Func_8006358(void);
extern void __SetSoundFXMode(int mode);
extern void __SetDestMap(int map, int entrance);

void OvlFunc_971_2009050(void)
{
    __Func_8006358();
    __SetSoundFXMode(2);
    __SetDestMap(1, 1);
}
