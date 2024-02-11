# zCMessageFilter

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCMessageFilter`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `0`<br/>
    — Gothic II: `0`<br/>
    **ZenKit Class:** `VMessageFilter`<br/>
    **Sources:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)

Transforms any incoming `OnTrigger` and `OnUntrigger` events before passing them on to the trigger target.

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - zCMessageFilter
      {: .sp-type}
        - [triggerTarget](#triggertarget) = ""
          {: .sp-string}
        - [onTrigger](#ontrigger) = MT_TRIGGER
          {: .sp-enum}
        - [onUntrigger](#onuntrigger) = MT_UNTRIGGER
          {: .sp-enum}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - zCMessageFilter
      {: .sp-type}
        - [triggerTarget](#triggertarget) = ""
          {: .sp-string}
        - [onTrigger](#ontrigger) = MT_TRIGGER
          {: .sp-enum}
        - [onUntrigger](#onuntrigger) = MT_UNTRIGGER
          {: .sp-enum}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - zCMessageFilter
      {: .sp-type}
        - [triggerTarget](#triggertarget) = ""
          {: .sp-string}
        - [onTrigger](#ontrigger) = MT_TRIGGER
          {: .sp-enum}
        - [onUntrigger](#onuntrigger) = MT_UNTRIGGER
          {: .sp-enum}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - zCMessageFilter
      {: .sp-type}
        - [triggerTarget](#triggertarget) = ""
          {: .sp-string}
        - [onTrigger](#ontrigger) = MT_TRIGGER
          {: .sp-enum}
        - [onUntrigger](#onuntrigger) = MT_UNTRIGGER
          {: .sp-enum}

## Properties

#### `triggerTarget` {: .sp-string}

:   The name of the target VObject. All `OnTrigger` and `OnUntrigger` messages received by this `zCMessageFilter`
    VObject are transformed according to [`onTrigger`](#onTrigger) and [`onUntrigger`](#onUntrigger) and then
    passed on to the VObject with the name specified in this field.

#### `onTrigger` {: .sp-enum}

:   The type of event to emit to the [`triggerTarget`](#triggerTarget) if this VObject receives an `OnTrigger` message.
    
    * `MT_NONE` — Don't emit another event.
    * `MT_TRIGGER` — Emit an `OnTrigger` event.
    * `MT_UNTRIGGER` — Emit an `OnUntrigger` event.
    * `MT_ENABLE` — Emit an `Enable` event.
    * `MT_DISABLE` — Emit a `Disable` event.
    * `MT_TOGGLE_ENABLED` — Emit a `ToggleEnabled` event.

#### `onUntrigger` {: .sp-enum}

:   The type of event to emit to the [`triggerTarget`](#triggerTarget) if this VObject receives an `OnUntrigger` message.

    * `MT_NONE` — Don't emit another event.
    * `MT_TRIGGER` — Emit an `OnTrigger` event.
    * `MT_UNTRIGGER` — Emit an `OnUntrigger` event.
    * `MT_ENABLE` — Emit an `Enable` event.
    * `MT_DISABLE` — Emit a `Disable` event.
    * `MT_TOGGLE_ENABLED` — Emit a `ToggleEnabled` event.
