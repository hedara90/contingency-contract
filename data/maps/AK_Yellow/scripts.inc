AK_Yellow_MapScripts::
	.byte 0


AKYellowAttendant::
# 6 "data/maps/AK_Yellow/scripts.pory"
	lock
# 7 "data/maps/AK_Yellow/scripts.pory"
	faceplayer
# 8 "data/maps/AK_Yellow/scripts.pory"
	msgbox AKYellowAttendant_Text_0
# 9 "data/maps/AK_Yellow/scripts.pory"
	callnative PopulateRandomYesNoChoice
# 10 "data/maps/AK_Yellow/scripts.pory"
	dynmultistack 0, 0, TRUE, 2, FALSE, 0, DYN_MULTICHOICE_CB_NONE
# 14 "data/maps/AK_Yellow/scripts.pory"
	switch VAR_RESULT
# 15 "data/maps/AK_Yellow/scripts.pory"
	case 0, AKYellowAttendant_2
# 18 "data/maps/AK_Yellow/scripts.pory"
	case 1, AKYellowAttendant_3
	return

AKYellowAttendant_2:
# 16 "data/maps/AK_Yellow/scripts.pory"
	closemessage
# 17 "data/maps/AK_Yellow/scripts.pory"
	goto AKYellowStartBattle
	return

AKYellowAttendant_3:
# 19 "data/maps/AK_Yellow/scripts.pory"
	closemessage
# 20 "data/maps/AK_Yellow/scripts.pory"
	release
	return


AKYellowStartBattle::
# 25 "data/maps/AK_Yellow/scripts.pory"
	getobjectfacingdirection LOCALID_ATTENDANT, VAR_TEMP_1
# 26 "data/maps/AK_Yellow/scripts.pory"
	switch VAR_TEMP_1
# 27 "data/maps/AK_Yellow/scripts.pory"
	case 1, AKYellowStartBattle_3
# 29 "data/maps/AK_Yellow/scripts.pory"
	case 2, AKYellowStartBattle_4
# 31 "data/maps/AK_Yellow/scripts.pory"
	case 3, AKYellowStartBattle_5
# 33 "data/maps/AK_Yellow/scripts.pory"
	case 4, AKYellowStartBattle_6
AKYellowStartBattle_1:
# 36 "data/maps/AK_Yellow/scripts.pory"
	waitmovement
# 37 "data/maps/AK_Yellow/scripts.pory"
	switch VAR_GAUNTLET_POSITION
# 38 "data/maps/AK_Yellow/scripts.pory"
	case 0, AKYellowStartBattle_9
# 46 "data/maps/AK_Yellow/scripts.pory"
	case 1, AKYellowStartBattle_10
# 53 "data/maps/AK_Yellow/scripts.pory"
	case 2, AKYellowStartBattle_11
# 60 "data/maps/AK_Yellow/scripts.pory"
	case 3, AKYellowStartBattle_12
AKYellowStartBattle_7:
# 68 "data/maps/AK_Yellow/scripts.pory"
	switch VAR_RESULT
# 69 "data/maps/AK_Yellow/scripts.pory"
	case 0, AKYellowStartBattle_15
# 74 "data/maps/AK_Yellow/scripts.pory"
	case 1, AKYellowStartBattle_16
AKYellowStartBattle_13:
# 77 "data/maps/AK_Yellow/scripts.pory"
	compare VAR_GAUNTLET_POSITION, 4
	goto_if_eq AKYellowStartBattle_18
AKYellowStartBattle_17:
# 83 "data/maps/AK_Yellow/scripts.pory"
	release
	end

AKYellowStartBattle_3:
# 28 "data/maps/AK_Yellow/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, AKYellowStartBattle_Movement_0
	goto AKYellowStartBattle_1

AKYellowStartBattle_4:
# 30 "data/maps/AK_Yellow/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, AKYellowStartBattle_Movement_1
	goto AKYellowStartBattle_1

AKYellowStartBattle_5:
# 32 "data/maps/AK_Yellow/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, AKYellowStartBattle_Movement_2
	goto AKYellowStartBattle_1

AKYellowStartBattle_6:
# 34 "data/maps/AK_Yellow/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, AKYellowStartBattle_Movement_2
	goto AKYellowStartBattle_1

AKYellowStartBattle_9:
# 40 "data/maps/AK_Yellow/scripts.pory"
	waitmovement
# 41 "data/maps/AK_Yellow/scripts.pory"
	trainerbattle_no_intro TRAINER_SHU, AKYellowAfter
# 42 "data/maps/AK_Yellow/scripts.pory"
	msgbox AKYellowStartBattle_Text_0
# 43 "data/maps/AK_Yellow/scripts.pory"
	closemessage
# 45 "data/maps/AK_Yellow/scripts.pory"
	waitmovement
	goto AKYellowStartBattle_7

AKYellowStartBattle_10:
# 48 "data/maps/AK_Yellow/scripts.pory"
	waitmovement
# 49 "data/maps/AK_Yellow/scripts.pory"
	trainerbattle_no_intro TRAINER_NIAN, AKYellowAfter
# 50 "data/maps/AK_Yellow/scripts.pory"
	msgbox AKYellowStartBattle_Text_0
# 52 "data/maps/AK_Yellow/scripts.pory"
	waitmovement
	goto AKYellowStartBattle_7

AKYellowStartBattle_11:
# 55 "data/maps/AK_Yellow/scripts.pory"
	waitmovement
# 56 "data/maps/AK_Yellow/scripts.pory"
	trainerbattle_no_intro TRAINER_ELYSIUM, AKYellowAfter
# 57 "data/maps/AK_Yellow/scripts.pory"
	msgbox AKYellowStartBattle_Text_0
# 59 "data/maps/AK_Yellow/scripts.pory"
	waitmovement
	goto AKYellowStartBattle_7

AKYellowStartBattle_12:
# 62 "data/maps/AK_Yellow/scripts.pory"
	waitmovement
# 63 "data/maps/AK_Yellow/scripts.pory"
	trainerbattle_no_intro TRAINER_IRENE, AKYellowAfter
# 64 "data/maps/AK_Yellow/scripts.pory"
	msgbox AKYellowStartBattle_Text_0
# 66 "data/maps/AK_Yellow/scripts.pory"
	waitmovement
	goto AKYellowStartBattle_7

AKYellowStartBattle_15:
# 70 "data/maps/AK_Yellow/scripts.pory"
	msgbox AKYellowStartBattle_Text_1
# 71 "data/maps/AK_Yellow/scripts.pory"
	waitmessage
# 72 "data/maps/AK_Yellow/scripts.pory"
	warpsilent MAP_CONVENTION_CENTER, 21, 24
# 73 "data/maps/AK_Yellow/scripts.pory"
	waitstate
	goto AKYellowStartBattle_13

AKYellowStartBattle_16:
# 75 "data/maps/AK_Yellow/scripts.pory"
	addvar VAR_GAUNTLET_POSITION, 1
	goto AKYellowStartBattle_13

AKYellowStartBattle_18:
# 80 "data/maps/AK_Yellow/scripts.pory"
	warpsilent MAP_CONVENTION_CENTER, 10, 10
# 81 "data/maps/AK_Yellow/scripts.pory"
	waitstate
	goto AKYellowStartBattle_17


# 28 "data/maps/AK_Yellow/scripts.pory"
AKYellowStartBattle_Movement_0:
# 28 "data/maps/AK_Yellow/scripts.pory"
	walk_right
# 28 "data/maps/AK_Yellow/scripts.pory"
	walk_right
	step_end

# 30 "data/maps/AK_Yellow/scripts.pory"
AKYellowStartBattle_Movement_1:
# 30 "data/maps/AK_Yellow/scripts.pory"
	walk_right
# 30 "data/maps/AK_Yellow/scripts.pory"
	walk_down
# 30 "data/maps/AK_Yellow/scripts.pory"
	walk_down
# 30 "data/maps/AK_Yellow/scripts.pory"
	walk_right
# 30 "data/maps/AK_Yellow/scripts.pory"
	walk_right
# 30 "data/maps/AK_Yellow/scripts.pory"
	walk_right
	step_end

# 32 "data/maps/AK_Yellow/scripts.pory"
AKYellowStartBattle_Movement_2:
# 32 "data/maps/AK_Yellow/scripts.pory"
	walk_down
# 32 "data/maps/AK_Yellow/scripts.pory"
	walk_right
# 32 "data/maps/AK_Yellow/scripts.pory"
	walk_right
# 32 "data/maps/AK_Yellow/scripts.pory"
	walk_right
	step_end

AKYellowAttendant_Text_0:
# 8 "data/maps/AK_Yellow/scripts.pory"
	.string "This is a message.$"

AKYellowStartBattle_Text_0:
# 42 "data/maps/AK_Yellow/scripts.pory"
	.string "Good battle!$"

AKYellowStartBattle_Text_1:
# 70 "data/maps/AK_Yellow/scripts.pory"
	.string "wat$"

AKYellowAfter::
# 87 "data/maps/AK_Yellow/scripts.pory"
	.string "After battle.$"
