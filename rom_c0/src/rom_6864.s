	.include "macros.inc"

@ GetSoundTableA
@ Takes no arguments. Returns a sound data table address.
.thumb_func_start Func_6864
	swi	0xb
	bx	lr
.func_end Func_6864

@ GetSoundTableB
@ Takes no arguments. Returns a second sound data table address.
.thumb_func_start Func_6868
	mov	r0, #0
	swi	0x19
	bx	lr
.func_end Func_6868

@ GetSoundTableC
@ Takes no arguments. Returns a third sound data table address.
.thumb_func_start Func_6870
	mov	r0, #1
	swi	0x19
	bx	lr
.func_end Func_6870
