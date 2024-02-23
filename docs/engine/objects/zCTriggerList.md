# zCTriggerList

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCTriggerList`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `59776`<br/>
    — Gothic II: `5505`<br/>
    **ZenKit Class:** `VTriggerList`<br/>
    **Sources:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)<br/>
    — [gothic-library.ru](http://www.gothic-library.ru/publ/class_zctriggerlist/1-1-0-534)

A special trigger which emits the `OnTrigger` events emitted by the [`zCTrigger`](zCTrigger.md) to multiple targets.
How the events are sent can be configured through [`listProcess`](#listprocess). The
[`triggerTarget`](zCTrigger.md#triggertarget) and [`fireDelaySec`](zCTrigger.md#firedelaysec) properties of
[`zCTrigger`](zCTrigger.md) should be ignored.

`OnUntrigger` events sent to the trigger list are propagated to all targets in the list.

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [zCTrigger](zCTrigger.md)
      {: .sp-class}
    - zCTriggerList
      {: .sp-class}
        - TriggerList
          {: .sp-folder}
            - [listProcess](#listprocess) = LP_ALL
              {: .sp-enum}
            - [triggerTarget0](#triggertarget) = ""
              {: .sp-string}
            - [fireDelay0](#firedelay) = 0
              {: .sp-float}
            - [triggerTarget1](#triggertarget) = ""
              {: .sp-string}
            - [fireDelay1](#firedelay) = 0
              {: .sp-float}
            - [triggerTarget2](#triggertarget) = ""
              {: .sp-string}
            - [fireDelay2](#firedelay) = 0
              {: .sp-float}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [zCTrigger](zCTrigger.md)
      {: .sp-class}
    - zCTriggerList
      {: .sp-class}
        - TriggerList
          {: .sp-folder}
            - [listProcess](#listprocess) = LP_ALL
              {: .sp-enum}
            - [triggerTarget0](#triggertarget) = ""
              {: .sp-string}
            - [fireDelay0](#firedelay) = 0
              {: .sp-float}
            - [triggerTarget1](#triggertarget) = ""
              {: .sp-string}
            - [fireDelay1](#firedelay) = 0
              {: .sp-float}
            - [triggerTarget2](#triggertarget) = ""
              {: .sp-string}
            - [fireDelay2](#firedelay) = 0
              {: .sp-float}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [zCTrigger](zCTrigger.md)
      {: .sp-class}
    - zCTriggerList
      {: .sp-class}
        - TriggerList
          {: .sp-folder}
            - [listProcess](#listprocess) = LP_ALL
              {: .sp-enum}
            - [triggerTarget0](#triggertarget) = ""
              {: .sp-string}
            - [fireDelay0](#firedelay) = 0
              {: .sp-float}
            - [triggerTarget1](#triggertarget) = ""
              {: .sp-string}
            - [fireDelay1](#firedelay) = 0
              {: .sp-float}
            - [triggerTarget2](#triggertarget) = ""
              {: .sp-string}
            - [fireDelay2](#firedelay) = 0
              {: .sp-float}
            - [actTarget](#acttarget) = 0
              {: .sp-int}
            - [sendOnTrigger](#sendontrigger) = TRUE
              {: .sp-bool}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [zCTrigger](zCTrigger.md)
      {: .sp-class}
    - zCTriggerList
      {: .sp-class}
        - TriggerList
          {: .sp-folder}
            - [listProcess](#listprocess) = LP_ALL
              {: .sp-enum}
            - [triggerTarget0](#triggertarget) = ""
              {: .sp-string}
            - [fireDelay0](#firedelay) = 0
              {: .sp-float}
            - [triggerTarget1](#triggertarget) = ""
              {: .sp-string}
            - [fireDelay1](#firedelay) = 0
              {: .sp-float}
            - [triggerTarget2](#triggertarget) = ""
              {: .sp-string}
            - [fireDelay2](#firedelay) = 0
              {: .sp-float}
            - [actTarget](#acttarget) = 0
              {: .sp-int}
            - [sendOnTrigger](#sendontrigger) = TRUE
              {: .sp-bool}

## Properties

#### `listProcess` {: .sp-enum}

:   Controls how events are sent to the targets.
    
    * `LP_ALL` — Every target in the trigger list is sent the `OnTrigger` event sequentially. The [`fireDelay`](#firedelay)
      of each target is taken into account so that target `n` is fired after `sum(fireDelay0, ..., fireDelayN-1)` seconds.
    * `LP_NEXT_ONE` — The event is sent to the next target in the list.
    * `LP_RAND_ONE` — The event is sent to a random target in the list.

#### `triggerTarget` {: .sp-string}

:   The name of the VObject to send events to. Behaves like [`zCTrigger.triggerTarget`](zCTrigger.md#triggertarget).

#### `fireDelay` {: .sp-float}

:   The delay after which to fire the event to the target. Behaves like [`zCTrigger.fireDelaySec`](zCTrigger.md#firedelaysec).


#### `actTarget` {: .sp-int}

:   Unknown.

    !!! warning
        This property is only available in saved games.

#### `sendOnTrigger` {: .sp-bool}

:   Unknown.

    !!! warning
        This property is only available in saved games.
