# oCMOB

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCMOB`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `47105`<br/>
    — Gothic II: `47105`<br/>
    **ZenKit Class:** `VMovableObject`<br/>
    **Sources:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)<br/>
    — [gothic-library.ru](http://www.gothic-library.ru/publ/ocmob_class/1-1-0-503)

A VObject which can optionally be moved and/or carried.

=== "Gothic 1"
    
    - [oCVob](oCVob.md)
      {: .sp-type}
    - oCMOB
      {: .sp-type}
        - MOB
          {: .sp-folder}
            - [focusName](#focusName) = ""
              {: .sp-string}
            - [hitpoints](#hitpoints) = 0
              {: .sp-int}
            - [damage](#damage) = 0
              {: .sp-int}
            - [moveable](#moveable) = FALSE
              {: .sp-bool}
            - [takeable](#takeable) = FALSE
              {: .sp-bool}
            - [focusOverride](#focusOverride) = FALSE
              {: .sp-bool}
            - [soundMaterial](#soundMaterial) = WOOD
              {: .sp-enum}
            - [visualDestroyed](#visualDestroyed) = ""
              {: .sp-string}
            - [owner](#owner) = ""
              {: .sp-string}
            - [ownerGuild](#ownerGuild) = ""
              {: .sp-string}
        - [isDestroyed](#isDestroyed) = ""
          {: .sp-bool}

=== "Gothic 2"

    - [oCVob](oCVob.md)
      {: .sp-type}
    - oCMOB
      {: .sp-type}
        - MOB
          {: .sp-folder}
            - [focusName](#focusName) = ""
              {: .sp-string}
            - [hitpoints](#hitpoints) = 0
              {: .sp-int}
            - [damage](#damage) = 0
              {: .sp-int}
            - [moveable](#moveable) = FALSE
              {: .sp-bool}
            - [takeable](#takeable) = FALSE
              {: .sp-bool}
            - [focusOverride](#focusOverride) = FALSE
              {: .sp-bool}
            - [soundMaterial](#soundMaterial) = WOOD
              {: .sp-enum}
            - [visualDestroyed](#visualDestroyed) = ""
              {: .sp-string}
            - [owner](#owner) = ""
              {: .sp-string}
            - [ownerGuild](#ownerGuild) = ""
              {: .sp-string}
        - [isDestroyed](#isDestroyed) = ""
          {: .sp-bool}

=== "Gothic 1 (Save)"

    - [oCVob](oCVob.md)
      {: .sp-type}
    - oCMOB
      {: .sp-type}
        - MOB
          {: .sp-folder}
            - [focusName](#focusName) = ""
              {: .sp-string}
            - [hitpoints](#hitpoints) = 0
              {: .sp-int}
            - [damage](#damage) = 0
              {: .sp-int}
            - [moveable](#moveable) = FALSE
              {: .sp-bool}
            - [takeable](#takeable) = FALSE
              {: .sp-bool}
            - [focusOverride](#focusOverride) = FALSE
              {: .sp-bool}
            - [soundMaterial](#soundMaterial) = WOOD
              {: .sp-enum}
            - [visualDestroyed](#visualDestroyed) = ""
              {: .sp-string}
            - [owner](#owner) = ""
              {: .sp-string}
            - [ownerGuild](#ownerGuild) = ""
              {: .sp-string}
        - [isDestroyed](#isDestroyed) = ""
          {: .sp-bool}

=== "Gothic 2 (Save)"

    - [oCVob](oCVob.md)
      {: .sp-type}
    - oCMOB
      {: .sp-type}
        - MOB
          {: .sp-folder}
            - [focusName](#focusName) = ""
              {: .sp-string}
            - [hitpoints](#hitpoints) = 0
              {: .sp-int}
            - [damage](#damage) = 0
              {: .sp-int}
            - [moveable](#moveable) = FALSE
              {: .sp-bool}
            - [takeable](#takeable) = FALSE
              {: .sp-bool}
            - [focusOverride](#focusOverride) = FALSE
              {: .sp-bool}
            - [soundMaterial](#soundMaterial) = WOOD
              {: .sp-enum}
            - [visualDestroyed](#visualDestroyed) = ""
              {: .sp-string}
            - [owner](#owner) = ""
              {: .sp-string}
            - [ownerGuild](#ownerGuild) = ""
              {: .sp-string}
        - [isDestroyed](#isDestroyed) = ""
          {: .sp-bool}

## Properties

#### `focusName` {: .t-str}

:   The name of the object as seen in-game when focusing it.

#### `hitpoints` {: .t-int}

:   The number of hitpoints of the object.

#### `damage` {: .t-int}

:   The damage dealt by the object when being used.

#### `moveable` {: .t-bool}

:   Whether the object can be moved by the player.
    <br />`TRUE` — The object can be moved. `FALSE` — The object can not be moved.

#### `takeable` {: .t-bool}

:   Whether the object can be carried by the player.
    <br />`TRUE` — The object can be carried. `FALSE` — The object can not be carried.

#### `focusOverride` {: .t-bool}

:   Unknown.

#### `soundMaterial` {: .t-enum}

:   The sound to play when the object is hit.

    * `WOOD` — Wood.
    * `STONE` — Stone.
    * `METAL` — Metal.
    * `LEATHER` — Leather.
    * `CLAY` — Clay.
    * `GLAS` — Glass.

#### `visualDestroyed` {: .t-str}

:   The name of the visual to use when the object is destroyed. Behaves like [zCVob.visual](zCVob.md#visual).

#### `owner` {: .t-str}

:   The name of the script instance of the NPC who is the owner of this object.

#### `ownerGuild` {: .t-str}

:   The name of the guild this object belongs to. This name corresponds to the script constant for
    each guild (i.e. `GIL_BAU`), found in `Constants.d`.

#### `isDestroyed` {: .t-bool}

:   Unknown.
