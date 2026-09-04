/* OvlFunc_959_200a718 -- 0x0200a718,
 * asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_c.s
 *
 * A cutscene beat: give slots 2, 3 and 1 the same speed and send each to its
 * own mark, wait for each to arrive and turn it to 0xc000, then turn slot 0 as
 * well.
 *
 * 55 of 62. Not a new blocker -- it is the straight-line repeated-constant
 * class at a scale that makes the usual remedy uneconomic, and it is recorded
 * for that reason rather than for anything novel.
 *
 * THE SHAPE. The function has NO BRANCH anywhere, and it uses three
 * multi-instruction constants over and over:
 *
 *     0x80 << 9   three times (the speed x)
 *     0x80 << 8   three times (the speed y)
 *     0xc0 << 8   four times  (the facing angle)
 *
 * With no branch, nothing dominates the repeats, so cse1 commons every one of
 * them into a register and gcse cprop -- strictly cross-block -- cannot undo
 * it. That is the recorded rule, and it predicts exactly what the screen shows.
 *
 * WHY IT IS PARKED RATHER THAN FAKEMATCHED. The idiom would work; the sibling
 * OvlFunc_959_200c704 in this same overlay took it and matched. But that
 * function had ONE repeated constant and needed three pin blocks plus a
 * barrier. This one has TEN repeats across three distinct constants, and the
 * "anchor every argument of any call you anchor any argument of" rule means
 * pinning whole argument lists, so it would carry roughly ten pin blocks in 62
 * instructions. That is within house norms by raw count -- the heaviest
 * existing fakematch carries seventeen launders -- but the cost is in building
 * and then tearing each one down to prove it load-bearing, which is the
 * discipline that keeps a fakematch honest.
 *
 * So this is a judgement call, not a wall: a competent fakematch is available
 * and was declined as poor value against the ~1,000 functions still open. If
 * the fakematch backlog is ever worked deliberately, this is a good candidate
 * -- the structure is regular, the three constants are independent, and the
 * sibling next door is a working template for the idiom.
 *
 * NOT TRIED, because the plain floor was measured first and the decision
 * followed from it: any pin, any barrier, any two-step spelling.
 */

extern void __MapActor_SetSpeed(int slot, int vx, int vy);
extern void __Func_809218c(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8092adc(int slot, int angle, int frames);

void OvlFunc_959_200a718(void)
{
    __MapActor_SetSpeed(2, 0x80 << 9, 0x80 << 8);
    __Func_809218c(2, 0xfc << 1, 0xd8);
    __MapActor_SetSpeed(3, 0x80 << 9, 0x80 << 8);
    __Func_809218c(3, 0xdc << 1, 0xe8);
    __MapActor_SetSpeed(1, 0x80 << 9, 0x80 << 8);
    __Func_809218c(1, 0xf0 << 1, 0xe0);
    __MapActor_WaitMovement(1);
    __Func_8092adc(1, 0xc0 << 8, 0);
    __MapActor_WaitMovement(2);
    __Func_8092adc(2, 0xc0 << 8, 0);
    __MapActor_WaitMovement(3);
    __Func_8092adc(3, 0xc0 << 8, 0);
    __Func_8092adc(0, 0xc0 << 8, 0);
}
