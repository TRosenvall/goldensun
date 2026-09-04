/* Func_80b04dc  --  0x080b04dc
 *
 * Cut out of goldensun/asm/rom_b0000/rom_b0070_a_a_c_a_c_a.s.
 *
 * Speaks one line and waits for it to finish. The caller passes a base message
 * id; three pieces of party state shift it to a variant -- the value at +0x3a9
 * selects one of two alternates, and a flag at +0x3ac a third -- then the line
 * is queued with the speaker's voice packed into the high half of the fourth
 * argument, and the function spins until the text engine reports done.
 *
 * THE RUNTIME SUBTRACTION OF TWO CONSTANTS IS THE WHOLE READING. The ROM does
 *
 *     ldr r3, =0xcc6 / ldr r2, =0xc9b / sub r3, r2 / add r5, r3
 *
 * -- two pool loads and a subtract, to compute 0x2b. gcc folds literal
 * arithmetic, so this cannot be two integers. It is the difference of two
 * SYMBOL ADDRESSES, which gcc has no way to fold: the message ids live in
 * message.sym as absolute symbols and the established idiom in this tree is
 * `extern int _MSG_xxx;` with `(int)(&_MSG_xxx)`. Written that way the pool
 * loads and the subtract fall out exactly.
 *
 * That also means the source is expressing an OFFSET FROM A BASE LINE rather
 * than three unrelated ids -- `msg += _MSG_cc6 - _MSG_c9b` shifts whatever the
 * caller passed by the same distance that separates two known lines. Reading it
 * as `msg = 0xcc6` would have compiled to a single pool load and lost that.
 *
 * `_MSG_cf1` was not in message.sym and is added here, which is the recorded
 * one-line practice for this family.
 *
 * TWO BYTE READS, TWO DIFFERENT INSTRUCTION SEQUENCES, and the source has to
 * say which. The field at +0x3a9 is read with `ldrsb` through a register offset
 * -- ldrsb has no immediate-offset form, hence the `mov r1, #0` first -- while
 * the flag at +0x3ac is read with `ldrb` and an explicit `lsl #24 / asr #24`.
 * Both are signed byte reads, and gcc picks between them on where the value
 * lands: read straight into an `int` it uses ldrsb, but assigned to a
 * `signed char` LOCAL first it emits the unsigned load and widens afterwards.
 * Getting this wrong is two instructions and nothing else, which makes it easy
 * to misread as noise.
 *
 * The wait is `while (_Func_8017364() == 0) WaitFrames(1);` followed by one
 * more WaitFrames -- the ROM's `b` into the middle of the loop is gcc's own
 * rotation, not a source `do`/`while`.
 */

extern unsigned char *iwram_3001f2c;
extern int _MSG_c9b;
extern int _MSG_cc6;
extern int _MSG_cf1;
extern int _MSG_d4c;
extern int _GetSpriteVoice(int id);
extern void _Func_8019a54(void);
extern void _Func_8017658(int a, int b, int c, int d);
extern int _Func_8017364(void);
extern void WaitFrames(int n);

void Func_80b04dc(int msg)
{
    unsigned char *s;
    int voice;
    int k;
    signed char t;

    s = iwram_3001f2c;
    voice = _GetSpriteVoice(*(unsigned short *)(s + (0xe9 << 2)));
    _Func_8019a54();
    k = (signed char)s[0x3a9];
    if (k == 2)
        msg += (int)(&_MSG_cc6) - (int)(&_MSG_c9b);
    if (k == 0)
        msg += (int)(&_MSG_cf1) - (int)(&_MSG_c9b);
    t = s[0xeb << 2];
    if (t != 0)
        msg += (int)(&_MSG_d4c) - (int)(&_MSG_c9b);
    _Func_8017658(msg, 5, 0, (voice << 16) | 0x22);
    while (_Func_8017364() == 0)
        WaitFrames(1);
    WaitFrames(1);
}
