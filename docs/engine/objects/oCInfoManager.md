# oCInfoManager

!!! abstract inline end "Quick Infos"
    **Type:** Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCInfoManager`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `0`<br/>
    — Gothic II: `0`<br/>
    **ZenKit Class:** *—*

<ul class="sp-list">
    <li class="sp-type">
        <span>oCSavegameInfo</span>
        <ul class="sp-list">
            <li class="sp-int"><a href="#NumOfEntries">NumOfEntries</a> = 0</li>
            <li class="sp-type"><a href="../oCInfo/">(inline) [oCInfo]</a><sup>1</sup></li>
        </ul>
    </li>
</ul>

<a name="NumOfEntries" class="t-int"></a> `NumOfEntries`

: The number of saved [`oCInfo`](oCInfo.md) objects.

[^1]: "Inline" means, that these objects are not wrapped in an archive object. Rather, their contents are simply
      all written to the parent object directly without encapsulation.
