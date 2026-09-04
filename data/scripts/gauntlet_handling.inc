Gauntlet_SetupCamera::
# 9 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_TIANA, Gauntlet_SetupCamera_Movement_0
# 10 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_PLAYER, Gauntlet_SetupCamera_Movement_1
# 11 "data/scripts/gauntlet_handling.pory"
	waitmovement LOCALID_PLAYER
	return


Gauntlet_MovePlayerBeforeBattle::
# 16 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_TIANA, Gauntlet_SetupCamera_Movement_0
# 17 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_PLAYER, Gauntlet_MovePlayerBeforeBattle_Movement_0
# 18 "data/scripts/gauntlet_handling.pory"
	waitmovement LOCALID_PLAYER
	return


Gauntlet_MoveOpponentBeforeBattle::
# 22 "data/scripts/gauntlet_handling.pory"
	switch VAR_GAUNTLET_POSITION
# 23 "data/scripts/gauntlet_handling.pory"
	case 0, Gauntlet_MoveOpponentBeforeBattle_3
# 26 "data/scripts/gauntlet_handling.pory"
	case 1, Gauntlet_MoveOpponentBeforeBattle_4
# 29 "data/scripts/gauntlet_handling.pory"
	case 2, Gauntlet_MoveOpponentBeforeBattle_5
# 32 "data/scripts/gauntlet_handling.pory"
	case 3, Gauntlet_MoveOpponentBeforeBattle_6
Gauntlet_MoveOpponentBeforeBattle_1:
# 36 "data/scripts/gauntlet_handling.pory"
	waitmovement
	return

Gauntlet_MoveOpponentBeforeBattle_3:
# 24 "data/scripts/gauntlet_handling.pory"
	addobject LOCALID_ONE
# 25 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_ONE, Gauntlet_MoveOpponentBeforeBattle_Movement_0
	goto Gauntlet_MoveOpponentBeforeBattle_1

Gauntlet_MoveOpponentBeforeBattle_4:
# 27 "data/scripts/gauntlet_handling.pory"
	addobject LOCALID_TWO
# 28 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_TWO, Gauntlet_MoveOpponentBeforeBattle_Movement_0
	goto Gauntlet_MoveOpponentBeforeBattle_1

Gauntlet_MoveOpponentBeforeBattle_5:
# 30 "data/scripts/gauntlet_handling.pory"
	addobject LOCALID_THREE
# 31 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_THREE, Gauntlet_MoveOpponentBeforeBattle_Movement_0
	goto Gauntlet_MoveOpponentBeforeBattle_1

Gauntlet_MoveOpponentBeforeBattle_6:
# 33 "data/scripts/gauntlet_handling.pory"
	addobject LOCALID_FOUR
# 34 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_FOUR, Gauntlet_MoveOpponentBeforeBattle_Movement_0
	goto Gauntlet_MoveOpponentBeforeBattle_1


Gauntlet_PostBattle::
# 40 "data/scripts/gauntlet_handling.pory"
	call Gauntlet_MoveOpponentAfterBattle
# 41 "data/scripts/gauntlet_handling.pory"
	call Gauntlet_Attendant
	return


Gauntlet_MoveOpponentAfterBattle::
# 45 "data/scripts/gauntlet_handling.pory"
	switch VAR_GAUNTLET_POSITION
# 46 "data/scripts/gauntlet_handling.pory"
	case 1, Gauntlet_MoveOpponentAfterBattle_3
# 47 "data/scripts/gauntlet_handling.pory"
	case 2, Gauntlet_MoveOpponentAfterBattle_4
# 48 "data/scripts/gauntlet_handling.pory"
	case 3, Gauntlet_MoveOpponentAfterBattle_5
Gauntlet_MoveOpponentAfterBattle_1:
# 51 "data/scripts/gauntlet_handling.pory"
	waitmovement
	return

Gauntlet_MoveOpponentAfterBattle_3:
# 46 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_ONE, Gauntlet_SetupCamera_Movement_1
	goto Gauntlet_MoveOpponentAfterBattle_1

Gauntlet_MoveOpponentAfterBattle_4:
# 47 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_TWO, Gauntlet_SetupCamera_Movement_1
	goto Gauntlet_MoveOpponentAfterBattle_1

Gauntlet_MoveOpponentAfterBattle_5:
# 48 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_THREE, Gauntlet_SetupCamera_Movement_1
	goto Gauntlet_MoveOpponentAfterBattle_1


Gauntlet_Attendant::
# 56 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_TIANA, Gauntlet_Attendant_Movement_0
# 57 "data/scripts/gauntlet_handling.pory"
	applymovement LOCALID_PLAYER, Gauntlet_Attendant_Movement_1
# 58 "data/scripts/gauntlet_handling.pory"
	waitmovement LOCALID_PLAYER
# 59 "data/scripts/gauntlet_handling.pory"
	msgbox Gauntlet_Attendant_Text_0
# 60 "data/scripts/gauntlet_handling.pory"
	goto Gauntlet_Multichoice
	return


Gauntlet_Multichoice::
# 64 "data/scripts/gauntlet_handling.pory"
	dynmultichoice 12, 0, TRUE, 4, 0, DYN_MULTICHOICE_CB_NONE, Gauntlet_Multichoice_Text_0, Gauntlet_Multichoice_Text_1, Gauntlet_Multichoice_Text_2, Gauntlet_Multichoice_Text_3
# 69 "data/scripts/gauntlet_handling.pory"
	switch VAR_RESULT
# 70 "data/scripts/gauntlet_handling.pory"
	case 0, Gauntlet_Multichoice_2
# 72 "data/scripts/gauntlet_handling.pory"
	case 1, Gauntlet_Multichoice_3
# 75 "data/scripts/gauntlet_handling.pory"
	case 2, Gauntlet_Multichoice_4
# 79 "data/scripts/gauntlet_handling.pory"
	case 3, Gauntlet_Multichoice_5
	return

Gauntlet_Multichoice_2:
# 71 "data/scripts/gauntlet_handling.pory"
	goto Gauntlet_Confirmation
	return

Gauntlet_Multichoice_3:
# 73 "data/scripts/gauntlet_handling.pory"
	msgbox Gauntlet_Multichoice_Text_4
# 74 "data/scripts/gauntlet_handling.pory"
	goto Gauntlet_Multichoice_Loop
	return

Gauntlet_Multichoice_4:
# 76 "data/scripts/gauntlet_handling.pory"
	callnative OpenPartyGauntlet
# 77 "data/scripts/gauntlet_handling.pory"
	waitstate
# 78 "data/scripts/gauntlet_handling.pory"
	goto Gauntlet_Multichoice_Loop
	return

Gauntlet_Multichoice_5:
# 80 "data/scripts/gauntlet_handling.pory"
	special HealPlayerParty
# 81 "data/scripts/gauntlet_handling.pory"
	warpsilent MAP_CONVENTION_CENTER, 21, 24
# 82 "data/scripts/gauntlet_handling.pory"
	waitstate
	return


Gauntlet_Multichoice_Loop::
# 87 "data/scripts/gauntlet_handling.pory"
	msgbox Gauntlet_Multichoice_Loop_Text_0
# 88 "data/scripts/gauntlet_handling.pory"
	goto Gauntlet_Multichoice
	return


Gauntlet_Confirmation::
# 92 "data/scripts/gauntlet_handling.pory"
	msgbox Gauntlet_Confirmation_Text_0
# 93 "data/scripts/gauntlet_handling.pory"
	callnative PopulateRandomYesNoChoice
# 94 "data/scripts/gauntlet_handling.pory"
	dynmultistack 0, 0, TRUE, 2, FALSE, 0, DYN_MULTICHOICE_CB_NONE
# 95 "data/scripts/gauntlet_handling.pory"
	switch VAR_RESULT
# 96 "data/scripts/gauntlet_handling.pory"
	case 0, Gauntlet_Confirmation_2
# 99 "data/scripts/gauntlet_handling.pory"
	case 1, Gauntlet_Confirmation_3
	return

Gauntlet_Confirmation_2:
# 97 "data/scripts/gauntlet_handling.pory"
	closemessage
	return

Gauntlet_Confirmation_3:
# 100 "data/scripts/gauntlet_handling.pory"
	closemessage
# 101 "data/scripts/gauntlet_handling.pory"
	goto Gauntlet_Multichoice_Loop
	return


Gauntlet_BattleWon::
# 106 "data/scripts/gauntlet_handling.pory"
	closemessage
# 107 "data/scripts/gauntlet_handling.pory"
	addvar VAR_GAUNTLET_POSITION, 1
	return


Gauntlet_BattleLost::
# 111 "data/scripts/gauntlet_handling.pory"
	closemessage
# 112 "data/scripts/gauntlet_handling.pory"
	special HealPlayerParty
# 113 "data/scripts/gauntlet_handling.pory"
	msgbox Gauntlet_BattleLost_Text_0
# 114 "data/scripts/gauntlet_handling.pory"
	waitmessage
# 115 "data/scripts/gauntlet_handling.pory"
	warpsilent MAP_CONVENTION_CENTER, 21, 24
# 116 "data/scripts/gauntlet_handling.pory"
	waitstate
	return


Gauntlet_AfterGauntletWin::
# 120 "data/scripts/gauntlet_handling.pory"
	closemessage
# 121 "data/scripts/gauntlet_handling.pory"
	special HealPlayerParty
# 122 "data/scripts/gauntlet_handling.pory"
	msgbox Gauntlet_AfterGauntletWin_Text_0
# 123 "data/scripts/gauntlet_handling.pory"
	waitmessage
# 124 "data/scripts/gauntlet_handling.pory"
	addmoney 500
	return


Gauntlet_WarpOutAfterWin::
# 129 "data/scripts/gauntlet_handling.pory"
	warpsilent MAP_CONVENTION_CENTER, 21, 24
# 130 "data/scripts/gauntlet_handling.pory"
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

# 25 "data/scripts/gauntlet_handling.pory"
Gauntlet_MoveOpponentBeforeBattle_Movement_0:
# 25 "data/scripts/gauntlet_handling.pory"
	walk_left
# 25 "data/scripts/gauntlet_handling.pory"
	walk_left
# 25 "data/scripts/gauntlet_handling.pory"
	walk_left
	step_end

# 56 "data/scripts/gauntlet_handling.pory"
Gauntlet_Attendant_Movement_0:
# 56 "data/scripts/gauntlet_handling.pory"
	walk_left
# 56 "data/scripts/gauntlet_handling.pory"
	walk_left
# 56 "data/scripts/gauntlet_handling.pory"
	face_up
	step_end

# 57 "data/scripts/gauntlet_handling.pory"
Gauntlet_Attendant_Movement_1:
# 57 "data/scripts/gauntlet_handling.pory"
	walk_left
# 57 "data/scripts/gauntlet_handling.pory"
	walk_left
# 57 "data/scripts/gauntlet_handling.pory"
	walk_left
# 57 "data/scripts/gauntlet_handling.pory"
	walk_left
	step_end

Gauntlet_Attendant_Text_0:
# 59 "data/scripts/gauntlet_handling.pory"
	.string "Well done! your next opponent will be X$"

Gauntlet_Multichoice_Text_0:
# 65 "data/scripts/gauntlet_handling.pory"
	.string "I'm good to go!$"

Gauntlet_Multichoice_Text_1:
# 66 "data/scripts/gauntlet_handling.pory"
	.string "Save progress$"

Gauntlet_Multichoice_Text_2:
# 67 "data/scripts/gauntlet_handling.pory"
	.string "Adjust party members$"

Gauntlet_Multichoice_Text_3:
# 68 "data/scripts/gauntlet_handling.pory"
	.string "Quit$"

Gauntlet_Multichoice_Text_4:
# 73 "data/scripts/gauntlet_handling.pory"
	.string "Save game$"

Gauntlet_Multichoice_Loop_Text_0:
# 87 "data/scripts/gauntlet_handling.pory"
	.string "Anything else?$"

Gauntlet_Confirmation_Text_0:
# 92 "data/scripts/gauntlet_handling.pory"
	.string "Are you ready for the next fight?$"

Gauntlet_BattleLost_Text_0:
# 113 "data/scripts/gauntlet_handling.pory"
	.string "wat$"

Gauntlet_AfterGauntletWin_Text_0:
# 122 "data/scripts/gauntlet_handling.pory"
	.string "You got ¥500 for winning!$"
