# zCVobAnimate

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCVobAnimate`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `12289`<br/>
    — Gothic II: `52224`<br/>
    **ZenKit Class:** `VAnimate`

A VObject used to create animated models. The visual of such objects can either be a
[morph mesh](../../engine/formats/morph-mesh.md) or a model with a skeletal animation
(i.e. a [model script](../../engine/formats/model-script.md)). The animation of these objects can be started
and stopped using `OnTrigger` and `OnUntrigger` events.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type">
        <span>zCVobLight</span>
        <ul class="sp-list">
            <li class="sp-bool"><a  href="#startOn">startOn</a> = TRUE</li>
        </ul>
    </li>
</ul>

## Properties

<a name="startOn" class="t-bool"></a> `startOn`

:   Whether to start this animation when loading the level.
    
    * `TRUE` — Immediately start the animation when the level is loaded.
    * `FALSE` — Don't automatically start the animation.

## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei) and the
Russian-language knowledge accumulator [gothic-library.ru](http://www.gothic-library.ru/publ/zcvobanimate/1-1-0-523).
