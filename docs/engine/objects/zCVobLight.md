# zCVobLight

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCVobLight`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `46080`<br/>
    — Gothic II: `39168`<br/>
    **ZenKit Class:** `VLight`

Represents a virtual light source which lights up nearby game objects. Point and spot light sources are supported and
may be animated with different colors and ranges.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type">
        <span>zCVobLight</span>
        <ul class="sp-list">
            <li class="sp-folder">
                <span>VobLight</span>
                <ul class="sp-list">
                    <li class="sp-string"><a href="#lightPresetInUse">lightPresetInUse</a> = ""</li>
                    <li class="sp-enum"><a href="#lightType">lightType</a> = POINT</li>
                    <li class="sp-float"><a href="#range">range</a> = 0</li>
                    <li class="sp-color"><a href="#color">color</a> = 255 255 255 255</li>
                    <li class="sp-float"><a href="#spotConeAngle">spotConeAngle</a> = 0</li>
                    <li class="sp-bool"><a href="#lightStatic">lightStatic</a> = TRUE</li>
                    <li class="sp-enum"><a href="#lightQuality">lightQuality</a> = HIGH</li>
                    <li class="sp-string"><a href="#lensflareFX">lensflareFX</a> = ""</li>
                </ul>
            </li>
            <li class="sp-folder">
                <span>Dynamic Light</span>
                <ul class="sp-list">
                    <li class="sp-bool"><a href="#turnedOn">turnedOn</a> = TRUE</li>
                    <li class="sp-string"><a href="#rangeAniScale">rangeAniScale</a> = ""</li>
                    <li class="sp-float"><a href="#rangeAniFPS">rangeAniFPS</a> = 0</li>
                    <li class="sp-bool"><a href="#rangeAniSmooth">rangeAniSmooth</a> = TRUE</li>
                    <li class="sp-string"><a href="#colorAniList">colorAniList</a> = ""</li>
                    <li class="sp-float"><a href="#colorAniFPS">colorAniFPS</a> = 0</li>
                    <li class="sp-bool"><a href="#colorAniSmooth">colorAniSmooth</a> = TRUE</li>
                    <li class="sp-bool"><a href="#canMove">canMove</a> = FALSE</li>
                </ul>
            </li>
        </ul>
    </li>
</ul>

## Properties

<a name="lightPresetInUse" class="t-str"></a> `lightPresetInUse`

:   The name of the selected light template. Templates can be found in `_work/data/Presets/LightPresets.zen`. This
    property is irrelevant for most use-cases.


<a name="lightType" class="t-enum"></a> `lightType`

:   The type of the light source. One of:
    
    * `POINT` — A point light source.
    * `SPOT` — A spot light source.


<a name="range" class="t-float"></a> `range`

:   The range in which this light source illuminates objects.

<a name="color" class="t-color"></a> `color`

:   The color of this light source in RGBA form.

<a name="spotConeAngle" class="t-float"></a> `spotConeAngle`

:   The angle of the beam cone for spotlights. Only relevant if [`lightType`](#lightType) is `SPOT`, ignored otherwise.


<a name="lightStatic" class="t-bool"></a> `lightStatic`

:   Determines whether this is a static light or not. Static lights are baked into the world when its lighting is
    compiled. This makes static lights faster at runtime, but it also means, that they cannot change in any way after
    the world is compiled.
    <br />`TRUE` — This light is a static light. `FALSE` — This light is a dynamic light.

<a name="lightQuality" class="t-enum"></a> `lightQuality`

:   Determines the quality of the light source. What exactly changes with these values is unknown.

    * `HIGH` — Highest quality.
    * `MEDIUM` — Medium quality.
    * `LOW_FASTEST` — Low quality (used for almost all lights).

<a name="lensflareFX" class="t-str"></a> `lensflareFX`

:   The name of the lensflare effect associated with this light. Empty if there is no associated lensflare effect.

<a name="turnedOn" class="t-bool"></a> `turnedOn`

:   Whether this dynamic light is turned on.
    <br />`TRUE` — The light is turned on. `FALSE` — The light is turned off.
    !!! warning
        This value is only valid for dynamic lights. Check [`lightStatic`](#lightStatic) before use.

<a name="rangeAniScale" class="t-str"></a> `rangeAniScale`

:   Frames for the light's range animation. Its value is a space-separated list of floats where each float is one key
    frame of the animation. A value of `1.0` corresponds to the initial [`range`](#range) of the light source, a value
    of `2.0` corresponds to double the initial [`range`](#range) and a value of `0.5` means half the initial
    [`range`](#range). The range animation is further controlled by [`rangeAniFPS`](#rangeAniFps) and
    [`rangeAniSmooth`](#rangeAniSmooth).
    !!! warning
        This value is only valid for dynamic lights. Check [`lightStatic`](#lightStatic) before use.

<a name="rangeAniFPS" class="t-float"></a> `rangeAniFPS`

:   The speed of the range animation in frames per second. The animation frames are defined in [`rangeAniScale`](#rangeAniScale)
    !!! warning
        This value is only valid for dynamic lights. Check [`lightStatic`](#lightStatic) before use.

<a name="rangeAniSmooth" class="t-bool"></a> `rangeAniSmooth`

:   Whether the range animation should smoothly interpolate between keyframes.
    <br />`TRUE` — Turn on interpolation. `FALSE` — Turn off interpolation
    !!! warning
        This value is only valid for dynamic lights. Check [`lightStatic`](#lightStatic) before use.

<a name="colorAniList" class="t-str"></a> `colorAniList`

:   Frames for the light's color animation. Its value is a space-separated list of colors where each element can either
    be an RGB color consisting of three integers enclosed by parentheses or a greyscale color consisting of a single
    integer. For example: `(255 0 0) 100 (255 50 180)` would result in <span style="color: #ff0000">#ff0000</span>
    <span style="color: #646464">#646464</span>, <span style="color: #ff32b4">#ff32b4</span>. The color animation is
    further controlled by [`colorAniFPS`](#colorAniFPS) and [`colorAniSmooth`](#colorAniSmooth).
    !!! warning
        This value is only valid for dynamic lights. Check [`lightStatic`](#lightStatic) before use.

<a name="colorAniFPS" class="t-float"></a> `colorAniFPS`

:   The speed of the color animation in frames per second. The animation frames are defined in [`colorAniList`](#colorAniList)
    !!! warning
        This value is only valid for dynamic lights. Check [`lightStatic`](#lightStatic) before use.

<a name="colorAniSmooth" class="t-bool"></a> `colorAniSmooth`

:   Whether the color animation should smoothly interpolate between keyframes.
    <br />`TRUE` — Turn on interpolation. `FALSE` — Turn off interpolation
    !!! warning
        This value is only valid for dynamic lights. Check [`lightStatic`](#lightStatic) before use.

<a name="canMove" class="t-bool"></a> `canMove`

:   Unknown.
    !!! warning
        This value is only valid for dynamic lights in **Gothic II**. Check [`lightStatic`](#lightStatic) before use.
