#ifndef GUARD_COMBATANT_H
#define GUARD_COMBATANT_H

#include "types.h"

/* The combatant record -- 0x14C bytes, one per party member or enemy.
 *
 * Func_77394 (GetCombatantRecord) resolves an id to one of these and is the
 * most-called cross-module function in the ROM, at 226 call sites:
 *
 *     ids 0x00..0x07   ewram_500 + id * 0x14C
 *     ids 0x80..0x85   [iwram_1f28] + id * 0x14C - 0xA600, so 0x80 lands at
 *                      offset 0 of that block
 *
 * Enemy ids return 0 when iwram_1f28 is null -- that is, outside battle.  Any
 * code walking enemy records must handle the null, and several ROM callers do
 * exactly that check.
 *
 * Only the fields the annotations establish are named.  Func_77428
 * (BuildCharacterSummary) is where the derived stats are computed, and at 1023
 * lines it is the largest routine in rom_77000 -- read it when a displayed
 * number does not match a stored one.
 */

typedef struct Combatant
{
    /* 0x00 */ u8 unk_00[0x0F];
    /* 0x0F */ u8 level;            /* Func_792fc raises this ONE LEVEL AT A
                                       TIME toward a target so every per-level
                                       effect fires                           */
    /* 0x10 */ s16 statA;           /* the signed pair Func_77428 reads first  */
    /* 0x12 */ s16 statB;
    /* 0x14 */ u8 unk_14[0x04];
    /* 0x18 */ u16 statC;           /* the three halfwords the summary uses    */
    /* 0x1A */ u16 statD;
    /* 0x1C */ u16 statE;
    /* 0x1E */ u8 statF;
    /* 0x1F */ u8 nibbleFields;     /* two 4-bit fields packed together        */
    /* 0x20 */ u8 unk_20[0x12C];
} Combatant;

/* Sanity: the record is 0x14C bytes. */
typedef char Combatant_size_check[sizeof(Combatant) == 0x14C ? 1 : -1];

#endif /* GUARD_COMBATANT_H */
