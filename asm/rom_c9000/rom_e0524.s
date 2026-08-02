	.include "macros.inc"

@ LoadEffectResource
@ r0=resource id, r1=destination, r2=skip-palette flag, r3=upload-palette flag.
@ Decompresses the resource with GetFile, then:
@   - when r3 is set, DMAs the first 0x80 bytes (a 64-colour palette) to
@     0x5000000 through the RAM-resident copier Func_1af8
@   - when r2 is set, advances past that palette so only the pixel data is used
@ and finally copies the remainder to the destination with DecompressLZ.
@ Every animation in this module stages its graphics through here.
.thumb_func_start LoadVFXFile  @ 0x080e0524
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r8, r1
	mov	r7, r2
	mov	r5, r3
	bl	GetFile
	mov	r6, r0
	cmp	r5, #0
	beq	.Le0548
	mov	r0, #0xa0
	ldr	r3, =Func_8001af8
	lsl	r0, #19
	mov	r1, r6
	mov	r2, #0x80
	bl	_call_via_r3
.Le0548:
	cmp	r7, #0
	beq	.Le054e
	add	r6, #0x80
.Le054e:
	mov	r0, r6
	mov	r1, r8
	bl	DecompressLZ
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end LoadVFXFile
