# oCLogTopic

!!! abstract inline end "Quick Infos"
    **Type:** Object<br/>
    **Format Name:** ZenGin Archive<br/>
    **File Extension:** `.ZEN`<br/>
    **Class Name:** `oCLogTopic`<br/>
    **Version Identifiers:**<br />
    — Gothic I: `0`<br/>
    — Gothic II: `0`<br/>
    **ZenKit Class:** *—*

=== "Gothic 1"

    - oCLogTopic
      {: .sp-type}
        - [TOPICDESCRIPTION](#topicdescription) = ""
          {: .sp-string}
        - [TOPICSECTION](#topicsection) = zELogSection_Missions
          {: .sp-enum}
        - [TOPICSTATUS](#topicstatus) = zELogTopicStatus_Free
          {: .sp-enum}
        - [LOGTOPICENTRYCOUNT](#logtopicentrycount) = 0
          {: .sp-int}
        - [LOGMANAGERENTRYCOUNT](#logmanagerentrycount) = 0
          {: .sp-int}
        - [ENTRYDESCRIPTION](#entrydescription) ... = ""
          {: .sp-string}

=== "Gothic 2"

    - oCLogTopic
      {: .sp-type}
        - [TOPICDESCRIPTION](#topicdescription) = ""
          {: .sp-string}
        - [TOPICSECTION](#topicsection) = zELogSection_Missions
          {: .sp-enum}
        - [TOPICSTATUS](#topicstatus) = zELogTopicStatus_Free
          {: .sp-enum}
        - [LOGTOPICENTRYCOUNT](#logtopicentrycount) = 0
          {: .sp-int}
        - [LOGMANAGERENTRYCOUNT](#logmanagerentrycount) = 0
          {: .sp-int}
        - [ENTRYDESCRIPTION](#entrydescription) ... = ""
          {: .sp-string}

=== "Gothic 1 (Save)"

    - oCLogTopic
      {: .sp-type}
        - [TOPICDESCRIPTION](#topicdescription) = ""
          {: .sp-string}
        - [TOPICSECTION](#topicsection) = zELogSection_Missions
          {: .sp-enum}
        - [TOPICSTATUS](#topicstatus) = zELogTopicStatus_Free
          {: .sp-enum}
        - [LOGTOPICENTRYCOUNT](#logtopicentrycount) = 0
          {: .sp-int}
        - [LOGMANAGERENTRYCOUNT](#logmanagerentrycount) = 0
          {: .sp-int}
        - [ENTRYDESCRIPTION](#entrydescription) ... = ""
          {: .sp-string}

=== "Gothic 2 (Save)"

    - oCLogTopic
      {: .sp-type}
        - [TOPICDESCRIPTION](#topicdescription) = ""
          {: .sp-string}
        - [TOPICSECTION](#topicsection) = zELogSection_Missions
          {: .sp-enum}
        - [TOPICSTATUS](#topicstatus) = zELogTopicStatus_Free
          {: .sp-enum}
        - [LOGTOPICENTRYCOUNT](#logtopicentrycount) = 0
          {: .sp-int}
        - [LOGMANAGERENTRYCOUNT](#logmanagerentrycount) = 0
          {: .sp-int}
        - [ENTRYDESCRIPTION](#entrydescription) ... = ""
          {: .sp-string}

## Properties

#### `TOPICDESCRIPTION` {: .t-str}

:   The name of the quest log entry.


#### `TOPICSECTION` {: .t-enum}

:   The quest log section the entry is in.

    * `zELogSection_Missions` — The entry is in one of the mission sections of the quest log.
    * `zELogSection_Notes` — The entry is in the general notes section of the quest log

#### `TOPICSTATUS` {: .t-enum}

:   The status the quest log entry.
    
    * `zELogTopicStatus_Free` — Unknown, makes the entry disappear from the quest log.
    * `zELogTopicStatus_Running` — The entry is in the 'running quests' section of the quest log.
    * `zELogTopicStatus_Success` — The entry is in the 'succeeded quests' section of the quest log.
    * `zELogTopicStatus_Failure` — The entry is in the 'failed quests' section of the quest log.
    * `zELogTopicStatus_Obsolete` — The entry is in the 'obsolete quests' section of the quest log.

    !!! warning
        This field is ignored unless [`TOPICSECTION`](#TOPICSECTION) is set to `zELogSection_Missions`.

#### `LOGTOPICENTRYCOUNT` {: .t-int}

:   The number of entries of this log topic.

#### `LOGMANAGERENTRYCOUNT` {: .t-int}

:   The same as [`LOGTOPICENTRYCOUNT`](#LOGTOPICENTRYCOUNT).

#### `ENTRYDESCRIPTION` {: .t-int}

:   A single entry in the log topic. Log entries are shown in chronological order on the detailed view of the
    log topic, separated by three dashes.
