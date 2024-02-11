# oCTriggerChangeLevel

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCTriggerChangeLevel`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `24577`<br/>
    — Gothic II: `24577`<br/>
    **ZenKit Class:** `VTriggerChangeLevel`<br/>
    **Sources:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)

A special [`zCTrigger`](zCTrigger.md) which in addition to performing all other trigger actions also causes the engine
to load another level when activated.

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - [zCTrigger](zCTrigger.md)
      {: .sp-type}
    - oCTriggerChangeLevel
      {: .sp-type}
        - [levelName](#levelname) = ""
          {: .sp-string}
        - [startVobName](#startvobname) = ""
          {: .sp-string}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - [zCTrigger](zCTrigger.md)
      {: .sp-type}
    - oCTriggerChangeLevel
      {: .sp-type}
        - [levelName](#levelname) = ""
          {: .sp-string}
        - [startVobName](#startvobname) = ""
          {: .sp-string}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - [zCTrigger](zCTrigger.md)
      {: .sp-type}
    - oCTriggerChangeLevel
      {: .sp-type}
        - [levelName](#levelname) = ""
          {: .sp-string}
        - [startVobName](#startvobname) = ""
          {: .sp-string}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - [zCTrigger](zCTrigger.md)
      {: .sp-type}
    - oCTriggerChangeLevel
      {: .sp-type}
        - [levelName](#levelname) = ""
          {: .sp-string}
        - [startVobName](#startvobname) = ""
          {: .sp-string}

## Properties

#### `levelName` {: .sp-string}

:   The name of the level to load including the file extension.

#### `startVobName` {: .sp-string}

:   The name of the VObject in the new level to place the player at.
