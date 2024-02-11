# zCCodeMaster

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCCodeMaster`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `0`<br/>
    — Gothic II: `0`<br/>
    **ZenKit Class:** `zCCodeMaster`

`zCCodeMaster` VObjects keep a list of 'slave' VObjects and keep track of events received by them. If the master
receives an `OnTrigger` event from a slave, it remembers that it did. After it has receives an `OnTrigger` message
from all slaves, it emits an `OnTrigger` event to the [`triggerTarget`](#triggerTarget). Receiving or sending an
`OnTrigger` event is also referred to as an 'activation' for this purpose.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type">
        <span>zCCodeMaster</span>
        <ul class="sp-list">
            <li class="sp-string"><a href="#triggerTarget">triggerTarget</a> = ""</li>
            <li class="sp-folder">
                <span>CodeMaster</span>
                <ul class="sp-list">
                    <li class="sp-bool"><a href="#orderRelevant">orderRelevant</a> = TRUE</li>
                    <li class="sp-folder">
                        <span>OrderRelevant</span>
                        <ul class="sp-list">
                            <li class="sp-bool"><a href="#firstFalseIsFailure">firstFalseIsFailure</a> = TRUE</li>
                            <li class="sp-string"><a href="#triggerTargetFailure">triggerTargetFailure</a> = ""</li>
                        </ul>
                    </li>
                    <li class="sp-folder">
                        <span>OrderNotRelevant</span>
                        <ul class="sp-list">
                            <li class="sp-bool"><a href="#untriggerCancels">untriggerCancels</a> = FALSE</li>
                        </ul>
                    </li>
                    <li class="sp-string"><a href="#slaveVobName">slaveVobName0</a> = ""</li>
                    <li class="sp-string"><a href="#slaveVobName">slaveVobName1</a> = ""</li>
                    <li class="sp-string"><a href="#slaveVobName">slaveVobName2</a> = ""</li>
                </ul>
            </li>
        </ul>
    </li>
</ul>

## Properties

### `triggerTarget` {: .sp-string}

:   The name of the VObject to send an `OnTrigger` event to after all slaves have fired in the correct order.

### `orderRelevant` {: .sp-bool}

:   Controls whether the master should keep track of the order it receives messages from its slaves.
    
    * `TRUE` — Only send an `OnTrigger` event to the [`triggerTarget`](#triggerTarget) if the slaves have fired in
      the exact order they are configured here. Also counts multiple activations.
    * `FALSE` — Send an `OnTrigger` event to the [`triggerTarget`](#triggerTarget) after all slaves have fired at least
      once, regardless of order.

### `firstFalseIsFailure` {: .sp-bool}

:   Controls when the slave activation sequence is considered to be incorrect. After the sequence is marked as
    incorrect an `OnTrigger` event is sent to [`triggerTargetFailure`](#triggerTargetFailure).
    
    * `TRUE` — The sequence is considered to be incorrect as soon as one slave is activated out-of-order.
    * `FALSE` — The sequence is considered to be incorrect only after all slaves have been activated at least once and
      they were activated out-of-order.

    !!! warning
        Only relevant if [`orderRelevant`](#orderRelevant) is set to `TRUE`.

### `triggerTargetFailure` {: .sp-string}

:   The name of the VObject to emit an `OnTrigger` event to if the activation sequence fails. This is controlled by
    [`firstFalseIsFailure`](#firstFalseIsFailure).

    !!! warning
        Only relevant if [`orderRelevant`](#orderRelevant) is set to `TRUE`.

### `untriggerCancels` {: .sp-bool}

:   Controls whether slaves can emit `OnUntrigger` events to the master VObject to mark themselves as deactivated.

    * `TRUE` — When the master receives an `OnUntrigger` event from the slave, it marks it as not having been activated.
    * `FALSE` — The master ignore `OnUntrigger` events from its slaves.
    
    !!! warning
        Only relevant if [`orderRelevant`](#orderRelevant) is set to `FALSE`.


### `slaveVobName` {: .sp-string}

:   The name of a slave VObject.

## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei) and the
Russian-language knowledge accumulator [gothic-library.ru](http://www.gothic-library.ru/publ/zccodemaster/1-1-0-526).
