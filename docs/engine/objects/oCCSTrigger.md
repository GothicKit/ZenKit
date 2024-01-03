# oCCSTrigger

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCCSTrigger`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `24577`<br/>
    — Gothic II: `24577`<br/>
    **ZenKit Class:** `VCutsceneTrigger`

A special trigger VObject which can start a cutscene. The [`triggerTarget`](zCTrigger.md#triggerTarget) must be the
name of the cutscene (without the extension) to be started. For this to work, the cutscene must already be loaded.
`OnUntrigger` events are ignored by `oCCSTrigger`s.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type"><a href="../zCTrigger/">zCTrigger</a></li>
    <li class="sp-type">
        <span>oCCSTrigger</span>
        <ul class="sp-list">
            <li class="sp-none">&lt;empty&gt;</li>
        </ul>
    </li>
</ul>


## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei) and the
Russian-language knowledge accumulator [gothic-library.ru](http://www.gothic-library.ru/publ/class_occstrigger/1-1-0-530).
