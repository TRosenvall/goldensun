/* OvlFunc_918_2009424 -- 0x02009424  -- LANDING, BYTE-IDENTICAL.
 *
 * MATCHES: objcmp --func "OK OvlFunc_918_2009424 -- 392 bytes, 141 encodings and
 * 55 relocations identical"; objcmp --whole the same, with the per-function line
 * "ok OvlFunc_918_2009424  141 encodings".  Was 4 of 141.
 * PINS 0 (shimcount clean, no inline asm).  FLAG-FREE.  DEVICE-FREE.
 *
 * Verify with: python3 tools/objcmp.py src/overlays/rom_7a5214/ovl_314_c_c_c_a_c.c asm/overlays/rom_7a5214/ovl_314_c_c_c_a_c.s --whole
 *
 * SPLIT: NONE.  grep -c func_start = 1 and tools/datacheck.py is CLEAN, so the
 * file lands WHOLE: no split, no linker-script edit, no exports.  Deleting
 * asm/overlays/rom_7a5214/ovl_314_c_c_c_a_c.s is the whole landing.
 *
 * ===== WHAT THE BLOCKER WAS: THE PARK'S OWN "KEEP THIS" ENTRY =====
 *
 * The park listed under "WHAT IS RIGHT AND SHOULD BE KEPT":
 *
 *     "The two fallthrough arms (case 4 into case 1, case 11 into case 8) are
 *      written as fallthroughs and come out as the ROM's `b`-less joins."
 *
 * That sentence was the blocker.  The joins are real, but the SOURCE does not
 * produce them by falling through -- it produces them by DUPLICATING the join
 * arm's body and letting CROSS-JUMPING merge the two copies.  Writing
 *
 *     case 4:  ... ; __WaitFrames(6); __MapActor_SetAnim(8, 1); break;
 *     case 1:        __MapActor_SetAnim(8, 1); break;
 *
 * gives the identical `b`-less join AND fixes the argument order at both
 * differing sites.  4 of 141 -> 0.  Same for case 11 into case 8 with anim 6.
 *
 * WHY THE ORDER MOVES WITH IT.  The park framed the residue as "position-
 * dependent scheduling inside one basic block" and read the ROM as uniformly
 * r1-first.  It is not.  The ROM's rule is visible once the whole stream is
 * read side by side:
 *
 *     r1-first  when another call follows in the same block   (idx 53, 58, 63, 72)
 *     r0-FIRST  at the block's last call                      (idx 68, 77)
 *
 * We always matched the r0-first sites.  The two sites we got wrong were the
 * last call of a FALLTHROUGH arm, where the fallthrough put the block boundary
 * in a place the ROM's source does not have one.  Duplicating the tail moves
 * the boundary; cross-jumping then removes the duplicate.  Threshold is two
 * matching insns before a shared jump (jump.c:675, :1602, :1607), and the
 * duplicated tail is `mov r1 / mov r0 / bl / b` -- comfortably over it.
 *
 * ===== MEASURED, AND ALL OF IT EXACTLY INERT AT 4 OF 141 (first idx 63) =====
 *
 *     do { } while (0) before the differing call, both sites        4  inert
 *     do { } while (0) after  the differing call, both sites        4  inert
 *     do { } while (0) before, case 4 only                         4  inert
 *     far-assigned locals for BOTH args at both sites              4  inert
 *     far-assigned local for the ANIM argument only                4  inert
 *     far-assigned local for the SLOT argument only                4  inert
 *     __asm__ volatile ("") before the differing call, both sites   4  inert
 *
 * So no barrier and no argument-naming lever touches this at all -- consistent
 * with the park's batch-205 finding that pins are inert at eight sites.  The
 * free variable was never the argument setup; it was the SHAPE OF THE JOIN.
 * CORRECTION TO THE PARK: its "Blocker class: argument-setup order" and its
 * "NEXT: nothing source-level" are both withdrawn.
 */
extern void __MapActor_SetAnim(int slot, int anim);
extern void __WaitFrames(int n);

void OvlFunc_918_2009424(int n)
{
    switch (n) {
    case 0:
        __MapActor_SetAnim(8, 1);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 3);
        break;
    case 2:
        __MapActor_SetAnim(8, 1);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 5);
        break;
    case 3:
        __MapActor_SetAnim(8, 1);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 4);
        break;
    case 4:
        __MapActor_SetAnim(8, 1);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 3);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 1);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 3);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 1);
        break;
    case 1:
        __MapActor_SetAnim(8, 1);
        break;
    case 5:
        __MapActor_SetAnim(8, 1);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 2);
        break;
    case 7:
        __MapActor_SetAnim(8, 6);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 8);
        break;
    case 9:
        __MapActor_SetAnim(8, 6);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 9);
        break;
    case 10:
        __MapActor_SetAnim(8, 6);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 0xa);
        break;
    case 11:
        __MapActor_SetAnim(8, 6);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 8);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 6);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 8);
        __WaitFrames(6);
        __MapActor_SetAnim(8, 6);
        break;
    case 8:
        __MapActor_SetAnim(8, 6);
        break;
    case 12:
        __MapActor_SetAnim(8, 6);
        break;
    }
    __WaitFrames(0xc);
}
