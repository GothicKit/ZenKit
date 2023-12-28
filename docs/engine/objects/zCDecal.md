# zCDecal

!!! abstract inline end "Quick Infos"
    **Type:** Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCDecal`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `0`<br/>
    — Gothic II: `64513`<br/>
    **ZenKit Class:** `VisualDecal`

Decals are used to insert flat 2-dimensional textures into the game world. It is used for things like spider webs and
waterfall splash effects which don't need to be 3-dimensional objects.

<ul class="sp-list">
    <li class="sp-type">
        <a href="../zCVisual/">zCVisual</a>
    </li>
    <li class="sp-type">
        <span>zCDecal</span>
        <ul class="sp-list">
            <li class="sp-string"><a href="#name">name</a> = ""</li>
            <li class="sp-misc"><a href="#decalDim">decalDim</a> = 25 25</li>
            <li class="sp-misc"><a href="#decalOffset">decalOffset</a> = 0 0</li>
            <li class="sp-bool"><a href="#decal2Sided">decal2Sided</a> = FALSE</li>
            <li class="sp-enum"><a href="#decalAlphaFunc">decalAlphaFunc</a> = NONE</li>
            <li class="sp-float"><a href="#decalTexAniFPS">decalTexAniFPS</a> = 0.0</li>
            <li class="sp-int"><a href="#decalAlphaWeight">decalAlphaWeight</a> = 255</li>
            <li class="sp-bool"><a href="#ignoreDaylight">ignoreDayLight</a> = FALSE</li>
        </ul>
    </li>
</ul>

## Properties

<a name="name" class="t-str"></a> `name`

:   The name of the decal texture. If the decal is part of a VObject, this field is the same as [zCVob.visual](zCVob.md#visual).

<a name="decalDim" class="t-raw"></a> `decalDim`

:   The size of the decal texture as a 2-dimensional vector. The first element is the width, and the second is the height.

<a name="decalOffset" class="t-raw"></a> `decalOffset`

:   The offset of the decal texture relative to the parent VObject's position as a 2-dimensional vector.

<a name="decal2Sided" class="t-bool"></a> `decal2Sided`

:   Enables or disables backface culling for the texture.
    <br />`TRUE` — Show the texture on both sides. `FALSE` — Show the texture on the front side only.

<a name="decalAlphaFunc" class="t-enum"></a> `decalAlphaFunc`

:   The function to use for calculating the texture's alpha channel.
    
    * `MAT_DEFAULT` — Use the texture's alpha channel as-is.
    * `NONE` — Treat the alpha channel as an "on or off" value.
    * `BLEND` — Mixing.
    * `ADD` — Addition.
    * `SUB` — Subtraction.
    * `MUL` — Multiplication.
    * `MUL2` — Multiplication.

<a name="decalTexAniFPS" class="t-float"></a> `decalTexAniFPS`

:   Determines the number of frames per **minute** to use for animated textures.

<a name="decalAlphaWeight" class="t-int"></a> `decalAlphaWeight`

:   Determines the transparency of the texture between `0` and `255`. Only relevant if
    [`decalAlphaFunc`](#decalAlphaFunc) is set to a value other than `NONE`. A value of `255`
    indicates that the texture is fully visible and a value of `0` indicates that the decal
    is invisible.
    !!! info
        This property is only available in Gothic II.

<a name="ignoreDaylight" class="t-bool"></a> `ignoreDayLight`

:   Determines whether to take the day lighting into account when calculation the brightness of this decal.
    <br />`TRUE` — To take into account daylight. `FALSE` — Do not take into account daylight.
    !!! info
        This property is only available in Gothic II.


