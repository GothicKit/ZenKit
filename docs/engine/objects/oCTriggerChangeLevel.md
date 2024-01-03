# oCTriggerChangeLevel

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCTriggerChangeLevel`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `24577`<br/>
    — Gothic II: `24577`<br/>
    **ZenKit Class:** `VTriggerChangeLevel`

A special [`zCTrigger`](zCTrigger.md) which in addition to performing all other trigger actions also causes the engine
to load another level when activated.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type"><a href="../zCTrigger/">zCTrigger</a></li>
    <li class="sp-type">
        <span>oCTriggerChangeLevel</span>
        <ul class="sp-list">
            <li class="sp-string"><a href="#levelName">levelName</a> = ""</li>
            <li class="sp-string"><a href="#startVobName">startVobName</a> = ""</li>
        </ul>
    </li>
</ul>

## Properties

<a name="levelName" class="t-str"></a> `levelName`

:   The name of the level to load including the file extension.

<a name="startVobName" class="t-str"></a> `startVobName`

:   The name of the VObject in the new level to place the player at.

## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei).
