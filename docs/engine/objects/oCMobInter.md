# oCMobInter

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCMobInter`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `35585`<br/>
    — Gothic II: `35585`<br/>
    **ZenKit Class:** `VInteractiveObject`

Interactive objects can be interacted with by NPCs and the player. Every time the object is interacted with, an
`OnTrigger` event is sent to the [`triggerTarget`](#triggerTarget) object.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type"><a href="../oCMOB/">oCMOB</a></li>
    <li class="sp-type">
        <span>oCMobInter</span>
        <ul class="sp-list">
            <li class="sp-int"><a href="#stateNum">stateNum</a> = 0</li>
            <li class="sp-string"><a href="#triggerTarget">triggerTarget</a> = ""</li>
            <li class="sp-string"><a href="#useWithItem">useWithItem</a> = ""</li>
            <li class="sp-string"><a href="#conditionFunc">conditionFunc</a> = ""</li>
            <li class="sp-string"><a href="#onStateFunc">onStateFunc</a> = ""</li>
            <li class="sp-bool"><a href="#rewind">rewind</a> = FALSE</li>
        </ul>
    </li>
</ul>

## Properties

<a name="stateNum" class="t-int"></a> `stateNum`

:   Unknown.

<a name="triggerTarget" class="t-str"></a> `triggerTarget`

:   The name of the VObject(s) to send an `OnTrigger` event to when this object is interacted with. Controlled by
    the [`rewind`](#rewind) property.

<a name="useWithItem" class="t-str"></a> `useWithItem`

:   The name of the item which the player or NPC must have in their inventory in order to interact with the object.
    Corresponds to the name of the item instance in the scripts.

<a name="conditionFunc" class="t-str"></a> `conditionFunc`

:   The name of a script function which, when called, determines whether the object can be interacted with. The script
    function returns an int which is either `0`, if the object should be disabled and `1` if it should be enabled.

<a name="onStateFunc" class="t-str"></a> `onStateFunc`

:   The name of a script function to be called when the object is being used.

<a name="rewind" class="t-bool"></a> `rewind`

:   Determines whether an `OnTrigger` event should be sent every time the object is interacted with or only the first time.
    <br />`TRUE` — Send `OnTrigger` events multiple times. `FALSE` — Only send the event once.

## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei) and the
Russian-language knowledge accumulator [gothic-library.ru](http://www.gothic-library.ru/publ/class_ocmobinter/1-1-0-504).
