#ifndef GUARD_SCENE_H
#define GUARD_SCENE_H

#include "types.h"

/* The scene / dialogue state block.
 *
 * `iwram_1ebc` is a POINTER to it, so every access in the ROM reads
 * [iwram_1ebc] first and then adds an offset.  In assembly that shows up as:
 *
 *     ldr  r3, =iwram_1ebc
 *     ldr  r3, [r3]            @ the block
 *     add  r3, #0x1C0          @ a field in it
 *
 * This is the block Func_916b0 initialises when a cutscene opens and Func_91750
 * tears down when it closes, and the one every overlay's map-load entry writes
 * its step delay into.  It is also the widest-referenced structure in the
 * project after Entity.
 *
 * The total size is not established -- offsets as high as +0x236 are read, and
 * nothing bounds it -- so this is deliberately NOT a struct.  Declaring one
 * would mean inventing a size and inventing every hole between the fields we
 * know.  The offsets below are what the annotations actually establish; use
 * them against a u8 * until enough of the gaps are filled to justify a layout.
 */

/* The block pointer itself. */
extern void *iwram_1ebc;

#define SCENE_BLOCK()  (*(u8 **)&iwram_1ebc)

/* --- scratch and slots ------------------------------------------------- */
#define SCENE_SCRATCH_WORD      0x010  /* general-purpose word map scripts use */
#define SCENE_SLOT_TABLE        0x014  /* entity slot table; the scene buffer
                                          also hangs off it at +0x3400        */

/* --- interaction ------------------------------------------------------- */
/* Func_8bc44 zeroes all four of these between interactions. */
#define SCENE_INTERACT_TARGET   0x16C  /* halfword: what was interacted with  */
#define SCENE_INTERACT_KIND     0x16E  /* halfword: Func_955b0 writes here    */
#define SCENE_PENDING_MESSAGE   0x170  /* halfword: line a trigger wants shown;
                                          0x3E7 is the "none" value           */
#define SCENE_INTERACT_EXTRA    0x172

/* --- movement and transitions ------------------------------------------ */
#define SCENE_TILE_LIST         0x118  /* walked backwards by Func_8b674      */
#define SCENE_CURRENT_TILE      0x11C  /* tile the player stands on, or -1    */
#define SCENE_TRANSITION_CACHE  0x17C
#define SCENE_STEP_EFFECT       0x1A0

/* --- scene mode and overlay -------------------------------------------- */
#define SCENE_MODE              0x19E  /* mode 3 skips the field path         */
#define SCENE_OVERLAY_ACTIVE    0x1C6  /* 1 while the screen overlay is up    */

/* --- pacing ------------------------------------------------------------ */
#define SCENE_STEP_DELAY        0x1C0  /* every overlay's map-load entry sets
                                          this; 0x100, 0x201, 0x204, 0x209
                                          are the values seen                 */
#define SCENE_MESSAGE_DELAY     0x1C8  /* frames Func_91e20 blocks for.  Unlike
                                          Func_9163c this ignores fast-forward */
#define SCENE_FAST_FORWARD      0x1CC  /* set while the player holds R; L
                                          (0x200) clears it.  Func_9163c
                                          honours it, Func_91e20 does not      */

/* --- messages ---------------------------------------------------------- */
#define SCENE_ACTIVE_MESSAGE    0x1D8  /* halfword: the id the dialogue system
                                          opens next.  Some handlers INCREMENT
                                          this rather than setting a flag --
                                          a running count across visits        */
#define SCENE_MESSAGE_SLOT_A    0x1DA  /* three id slots Func_916b0 resets to  */
#define SCENE_MESSAGE_SLOT_B    0x1DC  /* their "none" value                   */
#define SCENE_MESSAGE_SLOT_C    0x1DE

/* --- message window ---------------------------------------------------- */
#define SCENE_WINDOW_ACTIVE     0xCB6  /* Func_8e118 clears it                 */
#define SCENE_PROMPT_COUNT_A    0xCC2  /* the two counters Func_28df4 drives    */
#define SCENE_PROMPT_COUNT_B    0xCC4
#define SCENE_LOOPING_SOUND     0xCC8  /* id Func_91ff0 cached for Func_9202c
                                          to stop; -1 means none               */

#endif /* GUARD_SCENE_H */
