# zCCamTrj_KeyFrame

!!! abstract inline end "Quick Infos"
    **Type:** Virtual Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `zCCamTrj_KeyFrame`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `12289`<br/>
    — Gothic II: `52224`<br/>
    **ZenKit Class:** `VCameraTrajectoryFrame`<br/>
    **Sources:**<br/>
    — [gothic-library.ru](http://www.gothic-library.ru/publ/zccamtrj_keyframe/1-1-0-495)

=== "Gothic 1"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - zCCamTrj_KeyFrame
      {: .sp-class}
        - [time](#time) = -1.0
          {: .sp-float}
        - [angleRollDeg](#anglerolldeg) = 0.0
          {: .sp-float}
        - [camFOVScale](#camfovscale) = 0.0
          {: .sp-float}
        - [motionType](#motiontype) = SMOOTH
          {: .sp-enum}
        - [motionTypeFOV](#motiontypefov) = SMOOTH
          {: .sp-enum}
        - [motionTypeRoll](#motiontyperoll) = SMOOTH
          {: .sp-enum}
        - [motionTypeTimeScale](#motiontypetimescale) = SMOOTH
          {: .sp-enum}
        - Details
          {: .sp-folder}
            - [tension](#tension) = 0.0
              {: .sp-float}
            - [bias](#bias) = 0.0
              {: .sp-float}
            - [continuity](#continuity) = 0.0
              {: .sp-float}
            - [timeScale](#timescale) = 1.0
              {: .sp-float}
            - [timeIsFixed](#timeisfixed) = FALSE
              {: .sp-bool}
        - [originalPose](#originalpose) = 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
          {: .sp-misc}

=== "Gothic 2"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - zCCamTrj_KeyFrame
      {: .sp-class}
        - [time](#time) = -1.0
          {: .sp-float}
        - [angleRollDeg](#anglerolldeg) = 0.0
          {: .sp-float}
        - [camFOVScale](#camfovscale) = 0.0
          {: .sp-float}
        - [motionType](#motiontype) = SMOOTH
          {: .sp-enum}
        - [motionTypeFOV](#motiontypefov) = SMOOTH
          {: .sp-enum}
        - [motionTypeRoll](#motiontyperoll) = SMOOTH
          {: .sp-enum}
        - [motionTypeTimeScale](#motiontypetimescale) = SMOOTH
          {: .sp-enum}
        - Details
          {: .sp-folder}
            - [tension](#tension) = 0.0
              {: .sp-float}
            - [bias](#bias) = 0.0
              {: .sp-float}
            - [continuity](#continuity) = 0.0
              {: .sp-float}
            - [timeScale](#timescale) = 1.0
              {: .sp-float}
            - [timeIsFixed](#timeisfixed) = FALSE
              {: .sp-bool}
        - [originalPose](#originalpose) = 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
          {: .sp-misc}

=== "Gothic 1 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - zCCamTrj_KeyFrame
      {: .sp-class}
        - [time](#time) = -1.0
          {: .sp-float}
        - [angleRollDeg](#anglerolldeg) = 0.0
          {: .sp-float}
        - [camFOVScale](#camfovscale) = 0.0
          {: .sp-float}
        - [motionType](#motiontype) = SMOOTH
          {: .sp-enum}
        - [motionTypeFOV](#motiontypefov) = SMOOTH
          {: .sp-enum}
        - [motionTypeRoll](#motiontyperoll) = SMOOTH
          {: .sp-enum}
        - [motionTypeTimeScale](#motiontypetimescale) = SMOOTH
          {: .sp-enum}
        - Details
          {: .sp-folder}
            - [tension](#tension) = 0.0
              {: .sp-float}
            - [bias](#bias) = 0.0
              {: .sp-float}
            - [continuity](#continuity) = 0.0
              {: .sp-float}
            - [timeScale](#timescale) = 1.0
              {: .sp-float}
            - [timeIsFixed](#timeisfixed) = FALSE
              {: .sp-bool}
        - [originalPose](#originalpose) = 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
          {: .sp-misc}

=== "Gothic 2 (Save)"

    - [zCVob](zCVob.md)
      {: .sp-class}
    - zCCamTrj_KeyFrame
      {: .sp-class}
        - [time](#time) = -1.0
          {: .sp-float}
        - [angleRollDeg](#anglerolldeg) = 0.0
          {: .sp-float}
        - [camFOVScale](#camfovscale) = 0.0
          {: .sp-float}
        - [motionType](#motiontype) = SMOOTH
          {: .sp-enum}
        - [motionTypeFOV](#motiontypefov) = SMOOTH
          {: .sp-enum}
        - [motionTypeRoll](#motiontyperoll) = SMOOTH
          {: .sp-enum}
        - [motionTypeTimeScale](#motiontypetimescale) = SMOOTH
          {: .sp-enum}
        - Details
          {: .sp-folder}
            - [tension](#tension) = 0.0
              {: .sp-float}
            - [bias](#bias) = 0.0
              {: .sp-float}
            - [continuity](#continuity) = 0.0
              {: .sp-float}
            - [timeScale](#timescale) = 1.0
              {: .sp-float}
            - [timeIsFixed](#timeisfixed) = FALSE
              {: .sp-bool}
        - [originalPose](#originalpose) = 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
          {: .sp-misc}

## Properties

#### `time` {: .sp-float}

:   Unclear.

#### `angleRollDeg` {: .sp-float}

:   The camera rotation angle of the frame along the Z axis.

#### `camFOVScale` {: .sp-float}

:   Focal zoom of the camera's field of view (FOV) of the frame. Use reasonable values to avoid dizzying effects.

#### `motionType` {: .sp-enum}

:   The type of camera movement for the frame.

    * `SMOOTH` — The movement method is calculated taking into account nearby frames.
    * `LINEAR` — Constant speed in the current frame.
    * `STEP` - Step by step. Movement is jerky.
    * `SLOW` - The camera movement slows down in the current frame.
    * `FAST` - The camera movement accelerates in the current frame.
    * `CUSTOM` - Not implemented.

#### `motionTypeFOV` {: .sp-enum}

:   The type of focus change for the frame.

    * `SMOOTH` — The movement method is calculated taking into account nearby frames.
    * `LINEAR` — Constant speed in the current frame.
    * `STEP` - Step by step. Movement is jerky.
    * `SLOW` - The camera movement slows down in the current frame.
    * `FAST` - The camera movement accelerates in the current frame.
    * `CUSTOM` - Not implemented.

#### `motionTypeRoll` {: .sp-enum}

:   The type of rotation for the frame.

    * `SMOOTH` — The movement method is calculated taking into account nearby frames.
    * `LINEAR` — Constant speed in the current frame.
    * `STEP` - Step by step. Movement is jerky.
    * `SLOW` - The camera movement slows down in the current frame.
    * `FAST` - The camera movement accelerates in the current frame.
    * `CUSTOM` - Not implemented.

#### `motionTypeTimeScale` {: .sp-enum}

:   Unclear.

    * `SMOOTH` — The movement method is calculated taking into account nearby frames.
    * `LINEAR` — Constant speed in the current frame.
    * `STEP` - Step by step. Movement is jerky.
    * `SLOW` - The camera movement slows down in the current frame.
    * `FAST` - The camera movement accelerates in the current frame.
    * `CUSTOM` - Not implemented.

#### `tension` {: .sp-float}

:   Defines the bend of the camera motion curve at the specified key. Larger values result in a sharper bend.

#### `bias` {: .sp-float}

:   Determines the slope of a line in one direction. The direction of the slope is determined by the sign, and the
    steepness by the value.

#### `continuity` {: .sp-float}

:   Determines the amount of curve discontinuity at this animation key.

#### `timeScale` {: .sp-float}

:   Unknown.

#### `timeIsFixed` {: .sp-bool}

:   An auxiliary flag indicating the time of the key frame, which cannot be changed automatically.

    * `TRUE` — The time of this frame is set manually and cannot be changed automatically.
    * `FALSE` — The frame time is calculated from the total specified animation playback time.

#### `originalPose` {: .sp-misc}

:   Unknown.

    !!! warning
        This property is not available in the Spacer.
