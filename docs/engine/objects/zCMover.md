# zCMover

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCMover`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `39936`<br/>
    — Gothic II: `39936`<br/>
    **ZenKit Class:** `VMover`<br/>
    **Sources:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)<br/>
    — [gothic-library.ru](http://www.gothic-library.ru/publ/class_zcmover/1-1-0-533)


A VObject which can move along a pre-determined path in response to an event. Movers can be controlled through a set of
keyframes or the animation of their visuals. If the visual has an attached animation to be used as the mover's animation,
it must have the following animations which are run depending on the mover's state: `S_OPEN`, `S_CLOSED`, `T_CLOSED_2_OPEN`,
`T_OPEN_2_CLOSED`, `S_LOCKED`, `S_UNLOCKED`, `T_UNLOCKED_TO_LOCKED`, `T_LOCKED_TO_UNLOCKED`.

Each mover is in one of three states, "open", "closed" or "moving". Keyframe `0` (or `S_OPEN` when using the visual's
animation) corresponds to the "open" state while the last keyframe (or `S_CLOSED`) corresponds to the "closed" state.
When transitioning between the "open" and "closed" states, the mover is in the "moving" state during which it
sequentially moves between its keyframes (or runs the `T_CLOSED_2_OPEN` or `T_OPEN_TO_CLOSED` animations). Movers with
their [`moverBehavior`](#moverbehavior) set to `NSTATE_SINGLE_KEYS` are an exception to this rule: each keyframe can
individually be addressed as a state.

Movers are specialized triggers. Before beginning their animation, movers first filter the incoming events as per the
[`zCTrigger`](zCTrigger.md)'s rules and only start the animation if the event passes through the filters. When the mover reaches
the "open" state after being activated, it emits an `OnTrigger` event according to the fire behavior set for the
[`zCTrigger`](zCTrigger.md). Similarly, when it reaches the "closed" state it emits an `OnUntrigger` event.

Movers can be enabled and disabled using the `OnEnable`, `OnDisable` and `OnToggleDisabled` events.

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [zCTrigger](zCTrigger.md)
      {: .sp-class}
    - zCMover
      {: .sp-class}
        - Mover
          {: .sp-folder}
            - [moverBehavior](#moverbehavior) = 2STATE_TOGGLE
              {: .sp-enum}
            - [touchBlockerDamage](#touchblockerdamage) = 0.0
              {: .sp-float}
            - [stayOpenTimeSec](#stayopentimesec) = 0.0
              {: .sp-float}
            - [moverLocked](#moverlocked) = FALSE
              {: .sp-bool}
            - [autoLinkEnabled](#autolinkenabled) = FALSE
              {: .sp-bool}
            - Keyframe
              {: .sp-folder}
                - [numKeyframes](#numkeyframes) = 0
                  {: .sp-int}
                - [moveSpeed](#movespeed) = 0.0
                  {: .sp-float}
                - [posLerpType](#poslerptype) = CURVE
                  {: .sp-enum}
                - [speedType](#speedtype) = CONST
                  {: .sp-enum}
            - Sound
              {: .sp-folder}
                - [sfxOpenStart](#sfxopenstart) = ""
                  {: .sp-string}
                - [sfxOpenEnd](#sfxopenend) = ""
                  {: .sp-string}
                - [sfxMoving](#sfxmoving) = ""
                  {: .sp-string}
                - [sfxCloseStart](#sfxclosestart) = ""
                  {: .sp-string}
                - [sfxCloseEnd](#sfxcloseend) = ""
                  {: .sp-string}
                - [sfxLock](#sfxlock) = ""
                  {: .sp-string}
                - [sfxUnlock](#sfxunlock) = ""
                  {: .sp-string}
                - [sfxUseLocked](#sfxuselocked) = ""
                  {: .sp-string}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [zCTrigger](zCTrigger.md)
      {: .sp-class}
    - zCMover
      {: .sp-class}
        - Mover
          {: .sp-folder}
            - [moverBehavior](#moverbehavior) = 2STATE_TOGGLE
              {: .sp-enum}
            - [touchBlockerDamage](#touchblockerdamage) = 0.0
              {: .sp-float}
            - [stayOpenTimeSec](#stayopentimesec) = 0.0
              {: .sp-float}
            - [moverLocked](#moverlocked) = FALSE
              {: .sp-bool}
            - [autoLinkEnabled](#autolinkenabled) = FALSE
              {: .sp-bool}
            - [autoRotate](#autorotate) = FALSE
              {: .sp-bool}
            - Keyframe
              {: .sp-folder}
                - [numKeyframes](#numkeyframes) = 0
                  {: .sp-int}
                - [moveSpeed](#movespeed) = 0.0
                  {: .sp-float}
                - [posLerpType](#poslerptype) = CURVE
                  {: .sp-enum}
                - [speedType](#speedtype) = CONST
                  {: .sp-enum}
            - Sound
              {: .sp-folder}
                - [sfxOpenStart](#sfxopenstart) = ""
                  {: .sp-string}
                - [sfxOpenEnd](#sfxopenend) = ""
                  {: .sp-string}
                - [sfxMoving](#sfxmoving) = ""
                  {: .sp-string}
                - [sfxCloseStart](#sfxclosestart) = ""
                  {: .sp-string}
                - [sfxCloseEnd](#sfxcloseend) = ""
                  {: .sp-string}
                - [sfxLock](#sfxlock) = ""
                  {: .sp-string}
                - [sfxUnlock](#sfxunlock) = ""
                  {: .sp-string}
                - [sfxUseLocked](#sfxuselocked) = ""
                  {: .sp-string}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [zCTrigger](zCTrigger.md)
      {: .sp-class}
    - zCMover
      {: .sp-class}
        - Mover
          {: .sp-folder}
            - [moverBehavior](#moverbehavior) = 2STATE_TOGGLE
              {: .sp-enum}
            - [touchBlockerDamage](#touchblockerdamage) = 0.0
              {: .sp-float}
            - [stayOpenTimeSec](#stayopentimesec) = 0.0
              {: .sp-float}
            - [moverLocked](#moverlocked) = FALSE
              {: .sp-bool}
            - [autoLinkEnabled](#autolinkenabled) = FALSE
              {: .sp-bool}
            - Keyframe
              {: .sp-folder}
                - [numKeyframes](#numkeyframes) = 0
                  {: .sp-int}
                - [moveSpeed](#movespeed) = 0.0
                  {: .sp-float}
                - [posLerpType](#poslerptype) = CURVE
                  {: .sp-enum}
                - [speedType](#speedtype) = CONST
                  {: .sp-enum}
			- [actKeyPosDelta](#actkeyposdelta) = 0 0 0
              {: .sp-vec}
			- [actKeyframeF](#actkeyframef) = 0.0
              {: .sp-float}
			- [actKeyframe](#actkeyframe) = 0
              {: .sp-int}
			- [nextKeyframe](#nextkeyframe) = 0
              {: .sp-int}
			- [moveSpeedUnit](#movespeedunit) = 0.0 
              {: .sp-float}
			- [advanceDir](#advancedir) = 0.0
              {: .sp-float}
			- [moverState](#moverstate) = ?
              {: .sp-enum}
			- [numTriggerEvents](#numtriggerevents) = 0
              {: .sp-int}
			- [stayOpenTimeDest](#stayopentimedest) = 0.0
              {: .sp-float}
            - Sound
              {: .sp-folder}
                - [sfxOpenStart](#sfxopenstart) = ""
                  {: .sp-string}
                - [sfxOpenEnd](#sfxopenend) = ""
                  {: .sp-string}
                - [sfxMoving](#sfxmoving) = ""
                  {: .sp-string}
                - [sfxCloseStart](#sfxclosestart) = ""
                  {: .sp-string}
                - [sfxCloseEnd](#sfxcloseend) = ""
                  {: .sp-string}
                - [sfxLock](#sfxlock) = ""
                  {: .sp-string}
                - [sfxUnlock](#sfxunlock) = ""
                  {: .sp-string}
                - [sfxUseLocked](#sfxuselocked) = ""
                  {: .sp-string}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [zCTrigger](zCTrigger.md)
      {: .sp-class}
    - zCMover
      {: .sp-class}
        - Mover
          {: .sp-folder}
            - [moverBehavior](#moverbehavior) = 2STATE_TOGGLE
              {: .sp-enum}
            - [touchBlockerDamage](#touchblockerdamage) = 0.0
              {: .sp-float}
            - [stayOpenTimeSec](#stayopentimesec) = 0.0
              {: .sp-float}
            - [moverLocked](#moverlocked) = FALSE
              {: .sp-bool}
            - [autoLinkEnabled](#autolinkenabled) = FALSE
              {: .sp-bool}
            - [autoRotate](#autorotate) = FALSE
              {: .sp-bool}
            - Keyframe
              {: .sp-folder}
                - [numKeyframes](#numkeyframes) = 0
                  {: .sp-int}
                - [moveSpeed](#movespeed) = 0.0
                  {: .sp-float}
                - [posLerpType](#poslerptype) = CURVE
                  {: .sp-enum}
                - [speedType](#speedtype) = CONST
                  {: .sp-enum}
			- [actKeyPosDelta](#actkeyposdelta) = 0 0 0
              {: .sp-vec}
			- [actKeyframeF](#actkeyframef) = 0.0
              {: .sp-float}
			- [actKeyframe](#actkeyframe) = 0
              {: .sp-int}
			- [nextKeyframe](#nextkeyframe) = 0
              {: .sp-int}
			- [moveSpeedUnit](#movespeedunit) = 0.0 
              {: .sp-float}
			- [advanceDir](#advancedir) = 0.0
              {: .sp-float}
			- [moverState](#moverstate) = ?
              {: .sp-enum}
			- [numTriggerEvents](#numtriggerevents) = 0
              {: .sp-int}
			- [stayOpenTimeDest](#stayopentimedest) = 0.0
              {: .sp-float}
            - Sound
              {: .sp-folder}
                - [sfxOpenStart](#sfxopenstart) = ""
                  {: .sp-string}
                - [sfxOpenEnd](#sfxopenend) = ""
                  {: .sp-string}
                - [sfxMoving](#sfxmoving) = ""
                  {: .sp-string}
                - [sfxCloseStart](#sfxclosestart) = ""
                  {: .sp-string}
                - [sfxCloseEnd](#sfxcloseend) = ""
                  {: .sp-string}
                - [sfxLock](#sfxlock) = ""
                  {: .sp-string}
                - [sfxUnlock](#sfxunlock) = ""
                  {: .sp-string}
                - [sfxUseLocked](#sfxuselocked) = ""
                  {: .sp-string}

## Properties

#### `moverBehavior` {: .sp-enum}

:   Controls how the mover behaves in response to events ("activation").

    * `2STATE_TOGGLE` — Receiving either event causes the state of the mover to swap. The direction
      of the animation is reversed.
    * `2STATE_TRIGGER_CTRL` — An `OnTrigger` causes the mover to transition to the "open" state while an `OnUntrigger`
      event causes the mover to transition to the "closed" state.
    * `2STATE_OPEN_TIMED` — An `OnTrigger` event causes the mover to transition to the "open" state. It than transitions
      to the "closed" state after a fixed amount of time specified by [`stayOpenTimeSec`](#stayopentimesec).
    * `NSTATE_LOOP` —  The mover opens and closes in a loop indefinitely.
    * `NSTATE_SINGLE_KEYS` — The mover can transition to each keyframe separately. Often used in conjunction
      with a [`zCMoverController`](zCMoverController.md).

#### `touchBlockerDamage` {: .sp-float}

:   The amount of damage to deal to objects in the way of the mover.

#### `stayOpenTimeSec` {: .sp-float}

:   The number of seconds a mover with the `2STATE_OPEN_TIMED` [behavior](#moverbehavior) stays in the "open"
    state until transitioning to the "closed" state again.

    !!! warning
        Only relevant if [`moverBehavior`](#moverbehavior) is `2STATE_OPEN_TIMED`.

#### `moverLocked` {: .sp-bool}

:   Unclear.

#### `autoLinkEnabled` {: .sp-bool}

:   Determines whether the position of the object triggering the mover should be tied to the mover's position.
    
    * `TRUE` — When the mover transitions, the position of the triggering object follows the position of the mover.
    * `FALSE` — The triggering object's position is uncoupled from the position of the mover.

#### `autoRotate` {: .sp-bool}

:   Whether to automatically rotate the mover along its movement trajectory. For example, this is used for making fish
    always face in the direction they're moving.

    * `TRUE` — Automatically face the mover forward along its trajectory.
    * `FALSE` — Never automatically rotate the mover.

    !!! warning
        Only available in Gothic II.

#### `numKeyframes` {: .sp-int}

:   Unclear.

#### `moveSpeed` {: .sp-float}

:   The movement speed for transitioning between keyframes in units per millisecond.
    
    !!! warning
        Only relevant if manually specified keyframes are used. Ignored when using the visual for the animation.

#### `posLerpType` {: .sp-enum}

:   Defines how the mover should interpolate between the keyframe positions. Only affects the mover's position,
    never its rotation.

    * `CURVE` — Smoothly transition between keyframes.
    * `LINEAR` — Linearly transition between keyframes.
    
    !!! warning
        Only relevant if manually specified keyframes are used. Ignored when using the visual for the animation.

#### `speedType` {: .sp-enum}

:   Controls the acceleration and deceleration behavior of the mover.

    * `CONST` — Mover with a constant speed.
    * `SLOW_START_END` — Slowly accelerate at the first keyframe and decelerate at the last keyframe.
    * `SLOW_START` — Slowly accelerate at the first keyframe but don't decelerate at the end.
    * `SLOW_END` — Slowly decelerate at the last keyframe but don't accelerate at the start.
    * `SEG_SLOW_START_END` — Slowly accelerate at the beginning of each keyframe and decelerate at the end of each keyframe.
    * `SEG_SLOW_START` — Slowly accelerate at the beginning of each keyframe but don't decelerate at the end.
    * `SEG_SLOW_END` — Slowly decelerate at the end of each keyframe but don't accelerate at the beginning.
    
    !!! warning
        Only relevant if manually specified keyframes are used. Ignored when using the visual for the animation.

#### `sfxOpenStart` {: .sp-string}

:   The name of the sound to play at the beginning of the opening sequence.

#### `sfxOpenEnd` {: .sp-string}

:   The name of the sound to play at the end of the opening sequence.

#### `sfxMoving` {: .sp-string}

:   The name of the sound to play in a loop while the mover is transitioning between keyframes.

#### `sfxCloseStart` {: .sp-string}

:   The name of the sound to play at the beginning of the closing sequence.

#### `sfxCloseEnd` {: .sp-string}

:   The name of the sound to play at the end of the closing sequence.

#### `sfxLock` {: .sp-string}

:   The name of the sound to play when locking a mover.

#### `sfxUnlock` {: .sp-string}

:   The name of the sound to play when unlocking a mover.

#### `sfxUseLocked` {: .sp-string}

:   The name of the sound to play when using a locked mover.

#### `actKeyPosDelta` {: .sp-vec}

:   Unknown.

    !!! warning
        This property is only available in saved games.

#### `actKeyframeF` {: .sp-float}

:   Unknown.

    !!! warning
        This property is only available in saved games.

#### `actKeyframe` {: .sp-int}

:   Unknown.

    !!! warning
        This property is only available in saved games.

#### `nextKeyframe` {: .sp-int}

:   Unknown.

    !!! warning
        This property is only available in saved games.

#### `moveSpeedUnit` {: .sp-float}

:   Unknown.

    !!! warning
        This property is only available in saved games.

#### `advanceDir` {: .sp-float}

:   Unknown.

    !!! warning
        This property is only available in saved games.

#### `moverState` {: .sp-enum}

:   Unknown.

    !!! warning
        This property is only available in saved games.

#### `numTriggerEvents` {: .sp-int}

:   Unknown.

    !!! warning
        This property is only available in saved games.

#### `stayOpenTimeDest` {: .sp-float}

:   Unknown.

    !!! warning
        This property is only available in saved games.
