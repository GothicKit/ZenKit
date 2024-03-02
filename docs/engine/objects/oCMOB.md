# `oCMOB`

!!! abstract inline end "Quick Infos"
    **Type:** Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCMOB`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `47105`<br/>
    — Gothic II: `47105`<br/>
    **ZenKit Class:** `VMovableObject` <br/>
    **Sources:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)<br />
    — [gothic-library.ru](http://www.gothic-library.ru/publ/ocmob_class/1-1-0-503)

A VObject which can optionally be moved and/or carried.

=== "Gothic 1"
    - [zCVob](zCVob.md)
      {: .sp-class}
    - [oCVob](oCVob.md)
      {: .sp-class}
    - oCMOB
      {: .sp-class}
        - MOB
          {: .sp-folder}
            - [focusName](#focusname) = 
              {: .sp-string}
            - [hitpoints](#hitpoints) = 0
              {: .sp-int}
            - [damage](#damage) = 0
              {: .sp-int}
            - [moveable](#moveable) = FALSE
              {: .sp-bool}
            - [takeable](#takeable) = FALSE
              {: .sp-bool}
            - [focusOverride](#focusoverride) = FALSE
              {: .sp-bool}
            - [soundMaterial](#soundmaterial) = WOOD
              {: .sp-enum}
            - [visualDestroyed](#visualdestroyed) = 
              {: .sp-string}
            - [owner](#owner) = 
              {: .sp-string}
            - [ownerGuild](#ownerguild) = 
              {: .sp-string}
        - [isDestroyed](#isdestroyed) = FALSE
          {: .sp-bool}

=== "Gothic 2"
    - [zCVob](zCVob.md)
      {: .sp-class}
    - [oCVob](oCVob.md)
      {: .sp-class}
    - oCMOB
      {: .sp-class}
        - MOB
          {: .sp-folder}
            - [focusName](#focusname) = 
              {: .sp-string}
            - [hitpoints](#hitpoints) = 0
              {: .sp-int}
            - [damage](#damage) = 0
              {: .sp-int}
            - [moveable](#moveable) = FALSE
              {: .sp-bool}
            - [takeable](#takeable) = FALSE
              {: .sp-bool}
            - [focusOverride](#focusoverride) = FALSE
              {: .sp-bool}
            - [soundMaterial](#soundmaterial) = WOOD
              {: .sp-enum}
            - [visualDestroyed](#visualdestroyed) = 
              {: .sp-string}
            - [owner](#owner) = 
              {: .sp-string}
            - [ownerGuild](#ownerguild) = 
              {: .sp-string}
        - [isDestroyed](#isdestroyed) = FALSE
          {: .sp-bool}

=== "Gothic 1 (Save)"
    - [zCVob](zCVob.md)
      {: .sp-class}
    - [oCVob](oCVob.md)
      {: .sp-class}
    - oCMOB
      {: .sp-class}
        - MOB
          {: .sp-folder}
            - [focusName](#focusname) = 
              {: .sp-string}
            - [hitpoints](#hitpoints) = 0
              {: .sp-int}
            - [damage](#damage) = 0
              {: .sp-int}
            - [moveable](#moveable) = FALSE
              {: .sp-bool}
            - [takeable](#takeable) = FALSE
              {: .sp-bool}
            - [focusOverride](#focusoverride) = FALSE
              {: .sp-bool}
            - [soundMaterial](#soundmaterial) = WOOD
              {: .sp-enum}
            - [visualDestroyed](#visualdestroyed) = 
              {: .sp-string}
            - [owner](#owner) = 
              {: .sp-string}
            - [ownerGuild](#ownerguild) = 
              {: .sp-string}
        - [isDestroyed](#isdestroyed) = FALSE
          {: .sp-bool}

=== "Gothic 2 (Save)"
    - [zCVob](zCVob.md)
      {: .sp-class}
    - [oCVob](oCVob.md)
      {: .sp-class}
    - oCMOB
      {: .sp-class}
        - MOB
          {: .sp-folder}
            - [focusName](#focusname) = 
              {: .sp-string}
            - [hitpoints](#hitpoints) = 0
              {: .sp-int}
            - [damage](#damage) = 0
              {: .sp-int}
            - [moveable](#moveable) = FALSE
              {: .sp-bool}
            - [takeable](#takeable) = FALSE
              {: .sp-bool}
            - [focusOverride](#focusoverride) = FALSE
              {: .sp-bool}
            - [soundMaterial](#soundmaterial) = WOOD
              {: .sp-enum}
            - [visualDestroyed](#visualdestroyed) = 
              {: .sp-string}
            - [owner](#owner) = 
              {: .sp-string}
            - [ownerGuild](#ownerguild) = 
              {: .sp-string}
        - [isDestroyed](#isdestroyed) = FALSE
          {: .sp-bool}

## Properties

#### `focusName` {: .sp-string}

:   The name of the object as seen in-game when focusing it.

#### `hitpoints` {: .sp-int}

:   The number of hitpoints of the object.

#### `damage` {: .sp-int}

:   The damage dealt by the object when being used.

#### `moveable` {: .sp-bool}

:   Whether the object can be moved by the player.

    * `False` — The object can not be moved.
    * `True` — The object can be moved.

#### `takeable` {: .sp-bool}

:   Whether the object can be carried by the player.

    * `False` — The object can not be carried.
    * `True` — The object can be carried.

#### `focusOverride` {: .sp-bool}

:   Unknown.

#### `soundMaterial` {: .sp-enum}

:   The sound to play when the object is hit.

    * `WOOD` — Wood.
    * `STONE` — Stone.
    * `METAL` — Metal.
    * `LEATHER` — Leather.
    * `CLAY` — Clay.
    * `GLAS` — Glass.

#### `visualDestroyed` {: .sp-string}

:   The name of the visual to use when the object is destroyed. Behaves like [`zCVob:visual`](zCVob.md#visual).

#### `owner` {: .sp-string}

:   The name of the script instance of the NPC who is the owner of this object.

#### `ownerGuild` {: .sp-string}

:   The name of the guild this object belongs to. This name corresponds to the script constant for each guild (i.e. `GIL_BAU`), found in `Constants.d`.

#### `isDestroyed` {: .sp-bool}

:   Unknown.

