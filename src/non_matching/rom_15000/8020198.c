/* Func_8020198 -- 0x08020198, asm/rom_15000/rom_20198_a_a.s
 *
 * 68 of 68 lines, TWO differing, and they are two independent loads in the
 * opposite order.  Candidate at scratch/N0198_best.c.
 *
 *      rom   ldr r3, =0x741 / ldrb r0, [r7, #0x1d]
 *      ours  ldrb r0, [r7, #0x1d] / ldr r3, =0x741
 *
 * SOLVED, two things, and both are worth the note:
 *
 *   THE POOLED 9 IS A MESSAGE SYMBOL.  `ldr r0, =0x9` for a value an eight-bit
 *   `mov` builds is the pooled-constant tell, and Func_801e7c0's first argument
 *   is a message id -- three elevated files pass one to it.  `_MSG_09 = 0x09;`
 *   added to message.sym alongside the existing _MSG_0a.._MSG_0d; it takes the
 *   count from 18 differing to 17 and emits no bytes.  It is kept even though
 *   this function is parked, so the next attempt does not re-derive it.
 *
 *   THE STACK ARGUMENT NEEDS A CALL BETWEEN ITS ASSIGNMENT AND ITS USE.  The
 *   ROM spends a THIRD callee-saved register (it pushes {r5, r6, r7}) with r6
 *   holding the fifth argument at two six-argument calls and r7 holding the
 *   parameter.  Written beside its call the value gets a scratch register, we
 *   push only {r5, r6}, and the parameter lands in r6 -- 17 differing, almost
 *   all of it that one rename cascading.  Moving `n = 0;` up so it spans the
 *   preceding UIDrawText makes gcc spend r7, and 17 drops to 2.  This is the
 *   batch-149 rule (statement position picks the register class) paying off on
 *   a function where the whole diff was downstream of it.
 *
 *   Moving the SECOND assignment up instead works equally well; moving BOTH is
 *   worse (11 differing).  One crossing is enough and two is too many.
 *
 * BLOCKER: SCHEDULER TIE between two independent loads.  sched2 is producing
 * the ROM's order everywhere else in the function -- --no-sched2 goes to 24
 * differing -- so this is not a reason to reach for a flag.
 *
 * SCREENED AND UNCHANGED AT 2: the addends written the other way round
 * (`0x741 + p->f1d`); the constant named in a local first; the parameter
 * declared as a typed struct so the byte read is a field, which is the
 * batch-145 alias-set lever and does nothing here; and the per-declaration
 * prototype sweep over every callee.
 */
