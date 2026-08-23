Abyssal_Hunters_MapScripts::
	.byte 0


AbyssalHuntersAttendant::
# 6 "data/maps/Abyssal_Hunters/scripts.pory"
	lock
# 7 "data/maps/Abyssal_Hunters/scripts.pory"
	faceplayer
# 8 "data/maps/Abyssal_Hunters/scripts.pory"
	msgbox AbyssalHuntersAttendant_Text_0
# 9 "data/maps/Abyssal_Hunters/scripts.pory"
	callnative PopulateRandomYesNoChoice
# 10 "data/maps/Abyssal_Hunters/scripts.pory"
	dynmultistack 0, 0, TRUE, 2, FALSE, 0, DYN_MULTICHOICE_CB_NONE
# 14 "data/maps/Abyssal_Hunters/scripts.pory"
	switch VAR_RESULT
# 15 "data/maps/Abyssal_Hunters/scripts.pory"
	case 0, AbyssalHuntersAttendant_2
# 18 "data/maps/Abyssal_Hunters/scripts.pory"
	case 1, AbyssalHuntersAttendant_3
	return

AbyssalHuntersAttendant_2:
# 16 "data/maps/Abyssal_Hunters/scripts.pory"
	closemessage
# 17 "data/maps/Abyssal_Hunters/scripts.pory"
	goto AbyssalHuntersStartBattle
	return

AbyssalHuntersAttendant_3:
# 19 "data/maps/Abyssal_Hunters/scripts.pory"
	closemessage
# 20 "data/maps/Abyssal_Hunters/scripts.pory"
	release
	return


AbyssalHuntersStartBattle::
# 25 "data/maps/Abyssal_Hunters/scripts.pory"
	getobjectfacingdirection LOCALID_ATTENDANT, VAR_TEMP_1
# 26 "data/maps/Abyssal_Hunters/scripts.pory"
	switch VAR_TEMP_1
# 27 "data/maps/Abyssal_Hunters/scripts.pory"
	case 1, AbyssalHuntersStartBattle_3
# 29 "data/maps/Abyssal_Hunters/scripts.pory"
	case 2, AbyssalHuntersStartBattle_4
# 31 "data/maps/Abyssal_Hunters/scripts.pory"
	case 3, AbyssalHuntersStartBattle_5
# 33 "data/maps/Abyssal_Hunters/scripts.pory"
	case 4, AbyssalHuntersStartBattle_6
AbyssalHuntersStartBattle_1:
# 36 "data/maps/Abyssal_Hunters/scripts.pory"
	waitmovement
# 37 "data/maps/Abyssal_Hunters/scripts.pory"
	switch VAR_GAUNTLET_POSITION
# 38 "data/maps/Abyssal_Hunters/scripts.pory"
	case 0, AbyssalHuntersStartBattle_9
# 46 "data/maps/Abyssal_Hunters/scripts.pory"
	case 1, AbyssalHuntersStartBattle_10
# 53 "data/maps/Abyssal_Hunters/scripts.pory"
	case 2, AbyssalHuntersStartBattle_11
# 60 "data/maps/Abyssal_Hunters/scripts.pory"
	case 3, AbyssalHuntersStartBattle_12
AbyssalHuntersStartBattle_7:
# 68 "data/maps/Abyssal_Hunters/scripts.pory"
	switch VAR_RESULT
# 69 "data/maps/Abyssal_Hunters/scripts.pory"
	case 0, AbyssalHuntersStartBattle_15
# 74 "data/maps/Abyssal_Hunters/scripts.pory"
	case 1, AbyssalHuntersStartBattle_16
AbyssalHuntersStartBattle_13:
# 77 "data/maps/Abyssal_Hunters/scripts.pory"
	compare VAR_GAUNTLET_POSITION, 4
	goto_if_eq AbyssalHuntersStartBattle_18
AbyssalHuntersStartBattle_17:
# 83 "data/maps/Abyssal_Hunters/scripts.pory"
	release
	end

AbyssalHuntersStartBattle_3:
# 28 "data/maps/Abyssal_Hunters/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, AbyssalHuntersStartBattle_Movement_0
	goto AbyssalHuntersStartBattle_1

AbyssalHuntersStartBattle_4:
# 30 "data/maps/Abyssal_Hunters/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, AbyssalHuntersStartBattle_Movement_1
	goto AbyssalHuntersStartBattle_1

AbyssalHuntersStartBattle_5:
# 32 "data/maps/Abyssal_Hunters/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, AbyssalHuntersStartBattle_Movement_2
	goto AbyssalHuntersStartBattle_1

AbyssalHuntersStartBattle_6:
# 34 "data/maps/Abyssal_Hunters/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, AbyssalHuntersStartBattle_Movement_2
	goto AbyssalHuntersStartBattle_1

AbyssalHuntersStartBattle_9:
# 40 "data/maps/Abyssal_Hunters/scripts.pory"
	waitmovement
# 41 "data/maps/Abyssal_Hunters/scripts.pory"
	trainerbattle_no_intro TRAINER_SPECTER, AbyssalHuntersAfter
# 42 "data/maps/Abyssal_Hunters/scripts.pory"
	msgbox AbyssalHuntersStartBattle_Text_0
# 43 "data/maps/Abyssal_Hunters/scripts.pory"
	closemessage
# 45 "data/maps/Abyssal_Hunters/scripts.pory"
	waitmovement
	goto AbyssalHuntersStartBattle_7

AbyssalHuntersStartBattle_10:
# 48 "data/maps/Abyssal_Hunters/scripts.pory"
	waitmovement
# 49 "data/maps/Abyssal_Hunters/scripts.pory"
	trainerbattle_no_intro TRAINER_GLADIIA, AbyssalHuntersAfter
# 50 "data/maps/Abyssal_Hunters/scripts.pory"
	msgbox AbyssalHuntersStartBattle_Text_0
# 52 "data/maps/Abyssal_Hunters/scripts.pory"
	waitmovement
	goto AbyssalHuntersStartBattle_7

AbyssalHuntersStartBattle_11:
# 55 "data/maps/Abyssal_Hunters/scripts.pory"
	waitmovement
# 56 "data/maps/Abyssal_Hunters/scripts.pory"
	trainerbattle_no_intro TRAINER_SKADI, AbyssalHuntersAfter
# 57 "data/maps/Abyssal_Hunters/scripts.pory"
	msgbox AbyssalHuntersStartBattle_Text_0
# 59 "data/maps/Abyssal_Hunters/scripts.pory"
	waitmovement
	goto AbyssalHuntersStartBattle_7

AbyssalHuntersStartBattle_12:
# 62 "data/maps/Abyssal_Hunters/scripts.pory"
	waitmovement
# 63 "data/maps/Abyssal_Hunters/scripts.pory"
	trainerbattle_no_intro TRAINER_ULPIANUS, AbyssalHuntersAfter
# 64 "data/maps/Abyssal_Hunters/scripts.pory"
	msgbox AbyssalHuntersStartBattle_Text_0
# 66 "data/maps/Abyssal_Hunters/scripts.pory"
	waitmovement
	goto AbyssalHuntersStartBattle_7

AbyssalHuntersStartBattle_15:
# 70 "data/maps/Abyssal_Hunters/scripts.pory"
	msgbox AbyssalHuntersStartBattle_Text_1
# 71 "data/maps/Abyssal_Hunters/scripts.pory"
	waitmessage
# 72 "data/maps/Abyssal_Hunters/scripts.pory"
	warpsilent MAP_CONVENTION_CENTER, 21, 24
# 73 "data/maps/Abyssal_Hunters/scripts.pory"
	waitstate
	goto AbyssalHuntersStartBattle_13

AbyssalHuntersStartBattle_16:
# 75 "data/maps/Abyssal_Hunters/scripts.pory"
	addvar VAR_GAUNTLET_POSITION, 1
	goto AbyssalHuntersStartBattle_13

AbyssalHuntersStartBattle_18:
# 80 "data/maps/Abyssal_Hunters/scripts.pory"
	warpsilent MAP_CONVENTION_CENTER, 10, 10
# 81 "data/maps/Abyssal_Hunters/scripts.pory"
	waitstate
	goto AbyssalHuntersStartBattle_17


# 28 "data/maps/Abyssal_Hunters/scripts.pory"
AbyssalHuntersStartBattle_Movement_0:
# 28 "data/maps/Abyssal_Hunters/scripts.pory"
	walk_right
# 28 "data/maps/Abyssal_Hunters/scripts.pory"
	walk_right
	step_end

# 30 "data/maps/Abyssal_Hunters/scripts.pory"
AbyssalHuntersStartBattle_Movement_1:
# 30 "data/maps/Abyssal_Hunters/scripts.pory"
	walk_right
# 30 "data/maps/Abyssal_Hunters/scripts.pory"
	walk_down
# 30 "data/maps/Abyssal_Hunters/scripts.pory"
	walk_down
# 30 "data/maps/Abyssal_Hunters/scripts.pory"
	walk_right
# 30 "data/maps/Abyssal_Hunters/scripts.pory"
	walk_right
# 30 "data/maps/Abyssal_Hunters/scripts.pory"
	walk_right
	step_end

# 32 "data/maps/Abyssal_Hunters/scripts.pory"
AbyssalHuntersStartBattle_Movement_2:
# 32 "data/maps/Abyssal_Hunters/scripts.pory"
	walk_down
# 32 "data/maps/Abyssal_Hunters/scripts.pory"
	walk_right
# 32 "data/maps/Abyssal_Hunters/scripts.pory"
	walk_right
# 32 "data/maps/Abyssal_Hunters/scripts.pory"
	walk_right
	step_end

AbyssalHuntersAttendant_Text_0:
# 8 "data/maps/Abyssal_Hunters/scripts.pory"
	.string "This is a message.$"

AbyssalHuntersStartBattle_Text_0:
# 42 "data/maps/Abyssal_Hunters/scripts.pory"
	.string "Good battle!$"

AbyssalHuntersStartBattle_Text_1:
# 70 "data/maps/Abyssal_Hunters/scripts.pory"
	.string "wat$"

AbyssalHuntersAfter::
# 87 "data/maps/Abyssal_Hunters/scripts.pory"
	.string "After battle.$"
