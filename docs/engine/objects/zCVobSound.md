# zCVobSound

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCVobSound`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `12289`<br/>
    — Gothic II: `12289`<br/>
    **ZenKit Class:** `VSound`

Emits a sound defined by a script instance in a given radius. The sound playback can be activated by sending an
`OnTrigger` event to the VObject and sending an `OnUntrigger` event disables the sound playback.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type">
        <span>zCVobSound</span>
        <ul class="sp-list">
            <li class="sp-folder">
                <span>Sound</span>
                <ul class="sp-list">
                    <li class="sp-float"><a href="#sndVolume">sndVolume</a> = 100</li>
                    <li class="sp-enum"><a href="#sndMode">sndMode</a> = LOOPING</li>
                    <li class="sp-float"><a href="#sndRandDelay">sndRandDelay</a> = 5</li>
                    <li class="sp-float"><a href="#sndRandDelayVar">sndRandDelayVar</a> = 2</li>
                    <li class="sp-bool"><a href="#sndStartOn">sndStartOn</a> = TRUE</li>
                    <li class="sp-bool"><a href="#sndAmbient3D">sndAmbient3D</a> = FALSE</li>
                    <li class="sp-bool"><a href="#sndObstruction">sndObstruction</a> = FALSE</li>
                    <li class="sp-float"><a href="#sndConeAngle">sndConeAngle</a> = 0</li>
                    <li class="sp-enum"><a href="#sndVolType">sndVolType</a> = SPHERE</li>
                    <li class="sp-float"><a href="#sndRadius">sndRadius</a> = 1200</li>
                    <li class="sp-string"><a href="#sndName">sndName</a> = ""</li>
                </ul>
            </li>
        </ul>
    </li>
</ul>

## Properties

<a name="sndVolume" class="t-float"></a> `sndVolume`

:   The volume of the sound effect in percent (0-100).

<a name="sndMode" class="t-enum"></a> `sndMode`

:   Determines how the sound effect is played.

    * `LOOPING` — The sound effect is repeated endlessly
    * `ONCE` — The sound plays once
    * `RANDOM` — The sound effect is repeated endlessly with a random delay between repetitions

<a name="sndRandDelay" class="t-float"></a> `sndRandDelay`

:   The average delay between repetitions in seconds if [`sndMode`](#sndMode) is `RANDOM`.
    
    !!! warning
        Only relevant if [`sndMode`](#sndMode) is `RANDOM`.


<a name="sndRandDelayVar" class="t-float"></a> `sndRandDelayVar`

:   The maximum deviation of the random delay between repetitions if [`sndMode`](#sndMode) is `RANDOM`. The resulting
    delay will be a value between [`sndRandDelay`](#sndRandDelay) ± [`sndRandDelayVar`](#sndRandDelayVar).

    !!! warning
        Only relevant if [`sndMode`](#sndMode) is `RANDOM`.

<a name="sndStartOn" class="t-bool"></a> `sndStartOn`

:   Whether to start the sound when loading the world.
    
    * `TRUE` — Start the sound when the world is loaded
    * `FALSE` — Only start the sound in response to an `OnTrigger` event

<a name="sndAmbient3D" class="t-bool"></a> `sndAmbient3D`

:   Determines whether the sound is an omnidirectional ambient sound. It will change volume depending on the distance
    to this VObject, but it won't sound as if this VObject is emitting the effect.
    <br/>`TRUE` — Emit an ambient sound effect. `FALSE` — Emit a directional sound effect.


<a name="sndObstruction" class="t-bool"></a> `sndObstruction`

:   Whether to dampen the sound when there is no direct path from the VObject to the listener.
    <br/>`TRUE` — The sound is dampened by obstructions. `FALSE` — The sound is not dampened.


<a name="sndConeAngle" class="t-float"></a> `sndConeAngle`

:   When set to a value other than `0`, determines a cone in front of the VObject in which the sound can be heard.
    To listeners outside of that cone, the sound is not heard. Only values between `0` and `360` are valid.


<a name="sndVolType" class="t-enum"></a> `sndVolType`

:   Determines the type of volume used to check whether the sound can be heard.

    * `SV_SPHERE` — The sound the be heard in a sphere around this VObject.
    * `SV_ELLIPSOID` — The sound can be heard within an ellipsoid defined by the VObject's bounding box.

<a name="sndRadius" class="t-float"></a> `sndRadius`

:   The radius in which the sound can be heard.

<a name="sndName" class="t-str"></a> `sndName`

:   The name of the script instance describing the sound.

## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei) and the
Russian-language knowledge accumulator [gothic-library.ru](http://www.gothic-library.ru/publ/class_zcvobsound/1-1-0-701).
