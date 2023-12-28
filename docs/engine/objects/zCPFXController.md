# zCPFXController

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCPFXControler`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `12289`<br/>
    — Gothic II: `52224`<br/>
    **ZenKit Class:** `VParticleEffectController`

A particle system in virtual space. Its effect can be activated and deactivated using the `OnTrigger` and `OnUntrigger`
events.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type">
        <span>zCPFXController</span>
        <ul class="sp-list">
            <li class="sp-string"><a href="#pfxName">pfxName</a> = ""</li>
            <li class="sp-bool"><a href="#killVobWhenDone">killVobWhenDone</a> = TRUE</li>
            <li class="sp-bool"><a href="#pfxStartOn">pfxStartOn</a> = TRUE</li>
        </ul>
    </li>
</ul>

## Properties

<a name="pfxName" class="t-str"></a> `pfxName`

:   The name of the particle effect. Corresponds to a `.ZEN` file containing the particle system definition.

<a name="killVobWhenDone" class="t-bool"></a> `killVobWhenDone`

:   Disable the particle effect after it runs once.

<a name="pfxStartOn" class="t-bool"></a> `pfxStartOn`

:   Whether to start the particle effect when the level is loaded.
    
    * `TRUE` — Start the particle effect when loading the world.
    * `FALSE` —  Don't start the particle effect automatically.

