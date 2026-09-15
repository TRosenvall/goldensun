/*
 * ### BATCH 267 NOTE -- global_alloc, not local-alloc.
 *
 * Measured: `;; 5 regs to allocate: 35 34 50 32 33`. Whatever decides this
 * function's registers, it is global.c and not local-alloc's priority formula.
 */

