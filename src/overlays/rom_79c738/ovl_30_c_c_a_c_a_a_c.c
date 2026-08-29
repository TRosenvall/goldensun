/* Cluster OvlFunc_909_200828c..OvlFunc_909_200828c extracted from goldensun/asm/overlays/rom_79c738/ovl_30_c_c_a_c_a_a_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * A one-shot line of dialogue: say the standard message, add a second one the
 * first time through, then speak and set the flag.
 *
 * THIS WAS PARKED WITH C THAT WAS SEMANTICALLY WRONG, which is worth more than
 * the function. The parked version had __ActorMessage and __SetFlag INSIDE the
 * `if`:
 *
 *     if (__GetFlag(0x303)) { __MessageID(0x176c); __ActorMessage(0xf, 0);
 *                             __SetFlag(0x303); }
 *
 * The ROM runs both unconditionally -- its join label sits BEFORE the
 * __ActorMessage, not after the __SetFlag. So the parked code would have
 * skipped a line of speech and never set the flag on the path that matters.
 * The park note diagnosed constant-CSE, which was also true, and the wrong
 * control flow hid behind it for several rounds.
 *
 * That is the second time in this tree a park has carried a semantic error
 * under a plausible blocker diagnosis -- see the batch 20 report for the first.
 * A label position in the diff is worth reading before the instructions around
 * it.
 *
 * THE REMAINING DIFFICULTY IS REAL AND IS A BUILD FLAG. The flag id 0x303 is
 * used twice around a call, and at -O2 gcc hoists it into a callee-saved
 * register: a push, a pop and two moves spent to save one pool load. The ROM
 * loads it twice. This TU is built with -fno-rerun-cse-after-loop, which is the
 * specific pass responsible -- see the CSE_CFLAGS block in the Makefile for
 * which flags do NOT work and for the caveat on the evidence.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern int __GetFlag(int id);

void OvlFunc_909_200828c(void)
{
    __CutsceneStart();
    __MessageID(0x1756);
    if (__GetFlag(0x303))
        __MessageID(0x176c);
    __ActorMessage(0xf, 0);
    __SetFlag(0x303);
    __CutsceneEnd();
}
