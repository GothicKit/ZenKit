# oCTriggerScript

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCTriggerScript`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `24577`<br/>
    — Gothic II: `24577`<br/>
    **ZenKit Class:** `VTriggerScript`

A special [`zCTrigger`](zCTrigger.md) which in addition to performing all other trigger actions also calls a script
function when the trigger is activated. `OnUntrigger` events do not result in the script function being called.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type"><a href="../zCTrigger/">zCTrigger</a></li>
    <li class="sp-type">
        <span>oCTriggerScript</span>
        <ul class="sp-list">
            <li class="sp-string"><a href="#scriptFunc">scriptFunc</a> = ""</li>
        </ul>
    </li>
</ul>

## Properties

<a name="scriptFunc" class="t-str"></a> `scriptFunc`

:   The name script function to call when the trigger successfully processes a message.

## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei).
