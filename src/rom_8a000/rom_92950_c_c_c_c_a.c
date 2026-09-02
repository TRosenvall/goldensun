/* Func_80931ec -- 0x080931ec, from
 * goldensun/asm/rom_8a000/rom_92950_c_c_c_c.s.
 *
 * A two-speaker cutscene exchange: open a message box for each speaker with its
 * own voice, run both portraits, wait for the text to finish, wait for the
 * player to press A or B, then close both boxes and wait for each to close.
 *
 * THE .s HELD MORE THAN THIS FUNCTION. It also carried a `.section .rodata`
 * block exporting `.L9ed80`, which stage1.ld pulls on its own line and
 * Func_8092980 in another translation unit references. Replacing the whole file
 * with this .c dropped that data and the link failed with an undefined
 * reference -- caught by make compare, not by the screen. The file is now split
 * by hand into _a (this function) and _b (the rodata), because tools/split_s.py
 * splits at FUNCTION boundaries and would have kept trailing data with the
 * function it follows.
 *
 * > Before converting a whole .s, grep it for `.section`, `.global` and
 * > `.incrom`. A screen cannot see data another object needs.
 *
 * MATCHED ON THE FIRST SPELLING, from three oracles read off the reference
 * before writing anything:
 *
 *   THE EPILOGUE POPS INTO r0, so the function is `void` -- even though it
 *   keeps two results from _Func_8017658 in locals.
 *
 *   THE ARGUMENT FILL ORDER SAYS WHICH CALLEES RETURN A VALUE. The ROM fills
 *   `mov r2 / ldr r3 / mov r1 / mov r0` and `mov r1 / ldr r2 / mov r0 / ldr r3`
 *   -- r0 late in both -- so _Func_8017658 and _Func_8019da8 are declared
 *   `int`. gcc reserves r0 for a value-returning call and evaluates that
 *   argument last.
 *
 *   THE MESSAGE-ID INCREMENT IS THE ARGUMENT ITSELF, not a statement before the
 *   call. Written `(*(short *)(p + (0xec << 1)))++` in the argument list it
 *   produces the ROM's `ldrh / add / lsl / strh ... asr` interleave and lets
 *   gcc fold the base once so the second site reuses the register. The sibling
 *   Func_8093168 in src/rom_8a000/rom_92950_c_c_c_a.c uses the same
 *   `(0xec << 1)` idiom.
 *
 * ONE THING TO WATCH IF THIS EVER SHARES A HEADER. `GetSpriteVoice` is called
 * here WITH an argument, but its elevated definition in
 * src/rom_8a000/rom_91584_a_c_a_a.c is `int GetSpriteVoice(void)`. It is
 * declared unprototyped below so the argument is emitted at all; if a shared
 * header ever declares the `(void)` form this stops compiling. That also
 * suggests the original source called it without a visible prototype.
 */
extern unsigned char *iwram_3001ebc;
extern volatile int gKeyPress;
extern int Func_8092ba8(int id);
extern int GetSpriteVoice();
extern int _Func_8017658(int id, int a, int b, int c);
extern int _Func_8019da8(int a, int b, int c, int d);
extern int _Func_8017364(void);
extern int _Func_8017394(int h);
extern void _Func_8019e48(int a);
extern void _Func_8019a54(void);
extern void WaitFrames(int n);

void Func_80931ec(int s0, int x0, int y0, int p0, int p1,
		  int s1, int x1, int y1, int p2, int p3)
{
	unsigned char *p;
	int a;
	int b;
	int h0;
	int h1;

	p = iwram_3001ebc;
	a = Func_8092ba8(s0);
	b = Func_8092ba8(s1);
	h0 = _Func_8017658((*(short *)(p + (0xec << 1)))++, x0, y0,
			   GetSpriteVoice(a) << 16);
	_Func_8019da8(a, 0, p0, p1);
	h1 = _Func_8017658((*(short *)(p + (0xec << 1)))++, x1, y1,
			   GetSpriteVoice(b) << 16);
	_Func_8019da8(b, 0, p2, p3);
	while (_Func_8017364() == 0)
		WaitFrames(1);
	WaitFrames(1);
	while ((gKeyPress & 0x303) == 0)
		WaitFrames(1);
	WaitFrames(1);
	_Func_8019e48(a);
	_Func_8019e48(b);
	_Func_8019a54();
	WaitFrames(1);
	while (_Func_8017394(h0) == 0)
		WaitFrames(1);
	while (_Func_8017394(h1) == 0)
		WaitFrames(1);
	WaitFrames(1);
}
