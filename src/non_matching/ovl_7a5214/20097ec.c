/* OvlFunc_918_20097ec -- asm/overlays/rom_7a5214/ovl_17ec_a.s
 *
 * BLOCKER: TABLE BASE KEPT LIVE ACROSS A LOOP
 *
 * 18 of 47 differing.
 *
 * gcc keeps the table base in a callee-saved register for the whole loop where the
 * ROM RE-LOADS `ldr r6, =.L1ca8` in the loop latch, and the r5/r6 roles come out
 * swapped, costing one `mov r5, r3`.
 * * MEASURED: subscript L1ca8[k] 20 (and index-first operand order); byte-offset
 * pointer arithmetic *(short*)((char*)L1ca8 + off) 18 -- KEPT, this fixed the
 * operand order; a 6-short struct with L1ca8[i].field 57 of 59 -- MUCH worse, gcc
 * gives up on strength reduction and spends r9/r10; a named char *t base 49 --
 * worse, also drops ldrsh for ldrh+sign-extend; assignment order of the two
 * pre-loop locals 18 either way; -fno-strength-reduce 18;
 * -fno-rerun-cse-after-loop 45; -fno-gcse 54.
 * * Note the struct result: `add r5, #0xc` on a table of shorts means the source
 * counts in ELEMENTS OF THE BASE TYPE, not in records.
 */
