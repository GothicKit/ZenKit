# zCTouchDamage

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCTouchDamage`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `36865`<br/>
    — Gothic II: `36865`<br/>
    **ZenKit Class:** `VTouchDamage`

A VObject which damages other VObjects colliding with it.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type">
        <span>zCTouchDamage</span>
        <ul class="sp-list">
            <li class="sp-folder">
                <span>TouchDamage</span>
                <ul class="sp-list">
                    <li class="sp-float"><a href="#damage">damage</a> = 0</li>
                    <li class="sp-folder">
                        <span>DamageType</span>
                        <ul class="sp-list">
                            <li class="sp-bool"><a href="#Barrier">Barrier</a> = TRUE</li>
                            <li class="sp-bool"><a href="#Blunt">Blunt</a> = FALSE</li>
                            <li class="sp-bool"><a href="#Edge">Edge</a> = FALSE</li>
                            <li class="sp-bool"><a href="#Fire">Fire</a> = FALSE</li>
                            <li class="sp-bool"><a href="#Fly">Fly</a> = FALSE</li>
                            <li class="sp-bool"><a href="#Magic">Magic</a> = FALSE</li>
                            <li class="sp-bool"><a href="#Point">Point</a> = FALSE</li>
                            <li class="sp-bool"><a href="#Fall">Fall</a> = FALSE</li>
                        </ul>
                    </li>
                    <li class="sp-float"><a href="#damageRepeatDelaySec">damageRepeatDelaySec</a> = 0</li>
                    <li class="sp-float"><a href="#damageVolDownScale">damageVolDownScale</a> = 0</li>
                    <li class="sp-enum"><a href="#damageCollType">damageCollType</a> = BOX</li>
                </ul>
            </li>
        </ul>
    </li>
</ul>

## Properties

<a name="damage" class="t-float"></a> `damage`

:   The amount of damage being dealt.

<a name="Barrier" class="t-bool"></a> `Barrier`

:   `TRUE` — Deal barrier damage. `FALSE` — Do not deal barrier damage.

<a name="Blunt" class="t-bool"></a> `Blunt`

:   `TRUE` — Deal blunt damage (blunt weapons). `FALSE` — Do not deal blunt damage.

<a name="Edge" class="t-bool"></a> `Edge`

:   `TRUE` — Deal edge damage (sharp weapons). `FALSE` — Do not deal edge damage.

<a name="Fire" class="t-bool"></a> `Fire`

:   `TRUE` — Deal fire damage. `FALSE` — Do not deal fire damage.

<a name="Fly" class="t-bool"></a> `Fly`

:   `TRUE` — Deal knockout damage. `FALSE` — Do not deal knockout damage.

<a name="Magic" class="t-bool"></a> `Magic`

:   `TRUE` — Deal magic damage. `FALSE` — Do not deal magic damage.

<a name="Point" class="t-bool"></a> `Point`

:   `TRUE` — Deal point damage (bows and crossbows). `FALSE` — Do not deal point damage.

<a name="Fall" class="t-bool"></a> `Fall`

:   `TRUE` — Deal fall damage. `FALSE` — Do not deal fall damage.


<a name="damageRepeatDelaySec" class="t-float"></a> `damageRepeatDelaySec`

:   The delay between damage ticks when applying continuous damage. If set to `0`, only deals the damage once per
    collision. If set to a value greater than `0`, deals damage every `damageRepeatDelaySec` seconds.


<a name="damageVolDownScale" class="t-float"></a> `damageVolDownScale`

:   A scale value used in conjunction with [`damageCollType`](#damageCollType). Its function depends on the type of
    collision detection used.


<a name="damageCollType" class="t-enum"></a> `damageCollType`

:   The type of collision detection to use for damage calculations.

    * `NONE` — Disable collision detection and thus damage application.
    * `BOX` — Scale the [`bbox3dws`](zCVob.md#bbox3dws) of the `zCTouchDamage` object by
      [`damageVolDownScale`](#damageVolDownScale) and use this new bounding box to calculate
      collisions with other VObjects
    * `POINT` — Scale the [`bbox3dws`](zCVob.md#bbox3dws) of the colliding object by
      [`damageVolDownScale`](#damageVolDownScale) and only deal damage if it contains the center
      point of the `zCTouchDamage` object. If the [`visual`](zCVob.md#visual) of the `zCTouchDamage`
      object is a particle effect, test against every particle instead.

## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei) and the
Russian-language knowledge accumulator [gothic-library.ru](http://www.gothic-library.ru/publ/octouchdamage/1-1-0-522#damage).