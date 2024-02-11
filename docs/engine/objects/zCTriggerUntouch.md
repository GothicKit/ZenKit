# zCTriggerUntouch

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCTriggerUntouch`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `52224`<br/>
    — Gothic II: `52224`<br/>
    **ZenKit Class:** `VTriggerUntouch`<br/>
    **Sources:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)

A trigger which fires an `OnTrigger` event to its [`triggerTarget`](#triggerTarget) if any `OnUntouch` event is emitted
from an inanimate object within the object's bounding box.

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - zCTriggerUntouch
      {: .sp-type}
        - [triggerTarget](#triggertarget) = ""
          {: .sp-string}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - zCTriggerUntouch
      {: .sp-type}
        - [triggerTarget](#triggertarget) = ""
          {: .sp-string}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - zCTriggerUntouch
      {: .sp-type}
        - [triggerTarget](#triggertarget) = ""
          {: .sp-string}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - zCTriggerUntouch
      {: .sp-type}
        - [triggerTarget](#triggertarget) = ""
          {: .sp-string}

## Properties

#### `triggerTarget` {: .t-str}

:   The name of VObject to send the `OnTrigger` events to.
