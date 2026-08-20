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
# 71 "data/maps/Reconveners/scripts.pory"
	case 1, ReconvenersStartBattle_16
ReconvenersStartBattle_13:
# 74 "data/maps/Reconveners/scripts.pory"
	compare VAR_GAUNTLET_POSITION, 4
	goto_if_eq ReconvenersStartBattle_18
ReconvenersStartBattle_17:
# 80 "data/maps/Reconveners/scripts.pory"
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
	applymovement OBJ_EVENT_ID_PLAYER, ReconvenersStartBattle_Movement_3
	goto ReconvenersStartBattle_1

ReconvenersStartBattle_9:
# 39 "data/maps/Reconveners/scripts.pory"
	applymovement LOCALID_ONE, ReconvenersStartBattle_Movement_4
# 40 "data/maps/Reconveners/scripts.pory"
	waitmovement
# 41 "data/maps/Reconveners/scripts.pory"
	trainerbattle_no_intro TRAINER_ARDELIA, ReconvenersAfter
# 42 "data/maps/Reconveners/scripts.pory"
	msgbox ReconvenersStartBattle_Text_0
# 43 "data/maps/Reconveners/scripts.pory"
	closemessage
# 44 "data/maps/Reconveners/scripts.pory"
	applymovement LOCALID_ONE, ReconvenersStartBattle_Movement_5
# 45 "data/maps/Reconveners/scripts.pory"
	waitmovement
	goto ReconvenersStartBattle_7

ReconvenersStartBattle_10:
# 47 "data/maps/Reconveners/scripts.pory"
	applymovement LOCALID_TWO, ReconvenersStartBattle_Movement_6
# 48 "data/maps/Reconveners/scripts.pory"
	waitmovement
# 49 "data/maps/Reconveners/scripts.pory"
	trainerbattle_no_intro TRAINER_POG, ReconvenersAfter
# 50 "data/maps/Reconveners/scripts.pory"
	msgbox ReconvenersStartBattle_Text_0
# 51 "data/maps/Reconveners/scripts.pory"
	applymovement LOCALID_TWO, ReconvenersStartBattle_Movement_7
# 52 "data/maps/Reconveners/scripts.pory"
	waitmovement
	goto ReconvenersStartBattle_7

ReconvenersStartBattle_11:
# 54 "data/maps/Reconveners/scripts.pory"
	applymovement LOCALID_THREE, ReconvenersStartBattle_Movement_8
# 55 "data/maps/Reconveners/scripts.pory"
	waitmovement
# 56 "data/maps/Reconveners/scripts.pory"
	trainerbattle_no_intro TRAINER_GILBERTA, ReconvenersAfter
# 57 "data/maps/Reconveners/scripts.pory"
	msgbox ReconvenersStartBattle_Text_0
# 58 "data/maps/Reconveners/scripts.pory"
	applymovement LOCALID_THREE, ReconvenersStartBattle_Movement_9
# 59 "data/maps/Reconveners/scripts.pory"
	waitmovement
	goto ReconvenersStartBattle_7

ReconvenersStartBattle_12:
# 61 "data/maps/Reconveners/scripts.pory"
	applymovement LOCALID_FOUR, ReconvenersStartBattle_Movement_10
# 62 "data/maps/Reconveners/scripts.pory"
	waitmovement
# 63 "data/maps/Reconveners/scripts.pory"
	trainerbattle_no_intro TRAINER_LAEVATAIN, ReconvenersAfter
# 64 "data/maps/Reconveners/scripts.pory"
	msgbox ReconvenersStartBattle_Text_0
# 65 "data/maps/Reconveners/scripts.pory"
	applymovement LOCALID_FOUR, ReconvenersStartBattle_Movement_11
# 66 "data/maps/Reconveners/scripts.pory"
	waitmovement
	goto ReconvenersStartBattle_7

ReconvenersStartBattle_15:
# 70 "data/maps/Reconveners/scripts.pory"
	msgbox ReconvenersStartBattle_Text_1
	goto ReconvenersStartBattle_13

ReconvenersStartBattle_16:
# 72 "data/maps/Reconveners/scripts.pory"
	addvar VAR_GAUNTLET_POSITION, 1
	goto ReconvenersStartBattle_13

ReconvenersStartBattle_18:
# 77 "data/maps/Reconveners/scripts.pory"
	warpsilent MAP_CONVENTION_CENTER, 10, 10
# 78 "data/maps/Reconveners/scripts.pory"
	waitstate
	goto ReconvenersStartBattle_17


# 28 "data/maps/Reconveners/scripts.pory"
ReconvenersStartBattle_Movement_0:
# 28 "data/maps/Reconveners/scripts.pory"
	walk_right
# 28 "data/maps/Reconveners/scripts.pory"
	walk_right
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
# 32 "data/maps/Reconveners/scripts.pory"
	walk_right
# 32 "data/maps/Reconveners/scripts.pory"
	walk_right
	step_end

# 34 "data/maps/Reconveners/scripts.pory"
ReconvenersStartBattle_Movement_3:
# 34 "data/maps/Reconveners/scripts.pory"
	walk_down
# 34 "data/maps/Reconveners/scripts.pory"
	walk_right
# 34 "data/maps/Reconveners/scripts.pory"
	walk_right
# 34 "data/maps/Reconveners/scripts.pory"
	walk_right
	step_end

# 39 "data/maps/Reconveners/scripts.pory"
ReconvenersStartBattle_Movement_4:
# 39 "data/maps/Reconveners/scripts.pory"
	walk_down
# 39 "data/maps/Reconveners/scripts.pory"
	walk_down
# 39 "data/maps/Reconveners/scripts.pory"
	walk_left
# 39 "data/maps/Reconveners/scripts.pory"
	walk_left
	step_end

# 44 "data/maps/Reconveners/scripts.pory"
ReconvenersStartBattle_Movement_5:
# 44 "data/maps/Reconveners/scripts.pory"
	walk_right
# 44 "data/maps/Reconveners/scripts.pory"
	walk_right
# 44 "data/maps/Reconveners/scripts.pory"
	walk_up
# 44 "data/maps/Reconveners/scripts.pory"
	walk_up
	step_end

# 47 "data/maps/Reconveners/scripts.pory"
ReconvenersStartBattle_Movement_6:
# 47 "data/maps/Reconveners/scripts.pory"
	walk_down
# 47 "data/maps/Reconveners/scripts.pory"
	walk_left
# 47 "data/maps/Reconveners/scripts.pory"
	walk_left
	step_end

# 51 "data/maps/Reconveners/scripts.pory"
ReconvenersStartBattle_Movement_7:
# 51 "data/maps/Reconveners/scripts.pory"
	walk_right
# 51 "data/maps/Reconveners/scripts.pory"
	walk_right
# 51 "data/maps/Reconveners/scripts.pory"
	walk_up
	step_end

# 54 "data/maps/Reconveners/scripts.pory"
ReconvenersStartBattle_Movement_8:
# 54 "data/maps/Reconveners/scripts.pory"
	walk_up
# 54 "data/maps/Reconveners/scripts.pory"
	walk_left
# 54 "data/maps/Reconveners/scripts.pory"
	walk_left
	step_end

# 58 "data/maps/Reconveners/scripts.pory"
ReconvenersStartBattle_Movement_9:
# 58 "data/maps/Reconveners/scripts.pory"
	walk_right
# 58 "data/maps/Reconveners/scripts.pory"
	walk_right
# 58 "data/maps/Reconveners/scripts.pory"
	walk_down
	step_end

# 61 "data/maps/Reconveners/scripts.pory"
ReconvenersStartBattle_Movement_10:
# 61 "data/maps/Reconveners/scripts.pory"
	walk_up
# 61 "data/maps/Reconveners/scripts.pory"
	walk_up
# 61 "data/maps/Reconveners/scripts.pory"
	walk_left
# 61 "data/maps/Reconveners/scripts.pory"
	walk_left
	step_end

# 65 "data/maps/Reconveners/scripts.pory"
ReconvenersStartBattle_Movement_11:
# 65 "data/maps/Reconveners/scripts.pory"
	walk_right
# 65 "data/maps/Reconveners/scripts.pory"
	walk_right
# 65 "data/maps/Reconveners/scripts.pory"
	walk_down
# 65 "data/maps/Reconveners/scripts.pory"
	walk_down
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
# 84 "data/maps/Reconveners/scripts.pory"
	.string "After battle.$"
