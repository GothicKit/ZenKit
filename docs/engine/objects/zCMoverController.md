# zCMoverControler

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCMoverControler`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `0`<br/>
    — Gothic II: `0`<br/>
    **ZenKit Class:** `VMoverController`

A VObject used to control [`zCMover`](zCMover.md) objects of type `NSTATE_SINGLE_KEYS` specifically.
`zCMoverControler` objects react to incoming `OnTrigger` events to send a pre-configured event to the target mover
object, controlling its animation. Mover controllers can make movers go to a specific keyframe in their animation or
just step one keyframe back and forth at a time.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type">
        <span>zCMessageFilter</span>
        <ul class="sp-list">
            <li class="sp-string"><a href="#triggerTarget">triggerTarget</a> = ""</li>
            <li class="sp-enum"><a href="#moverMessage">moverMessage</a> = GOTO_KEY_FIXED_DIRECTLY</li>
            <li class="sp-int"><a href="#gotoFixedKey">gotoFixedKey</a> = 0</li>
        </ul>
    </li>
</ul>

## Properties

<a name="triggerTarget" class="t-str"></a> `triggerTarget`

:   The name of the [`zCMover`](zCMover.md) VOject that is controlled by this `zCMoverControler`.

<a name="moverMessage" class="t-enum"></a> `moverMessage`

:   The type of event to emit to the [`triggerTarget`](#triggerTarget) when this controller receives an `OnTrigger` event.

    * `GOTO_KEY_FIXED_DIRECTLY` — Make the target mover directly go to the keyframe specified
      in [`gotoFixedKey`](#gotoFixedKey), skipping all intermediate keyframes.
    * `GOTO_KEY_FIXED_ORDER` — Make the target mover go to the keyframe specified in in [`gotoFixedKey`](#gotoFixedKey)
      while still going through all intermediate keyframes.
    * `GOTO_KEY_NEXT` — Make the target mover go to the next keyframe in sequence. Wraps around to the first keyframe.
    * `GOTO_KEY_PREV` — Make the target mover go to the previous keyframe in sequence. Wraps around to the last keyframe.

<a name="gotoFixedKey" class="t-int"></a> `gotoFixedKey`

:   The keyframe of the mover referred to by [`triggerTarget`](#triggerTarget) to move to.
    !!! warning
        Only relevant if [`moverMessage`](#moverMessage) is `GOTO_KEY_FIXED_DIRECTLY` or `GOTO_KEY_FIXED_ORDER`.

## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei) and the
Russian-language knowledge accumulator [gothic-library.ru](http://www.gothic-library.ru/publ/class_zcmovercontroler/1-1-0-528).
