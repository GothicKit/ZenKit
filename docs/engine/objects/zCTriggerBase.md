# zCTriggerBase

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCTriggerBase`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `12289`<br/>
    — Gothic II: `52224`<br/>
    **ZenKit Class:** —<br/>

The base class for many triggers.

!!! warning
    This object is an abstract base class and cannot be instantiated by itself.

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - zCTriggerBase
      {: .sp-class}
        - [triggerTarget](#triggerTarget) = ""
          {: .sp-string}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - zCTriggerBase
      {: .sp-class}
        - [triggerTarget](#triggerTarget) = ""
          {: .sp-string}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - zCTriggerBase
      {: .sp-class}
        - [triggerTarget](#triggerTarget) = ""
          {: .sp-string}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - zCTriggerBase
      {: .sp-class}
        - [triggerTarget](#triggerTarget) = ""
          {: .sp-string}

## Properties

#### `triggerTarget` {: .sp-string}

:   The name of VObject to send `OnTrigger` and `OnUntrigger` events to after processing.
