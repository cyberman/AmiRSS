#ifndef LOC_H
#define LOC_H


/****************************************************************************/


/* This file was created automatically by CatComp.
 * Do NOT edit by hand!
 */


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifdef CATCOMP_ARRAY
#undef CATCOMP_NUMBERS
#undef CATCOMP_STRINGS
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#endif

#ifdef CATCOMP_BLOCK
#undef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif


/****************************************************************************/


#ifdef CATCOMP_NUMBERS

#define MSG_Error_ReqTitle 1
#define MSG_Error_ReqGadget 2
#define MSG_Error_NoMem 3
#define MSG_Error_NoLocale 4
#define MSG_Error_NoMUIMaster 5
#define MSG_Error_NoVTK 6
#define MSG_Error_NoUtility 7
#define MSG_Error_NoGfx 8
#define MSG_Error_DataTypes 9
#define MSG_Error_NoIFFParse 10
#define MSG_Error_NoRexxSysBase 11
#define MSG_Error_Icon 12
#define MSG_Error_NoCodesets 13
#define MSG_Error_Expat 14
#define MSG_Error_NoLoc 15
#define MSG_Error_NoAppClass 16
#define MSG_Error_NoWinClass 17
#define MSG_Error_NoRootScrollgroupClass 18
#define MSG_Error_NoRootStringClass 19
#define MSG_Error_NoRootBarClass 20
#define MSG_Error_NoGroupClass 21
#define MSG_Error_NoNewsClass 22
#define MSG_Error_NoGrouplistClass 23
#define MSG_Error_NoHTMLClass 24
#define MSG_Error_NoPicClass 25
#define MSG_Error_CantCreateDirs 26
#define MSG_Error_NoApp 27
#define MSG_Error_NoWin 28
#define MSG_Error_NoSocket 29
#define MSG_Error_NoHost 30
#define MSG_Error_CantConnect 31
#define MSG_Error_Send 32
#define MSG_Error_Recv 33
#define MSG_Error_ProtoError 34
#define MSG_Error_ServerError 35
#define MSG_Error_InvalidURL 36
#define MSG_Error_Aborted 37
#define MSG_Error_NoSocketBase 38
#define MSG_Group_UnknownEncoding 39
#define MSG_Group_CantParseXML 40
#define MSG_Group_Date_DSHMS 200
#define MSG_Group_Date_DHMS 201
#define MSG_Group_Date_HMS 202
#define MSG_Group_Date_Now 203
#define MSG_Status_ResolvingHost 204
#define MSG_Status_Connecting 205
#define MSG_Status_Sending 206
#define MSG_Status_Receiving 207
#define MSG_Status_Done 208
#define MSG_Status_ReadingFeed 209
#define MSG_Status_Updating 210
#define MSG_Status_Updated 211
#define MSG_Status_ReadingFeeds 212
#define MSG_Status_ReadingGroup 213
#define MSG_Status_GroupRead 214
#define MSG_App_Copyright 300
#define MSG_App_Description 301
#define MSG_Menu_Project 302
#define MSG_Menu_About 303
#define MSG_Menu_AboutMUI 304
#define MSG_Menu_Hide 305
#define MSG_Menu_Quit 306
#define MSG_Menu_Feeds 307
#define MSG_Menu_AddGroup 308
#define MSG_Menu_AddFeed 309
#define MSG_Menu_EditFeed 310
#define MSG_Menu_Remove 311
#define MSG_Menu_LastSaved 312
#define MSG_Menu_NoDragDrop 313
#define MSG_Menu_EditFeeds 314
#define MSG_Menu_Settings 315
#define MSG_Menu_EditPrefs 316
#define MSG_Menu_LastSavedPrefs 317
#define MSG_Menu_Snapshot 318
#define MSG_Menu_UnSnapshot 319
#define MSG_Menu_Help 320
#define MSG_Menu_MUI 321
#define MSG_Menu_QuickQuit 322
#define MSG_Menu_NoAutoUpdate 323
#define MSG_Menu_UpdateAll 324
#define MSG_Menu_FlushCookies 325
#define MSG_Menu_FlushImages 326
#define MSG_Menu_HideGroups 327
#define MSG_Menu_ScreenMode 328
#define MSG_Menu_SaveFeeds 329
#define MSG_Menu_HideBar 330
#define MSG_Menu_HidePreview 331
#define MSG_Menu_NewsPage 332
#define MSG_Menu_HTMLPage 333
#define MSG_Win_Title 400
#define MSG_Gad_Add 401
#define MSG_Gad_AddHelp 402
#define MSG_Gad_Edit 403
#define MSG_Gad_EditHelp 404
#define MSG_Gad_Remove 405
#define MSG_Gad_RemoveHelp 406
#define MSG_Gad_Feed 407
#define MSG_Gad_FeedHelp 408
#define MSG_Gad_Stop 409
#define MSG_Gad_StopHelp 410
#define MSG_Gad_Search 411
#define MSG_Gad_SearchHelp 412
#define MSG_Gad_Main 413
#define MSG_Gad_MainHelp 414
#define MSG_Gad_HTML 415
#define MSG_Gad_HTMLHelp 416
#define MSG_Gad_Prefs 417
#define MSG_Gad_PrefsHelp 418
#define MSG_Gad_Back 419
#define MSG_Gad_BackHelp 420
#define MSG_Gad_Next 421
#define MSG_Gad_NextHelp 422
#define MSG_Gad_Reload 423
#define MSG_Gad_ReloadHelp 424
#define MSG_Gad_URL 425
#define MSG_Gad_URLHelp 426
#define MSG_Gad_AddGroup 427
#define MSG_Gad_AddGroupHelp 428
#define MSG_Screen_Title 429
#define MSG_Win_TitleFeed 430
#define MSG_Screen_TitleFeed 431
#define MSG_CMenu_Configure 500
#define MSG_CMenu_DeleteAll 501
#define MSG_CMenu_Cut 502
#define MSG_CMenu_Copy 503
#define MSG_CMenu_Paste 504
#define MSG_CMenu_ViewMode_TextGfx 505
#define MSG_CMenu_ViewMode_Gfx 506
#define MSG_CMenu_ViewMode_Text 507
#define MSG_CMenu_Borderless 508
#define MSG_CMenu_Sunny 509
#define MSG_CMenu_Raised 510
#define MSG_CMenu_AddGroup 511
#define MSG_CMenu_AddFeed 512
#define MSG_CMenu_Edit 513
#define MSG_CMenu_Link 514
#define MSG_CMenu_DefWidthThis 515
#define MSG_CMenu_DefWidthAll 516
#define MSG_CMenu_DefOrderThis 517
#define MSG_CMenu_DefOrderAll 518
#define MSG_CMenu_CopyTitle 519
#define MSG_CMenu_CopyLink 520
#define MSG_CMenu_Browser 521
#define MSG_CMenu_SetAsDefault 522
#define MSG_CMenu_Feed 523
#define MSG_CMenu_Remove 524
#define MSG_CMenu_Editor 525
#define MSG_CMenu_Sort 526
#define MSG_CMenu_Validate 527
#define MSG_CMenu_OpenAll 528
#define MSG_CMenu_CloseAll 529
#define MSG_CMenu_AddGroupHere 530
#define MSG_CMenu_AddFeedHere 531
#define MSG_CMenu_AddQuickEMail 532
#define MSG_CMenu_Bookmark 533
#define MSG_CMenu_DownloadFavIcon 534
#define MSG_CMenu_Title_Feed 535
#define MSG_CMenu_Title_Group 536
#define MSG_CMenu_Title_List 537
#define MSG_CMenu_Title_New 538
#define MSG_CMenu_Title_TheBar 539
#define MSG_CMenu_Title_HTML 540
#define MSG_CMenu_Title_Edit 541
#define MSG_CMenu_ImBrowser 542
#define MSG_Prefs_Win_Title 600
#define MSG_Prefs_Title_Options 601
#define MSG_Prefs_Title_Locale 602
#define MSG_Prefs_Title_HTTP 603
#define MSG_Prefs_Title_Bar 604
#define MSG_Prefs_Title_NBar 605
#define MSG_Prefs_Use 606
#define MSG_Prefs_UseHelp 607
#define MSG_Prefs_Apply 608
#define MSG_Prefs_ApplyHelp 609
#define MSG_Prefs_Cancel 610
#define MSG_Prefs_CancelHelp 611
#define MSG_Prefs_GroupDoubleClick 612
#define MSG_Prefs_GroupDoubleClickHelp 613
#define MSG_Prefs_GroupDoubleClick_Nothing 614
#define MSG_Prefs_GroupDoubleClick_Edit 615
#define MSG_Prefs_GroupDoubleClick_Feed 616
#define MSG_Prefs_EntriesDoubleClick 617
#define MSG_Prefs_EntriesDoubleClickHelp 618
#define MSG_Prefs_EntriesDoubleClick_Nothing 619
#define MSG_Prefs_EntriesDoubleClick_Browser 620
#define MSG_Prefs_EntriesDoubleClick_GoToLink 621
#define MSG_Prefs_DescrClick 622
#define MSG_Prefs_DescrClickHelp 623
#define MSG_Prefs_DescrClick_Nothing 624
#define MSG_Prefs_DescrClick_Browser 625
#define MSG_Prefs_DescrClick_GoToLink 626
#define MSG_Prefs_Editor 627
#define MSG_Prefs_EditorHelp 628
#define MSG_Prefs_UserItem 629
#define MSG_Prefs_UserItemHelp 630
#define MSG_Prefs_UserItemName 631
#define MSG_Prefs_UserItemNameHelp 632
#define MSG_Prefs_NoListBar 633
#define MSG_Prefs_NoListBarHelp 634
#define MSG_Prefs_BoldTitles 635
#define MSG_Prefs_BoldTitlesHelp 636
#define MSG_Prefs_SubstDate 637
#define MSG_Prefs_SubstDateHelp 638
#define MSG_Prefs_RightMouse 639
#define MSG_Prefs_RightMouseHelp 640
#define MSG_Prefs_BoldGroups 641
#define MSG_Prefs_BoldGroupsHelp 642
#define MSG_Prefs_Codeset 643
#define MSG_Prefs_CodesetHelp 644
#define MSG_Prefs_GMTOffset 645
#define MSG_Prefs_GMTOffsetHelp 646
#define MSG_Prefs_GetFromSystem 647
#define MSG_Prefs_GetFromSystemHelp 648
#define MSG_Prefs_Proxy 649
#define MSG_Prefs_ProxyHelp 650
#define MSG_Prefs_ProxyPort 651
#define MSG_Prefs_ProxyPortHelp 652
#define MSG_Prefs_UseProxy 653
#define MSG_Prefs_UseProxyHelp 654
#define MSG_Prefs_Agent 655
#define MSG_Prefs_AgentHelp 656
#define MSG_Prefs_BPos 657
#define MSG_Prefs_BPosHelp 658
#define MSG_Prefs_BPos_Top 659
#define MSG_Prefs_BPos_Bottom 660
#define MSG_Prefs_BPos_Left 661
#define MSG_Prefs_BPos_Right 662
#define MSG_Prefs_BPos_InnerTop 663
#define MSG_Prefs_BPos_InnerBottom 664
#define MSG_Prefs_Layout 665
#define MSG_Prefs_LayoutHelp 666
#define MSG_Prefs_Layout_Up 667
#define MSG_Prefs_Layout_Center 668
#define MSG_Prefs_Layout_Down 669
#define MSG_Prefs_LPos 670
#define MSG_Prefs_LPosHelp 671
#define MSG_Prefs_LPos_Bottom 672
#define MSG_Prefs_LPos_Top 673
#define MSG_Prefs_LPos_Right 674
#define MSG_Prefs_LPos_Left 675
#define MSG_Prefs_ViewMode 676
#define MSG_Prefs_ViewModeHelp 677
#define MSG_Prefs_ViewMode_TextGfx 678
#define MSG_Prefs_ViewMode_Text 679
#define MSG_Prefs_ViewMode_Gfx 680
#define MSG_Prefs_Borderless 681
#define MSG_Prefs_BorderlessHelp 682
#define MSG_Prefs_Sunny 683
#define MSG_Prefs_SunnyHelp 684
#define MSG_Prefs_Raised 685
#define MSG_Prefs_RaisedHelp 686
#define MSG_Prefs_Scaled 687
#define MSG_Prefs_ScaledHelp 688
#define MSG_Prefs_Underscore 689
#define MSG_Prefs_UnderscoreHelp 690
#define MSG_Prefs_Frame 691
#define MSG_Prefs_FrameHelp 692
#define MSG_Prefs_BarSpacer 693
#define MSG_Prefs_BarSpacerHelp 694
#define MSG_Prefs_DragBar 695
#define MSG_Prefs_DragBarHelp 696
#define MSG_Prefs_ShowFeedImage 697
#define MSG_Prefs_ShowFeedImageHelp 698
#define MSG_Prefs_SobstituteWinChars 699
#define MSG_Prefs_SobstituteWinCharsHelp 700
#define MSG_Prefs_UseFeedsListImages 701
#define MSG_Prefs_UseFeedsListImagesHelp 702
#define MSG_Prefs_UseDefaultGroupImage 703
#define MSG_Prefs_UseDefaultGroupImageHelp 704
#define MSG_Prefs_MaxImageSize 705
#define MSG_Prefs_MaxImageSizeHelp 706
#define MSG_Prefs_ShowGroupNews 707
#define MSG_Prefs_ShowGroupNewsHelp 708
#define MSG_Prefs_NTLoadImages 709
#define MSG_Prefs_NTLoadImagesHelp 710
#define MSG_Prefs_UseHandMouse 711
#define MSG_Prefs_UseHandMouseHelp 712
#define MSG_Prefs_Popph_File 713
#define MSG_Prefs_Popph_Screen 714
#define MSG_Prefs_Popph_Link 715
#define MSG_Group_LocalFeed 800
#define MSG_Group_NoLocalFeed 801
#define MSG_Group_Welcome 802
#define MSG_EditFeed_WinTitleEditFeed 900
#define MSG_EditFeed_WinTitleEditGroup 901
#define MSG_EditFeed_WinTitleNewFeed 902
#define MSG_EditFeed_WinTitleNewGroup 903
#define MSG_EditFeed_URL 904
#define MSG_EditFeed_URLHelp 905
#define MSG_EditFeed_Name 906
#define MSG_EditFeed_NameHelp 907
#define MSG_EditFeed_File 908
#define MSG_EditFeed_FileHelp 909
#define Msg_EditFeed_OK 910
#define Msg_EditFeed_OKHelp 911
#define Msg_EditFeed_Cancel 912
#define Msg_EditFeed_CancelHelp 913
#define MSG_EditFeed_Pic 914
#define MSG_EditFeed_PicHelp 915
#define MSG_EditFeed_PicReqTitle 916
#define MSG_EditFeed_ShowLink 917
#define MSG_EditFeed_ShowLinkHelp 918
#define MSG_EditFeed_Update 919
#define MSG_EditFeed_UpdateHelp 920
#define MSG_EditFeed_Update_Never 921
#define MSG_EditFeed_Update_Min 922
#define MSG_Title_Headline 1100
#define MSG_Title_Date 1101
#define MSG_Title_Subject 1102
#define MSG_Title_Author 1103
#define MSG_SearchWin_Title 1200
#define MSG_SearchWin_Pattern 1201
#define MSG_SearchWin_PatternHelp 1202
#define MSG_SearchWin_Case 1203
#define MSG_SearchWin_CaseHelp 1204
#define MSG_SearchWin_Welcome 1205
#define MSG_SearchWin_First 1206
#define MSG_SearchWin_FirstHelp 1207
#define MSG_SearchWin_Succ 1208
#define MSG_SearchWin_SuccHelp 1209
#define MSG_SearchWin_Back 1210
#define MSG_SearchWin_BackHelp 1211
#define MSG_SearchWin_Cancel 1212
#define MSG_SearchWin_CancelHelp 1213
#define MSG_SearchWin_Searching 1214
#define MSG_SearchWin_Found 1215
#define MSG_SearchWin_NotFound 1216
#define MSG_CM_Win_Title 1400
#define MSG_MiniMail_Win_Title 1500

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define MSG_Error_ReqTitle_STR "amrss error"
#define MSG_Error_ReqGadget_STR "*_OK"
#define MSG_Error_NoMem_STR "Not enogh memory available."
#define MSG_Error_NoLocale_STR "Can't open locale.library ver 37 or higher:\namrss needs MorphOS 1.45 or higher."
#define MSG_Error_NoMUIMaster_STR "Can't open muimaster.library ver 20.5341 or higher:\ndid you think such a jewel could be based on\nsomething else than MUI 4?!?"
#define MSG_Error_NoVTK_STR "[Available error]"
#define MSG_Error_NoUtility_STR "Can't open utility.library ver 37 or higher:\namrss needs MorphOS 1.45 or higher."
#define MSG_Error_NoGfx_STR "Can't open graphics.library ver 37 or higher:\namrss needs MorphOS 1.45 or higher."
#define MSG_Error_DataTypes_STR "Can't open datatypes.library ver 37 or higher:\namrss needs MorphOS 1.45 or higher."
#define MSG_Error_NoIFFParse_STR "Can't open iffparse.library ver 37 or higher:\namrss needs MorphOS 1.45 or higher."
#define MSG_Error_NoRexxSysBase_STR "Can't open rexxsyslib.library ver 36 or higher:\nplease, install ARexx in your system."
#define MSG_Error_Icon_STR "Can't open icon.library ver 37 or higher:\namrss needs MorphOS 1.45 or higher."
#define MSG_Error_NoCodesets_STR "Can't open codesets.library ver 4 or higher:\nyou didn't use amrss installer, did you?"
#define MSG_Error_Expat_STR "Can't open expat.library ver 4 or higher:\nyou didn't use amrss installer, did you?"
#define MSG_Error_NoLoc_STR "Can't open default locale; please launch your locale\npreferences application and set a default locale."
#define MSG_Error_NoAppClass_STR "Can't create Application.mui subclass (app)."
#define MSG_Error_NoWinClass_STR "Can't create Window.mui subclass (win)."
#define MSG_Error_NoRootScrollgroupClass_STR "Can't create Scrollgroup.mui subclass (rootScrollgroup)."
#define MSG_Error_NoRootStringClass_STR "Can't create Textinput.mcc subclass (rootString)."
#define MSG_Error_NoRootBarClass_STR "Can't create TheBar.mcc subclass (rootBar)."
#define MSG_Error_NoGroupClass_STR "Can't create Group.mui subclass (group)."
#define MSG_Error_NoNewsClass_STR "Can't create List.mcc subclass (entriesList)."
#define MSG_Error_NoGrouplistClass_STR "Can't create NListtree.mcc subclass (grouplist)."
#define MSG_Error_NoHTMLClass_STR "Can't create HTMLview.mcc subclass (HTML)."
#define MSG_Error_NoPicClass_STR "Can't create Area.mui subclass (pic)."
#define MSG_Error_CantCreateDirs_STR "Can't create amrss directories."
#define MSG_Error_NoApp_STR "Can't create application."
#define MSG_Error_NoWin_STR "Can't open amrss window."
#define MSG_Error_NoSocket_STR "Can't create a TCP socket."
#define MSG_Error_NoHost_STR "Can't resolve host."
#define MSG_Error_CantConnect_STR "Can't connet to host."
#define MSG_Error_Send_STR "Error sending."
#define MSG_Error_Recv_STR "Error receiving."
#define MSG_Error_ProtoError_STR "Protocol error."
#define MSG_Error_ServerError_STR "Server problem"
#define MSG_Error_InvalidURL_STR "URL format not valid."
#define MSG_Error_Aborted_STR "Interrupted."
#define MSG_Error_NoSocketBase_STR "Can't open bsdsocket.library ver 4 or higher."
#define MSG_Group_UnknownEncoding_STR "Unknown encoding %s"
#define MSG_Group_CantParseXML_STR "Can't parse XML data (error %ld in line %ld: %s)"
#define MSG_Group_Date_DSHMS_STR "Date: %1$s %2$s (%3$ld days, %4$ldh, %5$ldm and %6$lds ago)  -  News: %7$ld"
#define MSG_Group_Date_DHMS_STR "Date: %1$s %2$s (%3$ld day, %4$ldh, %5$ldm and %6$lds ago)  -  News: %7$ld"
#define MSG_Group_Date_HMS_STR "Date: %1$s %2$s (%3$ldh, %4$ldm and %5$lds ago)  -  News: %6$ld"
#define MSG_Group_Date_Now_STR "Date: %1$s %2$s  -  News: %3$ld"
#define MSG_Status_ResolvingHost_STR "Resolving host..."
#define MSG_Status_Connecting_STR "Connecting to host..."
#define MSG_Status_Sending_STR "Sending request..."
#define MSG_Status_Receiving_STR "Receiving..."
#define MSG_Status_Done_STR "Done."
#define MSG_Status_ReadingFeed_STR "Reading feed..."
#define MSG_Status_Updating_STR "Updating feeds..."
#define MSG_Status_Updated_STR "Feeds updated."
#define MSG_Status_ReadingFeeds_STR "Reading feeds..."
#define MSG_Status_ReadingGroup_STR "Reading group..."
#define MSG_Status_GroupRead_STR "Group read."
#define MSG_App_Copyright_STR "Copyright 2004-2005 by Alfonso Ranieri"
#define MSG_App_Description_STR "Amiga RSS client by Alfonso Ranieri"
#define MSG_Menu_Project_STR "Project"
#define MSG_Menu_About_STR "?\0About..."
#define MSG_Menu_AboutMUI_STR "!\0About MUI..."
#define MSG_Menu_Hide_STR "H\0Hide"
#define MSG_Menu_Quit_STR "Q\0Quit"
#define MSG_Menu_Feeds_STR "Feeds"
#define MSG_Menu_AddGroup_STR "G\0Add group..."
#define MSG_Menu_AddFeed_STR "A\0Add feed..."
#define MSG_Menu_EditFeed_STR "E\0Edit..."
#define MSG_Menu_Remove_STR "R\0Remove"
#define MSG_Menu_LastSaved_STR "L\0Last saved"
#define MSG_Menu_NoDragDrop_STR "T\0No drag & drop"
#define MSG_Menu_EditFeeds_STR "W\0View feeds..."
#define MSG_Menu_Settings_STR "Settings"
#define MSG_Menu_EditPrefs_STR "P\0Edit..."
#define MSG_Menu_LastSavedPrefs_STR "D\0Last saved"
#define MSG_Menu_Snapshot_STR "B\0Snapshot window"
#define MSG_Menu_UnSnapshot_STR "N\0Unsnapshot window"
#define MSG_Menu_Help_STR "1\0Help"
#define MSG_Menu_MUI_STR "M\0MUI..."
#define MSG_Menu_QuickQuit_STR "Z\0Quick quit"
#define MSG_Menu_NoAutoUpdate_STR "J\0No Auto update"
#define MSG_Menu_UpdateAll_STR "U\0Update"
#define MSG_Menu_FlushCookies_STR "8\0Flush cookies"
#define MSG_Menu_FlushImages_STR "7\0Flush images"
#define MSG_Menu_HideGroups_STR "5\0Hide feeds"
#define MSG_Menu_ScreenMode_STR "9\0ScreenMode..."
#define MSG_Menu_SaveFeeds_STR "F\0Save feeds"
#define MSG_Menu_HideBar_STR "4\0Hide bar"
#define MSG_Menu_HidePreview_STR "6\0Hide preview"
#define MSG_Menu_NewsPage_STR "2\0News Page"
#define MSG_Menu_HTMLPage_STR "3\0HTML Page"
#define MSG_Win_Title_STR "amrss (%s)"
#define MSG_Gad_Add_STR "_Add"
#define MSG_Gad_AddHelp_STR "Add a new feed."
#define MSG_Gad_Edit_STR "_Edit"
#define MSG_Gad_EditHelp_STR "Edit the selected entry."
#define MSG_Gad_Remove_STR "_Remove"
#define MSG_Gad_RemoveHelp_STR "Remove the selected entry."
#define MSG_Gad_Feed_STR "_Update"
#define MSG_Gad_FeedHelp_STR "Update the selected feed."
#define MSG_Gad_Stop_STR "_Stop"
#define MSG_Gad_StopHelp_STR "Stop active connections."
#define MSG_Gad_Search_STR "Sear_ch"
#define MSG_Gad_SearchHelp_STR "Open the search window."
#define MSG_Gad_Main_STR "_News"
#define MSG_Gad_MainHelp_STR "Switch to news page."
#define MSG_Gad_HTML_STR "_HTML"
#define MSG_Gad_HTMLHelp_STR "Switch to HTML page."
#define MSG_Gad_Prefs_STR "_Prefs"
#define MSG_Gad_PrefsHelp_STR "Open the preferences window."
#define MSG_Gad_Back_STR "_Back"
#define MSG_Gad_BackHelp_STR "Go to the previous visited page."
#define MSG_Gad_Next_STR "S_ucc"
#define MSG_Gad_NextHelp_STR "Go to the next visited page."
#define MSG_Gad_Reload_STR "Re_load"
#define MSG_Gad_ReloadHelp_STR "Reload the current page."
#define MSG_Gad_URL_STR "_Location"
#define MSG_Gad_URLHelp_STR "The URL to reach."
#define MSG_Gad_AddGroup_STR "Add g_roup"
#define MSG_Gad_AddGroupHelp_STR "Add a new group."
#define MSG_Screen_Title_STR "amrss (%s)"
#define MSG_Win_TitleFeed_STR "amrss (%s) - %s"
#define MSG_Screen_TitleFeed_STR "amrss (%s) - %s"
#define MSG_CMenu_Configure_STR "Settings..."
#define MSG_CMenu_DeleteAll_STR "Delete all"
#define MSG_CMenu_Cut_STR "Cut"
#define MSG_CMenu_Copy_STR "Copy"
#define MSG_CMenu_Paste_STR "Paste"
#define MSG_CMenu_ViewMode_TextGfx_STR "Icons and text"
#define MSG_CMenu_ViewMode_Gfx_STR "Icons only"
#define MSG_CMenu_ViewMode_Text_STR "Text only"
#define MSG_CMenu_Borderless_STR "Borderless"
#define MSG_CMenu_Sunny_STR "Highlight"
#define MSG_CMenu_Raised_STR "Raised"
#define MSG_CMenu_AddGroup_STR "Add group..."
#define MSG_CMenu_AddFeed_STR "Add feed..."
#define MSG_CMenu_Edit_STR "Edit..."
#define MSG_CMenu_Link_STR "Go to link"
#define MSG_CMenu_DefWidthThis_STR "Default Width: this"
#define MSG_CMenu_DefWidthAll_STR "Default Width: all"
#define MSG_CMenu_DefOrderThis_STR "Default Order: this"
#define MSG_CMenu_DefOrderAll_STR "Default Order: all"
#define MSG_CMenu_CopyTitle_STR "Copy title to clip"
#define MSG_CMenu_CopyLink_STR "Copy link to clip"
#define MSG_CMenu_Browser_STR "Open with OpenURL..."
#define MSG_CMenu_SetAsDefault_STR "Default feed"
#define MSG_CMenu_Feed_STR "Update"
#define MSG_CMenu_Remove_STR "Remove"
#define MSG_CMenu_Editor_STR "View source..."
#define MSG_CMenu_Sort_STR "Sort"
#define MSG_CMenu_Validate_STR "Validate"
#define MSG_CMenu_OpenAll_STR "Open all"
#define MSG_CMenu_CloseAll_STR "Close all"
#define MSG_CMenu_AddGroupHere_STR "Add group here..."
#define MSG_CMenu_AddFeedHere_STR "Add feed here..."
#define MSG_CMenu_AddQuickEMail_STR "Send a quick email"
#define MSG_CMenu_Bookmark_STR "Add to CManager"
#define MSG_CMenu_DownloadFavIcon_STR "Download favicon"
#define MSG_CMenu_Title_Feed_STR "Feed"
#define MSG_CMenu_Title_Group_STR "Group"
#define MSG_CMenu_Title_List_STR "List"
#define MSG_CMenu_Title_New_STR "New"
#define MSG_CMenu_Title_TheBar_STR "TheBar"
#define MSG_CMenu_Title_HTML_STR "HTML"
#define MSG_CMenu_Title_Edit_STR "Edit"
#define MSG_CMenu_ImBrowser_STR "Open image with OpenURL..."
#define MSG_Prefs_Win_Title_STR "amrss preferences"
#define MSG_Prefs_Title_Options_STR "Options"
#define MSG_Prefs_Title_Locale_STR "Locale"
#define MSG_Prefs_Title_HTTP_STR "HTTP"
#define MSG_Prefs_Title_Bar_STR "Buttons bar"
#define MSG_Prefs_Title_NBar_STR "Navigation bar"
#define MSG_Prefs_Use_STR "_Use"
#define MSG_Prefs_UseHelp_STR "Use preferences and close the window."
#define MSG_Prefs_Apply_STR "_Apply"
#define MSG_Prefs_ApplyHelp_STR "Use preferences without closing the window."
#define MSG_Prefs_Cancel_STR "_Cancel"
#define MSG_Prefs_CancelHelp_STR "Close the window."
#define MSG_Prefs_GroupDoubleClick_STR "_On group double click"
#define MSG_Prefs_GroupDoubleClickHelp_STR "What to do on a double clicks on a feed."
#define MSG_Prefs_GroupDoubleClick_Nothing_STR "Do nothing"
#define MSG_Prefs_GroupDoubleClick_Edit_STR "Edit"
#define MSG_Prefs_GroupDoubleClick_Feed_STR "Feed"
#define MSG_Prefs_EntriesDoubleClick_STR "O_n news double click"
#define MSG_Prefs_EntriesDoubleClickHelp_STR "What to do on a double clicks on a new."
#define MSG_Prefs_EntriesDoubleClick_Nothing_STR "Do nothing"
#define MSG_Prefs_EntriesDoubleClick_Browser_STR "Open with OpenURL"
#define MSG_Prefs_EntriesDoubleClick_GoToLink_STR "Go to link"
#define MSG_Prefs_DescrClick_STR "On lin_k click"
#define MSG_Prefs_DescrClickHelp_STR "What to do on a clicks on a link."
#define MSG_Prefs_DescrClick_Nothing_STR "Do nothing"
#define MSG_Prefs_DescrClick_Browser_STR "Open with OpenURL"
#define MSG_Prefs_DescrClick_GoToLink_STR "Go to link"
#define MSG_Prefs_Editor_STR "_Viewer"
#define MSG_Prefs_EditorHelp_STR "The command to use to view files."
#define MSG_Prefs_UserItem_STR "U_ser item"
#define MSG_Prefs_UserItemHelp_STR "An item to add to the HTML context\nmenus. It will be enabled only on\nlinks."
#define MSG_Prefs_UserItemName_STR "_Item name"
#define MSG_Prefs_UserItemNameHelp_STR "The name for the above item."
#define MSG_Prefs_NoListBar_STR "_Don't use columns bars"
#define MSG_Prefs_NoListBarHelp_STR "If selected, columns bars are not shown."
#define MSG_Prefs_BoldTitles_STR "_Bold list titles"
#define MSG_Prefs_BoldTitlesHelp_STR "If selected, list titles are rendered bold."
#define MSG_Prefs_SubstDate_STR "Subs_titute \"today\", ..."
#define MSG_Prefs_SubstDateHelp_STR "Substitute \"today\", ... in dates."
#define MSG_Prefs_RightMouse_STR "_Right mouse changes active"
#define MSG_Prefs_RightMouseHelp_STR "If selected, right mouse button\nchanges active entry."
#define MSG_Prefs_BoldGroups_STR "Bo_ld groups"
#define MSG_Prefs_BoldGroupsHelp_STR "If selected, groups are rendered bold."
#define MSG_Prefs_Codeset_STR "C_odeset"
#define MSG_Prefs_CodesetHelp_STR "The codeset to use."
#define MSG_Prefs_GMTOffset_STR "_GMT Offset"
#define MSG_Prefs_GMTOffsetHelp_STR "Your offset from the Greenwich time."
#define MSG_Prefs_GetFromSystem_STR "G_et from system"
#define MSG_Prefs_GetFromSystemHelp_STR "Try to obtain information from the system."
#define MSG_Prefs_Proxy_STR "_Proxy"
#define MSG_Prefs_ProxyHelp_STR "An host address to use as proxy."
#define MSG_Prefs_ProxyPort_STR "P_ort"
#define MSG_Prefs_ProxyPortHelp_STR "The port of the proxy."
#define MSG_Prefs_UseProxy_STR "U_se proxy"
#define MSG_Prefs_UseProxyHelp_STR "If selected, the proxy is used."
#define MSG_Prefs_Agent_STR "A_gent"
#define MSG_Prefs_AgentHelp_STR "The User-Agent string to use."
#define MSG_Prefs_BPos_STR "_Bar position"
#define MSG_Prefs_BPosHelp_STR "Adjust the position of the buttons bar."
#define MSG_Prefs_BPos_Top_STR "Top"
#define MSG_Prefs_BPos_Bottom_STR "Bottom"
#define MSG_Prefs_BPos_Left_STR "Left"
#define MSG_Prefs_BPos_Right_STR "Right"
#define MSG_Prefs_BPos_InnerTop_STR "Inner top"
#define MSG_Prefs_BPos_InnerBottom_STR "Inner bottom"
#define MSG_Prefs_Layout_STR "Bu_ttons position"
#define MSG_Prefs_LayoutHelp_STR "Adjust the position of the buttons."
#define MSG_Prefs_Layout_Up_STR "Left (Top)"
#define MSG_Prefs_Layout_Center_STR "Center"
#define MSG_Prefs_Layout_Down_STR "Right (Down)"
#define MSG_Prefs_LPos_STR "_Label position"
#define MSG_Prefs_LPosHelp_STR "Adjust the position of the text\nfor \"Icons and text\" buttons."
#define MSG_Prefs_LPos_Bottom_STR "Bottom"
#define MSG_Prefs_LPos_Top_STR "Top"
#define MSG_Prefs_LPos_Right_STR "Right"
#define MSG_Prefs_LPos_Left_STR "Left"
#define MSG_Prefs_ViewMode_STR "A_ppareance"
#define MSG_Prefs_ViewModeHelp_STR "Adjust the appearance of the buttons bar."
#define MSG_Prefs_ViewMode_TextGfx_STR "Icons and text"
#define MSG_Prefs_ViewMode_Text_STR "Text only"
#define MSG_Prefs_ViewMode_Gfx_STR "Icons only"
#define MSG_Prefs_Borderless_STR "B_orderless"
#define MSG_Prefs_BorderlessHelp_STR "If selected, buttons are borderless."
#define MSG_Prefs_Sunny_STR "_Highlight"
#define MSG_Prefs_SunnyHelp_STR "If selected, buttons, normally in black\nand white, are colored when the\nmouse is over them."
#define MSG_Prefs_Raised_STR "_Raised"
#define MSG_Prefs_RaisedHelp_STR "If selected, buttons, normally frameless,\nare framed when the mouse is over them."
#define MSG_Prefs_Scaled_STR "Scale_d"
#define MSG_Prefs_ScaledHelp_STR "If selected, buttons are scaled\naccording to MUI settings."
#define MSG_Prefs_Underscore_STR "_Short cuts"
#define MSG_Prefs_UnderscoreHelp_STR "If selected, buttons use\nkeyboard short-cuts."
#define MSG_Prefs_Frame_STR "Butto_ns bar frame"
#define MSG_Prefs_FrameHelp_STR "If selected, no frame is drawn\naround the buttons."
#define MSG_Prefs_BarSpacer_STR "Bar spac_er"
#define MSG_Prefs_BarSpacerHelp_STR "If selected, a bar spacer is used\nas buttons separator."
#define MSG_Prefs_DragBar_STR "Dra_ggable buttons bar"
#define MSG_Prefs_DragBarHelp_STR "If selected, no buttons dragbar\nis available."
#define MSG_Prefs_ShowFeedImage_STR "Sho_w feeds images"
#define MSG_Prefs_ShowFeedImageHelp_STR "If selected, feeds images are download\nand shown."
#define MSG_Prefs_SobstituteWinChars_STR "Sobstitut_e Windows chars"
#define MSG_Prefs_SobstituteWinCharsHelp_STR "If selected, some Windows charsets\ncodes are translated to Amiga ones.\nNote that changes will be applied\nonly at next local or remote news\nloading."
#define MSG_Prefs_UseFeedsListImages_STR "Use _feeds and groups images"
#define MSG_Prefs_UseFeedsListImagesHelp_STR "If selected, feeds and groups images are used."
#define MSG_Prefs_UseDefaultGroupImage_STR "Use default _group image"
#define MSG_Prefs_UseDefaultGroupImageHelp_STR "If selected, the default group image is\nused for groups with no image defined."
#define MSG_Prefs_MaxImageSize_STR "_Max image size"
#define MSG_Prefs_MaxImageSizeHelp_STR "Max image width and height."
#define MSG_Prefs_ShowGroupNews_STR "Show grou_ps news"
#define MSG_Prefs_ShowGroupNewsHelp_STR "If selected, when a group is activated\nits feeds news are shown."
#define MSG_Prefs_NTLoadImages_STR "Don't _load images"
#define MSG_Prefs_NTLoadImagesHelp_STR "If selected, images are not load in\nHTML documents."
#define MSG_Prefs_UseHandMouse_STR "Use _hand mouse"
#define MSG_Prefs_UseHandMouseHelp_STR "If selected, the hand mouse\nis used on links."
#define MSG_Prefs_Popph_File_STR "File name"
#define MSG_Prefs_Popph_Screen_STR "Public screen name"
#define MSG_Prefs_Popph_Link_STR "Link"
#define MSG_Group_LocalFeed_STR "%s news read."
#define MSG_Group_NoLocalFeed_STR "%s has no news."
#define MSG_Group_Welcome_STR "Select a feed."
#define MSG_EditFeed_WinTitleEditFeed_STR "Edit feed"
#define MSG_EditFeed_WinTitleEditGroup_STR "Edit group"
#define MSG_EditFeed_WinTitleNewFeed_STR "Edit new feed"
#define MSG_EditFeed_WinTitleNewGroup_STR "Edit new group"
#define MSG_EditFeed_URL_STR "U_RL"
#define MSG_EditFeed_URLHelp_STR "The URL of the feed."
#define MSG_EditFeed_Name_STR "_Name"
#define MSG_EditFeed_NameHelp_STR "The name of the feed."
#define MSG_EditFeed_File_STR "File"
#define MSG_EditFeed_FileHelp_STR "The file where are stored feed's news."
#define Msg_EditFeed_OK_STR "_Use"
#define Msg_EditFeed_OKHelp_STR "Save changes."
#define Msg_EditFeed_Cancel_STR "_Cancel"
#define Msg_EditFeed_CancelHelp_STR "Exit without saving."
#define MSG_EditFeed_Pic_STR "_Image"
#define MSG_EditFeed_PicHelp_STR "A complete path to an image file\nto be used as entry image."
#define MSG_EditFeed_PicReqTitle_STR "Select an image"
#define MSG_EditFeed_ShowLink_STR "_Show link"
#define MSG_EditFeed_ShowLinkHelp_STR "If selected, the link of the new is shown\nrather than the description contents."
#define MSG_EditFeed_Update_STR "U_pdate"
#define MSG_EditFeed_UpdateHelp_STR "The feed is updated any this amount of minutes."
#define MSG_EditFeed_Update_Never_STR "Never"
#define MSG_EditFeed_Update_Min_STR "\33c any %ld mins\40"
#define MSG_Title_Headline_STR "Headline"
#define MSG_Title_Date_STR "Date"
#define MSG_Title_Subject_STR "Subject"
#define MSG_Title_Author_STR "Author"
#define MSG_SearchWin_Title_STR "Search"
#define MSG_SearchWin_Pattern_STR "S_tring"
#define MSG_SearchWin_PatternHelp_STR "The string to search for."
#define MSG_SearchWin_Case_STR "C_ase"
#define MSG_SearchWin_CaseHelp_STR "If selected, searching is case sensitive."
#define MSG_SearchWin_Welcome_STR "Search news."
#define MSG_SearchWin_First_STR "_Start"
#define MSG_SearchWin_FirstHelp_STR "Search first match."
#define MSG_SearchWin_Succ_STR "S_ucc"
#define MSG_SearchWin_SuccHelp_STR "Search next match."
#define MSG_SearchWin_Back_STR "_Back"
#define MSG_SearchWin_BackHelp_STR "Search previous match."
#define MSG_SearchWin_Cancel_STR "_Cancel"
#define MSG_SearchWin_CancelHelp_STR "Close search window."
#define MSG_SearchWin_Searching_STR "Searching..."
#define MSG_SearchWin_Found_STR "Found."
#define MSG_SearchWin_NotFound_STR "Not found."
#define MSG_CM_Win_Title_STR "amrss cm bookmarks"
#define MSG_MiniMail_Win_Title_STR "amrss mini mailer"

#endif /* CATCOMP_STRINGS */


/****************************************************************************/


#ifdef CATCOMP_ARRAY

struct CatCompArrayType
{
    LONG   cca_ID;
    STRPTR cca_Str;
};

static const struct CatCompArrayType CatCompArray[] =
{
    {MSG_Error_ReqTitle,(STRPTR)MSG_Error_ReqTitle_STR},
    {MSG_Error_ReqGadget,(STRPTR)MSG_Error_ReqGadget_STR},
    {MSG_Error_NoMem,(STRPTR)MSG_Error_NoMem_STR},
    {MSG_Error_NoLocale,(STRPTR)MSG_Error_NoLocale_STR},
    {MSG_Error_NoMUIMaster,(STRPTR)MSG_Error_NoMUIMaster_STR},
    {MSG_Error_NoVTK,(STRPTR)MSG_Error_NoVTK_STR},
    {MSG_Error_NoUtility,(STRPTR)MSG_Error_NoUtility_STR},
    {MSG_Error_NoGfx,(STRPTR)MSG_Error_NoGfx_STR},
    {MSG_Error_DataTypes,(STRPTR)MSG_Error_DataTypes_STR},
    {MSG_Error_NoIFFParse,(STRPTR)MSG_Error_NoIFFParse_STR},
    {MSG_Error_NoRexxSysBase,(STRPTR)MSG_Error_NoRexxSysBase_STR},
    {MSG_Error_Icon,(STRPTR)MSG_Error_Icon_STR},
    {MSG_Error_NoCodesets,(STRPTR)MSG_Error_NoCodesets_STR},
    {MSG_Error_Expat,(STRPTR)MSG_Error_Expat_STR},
    {MSG_Error_NoLoc,(STRPTR)MSG_Error_NoLoc_STR},
    {MSG_Error_NoAppClass,(STRPTR)MSG_Error_NoAppClass_STR},
    {MSG_Error_NoWinClass,(STRPTR)MSG_Error_NoWinClass_STR},
    {MSG_Error_NoRootScrollgroupClass,(STRPTR)MSG_Error_NoRootScrollgroupClass_STR},
    {MSG_Error_NoRootStringClass,(STRPTR)MSG_Error_NoRootStringClass_STR},
    {MSG_Error_NoRootBarClass,(STRPTR)MSG_Error_NoRootBarClass_STR},
    {MSG_Error_NoGroupClass,(STRPTR)MSG_Error_NoGroupClass_STR},
    {MSG_Error_NoNewsClass,(STRPTR)MSG_Error_NoNewsClass_STR},
    {MSG_Error_NoGrouplistClass,(STRPTR)MSG_Error_NoGrouplistClass_STR},
    {MSG_Error_NoHTMLClass,(STRPTR)MSG_Error_NoHTMLClass_STR},
    {MSG_Error_NoPicClass,(STRPTR)MSG_Error_NoPicClass_STR},
    {MSG_Error_CantCreateDirs,(STRPTR)MSG_Error_CantCreateDirs_STR},
    {MSG_Error_NoApp,(STRPTR)MSG_Error_NoApp_STR},
    {MSG_Error_NoWin,(STRPTR)MSG_Error_NoWin_STR},
    {MSG_Error_NoSocket,(STRPTR)MSG_Error_NoSocket_STR},
    {MSG_Error_NoHost,(STRPTR)MSG_Error_NoHost_STR},
    {MSG_Error_CantConnect,(STRPTR)MSG_Error_CantConnect_STR},
    {MSG_Error_Send,(STRPTR)MSG_Error_Send_STR},
    {MSG_Error_Recv,(STRPTR)MSG_Error_Recv_STR},
    {MSG_Error_ProtoError,(STRPTR)MSG_Error_ProtoError_STR},
    {MSG_Error_ServerError,(STRPTR)MSG_Error_ServerError_STR},
    {MSG_Error_InvalidURL,(STRPTR)MSG_Error_InvalidURL_STR},
    {MSG_Error_Aborted,(STRPTR)MSG_Error_Aborted_STR},
    {MSG_Error_NoSocketBase,(STRPTR)MSG_Error_NoSocketBase_STR},
    {MSG_Group_UnknownEncoding,(STRPTR)MSG_Group_UnknownEncoding_STR},
    {MSG_Group_CantParseXML,(STRPTR)MSG_Group_CantParseXML_STR},
    {MSG_Group_Date_DSHMS,(STRPTR)MSG_Group_Date_DSHMS_STR},
    {MSG_Group_Date_DHMS,(STRPTR)MSG_Group_Date_DHMS_STR},
    {MSG_Group_Date_HMS,(STRPTR)MSG_Group_Date_HMS_STR},
    {MSG_Group_Date_Now,(STRPTR)MSG_Group_Date_Now_STR},
    {MSG_Status_ResolvingHost,(STRPTR)MSG_Status_ResolvingHost_STR},
    {MSG_Status_Connecting,(STRPTR)MSG_Status_Connecting_STR},
    {MSG_Status_Sending,(STRPTR)MSG_Status_Sending_STR},
    {MSG_Status_Receiving,(STRPTR)MSG_Status_Receiving_STR},
    {MSG_Status_Done,(STRPTR)MSG_Status_Done_STR},
    {MSG_Status_ReadingFeed,(STRPTR)MSG_Status_ReadingFeed_STR},
    {MSG_Status_Updating,(STRPTR)MSG_Status_Updating_STR},
    {MSG_Status_Updated,(STRPTR)MSG_Status_Updated_STR},
    {MSG_Status_ReadingFeeds,(STRPTR)MSG_Status_ReadingFeeds_STR},
    {MSG_Status_ReadingGroup,(STRPTR)MSG_Status_ReadingGroup_STR},
    {MSG_Status_GroupRead,(STRPTR)MSG_Status_GroupRead_STR},
    {MSG_App_Copyright,(STRPTR)MSG_App_Copyright_STR},
    {MSG_App_Description,(STRPTR)MSG_App_Description_STR},
    {MSG_Menu_Project,(STRPTR)MSG_Menu_Project_STR},
    {MSG_Menu_About,(STRPTR)MSG_Menu_About_STR},
    {MSG_Menu_AboutMUI,(STRPTR)MSG_Menu_AboutMUI_STR},
    {MSG_Menu_Hide,(STRPTR)MSG_Menu_Hide_STR},
    {MSG_Menu_Quit,(STRPTR)MSG_Menu_Quit_STR},
    {MSG_Menu_Feeds,(STRPTR)MSG_Menu_Feeds_STR},
    {MSG_Menu_AddGroup,(STRPTR)MSG_Menu_AddGroup_STR},
    {MSG_Menu_AddFeed,(STRPTR)MSG_Menu_AddFeed_STR},
    {MSG_Menu_EditFeed,(STRPTR)MSG_Menu_EditFeed_STR},
    {MSG_Menu_Remove,(STRPTR)MSG_Menu_Remove_STR},
    {MSG_Menu_LastSaved,(STRPTR)MSG_Menu_LastSaved_STR},
    {MSG_Menu_NoDragDrop,(STRPTR)MSG_Menu_NoDragDrop_STR},
    {MSG_Menu_EditFeeds,(STRPTR)MSG_Menu_EditFeeds_STR},
    {MSG_Menu_Settings,(STRPTR)MSG_Menu_Settings_STR},
    {MSG_Menu_EditPrefs,(STRPTR)MSG_Menu_EditPrefs_STR},
    {MSG_Menu_LastSavedPrefs,(STRPTR)MSG_Menu_LastSavedPrefs_STR},
    {MSG_Menu_Snapshot,(STRPTR)MSG_Menu_Snapshot_STR},
    {MSG_Menu_UnSnapshot,(STRPTR)MSG_Menu_UnSnapshot_STR},
    {MSG_Menu_Help,(STRPTR)MSG_Menu_Help_STR},
    {MSG_Menu_MUI,(STRPTR)MSG_Menu_MUI_STR},
    {MSG_Menu_QuickQuit,(STRPTR)MSG_Menu_QuickQuit_STR},
    {MSG_Menu_NoAutoUpdate,(STRPTR)MSG_Menu_NoAutoUpdate_STR},
    {MSG_Menu_UpdateAll,(STRPTR)MSG_Menu_UpdateAll_STR},
    {MSG_Menu_FlushCookies,(STRPTR)MSG_Menu_FlushCookies_STR},
    {MSG_Menu_FlushImages,(STRPTR)MSG_Menu_FlushImages_STR},
    {MSG_Menu_HideGroups,(STRPTR)MSG_Menu_HideGroups_STR},
    {MSG_Menu_ScreenMode,(STRPTR)MSG_Menu_ScreenMode_STR},
    {MSG_Menu_SaveFeeds,(STRPTR)MSG_Menu_SaveFeeds_STR},
    {MSG_Menu_HideBar,(STRPTR)MSG_Menu_HideBar_STR},
    {MSG_Menu_HidePreview,(STRPTR)MSG_Menu_HidePreview_STR},
    {MSG_Menu_NewsPage,(STRPTR)MSG_Menu_NewsPage_STR},
    {MSG_Menu_HTMLPage,(STRPTR)MSG_Menu_HTMLPage_STR},
    {MSG_Win_Title,(STRPTR)MSG_Win_Title_STR},
    {MSG_Gad_Add,(STRPTR)MSG_Gad_Add_STR},
    {MSG_Gad_AddHelp,(STRPTR)MSG_Gad_AddHelp_STR},
    {MSG_Gad_Edit,(STRPTR)MSG_Gad_Edit_STR},
    {MSG_Gad_EditHelp,(STRPTR)MSG_Gad_EditHelp_STR},
    {MSG_Gad_Remove,(STRPTR)MSG_Gad_Remove_STR},
    {MSG_Gad_RemoveHelp,(STRPTR)MSG_Gad_RemoveHelp_STR},
    {MSG_Gad_Feed,(STRPTR)MSG_Gad_Feed_STR},
    {MSG_Gad_FeedHelp,(STRPTR)MSG_Gad_FeedHelp_STR},
    {MSG_Gad_Stop,(STRPTR)MSG_Gad_Stop_STR},
    {MSG_Gad_StopHelp,(STRPTR)MSG_Gad_StopHelp_STR},
    {MSG_Gad_Search,(STRPTR)MSG_Gad_Search_STR},
    {MSG_Gad_SearchHelp,(STRPTR)MSG_Gad_SearchHelp_STR},
    {MSG_Gad_Main,(STRPTR)MSG_Gad_Main_STR},
    {MSG_Gad_MainHelp,(STRPTR)MSG_Gad_MainHelp_STR},
    {MSG_Gad_HTML,(STRPTR)MSG_Gad_HTML_STR},
    {MSG_Gad_HTMLHelp,(STRPTR)MSG_Gad_HTMLHelp_STR},
    {MSG_Gad_Prefs,(STRPTR)MSG_Gad_Prefs_STR},
    {MSG_Gad_PrefsHelp,(STRPTR)MSG_Gad_PrefsHelp_STR},
    {MSG_Gad_Back,(STRPTR)MSG_Gad_Back_STR},
    {MSG_Gad_BackHelp,(STRPTR)MSG_Gad_BackHelp_STR},
    {MSG_Gad_Next,(STRPTR)MSG_Gad_Next_STR},
    {MSG_Gad_NextHelp,(STRPTR)MSG_Gad_NextHelp_STR},
    {MSG_Gad_Reload,(STRPTR)MSG_Gad_Reload_STR},
    {MSG_Gad_ReloadHelp,(STRPTR)MSG_Gad_ReloadHelp_STR},
    {MSG_Gad_URL,(STRPTR)MSG_Gad_URL_STR},
    {MSG_Gad_URLHelp,(STRPTR)MSG_Gad_URLHelp_STR},
    {MSG_Gad_AddGroup,(STRPTR)MSG_Gad_AddGroup_STR},
    {MSG_Gad_AddGroupHelp,(STRPTR)MSG_Gad_AddGroupHelp_STR},
    {MSG_Screen_Title,(STRPTR)MSG_Screen_Title_STR},
    {MSG_Win_TitleFeed,(STRPTR)MSG_Win_TitleFeed_STR},
    {MSG_Screen_TitleFeed,(STRPTR)MSG_Screen_TitleFeed_STR},
    {MSG_CMenu_Configure,(STRPTR)MSG_CMenu_Configure_STR},
    {MSG_CMenu_DeleteAll,(STRPTR)MSG_CMenu_DeleteAll_STR},
    {MSG_CMenu_Cut,(STRPTR)MSG_CMenu_Cut_STR},
    {MSG_CMenu_Copy,(STRPTR)MSG_CMenu_Copy_STR},
    {MSG_CMenu_Paste,(STRPTR)MSG_CMenu_Paste_STR},
    {MSG_CMenu_ViewMode_TextGfx,(STRPTR)MSG_CMenu_ViewMode_TextGfx_STR},
    {MSG_CMenu_ViewMode_Gfx,(STRPTR)MSG_CMenu_ViewMode_Gfx_STR},
    {MSG_CMenu_ViewMode_Text,(STRPTR)MSG_CMenu_ViewMode_Text_STR},
    {MSG_CMenu_Borderless,(STRPTR)MSG_CMenu_Borderless_STR},
    {MSG_CMenu_Sunny,(STRPTR)MSG_CMenu_Sunny_STR},
    {MSG_CMenu_Raised,(STRPTR)MSG_CMenu_Raised_STR},
    {MSG_CMenu_AddGroup,(STRPTR)MSG_CMenu_AddGroup_STR},
    {MSG_CMenu_AddFeed,(STRPTR)MSG_CMenu_AddFeed_STR},
    {MSG_CMenu_Edit,(STRPTR)MSG_CMenu_Edit_STR},
    {MSG_CMenu_Link,(STRPTR)MSG_CMenu_Link_STR},
    {MSG_CMenu_DefWidthThis,(STRPTR)MSG_CMenu_DefWidthThis_STR},
    {MSG_CMenu_DefWidthAll,(STRPTR)MSG_CMenu_DefWidthAll_STR},
    {MSG_CMenu_DefOrderThis,(STRPTR)MSG_CMenu_DefOrderThis_STR},
    {MSG_CMenu_DefOrderAll,(STRPTR)MSG_CMenu_DefOrderAll_STR},
    {MSG_CMenu_CopyTitle,(STRPTR)MSG_CMenu_CopyTitle_STR},
    {MSG_CMenu_CopyLink,(STRPTR)MSG_CMenu_CopyLink_STR},
    {MSG_CMenu_Browser,(STRPTR)MSG_CMenu_Browser_STR},
    {MSG_CMenu_SetAsDefault,(STRPTR)MSG_CMenu_SetAsDefault_STR},
    {MSG_CMenu_Feed,(STRPTR)MSG_CMenu_Feed_STR},
    {MSG_CMenu_Remove,(STRPTR)MSG_CMenu_Remove_STR},
    {MSG_CMenu_Editor,(STRPTR)MSG_CMenu_Editor_STR},
    {MSG_CMenu_Sort,(STRPTR)MSG_CMenu_Sort_STR},
    {MSG_CMenu_Validate,(STRPTR)MSG_CMenu_Validate_STR},
    {MSG_CMenu_OpenAll,(STRPTR)MSG_CMenu_OpenAll_STR},
    {MSG_CMenu_CloseAll,(STRPTR)MSG_CMenu_CloseAll_STR},
    {MSG_CMenu_AddGroupHere,(STRPTR)MSG_CMenu_AddGroupHere_STR},
    {MSG_CMenu_AddFeedHere,(STRPTR)MSG_CMenu_AddFeedHere_STR},
    {MSG_CMenu_AddQuickEMail,(STRPTR)MSG_CMenu_AddQuickEMail_STR},
    {MSG_CMenu_Bookmark,(STRPTR)MSG_CMenu_Bookmark_STR},
    {MSG_CMenu_DownloadFavIcon,(STRPTR)MSG_CMenu_DownloadFavIcon_STR},
    {MSG_CMenu_Title_Feed,(STRPTR)MSG_CMenu_Title_Feed_STR},
    {MSG_CMenu_Title_Group,(STRPTR)MSG_CMenu_Title_Group_STR},
    {MSG_CMenu_Title_List,(STRPTR)MSG_CMenu_Title_List_STR},
    {MSG_CMenu_Title_New,(STRPTR)MSG_CMenu_Title_New_STR},
    {MSG_CMenu_Title_TheBar,(STRPTR)MSG_CMenu_Title_TheBar_STR},
    {MSG_CMenu_Title_HTML,(STRPTR)MSG_CMenu_Title_HTML_STR},
    {MSG_CMenu_Title_Edit,(STRPTR)MSG_CMenu_Title_Edit_STR},
    {MSG_CMenu_ImBrowser,(STRPTR)MSG_CMenu_ImBrowser_STR},
    {MSG_Prefs_Win_Title,(STRPTR)MSG_Prefs_Win_Title_STR},
    {MSG_Prefs_Title_Options,(STRPTR)MSG_Prefs_Title_Options_STR},
    {MSG_Prefs_Title_Locale,(STRPTR)MSG_Prefs_Title_Locale_STR},
    {MSG_Prefs_Title_HTTP,(STRPTR)MSG_Prefs_Title_HTTP_STR},
    {MSG_Prefs_Title_Bar,(STRPTR)MSG_Prefs_Title_Bar_STR},
    {MSG_Prefs_Title_NBar,(STRPTR)MSG_Prefs_Title_NBar_STR},
    {MSG_Prefs_Use,(STRPTR)MSG_Prefs_Use_STR},
    {MSG_Prefs_UseHelp,(STRPTR)MSG_Prefs_UseHelp_STR},
    {MSG_Prefs_Apply,(STRPTR)MSG_Prefs_Apply_STR},
    {MSG_Prefs_ApplyHelp,(STRPTR)MSG_Prefs_ApplyHelp_STR},
    {MSG_Prefs_Cancel,(STRPTR)MSG_Prefs_Cancel_STR},
    {MSG_Prefs_CancelHelp,(STRPTR)MSG_Prefs_CancelHelp_STR},
    {MSG_Prefs_GroupDoubleClick,(STRPTR)MSG_Prefs_GroupDoubleClick_STR},
    {MSG_Prefs_GroupDoubleClickHelp,(STRPTR)MSG_Prefs_GroupDoubleClickHelp_STR},
    {MSG_Prefs_GroupDoubleClick_Nothing,(STRPTR)MSG_Prefs_GroupDoubleClick_Nothing_STR},
    {MSG_Prefs_GroupDoubleClick_Edit,(STRPTR)MSG_Prefs_GroupDoubleClick_Edit_STR},
    {MSG_Prefs_GroupDoubleClick_Feed,(STRPTR)MSG_Prefs_GroupDoubleClick_Feed_STR},
    {MSG_Prefs_EntriesDoubleClick,(STRPTR)MSG_Prefs_EntriesDoubleClick_STR},
    {MSG_Prefs_EntriesDoubleClickHelp,(STRPTR)MSG_Prefs_EntriesDoubleClickHelp_STR},
    {MSG_Prefs_EntriesDoubleClick_Nothing,(STRPTR)MSG_Prefs_EntriesDoubleClick_Nothing_STR},
    {MSG_Prefs_EntriesDoubleClick_Browser,(STRPTR)MSG_Prefs_EntriesDoubleClick_Browser_STR},
    {MSG_Prefs_EntriesDoubleClick_GoToLink,(STRPTR)MSG_Prefs_EntriesDoubleClick_GoToLink_STR},
    {MSG_Prefs_DescrClick,(STRPTR)MSG_Prefs_DescrClick_STR},
    {MSG_Prefs_DescrClickHelp,(STRPTR)MSG_Prefs_DescrClickHelp_STR},
    {MSG_Prefs_DescrClick_Nothing,(STRPTR)MSG_Prefs_DescrClick_Nothing_STR},
    {MSG_Prefs_DescrClick_Browser,(STRPTR)MSG_Prefs_DescrClick_Browser_STR},
    {MSG_Prefs_DescrClick_GoToLink,(STRPTR)MSG_Prefs_DescrClick_GoToLink_STR},
    {MSG_Prefs_Editor,(STRPTR)MSG_Prefs_Editor_STR},
    {MSG_Prefs_EditorHelp,(STRPTR)MSG_Prefs_EditorHelp_STR},
    {MSG_Prefs_UserItem,(STRPTR)MSG_Prefs_UserItem_STR},
    {MSG_Prefs_UserItemHelp,(STRPTR)MSG_Prefs_UserItemHelp_STR},
    {MSG_Prefs_UserItemName,(STRPTR)MSG_Prefs_UserItemName_STR},
    {MSG_Prefs_UserItemNameHelp,(STRPTR)MSG_Prefs_UserItemNameHelp_STR},
    {MSG_Prefs_NoListBar,(STRPTR)MSG_Prefs_NoListBar_STR},
    {MSG_Prefs_NoListBarHelp,(STRPTR)MSG_Prefs_NoListBarHelp_STR},
    {MSG_Prefs_BoldTitles,(STRPTR)MSG_Prefs_BoldTitles_STR},
    {MSG_Prefs_BoldTitlesHelp,(STRPTR)MSG_Prefs_BoldTitlesHelp_STR},
    {MSG_Prefs_SubstDate,(STRPTR)MSG_Prefs_SubstDate_STR},
    {MSG_Prefs_SubstDateHelp,(STRPTR)MSG_Prefs_SubstDateHelp_STR},
    {MSG_Prefs_RightMouse,(STRPTR)MSG_Prefs_RightMouse_STR},
    {MSG_Prefs_RightMouseHelp,(STRPTR)MSG_Prefs_RightMouseHelp_STR},
    {MSG_Prefs_BoldGroups,(STRPTR)MSG_Prefs_BoldGroups_STR},
    {MSG_Prefs_BoldGroupsHelp,(STRPTR)MSG_Prefs_BoldGroupsHelp_STR},
    {MSG_Prefs_Codeset,(STRPTR)MSG_Prefs_Codeset_STR},
    {MSG_Prefs_CodesetHelp,(STRPTR)MSG_Prefs_CodesetHelp_STR},
    {MSG_Prefs_GMTOffset,(STRPTR)MSG_Prefs_GMTOffset_STR},
    {MSG_Prefs_GMTOffsetHelp,(STRPTR)MSG_Prefs_GMTOffsetHelp_STR},
    {MSG_Prefs_GetFromSystem,(STRPTR)MSG_Prefs_GetFromSystem_STR},
    {MSG_Prefs_GetFromSystemHelp,(STRPTR)MSG_Prefs_GetFromSystemHelp_STR},
    {MSG_Prefs_Proxy,(STRPTR)MSG_Prefs_Proxy_STR},
    {MSG_Prefs_ProxyHelp,(STRPTR)MSG_Prefs_ProxyHelp_STR},
    {MSG_Prefs_ProxyPort,(STRPTR)MSG_Prefs_ProxyPort_STR},
    {MSG_Prefs_ProxyPortHelp,(STRPTR)MSG_Prefs_ProxyPortHelp_STR},
    {MSG_Prefs_UseProxy,(STRPTR)MSG_Prefs_UseProxy_STR},
    {MSG_Prefs_UseProxyHelp,(STRPTR)MSG_Prefs_UseProxyHelp_STR},
    {MSG_Prefs_Agent,(STRPTR)MSG_Prefs_Agent_STR},
    {MSG_Prefs_AgentHelp,(STRPTR)MSG_Prefs_AgentHelp_STR},
    {MSG_Prefs_BPos,(STRPTR)MSG_Prefs_BPos_STR},
    {MSG_Prefs_BPosHelp,(STRPTR)MSG_Prefs_BPosHelp_STR},
    {MSG_Prefs_BPos_Top,(STRPTR)MSG_Prefs_BPos_Top_STR},
    {MSG_Prefs_BPos_Bottom,(STRPTR)MSG_Prefs_BPos_Bottom_STR},
    {MSG_Prefs_BPos_Left,(STRPTR)MSG_Prefs_BPos_Left_STR},
    {MSG_Prefs_BPos_Right,(STRPTR)MSG_Prefs_BPos_Right_STR},
    {MSG_Prefs_BPos_InnerTop,(STRPTR)MSG_Prefs_BPos_InnerTop_STR},
    {MSG_Prefs_BPos_InnerBottom,(STRPTR)MSG_Prefs_BPos_InnerBottom_STR},
    {MSG_Prefs_Layout,(STRPTR)MSG_Prefs_Layout_STR},
    {MSG_Prefs_LayoutHelp,(STRPTR)MSG_Prefs_LayoutHelp_STR},
    {MSG_Prefs_Layout_Up,(STRPTR)MSG_Prefs_Layout_Up_STR},
    {MSG_Prefs_Layout_Center,(STRPTR)MSG_Prefs_Layout_Center_STR},
    {MSG_Prefs_Layout_Down,(STRPTR)MSG_Prefs_Layout_Down_STR},
    {MSG_Prefs_LPos,(STRPTR)MSG_Prefs_LPos_STR},
    {MSG_Prefs_LPosHelp,(STRPTR)MSG_Prefs_LPosHelp_STR},
    {MSG_Prefs_LPos_Bottom,(STRPTR)MSG_Prefs_LPos_Bottom_STR},
    {MSG_Prefs_LPos_Top,(STRPTR)MSG_Prefs_LPos_Top_STR},
    {MSG_Prefs_LPos_Right,(STRPTR)MSG_Prefs_LPos_Right_STR},
    {MSG_Prefs_LPos_Left,(STRPTR)MSG_Prefs_LPos_Left_STR},
    {MSG_Prefs_ViewMode,(STRPTR)MSG_Prefs_ViewMode_STR},
    {MSG_Prefs_ViewModeHelp,(STRPTR)MSG_Prefs_ViewModeHelp_STR},
    {MSG_Prefs_ViewMode_TextGfx,(STRPTR)MSG_Prefs_ViewMode_TextGfx_STR},
    {MSG_Prefs_ViewMode_Text,(STRPTR)MSG_Prefs_ViewMode_Text_STR},
    {MSG_Prefs_ViewMode_Gfx,(STRPTR)MSG_Prefs_ViewMode_Gfx_STR},
    {MSG_Prefs_Borderless,(STRPTR)MSG_Prefs_Borderless_STR},
    {MSG_Prefs_BorderlessHelp,(STRPTR)MSG_Prefs_BorderlessHelp_STR},
    {MSG_Prefs_Sunny,(STRPTR)MSG_Prefs_Sunny_STR},
    {MSG_Prefs_SunnyHelp,(STRPTR)MSG_Prefs_SunnyHelp_STR},
    {MSG_Prefs_Raised,(STRPTR)MSG_Prefs_Raised_STR},
    {MSG_Prefs_RaisedHelp,(STRPTR)MSG_Prefs_RaisedHelp_STR},
    {MSG_Prefs_Scaled,(STRPTR)MSG_Prefs_Scaled_STR},
    {MSG_Prefs_ScaledHelp,(STRPTR)MSG_Prefs_ScaledHelp_STR},
    {MSG_Prefs_Underscore,(STRPTR)MSG_Prefs_Underscore_STR},
    {MSG_Prefs_UnderscoreHelp,(STRPTR)MSG_Prefs_UnderscoreHelp_STR},
    {MSG_Prefs_Frame,(STRPTR)MSG_Prefs_Frame_STR},
    {MSG_Prefs_FrameHelp,(STRPTR)MSG_Prefs_FrameHelp_STR},
    {MSG_Prefs_BarSpacer,(STRPTR)MSG_Prefs_BarSpacer_STR},
    {MSG_Prefs_BarSpacerHelp,(STRPTR)MSG_Prefs_BarSpacerHelp_STR},
    {MSG_Prefs_DragBar,(STRPTR)MSG_Prefs_DragBar_STR},
    {MSG_Prefs_DragBarHelp,(STRPTR)MSG_Prefs_DragBarHelp_STR},
    {MSG_Prefs_ShowFeedImage,(STRPTR)MSG_Prefs_ShowFeedImage_STR},
    {MSG_Prefs_ShowFeedImageHelp,(STRPTR)MSG_Prefs_ShowFeedImageHelp_STR},
    {MSG_Prefs_SobstituteWinChars,(STRPTR)MSG_Prefs_SobstituteWinChars_STR},
    {MSG_Prefs_SobstituteWinCharsHelp,(STRPTR)MSG_Prefs_SobstituteWinCharsHelp_STR},
    {MSG_Prefs_UseFeedsListImages,(STRPTR)MSG_Prefs_UseFeedsListImages_STR},
    {MSG_Prefs_UseFeedsListImagesHelp,(STRPTR)MSG_Prefs_UseFeedsListImagesHelp_STR},
    {MSG_Prefs_UseDefaultGroupImage,(STRPTR)MSG_Prefs_UseDefaultGroupImage_STR},
    {MSG_Prefs_UseDefaultGroupImageHelp,(STRPTR)MSG_Prefs_UseDefaultGroupImageHelp_STR},
    {MSG_Prefs_MaxImageSize,(STRPTR)MSG_Prefs_MaxImageSize_STR},
    {MSG_Prefs_MaxImageSizeHelp,(STRPTR)MSG_Prefs_MaxImageSizeHelp_STR},
    {MSG_Prefs_ShowGroupNews,(STRPTR)MSG_Prefs_ShowGroupNews_STR},
    {MSG_Prefs_ShowGroupNewsHelp,(STRPTR)MSG_Prefs_ShowGroupNewsHelp_STR},
    {MSG_Prefs_NTLoadImages,(STRPTR)MSG_Prefs_NTLoadImages_STR},
    {MSG_Prefs_NTLoadImagesHelp,(STRPTR)MSG_Prefs_NTLoadImagesHelp_STR},
    {MSG_Prefs_UseHandMouse,(STRPTR)MSG_Prefs_UseHandMouse_STR},
    {MSG_Prefs_UseHandMouseHelp,(STRPTR)MSG_Prefs_UseHandMouseHelp_STR},
    {MSG_Prefs_Popph_File,(STRPTR)MSG_Prefs_Popph_File_STR},
    {MSG_Prefs_Popph_Screen,(STRPTR)MSG_Prefs_Popph_Screen_STR},
    {MSG_Prefs_Popph_Link,(STRPTR)MSG_Prefs_Popph_Link_STR},
    {MSG_Group_LocalFeed,(STRPTR)MSG_Group_LocalFeed_STR},
    {MSG_Group_NoLocalFeed,(STRPTR)MSG_Group_NoLocalFeed_STR},
    {MSG_Group_Welcome,(STRPTR)MSG_Group_Welcome_STR},
    {MSG_EditFeed_WinTitleEditFeed,(STRPTR)MSG_EditFeed_WinTitleEditFeed_STR},
    {MSG_EditFeed_WinTitleEditGroup,(STRPTR)MSG_EditFeed_WinTitleEditGroup_STR},
    {MSG_EditFeed_WinTitleNewFeed,(STRPTR)MSG_EditFeed_WinTitleNewFeed_STR},
    {MSG_EditFeed_WinTitleNewGroup,(STRPTR)MSG_EditFeed_WinTitleNewGroup_STR},
    {MSG_EditFeed_URL,(STRPTR)MSG_EditFeed_URL_STR},
    {MSG_EditFeed_URLHelp,(STRPTR)MSG_EditFeed_URLHelp_STR},
    {MSG_EditFeed_Name,(STRPTR)MSG_EditFeed_Name_STR},
    {MSG_EditFeed_NameHelp,(STRPTR)MSG_EditFeed_NameHelp_STR},
    {MSG_EditFeed_File,(STRPTR)MSG_EditFeed_File_STR},
    {MSG_EditFeed_FileHelp,(STRPTR)MSG_EditFeed_FileHelp_STR},
    {Msg_EditFeed_OK,(STRPTR)Msg_EditFeed_OK_STR},
    {Msg_EditFeed_OKHelp,(STRPTR)Msg_EditFeed_OKHelp_STR},
    {Msg_EditFeed_Cancel,(STRPTR)Msg_EditFeed_Cancel_STR},
    {Msg_EditFeed_CancelHelp,(STRPTR)Msg_EditFeed_CancelHelp_STR},
    {MSG_EditFeed_Pic,(STRPTR)MSG_EditFeed_Pic_STR},
    {MSG_EditFeed_PicHelp,(STRPTR)MSG_EditFeed_PicHelp_STR},
    {MSG_EditFeed_PicReqTitle,(STRPTR)MSG_EditFeed_PicReqTitle_STR},
    {MSG_EditFeed_ShowLink,(STRPTR)MSG_EditFeed_ShowLink_STR},
    {MSG_EditFeed_ShowLinkHelp,(STRPTR)MSG_EditFeed_ShowLinkHelp_STR},
    {MSG_EditFeed_Update,(STRPTR)MSG_EditFeed_Update_STR},
    {MSG_EditFeed_UpdateHelp,(STRPTR)MSG_EditFeed_UpdateHelp_STR},
    {MSG_EditFeed_Update_Never,(STRPTR)MSG_EditFeed_Update_Never_STR},
    {MSG_EditFeed_Update_Min,(STRPTR)MSG_EditFeed_Update_Min_STR},
    {MSG_Title_Headline,(STRPTR)MSG_Title_Headline_STR},
    {MSG_Title_Date,(STRPTR)MSG_Title_Date_STR},
    {MSG_Title_Subject,(STRPTR)MSG_Title_Subject_STR},
    {MSG_Title_Author,(STRPTR)MSG_Title_Author_STR},
    {MSG_SearchWin_Title,(STRPTR)MSG_SearchWin_Title_STR},
    {MSG_SearchWin_Pattern,(STRPTR)MSG_SearchWin_Pattern_STR},
    {MSG_SearchWin_PatternHelp,(STRPTR)MSG_SearchWin_PatternHelp_STR},
    {MSG_SearchWin_Case,(STRPTR)MSG_SearchWin_Case_STR},
    {MSG_SearchWin_CaseHelp,(STRPTR)MSG_SearchWin_CaseHelp_STR},
    {MSG_SearchWin_Welcome,(STRPTR)MSG_SearchWin_Welcome_STR},
    {MSG_SearchWin_First,(STRPTR)MSG_SearchWin_First_STR},
    {MSG_SearchWin_FirstHelp,(STRPTR)MSG_SearchWin_FirstHelp_STR},
    {MSG_SearchWin_Succ,(STRPTR)MSG_SearchWin_Succ_STR},
    {MSG_SearchWin_SuccHelp,(STRPTR)MSG_SearchWin_SuccHelp_STR},
    {MSG_SearchWin_Back,(STRPTR)MSG_SearchWin_Back_STR},
    {MSG_SearchWin_BackHelp,(STRPTR)MSG_SearchWin_BackHelp_STR},
    {MSG_SearchWin_Cancel,(STRPTR)MSG_SearchWin_Cancel_STR},
    {MSG_SearchWin_CancelHelp,(STRPTR)MSG_SearchWin_CancelHelp_STR},
    {MSG_SearchWin_Searching,(STRPTR)MSG_SearchWin_Searching_STR},
    {MSG_SearchWin_Found,(STRPTR)MSG_SearchWin_Found_STR},
    {MSG_SearchWin_NotFound,(STRPTR)MSG_SearchWin_NotFound_STR},
    {MSG_CM_Win_Title,(STRPTR)MSG_CM_Win_Title_STR},
    {MSG_MiniMail_Win_Title,(STRPTR)MSG_MiniMail_Win_Title_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/


#ifdef CATCOMP_BLOCK

static const char CatCompBlock[] =
{
    "\x00\x00\x00\x01\x00\x0C"
    MSG_Error_ReqTitle_STR "\x00"
    "\x00\x00\x00\x02\x00\x06"
    MSG_Error_ReqGadget_STR "\x00\x00"
    "\x00\x00\x00\x03\x00\x1C"
    MSG_Error_NoMem_STR "\x00"
    "\x00\x00\x00\x04\x00\x50"
    MSG_Error_NoLocale_STR "\x00"
    "\x00\x00\x00\x05\x00\x7E"
    MSG_Error_NoMUIMaster_STR "\x00"
    "\x00\x00\x00\x06\x00\x12"
    MSG_Error_NoVTK_STR "\x00"
    "\x00\x00\x00\x07\x00\x52"
    MSG_Error_NoUtility_STR "\x00\x00"
    "\x00\x00\x00\x08\x00\x52"
    MSG_Error_NoGfx_STR "\x00"
    "\x00\x00\x00\x09\x00\x54"
    MSG_Error_DataTypes_STR "\x00\x00"
    "\x00\x00\x00\x0A\x00\x52"
    MSG_Error_NoIFFParse_STR "\x00"
    "\x00\x00\x00\x0B\x00\x56"
    MSG_Error_NoRexxSysBase_STR "\x00"
    "\x00\x00\x00\x0C\x00\x4E"
    MSG_Error_Icon_STR "\x00"
    "\x00\x00\x00\x0D\x00\x56"
    MSG_Error_NoCodesets_STR "\x00"
    "\x00\x00\x00\x0E\x00\x54"
    MSG_Error_Expat_STR "\x00\x00"
    "\x00\x00\x00\x0F\x00\x68"
    MSG_Error_NoLoc_STR "\x00\x00"
    "\x00\x00\x00\x10\x00\x2E"
    MSG_Error_NoAppClass_STR "\x00\x00"
    "\x00\x00\x00\x11\x00\x28"
    MSG_Error_NoWinClass_STR "\x00"
    "\x00\x00\x00\x12\x00\x3A"
    MSG_Error_NoRootScrollgroupClass_STR "\x00\x00"
    "\x00\x00\x00\x13\x00\x32"
    MSG_Error_NoRootStringClass_STR "\x00"
    "\x00\x00\x00\x14\x00\x2C"
    MSG_Error_NoRootBarClass_STR "\x00"
    "\x00\x00\x00\x15\x00\x2A"
    MSG_Error_NoGroupClass_STR "\x00\x00"
    "\x00\x00\x00\x16\x00\x2E"
    MSG_Error_NoNewsClass_STR "\x00"
    "\x00\x00\x00\x17\x00\x32"
    MSG_Error_NoGrouplistClass_STR "\x00\x00"
    "\x00\x00\x00\x18\x00\x2C"
    MSG_Error_NoHTMLClass_STR "\x00\x00"
    "\x00\x00\x00\x19\x00\x26"
    MSG_Error_NoPicClass_STR "\x00"
    "\x00\x00\x00\x1A\x00\x20"
    MSG_Error_CantCreateDirs_STR "\x00"
    "\x00\x00\x00\x1B\x00\x1A"
    MSG_Error_NoApp_STR "\x00"
    "\x00\x00\x00\x1C\x00\x1A"
    MSG_Error_NoWin_STR "\x00\x00"
    "\x00\x00\x00\x1D\x00\x1C"
    MSG_Error_NoSocket_STR "\x00\x00"
    "\x00\x00\x00\x1E\x00\x14"
    MSG_Error_NoHost_STR "\x00"
    "\x00\x00\x00\x1F\x00\x16"
    MSG_Error_CantConnect_STR "\x00"
    "\x00\x00\x00\x20\x00\x10"
    MSG_Error_Send_STR "\x00\x00"
    "\x00\x00\x00\x21\x00\x12"
    MSG_Error_Recv_STR "\x00\x00"
    "\x00\x00\x00\x22\x00\x10"
    MSG_Error_ProtoError_STR "\x00"
    "\x00\x00\x00\x23\x00\x10"
    MSG_Error_ServerError_STR "\x00\x00"
    "\x00\x00\x00\x24\x00\x16"
    MSG_Error_InvalidURL_STR "\x00"
    "\x00\x00\x00\x25\x00\x0E"
    MSG_Error_Aborted_STR "\x00\x00"
    "\x00\x00\x00\x26\x00\x2E"
    MSG_Error_NoSocketBase_STR "\x00"
    "\x00\x00\x00\x27\x00\x14"
    MSG_Group_UnknownEncoding_STR "\x00"
    "\x00\x00\x00\x28\x00\x32"
    MSG_Group_CantParseXML_STR "\x00\x00"
    "\x00\x00\x00\xC8\x00\x4C"
    MSG_Group_Date_DSHMS_STR "\x00"
    "\x00\x00\x00\xC9\x00\x4C"
    MSG_Group_Date_DHMS_STR "\x00\x00"
    "\x00\x00\x00\xCA\x00\x40"
    MSG_Group_Date_HMS_STR "\x00"
    "\x00\x00\x00\xCB\x00\x20"
    MSG_Group_Date_Now_STR "\x00"
    "\x00\x00\x00\xCC\x00\x12"
    MSG_Status_ResolvingHost_STR "\x00"
    "\x00\x00\x00\xCD\x00\x16"
    MSG_Status_Connecting_STR "\x00"
    "\x00\x00\x00\xCE\x00\x14"
    MSG_Status_Sending_STR "\x00\x00"
    "\x00\x00\x00\xCF\x00\x0E"
    MSG_Status_Receiving_STR "\x00\x00"
    "\x00\x00\x00\xD0\x00\x06"
    MSG_Status_Done_STR "\x00"
    "\x00\x00\x00\xD1\x00\x10"
    MSG_Status_ReadingFeed_STR "\x00"
    "\x00\x00\x00\xD2\x00\x12"
    MSG_Status_Updating_STR "\x00"
    "\x00\x00\x00\xD3\x00\x10"
    MSG_Status_Updated_STR "\x00\x00"
    "\x00\x00\x00\xD4\x00\x12"
    MSG_Status_ReadingFeeds_STR "\x00\x00"
    "\x00\x00\x00\xD5\x00\x12"
    MSG_Status_ReadingGroup_STR "\x00\x00"
    "\x00\x00\x00\xD6\x00\x0C"
    MSG_Status_GroupRead_STR "\x00"
    "\x00\x00\x01\x2C\x00\x28"
    MSG_App_Copyright_STR "\x00\x00"
    "\x00\x00\x01\x2D\x00\x24"
    MSG_App_Description_STR "\x00"
    "\x00\x00\x01\x2E\x00\x08"
    MSG_Menu_Project_STR "\x00"
    "\x00\x00\x01\x2F\x00\x0C"
    MSG_Menu_About_STR "\x00\x00"
    "\x00\x00\x01\x30\x00\x10"
    MSG_Menu_AboutMUI_STR "\x00\x00"
    "\x00\x00\x01\x31\x00\x08"
    MSG_Menu_Hide_STR "\x00\x00"
    "\x00\x00\x01\x32\x00\x08"
    MSG_Menu_Quit_STR "\x00\x00"
    "\x00\x00\x01\x33\x00\x06"
    MSG_Menu_Feeds_STR "\x00"
    "\x00\x00\x01\x34\x00\x10"
    MSG_Menu_AddGroup_STR "\x00\x00"
    "\x00\x00\x01\x35\x00\x0E"
    MSG_Menu_AddFeed_STR "\x00"
    "\x00\x00\x01\x36\x00\x0A"
    MSG_Menu_EditFeed_STR "\x00"
    "\x00\x00\x01\x37\x00\x0A"
    MSG_Menu_Remove_STR "\x00\x00"
    "\x00\x00\x01\x38\x00\x0E"
    MSG_Menu_LastSaved_STR "\x00\x00"
    "\x00\x00\x01\x39\x00\x12"
    MSG_Menu_NoDragDrop_STR "\x00\x00"
    "\x00\x00\x01\x3A\x00\x10"
    MSG_Menu_EditFeeds_STR "\x00"
    "\x00\x00\x01\x3B\x00\x0A"
    MSG_Menu_Settings_STR "\x00\x00"
    "\x00\x00\x01\x3C\x00\x0A"
    MSG_Menu_EditPrefs_STR "\x00"
    "\x00\x00\x01\x3D\x00\x0E"
    MSG_Menu_LastSavedPrefs_STR "\x00\x00"
    "\x00\x00\x01\x3E\x00\x12"
    MSG_Menu_Snapshot_STR "\x00"
    "\x00\x00\x01\x3F\x00\x14"
    MSG_Menu_UnSnapshot_STR "\x00"
    "\x00\x00\x01\x40\x00\x08"
    MSG_Menu_Help_STR "\x00\x00"
    "\x00\x00\x01\x41\x00\x0A"
    MSG_Menu_MUI_STR "\x00\x00"
    "\x00\x00\x01\x42\x00\x0E"
    MSG_Menu_QuickQuit_STR "\x00\x00"
    "\x00\x00\x01\x43\x00\x12"
    MSG_Menu_NoAutoUpdate_STR "\x00\x00"
    "\x00\x00\x01\x44\x00\x0A"
    MSG_Menu_UpdateAll_STR "\x00\x00"
    "\x00\x00\x01\x45\x00\x10"
    MSG_Menu_FlushCookies_STR "\x00"
    "\x00\x00\x01\x46\x00\x10"
    MSG_Menu_FlushImages_STR "\x00\x00"
    "\x00\x00\x01\x47\x00\x0E"
    MSG_Menu_HideGroups_STR "\x00\x00"
    "\x00\x00\x01\x48\x00\x10"
    MSG_Menu_ScreenMode_STR "\x00"
    "\x00\x00\x01\x49\x00\x0E"
    MSG_Menu_SaveFeeds_STR "\x00\x00"
    "\x00\x00\x01\x4A\x00\x0C"
    MSG_Menu_HideBar_STR "\x00\x00"
    "\x00\x00\x01\x4B\x00\x10"
    MSG_Menu_HidePreview_STR "\x00\x00"
    "\x00\x00\x01\x4C\x00\x0C"
    MSG_Menu_NewsPage_STR "\x00"
    "\x00\x00\x01\x4D\x00\x0C"
    MSG_Menu_HTMLPage_STR "\x00"
    "\x00\x00\x01\x90\x00\x0C"
    MSG_Win_Title_STR "\x00\x00"
    "\x00\x00\x01\x91\x00\x06"
    MSG_Gad_Add_STR "\x00\x00"
    "\x00\x00\x01\x92\x00\x10"
    MSG_Gad_AddHelp_STR "\x00"
    "\x00\x00\x01\x93\x00\x06"
    MSG_Gad_Edit_STR "\x00"
    "\x00\x00\x01\x94\x00\x1A"
    MSG_Gad_EditHelp_STR "\x00\x00"
    "\x00\x00\x01\x95\x00\x08"
    MSG_Gad_Remove_STR "\x00"
    "\x00\x00\x01\x96\x00\x1C"
    MSG_Gad_RemoveHelp_STR "\x00\x00"
    "\x00\x00\x01\x97\x00\x08"
    MSG_Gad_Feed_STR "\x00"
    "\x00\x00\x01\x98\x00\x1A"
    MSG_Gad_FeedHelp_STR "\x00"
    "\x00\x00\x01\x99\x00\x06"
    MSG_Gad_Stop_STR "\x00"
    "\x00\x00\x01\x9A\x00\x1A"
    MSG_Gad_StopHelp_STR "\x00\x00"
    "\x00\x00\x01\x9B\x00\x08"
    MSG_Gad_Search_STR "\x00"
    "\x00\x00\x01\x9C\x00\x18"
    MSG_Gad_SearchHelp_STR "\x00"
    "\x00\x00\x01\x9D\x00\x06"
    MSG_Gad_Main_STR "\x00"
    "\x00\x00\x01\x9E\x00\x16"
    MSG_Gad_MainHelp_STR "\x00\x00"
    "\x00\x00\x01\x9F\x00\x06"
    MSG_Gad_HTML_STR "\x00"
    "\x00\x00\x01\xA0\x00\x16"
    MSG_Gad_HTMLHelp_STR "\x00\x00"
    "\x00\x00\x01\xA1\x00\x08"
    MSG_Gad_Prefs_STR "\x00\x00"
    "\x00\x00\x01\xA2\x00\x1E"
    MSG_Gad_PrefsHelp_STR "\x00\x00"
    "\x00\x00\x01\xA3\x00\x06"
    MSG_Gad_Back_STR "\x00"
    "\x00\x00\x01\xA4\x00\x22"
    MSG_Gad_BackHelp_STR "\x00\x00"
    "\x00\x00\x01\xA5\x00\x06"
    MSG_Gad_Next_STR "\x00"
    "\x00\x00\x01\xA6\x00\x1E"
    MSG_Gad_NextHelp_STR "\x00\x00"
    "\x00\x00\x01\xA7\x00\x08"
    MSG_Gad_Reload_STR "\x00"
    "\x00\x00\x01\xA8\x00\x1A"
    MSG_Gad_ReloadHelp_STR "\x00\x00"
    "\x00\x00\x01\xA9\x00\x0A"
    MSG_Gad_URL_STR "\x00"
    "\x00\x00\x01\xAA\x00\x12"
    MSG_Gad_URLHelp_STR "\x00"
    "\x00\x00\x01\xAB\x00\x0C"
    MSG_Gad_AddGroup_STR "\x00\x00"
    "\x00\x00\x01\xAC\x00\x12"
    MSG_Gad_AddGroupHelp_STR "\x00\x00"
    "\x00\x00\x01\xAD\x00\x0C"
    MSG_Screen_Title_STR "\x00\x00"
    "\x00\x00\x01\xAE\x00\x10"
    MSG_Win_TitleFeed_STR "\x00"
    "\x00\x00\x01\xAF\x00\x10"
    MSG_Screen_TitleFeed_STR "\x00"
    "\x00\x00\x01\xF4\x00\x0C"
    MSG_CMenu_Configure_STR "\x00"
    "\x00\x00\x01\xF5\x00\x0C"
    MSG_CMenu_DeleteAll_STR "\x00\x00"
    "\x00\x00\x01\xF6\x00\x04"
    MSG_CMenu_Cut_STR "\x00"
    "\x00\x00\x01\xF7\x00\x06"
    MSG_CMenu_Copy_STR "\x00\x00"
    "\x00\x00\x01\xF8\x00\x06"
    MSG_CMenu_Paste_STR "\x00"
    "\x00\x00\x01\xF9\x00\x10"
    MSG_CMenu_ViewMode_TextGfx_STR "\x00\x00"
    "\x00\x00\x01\xFA\x00\x0C"
    MSG_CMenu_ViewMode_Gfx_STR "\x00\x00"
    "\x00\x00\x01\xFB\x00\x0A"
    MSG_CMenu_ViewMode_Text_STR "\x00"
    "\x00\x00\x01\xFC\x00\x0C"
    MSG_CMenu_Borderless_STR "\x00\x00"
    "\x00\x00\x01\xFD\x00\x0A"
    MSG_CMenu_Sunny_STR "\x00"
    "\x00\x00\x01\xFE\x00\x08"
    MSG_CMenu_Raised_STR "\x00\x00"
    "\x00\x00\x01\xFF\x00\x0E"
    MSG_CMenu_AddGroup_STR "\x00\x00"
    "\x00\x00\x02\x00\x00\x0C"
    MSG_CMenu_AddFeed_STR "\x00"
    "\x00\x00\x02\x01\x00\x08"
    MSG_CMenu_Edit_STR "\x00"
    "\x00\x00\x02\x02\x00\x0C"
    MSG_CMenu_Link_STR "\x00\x00"
    "\x00\x00\x02\x03\x00\x14"
    MSG_CMenu_DefWidthThis_STR "\x00"
    "\x00\x00\x02\x04\x00\x14"
    MSG_CMenu_DefWidthAll_STR "\x00\x00"
    "\x00\x00\x02\x05\x00\x14"
    MSG_CMenu_DefOrderThis_STR "\x00"
    "\x00\x00\x02\x06\x00\x14"
    MSG_CMenu_DefOrderAll_STR "\x00\x00"
    "\x00\x00\x02\x07\x00\x14"
    MSG_CMenu_CopyTitle_STR "\x00\x00"
    "\x00\x00\x02\x08\x00\x12"
    MSG_CMenu_CopyLink_STR "\x00"
    "\x00\x00\x02\x09\x00\x16"
    MSG_CMenu_Browser_STR "\x00\x00"
    "\x00\x00\x02\x0A\x00\x0E"
    MSG_CMenu_SetAsDefault_STR "\x00\x00"
    "\x00\x00\x02\x0B\x00\x08"
    MSG_CMenu_Feed_STR "\x00\x00"
    "\x00\x00\x02\x0C\x00\x08"
    MSG_CMenu_Remove_STR "\x00\x00"
    "\x00\x00\x02\x0D\x00\x10"
    MSG_CMenu_Editor_STR "\x00\x00"
    "\x00\x00\x02\x0E\x00\x06"
    MSG_CMenu_Sort_STR "\x00\x00"
    "\x00\x00\x02\x0F\x00\x0A"
    MSG_CMenu_Validate_STR "\x00\x00"
    "\x00\x00\x02\x10\x00\x0A"
    MSG_CMenu_OpenAll_STR "\x00\x00"
    "\x00\x00\x02\x11\x00\x0A"
    MSG_CMenu_CloseAll_STR "\x00"
    "\x00\x00\x02\x12\x00\x12"
    MSG_CMenu_AddGroupHere_STR "\x00"
    "\x00\x00\x02\x13\x00\x12"
    MSG_CMenu_AddFeedHere_STR "\x00\x00"
    "\x00\x00\x02\x14\x00\x14"
    MSG_CMenu_AddQuickEMail_STR "\x00\x00"
    "\x00\x00\x02\x15\x00\x10"
    MSG_CMenu_Bookmark_STR "\x00"
    "\x00\x00\x02\x16\x00\x12"
    MSG_CMenu_DownloadFavIcon_STR "\x00\x00"
    "\x00\x00\x02\x17\x00\x06"
    MSG_CMenu_Title_Feed_STR "\x00\x00"
    "\x00\x00\x02\x18\x00\x06"
    MSG_CMenu_Title_Group_STR "\x00"
    "\x00\x00\x02\x19\x00\x06"
    MSG_CMenu_Title_List_STR "\x00\x00"
    "\x00\x00\x02\x1A\x00\x04"
    MSG_CMenu_Title_New_STR "\x00"
    "\x00\x00\x02\x1B\x00\x08"
    MSG_CMenu_Title_TheBar_STR "\x00\x00"
    "\x00\x00\x02\x1C\x00\x06"
    MSG_CMenu_Title_HTML_STR "\x00\x00"
    "\x00\x00\x02\x1D\x00\x06"
    MSG_CMenu_Title_Edit_STR "\x00\x00"
    "\x00\x00\x02\x1E\x00\x1C"
    MSG_CMenu_ImBrowser_STR "\x00\x00"
    "\x00\x00\x02\x58\x00\x12"
    MSG_Prefs_Win_Title_STR "\x00"
    "\x00\x00\x02\x59\x00\x08"
    MSG_Prefs_Title_Options_STR "\x00"
    "\x00\x00\x02\x5A\x00\x08"
    MSG_Prefs_Title_Locale_STR "\x00\x00"
    "\x00\x00\x02\x5B\x00\x06"
    MSG_Prefs_Title_HTTP_STR "\x00\x00"
    "\x00\x00\x02\x5C\x00\x0C"
    MSG_Prefs_Title_Bar_STR "\x00"
    "\x00\x00\x02\x5D\x00\x10"
    MSG_Prefs_Title_NBar_STR "\x00\x00"
    "\x00\x00\x02\x5E\x00\x06"
    MSG_Prefs_Use_STR "\x00\x00"
    "\x00\x00\x02\x5F\x00\x26"
    MSG_Prefs_UseHelp_STR "\x00"
    "\x00\x00\x02\x60\x00\x08"
    MSG_Prefs_Apply_STR "\x00\x00"
    "\x00\x00\x02\x61\x00\x2C"
    MSG_Prefs_ApplyHelp_STR "\x00"
    "\x00\x00\x02\x62\x00\x08"
    MSG_Prefs_Cancel_STR "\x00"
    "\x00\x00\x02\x63\x00\x12"
    MSG_Prefs_CancelHelp_STR "\x00"
    "\x00\x00\x02\x64\x00\x18"
    MSG_Prefs_GroupDoubleClick_STR "\x00\x00"
    "\x00\x00\x02\x65\x00\x2A"
    MSG_Prefs_GroupDoubleClickHelp_STR "\x00\x00"
    "\x00\x00\x02\x66\x00\x0C"
    MSG_Prefs_GroupDoubleClick_Nothing_STR "\x00\x00"
    "\x00\x00\x02\x67\x00\x06"
    MSG_Prefs_GroupDoubleClick_Edit_STR "\x00\x00"
    "\x00\x00\x02\x68\x00\x06"
    MSG_Prefs_GroupDoubleClick_Feed_STR "\x00\x00"
    "\x00\x00\x02\x69\x00\x16"
    MSG_Prefs_EntriesDoubleClick_STR "\x00"
    "\x00\x00\x02\x6A\x00\x28"
    MSG_Prefs_EntriesDoubleClickHelp_STR "\x00"
    "\x00\x00\x02\x6B\x00\x0C"
    MSG_Prefs_EntriesDoubleClick_Nothing_STR "\x00\x00"
    "\x00\x00\x02\x6C\x00\x12"
    MSG_Prefs_EntriesDoubleClick_Browser_STR "\x00"
    "\x00\x00\x02\x6D\x00\x0C"
    MSG_Prefs_EntriesDoubleClick_GoToLink_STR "\x00\x00"
    "\x00\x00\x02\x6E\x00\x10"
    MSG_Prefs_DescrClick_STR "\x00\x00"
    "\x00\x00\x02\x6F\x00\x22"
    MSG_Prefs_DescrClickHelp_STR "\x00"
    "\x00\x00\x02\x70\x00\x0C"
    MSG_Prefs_DescrClick_Nothing_STR "\x00\x00"
    "\x00\x00\x02\x71\x00\x12"
    MSG_Prefs_DescrClick_Browser_STR "\x00"
    "\x00\x00\x02\x72\x00\x0C"
    MSG_Prefs_DescrClick_GoToLink_STR "\x00\x00"
    "\x00\x00\x02\x73\x00\x08"
    MSG_Prefs_Editor_STR "\x00"
    "\x00\x00\x02\x74\x00\x22"
    MSG_Prefs_EditorHelp_STR "\x00"
    "\x00\x00\x02\x75\x00\x0C"
    MSG_Prefs_UserItem_STR "\x00\x00"
    "\x00\x00\x02\x76\x00\x4C"
    MSG_Prefs_UserItemHelp_STR "\x00"
    "\x00\x00\x02\x77\x00\x0C"
    MSG_Prefs_UserItemName_STR "\x00\x00"
    "\x00\x00\x02\x78\x00\x1E"
    MSG_Prefs_UserItemNameHelp_STR "\x00\x00"
    "\x00\x00\x02\x79\x00\x18"
    MSG_Prefs_NoListBar_STR "\x00"
    "\x00\x00\x02\x7A\x00\x2A"
    MSG_Prefs_NoListBarHelp_STR "\x00\x00"
    "\x00\x00\x02\x7B\x00\x12"
    MSG_Prefs_BoldTitles_STR "\x00"
    "\x00\x00\x02\x7C\x00\x2C"
    MSG_Prefs_BoldTitlesHelp_STR "\x00"
    "\x00\x00\x02\x7D\x00\x1A"
    MSG_Prefs_SubstDate_STR "\x00\x00"
    "\x00\x00\x02\x7E\x00\x22"
    MSG_Prefs_SubstDateHelp_STR "\x00"
    "\x00\x00\x02\x7F\x00\x1C"
    MSG_Prefs_RightMouse_STR "\x00"
    "\x00\x00\x02\x80\x00\x36"
    MSG_Prefs_RightMouseHelp_STR "\x00"
    "\x00\x00\x02\x81\x00\x0E"
    MSG_Prefs_BoldGroups_STR "\x00\x00"
    "\x00\x00\x02\x82\x00\x28"
    MSG_Prefs_BoldGroupsHelp_STR "\x00\x00"
    "\x00\x00\x02\x83\x00\x0A"
    MSG_Prefs_Codeset_STR "\x00\x00"
    "\x00\x00\x02\x84\x00\x14"
    MSG_Prefs_CodesetHelp_STR "\x00"
    "\x00\x00\x02\x85\x00\x0C"
    MSG_Prefs_GMTOffset_STR "\x00"
    "\x00\x00\x02\x86\x00\x26"
    MSG_Prefs_GMTOffsetHelp_STR "\x00\x00"
    "\x00\x00\x02\x87\x00\x12"
    MSG_Prefs_GetFromSystem_STR "\x00\x00"
    "\x00\x00\x02\x88\x00\x2C"
    MSG_Prefs_GetFromSystemHelp_STR "\x00\x00"
    "\x00\x00\x02\x89\x00\x08"
    MSG_Prefs_Proxy_STR "\x00\x00"
    "\x00\x00\x02\x8A\x00\x22"
    MSG_Prefs_ProxyHelp_STR "\x00\x00"
    "\x00\x00\x02\x8B\x00\x06"
    MSG_Prefs_ProxyPort_STR "\x00"
    "\x00\x00\x02\x8C\x00\x18"
    MSG_Prefs_ProxyPortHelp_STR "\x00\x00"
    "\x00\x00\x02\x8D\x00\x0C"
    MSG_Prefs_UseProxy_STR "\x00\x00"
    "\x00\x00\x02\x8E\x00\x20"
    MSG_Prefs_UseProxyHelp_STR "\x00"
    "\x00\x00\x02\x8F\x00\x08"
    MSG_Prefs_Agent_STR "\x00\x00"
    "\x00\x00\x02\x90\x00\x1E"
    MSG_Prefs_AgentHelp_STR "\x00"
    "\x00\x00\x02\x91\x00\x0E"
    MSG_Prefs_BPos_STR "\x00"
    "\x00\x00\x02\x92\x00\x28"
    MSG_Prefs_BPosHelp_STR "\x00"
    "\x00\x00\x02\x93\x00\x04"
    MSG_Prefs_BPos_Top_STR "\x00"
    "\x00\x00\x02\x94\x00\x08"
    MSG_Prefs_BPos_Bottom_STR "\x00\x00"
    "\x00\x00\x02\x95\x00\x06"
    MSG_Prefs_BPos_Left_STR "\x00\x00"
    "\x00\x00\x02\x96\x00\x06"
    MSG_Prefs_BPos_Right_STR "\x00"
    "\x00\x00\x02\x97\x00\x0A"
    MSG_Prefs_BPos_InnerTop_STR "\x00"
    "\x00\x00\x02\x98\x00\x0E"
    MSG_Prefs_BPos_InnerBottom_STR "\x00\x00"
    "\x00\x00\x02\x99\x00\x12"
    MSG_Prefs_Layout_STR "\x00"
    "\x00\x00\x02\x9A\x00\x24"
    MSG_Prefs_LayoutHelp_STR "\x00"
    "\x00\x00\x02\x9B\x00\x0C"
    MSG_Prefs_Layout_Up_STR "\x00\x00"
    "\x00\x00\x02\x9C\x00\x08"
    MSG_Prefs_Layout_Center_STR "\x00\x00"
    "\x00\x00\x02\x9D\x00\x0E"
    MSG_Prefs_Layout_Down_STR "\x00\x00"
    "\x00\x00\x02\x9E\x00\x10"
    MSG_Prefs_LPos_STR "\x00"
    "\x00\x00\x02\x9F\x00\x3E"
    MSG_Prefs_LPosHelp_STR "\x00"
    "\x00\x00\x02\xA0\x00\x08"
    MSG_Prefs_LPos_Bottom_STR "\x00\x00"
    "\x00\x00\x02\xA1\x00\x04"
    MSG_Prefs_LPos_Top_STR "\x00"
    "\x00\x00\x02\xA2\x00\x06"
    MSG_Prefs_LPos_Right_STR "\x00"
    "\x00\x00\x02\xA3\x00\x06"
    MSG_Prefs_LPos_Left_STR "\x00\x00"
    "\x00\x00\x02\xA4\x00\x0C"
    MSG_Prefs_ViewMode_STR "\x00"
    "\x00\x00\x02\xA5\x00\x2A"
    MSG_Prefs_ViewModeHelp_STR "\x00"
    "\x00\x00\x02\xA6\x00\x10"
    MSG_Prefs_ViewMode_TextGfx_STR "\x00\x00"
    "\x00\x00\x02\xA7\x00\x0A"
    MSG_Prefs_ViewMode_Text_STR "\x00"
    "\x00\x00\x02\xA8\x00\x0C"
    MSG_Prefs_ViewMode_Gfx_STR "\x00\x00"
    "\x00\x00\x02\xA9\x00\x0C"
    MSG_Prefs_Borderless_STR "\x00"
    "\x00\x00\x02\xAA\x00\x26"
    MSG_Prefs_BorderlessHelp_STR "\x00\x00"
    "\x00\x00\x02\xAB\x00\x0C"
    MSG_Prefs_Sunny_STR "\x00\x00"
    "\x00\x00\x02\xAC\x00\x5C"
    MSG_Prefs_SunnyHelp_STR "\x00"
    "\x00\x00\x02\xAD\x00\x08"
    MSG_Prefs_Raised_STR "\x00"
    "\x00\x00\x02\xAE\x00\x52"
    MSG_Prefs_RaisedHelp_STR "\x00"
    "\x00\x00\x02\xAF\x00\x08"
    MSG_Prefs_Scaled_STR "\x00"
    "\x00\x00\x02\xB0\x00\x3C"
    MSG_Prefs_ScaledHelp_STR "\x00\x00"
    "\x00\x00\x02\xB1\x00\x0C"
    MSG_Prefs_Underscore_STR "\x00"
    "\x00\x00\x02\xB2\x00\x2E"
    MSG_Prefs_UnderscoreHelp_STR "\x00"
    "\x00\x00\x02\xB3\x00\x14"
    MSG_Prefs_Frame_STR "\x00\x00"
    "\x00\x00\x02\xB4\x00\x34"
    MSG_Prefs_FrameHelp_STR "\x00\x00"
    "\x00\x00\x02\xB5\x00\x0C"
    MSG_Prefs_BarSpacer_STR "\x00"
    "\x00\x00\x02\xB6\x00\x38"
    MSG_Prefs_BarSpacerHelp_STR "\x00"
    "\x00\x00\x02\xB7\x00\x18"
    MSG_Prefs_DragBar_STR "\x00\x00"
    "\x00\x00\x02\xB8\x00\x2E"
    MSG_Prefs_DragBarHelp_STR "\x00"
    "\x00\x00\x02\xB9\x00\x14"
    MSG_Prefs_ShowFeedImage_STR "\x00\x00"
    "\x00\x00\x02\xBA\x00\x32"
    MSG_Prefs_ShowFeedImageHelp_STR "\x00"
    "\x00\x00\x02\xBB\x00\x1A"
    MSG_Prefs_SobstituteWinChars_STR "\x00"
    "\x00\x00\x02\xBC\x00\x94"
    MSG_Prefs_SobstituteWinCharsHelp_STR "\x00"
    "\x00\x00\x02\xBD\x00\x1E"
    MSG_Prefs_UseFeedsListImages_STR "\x00\x00"
    "\x00\x00\x02\xBE\x00\x30"
    MSG_Prefs_UseFeedsListImagesHelp_STR "\x00\x00"
    "\x00\x00\x02\xBF\x00\x1A"
    MSG_Prefs_UseDefaultGroupImage_STR "\x00\x00"
    "\x00\x00\x02\xC0\x00\x50"
    MSG_Prefs_UseDefaultGroupImageHelp_STR "\x00\x00"
    "\x00\x00\x02\xC1\x00\x10"
    MSG_Prefs_MaxImageSize_STR "\x00"
    "\x00\x00\x02\xC2\x00\x1C"
    MSG_Prefs_MaxImageSizeHelp_STR "\x00"
    "\x00\x00\x02\xC3\x00\x12"
    MSG_Prefs_ShowGroupNews_STR "\x00"
    "\x00\x00\x02\xC4\x00\x42"
    MSG_Prefs_ShowGroupNewsHelp_STR "\x00\x00"
    "\x00\x00\x02\xC5\x00\x14"
    MSG_Prefs_NTLoadImages_STR "\x00\x00"
    "\x00\x00\x02\xC6\x00\x34"
    MSG_Prefs_NTLoadImagesHelp_STR "\x00"
    "\x00\x00\x02\xC7\x00\x10"
    MSG_Prefs_UseHandMouse_STR "\x00"
    "\x00\x00\x02\xC8\x00\x2E"
    MSG_Prefs_UseHandMouseHelp_STR "\x00"
    "\x00\x00\x02\xC9\x00\x0A"
    MSG_Prefs_Popph_File_STR "\x00"
    "\x00\x00\x02\xCA\x00\x14"
    MSG_Prefs_Popph_Screen_STR "\x00\x00"
    "\x00\x00\x02\xCB\x00\x06"
    MSG_Prefs_Popph_Link_STR "\x00\x00"
    "\x00\x00\x03\x20\x00\x0E"
    MSG_Group_LocalFeed_STR "\x00"
    "\x00\x00\x03\x21\x00\x10"
    MSG_Group_NoLocalFeed_STR "\x00"
    "\x00\x00\x03\x22\x00\x10"
    MSG_Group_Welcome_STR "\x00\x00"
    "\x00\x00\x03\x84\x00\x0A"
    MSG_EditFeed_WinTitleEditFeed_STR "\x00"
    "\x00\x00\x03\x85\x00\x0C"
    MSG_EditFeed_WinTitleEditGroup_STR "\x00\x00"
    "\x00\x00\x03\x86\x00\x0E"
    MSG_EditFeed_WinTitleNewFeed_STR "\x00"
    "\x00\x00\x03\x87\x00\x10"
    MSG_EditFeed_WinTitleNewGroup_STR "\x00\x00"
    "\x00\x00\x03\x88\x00\x06"
    MSG_EditFeed_URL_STR "\x00\x00"
    "\x00\x00\x03\x89\x00\x16"
    MSG_EditFeed_URLHelp_STR "\x00\x00"
    "\x00\x00\x03\x8A\x00\x06"
    MSG_EditFeed_Name_STR "\x00"
    "\x00\x00\x03\x8B\x00\x16"
    MSG_EditFeed_NameHelp_STR "\x00"
    "\x00\x00\x03\x8C\x00\x06"
    MSG_EditFeed_File_STR "\x00\x00"
    "\x00\x00\x03\x8D\x00\x28"
    MSG_EditFeed_FileHelp_STR "\x00\x00"
    "\x00\x00\x03\x8E\x00\x06"
    Msg_EditFeed_OK_STR "\x00\x00"
    "\x00\x00\x03\x8F\x00\x0E"
    Msg_EditFeed_OKHelp_STR "\x00"
    "\x00\x00\x03\x90\x00\x08"
    Msg_EditFeed_Cancel_STR "\x00"
    "\x00\x00\x03\x91\x00\x16"
    Msg_EditFeed_CancelHelp_STR "\x00\x00"
    "\x00\x00\x03\x92\x00\x08"
    MSG_EditFeed_Pic_STR "\x00\x00"
    "\x00\x00\x03\x93\x00\x3C"
    MSG_EditFeed_PicHelp_STR "\x00"
    "\x00\x00\x03\x94\x00\x10"
    MSG_EditFeed_PicReqTitle_STR "\x00"
    "\x00\x00\x03\x95\x00\x0C"
    MSG_EditFeed_ShowLink_STR "\x00\x00"
    "\x00\x00\x03\x96\x00\x50"
    MSG_EditFeed_ShowLinkHelp_STR "\x00"
    "\x00\x00\x03\x97\x00\x08"
    MSG_EditFeed_Update_STR "\x00"
    "\x00\x00\x03\x98\x00\x30"
    MSG_EditFeed_UpdateHelp_STR "\x00"
    "\x00\x00\x03\x99\x00\x06"
    MSG_EditFeed_Update_Never_STR "\x00"
    "\x00\x00\x03\x9A\x00\x12"
    MSG_EditFeed_Update_Min_STR "\x00\x00"
    "\x00\x00\x04\x4C\x00\x0A"
    MSG_Title_Headline_STR "\x00\x00"
    "\x00\x00\x04\x4D\x00\x06"
    MSG_Title_Date_STR "\x00\x00"
    "\x00\x00\x04\x4E\x00\x08"
    MSG_Title_Subject_STR "\x00"
    "\x00\x00\x04\x4F\x00\x08"
    MSG_Title_Author_STR "\x00\x00"
    "\x00\x00\x04\xB0\x00\x08"
    MSG_SearchWin_Title_STR "\x00\x00"
    "\x00\x00\x04\xB1\x00\x08"
    MSG_SearchWin_Pattern_STR "\x00"
    "\x00\x00\x04\xB2\x00\x1A"
    MSG_SearchWin_PatternHelp_STR "\x00"
    "\x00\x00\x04\xB3\x00\x06"
    MSG_SearchWin_Case_STR "\x00"
    "\x00\x00\x04\xB4\x00\x2A"
    MSG_SearchWin_CaseHelp_STR "\x00"
    "\x00\x00\x04\xB5\x00\x0E"
    MSG_SearchWin_Welcome_STR "\x00\x00"
    "\x00\x00\x04\xB6\x00\x08"
    MSG_SearchWin_First_STR "\x00\x00"
    "\x00\x00\x04\xB7\x00\x14"
    MSG_SearchWin_FirstHelp_STR "\x00"
    "\x00\x00\x04\xB8\x00\x06"
    MSG_SearchWin_Succ_STR "\x00"
    "\x00\x00\x04\xB9\x00\x14"
    MSG_SearchWin_SuccHelp_STR "\x00\x00"
    "\x00\x00\x04\xBA\x00\x06"
    MSG_SearchWin_Back_STR "\x00"
    "\x00\x00\x04\xBB\x00\x18"
    MSG_SearchWin_BackHelp_STR "\x00\x00"
    "\x00\x00\x04\xBC\x00\x08"
    MSG_SearchWin_Cancel_STR "\x00"
    "\x00\x00\x04\xBD\x00\x16"
    MSG_SearchWin_CancelHelp_STR "\x00\x00"
    "\x00\x00\x04\xBE\x00\x0E"
    MSG_SearchWin_Searching_STR "\x00\x00"
    "\x00\x00\x04\xBF\x00\x08"
    MSG_SearchWin_Found_STR "\x00\x00"
    "\x00\x00\x04\xC0\x00\x0C"
    MSG_SearchWin_NotFound_STR "\x00\x00"
    "\x00\x00\x05\x78\x00\x14"
    MSG_CM_Win_Title_STR "\x00\x00"
    "\x00\x00\x05\xDC\x00\x12"
    MSG_MiniMail_Win_Title_STR "\x00"
};

#endif /* CATCOMP_BLOCK */


/****************************************************************************/


struct LocaleInfo
{
    APTR li_LocaleBase;
    APTR li_Catalog;
};


#ifdef CATCOMP_CODE

STRPTR GetString(struct LocaleInfo *li, LONG stringNum)
{
LONG   *l;
UWORD  *w;
STRPTR  builtIn;

    l = (LONG *)CatCompBlock;

    while (*l != stringNum)
    {
        w = (UWORD *)((ULONG)l + 4);
        l = (LONG *)((ULONG)l + (ULONG)*w + 6);
    }
    builtIn = (STRPTR)((ULONG)l + 6);

#undef LocaleBase
#define LocaleBase li->li_LocaleBase
    
    if (LocaleBase)
        return(GetCatalogStr(li->li_Catalog,stringNum,builtIn));
#undef LocaleBase

    return(builtIn);
}


#endif /* CATCOMP_CODE */


/****************************************************************************/


#endif /* LOC_H */
