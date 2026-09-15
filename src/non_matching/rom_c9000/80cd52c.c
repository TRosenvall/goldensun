/*
 * ### BATCH 267 NOTE -- global_alloc, not local-alloc.
 *
 * Measured: `;; 7 regs to allocate: 37 41 33 32 34 35 36`. The values this
 * function ends on are GLOBAL allocnos, so global.c's `allocno_compare` and
 * `find_reg` are the place to read, not local-alloc.c's priority formula.
 * Recorded here because batch 266 grouped this function with two that ARE
 * local-alloc and pointed the next reader at the wrong file.
 */

