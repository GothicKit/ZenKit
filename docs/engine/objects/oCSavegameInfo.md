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

=== "Gothic 1"

    - oCSavegameInfo
      {: .sp-type}
        - [Title](#Title) = ""
          {: .sp-string}
        - [WorldName](#WorldName) = ""
          {: .sp-string}
        - [TimeDay](#TimeDay) = 0
          {: .sp-int}
        - [TimeHour](#TimeHour) = 0
          {: .sp-int}
        - [TimeMinute](#TimeMinute) = 0
          {: .sp-int}
        - [SaveDate](#SaveDate) = "01.01.0001 - 00:00"
          {: .sp-string}
        - [VersionMajor](#VersionMajor) = 0
          {: .sp-int}
        - [VersionMinor](#VersionMinor) = 0
          {: .sp-int}
        - [PlayTimeSeconds](#PlayTimeSeconds) = 0
          {: .sp-int}
        - [VersionPoint](#VersionPoint) = 0
          {: .sp-int}
        - [VersionInt](#VersionInt) = 0
          {: .sp-int}
        - [VersionAppName](#VersionAppName) = ""
          {: .sp-string}

=== "Gothic 2"

    - oCSavegameInfo
      {: .sp-type}
        - [Title](#Title) = ""
          {: .sp-string}
        - [WorldName](#WorldName) = ""
          {: .sp-string}
        - [TimeDay](#TimeDay) = 0
          {: .sp-int}
        - [TimeHour](#TimeHour) = 0
          {: .sp-int}
        - [TimeMinute](#TimeMinute) = 0
          {: .sp-int}
        - [SaveDate](#SaveDate) = "01.01.0001 - 00:00"
          {: .sp-string}
        - [VersionMajor](#VersionMajor) = 0
          {: .sp-int}
        - [VersionMinor](#VersionMinor) = 0
          {: .sp-int}
        - [PlayTimeSeconds](#PlayTimeSeconds) = 0
          {: .sp-int}
        - [VersionPoint](#VersionPoint) = 0
          {: .sp-int}
        - [VersionInt](#VersionInt) = 0
          {: .sp-int}
        - [VersionAppName](#VersionAppName) = ""
          {: .sp-string}

=== "Gothic 1 (Save)"

    - oCSavegameInfo
      {: .sp-type}
        - [Title](#Title) = ""
          {: .sp-string}
        - [WorldName](#WorldName) = ""
          {: .sp-string}
        - [TimeDay](#TimeDay) = 0
          {: .sp-int}
        - [TimeHour](#TimeHour) = 0
          {: .sp-int}
        - [TimeMinute](#TimeMinute) = 0
          {: .sp-int}
        - [SaveDate](#SaveDate) = "01.01.0001 - 00:00"
          {: .sp-string}
        - [VersionMajor](#VersionMajor) = 0
          {: .sp-int}
        - [VersionMinor](#VersionMinor) = 0
          {: .sp-int}
        - [PlayTimeSeconds](#PlayTimeSeconds) = 0
          {: .sp-int}
        - [VersionPoint](#VersionPoint) = 0
          {: .sp-int}
        - [VersionInt](#VersionInt) = 0
          {: .sp-int}
        - [VersionAppName](#VersionAppName) = ""
          {: .sp-string}

=== "Gothic 2 (Save)"

    - oCSavegameInfo
      {: .sp-type}
        - [Title](#Title) = ""
          {: .sp-string}
        - [WorldName](#WorldName) = ""
          {: .sp-string}
        - [TimeDay](#TimeDay) = 0
          {: .sp-int}
        - [TimeHour](#TimeHour) = 0
          {: .sp-int}
        - [TimeMinute](#TimeMinute) = 0
          {: .sp-int}
        - [SaveDate](#SaveDate) = "01.01.0001 - 00:00"
          {: .sp-string}
        - [VersionMajor](#VersionMajor) = 0
          {: .sp-int}
        - [VersionMinor](#VersionMinor) = 0
          {: .sp-int}
        - [PlayTimeSeconds](#PlayTimeSeconds) = 0
          {: .sp-int}
        - [VersionPoint](#VersionPoint) = 0
          {: .sp-int}
        - [VersionInt](#VersionInt) = 0
          {: .sp-int}
        - [VersionAppName](#VersionAppName) = ""
          {: .sp-string}

## Properties

#### `Title` {: .sp-string}

:   The name of the save-game as provided by the player.

#### `WorldName` {: .sp-string}

:   The name of the world the hero was in when creating the save. For example: `"WORLD"` or `"NEWORLD"`.

#### `TimeDay` {: .sp-int}

:   The number of in-game days played in the save-game.

#### `TimeHour` {: .sp-int}

:   The hour of the in-game [day](#TimeDay) when the save-game was created (values from `0` to `24`).

#### `TimeMinute` {: .sp-int}

:   The minute of the in-game [hour](#TimeHour) when the save-game was created (values from `0` to `59`).

#### `SaveDate` {: .sp-string}

:   The real-world date at which the save-game was created in a human-readable format. Its value conforms to this
    `strftime` format string: `%d.%m.%Y - %H:%M`

#### `VersionMajor` {: .sp-int}

:   The major version of the save-game format. Known values for *Gothic I*: \[`1`\], known values for *Gothic II*: \[`2`\]

#### `VersionMinor` {: .sp-int}

:   The minor version of the save-game format. Known values for *Gothic I*: \[`87`\], known values for *Gothic II*: \[`6`\]

#### `PlayTimeSeconds` {: .sp-int}

:   The total number of real-time seconds played in this save-game.

#### `VersionPoint` {: .sp-int}

:   Unknown. Known values: \[`0`\]

    !!! warning
        This property is only available in Gothic II.

#### `VersionInt` {: .sp-int}

:   Unknown. Known values: \[`0`\]

    !!! warning
        This property is only available in Gothic II.

#### `VersionAppName` {: .sp-string}

:   The name and version number of the version of Gothic used to create the save-game.

    !!! warning
        This property is only available in Gothic II.
