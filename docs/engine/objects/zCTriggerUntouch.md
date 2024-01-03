# zCTriggerUntouch

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCTriggerUntouch`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `52224`<br/>
    — Gothic II: `52224`<br/>
    **ZenKit Class:** `VTriggerUntouch`

A trigger which fires an `OnTrigger` event to its [`triggerTarget`](#triggerTarget) if any `OnUntouch` event is emitted
from an inanimate object within the object's bounding box.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type">
        <span>zCTriggerUntouch</span>
        <ul class="sp-list">
            <li class="sp-string"><a href="#triggerTarget">triggerTarget</a> = ""</li>
        </ul>
    </li>
</ul>

## Properties

<a name="triggerTarget" class="t-str"></a> `triggerTarget`

:   The name of VObject to send the `OnTrigger` events to.


## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei).
