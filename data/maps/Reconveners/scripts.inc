Reconveners_MapScripts::
	.byte 0


ReconvenersAttendant::
# 6 "data/maps/Reconveners/scripts.pory"
	lock
# 7 "data/maps/Reconveners/scripts.pory"
	faceplayer
# 8 "data/maps/Reconveners/scripts.pory"
	msgbox ReconvenersAttendant_Text_0
# 9 "data/maps/Reconveners/scripts.pory"
	callnative PopulateRandomYesNoChoice
# 10 "data/maps/Reconveners/scripts.pory"
	dynmultistack 0, 0, TRUE, 2, FALSE, 0, DYN_MULTICHOICE_CB_NONE
# 14 "data/maps/Reconveners/scripts.pory"
	switch VAR_RESULT
# 15 "data/maps/Reconveners/scripts.pory"
	case 0, ReconvenersAttendant_2
# 18 "data/maps/Reconveners/scripts.pory"
	case 1, ReconvenersAttendant_3
	return

ReconvenersAttendant_2:
# 16 "data/maps/Reconveners/scripts.pory"
	closemessage
# 17 "data/maps/Reconveners/scripts.pory"
	goto ReconvenersStartBattle
	return

ReconvenersAttendant_3:
# 19 "data/maps/Reconveners/scripts.pory"
	closemessage
# 20 "data/maps/Reconveners/scripts.pory"
	release
	return


ReconvenersStartBattle::
# 25 "data/maps/Reconveners/scripts.pory"
	getobjectfacingdirection LOCALID_ATTENDANT, VAR_TEMP_1
# 26 "data/maps/Reconveners/scripts.pory"
	switch VAR_TEMP_1
# 27 "data/maps/Reconveners/scripts.pory"
	case 1, ReconvenersStartBattle_3
# 29 "data/maps/Reconveners/scripts.pory"
	case 2, ReconvenersStartBattle_4
# 31 "data/maps/Reconveners/scripts.pory"
	case 3, ReconvenersStartBattle_5
# 33 "data/maps/Reconveners/scripts.pory"
	case 4, ReconvenersStartBattle_6
ReconvenersStartBattle_1:
# 36 "data/maps/Reconveners/scripts.pory"
	waitmovement
# 37 "data/maps/Reconveners/scripts.pory"
	switch VAR_GAUNTLET_POSITION
# 38 "data/maps/Reconveners/scripts.pory"
	case 0, ReconvenersStartBattle_9
# 46 "data/maps/Reconveners/scripts.pory"
	case 1, ReconvenersStartBattle_10
# 53 "data/maps/Reconveners/scripts.pory"
	case 2, ReconvenersStartBattle_11
# 60 "data/maps/Reconveners/scripts.pory"
	case 3, ReconvenersStartBattle_12
ReconvenersStartBattle_7:
# 68 "data/maps/Reconveners/scripts.pory"
	switch VAR_RESULT
# 69 "data/maps/Reconveners/scripts.pory"
	case 0, ReconvenersStartBattle_15
# 74 "data/maps/Reconveners/scripts.pory"
	case 1, ReconvenersStartBattle_16
ReconvenersStartBattle_13:
# 77 "data/maps/Reconveners/scripts.pory"
	compare VAR_GAUNTLET_POSITION, 4
	goto_if_eq ReconvenersStartBattle_18
ReconvenersStartBattle_17:
# 83 "data/maps/Reconveners/scripts.pory"
	release
	end

ReconvenersStartBattle_3:
# 28 "data/maps/Reconveners/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, ReconvenersStartBattle_Movement_0
	goto ReconvenersStartBattle_1

ReconvenersStartBattle_4:
# 30 "data/maps/Reconveners/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, ReconvenersStartBattle_Movement_1
	goto ReconvenersStartBattle_1

ReconvenersStartBattle_5:
# 32 "data/maps/Reconveners/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, ReconvenersStartBattle_Movement_2
	goto ReconvenersStartBattle_1

ReconvenersStartBattle_6:
# 34 "data/maps/Reconveners/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, ReconvenersStartBattle_Movement_2
	goto ReconvenersStartBattle_1

ReconvenersStartBattle_9:
# 40 "data/maps/Reconveners/scripts.pory"
	waitmovement
# 41 "data/maps/Reconveners/scripts.pory"
	trainerbattle_no_intro TRAINER_ARDELIA, ReconvenersAfter
# 42 "data/maps/Reconveners/scripts.pory"
	msgbox ReconvenersStartBattle_Text_0
# 43 "data/maps/Reconveners/scripts.pory"
	closemessage
# 45 "data/maps/Reconveners/scripts.pory"
	waitmovement
	goto ReconvenersStartBattle_7

ReconvenersStartBattle_10:
# 48 "data/maps/Reconveners/scripts.pory"
	waitmovement
# 49 "data/maps/Reconveners/scripts.pory"
	trainerbattle_no_intro TRAINER_POG, ReconvenersAfter
# 50 "data/maps/Reconveners/scripts.pory"
	msgbox ReconvenersStartBattle_Text_0
# 52 "data/maps/Reconveners/scripts.pory"
	waitmovement
	goto ReconvenersStartBattle_7

ReconvenersStartBattle_11:
# 55 "data/maps/Reconveners/scripts.pory"
	waitmovement
# 56 "data/maps/Reconveners/scripts.pory"
	trainerbattle_no_intro TRAINER_GILBERTA, ReconvenersAfter
# 57 "data/maps/Reconveners/scripts.pory"
	msgbox ReconvenersStartBattle_Text_0
# 59 "data/maps/Reconveners/scripts.pory"
	waitmovement
	goto ReconvenersStartBattle_7

ReconvenersStartBattle_12:
# 62 "data/maps/Reconveners/scripts.pory"
	waitmovement
# 63 "data/maps/Reconveners/scripts.pory"
	trainerbattle_no_intro TRAINER_LAEVATAIN, ReconvenersAfter
# 64 "data/maps/Reconveners/scripts.pory"
	msgbox ReconvenersStartBattle_Text_0
# 66 "data/maps/Reconveners/scripts.pory"
	waitmovement
	goto ReconvenersStartBattle_7

ReconvenersStartBattle_15:
# 70 "data/maps/Reconveners/scripts.pory"
	msgbox ReconvenersStartBattle_Text_1
# 71 "data/maps/Reconveners/scripts.pory"
	waitmessage
# 72 "data/maps/Reconveners/scripts.pory"
	warpsilent MAP_CONVENTION_CENTER, 21, 24
# 73 "data/maps/Reconveners/scripts.pory"
	waitstate
	goto ReconvenersStartBattle_13

ReconvenersStartBattle_16:
# 75 "data/maps/Reconveners/scripts.pory"
	addvar VAR_GAUNTLET_POSITION, 1
	goto ReconvenersStartBattle_13

ReconvenersStartBattle_18:
# 80 "data/maps/Reconveners/scripts.pory"
	warpsilent MAP_CONVENTION_CENTER, 10, 10
# 81 "data/maps/Reconveners/scripts.pory"
	waitstate
	goto ReconvenersStartBattle_17


# 28 "data/maps/Reconveners/scripts.pory"
ReconvenersStartBattle_Movement_0:
# 28 "data/maps/Reconveners/scripts.pory"
	walk_right
# 28 "data/maps/Reconveners/scripts.pory"
	walk_right
	step_end

# 30 "data/maps/Reconveners/scripts.pory"
ReconvenersStartBattle_Movement_1:
# 30 "data/maps/Reconveners/scripts.pory"
	walk_right
# 30 "data/maps/Reconveners/scripts.pory"
	walk_down
# 30 "data/maps/Reconveners/scripts.pory"
	walk_down
# 30 "data/maps/Reconveners/scripts.pory"
	walk_right
# 30 "data/maps/Reconveners/scripts.pory"
	walk_right
# 30 "data/maps/Reconveners/scripts.pory"
	walk_right
	step_end

# 32 "data/maps/Reconveners/scripts.pory"
ReconvenersStartBattle_Movement_2:
# 32 "data/maps/Reconveners/scripts.pory"
	walk_down
# 32 "data/maps/Reconveners/scripts.pory"
	walk_right
# 32 "data/maps/Reconveners/scripts.pory"
	walk_right
# 32 "data/maps/Reconveners/scripts.pory"
	walk_right
	step_end

ReconvenersAttendant_Text_0:
# 8 "data/maps/Reconveners/scripts.pory"
	.string "This is a message.$"

ReconvenersStartBattle_Text_0:
# 42 "data/maps/Reconveners/scripts.pory"
	.string "Good battle!$"

ReconvenersStartBattle_Text_1:
# 70 "data/maps/Reconveners/scripts.pory"
	.string "wat$"

ReconvenersAfter::
# 87 "data/maps/Reconveners/scripts.pory"
	.string "After battle.$"
