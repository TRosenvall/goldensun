	.include "macros.inc"

@ ScriptOp_CommitCursor
@ Entity script opcode handler (dispatched from Data_13624 by Func_a494).
@ r0=entity. Folds the word cursor at +0x04 into the script base at +0x00
@ (base += cursor * 4 + 4) and resets the cursor to 0, so the instruction after
@ the current one becomes the new script origin. Returns 1 to keep the VM
@ running this frame.
.thumb_func_start ActorCmd_SetScript  @ 0x0800ca2c
	mov	r3, #4
	ldrsh	r2, [r0, r3]
	ldr	r3, [r0]
	lsl	r2, #2
	add	r3, r2
	add	r3, #4
	str	r3, [r0]
	mov	r3, #0
	strh	r3, [r0, #4]
	mov	r0, #1
	bx	lr
.func_end  ActorCmd_SetScript

