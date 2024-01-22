# oCSavegameInfo

!!! abstract inline end "Quick Infos"
    **Type:** Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCSavegameInfo`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `0`<br/>
    — Gothic II: `0`<br/>
    **ZenKit Class:** `SaveMetadata`

<ul class="sp-list">
    <li class="sp-type">
        <span>oCSavegameInfo</span>
        <ul class="sp-list">
            <li class="sp-string"><a href="#Title">Title</a> = ""</li>
            <li class="sp-string"><a href="#WorldName">WorldName</a> = ""</li>
            <li class="sp-int"><a href="#TimeDay">TimeDay</a> = 0</li>
            <li class="sp-int"><a href="#TimeHour">TimeHour</a> = 0</li>
            <li class="sp-int"><a href="#TimeMinute">TimeMinute</a> = 0</li>
            <li class="sp-string"><a href="#SaveDate">SaveDate</a> = "01.01.0001 - 00:00"</li>
            <li class="sp-int"><a href="#VersionMajor">VersionMajor</a> = 0</li>
            <li class="sp-int"><a href="#VersionMinor">VersionMinor</a> = 0</li>
            <li class="sp-int"><a href="#PlayTimeSeconds">PlayTimeSeconds</a> = 0</li>
            <li class="sp-int"><a href="#VersionPoint">VersionPoint</a> = 0</li>
            <li class="sp-int"><a href="#VersionInt">VersionInt</a> = 0</li>
            <li class="sp-string"><a href="#VersionAppName">VersionAppName</a> = ""</li>
        </ul>
    </li>
</ul>

<a name="Title" class="t-str"></a> `Title`

: The name of the save-game as provided by the player.

<a name="WorldName" class="t-str"></a> `WorldName`

: The name of the world the hero was in when creating the save. For example: `"WORLD"` or `"NEWORLD"`.

<a name="TimeDay" class="t-int"></a> `TimeDay`

: The number of in-game days played in the save-game.

<a name="TimeHour" class="t-int"></a> `TimeHour`

: The hour of the in-game [day](#TimeDay) when the save-game was created (values from `0` to `24`).

<a name="TimeMinute" class="t-int"></a> `TimeMinute`

: The minute of the in-game [hour](#TimeHour) when the save-game was created (values from `0` to `59`).

<a name="SaveDate" class="t-str"></a> `SaveDate`

: The real-world date at which the save-game was created in a human-readable format. Its value conforms to this
`strftime` format string: `%d.%m.%Y - %H:%M`

<a name="VersionMajor" class="t-int"></a> `VersionMajor`

:   The major version of the save-game format. Known values for *Gothic I*: \[`1`\], known values for *Gothic II*: \[`2`\]

<a name="VersionMinor" class="t-int"></a> `VersionMinor`

:   The minor version of the save-game format. Known values for *Gothic I*: \[`87`\], known values for *Gothic II*: \[`6`\]

<a name="PlayTimeSeconds" class="t-int"></a> `PlayTimeSeconds`

:   The total number of real-time seconds played in this save-game.

<a name="VersionPoint" class="t-int"></a> `VersionPoint`

:   Unknown. Known values: \[`0`\]

    !!! warning
        This property is only available in Gothic II.

<a name="VersionInt" class="t-int"></a> `VersionInt`

:   Unknown. Known values: \[`0`\]

    !!! warning
        This property is only available in Gothic II.

<a name="VersionAppName" class="t-str"></a> `VersionAppName`

:   The name and version number of the version of Gothic used to create the save-game.

    !!! warning
        This property is only available in Gothic II.
