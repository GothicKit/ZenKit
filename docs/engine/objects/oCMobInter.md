# oCMobInter

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCMobInter`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `35585`<br/>
    — Gothic II: `35585`<br/>
    **ZenKit Class:** `VInteractiveObject`<br/>
    **Sources:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)<br/>
    — [gothic-library.ru](http://www.gothic-library.ru/publ/class_ocmobinter/1-1-0-504)


Interactive objects can be interacted with by NPCs and the player. Every time the object is interacted with, an
`OnTrigger` event is sent to the [`triggerTarget`](#triggerTarget) object.

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [oCVob](oCVob.md)
      {: .sp-class}
    - [oCMOB](oCMOB.md)
      {: .sp-class}
    - [oCMobInter](oCMobInter.md)
      {: .sp-class}
        - [stateNum](#statenum) = 0
          {: .sp-int}
        - [triggerTarget](#triggertarget) = ""
          {: .sp-string}
        - [useWithItem](#usewithitem) = ""
          {: .sp-string}
        - [conditionFunc](#conditionfunc) = ""
          {: .sp-string}
        - [onStateFunc](#onstatefunc) = ""
          {: .sp-string}
        - [rewind](#rewind) = FALSE
          {: .sp-bool}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [oCVob](oCVob.md)
      {: .sp-class}
    - [oCMOB](oCMOB.md)
      {: .sp-class}
    - [oCMobInter](oCMobInter.md)
      {: .sp-class}
        - [stateNum](#statenum) = 0
          {: .sp-int}
        - [triggerTarget](#triggertarget) = ""
          {: .sp-string}
        - [useWithItem](#usewithitem) = ""
          {: .sp-string}
        - [conditionFunc](#conditionfunc) = ""
          {: .sp-string}
        - [onStateFunc](#onstatefunc) = ""
          {: .sp-string}
        - [rewind](#rewind) = FALSE
          {: .sp-bool}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [oCVob](oCVob.md)
      {: .sp-class}
    - [oCMOB](oCMOB.md)
      {: .sp-class}
    - [oCMobInter](oCMobInter.md)
      {: .sp-class}
        - [stateNum](#statenum) = 0
          {: .sp-int}
        - [triggerTarget](#triggertarget) = ""
          {: .sp-string}
        - [useWithItem](#usewithitem) = ""
          {: .sp-string}
        - [conditionFunc](#conditionfunc) = ""
          {: .sp-string}
        - [onStateFunc](#onstatefunc) = ""
          {: .sp-string}
        - [rewind](#rewind) = FALSE
          {: .sp-bool}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [oCVob](oCVob.md)
      {: .sp-class}
    - [oCMOB](oCMOB.md)
      {: .sp-class}
    - [oCMobInter](oCMobInter.md)
      {: .sp-class}
        - [stateNum](#statenum) = 0
          {: .sp-int}
        - [triggerTarget](#triggertarget) = ""
          {: .sp-string}
        - [useWithItem](#usewithitem) = ""
          {: .sp-string}
        - [conditionFunc](#conditionfunc) = ""
          {: .sp-string}
        - [onStateFunc](#onstatefunc) = ""
          {: .sp-string}
        - [rewind](#rewind) = FALSE
          {: .sp-bool}

## Properties

#### `stateNum` {: .sp-int}

:   Unknown.

#### `triggerTarget` {: .sp-string}

:   The name of the VObject(s) to send an `OnTrigger` event to when this object is interacted with. Controlled by
    the [`rewind`](#rewind) property.

#### `useWithItem` {: .sp-string}

:   The name of the item which the player or NPC must have in their inventory in order to interact with the object.
    Corresponds to the name of the item instance in the scripts.

#### `conditionFunc` {: .sp-string}

:   The name of a script function which, when called, determines whether the object can be interacted with. The script
    function returns an int which is either `0`, if the object should be disabled and `1` if it should be enabled.

#### `onStateFunc` {: .sp-string}

:   The name of a script function to be called when the object is being used.

#### `rewind` {: .sp-bool}

:   Determines whether an `OnTrigger` event should be sent every time the object is interacted with or only the first time.
    <br />`TRUE` — Send `OnTrigger` events multiple times. `FALSE` — Only send the event once.
