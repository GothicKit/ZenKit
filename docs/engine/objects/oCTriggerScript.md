# oCTriggerScript

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCTriggerScript`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `24577`<br/>
    — Gothic II: `24577`<br/>
    **ZenKit Class:** `VTriggerScript`<br/>
    **Sources:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)

A special [`zCTrigger`](zCTrigger.md) which in addition to performing all other trigger actions also calls a script
function when the trigger is activated. `OnUntrigger` events do not result in the script function being called.

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - [zCTrigger](zCTrigger.md)
      {: .sp-type}
    - oCTriggerScript
      {: .sp-type}
        - [scriptFunc](#scriptfunc) = ""
          {: .sp-string}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - [zCTrigger](zCTrigger.md)
      {: .sp-type}
    - oCTriggerScript
      {: .sp-type}
        - [scriptFunc](#scriptfunc) = ""
          {: .sp-string}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - [zCTrigger](zCTrigger.md)
      {: .sp-type}
    - oCTriggerScript
      {: .sp-type}
        - [scriptFunc](#scriptfunc) = ""
          {: .sp-string}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - [zCTrigger](zCTrigger.md)
      {: .sp-type}
    - oCTriggerScript
      {: .sp-type}
        - [scriptFunc](#scriptfunc) = ""
          {: .sp-string}

## Properties

#### `scriptFunc` {: .t-str}

:   The name script function to call when the trigger successfully processes a message.
