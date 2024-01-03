# zCTriggerWorldStart

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCTriggerWorldStart`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `52224`<br/>
    — Gothic II: `52224`<br/>
    **ZenKit Class:** `VTriggerWorldStart`

A special trigger which fires an `OnTrigger` event to its [target](#triggerTarget) when the world is loaded and started.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type">
        <span>oCTriggerChangeLevel</span>
        <ul class="sp-list">
            <li class="sp-string"><a href="#triggerTarget">triggerTarget</a> = ""</li>
            <li class="sp-bool"><a href="#fireOnlyFirstTime">fireOnlyFirstTime</a> = FALSE</li>
        </ul>
    </li>
</ul>

## Properties

<a name="triggerTarget" class="t-str"></a> `triggerTarget`

:   The name of VObject to send an `OnTrigger` event to when the world is loaded and started.

<a name="fireOnlyFirstTime" class="t-bool"></a> `fireOnlyFirstTime`

:   Determines whether to fire the `OnTrigger` event only the first time the world is loaded.
    
    * `TRUE` — Only fire the event if this is the first time the world is loaded.
    * `FALSE` — Fire the event every time the world is loaded.

## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei).
