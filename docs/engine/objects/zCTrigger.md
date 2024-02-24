# zCTrigger

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCTrigger`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `47105`<br/>
    — Gothic II: `47105`<br/>
    **ZenKit Class:** `VTrigger`<br/>
    **Sources:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)<br/>
    — [gothic-library.ru](http://www.gothic-library.ru/publ/class_zctrigger/1-1-0-529)

VObject to process and relay specific filtered events. When a `zCTrigger` receives an `OnTrigger`, `OnDamage`, or 
`OnTouch` event it checks whether the event source (an [`oCNpc`](oCNpc.md), the player or another VObject) should
be acknowledged (see [`respondToNPC`](#respondtonpc), [`respondToPC`](#respondtopc) and
[`respondToObject`](#respondtoobject) respectively). It then verifies if it should react to the specific event by
checking the [`reactToOnTrigger`](#reacttoontrigger), [`reactToOnDamage`](#reacttoondamage) and
[`reactToOnTouch`](#reacttoontouch) properties. If both checks succeed, an `OnTrigger` event is sent to the
[`triggerTarget`](#triggertarget) and the trigger is considered to be *activated*. `zCTrigger` objects can be enabled
and disabled by firing `OnEnable`, `OnDisable` and `OnToggleEnabled` events at them.

!!! tip
    The `respondTo{Object,PC,NPC,VobName}` properties are logically linked by and "or" expression.

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [*zCTriggerBase*](zCTriggerBase.md)
      {: .sp-class}
    - zCTrigger
      {: .sp-class}
        - Trigger
          {: .sp-folder}
            - ActivationFilter
              {: .sp-folder}
                - [reactToOnTrigger](#reacttoontrigger) = TRUE
                  {: .sp-bool}
                - [reactToOnTouch](#reacttoontouch) = TRUE
                  {: .sp-bool}
                - [reactToOnDamage](#reacttoondamage) = TRUE
                  {: .sp-bool}
                - [respondToObject](#respondtoobject) = TRUE
                  {: .sp-bool}
                - [respondToPC](#respondtopc) = TRUE
                  {: .sp-bool}
                - [respondToNPC](#respondtonpc) = TRUE
                  {: .sp-bool}
                - [startEnabled](#startenabled) = TRUE
                  {: .sp-bool}
                - [respondToVobName](#respondtovobname) = ""
                  {: .sp-string}
                - [numCanBeActivated](#numcanbeactivated) = -1
                  {: .sp-int}
                - [retriggerWaitSec](#retriggerwaitsec) = 0
                  {: .sp-float}
                - [damageThreshold](#damagethreshold) = 0
                  {: .sp-float}
            - FireBehavior
              {: .sp-folder}
                - [fireDelaySec](#firedelaysec) = 0
                  {: .sp-float}
                - [sendUntrigger](#senduntrigger) = TRUE
                  {: .sp-bool}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [*zCTriggerBase*](zCTriggerBase.md)
      {: .sp-class}
    - zCTrigger
      {: .sp-class}
        - Trigger
          {: .sp-folder}
            - ActivationFilter
              {: .sp-folder}
                - [reactToOnTrigger](#reacttoontrigger) = TRUE
                  {: .sp-bool}
                - [reactToOnTouch](#reacttoontouch) = TRUE
                  {: .sp-bool}
                - [reactToOnDamage](#reacttoondamage) = TRUE
                  {: .sp-bool}
                - [respondToObject](#respondtoobject) = TRUE
                  {: .sp-bool}
                - [respondToPC](#respondtopc) = TRUE
                  {: .sp-bool}
                - [respondToNPC](#respondtonpc) = TRUE
                  {: .sp-bool}
                - [startEnabled](#startenabled) = TRUE
                  {: .sp-bool}
                - [respondToVobName](#respondtovobname) = ""
                  {: .sp-string}
                - [numCanBeActivated](#numcanbeactivated) = -1
                  {: .sp-int}
                - [retriggerWaitSec](#retriggerwaitsec) = 0
                  {: .sp-float}
                - [damageThreshold](#damagethreshold) = 0
                  {: .sp-float}
            - FireBehavior
              {: .sp-folder}
                - [fireDelaySec](#firedelaysec) = 0
                  {: .sp-float}
                - [sendUntrigger](#senduntrigger) = TRUE
                  {: .sp-bool}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [*zCTriggerBase*](zCTriggerBase.md)
      {: .sp-class}
    - zCTrigger
      {: .sp-class}
        - Trigger
          {: .sp-folder}
            - ActivationFilter
              {: .sp-folder}
                - [reactToOnTrigger](#reacttoontrigger) = TRUE
                  {: .sp-bool}
                - [reactToOnTouch](#reacttoontouch) = TRUE
                  {: .sp-bool}
                - [reactToOnDamage](#reacttoondamage) = TRUE
                  {: .sp-bool}
                - [respondToObject](#respondtoobject) = TRUE
                  {: .sp-bool}
                - [respondToPC](#respondtopc) = TRUE
                  {: .sp-bool}
                - [respondToNPC](#respondtonpc) = TRUE
                  {: .sp-bool}
                - [startEnabled](#startenabled) = TRUE
                  {: .sp-bool}
                - [respondToVobName](#respondtovobname) = ""
                  {: .sp-string}
                - [numCanBeActivated](#numcanbeactivated) = -1
                  {: .sp-int}
                - [retriggerWaitSec](#retriggerwaitsec) = 0
                  {: .sp-float}
                - [damageThreshold](#damagethreshold) = 0
                  {: .sp-float}
            - FireBehavior
              {: .sp-folder}
                - [fireDelaySec](#firedelaysec) = 0
                  {: .sp-float}
                - [sendUntrigger](#senduntrigger) = TRUE
                  {: .sp-bool}
            - [nextTimeTriggerable](#nexttimetriggerable) = 0.0
              {: .sp-float}
            - [savedOtherVob](#savedothervob) = NULL
              {: .sp-class}
            - [countCanBeActivated](#countcanbeactivated)
              {: .sp-int}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [*zCTriggerBase*](zCTriggerBase.md)
      {: .sp-class}
    - zCTrigger
      {: .sp-class}
        - Trigger
          {: .sp-folder}
            - ActivationFilter
              {: .sp-folder}
                - [reactToOnTrigger](#reacttoontrigger) = TRUE
                  {: .sp-bool}
                - [reactToOnTouch](#reacttoontouch) = TRUE
                  {: .sp-bool}
                - [reactToOnDamage](#reacttoondamage) = TRUE
                  {: .sp-bool}
                - [respondToObject](#respondtoobject) = TRUE
                  {: .sp-bool}
                - [respondToPC](#respondtopc) = TRUE
                  {: .sp-bool}
                - [respondToNPC](#respondtonpc) = TRUE
                  {: .sp-bool}
                - [startEnabled](#startenabled) = TRUE
                  {: .sp-bool}
                - [respondToVobName](#respondtovobname) = ""
                  {: .sp-string}
                - [numCanBeActivated](#numcanbeactivated) = -1
                  {: .sp-int}
                - [retriggerWaitSec](#retriggerwaitsec) = 0
                  {: .sp-float}
                - [damageThreshold](#damagethreshold) = 0
                  {: .sp-float}
            - FireBehavior
              {: .sp-folder}
                - [fireDelaySec](#firedelaysec) = 0
                  {: .sp-float}
                - [sendUntrigger](#senduntrigger) = TRUE
                  {: .sp-bool}
            - [nextTimeTriggerable](#nexttimetriggerable) = 0.0
              {: .sp-float}
            - [savedOtherVob](#savedothervob) = NULL
              {: .sp-class}
            - [countCanBeActivated](#countcanbeactivated) = 0
              {: .sp-int}
            - [isEnabled](#isenabled) = TRUE
              {: .sp-bool}

## Properties

#### `reactToOnTrigger` {: .sp-bool}

:   Whether this trigger should react to `OnTrigger` events.
    
    * `TRUE` — Do react to `OnTrigger` events by sending an `OnTrigger` message to the [`triggerTarget`](#triggertarget)
      If [`sendUntrigger`](#senduntrigger) is set to `TRUE`, also sends an `OnUntrigger` event to the [`triggerTarget`](#triggertarget)
      if the trigger receives an `OnUntrigger` event.
    * `FALSE` — Ignore `OnTrigger` events.

#### `reactToOnTouch` {: .sp-bool}

:   Whether this trigger should react to `OnTouch` events.

    * `TRUE` — Do react to `OnTouch` events by sending an `OnTrigger` message to the [`triggerTarget`](#triggertarget)
      If [`sendUntrigger`](#senduntrigger) is set to `TRUE`, also sends an `OnUntrigger` event to the [`triggerTarget`](#triggertarget)
      if the trigger receives an `OnUntouch` event.
    * `FALSE` — Ignore `OnTouch` events.

#### `reactToOnDamage` {: .sp-bool}

:   Whether this trigger should react to `OnDamage` events.

    * `TRUE` — Do react to `OnDamage` events by sending an `OnTrigger` message to the [`triggerTarget`](#triggertarget)
    * `FALSE` — Ignore `OnDamage` events.

#### `respondToObject` {: .sp-bool}

:   Whether this trigger should process events coming from inanimate objects.

    * `TRUE` — Do process events from inanimate objects.
    * `FALSE` — Ignore events from inanimate objects.

#### `respondToPC` {: .sp-bool}

:   Whether this trigger should process events coming from the player.

    * `TRUE` — Do process events from the player.
    * `FALSE` — Ignore events from the player.

#### `respondToNPC` {: .sp-bool}

:   Whether this trigger should process events coming from NPCs.

    * `TRUE` — Do process events from NPCs.
    * `FALSE` — Ignore events from NPCs.

#### `startEnabled` {: .sp-bool}

:   Determines whether the trigger is initially enabled. Enabled triggers will process incoming events and send
    outgoing events while disabled triggers do not. Triggers can be activated and deactivated at runtime by sending
    them `OnEnable`, `OnDisable` or `OnToggleEnabled` events.

    * `TRUE` — Enable the trigger when the world is loaded.
    * `FALSE` — Do not enable the trigger when the world is loaded.

#### `respondToVobName` {: .sp-string}

:   Whether this trigger should process events coming from VObjects with this name. If empty, match no VObject
    name directly.

#### `numCanBeActivated` {: .sp-int}

:   The number of times the trigger will process incoming events. If set to `-1` the trigger will process
    an infinite number of events. 

#### `retriggerWaitSec` {: .sp-float}

:   The number of seconds that have to elapse after processing an event before the trigger will process additional events.
    All events received by the trigger during that time are ignored.

#### `damageThreshold` {: .sp-float}

:   The amount of damage which must be dealt for the trigger to react to an `OnDamage` event.

#### `fireDelaySec` {: .sp-float}

:   The number of seconds to wait before emitting the `OnTrigger` event after processing.

#### `sendUntrigger` {: .sp-bool}

:   Whether to send and `OnUntrigger` event to the [`triggerTarget`](#triggertarget) after the trigger receives an
    `OnUntrigger` or `OnUntouch` event. Only fires the `OnUntrigger` event if [`reactToOnTrigger`](#reacttoontrigger)
    and [`reactToOnTouch`](#reacttoontouch) are set to `TRUE` respectively.

#### `nextTimeTriggerable` {: .sp-float}

:   Unknown.

    !!! warning
        This property is only available in saved games.

#### `savedOtherVob` {: .sp-class}

:   Unknown.

    !!! warning
        This property is only available in saved games.

#### `countCanBeActivated` {: .sp-int}

:   Unknown.

    !!! warning
        This property is only available in saved games.

#### `isEnabled` {: .sp-bool}

:   Unknown.

    !!! warning
        This property is only available in saved games made using Gothic II.
