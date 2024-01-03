# zCTrigger

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCTrigger`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `47105`<br/>
    — Gothic II: `47105`<br/>
    **ZenKit Class:** `VTrigger`

VObject to process and relay specific filtered events. When a `zCTrigger` receives an `OnTrigger`, `OnDamage`, or 
`OnTouch` event it checks whether the event source (an [`oCNpc`](oCNpc.md), the player or another VObject) should
be acknowledged (see [`respondToNPC`](#respondToNPC), [`respondToPC`](#respondToPC) and
[`respondToObject`](#respondToObject) respectively). It then verifies if it should react to the specific event by
checking the [`reactToOnTrigger`](#reactToOnTrigger), [`reactToOnDamage`](#reactToOnDamage) and
[`reactToOnTouch`](#reactToOnTouch) properties. If both checks succeed, an `OnTrigger` event is sent to the
[`triggerTarget`](#triggerTarget). `zCTrigger` objects can be enabled and disabled by firing `OnEnable`, `OnDisable`
and `OnToggleEnabled` events at them.

!!! note
    The `respondTo{Object,PC,NPC,VobName}` properties are logically linked by and "or" expression.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type">
        <span>zCTrigger</span>
        <ul class="sp-list">
            <li class="sp-string"><a href="#triggerTarget">triggerTarget</a> = ""</li>
            <li class="sp-folder">
                <span>Trigger</span>
                <ul class="sp-list">
                    <li class="sp-folder">
                        <span>ActivationFilter</span>
                        <ul class="sp-list">
                            <li class="sp-bool"><a href="#reactToOnTrigger">reactToOnTrigger</a> = TRUE</li>
                            <li class="sp-bool"><a href="#reactToOnTouch">reactToOnTouch</a> = TRUE</li>
                            <li class="sp-bool"><a href="#reactToOnDamage">reactToOnDamage</a> = TRUE</li>
                            <li class="sp-bool"><a href="#respondToObject">respondToObject</a> = TRUE</li>
                            <li class="sp-bool"><a href="#respondToPC">respondToPC</a> = TRUE</li>
                            <li class="sp-bool"><a href="#respondToNPC">respondToNPC</a> = TRUE</li>
                            <li class="sp-bool"><a href="#startEnabled">startEnabled</a> = TRUE</li>
                            <li class="sp-string"><a href="#respondToVobName">respondToVobName</a> = ""</li>
                            <li class="sp-int"><a href="#numCanBeActivated">numCanBeActivated</a> = -1</li>
                            <li class="sp-float"><a href="#retriggerWaitSec">retriggerWaitSec</a> = 0</li>
                            <li class="sp-float"><a href="#damageThreshold">damageThreshold</a> = 0</li>
                        </ul>
                    </li>
                    <li class="sp-folder">
                        <span>FireBehavior</span>
                        <ul class="sp-list">
                            <li class="sp-float"><a href="#fireDelaySec">fireDelaySec</a> = 0</li>
                            <li class="sp-bool"><a href="#sendUntrigger">sendUntrigger</a> = TRUE</li>
                        </ul>
                    </li>
                </ul>
            </li>
        </ul>
    </li>
</ul>

## Properties

<a name="triggerTarget" class="t-str"></a> `triggerTarget`

:   The name of VObject to send `OnTrigger` and `OnUntrigger` events to after processing.

<a name="reactToOnTrigger" class="t-bool"></a> `reactToOnTrigger`

:   Whether this trigger should react to `OnTrigger` events.
    
    * `TRUE` — Do react to `OnTrigger` events by sending an `OnTrigger` message to the [`triggerTarget`](#triggerTarget)
      If [`sendUntrigger`](#sendUntrigger) is set to `TRUE`, also sends an `OnUntrigger` event to the [`triggerTarget`](#triggerTarget)
      if the trigger receives an `OnUntrigger` event.
    * `FALSE` — Ignore `OnTrigger` events.

<a name="reactToOnTouch" class="t-bool"></a> `reactToOnTouch`

:   Whether this trigger should react to `OnTouch` events.

    * `TRUE` — Do react to `OnTouch` events by sending an `OnTrigger` message to the [`triggerTarget`](#triggerTarget)
      If [`sendUntrigger`](#sendUntrigger) is set to `TRUE`, also sends an `OnUntrigger` event to the [`triggerTarget`](#triggerTarget)
      if the trigger receives an `OnUntouch` event.
    * `FALSE` — Ignore `OnTouch` events.

<a name="reactToOnDamage" class="t-bool"></a> `reactToOnDamage`

:   Whether this trigger should react to `OnDamage` events.

    * `TRUE` — Do react to `OnDamage` events by sending an `OnTrigger` message to the [`triggerTarget`](#triggerTarget)
    * `FALSE` — Ignore `OnDamage` events.

<a name="respondToObject" class="t-bool"></a> `respondToObject`

:   Whether this trigger should process events coming from inanimate objects.

    * `TRUE` — Do process events from inanimate objects.
    * `FALSE` — Ignore events from inanimate objects.

<a name="respondToPC" class="t-bool"></a> `respondToPC`

:   Whether this trigger should process events coming from the player.

    * `TRUE` — Do process events from the player.
    * `FALSE` — Ignore events from the player.

<a name="respondToNPC" class="t-bool"></a> `respondToNPC`

:   Whether this trigger should process events coming from NPCs.

    * `TRUE` — Do process events from NPCs.
    * `FALSE` — Ignore events from NPCs.

<a name="startEnabled" class="t-bool"></a> `startEnabled`

:   Determines whether the trigger is initially enabled. Enabled triggers will process incoming events and send
    outgoing events while disabled triggers do not. Triggers can be activated and deactivated at runtime by sending
    them `OnEnable`, `OnDisable` or `OnToggleEnabled` events.

    * `TRUE` — Enable the trigger when the world is loaded.
    * `FALSE` — Do not enable the trigger when the world is loaded.

<a name="respondToVobName" class="t-str"></a> `respondToVobName`

:   Whether this trigger should process events coming from VObjects with this name. If empty, match no VObject
    name directly.

<a name="numCanBeActivated" class="t-int"></a> `numCanBeActivated`

:   The number of times the trigger will process incoming events. If set to `-1` the trigger will process
    an infinite number of events. 

<a name="retriggerWaitSec" class="t-float"></a> `retriggerWaitSec`

:   The number of seconds that have to elapse after processing an event before the trigger will process additional events.
    All events received by the trigger during that time are ignored.

<a name="damageThreshold" class="t-float"></a> `damageThreshold`

:   The amount of damage which must be dealt for the trigger to react to an `OnDamage` event.

<a name="fireDelaySec" class="t-float"></a> `fireDelaySec`

:   The number of seconds to wait before emitting the `OnTrigger` event after processing.

<a name="sendUntrigger" class="t-bool"></a> `sendUntrigger`

:   Whether to send and `OnUntrigger` event to the [`triggerTarget`](#triggerTarget) after the trigger receives an
    `OnUntrigger` or `OnUntouch` event. Only fires the `OnUntrigger` event if [`reactToOnTrigger`](#reactToOnTrigger)
    and [`reactToOnTouch`](#reactToOnTouch) are set to `TRUE` respectively.


## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei) and the
Russian-language knowledge accumulator [gothic-library.ru](http://www.gothic-library.ru/publ/class_zctrigger/1-1-0-529).
