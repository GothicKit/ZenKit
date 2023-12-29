# zCMessageFilter

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCMessageFilter`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `0`<br/>
    — Gothic II: `0`<br/>
    **ZenKit Class:** `VMessageFilter`

Transforms any incoming `OnTrigger` and `OnUntrigger` events before passing them on to the trigger target.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type">
        <span>zCMessageFilter</span>
        <ul class="sp-list">
            <li class="sp-string"><a href="#triggerTarget">triggerTarget</a> = ""</li>
            <li class="sp-enum"><a href="#onTrigger">onTrigger</a> = MT_TRIGGER</li>
            <li class="sp-enum"><a href="#onUntrigger">onUntrigger</a> = MT_UNTRIGGER</li>
        </ul>
    </li>
</ul>

## Properties

<a name="triggerTarget" class="t-str"></a> `triggerTarget`

:   The name of the target VObject. All `OnTrigger` and `OnUntrigger` messages received by this `zCMessageFilter`
    VObject are transformed according to [`onTrigger`](#onTrigger) and [`onUntrigger`](#onUntrigger) and then
    passed on to the VObject with the name specified in this field.

<a name="onTrigger" class="t-enum"></a> `onTrigger`

:   The type of event to emit to the [`triggerTarget`](#triggerTarget) if this VObject receives an `OnTrigger` message.
    
    * `MT_NONE` — Don't emit another event.
    * `MT_TRIGGER` — Emit an `OnTrigger` event.
    * `MT_UNTRIGGER` — Emit an `OnUntrigger` event.
    * `MT_ENABLE` — Emit an `Enable` event.
    * `MT_DISABLE` — Emit a `Disable` event.
    * `MT_TOGGLE_ENABLED` — Emit a `ToggleEnabled` event.

<a name="onUntrigger" class="t-enum"></a> `onUntrigger`

:   The type of event to emit to the [`triggerTarget`](#triggerTarget) if this VObject receives an `OnUntrigger` message.

    * `MT_NONE` — Don't emit another event.
    * `MT_TRIGGER` — Emit an `OnTrigger` event.
    * `MT_UNTRIGGER` — Emit an `OnUntrigger` event.
    * `MT_ENABLE` — Emit an `Enable` event.
    * `MT_DISABLE` — Emit a `Disable` event.
    * `MT_TOGGLE_ENABLED` — Emit a `ToggleEnabled` event.

## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)
