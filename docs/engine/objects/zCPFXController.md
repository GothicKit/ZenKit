# zCPFXController

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCPFXControler`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `12289`<br/>
    — Gothic II: `52224`<br/>
    **ZenKit Class:** `VParticleEffectController`<br/>
    **Sources:**<br/>
    — [spacerhilfedatei.sph](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei)<br/>
    — [gothic-library.ru](http://www.gothic-library.ru/publ/zcpfxcontroler/1-1-0-518)

A particle system in virtual space. Its effect can be activated and deactivated using the `OnTrigger` and `OnUntrigger`
events.

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [*zCEffect*](zCEffect.md)
      {: .sp-class}
    - zCPFXController
      {: .sp-class}
        - [pfxName](#pfxname) = ""
          {: .sp-string}
        - [killVobWhenDone](#killvobwhendone) = TRUE
          {: .sp-bool}
        - [pfxStartOn](#pfxstarton) = TRUE
          {: .sp-bool}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [*zCEffect*](zCEffect.md)
      {: .sp-class}
    - zCPFXController
      {: .sp-class}
        - [pfxName](#pfxname) = ""
          {: .sp-string}
        - [killVobWhenDone](#killvobwhendone) = TRUE
          {: .sp-bool}
        - [pfxStartOn](#pfxstarton) = TRUE
          {: .sp-bool}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [*zCEffect*](zCEffect.md)
      {: .sp-class}
    - zCPFXController
      {: .sp-class}
        - [pfxName](#pfxname) = ""
          {: .sp-string}
        - [killVobWhenDone](#killvobwhendone) = TRUE
          {: .sp-bool}
        - [pfxStartOn](#pfxstarton) = TRUE
          {: .sp-bool}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - [*zCEffect*](zCEffect.md)
      {: .sp-class}
    - zCPFXController
      {: .sp-class}
        - [pfxName](#pfxname) = ""
          {: .sp-string}
        - [killVobWhenDone](#killvobwhendone) = TRUE
          {: .sp-bool}
        - [pfxStartOn](#pfxstarton) = TRUE
          {: .sp-bool}

## Properties

#### `pfxName` {: .sp-string}

:   The name of the particle effect. Corresponds to a `.ZEN` file containing the particle system definition.

#### `killVobWhenDone` {: .sp-bool}

:   Disable the particle effect after it runs once.

#### `pfxStartOn` {: .sp-bool}

:   Whether to start the particle effect when the level is loaded.
    
    * `TRUE` — Start the particle effect when loading the world.
    * `FALSE` —  Don't start the particle effect automatically.
