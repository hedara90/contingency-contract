@ Graphics Id Commands
	@ Stores graphicsId (species, shinyness, gender) about the first pokemon in party that can learn 'move' in 'var'
	.macro setvar_moncanlearnmove_gfx var:req move:req
	callnative Script_SetVar_MonCanLearnMoveGfx
	.2byte \var
	.2byte \move
	.endm

	@ Stores graphicsId (species, shinyness, gender) about the first pokemon in party that knows 'move' in 'var'
	.macro setvar_monknowsmove_gfx var:req move:req
	callnative Script_SetVar_MonKnowsMoveGfx
	.2byte \var
	.2byte \move
	.endm

	@ Stores graphicsId (species, shinyness, gender) about a random pokemon in party
	.macro setvar_choosemon_gfx var:req
	callnative Script_SetVar_ChooseMonGfx
	.2byte \var
	.endm

	@ Stores graphicsId (species, shinyness, gender) about a random pokemon in party
	.macro setvar_randompartymon_gfx var:req
	callnative Script_SetVar_RandomPartyMonGfx
	.2byte \var
	.endm

@ Morph (transformation) Commands
	@ Change the graphics the object event with `localId` using the transformation method `type` (enum MorphType)
	@ If no `map` is specified, then the current map is used.
	.macro morphobject localId:req, type:req, graphicsId=NUM_OBJ_EVENT_GFX, map
	.ifb \map
		callnative Script_MorphObject
		.byte \localId
		.2byte \graphicsId
		.byte \type
	.else
		callnative Script_MorphObjectAt
		.byte \localId
		.2byte \graphicsId
		.byte \type
		map \map
	.endif
	.endm

	@ instantmorph, mosaicmorph and spinmorph are wrappers around morph object that already specify the transformation type and include a sound effect
	.macro instantmorph localId:req, graphicsId=NUM_OBJ_EVENT_GFX, map
	playse SE_BALL_THROW
	.ifb \map
		morphobject \localId, INSTANT_MORPH, \graphicsId
	.else
		morphobject \localId, INSTANT_MORPH, \graphicsId. \map
	.endif
	.endm

	.macro mosaicmorph localId:req, graphicsId=NUM_OBJ_EVENT_GFX, map
	playse SE_M_TELEPORT
	.ifb \map
		morphobject \localId, MOSAIC_MORPH, \graphicsId
	.else
		morphobject \localId, MOSAIC_MORPH, \graphicsId. \map
	.endif
	.endm

	.macro spinmorph localId:req, graphicsId=NUM_OBJ_EVENT_GFX, map
	playse SE_RG_CARD_FLIPPING
	.ifb \map
		morphobject \localId, SPIN_MORPH, \graphicsId
	.else
		morphobject \localId, SPIN_MORPH, \graphicsId. \map
	.endif
	.endm

@ Bird Commands (not fully polished, only works with centered player character, use with caution)
	@ Play the beginning of the fly-out animation and turn the player avatar into graphicsId at the end (also moves player one tile to the right)
	.macro jumponrightbird graphicsId:req localId=LOCALID_PLAYER
	callnative Script_JumpOnBird
	.byte \localId
	.2byte \graphicsId
	.byte TRUE
	waitstate
	applymovement \localId, Common_Movement_WalkRightFast
	waitmovement
	.endm

	@ Play the beginning of the fly-out animation and turn the player avatar into graphicsId at the end (also moves player one tile to the left)
	.macro jumponleftbird graphicsId:req localId=LOCALID_PLAYER
	callnative Script_JumpOnBird
	.byte \localId
	.2byte \graphicsId
	.byte FALSE
	waitstate
	applymovement \localId, Common_Movement_WalkLeftFast
	waitmovement
	.endm

	@ Play the end of the fly-in animation and turn the player avatar back to normal
	.macro jumpoffbird localId=LOCALID_PLAYER, graphicsId=NUM_OBJ_EVENT_GFX
	callnative Script_JumpOffBird
	.byte \localId
	.2byte \graphicsId
	.endm