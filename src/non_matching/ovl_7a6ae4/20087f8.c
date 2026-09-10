/* OvlFunc_920_20087f8 -- 0x020087f8, asm/overlays/rom_7a6ae4/ovl_30_c_c_a_c.s
 *
 * 32 differing of 119, AT THE ROM'S EXACT SIZE (268 bytes) with the relocation
 * count and ORDER identical, and the instruction stream the ROM's insn for insn.
 * Best candidate: scratch_elev/b256/small/final/ovl_30_c_c_a_c.PARK.c; the full
 * sweep is in that directory's NOTES.md.
 *
 * 116 -> 32 CAME FROM THE PUSH TELL, and it is a clean example of it. The ROM
 * pushes r8, r9 AND r10; plain C pushes two, because gcc reuses r7 for the table
 * base and then again for `mov r7, sp`. `int *q = v;` makes those conflict and
 * r9 appears. Then a 12-position placement sweep of `q = v;` -- UNIMODAL, at
 * 55/47/47/47/45/43/41/32/32/32/116 -- plus `d <<= 16;` as its own statement at
 * both probe sites, and DELETING a named `y` local.
 *
 * BLOCKER CLASS: low-register rotation from reload's preference pass. This is
 * the recorded "parameter pointer one register too low" shape, seen here on the
 * CALL-USED registers rather than the callee-saved ones. Block 2 is identical
 * insn for insn and differs only in register names; block 1's transposition is
 * downstream of it -- the ROM reuses r4 for both the table-base reload and
 * `mov r4, r8`, creating an anti-dependence that pulls `mov r9, r4` earlier.
 *
 * DISCRIMINATORS RUN IN THE RECORDED ORDER, AND THREE AXES ARE CLOSED:
 *
 *   CONTROL FLOW -- the nested form measures exactly 32. Eliminated.
 *   ALIAS -- `-fno-schedule-insns2` REGRESSES, 69 -> 77, so by the sign rule
 *            sched2 is already producing the ROM's order. Confirmed anyway with
 *            the union high-half fields that solved this function's OWN 18-copy
 *            family: INERT at 32, not one encoding moved.
 *   ALLOCATION -- 15 spellings, all exactly 32.
 *   ORDER, run last -- ALL TWENTY-FOUR declaration permutations give exactly
 *            32. That is a clean 24-of-24 datum for "declaration order is
 *            usually inert". Pins at the probe sites are WORSE, 41.
 *
 * So do not re-spend control flow, alias, or declaration order here.
 *
 * NEXT STEP IS `.18.greg`, NOT MORE SOURCE PERMUTATIONS. The residue is a
 * register naming rotation with the schedule already correct, which is what the
 * reload dump shows directly.
 *
 * LANDING SHAPE IF CLOSED: src/overlays/rom_7a6ae4/ovl_30_c_c_a_c.c.
 * `asmfacts.py` says WHOLE -- convert directly. One function, no data, no
 * split. `makefile_flags()` is empty with no wildcard hazard. The one `.ld`
 * line, overlays/rom_7a6ae4/overlay.ld:40, MUST KEEP its asm/ path. The data
 * label it reads is already `.global` in a sibling `.s` -- grepped.
 */
