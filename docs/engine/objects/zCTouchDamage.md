# zCTouchDamage

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCTouchDamage`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `36865`<br/>
    — Gothic II: `36865`<br/>
    **ZenKit Class:** `VTouchDamage`<br/>
    **Source:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)<br/>
    — [gothic-library.ru](http://www.gothic-library.ru/publ/octouchdamage/1-1-0-522#damage)

A VObject which damages other VObjects colliding with it.

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [zCEffect](zCEffect.md)
      {: .sp-class}
    - zCTouchDamage
      {: .sp-class}
        - TouchDamage
          {: .sp-folder}
            - [damage](#damage) = 0
              {: .sp-float}
            - DamageType
              {: .sp-folder}
                - [Barrier](#barrier) = TRUE
                  {: .sp-bool}
                - [Blunt](#blunt) = FALSE
                  {: .sp-bool}
                - [Edge](#edge) = FALSE
                  {: .sp-bool}
                - [Fire](#fire) = FALSE
                  {: .sp-bool}
                - [Fly](#fly) = FALSE
                  {: .sp-bool}
                - [Magic](#magic) = FALSE
                  {: .sp-bool}
                - [Point](#point) = FALSE
                  {: .sp-bool}
                - [Fall](#fall) = FALSE
                  {: .sp-bool}
            - [damageRepeatDelaySec](#damagerepeatdelaysec) = 0
              {: .sp-float}
            - [damageVolDownScale](#damagevoldownscale) = 0
              {: .sp-float}
            - [damageCollType](#damagecolltype) = BOX
              {: .sp-enum}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [zCEffect](zCEffect.md)
      {: .sp-class}
    - zCTouchDamage
      {: .sp-class}
        - TouchDamage
          {: .sp-folder}
            - [damage](#damage) = 0
              {: .sp-float}
            - DamageType
              {: .sp-folder}
                - [Barrier](#barrier) = TRUE
                  {: .sp-bool}
                - [Blunt](#blunt) = FALSE
                  {: .sp-bool}
                - [Edge](#edge) = FALSE
                  {: .sp-bool}
                - [Fire](#fire) = FALSE
                  {: .sp-bool}
                - [Fly](#fly) = FALSE
                  {: .sp-bool}
                - [Magic](#magic) = FALSE
                  {: .sp-bool}
                - [Point](#point) = FALSE
                  {: .sp-bool}
                - [Fall](#fall) = FALSE
                  {: .sp-bool}
            - [damageRepeatDelaySec](#damagerepeatdelaysec) = 0
              {: .sp-float}
            - [damageVolDownScale](#damagevoldownscale) = 0
              {: .sp-float}
            - [damageCollType](#damagecolltype) = BOX
              {: .sp-enum}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [zCEffect](zCEffect.md)
      {: .sp-class}
    - zCTouchDamage
      {: .sp-class}
        - TouchDamage
          {: .sp-folder}
            - [damage](#damage) = 0
              {: .sp-float}
            - DamageType
              {: .sp-folder}
                - [Barrier](#barrier) = TRUE
                  {: .sp-bool}
                - [Blunt](#blunt) = FALSE
                  {: .sp-bool}
                - [Edge](#edge) = FALSE
                  {: .sp-bool}
                - [Fire](#fire) = FALSE
                  {: .sp-bool}
                - [Fly](#fly) = FALSE
                  {: .sp-bool}
                - [Magic](#magic) = FALSE
                  {: .sp-bool}
                - [Point](#point) = FALSE
                  {: .sp-bool}
                - [Fall](#fall) = FALSE
                  {: .sp-bool}
            - [damageRepeatDelaySec](#damagerepeatdelaysec) = 0
              {: .sp-float}
            - [damageVolDownScale](#damagevoldownscale) = 0
              {: .sp-float}
            - [damageCollType](#damagecolltype) = BOX
              {: .sp-enum}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [zCEffect](zCEffect.md)
      {: .sp-class}
    - zCTouchDamage
      {: .sp-class}
        - TouchDamage
          {: .sp-folder}
            - [damage](#damage) = 0
              {: .sp-float}
            - DamageType
              {: .sp-folder}
                - [Barrier](#barrier) = TRUE
                  {: .sp-bool}
                - [Blunt](#blunt) = FALSE
                  {: .sp-bool}
                - [Edge](#edge) = FALSE
                  {: .sp-bool}
                - [Fire](#fire) = FALSE
                  {: .sp-bool}
                - [Fly](#fly) = FALSE
                  {: .sp-bool}
                - [Magic](#magic) = FALSE
                  {: .sp-bool}
                - [Point](#point) = FALSE
                  {: .sp-bool}
                - [Fall](#fall) = FALSE
                  {: .sp-bool}[zCCodeMaster.md](zCCodeMaster.md)
            - [damageRepeatDelaySec](#damagerepeatdelaysec) = 0
              {: .sp-float}
            - [damageVolDownScale](#damagevoldownscale) = 0
              {: .sp-float}
            - [damageCollType](#damagecolltype) = BOX
              {: .sp-enum}

## Properties

#### `damage` {: .sp-float}

:   The amount of damage being dealt.

#### `Barrier` {: .sp-bool}

:   `TRUE` — Deal barrier damage. `FALSE` — Do not deal barrier damage.

#### `Blunt` {: .sp-bool}

:   `TRUE` — Deal blunt damage (blunt weapons). `FALSE` — Do not deal blunt damage.

#### `Edge` {: .sp-bool}

:   `TRUE` — Deal edge damage (sharp weapons). `FALSE` — Do not deal edge damage.

#### `Fire` {: .sp-bool}

:   `TRUE` — Deal fire damage. `FALSE` — Do not deal fire damage.

#### `Fly` {: .sp-bool}

:   `TRUE` — Deal knockout damage. `FALSE` — Do not deal knockout damage.

#### `Magic` {: .sp-bool}

:   `TRUE` — Deal magic damage. `FALSE` — Do not deal magic damage.

#### `Point` {: .sp-bool}

:   `TRUE` — Deal point damage (bows and crossbows). `FALSE` — Do not deal point damage.

#### `Fall` {: .sp-bool}

:   `TRUE` — Deal fall damage. `FALSE` — Do not deal fall damage.


#### `damageRepeatDelaySec` {: .sp-float}

:   The delay between damage ticks when applying continuous damage. If set to `0`, only deals the damage once per
    collision. If set to a value greater than `0`, deals damage every `damageRepeatDelaySec` seconds.


#### `damageVolDownScale` {: .sp-float}

:   A scale value used in conjunction with [`damageCollType`](#damageCollType). Its function depends on the type of
    collision detection used.


#### `damageCollType` {: .sp-enum}

:   The type of collision detection to use for damage calculations.

    * `NONE` — Disable collision detection and thus damage application.
    * `BOX` — Scale the [`bbox3dws`](zCVob.md#bbox3dws) of the `zCTouchDamage` object by
      [`damageVolDownScale`](#damageVolDownScale) and use this new bounding box to calculate
      collisions with other VObjects
    * `POINT` — Scale the [`bbox3dws`](zCVob.md#bbox3dws) of the colliding object by
      [`damageVolDownScale`](#damageVolDownScale) and only deal damage if it contains the center
      point of the `zCTouchDamage` object. If the [`visual`](zCVob.md#visual) of the `zCTouchDamage`
      object is a particle effect, test against every particle instead.
