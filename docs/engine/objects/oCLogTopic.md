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

<ul class="sp-list">
    <li class="sp-type">
        <span>oCLogTopic</span>
        <ul class="sp-list">
            <li class="sp-string"><a href="#TOPICDESCRIPTION">TOPICDESCRIPTION</a> = ""</li>
            <li class="sp-enum"><a href="#TOPICSECTION">TOPICSECTION</a> = zELogSection_Missions</li>
            <li class="sp-enum"><a href="#TOPICSTATUS">TOPICSTATUS</a> = zELogTopicStatus_Free</li>
            <li class="sp-int"><a href="#LOGTOPICENTRYCOUNT">LOGTOPICENTRYCOUNT</a> = 0</li>
            <li class="sp-int"><a href="#LOGMANAGERENTRYCOUNT">LOGMANAGERENTRYCOUNT</a> = 0</li>
            <li class="sp-string"><a href="#ENTRYDESCRIPTION">[ENTRYDESCRIPTION]</a> = ""</li>
        </ul>
    </li>
</ul>

<a name="TOPICDESCRIPTION" class="t-str"></a> `TOPICDESCRIPTION`

:   The name of the quest log entry.


<a name="TOPICSECTION" class="t-enum"></a> `TOPICSECTION`

:   The quest log section the entry is in.

    * `zELogSection_Missions` — The entry is in one of the mission sections of the quest log.
    * `zELogSection_Notes` — The entry is in the general notes section of the quest log

<a name="TOPICSTATUS" class="t-enum"></a> `TOPICSTATUS`

:   The status the quest log entry.
    
    * `zELogTopicStatus_Free` — Unknown, makes the entry disappear from the quest log.
    * `zELogTopicStatus_Running` — The entry is in the 'running quests' section of the quest log.
    * `zELogTopicStatus_Success` — The entry is in the 'succeeded quests' section of the quest log.
    * `zELogTopicStatus_Failure` — The entry is in the 'failed quests' section of the quest log.
    * `zELogTopicStatus_Obsolete` — The entry is in the 'obsolete quests' section of the quest log.

    !!! warning
        This field is ignored unless [`TOPICSECTION`](#TOPICSECTION) is set to `zELogSection_Missions`.

<a name="LOGTOPICENTRYCOUNT" class="t-int"></a> `LOGTOPICENTRYCOUNT`

:   The number of entries of this log topic.

<a name="LOGMANAGERENTRYCOUNT" class="t-int"></a> `LOGMANAGERENTRYCOUNT`

:   The same as [`LOGTOPICENTRYCOUNT`](#LOGTOPICENTRYCOUNT).

<a name="ENTRYDESCRIPTION" class="t-int"></a> `ENTRYDESCRIPTION`

:   A single entry in the log topic. Log entries are shown in chronological order on the detailed view of the
    log topic, separated by three dashes.
