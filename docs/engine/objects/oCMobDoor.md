# oCMobDoor

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCMobDoor`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `64513`<br/>
    — Gothic II: `64513`<br/>
    **ZenKit Class:** `VDoor`<br/>
    **Sources:**<br/>
    — [gothic-library.ru](http://www.gothic-library.ru/publ/ocmobdoor/1-1-0-510)

A VObject representing a door. This is also used for NPCs navigating the waynet.

=== "Gothic 1"

    - [oCVob](oCVob.md)
      {: .sp-type}
    - [oCMOB](oCMOB.md)
      {: .sp-type}
    - [oCMobInter](oCMobInter.md)
      {: .sp-type}
    - oCMobContainer
      {: .sp-type}
        - Lockable
          {: .sp-folder}
            - [locked](#locked) = FALSE
              {: .sp-bool}
            - [keyInstance](#keyinstance) = ""
              {: .sp-string}
            - [pickLockStr](#picklockstr) = ""
              {: .sp-string}

=== "Gothic 2"

    - [oCVob](oCVob.md)
      {: .sp-type}
    - [oCMOB](oCMOB.md)
      {: .sp-type}
    - [oCMobInter](oCMobInter.md)
      {: .sp-type}
    - oCMobContainer
      {: .sp-type}
        - Lockable
          {: .sp-folder}
            - [locked](#locked) = FALSE
              {: .sp-bool}
            - [keyInstance](#keyinstance) = ""
              {: .sp-string}
            - [pickLockStr](#picklockstr) = ""
              {: .sp-string}

=== "Gothic 1 (Save)"

    - [oCVob](oCVob.md)
      {: .sp-type}
    - [oCMOB](oCMOB.md)
      {: .sp-type}
    - [oCMobInter](oCMobInter.md)
      {: .sp-type}
    - oCMobContainer
      {: .sp-type}
        - Lockable
          {: .sp-folder}
            - [locked](#locked) = FALSE
              {: .sp-bool}
            - [keyInstance](#keyinstance) = ""
              {: .sp-string}
            - [pickLockStr](#picklockstr) = ""
              {: .sp-string}

=== "Gothic 2 (Save)"

    - [oCVob](oCVob.md)
      {: .sp-type}
    - [oCMOB](oCMOB.md)
      {: .sp-type}
    - [oCMobInter](oCMobInter.md)
      {: .sp-type}
    - oCMobContainer
      {: .sp-type}
        - Lockable
          {: .sp-folder}
            - [locked](#locked) = FALSE
              {: .sp-bool}
            - [keyInstance](#keyinstance) = ""
              {: .sp-string}
            - [pickLockStr](#picklockstr) = ""
              {: .sp-string}

## Properties

#### `locked` {: .t-bool}

:   Determines whether the container is locked.
    <br/>`TRUE` — The container is locked. `FALSE` — The container is not locked.

#### `keyInstance` {: .t-str}

:   The name of the item script instance which unlocks the container.

    !!! tip
        Only relevant if the container is locked.

#### `pickLockStr` {: .t-str}

:   The combination which unlocks this container when picking the lock. Each character of the string is either `R` or
    `L` where `R` stands for "Right" and `L` stands for "Left". If empty, the lock can not be picked.
    <br />Example: `LRRLLRL`.

    !!! tip
        Only relevant if the container is locked.
