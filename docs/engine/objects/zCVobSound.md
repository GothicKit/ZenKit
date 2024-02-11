# zCVobSound

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCVobSound`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `12289`<br/>
    — Gothic II: `12289`<br/>
    **ZenKit Class:** `VSound`<br/>
    **Sources:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)<br/>
    — [gothic-library.ru](http://www.gothic-library.ru/publ/class_zcvobsound/1-1-0-701)

Emits a sound defined by a script instance in a given radius. The sound playback can be activated by sending an
`OnTrigger` event to the VObject and sending an `OnUntrigger` event disables the sound playback.

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - zCVobSound
      {: .sp-class}
        - Sound
          {: .sp-folder}
            - [sndVolume](#sndvolume) = 100
              {: .sp-float}
            - [sndMode](#sndmode) = LOOPING
              {: .sp-enum}
            - [sndRandDelay](#sndranddelay) = 5
              {: .sp-float}
            - [sndRandDelayVar](#sndranddelayvar) = 2
              {: .sp-float}
            - [sndStartOn](#sndstarton) = TRUE
              {: .sp-bool}
            - [sndAmbient3D](#sndambient3d) = FALSE
              {: .sp-bool}
            - [sndObstruction](#sndobstruction) = FALSE
              {: .sp-bool}
            - [sndConeAngle](#sndconeangle) = 0
              {: .sp-float}
            - [sndVolType](#sndvoltype) = SPHERE
              {: .sp-enum}
            - [sndRadius](#sndradius) = 1200
              {: .sp-float}
            - [sndName](#sndname) = ""
              {: .sp-string}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - zCVobSound
      {: .sp-class}
        - Sound
          {: .sp-folder}
            - [sndVolume](#sndvolume) = 100
              {: .sp-float}
            - [sndMode](#sndmode) = LOOPING
              {: .sp-enum}
            - [sndRandDelay](#sndranddelay) = 5
              {: .sp-float}
            - [sndRandDelayVar](#sndranddelayvar) = 2
              {: .sp-float}
            - [sndStartOn](#sndstarton) = TRUE
              {: .sp-bool}
            - [sndAmbient3D](#sndambient3d) = FALSE
              {: .sp-bool}
            - [sndObstruction](#sndobstruction) = FALSE
              {: .sp-bool}
            - [sndConeAngle](#sndconeangle) = 0
              {: .sp-float}
            - [sndVolType](#sndvoltype) = SPHERE
              {: .sp-enum}
            - [sndRadius](#sndradius) = 1200
              {: .sp-float}
            - [sndName](#sndname) = ""
              {: .sp-string}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - zCVobSound
      {: .sp-class}
        - Sound
          {: .sp-folder}
            - [sndVolume](#sndvolume) = 100
              {: .sp-float}
            - [sndMode](#sndmode) = LOOPING
              {: .sp-enum}
            - [sndRandDelay](#sndranddelay) = 5
              {: .sp-float}
            - [sndRandDelayVar](#sndranddelayvar) = 2
              {: .sp-float}
            - [sndStartOn](#sndstarton) = TRUE
              {: .sp-bool}
            - [sndAmbient3D](#sndambient3d) = FALSE
              {: .sp-bool}
            - [sndObstruction](#sndobstruction) = FALSE
              {: .sp-bool}
            - [sndConeAngle](#sndconeangle) = 0
              {: .sp-float}
            - [sndVolType](#sndvoltype) = SPHERE
              {: .sp-enum}
            - [sndRadius](#sndradius) = 1200
              {: .sp-float}
            - [sndName](#sndname) = ""
              {: .sp-string}
        - [soundIsRunning](#soundisrunning) = FALSE
          {: .sp-bool}
        - [soundAllowedToRun](#soundallowedtorun) = FALSE
          {: .sp-bool}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - zCVobSound
      {: .sp-class}
        - Sound
          {: .sp-folder}
            - [sndVolume](#sndvolume) = 100
              {: .sp-float}
            - [sndMode](#sndmode) = LOOPING
              {: .sp-enum}
            - [sndRandDelay](#sndranddelay) = 5
              {: .sp-float}
            - [sndRandDelayVar](#sndranddelayvar) = 2
              {: .sp-float}
            - [sndStartOn](#sndstarton) = TRUE
              {: .sp-bool}
            - [sndAmbient3D](#sndambient3d) = FALSE
              {: .sp-bool}
            - [sndObstruction](#sndobstruction) = FALSE
              {: .sp-bool}
            - [sndConeAngle](#sndconeangle) = 0
              {: .sp-float}
            - [sndVolType](#sndvoltype) = SPHERE
              {: .sp-enum}
            - [sndRadius](#sndradius) = 1200
              {: .sp-float}
            - [sndName](#sndname) = ""
              {: .sp-string}
        - [soundIsRunning](#soundisrunning) = FALSE
          {: .sp-bool}
        - [soundAllowedToRun](#soundallowedtorun) = FALSE
          {: .sp-bool}

## Properties

#### `sndVolume` {: .sp-float}

:   The volume of the sound effect in percent (0-100).

#### `sndMode` {: .sp-enum}

:   Determines how the sound effect is played.

    * `LOOPING` — The sound effect is repeated endlessly
    * `ONCE` — The sound plays once
    * `RANDOM` — The sound effect is repeated endlessly with a random delay between repetitions

#### `sndRandDelay` {: .sp-float}

:   The average delay between repetitions in seconds if [`sndMode`](#sndMode) is `RANDOM`.
    
    !!! warning
        Only relevant if [`sndMode`](#sndMode) is `RANDOM`.


#### `sndRandDelayVar` {: .sp-float}

:   The maximum deviation of the random delay between repetitions if [`sndMode`](#sndMode) is `RANDOM`. The resulting
    delay will be a value between [`sndRandDelay`](#sndRandDelay) ± [`sndRandDelayVar`](#sndRandDelayVar).

    !!! warning
        Only relevant if [`sndMode`](#sndMode) is `RANDOM`.

#### `sndStartOn` {: .sp-bool}

:   Whether to start the sound when loading the world.
    
    * `TRUE` — Start the sound when the world is loaded
    * `FALSE` — Only start the sound in response to an `OnTrigger` event

#### `sndAmbient3D` {: .sp-bool}

:   Determines whether the sound is an omnidirectional ambient sound. It will change volume depending on the distance
    to this VObject, but it won't sound as if this VObject is emitting the effect.
    <br/>`TRUE` — Emit an ambient sound effect. `FALSE` — Emit a directional sound effect.


#### `sndObstruction` {: .sp-bool}

:   Whether to dampen the sound when there is no direct path from the VObject to the listener.
    <br/>`TRUE` — The sound is dampened by obstructions. `FALSE` — The sound is not dampened.


#### `sndConeAngle` {: .sp-float}

:   When set to a value other than `0`, determines a cone in front of the VObject in which the sound can be heard.
    To listeners outside of that cone, the sound is not heard. Only values between `0` and `360` are valid.


#### `sndVolType` {: .sp-enum}

:   Determines the type of volume used to check whether the sound can be heard.

    * `SV_SPHERE` — The sound the be heard in a sphere around this VObject.
    * `SV_ELLIPSOID` — The sound can be heard within an ellipsoid defined by the VObject's bounding box.

#### `sndRadius` {: .sp-float}

:   The radius in which the sound can be heard.

#### `sndName` {: .sp-string}

:   The name of the script instance describing the sound.

#### `soundIsRunning` {: .sp-bool}

:   Unknown.

    !!! warning
        This property is only available in saved games.

#### `soundAllowedToRun` {: .sp-bool}

:   Unknown.

    !!! warning
        This property is only available in saved games.
