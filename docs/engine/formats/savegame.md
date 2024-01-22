# ZenGin Save Games

!!! abstract inline end "Quick Infos"
    **Type:** Asset Format<br/>
    **Format Name:** SaveGame<br/>
    **File Extension:** *Folder*<br/>
    **Class Name:** *—*<br/>
    **Encoding:** *Various*

*ZenGin* save-games are essentially just folders containing a set of files holding various data relevant to the world
and script state.

* [`LOG.SAV`](#logsav) — Contains the hero's attributes (such as XP and health) over time
* [`SAVEHDR.SAV`](#savehdrsav) — Contains the path of the world file the hero was in when creating the save
* [`SAVEINFO.SAV`](#saveinfosav) — Contains general metadata about the save-game such as its name and when it was created
* [`SAVEDAT.SAV`](#savedatsav) — Contains the script and player state among other miscellaneous values
* `THUMB.SAV` — A texture file containing a thumbnail for the save.

The rest of the files in the save will be the world state of each level the player has been in. For example in Gothic 1,
every save-game will have a `WORLD.SAV` containing the VOb-tree and Waynet of the main game world in the state it was
in when creating the save.

## `LOG.SAV`

The log file contains the hero's attributes over time. It takes the form of a tab-separated values file (TSV) where each
row represents the hero's attribute state at the point of time the save was created. The first line acts as a column
header descriptor. Here's an example file taken from a *Gothic II* save (though the columns are the same for both games):

```title="LOG.SAV"
minutes	day	hour	minute	level	experience	learnpoints	guild	strength	dexterity	max.hitpoints	max.mana	1handed	2handed	bow	crossbow	circle	picklock	pickpocket	sneak	acrobatic	
0	0	8	0	0	0	0	Gildenlos	10	10	40	10	0	0	0	0	0	0	0	0	0	
6	0	8	6	0	0	0	Gildenlos	10	10	40	10	0	0	0	0	0	0	0	0	0	
60	0	9	0	0	200	0	Gildenlos	10	10	40	10	0	0	0	0	0	0	0	0	0	
```

## `SAVEHDR.SAV`

This file contains only one line with the path of the world file to be loaded initially when playing the save. It refers
to the world file as it is found in the `worlds.vdf` file. In *Gothic I*, for example, the file might contain 

```title="SAVEHDR.SAV"
WORLD.ZEN

```

if the save was made while the hero was in the main game world. If the hero was in another world at the time, this file
would contain the path of that world instead.

## `SAVEINFO.SAV`

Contains basic metadata information about the save-game. It is presented in [ZenGin Archive](archive.md) format and was
originally only used to generate the save-game list preview when browsing saved games. It contains a single
[`oCSavegameInfo`](../objects/oCSavegameInfo.md) object.

## `SAVEDAT.SAV`

Contains most of the script and world state information of the save-game. It uses the [ZenGin Archive](archive.md)
format and structured as follows:

<ul class="sp-list">
    <li class="sp-int"><a href="#day">day</a> = 0</li>
    <li class="sp-int"><a href="#hour">hour</a> = 0</li>
    <li class="sp-int"><a href="#min">min</a> = 0</li>
    <li class="sp-type"><a href="../../objects/oCInfoManager/">(inline) oCInfoManager</a><sup>1</sup></li>
    <li class="sp-type"><a href="../../objects/oCMissionManager/">(inline) oCMissionManager</a><sup>1</sup></li>
    <li class="sp-type"><a href="../../objects/oCLogManager/">(inline) oCLogManager</a><sup>1</sup></li>
    <li class="sp-type"><a href="../../objects/oCCSManager/">oCCSManager</a></li>
    <li class="sp-int"><a href="#numSymbols">numSymbols</a> = 0</li>
    <li class="sp-folder">
        <span>[Symbol]</span>
        <ul class="sp-list">
            <li class="sp-string"><a href="#symName">symName0</a> = ""</li>
            <li class="sp-int"><a href="#symValue">symValue0</a> = 0</li>
            <li class="sp-int"><a href="#symNameCnt">symName0cnt</a> = 0</li>
            <li class="sp-int"><a href="#symValue_">symValue0_0</a> = 0</li>
        </ul>
    </li>
    <li class="sp-misc"><a href="#guildTable">guildTable</a> = #</li>
</ul>

<a name="day" class="t-int"></a> `day`

:   The number of in-game days played in the save-game.

    !!! info
        This value is the true time to be set when loading the save. The value stored in [`SAVEINFO.SAV`](saveinfosav)
        is discarded and only used in the save-game preview.

<a name="hour" class="t-int"></a> `hour`

:   The hour of the in-game [day](#day) when the save-game was created (values from `0` to `24`).

    !!! info
        This value is the true time to be set when loading the save. The value stored in [`SAVEINFO.SAV`](saveinfosav)
        is discarded and only used in the save-game preview.

<a name="min" class="t-int"></a> `min`

:   The minute of the in-game [hour](#hour) when the save-game was created (values from `0` to `59`).

    !!! info
        This value is the true time to be set when loading the save. The value stored in [`SAVEINFO.SAV`](saveinfosav)
        is discarded and only used in the save-game preview.

<a name="numSymbols" class="t-int"></a> `numSymbols`

:   The number of script symbol values that are stored in the archive.

<a name="symName" class="t-string"></a> `symName0`

:   The name of the symbol the next value is stored for. Appears for every stored symbol value and is suffixed with the
    index in the saved symbol list.

<a name="symValue" class="t-int"></a> `symValue0`

:   The single value saved for the symbol.
    
    !!! warning
        Only available in *Gothic I*-saves. *Gothic II* allows for storing multiple values per symbol; see [symName0cnt](#symNameCnt).

<a name="symNameCnt" class="t-int"></a> `symName0cnt`

:   The number of values stored for the symbol. Appears for every stored symbol value.

    !!! warning
        Only available in *Gothic II*-saves.

<a name="symValue_" class="t-int"></a> `symValue0_0`

:   The value of the symbol at the index given by the suffix. Stored sequentially.

    !!! warning
        Only available in *Gothic II*-saves.

<a name="guildTable" class="t-misc"></a> `guildTable`

:   The attitude values for each in-game guild towards each other. Stored as a 42x42 matrix in the `raw` format.


[^1]: "Inline" means, that these objects are not wrapped in an archive object. Rather, their contents are simply
      all written to the parent object directly without encapsulation.