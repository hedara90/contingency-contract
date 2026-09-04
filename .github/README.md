This is the README for Jamie's Transformation Pack feature branch, please refer to another branch for the pokeemerald-expansion README

# Videos

## Instant Morph
https://github.com/user-attachments/assets/7ae1551f-83fc-4881-9c66-eff1aeadbcdf
```
OldaleTown_EventScript_Gir::
	instantmorph LOCALID_PLAYER, OBJ_EVENT_GFX_GIRL_3
	end

OldaleTown_EventScript_MartEmployee::
	instantmorph LOCALID_PLAYER
	end
```
## Spin Morph
https://github.com/user-attachments/assets/37eb32d3-9bb1-485c-abea-2c224ea99e1f
```
OldaleTown_EventScript_Zoroark::
	setvar_randompartymon_gfx VAR_0x8005
	spinmorph 1, VAR_0x8005
	end
```

## Mosaic Morph
https://github.com/user-attachments/assets/06e45666-b5c8-442c-b3df-70ff4c317c72
```
OldaleTown_EventScript_Ditto::
	chooseboxmon SELECT_PC_MON_DAYCARE @ I call chooseboxmon with SELECT_PC_MON_DAYCARE insetad of no argument because it forbids picking an egg
	waitstate
	setvar_choosemon_gfx VAR_0x8005
	mosaicmorph 1, VAR_0x8005
	end
```

## Bird Transform (BETA)
https://github.com/user-attachments/assets/435edc97-62d7-4284-9266-5eca6c3e9556
```
OldaleTown_EventScript_Girl::
	setvar_moncanlearnmove_gfx VAR_0x8005, MOVE_FLY
	jumponleftbird VAR_0x8005
	applymovement LOCALID_PLAYER, MovementStuff
	waitmovement
	jumpoffbird
	end

MovementStuff::
	walk_up
	walk_down
	step_end
```

# Usage

## Transformations

A transformation needs 3 arguments: the object you want to transform, the graphics you want to change the object into and the type of effects you want to accompany the trasnformation (referred to as "morph/morphing" in the code to avoid search conflicts with the battle move Transform).
This is why the main transformation command  has the following arguments.
`.macro morphobject localId:req, type:req, graphicsId=NUM_OBJ_EVENT_GFX, map`
- `localId` to represent the object you want to morph with an optional map argument at the end in case you want to refer to an object from a different map
- `type` indicates the transformation effect you want to be applied. These are defined as `enum MorphType` and the possbile values are `INSTANT_MORPH`, `SPIN_MORPH` and `MOSAIC_MORPH`
- `graphicsId` to represent the image you want to change the object into. Check the dedicated section to understand how graphicsId work. If the graphicsId argument is not present, the object will try to change into its "original" form. For Regular object events for example, it would be the graphicsId defined in their template.

In addition to `morphobject`, we have three wrappers functions that take one less argument and include the morph type
`instantmorph/spinmorph/mosaicmorph localId:req, graphicsId=NUM_OBJ_EVENT_GFX, map`
These wrappers also play a sound effect to accompany the transformation. We recommand watching the videos examples to see how the different effects look like

### Bird Transformations (BETA)

This feature branch also contains three additional script commands: `jumponrightbird`, `jumponleftbird` and `jumpoffbird`. These commands are not fully polished. They only works with a centered player character and should be used with caution. I included them because they are somewhat similar and might be interested to some people but they are provided as is and you have to fix them yourself if you have issues.

## Graphics ID

Graphics IDs are the main arguments for transformations. They are the `OBJ_EVENT_GFX_` values you can find in `include/constants/event_objects`.
You can also use `OBJ_EVENT_GFX_SPECIES(SPECIES_NAME)` to get the graphics id of an overworld pokemon. For example `OBJ_EVENT_GFX_SPECIES(MAWILE)`
Jamie's Transformation Pack includes new script commands that generate graphics id based on the pokemon in your party and stores them in variables.

Before going into details about each function, here are a few things they have in common:
- they take a var like `VAR_0x8005` as their first argument and store a graphics id in that var which can then be used as an argument in a transformation command
- if they have an invalid result in some way, they will store `OBJ_EVENT_MON` which is the graphics ID for `SPECIES_NONE` and looks like a substitute doll
- they store the graphics of a pokemon not just a species and as such take into account shinyness and gender dimorphism when relevant

### `setvar_moncanlearnmove_gfx` / `setvar_monknowsmove_gfx`

These functions take two arguments, the var you want the graphics id to be stored in and a move. The game will find the first pokemon in your party that has the move in its moveset for `setvar_monknowsmove_gfx` or has the move in its teachables list (TM/HM + tutor) for `setvar_moncanlearnmove_gfx`

### `setvar_choosemon_gfx`

This function takes one the var you want the graphics id to be stored in. The function is designed to work with the result of `special ChoosePartyMon`/`chooseboxmon` allowing you to let the player pick the mon that will we be used as a basis for graphics from the party or PC

### `setvar_randompartymon_gfx`

This function only takes one argument, the var you want the graphics id to be stored in and will return the graphics id of a random party member

# Questions and Bug Reports

If you have difficulties or encounter a bug, you can post in the #romhacking-help channel of the Team Aqua Hideout (TAH) discord server and ping @Jamie
