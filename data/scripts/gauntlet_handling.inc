Gauntlet_SetupCamera::
# 9 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_TIANA, Gauntlet_SetupCamera_Movement_0
# 10 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_PLAYER, Gauntlet_SetupCamera_Movement_1
# 11 "data/scripts/gauntlet_handling.pory"
	waitmovement LOCALID_CAMERA
	return


Gauntlet_MovePlayerBeforeBattle::
# 16 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_TIANA, Gauntlet_SetupCamera_Movement_0
# 17 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_PLAYER, Gauntlet_MovePlayerBeforeBattle_Movement_0
# 18 "data/scripts/gauntlet_handling.pory"
	waitmovement LOCALID_PLAYER
	return


Gauntlet_OpponentMovement_BeforeBattle::
# 22 "data/scripts/gauntlet_handling.pory"
	switch VAR_GAUNTLET_POSITION
# 23 "data/scripts/gauntlet_handling.pory"
	case 0, Gauntlet_OpponentMovement_BeforeBattle_3
# 24 "data/scripts/gauntlet_handling.pory"
	case 1, Gauntlet_OpponentMovement_BeforeBattle_4
# 25 "data/scripts/gauntlet_handling.pory"
	case 2, Gauntlet_OpponentMovement_BeforeBattle_5
# 26 "data/scripts/gauntlet_handling.pory"
	case 3, Gauntlet_OpponentMovement_BeforeBattle_6
Gauntlet_OpponentMovement_BeforeBattle_1:
# 28 "data/scripts/gauntlet_handling.pory"
	waitmovement
	return

Gauntlet_OpponentMovement_BeforeBattle_3:
# 23 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_ONE, Gauntlet_OpponentMovement_BeforeBattle_Movement_0
	goto Gauntlet_OpponentMovement_BeforeBattle_1

Gauntlet_OpponentMovement_BeforeBattle_4:
# 24 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_TWO, Gauntlet_OpponentMovement_BeforeBattle_Movement_0
	goto Gauntlet_OpponentMovement_BeforeBattle_1

Gauntlet_OpponentMovement_BeforeBattle_5:
# 25 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_THREE, Gauntlet_OpponentMovement_BeforeBattle_Movement_0
	goto Gauntlet_OpponentMovement_BeforeBattle_1

Gauntlet_OpponentMovement_BeforeBattle_6:
# 26 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_FOUR, Gauntlet_OpponentMovement_BeforeBattle_Movement_0
	goto Gauntlet_OpponentMovement_BeforeBattle_1


Gauntlet_PostBattle::
# 32 "data/scripts/gauntlet_handling.pory"
	call Gauntlet_CheckBattleResult
# 33 "data/scripts/gauntlet_handling.pory"
	call Gauntlet_OpponentMovement_AfterBattle
# 34 "data/scripts/gauntlet_handling.pory"
	call Gauntlet_Attendant
	return


Gauntlet_OpponentMovement_AfterBattle::
# 38 "data/scripts/gauntlet_handling.pory"
	switch VAR_GAUNTLET_POSITION
# 39 "data/scripts/gauntlet_handling.pory"
	case 0, Gauntlet_OpponentMovement_AfterBattle_3
# 40 "data/scripts/gauntlet_handling.pory"
	case 1, Gauntlet_OpponentMovement_AfterBattle_4
# 41 "data/scripts/gauntlet_handling.pory"
	case 2, Gauntlet_OpponentMovement_AfterBattle_5
# 42 "data/scripts/gauntlet_handling.pory"
	case 3, Gauntlet_OpponentMovement_AfterBattle_6
Gauntlet_OpponentMovement_AfterBattle_1:
# 44 "data/scripts/gauntlet_handling.pory"
	waitmovement
	return

Gauntlet_OpponentMovement_AfterBattle_3:
# 39 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_ONE, Gauntlet_SetupCamera_Movement_1
	goto Gauntlet_OpponentMovement_AfterBattle_1

Gauntlet_OpponentMovement_AfterBattle_4:
# 40 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_TWO, Gauntlet_SetupCamera_Movement_1
	goto Gauntlet_OpponentMovement_AfterBattle_1

Gauntlet_OpponentMovement_AfterBattle_5:
# 41 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_TWO, Gauntlet_SetupCamera_Movement_1
	goto Gauntlet_OpponentMovement_AfterBattle_1

Gauntlet_OpponentMovement_AfterBattle_6:
# 42 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_THREE, Gauntlet_SetupCamera_Movement_1
	goto Gauntlet_OpponentMovement_AfterBattle_1


Gauntlet_Attendant::
# 49 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_TIANA, Gauntlet_Attendant_Movement_0
# 50 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_PLAYER, Gauntlet_Attendant_Movement_1
# 51 "data/scripts/gauntlet_handling.pory"
	waitmovement LOCALID_PLAYER
# 52 "data/scripts/gauntlet_handling.pory"
	msgbox Gauntlet_Attendant_Text_0
# 53 "data/scripts/gauntlet_handling.pory"
	goto Gauntlet_Multichoice
	return


Gauntlet_Multichoice::
# 57 "data/scripts/gauntlet_handling.pory"
	dynmultichoice 12, 0, TRUE, 4, 0, DYN_MULTICHOICE_CB_NONE, Gauntlet_Multichoice_Text_0, Gauntlet_Multichoice_Text_1, Gauntlet_Multichoice_Text_2, Gauntlet_Multichoice_Text_3
# 62 "data/scripts/gauntlet_handling.pory"
	switch VAR_RESULT
# 63 "data/scripts/gauntlet_handling.pory"
	case 0, Gauntlet_Multichoice_2
# 65 "data/scripts/gauntlet_handling.pory"
	case 1, Gauntlet_Multichoice_3
# 68 "data/scripts/gauntlet_handling.pory"
	case 2, Gauntlet_Multichoice_4
# 72 "data/scripts/gauntlet_handling.pory"
	case 3, Gauntlet_Multichoice_5
	return

Gauntlet_Multichoice_2:
# 64 "data/scripts/gauntlet_handling.pory"
	goto Gauntlet_Confirmation
	return

Gauntlet_Multichoice_3:
# 66 "data/scripts/gauntlet_handling.pory"
	msgbox Gauntlet_Multichoice_Text_4
# 67 "data/scripts/gauntlet_handling.pory"
	goto Gauntlet_Multichoice_Loop
	return

Gauntlet_Multichoice_4:
# 69 "data/scripts/gauntlet_handling.pory"
	callnative OpenPartyGauntlet
# 70 "data/scripts/gauntlet_handling.pory"
	waitstate
# 71 "data/scripts/gauntlet_handling.pory"
	goto Gauntlet_Multichoice_Loop
	return

Gauntlet_Multichoice_5:
# 73 "data/scripts/gauntlet_handling.pory"
	special HealPlayerParty
# 74 "data/scripts/gauntlet_handling.pory"
	warpsilent MAP_CONVENTION_CENTER, 21, 24
# 75 "data/scripts/gauntlet_handling.pory"
	waitstate
	return


Gauntlet_Multichoice_Loop::
# 80 "data/scripts/gauntlet_handling.pory"
	msgbox Gauntlet_Multichoice_Loop_Text_0
# 81 "data/scripts/gauntlet_handling.pory"
	goto Gauntlet_Multichoice
	return


Gauntlet_Confirmation::
# 85 "data/scripts/gauntlet_handling.pory"
	msgbox Gauntlet_Confirmation_Text_0
# 86 "data/scripts/gauntlet_handling.pory"
	callnative PopulateRandomYesNoChoice
# 87 "data/scripts/gauntlet_handling.pory"
	dynmultistack 0, 0, TRUE, 2, FALSE, 0, DYN_MULTICHOICE_CB_NONE
# 88 "data/scripts/gauntlet_handling.pory"
	switch VAR_RESULT
# 89 "data/scripts/gauntlet_handling.pory"
	case 0, Gauntlet_Confirmation_2
# 92 "data/scripts/gauntlet_handling.pory"
	case 1, Gauntlet_Confirmation_3
	return

Gauntlet_Confirmation_2:
# 90 "data/scripts/gauntlet_handling.pory"
	closemessage
	return

Gauntlet_Confirmation_3:
# 93 "data/scripts/gauntlet_handling.pory"
	closemessage
# 94 "data/scripts/gauntlet_handling.pory"
	goto Gauntlet_Multichoice_Loop
	return


Gauntlet_CheckBattleResult::
# 99 "data/scripts/gauntlet_handling.pory"
	switch VAR_RESULT
# 106 "data/scripts/gauntlet_handling.pory"
	case 1, Gauntlet_CheckBattleResult_4
# 101 "data/scripts/gauntlet_handling.pory"
	special HealPlayerParty
# 102 "data/scripts/gauntlet_handling.pory"
	msgbox Gauntlet_CheckBattleResult_Text_0
# 103 "data/scripts/gauntlet_handling.pory"
	waitmessage
# 104 "data/scripts/gauntlet_handling.pory"
	warpsilent MAP_CONVENTION_CENTER, 21, 24
# 105 "data/scripts/gauntlet_handling.pory"
	waitstate
Gauntlet_CheckBattleResult_1:
# 109 "data/scripts/gauntlet_handling.pory"
	compare VAR_GAUNTLET_POSITION, 4
	goto_if_eq Gauntlet_CheckBattleResult_5
	return

Gauntlet_CheckBattleResult_4:
# 107 "data/scripts/gauntlet_handling.pory"
	addvar VAR_GAUNTLET_POSITION, 1
	goto Gauntlet_CheckBattleResult_1

Gauntlet_CheckBattleResult_5:
# 110 "data/scripts/gauntlet_handling.pory"
	special HealPlayerParty
# 111 "data/scripts/gauntlet_handling.pory"
	msgbox Gauntlet_CheckBattleResult_Text_1
# 112 "data/scripts/gauntlet_handling.pory"
	waitmessage
# 113 "data/scripts/gauntlet_handling.pory"
	addmoney 500
# 114 "data/scripts/gauntlet_handling.pory"
	victoryscreen GAUNTLET_ABYSSAL_HUNTERS, FALSE
# 115 "data/scripts/gauntlet_handling.pory"
	waitstate
# 116 "data/scripts/gauntlet_handling.pory"
	warpsilent MAP_CONVENTION_CENTER, 21, 24
# 117 "data/scripts/gauntlet_handling.pory"
	waitstate
	return


# 9 "data/scripts/gauntlet_handling.pory"
Gauntlet_SetupCamera_Movement_0:
# 9 "data/scripts/gauntlet_handling.pory"
	walk_right
# 9 "data/scripts/gauntlet_handling.pory"
	walk_right
	step_end

# 10 "data/scripts/gauntlet_handling.pory"
Gauntlet_SetupCamera_Movement_1:
# 10 "data/scripts/gauntlet_handling.pory"
	walk_right
# 10 "data/scripts/gauntlet_handling.pory"
	walk_right
# 10 "data/scripts/gauntlet_handling.pory"
	walk_right
	step_end

# 17 "data/scripts/gauntlet_handling.pory"
Gauntlet_MovePlayerBeforeBattle_Movement_0:
# 17 "data/scripts/gauntlet_handling.pory"
	walk_right
# 17 "data/scripts/gauntlet_handling.pory"
	walk_right
# 17 "data/scripts/gauntlet_handling.pory"
	walk_right
# 17 "data/scripts/gauntlet_handling.pory"
	walk_right
	step_end

# 23 "data/scripts/gauntlet_handling.pory"
Gauntlet_OpponentMovement_BeforeBattle_Movement_0:
# 23 "data/scripts/gauntlet_handling.pory"
	walk_left
# 23 "data/scripts/gauntlet_handling.pory"
	walk_left
# 23 "data/scripts/gauntlet_handling.pory"
	walk_left
	step_end

# 49 "data/scripts/gauntlet_handling.pory"
Gauntlet_Attendant_Movement_0:
# 49 "data/scripts/gauntlet_handling.pory"
	walk_left
# 49 "data/scripts/gauntlet_handling.pory"
	walk_left
# 49 "data/scripts/gauntlet_handling.pory"
	face_up
	step_end

# 50 "data/scripts/gauntlet_handling.pory"
Gauntlet_Attendant_Movement_1:
# 50 "data/scripts/gauntlet_handling.pory"
	walk_left
# 50 "data/scripts/gauntlet_handling.pory"
	walk_left
# 50 "data/scripts/gauntlet_handling.pory"
	walk_left
# 50 "data/scripts/gauntlet_handling.pory"
	walk_left
	step_end

Gauntlet_Attendant_Text_0:
# 52 "data/scripts/gauntlet_handling.pory"
	.string "Well done! your next opponent will be X$"

Gauntlet_Multichoice_Text_0:
# 58 "data/scripts/gauntlet_handling.pory"
	.string "I'm good to go!$"

Gauntlet_Multichoice_Text_1:
# 59 "data/scripts/gauntlet_handling.pory"
	.string "Save progress$"

Gauntlet_Multichoice_Text_2:
# 60 "data/scripts/gauntlet_handling.pory"
	.string "Adjust party members$"

Gauntlet_Multichoice_Text_3:
# 61 "data/scripts/gauntlet_handling.pory"
	.string "Quit$"

Gauntlet_Multichoice_Text_4:
# 66 "data/scripts/gauntlet_handling.pory"
	.string "Save game$"

Gauntlet_Multichoice_Loop_Text_0:
# 80 "data/scripts/gauntlet_handling.pory"
	.string "Anything else?$"

Gauntlet_Confirmation_Text_0:
# 85 "data/scripts/gauntlet_handling.pory"
	.string "Are you ready for the next fight?$"

Gauntlet_CheckBattleResult_Text_0:
# 102 "data/scripts/gauntlet_handling.pory"
	.string "wat$"

Gauntlet_CheckBattleResult_Text_1:
# 111 "data/scripts/gauntlet_handling.pory"
	.string "You got ¥500 for winning!$"
