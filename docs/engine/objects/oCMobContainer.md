# oCMobContainer

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCMobContainer`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `64513`<br/>
    — Gothic II: `64513`<br/>
    **ZenKit Class:** `VContainer`<br/>
    **Sources:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)<br/>
    — [gothic-library.ru](http://www.gothic-library.ru/publ/ocmobcontainer/1-1-0-509)

An object which contains items.

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [oCVob](oCVob.md)
      {: .sp-class}
    - [oCMOB](oCMOB.md)
      {: .sp-class}
    - [oCMobInter](oCMobInter.md)
      {: .sp-class}
    - oCMobContainer
      {: .sp-class}
        - Lockable
          {: .sp-folder}
            - [locked](#locked) = FALSE
              {: .sp-bool}
            - [keyInstance](#keyinstance) = ""
              {: .sp-string}
            - [pickLockStr](#picklockstr) = ""
              {: .sp-string}
        - Container
          {: .sp-folder}
            - [contains](#contains) = ""
              {: .sp-string}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [oCVob](oCVob.md)
      {: .sp-class}
    - [oCMOB](oCMOB.md)
      {: .sp-class}
    - [oCMobInter](oCMobInter.md)
      {: .sp-class}
    - oCMobContainer
      {: .sp-class}
        - Lockable
          {: .sp-folder}
            - [locked](#locked) = FALSE
              {: .sp-bool}
            - [keyInstance](#keyinstance) = ""
              {: .sp-string}
            - [pickLockStr](#picklockstr) = ""
              {: .sp-string}
        - Container
          {: .sp-folder}
            - [contains](#contains) = ""
              {: .sp-string}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [oCVob](oCVob.md)
      {: .sp-class}
    - [oCMOB](oCMOB.md)
      {: .sp-class}
    - [oCMobInter](oCMobInter.md)
      {: .sp-class}
    - oCMobContainer
      {: .sp-class}
        - Lockable
          {: .sp-folder}
            - [locked](#locked) = FALSE
              {: .sp-bool}
            - [keyInstance](#keyinstance) = ""
              {: .sp-string}
            - [pickLockStr](#picklockstr) = ""
              {: .sp-string}
        - Container
          {: .sp-folder}
            - [contains](#contains) = ""
              {: .sp-string}
        - [NumOfEntries](#numofentries) = 0
          {: .sp-int}
        - [oCItem](oCItem.md) ... = NULL
          {: .sp-class}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [oCVob](oCVob.md)
      {: .sp-class}
    - [oCMOB](oCMOB.md)
      {: .sp-class}
    - [oCMobInter](oCMobInter.md)
      {: .sp-class}
    - oCMobContainer
      {: .sp-class}
        - Lockable
          {: .sp-folder}
            - [locked](#locked) = FALSE
              {: .sp-bool}
            - [keyInstance](#keyinstance) = ""
              {: .sp-string}
            - [pickLockStr](#picklockstr) = ""
              {: .sp-string}
        - Container
          {: .sp-folder}
            - [contains](#contains) = ""
              {: .sp-string}
        - [NumOfEntries](#numofentries) = 0
          {: .sp-int}
        - [oCItem](oCItem.md) ... = NULL
          {: .sp-class}

## Properties

#### `locked` {: .sp-bool}

:   Determines whether the container is locked.
    <br/>`TRUE` — The container is locked. `FALSE` — The container is not locked.

#### `keyInstance` {: .sp-string}

:   The name of the item script instance which unlocks the container.

    !!! tip
        Only relevant if the container is locked.

#### `pickLockStr` {: .sp-string}

:   The combination which unlocks this container when picking the lock. Each character of the string is either `R` or
    `L` where `R` stands for "Right" and `L` stands for "Left". If empty, the lock can not be picked.
    <br />Example: `LRRLLRL`.

    !!! tip
        Only relevant if the container is locked.

#### `contains` {: .sp-string}

:   The items found inside the container as a comma-separated list. Each element of the list starts with the name of the
    item script instance and is optionally followed by a colon and a number, indicating the number of that item to be
    found inside.
    <br />Example: `ItMi_Gold:75, ItFo_Fish:2, ItMi_Quartz`.

#### `NumOfEntries` {: .sp-int}

:   The number of items in the container.

    !!! warning
        This property is only available in saved games.
