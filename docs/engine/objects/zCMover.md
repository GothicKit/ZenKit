# zCMover

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCMover`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `39936`<br/>
    — Gothic II: `39936`<br/>
    **ZenKit Class:** `VMover`

A VObject which can move along a pre-determined path in response to an event. Movers can be controlled through a set of
keyframes or the animation of their visuals. If the visual has an attached animation to be used as the mover's animation,
it must have the following animations which are run depending on the mover's state: `S_OPEN`, `S_CLOSED`, `T_CLOSED_2_OPEN`,
`T_OPEN_2_CLOSED`, `S_LOCKED`, `S_UNLOCKED`, `T_UNLOCKED_TO_LOCKED`, `T_LOCKED_TO_UNLOCKED`.

Each mover is in one of three states, "open", "closed" or "moving". Keyframe `0` (or `S_OPEN` when using the visual's
animation) corresponds to the "open" state while the last keyframe (or `S_CLOSED`) corresponds to the "closed" state.
When transitioning between the "open" and "closed" states, the mover is in the "moving" state during which it
sequentially moves between its keyframes (or runs the `T_CLOSED_2_OPEN` or `T_OPEN_TO_CLOSED` animations). Movers with
their [`moverBehavior`](#moverBehavior) set to `NSTATE_SINGLE_KEYS` are an exception to this rule: each keyframe can
individually be addressed as a state.

Movers are specialized triggers. Before beginning their animation, movers first filter the incoming events as per the
[`zCTrigger`](zCTrigger.md)'s rules and only start the animation if the event passes through the filters. When the
the mover reaches the "open" state after being activated, it emits an `OnTrigger` event according to the fire behavior
set for the [`zCTrigger`](zCTrigger.md). Similarly, when it reaches the "closed" state it emits an `OnUntrigger` event.

Movers can be enabled and disabled using the `OnEnable`, `OnDisable` and `OnToggleDisabled` events.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type"><a href="../zCTrigger/">zCTrigger</a></li>
    <li class="sp-type">
        <span>zCMover</span>
        <ul class="sp-list">
            <li class="sp-folder">
                <span>Mover</span>
                <ul class="sp-list">
                    <li class="sp-enum"><a href="#moverBehavior">moverBehavior</a> = 2STATE_TOGGLE</li>
                    <li class="sp-float"><a href="#touchBlockerDamage">touchBlockerDamage</a> = 0.0</li>
                    <li class="sp-float"><a href="#stayOpenTimeSec">stayOpenTimeSec</a> = 0.0</li>
                    <li class="sp-bool"><a href="#moverLocked">moverLocked</a> = FALSE</li>
                    <li class="sp-bool"><a href="#autoLinkEnabled">autoLinkEnabled</a> = FALSE</li>
                    <li class="sp-bool"><a href="#autoRotate">autoRotate</a> = FALSE</li>
                    <li class="sp-folder">
                        <span>Keyframe</span>
                        <ul class="sp-list">
                            <li class="sp-int"><a href="#numKeyframes">numKeyframes</a> = 0</li>
                            <li class="sp-float"><a href="#moveSpeed">moveSpeed</a> = 0.0</li>
                            <li class="sp-enum"><a href="#posLerpType">posLerpType</a> = CURVE</li>
                            <li class="sp-enum"><a href="#speedType">speedType</a> = CONST</li>
                        </ul>
                    </li>
                    <li class="sp-folder">
                        <span>Sound</span>
                        <ul class="sp-list">
                            <li class="sp-string"><a href="#sfxOpenStart">sfxOpenStart</a> = ""</li>
                            <li class="sp-string"><a href="#sfxOpenEnd">sfxOpenEnd</a> = ""</li>
                            <li class="sp-string"><a href="#sfxMoving">sfxMoving</a> = ""</li>
                            <li class="sp-string"><a href="#sfxCloseStart">sfxCloseStart</a> = ""</li>
                            <li class="sp-string"><a href="#sfxCloseEnd">sfxCloseEnd</a> = ""</li>
                            <li class="sp-string"><a href="#sfxLock">sfxLock</a> = ""</li>
                            <li class="sp-string"><a href="#sfxUnlock">sfxUnlock</a> = ""</li>
                            <li class="sp-string"><a href="#sfxUseLocked">sfxUseLocked</a> = ""</li>
                        </ul>
                    </li>
                </ul>
            </li>
        </ul>
    </li>
</ul>

## Properties

<a name="moverBehavior" class="t-enum"></a> `moverBehavior`

:   Controls how the mover behaves in response to events ("activation").

    * `2STATE_TOGGLE` — Receiving either event causes the state of the mover to swap. The direction
      of the animation is reversed.
    * `2STATE_TRIGGER_CTRL` — An `OnTrigger` causes the mover to transition to the "open" state while an `OnUntrigger`
      event causes the mover to transition to the "closed" state.
    * `2STATE_OPEN_TIMED` — An `OnTrigger` event causes the mover to transition to the "open" state. It than transitions
      to the "closed" state after a fixed amount of time specified by [`stayOpenTimeSec`](#stayOpenTimeSec).
    * `NSTATE_LOOP` —  The mover opens and closes in a loop indefinitely.
    * `NSTATE_SINGLE_KEYS` — The mover can transition to each keyframe separately. Often used in conjunction
      with a [`zCMoverController`](zCMoverController.md).

<a name="touchBlockerDamage" class="t-float"></a> `touchBlockerDamage`

:   The amount of damage to deal to objects in the way of the mover.

<a name="stayOpenTimeSec" class="t-float"></a> `stayOpenTimeSec`

:   The number of seconds a mover with the `2STATE_OPEN_TIMED` [behavior](#moverBehavior) stays in the "open"
    state until transitioning to the "closed" state again.

    !!! warning
        Only relevant if [`moverBehavior`](#moverBehavior) is `2STATE_OPEN_TIMED`.

<a name="moverLocked" class="t-bool"></a> `moverLocked`

:   Unclear.

<a name="autoLinkEnabled" class="t-bool"></a> `autoLinkEnabled`

:   Determines whether the position of the object triggering the mover should be tied to the mover's position.
    
    * `TRUE` — When the mover transitions, the position of the triggering object follows the position of the mover.
    * `FALSE` — The triggering object's position is uncoupled from the position of the mover.

<a name="autoRotate" class="t-bool"></a> `autoRotate`

:   Whether to automatically rotate the mover along its movement trajectory. For example, this is used for making fish
    always face in the direction they're moving.

    * `TRUE` — Automatically face the mover forward along its trajectory.
    * `FALSE` — Never automatically rotate the mover.

    !!! warning
        Only available in Gothic II.

<a name="numKeyframes" class="t-int"></a> `numKeyframes`

:   Unclear.

<a name="moveSpeed" class="t-float"></a> `moveSpeed`

:   The movement speed for transitioning between keyframes in units per millisecond.
    
    !!! warning
        Only relevant if manually specified keyframes are used. Ignored when using the visual for the animation.

<a name="posLerpType" class="t-enum"></a> `posLerpType`

:   Defines how the mover should interpolate between the keyframe positions. Only affects the mover's position,
    never its rotation.

    * `CURVE` — Smoothly transition between keyframes.
    * `LINEAR` — Linearly transition between keyframes.
    
    !!! warning
        Only relevant if manually specified keyframes are used. Ignored when using the visual for the animation.

<a name="speedType" class="t-enum"></a> `speedType`

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

<a name="sfxOpenStart" class="t-str"></a> `sfxOpenStart`

:   The name of the sound to play at the beginning of the opening sequence.

<a name="sfxOpenEnd" class="t-str"></a> `sfxOpenEnd`

:   The name of the sound to play at the end of the opening sequence.

<a name="sfxMoving" class="t-str"></a> `sfxMoving`

:   The name of the sound to play in a loop while the mover is transitioning between keyframes.

<a name="sfxCloseStart" class="t-str"></a> `sfxCloseStart`

:   The name of the sound to play at the beginning of the closing sequence.

<a name="sfxCloseEnd" class="t-str"></a> `sfxCloseEnd`

:   The name of the sound to play at the end of the closing sequence.

<a name="sfxLock" class="t-str"></a> `sfxLock`

:   The name of the sound to play when locking a mover.

<a name="sfxUnlock" class="t-str"></a> `sfxUnlock`

:   The name of the sound to play when unlocking a mover.

<a name="sfxUseLocked" class="t-str"></a> `sfxUseLocked`

:   The name of the sound to play when using a locked mover.

## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei) and the
Russian-language knowledge accumulator [gothic-library.ru](http://www.gothic-library.ru/publ/class_zcmover/1-1-0-533).
