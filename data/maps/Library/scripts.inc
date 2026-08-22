Library_MapScripts::
# 2 "data/maps/Library/scripts.pory"
	map_script MAP_SCRIPT_ON_RESUME, Library_MapScripts_MAP_SCRIPT_ON_RESUME
	.byte 0

Library_MapScripts_MAP_SCRIPT_ON_RESUME:
# 3 "data/maps/Library/scripts.pory"
	callnative Init_PulseNeonLights
	return

