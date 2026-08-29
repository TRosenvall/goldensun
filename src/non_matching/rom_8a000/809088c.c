/* Func_809088c -- 0x0809088c, asm/rom_8a000/rom_8d9a4_c_c_c_a_a_a_c.s
 *
 * Twin of Func_80f2ebc; differs in one constant (0x53f against 0x5ff).  The
 * full analysis is in ../rom_f2000/80f2ebc.c and is not repeated here.  Both
 * members are blocked on the same thing: the C is finished, and reaching
 * divsi3_RAM rather than __divsi3 needs a per-object symbol rename that the
 * main ROM's single link cannot express as an alias.
 */
