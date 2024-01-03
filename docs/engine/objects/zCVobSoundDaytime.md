# zCVobSoundDaytime

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCVobSoundDaytime`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `12289`<br/>
    — Gothic II: `12289`<br/>
    **ZenKit Class:** `VSoundDaytime`

Emits a sound defined by a script instance in a given radius during a given time during the day. Optionally, another
sound can be defined which is played outside the defined time range (example: birds during the day and crickets at night). 

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type"><a href="../zCVobSound/">zCVobSound</a></li>
    <li class="sp-type">
        <span>zCVobSoundDaytime</span>
        <ul class="sp-list">
            <li class="sp-folder">
                <span>SoundDaytime</span>
                <ul class="sp-list">
                    <li class="sp-float"><a href="#sndStartTime">sndStartTime</a> = 0</li>
                    <li class="sp-float"><a href="#sndEndTime">sndEndTime</a> = 0</li>
                    <li class="sp-string"><a href="#sndName2">sndName2</a> = ""</li>
                </ul>
            </li>
        </ul>
    </li>
</ul>

## Properties

<a name="sndStartTime" class="t-float"></a> `sndStartTime`

:   The time of day after which the sound can be heard (`13.5` corresponds to `13:30`). Must be less
    than [`sndEndTime`](#sndEndTime).

<a name="sndEndTime" class="t-float"></a> `sndEndTime`

:   The time of day after which the sound can no longer be heard (`13.5` corresponds to `13:30`). Must be greater than
    [`sndStartTime`](#sndStartTime).

<a name="sndName2" class="t-str"></a> `sndName2`

:   The name of the script instance describing the sound to play outside the hours defined by
    [`sndStartTime`](#sndStartTime) and [`sndEndTime`](#sndEndTime). May be empty.

## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei) and the
Russian-language knowledge accumulator [gothic-library.ru](http://www.gothic-library.ru/publ/class_zcvobsounddaytime/1-1-0-702).
