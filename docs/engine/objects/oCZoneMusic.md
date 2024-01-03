# oCZoneMusic

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCZoneMusic`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `0`<br/>
    — Gothic II: `0`<br/>
    **ZenKit Class:** `VZoneMusic`

A VObject which defines the music to be played within its bounding box. Music zones may overlap; if they do, their
[`priority`](#priority) property determines which music zone should be used. The camera object is used to test which
music zone applies. If no music zone applies the music defined in the optional [`zCZoneMusicDefault`](#zCZoneMusicDefault)
is used.

The name of the music theme to be played is defined by the [`vobName`](zCVob.md#vobname).

!!! note
    If the suffix `_NCI` or `_NCO` is added to the end of the zone name, it snows instead of rains in the zone.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type">
        <span>oCZoneMusic</span>
        <ul class="sp-list">
            <li class="sp-bool"><a href="#enabled">enabled</a> = TRUE</li>
            <li class="sp-int"><a href="#priority">priority</a> = 0</li>
            <li class="sp-bool"><a href="#ellipsoid">ellipsoid</a> = FALSE</li>
            <li class="sp-float"><a href="#reverbLevel">reverbLevel</a> = 0</li>
            <li class="sp-float"><a href="#volumeLevel">volumeLevel</a> = 100</li>
            <li class="sp-bool"><a href="#loop">loop</a> = TRUE</li>
        </ul>
    </li>
</ul>

## Properties

<a name="enabled" class="t-bool"></a> `enabled`

:   Whether the music zone is enabled when loading the level. When disabled, no music will be played in the zone.
    <br/>`TRUE` — Enable the music zone when loading the level. `FALSE` — The not enable the zone.

<a name="priority" class="t-int"></a> `priority`

:   The priority of the music zone if two zones overlap. A higher number means a higher priority. `0` is the lowest
    possible priority.

    !!! warning
        Only relevant if two `zCZoneMusic` zones overlap.

<a name="ellipsoid" class="t-bool"></a> `ellipsoid`

:   Determines the shape of the zone.
    
    * `TRUE` — The zone forms an ellipsoid within the bounding box.
    * `FALSE` — The zone exactly mirrors the bounding box.

<a name="reverbLevel" class="t-float"></a> `reverbLevel`

:   Unclear.

<a name="volumeLevel" class="t-float"></a> `volumeLevel`

:   Unclear.

<a name="loop" class="t-bool"></a> `loop`

:   Unclear.

## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei) and the
Russian-language knowledge accumulator [gothic-library.ru](http://www.gothic-library.ru/publ/class_oczonemusic/1-1-0-700).
