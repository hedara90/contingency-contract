AK_Green_MapScripts::
	.byte 0


AKGreenAttendant::
# 6 "data/maps/AK_Green/scripts.pory"
	lock
# 7 "data/maps/AK_Green/scripts.pory"
	faceplayer
# 8 "data/maps/AK_Green/scripts.pory"
	msgbox AKGreenAttendant_Text_0
# 9 "data/maps/AK_Green/scripts.pory"
	callnative PopulateRandomYesNoChoice
# 10 "data/maps/AK_Green/scripts.pory"
	dynmultistack 0, 0, TRUE, 2, FALSE, 0, DYN_MULTICHOICE_CB_NONE
# 14 "data/maps/AK_Green/scripts.pory"
	switch VAR_RESULT
# 15 "data/maps/AK_Green/scripts.pory"
	case 0, AKGreenAttendant_2
# 18 "data/maps/AK_Green/scripts.pory"
	case 1, AKGreenAttendant_3
	return

AKGreenAttendant_2:
# 16 "data/maps/AK_Green/scripts.pory"
	closemessage
# 17 "data/maps/AK_Green/scripts.pory"
	goto AKGreenStartBattle
	return

AKGreenAttendant_3:
# 19 "data/maps/AK_Green/scripts.pory"
	closemessage
# 20 "data/maps/AK_Green/scripts.pory"
	release
	return


AKGreenStartBattle::
# 25 "data/maps/AK_Green/scripts.pory"
	getobjectfacingdirection LOCALID_ATTENDANT, VAR_TEMP_1
# 26 "data/maps/AK_Green/scripts.pory"
	switch VAR_TEMP_1
# 27 "data/maps/AK_Green/scripts.pory"
	case 1, AKGreenStartBattle_3
# 29 "data/maps/AK_Green/scripts.pory"
	case 2, AKGreenStartBattle_4
# 31 "data/maps/AK_Green/scripts.pory"
	case 3, AKGreenStartBattle_5
# 33 "data/maps/AK_Green/scripts.pory"
	case 4, AKGreenStartBattle_6
AKGreenStartBattle_1:
# 36 "data/maps/AK_Green/scripts.pory"
	waitmovement
# 37 "data/maps/AK_Green/scripts.pory"
	switch VAR_GAUNTLET_POSITION
# 38 "data/maps/AK_Green/scripts.pory"
	case 0, AKGreenStartBattle_9
# 46 "data/maps/AK_Green/scripts.pory"
	case 1, AKGreenStartBattle_10
# 53 "data/maps/AK_Green/scripts.pory"
	case 2, AKGreenStartBattle_11
# 60 "data/maps/AK_Green/scripts.pory"
	case 3, AKGreenStartBattle_12
AKGreenStartBattle_7:
# 68 "data/maps/AK_Green/scripts.pory"
	switch VAR_RESULT
# 69 "data/maps/AK_Green/scripts.pory"
	case 0, AKGreenStartBattle_15
# 74 "data/maps/AK_Green/scripts.pory"
	case 1, AKGreenStartBattle_16
AKGreenStartBattle_13:
# 77 "data/maps/AK_Green/scripts.pory"
	compare VAR_GAUNTLET_POSITION, 4
	goto_if_eq AKGreenStartBattle_18
AKGreenStartBattle_17:
# 83 "data/maps/AK_Green/scripts.pory"
	release
	end

AKGreenStartBattle_3:
# 28 "data/maps/AK_Green/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, AKGreenStartBattle_Movement_0
	goto AKGreenStartBattle_1

AKGreenStartBattle_4:
# 30 "data/maps/AK_Green/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, AKGreenStartBattle_Movement_1
	goto AKGreenStartBattle_1

AKGreenStartBattle_5:
# 32 "data/maps/AK_Green/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, AKGreenStartBattle_Movement_2
	goto AKGreenStartBattle_1

AKGreenStartBattle_6:
# 34 "data/maps/AK_Green/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, AKGreenStartBattle_Movement_2
	goto AKGreenStartBattle_1

AKGreenStartBattle_9:
# 40 "data/maps/AK_Green/scripts.pory"
	waitmovement
# 41 "data/maps/AK_Green/scripts.pory"
	trainerbattle_no_intro TRAINER_CARNELIAN, AKGreenAfter
# 42 "data/maps/AK_Green/scripts.pory"
	msgbox AKGreenStartBattle_Text_0
# 43 "data/maps/AK_Green/scripts.pory"
	closemessage
# 45 "data/maps/AK_Green/scripts.pory"
	waitmovement
	goto AKGreenStartBattle_7

AKGreenStartBattle_10:
# 48 "data/maps/AK_Green/scripts.pory"
	waitmovement
# 49 "data/maps/AK_Green/scripts.pory"
	trainerbattle_no_intro TRAINER_PENANCE, AKGreenAfter
# 50 "data/maps/AK_Green/scripts.pory"
	msgbox AKGreenStartBattle_Text_0
# 52 "data/maps/AK_Green/scripts.pory"
	waitmovement
	goto AKGreenStartBattle_7

AKGreenStartBattle_11:
# 55 "data/maps/AK_Green/scripts.pory"
	waitmovement
# 56 "data/maps/AK_Green/scripts.pory"
	trainerbattle_no_intro TRAINER_PRAMANIX, AKGreenAfter
# 57 "data/maps/AK_Green/scripts.pory"
	msgbox AKGreenStartBattle_Text_0
# 59 "data/maps/AK_Green/scripts.pory"
	waitmovement
	goto AKGreenStartBattle_7

AKGreenStartBattle_12:
# 62 "data/maps/AK_Green/scripts.pory"
	waitmovement
# 63 "data/maps/AK_Green/scripts.pory"
	trainerbattle_no_intro TRAINER_FIAMETTA, AKGreenAfter
# 64 "data/maps/AK_Green/scripts.pory"
	msgbox AKGreenStartBattle_Text_0
# 66 "data/maps/AK_Green/scripts.pory"
	waitmovement
	goto AKGreenStartBattle_7

AKGreenStartBattle_15:
# 70 "data/maps/AK_Green/scripts.pory"
	msgbox AKGreenStartBattle_Text_1
# 71 "data/maps/AK_Green/scripts.pory"
	waitmessage
# 72 "data/maps/AK_Green/scripts.pory"
	warpsilent MAP_CONVENTION_CENTER, 21, 24
# 73 "data/maps/AK_Green/scripts.pory"
	waitstate
	goto AKGreenStartBattle_13

AKGreenStartBattle_16:
# 75 "data/maps/AK_Green/scripts.pory"
	addvar VAR_GAUNTLET_POSITION, 1
	goto AKGreenStartBattle_13

AKGreenStartBattle_18:
# 80 "data/maps/AK_Green/scripts.pory"
	warpsilent MAP_CONVENTION_CENTER, 10, 10
# 81 "data/maps/AK_Green/scripts.pory"
	waitstate
	goto AKGreenStartBattle_17


# 28 "data/maps/AK_Green/scripts.pory"
AKGreenStartBattle_Movement_0:
# 28 "data/maps/AK_Green/scripts.pory"
	walk_right
# 28 "data/maps/AK_Green/scripts.pory"
	walk_right
	step_end

# 30 "data/maps/AK_Green/scripts.pory"
AKGreenStartBattle_Movement_1:
# 30 "data/maps/AK_Green/scripts.pory"
	walk_right
# 30 "data/maps/AK_Green/scripts.pory"
	walk_down
# 30 "data/maps/AK_Green/scripts.pory"
	walk_down
# 30 "data/maps/AK_Green/scripts.pory"
	walk_right
# 30 "data/maps/AK_Green/scripts.pory"
	walk_right
# 30 "data/maps/AK_Green/scripts.pory"
	walk_right
	step_end

# 32 "data/maps/AK_Green/scripts.pory"
AKGreenStartBattle_Movement_2:
# 32 "data/maps/AK_Green/scripts.pory"
	walk_down
# 32 "data/maps/AK_Green/scripts.pory"
	walk_right
# 32 "data/maps/AK_Green/scripts.pory"
	walk_right
# 32 "data/maps/AK_Green/scripts.pory"
	walk_right
	step_end

AKGreenAttendant_Text_0:
# 8 "data/maps/AK_Green/scripts.pory"
	.string "This is a message.$"

AKGreenStartBattle_Text_0:
# 42 "data/maps/AK_Green/scripts.pory"
	.string "Good battle!$"

AKGreenStartBattle_Text_1:
# 70 "data/maps/AK_Green/scripts.pory"
	.string "wat$"

AKGreenAfter::
# 87 "data/maps/AK_Green/scripts.pory"
	.string "After battle.$"
