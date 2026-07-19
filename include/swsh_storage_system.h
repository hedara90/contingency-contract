#ifndef GUARD_SWSH_STORAGE_SYSTEM_H
#define GUARD_SWSH_STORAGE_SYSTEM_H

#include "constants/form_change_types.h"
#include "pokemon_storage_system.h"

#define SWSH_STORAGE_SYSTEM           TRUE

#if TOTAL_BOXES_COUNT > 15
#define SWSH_STORAGE_CHOOSE_BOX_GRID  FALSE // fits only 5×3 grid; auto disable if > 15 boxes
#else
#define SWSH_STORAGE_CHOOSE_BOX_GRID  TRUE  // manually turns grid on or off
#endif

#define SWSH_STORAGE_SCROLLING_BG     TRUE

void ShowPokemonStorageSystemPC_SwSh(void);
void ShowPokemonPCFromParty_SwSh(void);
void ChooseMonFromStorage_SwSh(void);
void SetMonFormPSS_SwSh(struct BoxPokemon *boxMon, enum FormChanges method);
void SetMonFormPSS_ItemHold_SwSh(struct BoxPokemon *boxMon);
void UpdateSpeciesSpritePSS_SwSh(struct BoxPokemon *boxMon);

#endif // GUARD_SWSH_STORAGE_SYSTEM_H
