## WORK_IN_PROGRESS

Plugin to add moreHUD icons for items required for quests.

Grabs and parses files to determine what items should have icons added.

Heavily inspired by PO3 packages: https://github.com/powerof3
Created from template: https://github.com/SkyrimScripting/SKSE_Template_HelloWorld

### Config Format

The scripts pull in all files ending in _ITM.ini.

Comments can be made using ; at the beggining of a line (e.g. ;This is a comment).

There are multiple types of configs you can use to mark quest items, as listed below.

#### Formlist

```
; Display icon for any item in formlist
FormList = Favor110GiftList
; Display icon for any item in formlist if provided quest is active
FormList = 0x000D9CF8~Skyrim.esm|0x000D9B65~Skyrim.esm
```

The formlist option will show all items within the formlist as quest items. You can provide a quest as a second parameter if you would like only to display if the quest is active.

#### Form

```
; Display icon on item if quest is active
Form = DaedraHeart|0x0003B681~Skyrim.esm
```

The form option will show the icon on the specified item if the provided quest is active.

#### Quest Alias

```
; Display icon for quest alias of id 22 on quest if quest active
Alias = 0x000403B0~Skyrim.esm?22
```

The alais option will show the icon on the specified quest alias id if the provided quest is active.