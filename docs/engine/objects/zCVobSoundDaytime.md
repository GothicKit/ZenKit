# zCVobSoundDaytime

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCVobSoundDaytime`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `12289`<br/>
    — Gothic II: `12289`<br/>
    **ZenKit Class:** `VSoundDaytime`<br/>
    **Sources:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)<br/>
    — [gothic-library.ru](http://www.gothic-library.ru/publ/class_zcvobsounddaytime/1-1-0-702)

Emits a sound defined by a script instance in a given radius during a given time during the day. Optionally, another
sound can be defined which is played outside the defined time range (example: birds during the day and crickets at night). 

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - [zCVobSound](zCVobSound.md)
      {: .sp-type}
    - zCVobSoundDaytime
      {: .sp-type}
        - SoundDaytime
          {: .sp-folder}
            - [sndStartTime](#sndstarttime) = 0
              {: .sp-float}
            - [sndEndTime](#sndendtime) = 0
              {: .sp-float}
            - [sndName2](#sndname2) = ""
              {: .sp-string}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - [zCVobSound](zCVobSound.md)
      {: .sp-type}
    - zCVobSoundDaytime
      {: .sp-type}
        - SoundDaytime
          {: .sp-folder}
            - [sndStartTime](#sndstarttime) = 0
              {: .sp-float}
            - [sndEndTime](#sndendtime) = 0
              {: .sp-float}
            - [sndName2](#sndname2) = ""
              {: .sp-string}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - [zCVobSound](zCVobSound.md)
      {: .sp-type}
    - zCVobSoundDaytime
      {: .sp-type}
        - SoundDaytime
          {: .sp-folder}
            - [sndStartTime](#sndstarttime) = 0
              {: .sp-float}
            - [sndEndTime](#sndendtime) = 0
              {: .sp-float}
            - [sndName2](#sndname2) = ""
              {: .sp-string}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-type}
    - [zCVobSound](zCVobSound.md)
      {: .sp-type}
    - zCVobSoundDaytime
      {: .sp-type}
        - SoundDaytime
          {: .sp-folder}
            - [sndStartTime](#sndstarttime) = 0
              {: .sp-float}
            - [sndEndTime](#sndendtime) = 0
              {: .sp-float}
            - [sndName2](#sndname2) = ""
              {: .sp-string}

## Properties

#### `sndStartTime` {: .sp-float}

:   The time of day after which the sound can be heard (`13.5` corresponds to `13:30`). Must be less
    than [`sndEndTime`](#sndEndTime).

#### `sndEndTime` {: .sp-float}

:   The time of day after which the sound can no longer be heard (`13.5` corresponds to `13:30`). Must be greater than
    [`sndStartTime`](#sndStartTime).

#### `sndName2` {: .sp-string}

:   The name of the script instance describing the sound to play outside the hours defined by
    [`sndStartTime`](#sndStartTime) and [`sndEndTime`](#sndEndTime). May be empty.
