# zCTriggerList

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCTriggerList`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `59776`<br/>
    — Gothic II: `5505`<br/>
    **ZenKit Class:** `VTriggerList`

A special trigger which emits the `OnTrigger` events emitted by the [`zCTrigger`](zCTrigger.md) to multiple targets.
How the events are sent can be configured through [`listProcess`](#listProcess). The
[`triggerTarget`](zCTrigger.md#triggerTarget) and [`fireDelaySec`](zCTrigger.md#fireDelaySec) properties of
[`zCTrigger`](zCTrigger.md) should be ignored.

`OnUntrigger` events sent to the trigger list are propagated to all targets in the list.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type"><a href="../zCTrigger/">zCTrigger</a></li>
    <li class="sp-type">
        <span>zCTriggerList</span>
        <ul class="sp-list">
            <li class="sp-folder">
                <span>TriggerList</span>
                <ul class="sp-list">
                    <li class="sp-enum"><a href="#listProcess">listProcess</a> = LP_ALL</li>
                    <li class="sp-string"><a href="#triggerTarget">triggerTarget0</a> = ""</li>
                    <li class="sp-float"><a href="#fireDelay">fireDelay0</a> = 0</li>
                    <li class="sp-string"><a href="#triggerTarget">triggerTarget1</a> = ""</li>
                    <li class="sp-float"><a href="#fireDelay">fireDelay1</a> = 0</li>
                    <li class="sp-string"><a href="#triggerTarget">triggerTarget2</a> = ""</li>
                    <li class="sp-float"><a href="#fireDelay">fireDelay2</a> = 0</li>
                </ul>
            </li>
        </ul>
    </li>
</ul>

## Properties

<a name="listProcess" class="t-enum"></a> `listProcess`

:   Controls how events are sent to the targets.
    
    * `LP_ALL` — Every target in the trigger list is sent the `OnTrigger` event sequentially. The [`fireDelay`](#fireDelay)
      of each target is taken into account so that target `n` is fired after `sum(fireDelay0, ..., fireDelayN-1)` seconds.
    * `LP_NEXT_ONE` — The event is sent to the next target in the list.
    * `LP_RAND_ONE` — The event is sent to a random target in the list.

<a name="triggerTarget" class="t-string"></a> `triggerTarget`

:   The name of the VObject to send events to. Behaves like [`zCTrigger.triggerTarget`](zCTrigger.md#triggerTarget).

<a name="fireDelay" class="t-float"></a> `fireDelay`

:   The delay after which to fire the event to the target. Behaves like [`zCTrigger.fireDelaySec`](zCTrigger.md#fireDelaySec).


## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei) and the
Russian-language knowledge accumulator [gothic-library.ru](http://www.gothic-library.ru/publ/class_zctriggerlist/1-1-0-534).
