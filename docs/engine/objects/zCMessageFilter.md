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

Transforms any incoming `OnTrigger` and `OnUntrigger` events before passing them on to the trigger target. All 
`OnTrigger` and `OnUntrigger` messages received by this `zCMessageFilter` VObject are transformed according to 
[`onTrigger`](#ontrigger) and [`onUntrigger`](#onuntrigger) and then passed on to the VObject with the name specified 
in the [`triggerTarget`](zCTriggerBase.md#triggertarget).

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [*zCTriggerBase*](zCTriggerBase.md)
      {: .sp-class}
    - zCMessageFilter
      {: .sp-class}
        - [onTrigger](#ontrigger) = MT_TRIGGER
          {: .sp-enum}
        - [onUntrigger](#onuntrigger) = MT_UNTRIGGER
          {: .sp-enum}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [*zCTriggerBase*](zCTriggerBase.md)
      {: .sp-class}
    - zCMessageFilter
      {: .sp-class}
        - [onTrigger](#ontrigger) = MT_TRIGGER
          {: .sp-enum}
        - [onUntrigger](#onuntrigger) = MT_UNTRIGGER
          {: .sp-enum}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [*zCTriggerBase*](zCTriggerBase.md)
      {: .sp-class}
    - zCMessageFilter
      {: .sp-class}
        - [onTrigger](#ontrigger) = MT_TRIGGER
          {: .sp-enum}
        - [onUntrigger](#onuntrigger) = MT_UNTRIGGER
          {: .sp-enum}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [*zCTriggerBase*](zCTriggerBase.md)
      {: .sp-class}
    - zCMessageFilter
      {: .sp-class}
        - [onTrigger](#ontrigger) = MT_TRIGGER
          {: .sp-enum}
        - [onUntrigger](#onuntrigger) = MT_UNTRIGGER
          {: .sp-enum}

## Properties

#### `onTrigger` {: .sp-enum}

:   The type of event to emit to the [`triggerTarget`](#triggertarget) if this VObject receives an `OnTrigger` message.
    
    * `MT_NONE` — Don't emit another event.
    * `MT_TRIGGER` — Emit an `OnTrigger` event.
    * `MT_UNTRIGGER` — Emit an `OnUntrigger` event.
    * `MT_ENABLE` — Emit an `Enable` event.
    * `MT_DISABLE` — Emit a `Disable` event.
    * `MT_TOGGLE_ENABLED` — Emit a `ToggleEnabled` event.

#### `onUntrigger` {: .sp-enum}

:   The type of event to emit to the [`triggerTarget`](#triggertarget) if this VObject receives an `OnUntrigger` message.

    * `MT_NONE` — Don't emit another event.
    * `MT_TRIGGER` — Emit an `OnTrigger` event.
    * `MT_UNTRIGGER` — Emit an `OnUntrigger` event.
    * `MT_ENABLE` — Emit an `Enable` event.
    * `MT_DISABLE` — Emit a `Disable` event.
    * `MT_TOGGLE_ENABLED` — Emit a `ToggleEnabled` event.
