# zCTriggerWorldStart

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCTriggerWorldStart`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `52224`<br/>
    — Gothic II: `52224`<br/>
    **ZenKit Class:** `VTriggerWorldStart`<br/>
    **Sources:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)

A special trigger which fires an `OnTrigger` event to its [target](#triggerTarget) when the world is loaded and started.

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - zCTriggerWorldStart
      {: .sp-type}
        - [triggerTarget](#triggertarget) = ""
          {: .sp-string}
        - [fireOnlyFirstTime](#fireonlyfirsttime) = FALSE
          {: .sp-bool}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - zCTriggerWorldStart
      {: .sp-type}
        - [triggerTarget](#triggertarget) = ""
          {: .sp-string}
        - [fireOnlyFirstTime](#fireonlyfirsttime) = FALSE
          {: .sp-bool}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - zCTriggerWorldStart
      {: .sp-type}
        - [triggerTarget](#triggertarget) = ""
          {: .sp-string}
        - [fireOnlyFirstTime](#fireonlyfirsttime) = FALSE
          {: .sp-bool}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - zCTriggerWorldStart
      {: .sp-type}
        - [triggerTarget](#triggertarget) = ""
          {: .sp-string}
        - [fireOnlyFirstTime](#fireonlyfirsttime) = FALSE
          {: .sp-bool}

## Properties

#### `triggerTarget` {: .t-str}

:   The name of VObject to send an `OnTrigger` event to when the world is loaded and started.

#### `fireOnlyFirstTime` {: .t-bool}

:   Determines whether to fire the `OnTrigger` event only the first time the world is loaded.
    
    * `TRUE` — Only fire the event if this is the first time the world is loaded.
    * `FALSE` — Fire the event every time the world is loaded.
