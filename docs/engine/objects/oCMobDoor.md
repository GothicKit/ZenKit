# oCMobDoor

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCMobDoor`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `64513`<br/>
    — Gothic II: `64513`<br/>
    **ZenKit Class:** `VDoor`

A VObject representing a door. This is also used for NPCs navigating the waynet.

<ul class="sp-list">
    <li class="sp-type"><a href="../zCVob/">zCVob</a></li>
    <li class="sp-type"><a href="../oCMOB/">oCMOB</a></li>
    <li class="sp-type"><a href="../oCMOBInter/">oCMOBInter</a></li>
    <li class="sp-type">
        <span>oCMobDoor</span>
        <ul class="sp-list">
            <li class="sp-folder">
                <span>Lockable</span>
                <ul class="sp-list">
                    <li class="sp-bool"><a href="#locked">locked</a> = FALSE</li>
                    <li class="sp-string"><a href="#keyInstance">keyInstance</a> = ""</li>
                    <li class="sp-string"><a href="#pickLockStr">pickLockStr</a> = ""</li>
                </ul>
            </li>
        </ul>
    </li>
</ul>

## Properties

<a name="locked" class="t-bool"></a> `locked`

:   Determines whether the container is locked.
    <br/>`TRUE` — The container is locked. `FALSE` — The container is not locked.

<a name="keyInstance" class="t-str"></a> `keyInstance`

:   The name of the item script instance which unlocks the container.
    !!! warning
        Only relevant if the container is locked.

<a name="pickLockStr" class="t-str"></a> `pickLockStr`

:   The combination which unlocks this container when picking the lock. Each character of the string is either `R` or
    `L` where `R` stands for "Right" and `L` stands for "Left". If empty, the lock can not be picked.
    <br />Example: `LRRLLRL`.
    !!! warning
        Only relevant if the container is locked.

## Sources

The Russian-language knowledge accumulator [gothic-library.ru](http://www.gothic-library.ru/publ/ocmobdoor/1-1-0-510).
