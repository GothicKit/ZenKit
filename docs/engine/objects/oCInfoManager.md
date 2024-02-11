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

=== "Gothic 1"

    - oCInfoManager
      {: .sp-class}
        - &lt;empty&gt;
          {: .sp-none}

=== "Gothic 2"

    - oCInfoManager
      {: .sp-class}
        - &lt;empty&gt;
          {: .sp-none}

=== "Gothic 1 (Save)"

    - oCInfoManager
      {: .sp-class}
        - [NumOfEntries](#numofentries) = 0
          {: .sp-int}
        - (inline)[^1] [oCInfo](oCInfo.md) ...
          {: .sp-class}

=== "Gothic 2 (Save)"

    - oCInfoManager
      {: .sp-class}
        - [NumOfEntries](#numofentries) = 0
          {: .sp-int}
        - (inline)[^1] [oCInfo](oCInfo.md) ...
          {: .sp-class}

## Properties

#### `NumOfEntries` {: .sp-int}

:   The number of saved [`oCInfo`](oCInfo.md) objects.

    !!! warning
        This property is only available in saved games.

[^1]: "Inline" means, that these objects are not wrapped in an archive object. Rather, their contents are simply
      all written to the parent object directly without encapsulation.
