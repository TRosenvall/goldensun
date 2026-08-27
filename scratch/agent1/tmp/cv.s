	.include "macros.inc"
	.thumb
foo:
	.call_via r5
	bl	_call_via_r5
