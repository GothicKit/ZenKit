# zCMoverControler

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCMoverControler`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `0`<br/>
    — Gothic II: `0`<br/>
    **ZenKit Class:** `VMoverController`<br/>
    **Sources:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)<br/>
    — [gothic-library.ru](http://www.gothic-library.ru/publ/class_zcmovercontroler/1-1-0-528)

A VObject used to control [`zCMover`](zCMover.md) objects of type `NSTATE_SINGLE_KEYS` specifically.
`zCMoverControler` objects react to incoming `OnTrigger` events to send a pre-configured event to the target mover
object named by the [`triggerTarget`](zCTriggerBase.md#triggertarget) propery, controlling its animation. Mover 
controllers can make movers go to a specific keyframe in their animation or just step one keyframe back and forth 
at a time.

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [*zCTriggerBase*](zCTriggerBase.md)
      {: .sp-class}
    - zCMoverControler
      {: .sp-class}
        - [moverMessage](#movermessage) = GOTO_KEY_FIXED_DIRECTLY
          {: .sp-enum}
        - [gotoFixedKey](#gotofixedkey) = 0
          {: .sp-int}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [*zCTriggerBase*](zCTriggerBase.md)
      {: .sp-class}
    - zCMoverControler
      {: .sp-class}
        - [moverMessage](#movermessage) = GOTO_KEY_FIXED_DIRECTLY
          {: .sp-enum}
        - [gotoFixedKey](#gotofixedkey) = 0
          {: .sp-int}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [*zCTriggerBase*](zCTriggerBase.md)
      {: .sp-class}
    - zCMoverControler
      {: .sp-class}
        - [moverMessage](#movermessage) = GOTO_KEY_FIXED_DIRECTLY
          {: .sp-enum}
        - [gotoFixedKey](#gotofixedkey) = 0
          {: .sp-int}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [*zCTriggerBase*](zCTriggerBase.md)
      {: .sp-class}
    - zCMoverControler
      {: .sp-class}
        - [moverMessage](#movermessage) = GOTO_KEY_FIXED_DIRECTLY
          {: .sp-enum}
        - [gotoFixedKey](#gotofixedkey) = 0
          {: .sp-int}


## Properties

#### `moverMessage` {: .sp-enum}

:   The type of event to emit to the [`triggerTarget`](zCTriggerBase.md#triggertarget) when this controller receives an `OnTrigger` event.

    * `GOTO_KEY_FIXED_DIRECTLY` — Make the target mover directly go to the keyframe specified
      in [`gotoFixedKey`](#gotoFixedKey), skipping all intermediate keyframes.
    * `GOTO_KEY_FIXED_ORDER` — Make the target mover go to the keyframe specified in in [`gotoFixedKey`](#gotofixedkey)
      while still going through all intermediate keyframes.
    * `GOTO_KEY_NEXT` — Make the target mover go to the next keyframe in sequence. Wraps around to the first keyframe.
    * `GOTO_KEY_PREV` — Make the target mover go to the previous keyframe in sequence. Wraps around to the last keyframe.

#### `gotoFixedKey` {: .sp-int}

:   The keyframe of the mover referred to by [`triggerTarget`](zCTriggerBase.md#triggertarget) to move to.

    !!! warning
        Only relevant if [`moverMessage`](#movermessage) is `GOTO_KEY_FIXED_DIRECTLY` or `GOTO_KEY_FIXED_ORDER`.
