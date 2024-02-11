# zCDecal

!!! abstract inline end "Quick Infos"
    **Type:** Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCDecal`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `0`<br/>
    — Gothic II: `64513`<br/>
    **ZenKit Class:** `VisualDecal`<br/>
    **Sources:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)<br/>
    — [gothic-library.ru](http://www.gothic-library.ru/publ/ocitem_class/1-1-0-502)

Decals are used to insert flat 2-dimensional textures into the game world. It is used for things like spider webs and
waterfall splash effects which don't need to be 3-dimensional objects.

=== "Gothic 1"

    - [zCVisual](zCVisual.md)
      {: .sp-class}
    - zCDecal
      {: .sp-class}
        - [name](#name) = ""
          {: .sp-string}
        - [decalDim](#decaldim) = 25 25
          {: .sp-misc}
        - [decalOffset](#decaloffset) = 0 0
          {: .sp-misc}
        - [decal2Sided](#decal2sided) = FALSE
          {: .sp-bool}
        - [decalAlphaFunc](#decalalphafunc) = NONE
          {: .sp-enum}
        - [decalTexAniFPS](#decaltexanifps) = 0.0
          {: .sp-float}

=== "Gothic 2"

    - [zCVisual](zCVisual.md)
      {: .sp-class}
    - zCDecal
      {: .sp-class}
        - [name](#name) = ""
          {: .sp-string}
        - [decalDim](#decaldim) = 25 25
          {: .sp-misc}
        - [decalOffset](#decaloffset) = 0 0
          {: .sp-misc}
        - [decal2Sided](#decal2sided) = FALSE
          {: .sp-bool}
        - [decalAlphaFunc](#decalalphafunc) = NONE
          {: .sp-enum}
        - [decalTexAniFPS](#decaltexanifps) = 0.0
          {: .sp-float}
        - [decalAlphaWeight](#decalalphaweight) = 255
          {: .sp-int}
        - [ignoreDayLight](#ignoredaylight) = FALSE
          {: .sp-bool}

=== "Gothic 1 (Save)"

    - [zCVisual](zCVisual.md)
      {: .sp-class}
    - zCDecal
      {: .sp-class}
        - [name](#name) = ""
          {: .sp-string}
        - [decalDim](#decaldim) = 25 25
          {: .sp-misc}
        - [decalOffset](#decaloffset) = 0 0
          {: .sp-misc}
        - [decal2Sided](#decal2sided) = FALSE
          {: .sp-bool}
        - [decalAlphaFunc](#decalalphafunc) = NONE
          {: .sp-enum}
        - [decalTexAniFPS](#decaltexanifps) = 0.0
          {: .sp-float}

=== "Gothic 2 (Save)"

    - [zCVisual](zCVisual.md)
      {: .sp-class}
    - zCDecal
      {: .sp-class}
        - [name](#name) = ""
          {: .sp-string}
        - [decalDim](#decaldim) = 25 25
          {: .sp-misc}
        - [decalOffset](#decaloffset) = 0 0
          {: .sp-misc}
        - [decal2Sided](#decal2sided) = FALSE
          {: .sp-bool}
        - [decalAlphaFunc](#decalalphafunc) = NONE
          {: .sp-enum}
        - [decalTexAniFPS](#decaltexanifps) = 0.0
          {: .sp-float}
        - [decalAlphaWeight](#decalalphaweight) = 255
          {: .sp-int}
        - [ignoreDayLight](#ignoredaylight) = FALSE
          {: .sp-bool}

## Properties

#### `name` {: .sp-string}

:   The name of the decal texture. If the decal is part of a VObject, this field is the same as [zCVob.visual](zCVob.md#visual).

#### `decalDim` {: .sp-misc}

:   The size of the decal texture as a 2-dimensional vector. The first element is the width, and the second is the height.

#### `decalOffset` {: .sp-misc}

:   The offset of the decal texture relative to the parent VObject's position as a 2-dimensional vector.

#### `decal2Sided` {: .sp-bool}

:   Enables or disables backface culling for the texture.
    <br />`TRUE` — Show the texture on both sides. `FALSE` — Show the texture on the front side only.

#### `decalAlphaFunc` {: .sp-enum}

:   The function to use for calculating the texture's alpha channel.
    
    * `MAT_DEFAULT` — Use the texture's alpha channel as-is.
    * `NONE` — Treat the alpha channel as an "on or off" value.
    * `BLEND` — Mixing.
    * `ADD` — Addition.
    * `SUB` — Subtraction.
    * `MUL` — Multiplication.
    * `MUL2` — Multiplication.

#### `decalTexAniFPS` {: .sp-float}

:   Determines the number of frames per **minute** to use for animated textures.

#### `decalAlphaWeight` {: .sp-int}

:   Determines the transparency of the texture between `0` and `255`. Only relevant if
    [`decalAlphaFunc`](#decalAlphaFunc) is set to a value other than `NONE`. A value of `255`
    indicates that the texture is fully visible and a value of `0` indicates that the decal
    is invisible.

    !!! warning
        This property is only available in Gothic II.

#### `ignoreDayLight` {: .sp-bool}

:   Determines whether to take the day lighting into account when calculation the brightness of this decal.
    <br />`TRUE` — To take into account daylight. `FALSE` — Do not take into account daylight.

    !!! warning
        This property is only available in Gothic II.
