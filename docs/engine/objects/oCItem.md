# `oCItem`

!!! abstract inline end "Quick Infos"
    **Type:** Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCItem`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `0`<br/>
    — Gothic II: `0`<br/>
    **ZenKit Class:** `—` <br/>
    **Sources:**<br/>
    — [gothic-library.ru](http://www.gothic-library.ru/publ/class_occstrigger/1-1-0-530)

Represents an item in the game world. Items are special VObjects which are tied to a script instance which defines their attributes. They have physics enabled and can be targeted and picked up by the player. 

=== "Gothic 1"
    - [zCVob](zCVob.md)
      {: .sp-class}
    - [oCVob](oCVob.md)
      {: .sp-class}
    - oCItem
      {: .sp-class}
        - [itemInstance](#iteminstance) = 
          {: .sp-string}

=== "Gothic 2"
    - [zCVob](zCVob.md)
      {: .sp-class}
    - [oCVob](oCVob.md)
      {: .sp-class}
    - oCItem
      {: .sp-class}
        - [itemInstance](#iteminstance) = 
          {: .sp-string}

=== "Gothic 1 (Save)"
    - [zCVob](zCVob.md)
      {: .sp-class}
    - [oCVob](oCVob.md)
      {: .sp-class}
    - oCItem
      {: .sp-class}
        - [itemInstance](#iteminstance) = 
          {: .sp-string}
        - [amount](#amount) = 
          {: .sp-int}
        - [flags](#flags) = 
          {: .sp-int}

=== "Gothic 2 (Save)"
    - [zCVob](zCVob.md)
      {: .sp-class}
    - [oCVob](oCVob.md)
      {: .sp-class}
    - oCItem
      {: .sp-class}
        - [itemInstance](#iteminstance) = 
          {: .sp-string}
        - [amount](#amount) = 
          {: .sp-int}
        - [flags](#flags) = 
          {: .sp-int}

## Properties

#### `itemInstance` {: .sp-string}

:   The name of the script instance representing the item.

#### `amount` {: .sp-int}

:   Unknown.

    !!! warning
        This property is only available in saved games.

#### `flags` {: .sp-int}

:   Unknown.

    !!! warning
        This property is only available in saved games.

