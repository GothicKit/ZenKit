# zCEarthquake

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCEarthquake`<br/>
    **Version Identifiers:**<br />
    — Gothic I: *Unused*<br/>
    — Gothic II: `52224`<br/>
    **ZenKit Class:** `VEarthquake`

A VObject used to create an earthquake effect when it receives an `OnTrigger` event. An earthquake is represented
by a shaking camera.

!!! warning
    This VObject is only available in Gothic II.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type">
        <span>zCEarthquake</span>
        <ul class="sp-list">
            <li class="sp-folder">
                <span>Earthquake</span>
                <ul class="sp-list">
                    <li class="sp-float"><a href="#radius">radius</a> = 200</li>
                    <li class="sp-float"><a href="#timeSec">timeSec</a> = 5</li>
                    <li class="sp-vec"><a href="#amplitudeCM">amplitudeCM</a> = 0 0 0</li>
                </ul>
            </li>
        </ul>
    </li>
</ul>

## Properties

<a name="radius" class="t-float"></a> `radius`

:   The radius of the earthquake effect

<a name="timeSec" class="t-float"></a> `timeSec`

:   The duration of the earthquake effect in seconds.

<a name="amplitudeCM" class="t-float"></a> `amplitudeCM`

:   The amplitude of the earthquake effect.

## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei) and the
Russian-language knowledge accumulator [gothic-library.ru](http://www.gothic-library.ru/publ/zcearthquake/1-1-0-516).
