# oCMOB

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCMOB`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `47105`<br/>
    — Gothic II: `47105`<br/>
    **ZenKit Class:** `VMovableObject`

A VObject which can optionally be moved and/or carried.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type">
        <span>oCMOB</span>
        <ul class="sp-list">
            <li class="sp-folder">
                <span>MOB</span>
                <ul class="sp-list">
                    <li class="sp-string"><a href="#focusName">focusName</a> = ""</li>
                    <li class="sp-int"><a href="#hitpoints">hitpoints</a> = 0</li>
                    <li class="sp-int"><a href="#damage">damage</a> = 0</li>
                    <li class="sp-bool"><a href="#moveable">moveable</a> = FALSE</li>
                    <li class="sp-bool"><a href="#takeable">takeable</a> = FALSE</li>
                    <li class="sp-bool"><a href="#focusOverride">focusOverride</a> = FALSE</li>
                    <li class="sp-enum"><a href="#soundMaterial">soundMaterial</a> = WOOD</li>
                    <li class="sp-string"><a href="#visualDestroyed">visualDestroyed</a> = ""</li>
                    <li class="sp-string"><a href="#owner">owner</a> = ""</li>
                    <li class="sp-string"><a href="#ownerGuild">ownerGuild</a> = ""</li>
                </ul>
            </li>
            <li class="sp-bool"><a href="#isDestroyed">isDestroyed</a> = ""</li>
        </ul>
    </li>
</ul>

## Properties

<a name="focusName" class="t-str"></a> `focusName`

:   The name of the object as seen in-game when focusing it.

<a name="hitpoints" class="t-int"></a> `hitpoints`

:   The number of hitpoints of the object.

<a name="damage" class="t-int"></a> `damage`

:   The damage dealt by the object when being used.

<a name="moveable" class="t-bool"></a> `moveable`

:   Whether the object can be moved by the player.
    <br />`TRUE` — The object can be moved. `FALSE` — The object can not be moved.

<a name="takeable" class="t-bool"></a> `takeable`

:   Whether the object can be carried by the player.
    <br />`TRUE` — The object can be carried. `FALSE` — The object can not be carried.

<a name="focusOverride" class="t-bool"></a> `focusOverride`

:   Unknown.

<a name="soundMaterial" class="t-enum"></a> `soundMaterial`

:   The sound to play when the object is hit.

    * `WOOD` — Wood.
    * `STONE` — Stone.
    * `METAL` — Metal.
    * `LEATHER` — Leather.
    * `CLAY` — Clay.
    * `GLAS` — Glass.

<a name="visualDestroyed" class="t-str"></a> `visualDestroyed`

:   The name of the visual to use when the object is destroyed. Behaves like [zCVob.visual](zCVob.md#visual).

<a name="owner" class="t-str"></a> `owner`

:   The name of the script instance of the NPC who is the owner of this object.

<a name="ownerGuild" class="t-str"></a> `ownerGuild`

:   The name of the guild this object belongs to. This name corresponds to the script constant for
    each guild (i.e. `GIL_BAU`), found in `Constants.d`.

<a name="isDestroyed" class="t-bool"></a> `isDestroyed`

:   Unknown.

## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei) and the
Russian-language knowledge accumulator [gothic-library.ru](http://www.gothic-library.ru/publ/ocmob_class/1-1-0-503).
