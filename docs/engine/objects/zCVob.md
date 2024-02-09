# zCVob

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCVob`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `12289`<br/>
    — Gothic II: `52224`<br/>
    **ZenKit Class:** `VirtualObject`

Represents so-called virtual objects which appear in the game world. `zCVob` is the base class for all virtual
objects which include everything from static models like boxes to interactive elements like doors and chests and
invisible trigger objects. They are also referred to as VObjects in this documentation.

=== "Gothic 1"
    
    - zCVob
    {: .sp-type}
        - Internals
        {: .sp-folder}
            - [pack](#pack) = 0
            {: .sp-int}
            - [presetName](#presetname) = ""
            {: .sp-string}
            - [bbox3DWS](#bbox3dws) = 0 0 0 0 0 0
            {: .sp-misc}
            - [trafoOSToWSRot](#trafoostowsrot) = 0
            {: .sp-misc}
            - [trafoOSToWSPOS](#trafoostowspos) = 0 0 200
            {: .sp-vec}
        - Vob
        {: .sp-folder}
            - [vobName](#vobname) = ""
            {: .sp-string}
            - [visual](#visual) = ""
            {: .sp-string}
            - [showVisual](#showvisual) = TRUE
            {: .sp-bool}
            - [visualCamAlign](#visualcamalign) = NONE
            {: .sp-enum}
            - [cdStatic](#cdstatic) = FALSE
            {: .sp-bool}
            - [cdDyn](#cddyn) = FALSE
            {: .sp-bool}
            - [staticVob](#staticvob) = FALSE
            {: .sp-bool}
            - [dynShadow](#dynshadow) = DS_NONE
            {: .sp-enum}
        - visual = NULL
        {: .sp-type}
        - ai = NULL
        {: .sp-type}

=== "Gothic 2"

    - zCVob
    {: .sp-type}
        - Internals
        {: .sp-folder}
            - [pack](#pack) = 0
            {: .sp-int}
            - [presetName](#presetname) = ""
            {: .sp-string}
            - [bbox3DWS](#bbox3dws) = 0 0 0 0 0 0
            {: .sp-misc}
            - [trafoOSToWSRot](#trafoostowsrot) = 0
            {: .sp-misc}
            - [trafoOSToWSPOS](#trafoostowspos) = 0 0 200
            {: .sp-vec}
        - Vob
        {: .sp-folder}
            - [vobName](#vobname) = ""
            {: .sp-string}
            - [visual](#visual) = ""
            {: .sp-string}
            - [showVisual](#showvisual) = TRUE
            {: .sp-bool}
            - [visualCamAlign](#visualcamalign) = NONE
            {: .sp-enum}
            - [visualAniMode](#visualanimode) = NONE
            {: .sp-enum}
            - [visualAniModeStrength](#visualanimodestrength) = 0.0
            {: .sp-float}
            - [vobFarClipZScale](#vobfarclipzscale) = 1.0
            {: .sp-float}
            - [cdStatic](#cdstatic) = FALSE
            {: .sp-bool}
            - [cdDyn](#cddyn) = FALSE
            {: .sp-bool}
            - [staticVob](#staticvob) = FALSE
            {: .sp-bool}
            - [dynShadow](#dynshadow) = DS_NONE
            {: .sp-enum}
            - [zbias](#zbias) = 1
            {: .sp-int}
            - [isAmbient](#isambient) = FALSE
            {: .sp-bool}
        - visual = NULL
        {: .sp-type}
        - ai = NULL
        {: .sp-type}


=== "Gothic 1 (Save)"

    - zCVob
    {: .sp-type}
        - Internals
        {: .sp-folder}
            - [pack](#pack) = 0
            {: .sp-int}
            - [presetName](#presetname) = ""
            {: .sp-string}
            - [bbox3DWS](#bbox3dws) = 0 0 0 0 0 0
            {: .sp-misc}
            - [trafoOSToWSRot](#trafoostowsrot) = 0
            {: .sp-misc}
            - [trafoOSToWSPOS](#trafoostowspos) = 0 0 200
            {: .sp-vec}
        - Vob
        {: .sp-folder}
            - [vobName](#vobname) = ""
            {: .sp-string}
            - [visual](#visual) = ""
            {: .sp-string}
            - [showVisual](#showvisual) = TRUE
            {: .sp-bool}
            - [visualCamAlign](#visualcamalign) = NONE
            {: .sp-enum}
            - [cdStatic](#cdstatic) = FALSE
            {: .sp-bool}
            - [cdDyn](#cddyn) = FALSE
            {: .sp-bool}
            - [staticVob](#staticvob) = FALSE
            {: .sp-bool}
            - [dynShadow](#dynshadow) = DS_NONE
            {: .sp-enum}
        - visual = NULL
        {: .sp-type}
        - ai = NULL
        {: .sp-type}
        - [sleepMode](#sleepmode) = 0
        {: .sp-int}
        - [nextOnTimer](#nextontimer) = 0.0
        {: .sp-float}

=== "Gothic 2 (Save)"

    - zCVob
    {: .sp-type}
        - Internals
        {: .sp-folder}
            - [pack](#pack) = 0
            {: .sp-int}
            - [presetName](#presetname) = ""
            {: .sp-string}
            - [bbox3DWS](#bbox3dws) = 0 0 0 0 0 0
            {: .sp-misc}
            - [trafoOSToWSRot](#trafoostowsrot) = 0
            {: .sp-misc}
            - [trafoOSToWSPOS](#trafoostowspos) = 0 0 200
            {: .sp-vec}
        - Vob
        {: .sp-folder}
            - [vobName](#vobname) = ""
            {: .sp-string}
            - [visual](#visual) = ""
            {: .sp-string}
            - [showVisual](#showvisual) = TRUE
            {: .sp-bool}
            - [visualCamAlign](#visualcamalign) = NONE
            {: .sp-enum}
            - [visualAniMode](#visualanimode) = NONE
            {: .sp-enum}
            - [visualAniModeStrength](#visualanimodestrength) = 0.0
            {: .sp-float}
            - [vobFarClipZScale](#vobfarclipzscale) = 1.0
            {: .sp-float}
            - [cdStatic](#cdstatic) = FALSE
            {: .sp-bool}
            - [cdDyn](#cddyn) = FALSE
            {: .sp-bool}
            - [staticVob](#staticvob) = FALSE
            {: .sp-bool}
            - [dynShadow](#dynshadow) = DS_NONE
            {: .sp-enum}
            - [zbias](#zbias) = 1
            {: .sp-int}
            - [isAmbient](#isambient) = FALSE
            {: .sp-bool}
        - visual = NULL
        {: .sp-type}
        - ai = NULL
        {: .sp-type}
        - [sleepMode](#sleepmode) = 0
        {: .sp-int}
        - [nextOnTimer](#nextontimer) = 0.0
        {: .sp-float}

## Properties

#### `pack` {: .t-int}

:   Internal property which indicates how the data associated with this VObject is stored. Can not be changed in the
    Spacer and is inaccessible in *ZenKit*.

#### `presetName` {: .t-str}

:   The name of the template used to create this VObject in the Spacer. Exposed as a normal field by *ZenKit*.


#### `bbox3DWS` {: .t-raw}

:   The 3-dimensional bounding box of this VObject to be used for collision detection. This is used in conjunction with
    [`cdStatic`](#cdstatic) and [`cdDyn`](#cddyn) to detect whether two VObject are colliding. Depending on the kind of
    VObject collision with it may do different things which are documented for each VObject separately.

    The bounding box is described by two opposite diagonal points in 3D world space coordinates.

#### `trafoOSToWSRot` {: .t-raw}

:   The rotation of this VObject in world space, represented by a 3x3 rotation matrix.

#### `trafoOSToWSPos` {: .t-vec}

:   The coordinates of this VObject in world space.

#### `vobName` {: .t-str}

:   The name of this VObject. VObject names are not necessarily unique but are sometimes helpful for identifying them.
    For some VObjects, like [zCVobSpot](zCVobSpot.md) or [zCTrigger](zCTrigger.md) setting the `vobName` is required

#### `visual` {: .t-str}

:   The name of the visual component associated with this VObject. This is usually the name of a file containing the
    data for a visual object. The following file formats are supported:

    * `*.3DS` — Static models
    * `*.PFX` — Particle effects
    * `*.TGA` — Textures
    * `*.MDS`, `.ASC` — Interactive or rigged models
    * `*.MMS` — Morph-mesh animated models

#### `showVisual` {: .t-bool}

:   Indicates whether this VObject should display its associated visual.
    <br /> `TRUE` — Display the visual. `FALSE` — Hide the visual.

#### `visualCamAlign` {: .t-enum}

:   Indicates how this VObject should be aligned in relation to the camera. This may be used with grass or flowers
    which only consist of a 2-dimensional sprite to have it always face the camera, for example. Possible value are

    * `NONE` — The sprite is not affected by the camera's rotation.
    * `YAW` — The sprite rotates with the camera's yaw axis.
    * `FULL` — The sprite rotates alonside the camera.

#### `visualAniMode` {: .t-enum}

:   The type of wind animation to apply to the VObject. One of
    
    * `NONE` — No wave animation.
    * `WIND` — Indicates that the object should be animated as if shifting in strong wind. Used mostly for animating
      grass and other small foliage.
    * `WIND2` — Indicates that the object should be animated as if shifting in light wind. Used mostly for animating trees.

    !!! warning
        This property is only available in Gothic II.

#### `visualAniModeStrength` {: .t-float}

:   Indicates the strength of the animation set through [`visualAniMode`](#visualanimode). This value is ignored if
    [`visualAniMode`](#visualanimode) is set to `NONE`.

    !!! warning
        This property is only available in Gothic II.

#### `vobFarClipZScale` {: .t-float}

:   Indicates the draw distance of this VObject. Its value should be between 0 and 2. 0 Means, that this object is
    never drawn on screen but collisions are still calculated. A value of 2 means, that the draw distance of this
    VObject is the same as the draw distance specified in the [`zCZoneVobFarPlane`](zCZoneVobFarPlane.md) object.

    !!! warning
        This property is only available in Gothic II.

#### `cdStatic` {: .t-bool}

:   Indicates whether this VObject should collide with other VObjects. This is used for placing the object in the
    ZenGin map editor, the *Spacer*, where it will prevent the VObject being placed wither other VObjects. This setting
    is irrelevant for runtime collision detection.

    * `TRUE` — Collide with all other VObjects.
    * `FALSE` — Don't collide with other static VObjects.
    

#### `cdDyn` {: .t-bool}

:   Indicates whether this VObject should collide with dynamic objects. For this purpose, dynamic objects are the
    player, NPCs and items. If this flag is set, implementations should apply collision detection to this VObject.
    
    * `TRUE` — Collide with dynamic VObjects.
    * `FALSE` — Don't collide with dynamic VObjects.

#### `staticVob` {: .t-bool}

:   Indicates whether this VObject should be included during static lighting calculations. These lighting calculations
    are done at compile-time and will bake VObjects with this flag into the light-maps available from the world mesh.
    
    * `TRUE` — Exclude this VObject from dynamic lighting calculations.
    * `FALSE` — Include this VObject in dynamic lighting calculations.

#### `dynShadow` {: .t-enum}

:   The type of dynamic shadow to be used for this VObject.
    
    * `DS_NONE` — The VObject does not cast any shadow.
    * `DS_BLOB` — The VObject casts a dark circle at its base.

#### `zbias` {: .t-int}

:   The depth-bias for this VObject. This value is passed directly to Direct3D when rendering the associated visual of
    this VObject. Sadly, because documentation is not available for the very old Direct3D version used by the ZenGin,
    the exact behavior it would have resulted in are unknown.

    !!! warning
        This property is only available in Gothic II.

#### `isAmbient` {: .t-bool}

:   Indicates that this VObject is *ambient*. It looks like this VObject setting was a system used during development
    when access to the game's source code was available. Basically, the global variable `zCWorld::s_bAmbientVobsEnabled`
    could be used to hide or show VObjects which have the `isAmbient` flag set. In release builds, this variable is
    always set to `true`, thus the `isAmbient` flag does not have any perceivable effect on the game. It follows, that
    this field should be ignored by most implementations.

    !!! warning
        This property is only available in Gothic II.

#### `sleepMode` {: .t-int}

:   Unknown.

    !!! warning
        This property is only available in saved games.

#### `nextOnTimer` {: .t-float}

:   Unknown.

    !!! warning
        This property is only available in saved games.


## Hierarchy

The following list shows the virtual object class hierarchy. Click on any of the VObjects to see more details.

* [zCVob](zCVob.md)
    * [zCVobSpot](zCVobSpot.md)
    * [zCVobStair](zCVobStair.md)
    * [zCVobStartpoint](zCVobStartpoint.md)
    * [zCVobLevelCompo](zCVobLevelCompo.md)
    * [zCCSCamera](zCCSCamera.md)
    * [zCCamTrj_KeyFrame](zCCamTrj_KeyFrame.md)
    * [zCVobLight](zCVobLight.md)
    * [zCVobAnimate](zCVobAnimate.md)
    * [oCItem](oCItem.md)
    * [zCVobLensFlare](zCVobLensFlare.md)
    * [zCPFXController](zCPFXController.md)
    * [zCMessageFilter](zCMessageFilter.md)
    * [zCCodeMaster](zCCodeMaster.md)
    * [zCMoverController](zCMoverController.md)
    * [oCTouchDamage](oCTouchDamage.md)
    * [zCEarthquake](zCEarthquake.md)
    * [oCNpc](oCNpc.md)
    * [zCVobScreenFX](zCVobScreenFX.md)
    * [oCMOB](oCMOB.md)
        * [oCMobInter](oCMobInter.md)
            * [oCMobFire](oCMobFire.md)
            * [oCMobContainer](oCMobContainer.md)
            * [oCMobDoor](oCMobDoor.md)
            * [oCMobLadder](oCMobLadder.md)
            * [oCMobSwitch](oCMobSwitch.md)
            * [oCMobWheel](oCMobWheel.md)
            * [oCMobBed](oCMobBed.md)
    * [zCVobSound](zCVobSound.md)
        * [zCVobSoundDaytime](zCVobSoundDaytime.md)
    * [zCTrigger](zCTrigger.md)
        * [oCCSTrigger](oCCSTrigger.md)
        * [zCMover](zCMover.md)
        * [zCTriggerList](zCTriggerList.md)
        * [oCTriggerScript](oCTriggerScript.md)
        * [oCTriggerChangeLevel](oCTriggerChangeLevel.md)
    * [zCTriggerWorldStart](zCTriggerWorldStart.md)
    * [zCTriggerUntouch](zCTriggerUntouch.md)
    * [oCZoneMusic](oCZoneMusic.md)
        * [oCZoneMusicDefault](oCZoneMusicDefault.md)
    * [zCZoneVobFarPlane](zCZoneVobFarPlane.md)
        * [zCZoneVobFarPlaneDefault](zCZoneVobFarPlaneDefault.md)
    * [zCZoneZFog](zCZoneZFog.md)
        * [zCZoneZFogDefault](zCZoneZFogDefault.md)

## Sources

The help file for the Spacer, found [here](https://wiki.worldofgothic.de/doku.php?id=spacer:hilfedatei) and the
Russian-language knowledge accumulator [gothic-library.ru](http://www.gothic-library.ru/publ/class_zcvob/1-1-0-467).
