# `oCInfo`

!!! abstract inline end "Quick Infos"
    **Type:** Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCInfo`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `0`<br/>
    — Gothic II: `0`<br/>
    **ZenKit Class:** `—` 

=== "Gothic 1"
    - oCInfo
      {: .sp-class}
        - &lt;empty&gt;
          {: .sp-none}

=== "Gothic 2"
    - oCInfo
      {: .sp-class}
        - &lt;empty&gt;
          {: .sp-none}

=== "Gothic 1 (Save)"
    - oCInfo
      {: .sp-class}
        - [Told](#told) = FALSE
          {: .sp-bool}
        - [InstName](#instname) = 
          {: .sp-string}

=== "Gothic 2 (Save)"
    - oCInfo
      {: .sp-class}
        - [Told](#told) = FALSE
          {: .sp-bool}
        - [InstName](#instname) = 
          {: .sp-string}

## Properties

#### `Told` {: .sp-bool}

:   Whether the hero has already been told this info.

    * `False` — The info has not yet been told.
    * `True` — The info has already been told.
    !!! warning
        This property is only available in saved games.

#### `InstName` {: .sp-string}

:   The name of the script instance representing this info.

    !!! warning
        This property is only available in saved games.

