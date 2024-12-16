Scriptname mkyQuestIcons_MainQuestScript extends Quest

; TODO fetch this from a file
String[] Property data Auto

FormList Property activeItems Auto
FormList Property buildingList Auto

Quest Property testing Auto

Event OnInit()
    data = mkyQuestIcons_SKSEFunctions.FetchConfigs()
    registerForUpdate(5)
EndEvent

; Expected format is Type=FormToCheck|Quest

Event OnUpdate()
    buildingList.Revert()
    Int index = data.Length
    While (index > 0)
        index -= 1
        string fullString = data[index]
        Debug.Notification(fullString)
        ; Check for comments
        if (StringUtil.GetNthChar(fullString, 0) != ";")
            ; TODO move this registering to on init instead of on update
            ; TODO might wanna do a trim on this
            ; TODO add Debug.Trace to checks below to log if not considering a line
            ;Expects 1 or 2 params
            string[] split1 = StringUtil.Split(fullString, "=")
            string type = split1[0]

            if (type == "FormList")
                ;Expects 1 or 2 params
                string[] split2 = StringUtil.Split(split1[1], "|")
                ; TODO make sure split2 length is correct

                ; Check that linked quest is active and not completed
                Bool questCheck = true
                if split2.Length > 1
                    string[] questSplit = StringUtil.Split(split2[1], "~")
                    ; TODO how to get it based on reference id?
                    Quest loadedQuest = FindForm(questSplit[0], questSplit[1]) as Quest
                    questCheck = loadedQuest.IsActive() && !loadedQuest.IsCompleted()
                endIf

                string[] split3 = StringUtil.Split(split2[0], "~")
                if split3.Length == 2
                    FormList importedFormList = FindForm(split3[0], split3[1]) as FormList
                    if (questCheck)
                        buildingList.AddForms(importedFormList.ToArray())
                    Else
                        ; TODO unregister item (if its registered ideally)
                        ;Debug.Notification("questCheck failed for formlist with quest " + split2[1])
                    endIf
                endif
            ElseIf (type == "Form")
                ;Expects 1 or 2 params
                string[] split2 = StringUtil.Split(split1[1], "|")

                ; Check that linked quest is active and not completed
                Bool questCheck = true
                if split2.Length > 1
                    string[] questSplit = StringUtil.Split(split2[1], "~")
                    ; TODO how to get it based on reference id?
                    Quest loadedQuest = FindForm(questSplit[0], questSplit[1]) as Quest
                    ;Debug.Notification("isactive? " + loadedQuest.IsActive())
                    ;Debug.Notification("iscomplete? " + loadedQuest.IsCompleted())
                    questCheck = loadedQuest.IsActive() && !loadedQuest.IsCompleted()
                endIf

                string[] split3 = StringUtil.Split(split2[0], "~")
                if split3.Length == 2
                    if (questCheck)
                        Form importedForm = FindForm(split3[0], split3[1])

                        if importedForm == None
                            Debug.Notification("Could not import form with id " + split3[0] + " from " + split3[1])
                        else
                            buildingList.AddForm(importedForm)
                        endif
                    Else
                        ; TODO unregister item (if its registered ideally)
                        Debug.Notification("questCheck failed for form with quest " + split2[1])
                    endIf
                endif
            endif
        endif
    endWhile
    
    activeItems.Revert()
    activeItems.AddForms(buildingList.ToArray())
    AhzMoreHUD.RegisterIconFormList("questIcon", activeItems)
    AhzMoreHudIE.RegisterIconFormList("questIcon", activeItems)
EndEvent

Form Function FindForm(string formId, string esp)
    return Game.GetFormFromFile(PO3_SKSEFunctions.StringToInt(formId), esp)
EndFunction
