# zCVobLight

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCVobLight`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `46080`<br/>
    — Gothic II: `39168`<br/>
    **ZenKit Class:** `VLight`<br/>
    **Sources:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)<br/>
    — [gothic-library.ru](http://www.gothic-library.ru/publ/class_zcvoblight/1-1-0-497)

Represents a virtual light source which lights up nearby game objects. Point and spot light sources are supported and
may be animated with different colors and ranges.

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - zCVobLight
      {: .sp-type}
        - VobLight
          {: .sp-folder}
            - [lightPresetInUse](#lightpresetinuse) = ""
              {: .sp-string}
            - [lightType](#lighttype) = POINT
              {: .sp-enum}
            - [range](#range) = 0
              {: .sp-float}
            - [color](#color) = 255 255 255 255
              {: .sp-color}
            - [spotConeAngle](#spotconeangle) = 0
              {: .sp-float}
            - [lightStatic](#lightstatic) = TRUE
              {: .sp-bool}
            - [lightQuality](#lightquality) = HIGH
              {: .sp-enum}
            - [lensflareFX](#lensflarefx) = ""
              {: .sp-string}
        - Dynamic Light
          {: .sp-folder}
            - [turnedOn](#turnedon) = TRUE
              {: .sp-bool}
            - [rangeAniScale](#rangeaniscale) = ""
              {: .sp-string}
            - [rangeAniFPS](#rangeanifps) = 0
              {: .sp-float}
            - [rangeAniSmooth](#rangeanismooth) = TRUE
              {: .sp-bool}
            - [colorAniList](#coloranilist) = ""
              {: .sp-string}
            - [colorAniFPS](#coloranifps) = 0
              {: .sp-float}
            - [colorAniSmooth](#coloranismooth) = TRUE
              {: .sp-bool}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - zCVobLight
      {: .sp-type}
        - VobLight
          {: .sp-folder}
            - [lightPresetInUse](#lightpresetinuse) = ""
              {: .sp-string}
            - [lightType](#lighttype) = POINT
              {: .sp-enum}
            - [range](#range) = 0
              {: .sp-float}
            - [color](#color) = 255 255 255 255
              {: .sp-color}
            - [spotConeAngle](#spotconeangle) = 0
              {: .sp-float}
            - [lightStatic](#lightstatic) = TRUE
              {: .sp-bool}
            - [lightQuality](#lightquality) = HIGH
              {: .sp-enum}
            - [lensflareFX](#lensflarefx) = ""
              {: .sp-string}
        - Dynamic Light
          {: .sp-folder}
            - [turnedOn](#turnedon) = TRUE
              {: .sp-bool}
            - [rangeAniScale](#rangeaniscale) = ""
              {: .sp-string}
            - [rangeAniFPS](#rangeanifps) = 0
              {: .sp-float}
            - [rangeAniSmooth](#rangeanismooth) = TRUE
              {: .sp-bool}
            - [colorAniList](#coloranilist) = ""
              {: .sp-string}
            - [colorAniFPS](#coloranifps) = 0
              {: .sp-float}
            - [colorAniSmooth](#coloranismooth) = TRUE
              {: .sp-bool}
            - [canMove](#canmove) = FALSE
              {: .sp-bool}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - zCVobLight
      {: .sp-type}
        - VobLight
          {: .sp-folder}
            - [lightPresetInUse](#lightpresetinuse) = ""
              {: .sp-string}
            - [lightType](#lighttype) = POINT
              {: .sp-enum}
            - [range](#range) = 0
              {: .sp-float}
            - [color](#color) = 255 255 255 255
              {: .sp-color}
            - [spotConeAngle](#spotconeangle) = 0
              {: .sp-float}
            - [lightStatic](#lightstatic) = TRUE
              {: .sp-bool}
            - [lightQuality](#lightquality) = HIGH
              {: .sp-enum}
            - [lensflareFX](#lensflarefx) = ""
              {: .sp-string}
        - Dynamic Light
          {: .sp-folder}
            - [turnedOn](#turnedon) = TRUE
              {: .sp-bool}
            - [rangeAniScale](#rangeaniscale) = ""
              {: .sp-string}
            - [rangeAniFPS](#rangeanifps) = 0
              {: .sp-float}
            - [rangeAniSmooth](#rangeanismooth) = TRUE
              {: .sp-bool}
            - [colorAniList](#coloranilist) = ""
              {: .sp-string}
            - [colorAniFPS](#coloranifps) = 0
              {: .sp-float}
            - [colorAniSmooth](#coloranismooth) = TRUE
              {: .sp-bool}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - zCVobLight
      {: .sp-type}
        - VobLight
          {: .sp-folder}
            - [lightPresetInUse](#lightpresetinuse) = ""
              {: .sp-string}
            - [lightType](#lighttype) = POINT
              {: .sp-enum}
            - [range](#range) = 0
              {: .sp-float}
            - [color](#color) = 255 255 255 255
              {: .sp-color}
            - [spotConeAngle](#spotconeangle) = 0
              {: .sp-float}
            - [lightStatic](#lightstatic) = TRUE
              {: .sp-bool}
            - [lightQuality](#lightquality) = HIGH
              {: .sp-enum}
            - [lensflareFX](#lensflarefx) = ""
              {: .sp-string}
        - Dynamic Light
          {: .sp-folder}
            - [turnedOn](#turnedon) = TRUE
              {: .sp-bool}
            - [rangeAniScale](#rangeaniscale) = ""
              {: .sp-string}
            - [rangeAniFPS](#rangeanifps) = 0
              {: .sp-float}
            - [rangeAniSmooth](#rangeanismooth) = TRUE
              {: .sp-bool}
            - [colorAniList](#coloranilist) = ""
              {: .sp-string}
            - [colorAniFPS](#coloranifps) = 0
              {: .sp-float}
            - [colorAniSmooth](#coloranismooth) = TRUE
              {: .sp-bool}
            - [canMove](#canmove) = FALSE
              {: .sp-bool}

## Properties

#### `lightPresetInUse` {: .sp-string}

:   The name of the selected light template. Templates can be found in `_work/data/Presets/LightPresets.zen`. This
    property is irrelevant for most use-cases.


#### `lightType` {: .sp-enum}

:   The type of the light source. One of:
    
    * `POINT` — A point light source.
    * `SPOT` — A spot light source.

#### `range` {: .sp-float}

:   The range in which this light source illuminates objects.

#### `color` {: .sp-color}

:   The color of this light source in RGBA form.

#### `spotConeAngle` {: .sp-float}

:   The angle of the beam cone for spotlights. Only relevant if [`lightType`](#lightType) is `SPOT`, ignored otherwise.


#### `lightStatic` {: .sp-bool}

:   Determines whether this is a static light or not. Static lights are baked into the world when its lighting is
    compiled. This makes static lights faster at runtime, but it also means, that they cannot change in any way after
    the world is compiled.
    <br />`TRUE` — This light is a static light. `FALSE` — This light is a dynamic light.

#### `lightQuality` {: .sp-enum}

:   Determines the quality of the light source. What exactly changes with these values is unknown.

    * `HIGH` — Highest quality.
    * `MEDIUM` — Medium quality.
    * `LOW_FASTEST` — Low quality (used for almost all lights).

#### `lensflareFX` {: .sp-string}

:   The name of the lensflare effect associated with this light. Empty if there is no associated lensflare effect.

#### `turnedOn` {: .sp-bool}

:   Whether this dynamic light is turned on.
    <br />`TRUE` — The light is turned on. `FALSE` — The light is turned off.

    !!! warning
        This value is only valid for dynamic lights. Check [`lightStatic`](#lightStatic) before use.

#### `rangeAniScale` {: .sp-string}

:   Frames for the light's range animation. Its value is a space-separated list of floats where each float is one key
    frame of the animation. A value of `1.0` corresponds to the initial [`range`](#range) of the light source, a value
    of `2.0` corresponds to double the initial [`range`](#range) and a value of `0.5` means half the initial
    [`range`](#range). The range animation is further controlled by [`rangeAniFPS`](#rangeAniFps) and
    [`rangeAniSmooth`](#rangeAniSmooth).

    !!! warning
        This value is only valid for dynamic lights. Check [`lightStatic`](#lightStatic) before use.

#### `rangeAniFPS` {: .sp-float}

:   The speed of the range animation in frames per second. The animation frames are defined in [`rangeAniScale`](#rangeAniScale)

    !!! warning
        This value is only valid for dynamic lights. Check [`lightStatic`](#lightStatic) before use.

#### `rangeAniSmooth` {: .sp-bool}

:   Whether the range animation should smoothly interpolate between keyframes.
    <br />`TRUE` — Turn on interpolation. `FALSE` — Turn off interpolation

    !!! warning
        This value is only valid for dynamic lights. Check [`lightStatic`](#lightStatic) before use.

#### `colorAniList` {: .sp-string}

:   Frames for the light's color animation. Its value is a space-separated list of colors where each element can either
    be an RGB color consisting of three integers enclosed by parentheses or a greyscale color consisting of a single
    integer. For example: `(255 0 0) 100 (255 50 180)` would result in <span style="color: #ff0000">#ff0000</span>
    <span style="color: #646464">#646464</span>, <span style="color: #ff32b4">#ff32b4</span>. The color animation is
    further controlled by [`colorAniFPS`](#colorAniFPS) and [`colorAniSmooth`](#colorAniSmooth).

    !!! warning
        This value is only valid for dynamic lights. Check [`lightStatic`](#lightStatic) before use.

#### `colorAniFPS` {: .sp-float}

:   The speed of the color animation in frames per second. The animation frames are defined in [`colorAniList`](#colorAniList)

    !!! warning
        This value is only valid for dynamic lights. Check [`lightStatic`](#lightStatic) before use.

#### `colorAniSmooth` {: .sp-bool}

:   Whether the color animation should smoothly interpolate between keyframes.
    <br />`TRUE` — Turn on interpolation. `FALSE` — Turn off interpolation

    !!! warning
        This value is only valid for dynamic lights. Check [`lightStatic`](#lightStatic) before use.

#### `canMove` {: .sp-bool}

:   Unknown.

    !!! warning
        This value is only valid for dynamic lights in **Gothic II**. Check [`lightStatic`](#lightStatic) before use.
