#ifndef _RSS_H
#define _RSS_H

#define __NOLIBBASE__
#define __USE_SYSBASE

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/icon.h>
#include <proto/openurl.h>
#include <proto/rexxsyslib.h>
#include <proto/codesets.h>
#include <proto/expat.h>
#include <proto/Asyncio.h>

#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/dos.h>

#include <clib/alib_protos.h>
#include <clib/debug_protos.h>

#include <libraries/mui.h>
#include <libraries/asl.h>
#include <libraries/gadtools.h>

#include <mui/MUIundoc.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include <mui/NListtree_mcc.h>
#include <mui/HTMLview_mcc.h>
#include <mui/TheBar_mcc.h>
#include <mui/Textinput_mcc.h>
#include <mui/Busy_mcc.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#ifndef __MORPHOS__
#include <dos.h>
#endif

#include "md5.h"
#include "http.h"
#include "lineread.h"
#include "uri.h"
#include "macros.h"

//#define USE_BASTARD_MACROS
//#include <poolbastard.h>

/***********************************************************************/

/*
** Defaults and constants
*/

#define DEF_BASE         "AMRSS"

#define DEF_WLEFT         60
#define DEF_WRIGHT       190
#define DEF_WRIGHTUP     150
#define DEF_WRIGHTDOWN    80
#define DEF_HARDSPACING    2

#define DEF_ViewMode     MUIV_TheBar_ViewMode_TextGfx
#define DEF_LPos         MUIV_TheBar_LabelPos_Bottom
#define DEF_BPos         BPOS_Top
#define DEF_Layout       MUIV_TheBar_BarPos_Left
#define DEF_BFlags       (BFLG_BarSpacer|BFLG_EnableKeys|BFLG_DragBar|BFLG_Frame)

#define DEF_NViewMode    MUIV_TheBar_ViewMode_TextGfx
#define DEF_NLPos        MUIV_TheBar_LabelPos_Bottom
#define DEF_NBPos        BPOS_Bottom
#define DEF_NLayout      MUIV_TheBar_BarPos_Left
#define DEF_NBFlags      (BFLG_BarSpacer|BFLG_EnableKeys|BFLG_DragBar|BFLG_Frame)

#define DEF_PFLAGS       (PFLG_ShowFeedImage|PFLG_UseHandMouse)
#define DEF_GDC          GDC_Feed
#define DEF_EDC          EDC_GoToLink
#define DEF_DC           DC_GoToLink

#define DEF_PROXYPORT    8080
#define DEF_CONSOLE      "CON:0/0/400/100/amrss/Auto Close"
#define DEF_AGENT        PRG"/"VRSTRING " Amiga RSS client"
#define DEF_EDITOR       "multiview %s"

#define DEF_ETCDIR       "PROGDIR:etc"
#define DEF_DBDIR        DEF_ETCDIR"/db"
#define DEF_IMGDIR       DEF_ETCDIR"/img"
#define DEF_OPTSDIR      DEF_ETCDIR"/opts"
#define DEF_LIBSDIR      DEF_ETCDIR"/libs"
#define DEF_DOCSDIR      DEF_ETCDIR"/docs"
#define DEF_GROUPPIC     DEF_IMGDIR"/Group"

#define DEF_CATNAME      PRG".catalog"
#define DEF_ICON         "PROGDIR:"PRG
#define DEF_ALERTICON    DEF_IMGDIR "/" PRG "_Alert"
#define DEF_FEEDS        DEF_DBDIR"/feeds"
#define DEF_HELPFMT      "file://"DEF_DOCSDIR"/%s"

#define DEF_XMLDATASIZE    1024
#define DEF_FILESIZE        256
#define DEF_URLSIZE         256
#define DEF_CONSOLESIZE     256
#define DEF_CODESETSIZE     128
#define DEF_AGENTSIZE       128
#define DEF_EDITORSIZE      128
#define DEF_NAMESIZE         64
#define DEF_PUBSCREENAME     64
#define DEF_WINTITLESIZE    128
#define DEF_SCREENTITLESIZE 128
#define DEF_SEARCHSIZE       64
#define DEF_USERITEM         64
#define DEF_USERITEMNAME     16

/***********************************************************************/
/*
** Preferences
*/

/* struct used to store TheBar prefereces */
struct bprefs
{
    ULONG viewMode;
    ULONG lPos;
    ULONG bPos;
    ULONG layout;
    ULONG bFlags;
};

/* flags */
enum
{
    BFLG_Borderless = 1<<0,
    BFLG_Sunny      = 1<<1,
    BFLG_Raised     = 1<<2,
    BFLG_Scaled     = 1<<3,
    BFLG_BarSpacer  = 1<<4,
    BFLG_EnableKeys = 1<<5,
    BFLG_Frame      = 1<<6,
    BFLG_DragBar    = 1<<7,
};

/* bPos */
enum
{
    BPOS_Horiz       = 1<<0,
    BPOS_TopMode     = 1<<1,
    BPOS_LeftMode    = 1<<2,
    BPOS_InnerMode   = 1<<3,

    BPOS_Top         = (1<<4)|BPOS_Horiz|BPOS_TopMode,
    BPOS_Bottom      = (1<<5)|BPOS_Horiz|BPOS_TopMode,
    BPOS_Left        = (1<<6)|BPOS_LeftMode,
    BPOS_Right       = (1<<7)|BPOS_LeftMode,
    BPOS_InnerTop    = (1<<8)|BPOS_Horiz|BPOS_InnerMode,
    BPOS_InnerBottom = (1<<9)|BPOS_Horiz|BPOS_InnerMode,
};

/* struct used to store groups weights */
struct wprefs
{
    ULONG left;
    ULONG right;
    ULONG rightUp;
    ULONG rightDown;
};

/* the preferecense */
struct prefs
{
    struct bprefs        bprefs;
    struct bprefs        nbprefs;
    struct wprefs        wprefs;
    ULONG                noDragDrop;
    ULONG                noAutoUpdate;
    ULONG	             hideGroups;
    ULONG	             hideBar;
    ULONG	             hidePreview;

    ULONG                groupDoubleClick;
    ULONG                entriesDoubleClick;
    ULONG                descrClick;
    UBYTE                editor[DEF_EDITORSIZE];
    UBYTE                userItemName[DEF_USERITEMNAME];
    UBYTE                userItem[DEF_USERITEM];
    UBYTE                console[DEF_CONSOLESIZE];
    ULONG                maxImageSize;
    ULONG                flags;

    UBYTE                codeset[DEF_CODESETSIZE];
    LONG                 GMTOffset;

    UBYTE                proxy[DEF_URLSIZE];
    int                  proxyPort;
    UBYTE                agent[DEF_AGENTSIZE];
    ULONG                httpFlags;

    ULONG                bwinFlags;
};                           

/* groupDoubleClick */
enum
{
    GDC_Nothing,
    GDC_Edit,
    GDC_Feed,
};

/* entriesDoubleClick */
enum
{
    EDC_Nothing,
    EDC_Browser,
    EDC_GoToLink,
};

/* descrClick */
enum
{
    DC_Nothing,
    DC_Browser,
    DC_GoToLink,
};

/* flags */
enum
{
    PFLG_NoListBar            = 1<<0,
    PFLG_BoldTitles           = 1<<1,
    PFLG_RightMouse           = 1<<2,
    PFLG_BoldGroups           = 1<<3,
    PFLG_SubstDate            = 1<<4,
    PFLG_ShowFeedImage        = 1<<5,
    PFLG_SubstWinChars        = 1<<6,
    PFLG_UseFeedsListImages   = 1<<7,
    PFLG_UseDefaultGroupImage = 1<<8,
    PFLG_ScaleImages          = 1<<9,
    PFLG_ShowGroupNews        = 1<<10,
    PFLG_UseHandMouse         = 1<<11,
};

/* httpFlags */
enum
{
    PHFLG_UseProxy            = 1<<0,
    PHFLG_NTLoadImages        = 1<<1,
};

/* bwinFlags */
enum
{
    PBWFLG_Autoshow    = 1<<0,
    PBWFLG_ShowDragBar = 1<<1,
    PBWFLG_ShowSize    = 1<<2,
    PBWFLG_DragBarTop  = 1<<3,
};

/* some macro */
#define BPREFS(p) ((struct bprefs *)(p))
#define WPREFS(p) ((struct wprefs *)(p))
#define PREFS(p)  ((struct prefs *)(p))

/***********************************************************************/
/*
** Groups and feeds
*/

/* struct used to hold feed information update at download */
struct feedInfo
{
    STRPTR           author;
    STRPTR           subject;
    STRPTR           title;
    STRPTR           descr;
    STRPTR           link;
    STRPTR           image;
    STRPTR           imageLink;
    LONG             iwidth;
    LONG             iheight;
    struct DateStamp date;
    struct DateStamp lastUpdate;
    ULONG            news;

    ULONG            authorLen;
    ULONG            subjectLen;
    ULONG            titleLen;
    ULONG            descrLen;
    ULONG            linkLen;
    ULONG            imageLen;
    ULONG            imageLinkLen;
};

struct pdata
{
    struct IClass   *cl;
    Object          *obj;
    Object          *list;
    struct data     *data;
    APTR            pool;
    struct feed     *feed;
    struct entry    *entry;
    ULONG           flags;
    STRPTR          curr;
    UBYTE           buf[DEF_XMLDATASIZE];
    STRPTR          encoding;
    ULONG           clen;
    struct feedInfo info;
    MD5_CTX         ctx;
};

enum
{
    PFLG_Item      = 1<<0,
    PFLG_Valid     = 1<<1,
    PFLG_Allocated = 1<<2,
    PFLG_Channel   = 1<<3,
    PFLG_Image     = 1<<4,
    PFLG_Atom      = 1<<5,

    PFLG_FSubject  = 1<<16,
    PFLG_FDate     = 1<<17,
    PFLG_Active    = 1<<18,
    PFLG_UpdateMD5 = 1<<19,
    PFLG_Group     = 1<<20,
};

struct image
{
    struct MinNode link;
    BPTR           lock;
    APTR           picIm;
    Object         *picObj;
    ULONG          count;
};

#define IMAGE(im) ((struct image *)(im))

/* struct used to store groups and feed */
struct feed
{
    struct MinNode                link;
    struct SignalSemaphore        sem;
    APTR                          pool;
    STRPTR                        name;
    STRPTR                        URL;
    Object                        *ewin;
    struct feedInfo               info;
    STRPTR                        pic;
    struct image                  *picIm;
    UBYTE                         digest[16];
    UBYTE                         digestBack[16];

    struct MsgPort                port;
    struct MsgPort                resPort;
    struct Process                *proc;
    struct taskMsg                tmsg;
    XML_Parser                    parser;
    struct pdata                  pdata;
    STRPTR                        errorBuf;

    BPTR                          fh;
    STRPTR                        file;
    STRPTR                        tempFile;
    STRPTR                        favIconFile;

    struct MUI_NListtree_TreeNode *tn;

    struct DateStamp              lastUpdate;
    ULONG                         updateMin;

    ULONG                         flags;
};

/* flags */
enum
{
    FFLG_Group        = 1<<0,
    FFLG_Opened       = 1<<1,
    FFLG_SkipUTF8     = 1<<2,
    FFLG_ShowLink     = 1<<4,
    FFLG_ForceOpenURL = 1<<5,
    FFLG_Task         = 1<<6,
    FFLG_Active       = 1<<7,
    FFLG_Delete       = 1<<8,
    FFLG_Failure      = 1<<9,
    FFLG_Read         = 1<<10,
    FFLG_News         = 1<<11,
    FFLG_Local        = 1<<12,
    FFLG_FavIcon      = 1<<13,
    FFLG_UsePic       = 1<<14,
};

#define FEED(f) ((struct feed *)(f))

/***********************************************************************/
/*
** Tags
*/

/* This is the same tag from FreeDB. I hate TAG_USER */
#define _TAG(n) ((int)0xfec901f4+(n))

/***********************************************************************/
/*
** General methods and attributes
*/

/* Methods */
#define MUIM_RSS_HTTPStatus     _TAG(0)
#define MUIM_RSS_GotoURL        _TAG(1)

struct MUIP_RSS_GotoURL
{
    ULONG  MethodID;
    STRPTR URL;
    ULONG  flags;
};

enum
{
    MUIV_RSS_GotoURL_IgnorePrefs = 1<<0,
    MUIV_RSS_GotoURL_NoTarget    = 1<<1,
};

struct MUIP_RSS_HTTPStatus
{
    ULONG MethodID;
    ULONG type;
    ULONG arg;
    ULONG flags;
};

/* Attributes */
#define MUIA_RSS_App            _TAG(0)
#define MUIA_RSS_Win            _TAG(1)
#define MUIA_RSS_Group          _TAG(2)
#define MUIA_RSS_Groups         _TAG(3)
#define MUIA_RSS_Entries        _TAG(4)
#define MUIA_RSS_Prefs          _TAG(5)
#define MUIA_RSS_BarPrefs       _TAG(6)
#define MUIA_RSS_Codeset        _TAG(7)
#define MUIA_RSS_NBarPrefs      _TAG(8)
#define MUIA_RSS_BarPos         _TAG(9)
#define MUIA_RSS_NBarPos        _TAG(10)
#define MUIA_RSS_Pool           _TAG(11)
#define MUIA_RSS_OneWay         _TAG(12)
#define MUIA_RSS_Key            _TAG(13)
#define MUIA_RSS_NotifyPort     _TAG(14)
#define MUIA_RSS_YouHaveNews    _TAG(15)
#define MUIA_RSS_URLInArgs 	    _TAG(16)
#define MUIA_RSS_Compact   	    _TAG(17)

/***********************************************************************/
/*
** app class
*/

/* methods */

#define MUIM_App_DisposeWin     _TAG(30)
#define MUIM_App_About          _TAG(31)
#define MUIM_App_PrefsWin       _TAG(32)
#define MUIM_App_LoadPrefs      _TAG(33)
#define MUIM_App_SavePrefs      _TAG(34)
#define MUIM_App_HandleMsg      _TAG(35)
#define MUIM_App_LoadFeeds      _TAG(36)
#define MUIM_App_CMWin          _TAG(37)
#define MUIM_App_NewApp         _TAG(38)
#define MUIM_App_MiniMailWin    _TAG(39)
#define MUIM_App_QuickQuit      _TAG(40)
#define MUIM_App_HandleTimer    _TAG(41)

/* structures */

struct MUIP_App_NewApp
{
    ULONG            MethodID;
    struct parseArgs *pa;
};

struct MUIP_App_DisposeWin
{
    ULONG  MethodID;
    Object *win;
};

struct MUIP_App_PrefsWin
{
    ULONG MethodID;
    ULONG page;
};

struct MUIP_App_MiniMailWin
{
    ULONG  MethodID;
    STRPTR addr;
};

/* Attributes */

#define MUIA_App_ParseArgs      _TAG(30)
#define MUIA_App_NoDragDrop     _TAG(31)
#define MUIA_App_NoAutoUpdate   _TAG(32)
#define MUIA_App_HideGroups     _TAG(33)
#define MUIA_App_ScreenTitle    _TAG(34)
#define MUIA_App_SavePrefs      _TAG(35)
#define MUIA_App_HideBar        _TAG(36)
#define MUIA_App_HidePreview    _TAG(37)
#define MUIA_App_Title     	    _TAG(38)

/***********************************************************************/
/*
** win class
*/

/* Attributes */

#define MUIA_Win_Forward _TAG(60)

/***********************************************************************/
/*
** group class
*/

/* Methods */

#define MUIM_MainGroup_ActivePage   _TAG(70)
#define MUIM_MainGroup_EntryChange  _TAG(71)
#define MUIM_MainGroup_SetInfo      _TAG(72)
#define MUIM_MainGroup_SetFeedInfo  _TAG(73)
#define MUIM_MainGroup_Search       _TAG(74)
#define MUIM_MainGroup_Stop         _TAG(75)
#define MUIM_MainGroup_CurrentURL   _TAG(76)
#define MUIM_MainGroup_UserURL      _TAG(77)
#define MUIM_MainGroup_Editor       _TAG(78)
#define MUIM_MainGroup_FlushImages  _TAG(79)
#define MUIM_MainGroup_FlushCookies _TAG(80)
#define MUIM_MainGroup_AppMessage   _TAG(81)

/* Structures */

struct MUIP_MainGroup_ActivePage
{
    ULONG  MethodID;
    LONG   page;
};

enum
{
    MUIV_MainGroup_ActivePage_News,
    MUIV_MainGroup_ActivePage_HTML,
};

struct MUIP_MainGroup_SetInfo
{
    ULONG  MethodID;
    STRPTR string;
    ULONG  id;
};

struct MUIP_MainGroup_SetFeedInfo
{
    ULONG           MethodID;
    struct feedInfo *fi;
};

struct MUIP_MainGroup_Editor
{
    ULONG  MethodID;
    STRPTR file;
};

struct MUIP_MainGroup_CurrentURL
{
    ULONG  MethodID;
    STRPTR URL;
    ULONG  showString;
};

struct MUIP_MainGroup_UserURL
{
    ULONG  MethodID;
    STRPTR URL;
    ULONG  oneWay;
};

struct MUIP_MainGroup_AppMessage
{
    ULONG  	      MethodID;
    struct AppMessage *amsg;
};

/* Attributes */

#define MUIA_MainGroup_Back         _TAG(70)
#define MUIA_MainGroup_Succ         _TAG(71)
#define MUIA_MainGroup_Reload       _TAG(72)
#define MUIA_MainGroup_Status       _TAG(73)

enum
{
    MUIV_MainGroup_Status_None,
    MUIV_MainGroup_Status_Group,
    MUIV_MainGroup_Status_Feed,
    MUIV_MainGroup_Status_Feeding,
};

/***********************************************************************/
/*
** grouplist class
*/

/* Methods */

#define MUIM_GroupList_LoadFeeds       _TAG(90)
#define MUIM_GroupList_SaveFeeds       _TAG(91)
#define MUIM_GroupList_EditFeed        _TAG(92)
#define MUIM_GroupList_UpdateFeed      _TAG(93)
#define MUIM_GroupList_Remove          _TAG(94)
#define MUIM_GroupList_FindName        _TAG(95)
#define MUIM_GroupList_FindURL         _TAG(96)
#define MUIM_GroupList_AddFeed         _TAG(97)
#define MUIM_GroupList_DoubleClick     _TAG(98)
#define MUIM_GroupList_SetDefaultFeed  _TAG(99)
#define MUIM_GroupList_Feed            _TAG(100)
#define MUIM_GroupList_HandleEvent     _TAG(101)
#define MUIM_GroupList_Stop            _TAG(102)
#define MUIM_GroupList_AutoUpdate      _TAG(103)
#define MUIM_GroupList_HandleTimer     _TAG(104)
#define MUIM_GroupList_UpdateAll       _TAG(105)
#define MUIM_GroupList_DownloadFavIcon _TAG(106)
#define MUIM_GroupList_KillThatMess    _TAG(107)

/* Structures */

struct MUIP_GroupList_Feed
{
    ULONG                         MethodID;
    ULONG                         mode;
    struct MUI_NListtree_TreeNode *tn;
};

enum
{
    MUIV_GroupList_Feed_Local,
    MUIV_GroupList_Feed_Remote,
};

#define MUIV_GroupList_Feed_Active ((struct MUI_NListtree_TreeNode *)(1))

enum
{
    MUIV_GroupList_SaveFeeds_OK,
    MUIV_GroupList_SaveFeeds_Rename,
    MUIV_GroupList_SaveFeeds_Failure,
};

struct MUIP_GroupList_EditFeed
{
    ULONG MethodID;
    ULONG flags;
};

enum
{
    MUIV_GroupList_EditFeed_New   = 1<<0,
    MUIV_GroupList_EditFeed_Group = 1<<1,
    MUIV_GroupList_EditFeed_Sub   = 1<<2,
};

struct MUIP_GroupList_UpdateFeed
{
    ULONG                         MethodID;
    ULONG                         success;
    struct MUI_NListtree_TreeNode *list;
    struct MUI_NListtree_TreeNode *node;
    struct feed                   *feed;
    STRPTR                        name;
    STRPTR                        URL;
    STRPTR                        pic;
    ULONG                         updateMin;
    ULONG                         flags;
};

enum
{
    MUIV_GroupList_UpdateFeed_ShowLink     = 1<<0,
    MUIV_GroupList_UpdateFeed_UsePic       = 1<<1,
    MUIV_GroupList_UpdateFeed_ForceOpenURL = 1<<2,
};

struct MUIP_GroupList_Find
{
    ULONG  MethodID;
    STRPTR string;
};

struct MUIP_GroupList_AddFeed
{
    ULONG  MethodID;
    STRPTR name;
    STRPTR URL;
    STRPTR pic;
};

struct MUIP_GroupList_DownloadFavIcon
{
    ULONG                         MethodID;
    struct MUI_NListtree_TreeNode *tn;
};

enum
{
    MUIV_GroupList_DownloadFavIcon_Active,
};

/***********************************************************************/
/*
** entrieslist class
*/

/* Methods */

#define MUIM_Entries_DoubleClick _TAG(120)

/* Structures */

struct entry
{
    APTR             pool;
    struct feed      *feed;

    STRPTR           author;
    STRPTR           subject;
    STRPTR           title;
    STRPTR           descr;
    STRPTR           link;
    struct DateStamp date;

    ULONG            authorLen;
    ULONG            subjectLen;
    ULONG            titleLen;
    ULONG            descrLen;
    ULONG            linkLen;

    ULONG            flags;
};

enum
{
    EFLG_NtFreeTitle = 1<<0,
    EFLG_Disposing   = 1<<1,
    EFLG_Group       = 1<<2,
};

/* Attributes */

#define MUIA_Entries_Format      _TAG(120)

enum
{
    MUIV_Entries_Date    = 1<<0,
    MUIV_Entries_Subject = 1<<1,
};

/***********************************************************************/
/*
** editfeed class
*/

/* Methods */

#define MUIM_EditFeed_Close       _TAG(130)
#define MUIM_EditFeed_EditChange  _TAG(131)
#define MUIM_EditFeed_GoURL       _TAG(132)

/* Structures */

struct MUIP_EditFeed_Close
{
    ULONG MethodID;
    ULONG mode;
};

/* Attributes */

#define MUIA_EditFeed_ListObj     _TAG(130)
#define MUIA_EditFeed_List        _TAG(131)
#define MUIA_EditFeed_Node        _TAG(132)
#define MUIA_EditFeed_Feed        _TAG(133)

/***********************************************************************/
/*
** HTML class
*/

/* Methods */

#define MUIM_HTML_GoURL        _TAG(150)
#define MUIM_HTML_Clear        _TAG(151)
#define MUIM_HTML_GoBack       _TAG(152)
#define MUIM_HTML_GoSucc       _TAG(153)
#define MUIM_HTML_BlindGoURL   _TAG(154)
#define MUIM_HTML_FlushCookies _TAG(155)

/* Structures */

struct MUIP_HTML_GoURL
{
    ULONG  MethodID;
    STRPTR URL;
};

/* Attributes */

#define MUIA_HTML_Simple        _TAG(150)
#define MUIA_HTML_PMenu         _TAG(151)
#define MUIA_HTML_CurrentHost   _TAG(152)
#define MUIA_HTML_Data          _TAG(153)

/***********************************************************************/
/*
** prefs class
*/

/* Methods */

#define MUIM_Prefs_UsePrefs  _TAG(170)
#define MUIM_Prefs_System    _TAG(171)
#define MUIM_Prefs_TestPrefs _TAG(172)

/* Structures */

struct MUIP_Prefs_UsePrefs
{
    ULONG MethodID;
    ULONG mode;
};

enum
{
    MUIV_Prefs_UsePrefs_Use,
    MUIV_Prefs_UsePrefs_Apply,
    MUIV_Prefs_UsePrefs_Cancel,
};

/* Attributes */

#define MUIA_RSS_Prefs_Page _TAG(170)

enum
{
    MUIV_Prefs_Page_Options,
    MUIV_Prefs_Page_Locale,
    MUIV_Prefs_Page_HTTP,
    MUIV_Prefs_Page_Bar,
    MUIV_Prefs_Page_NBar,

    MUIV_Prefs_Page_None = 256,
};


/***********************************************************************/
/*
** search class
*/

/* Methods */

#define MUIM_Search_Search  _TAG(190)

/* Structures */

struct MUIP_Search_Search
{
    ULONG MethodID;
    ULONG mode;
};

enum
{
    MUIV_Search_Top,
    MUIV_Search_Succ,
    MUIV_Search_Back,
};

/* Attributes */

#define MUIA_Search_ListObj _TAG(190)
#define MUIA_Search_HTMLObj _TAG(191)

/***********************************************************************/
/*
** rootbar class
*/

/* Attributes */

#define MUIA_RootBar_NBar _TAG(210)

/***********************************************************************/
/*
** pic class
*/

/* Attributes */

#define MUIA_Pic_File           _TAG(230)
#define MUIA_Pic_Transparent    _TAG(231)
#define MUIA_Pic_ScaledMaxSize  _TAG(232)

/***********************************************************************/
/*
** miniMail class
*/

/* Attributes */

#define MUIA_MiniMail_Addr      _TAG(250)

/***********************************************************************/
/*
** rootscrollgroup class
*/

/* Attributes */

#define MUIA_RootScrollGroup_HDeltaFactor _TAG(260)
#define MUIA_RootScrollGroup_VDeltaFactor _TAG(261)

/***********************************************************************/
/*
** popupcodesets class
*/

/* Attributes */

#define MUIA_Popupcodesets_Acknowledge _TAG(270)

/***********************************************************************/
/*
** Popph class
*/

/* Methods */

#define MUIM_Popph_RequestFile _TAG(280)

/* Attributes */
#define MUIA_Popph_Syms        _TAG(280)
#define MUIA_Popph_Names       _TAG(281)
#define MUIA_Popph_MaxLen      _TAG(282)
#define MUIA_Popph_Key         _TAG(283)
#define MUIA_Popph_Asl         _TAG(284)

/***********************************************************************/
/*
** Macros
*/

#define rootScrollgroupObject NewObject(rootScrollgroupClass->mcc_Class,NULL
#define rootStringObject      NewObject(rootStringClass->mcc_Class,NULL
#define rootBarObject         NewObject(rootBarClass->mcc_Class,NULL
#define groupListObject       NewObject(groupListClass->mcc_Class,NULL
#define entriesListObject     NewObject(entriesListClass->mcc_Class,NULL
#define HTMLObject            NewObject(HTMLClass->mcc_Class,NULL
#define groupObject           NewObject(groupClass->mcc_Class,NULL
#define GMTObject             NewObject(GMTClass->mcc_Class,NULL
#define popupCodesetsObject   NewObject(popupCodesetsClass->mcc_Class,NULL
#define agentObject           NewObject(agentClass->mcc_Class,NULL
#define editFeedObject        NewObject(editFeedClass->mcc_Class,NULL
#define prefsObject           NewObject(prefsClass->mcc_Class,NULL
#define popphObject           NewObject(popphClass->mcc_Class,NULL
#define CMObject              NewObject(CMClass->mcc_Class,NULL
#define miniMailObject        NewObject(miniMailClass->mcc_Class,NULL
#define searchObject          NewObject(searchClass->mcc_Class,NULL
#define picObject             NewObject(picClass->mcc_Class,NULL
#define winObject             NewObject(winClass->mcc_Class,NULL
#define bwinObject            NewObject(bwinClass->mcc_Class,NULL
#define appObject             NewObject(appClass->mcc_Class,NULL

#define METITLE               {NM_TITLE,"",0,0,0,0}
#define MEDTITLE(d)           {NM_TITLE,"",0,0,0,(APTR)(d)}
#define MTITLE(t)             {NM_TITLE,(STRPTR)(t),0,0,0,(APTR)(t)}
#define MITEM(t,d)            {NM_ITEM,(STRPTR)(t),0,0,0,(APTR)(d)}
#define MTITEM(t,d)           {NM_ITEM,(STRPTR)(t),0,CHECKIT|MENUTOGGLE,0,(APTR)(d)}
#define MECITEM(t,d,e)        {NM_ITEM,(STRPTR)(t),0,CHECKIT|CHECKED,(LONG)(e),(APTR)(d)}
#define MEITEM(t,d,e)         {NM_ITEM,(STRPTR)(t),0,CHECKIT,(LONG)(e),(APTR)(d)}
#define MBAR                  {NM_ITEM,(STRPTR)NM_BARLABEL,0,0,0,NULL}
#define MSUBITEM(t,d)         {NM_SUB,(STRPTR)(t),0,0,0,(APTR)(d)}
#define MESUBITEM(t,d,e)      {NM_SUB, (STRPTR)(t),0,CHECKIT,(LONG)(e),(APTR)(d)}
#define MSUBBAR               {NM_SUB,(STRPTR)NM_BARLABEL,0,0,0,NULL}
#define MEND                  {NM_END,NULL,0,0,0,NULL}

#define wspace(w)             RectangleObject, MUIA_Weight, w, End
#define fhspace()             RectangleObject, MUIA_FixHeightTxt, "-", End
#define _sbbut(sb,id)         ((Object *)DoMethod((Object *)(sb),MUIM_TheBar_GetObject,(ULONG)(id)))

#undef set
#undef get
#undef nnset
#define get(obj,attr,store) 	       GetAttr((ULONG)(attr),(Object *)obj,(ULONG *)(store))
#define set(obj,attr,value) 	       SetAttrs((Object *)(obj),(ULONG)(attr),(ULONG)(value),TAG_DONE)
#define nnset(obj,attr,value) 	       SetAttrs((Object *)(obj),MUIA_NoNotify,TRUE,(ULONG)(attr),(ULONG)(value),TAG_DONE)
#define superset(cl,obj,attr,value)    SetSuperAttrs((APTR)(cl),(Object *)(obj),(ULONG)(attr),(ULONG)(value),TAG_DONE)
#define supernnset(cl,obj,attr,value)  SetSuperAttrs((APTR)(cl),(Object *)(obj),(ULONG)(attr),(ULONG)(value),MUIA_NoNotify,TRUE,TAG_DONE)
#define superget(cl,obj,attr,valPtr)   DoSuperMethod((APTR)(cl),(Object *)(obj),OM_GET,(ULONG)(attr),(ULONG)(valPtr))
#define nfset(obj,attr,value)          SetAttrs((Object *)(obj),(ULONG)(attr),(ULONG)(value),MUIA_Group_Forward,FALSE,TAG_DONE)

#define GETNUM(a) (*((LONG *)a))

/***********************************************************************/
/*
** NewMouse wheel events
*/

#ifndef NM_WHEEL_UP
#define NM_WHEEL_UP      0x7a
#endif

#ifndef NM_WHEEL_DOWN
#define NM_WHEEL_DOWN    0x7b
#endif

/***********************************************************************/
/*
** Arguments
*/

struct parseArgs
{
    UBYTE   pubscreen[DEF_PUBSCREENAME];
    STRPTR  URL;
    ULONG   flags;
};

/* flags */
enum
{
    PAFLG_Pubscreen = 1<<0,
    PAFLG_URL       = 1<<1,
    PAFLG_Compact   = 1<<2,
    PAFLG_BWin      = 1<<3,
    PAFLG_Backdrop  = 1<<4,
};

/***********************************************************************/
/*
** statusMsg
*/

struct HTTPStatusMsg
{
    struct Message link;
    ULONG          type;
    ULONG          arg;
    ULONG          flags;
};

/* type */
enum
{
    HSMTYPE_Busy,
    HSMTYPE_ToDo,
    HSMTYPE_Done,
};

enum
{
    HSMFLG_NoLen = 1<<0,
    HSMFLG_Queue = 1<<1,
};

/***********************************************************************/

struct astream
{
    STRPTR buf;
    int    size;

    STRPTR ptr;
    int    counter;

    ULONG  flags;
};

enum
{
    ASFLG_Small = 1<<1,
    ASFLG_Stop  = 1<<8,
};

/***********************************************************************/
/*
** Globals
*/

/* Libraries */
extern struct ExecBase         *SysBase;
extern struct DosLibrary       *DOSBase;
extern struct IntuitionBase    *IntuitionBase;

extern struct Library          *UtilityBase;
extern struct GfxBase          *GfxBase;
extern struct Library          *DataTypesBase;
extern struct Library          *CyberGfxBase;
extern struct Library          *IFFParseBase;
extern struct RxsLib           *RexxSysBase;
extern struct LocaleBase       *LocaleBase;
extern struct Library          *MUIMasterBase;
extern struct Library          *OpenURLBase;
extern struct Library          *CodesetsBase;
extern struct Library          *ExpatBase;
extern struct Library          *IconBase;
extern struct Library          *AsyncIOBase;

/* MUI Classes */
extern struct MUI_CustomClass  *picClass;
extern struct MUI_CustomClass  *CMClass;
extern struct MUI_CustomClass  *miniMailClass;
extern struct MUI_CustomClass  *popphClass;
extern struct MUI_CustomClass  *groupClass;
extern struct MUI_CustomClass  *groupListClass;
extern struct MUI_CustomClass  *entriesListClass;
extern struct MUI_CustomClass  *HTMLClass;
extern struct MUI_CustomClass  *rootBarClass;
extern struct MUI_CustomClass  *rootStringClass;
extern struct MUI_CustomClass  *rootScrollgroupClass;
extern struct MUI_CustomClass  *editFeedClass;
extern struct MUI_CustomClass  *popupCodesetsClass;
extern struct MUI_CustomClass  *agentClass;
extern struct MUI_CustomClass  *GMTClass;
extern struct MUI_CustomClass  *prefsClass;
extern struct MUI_CustomClass  *searchClass;
extern struct MUI_CustomClass  *winClass;
extern struct MUI_CustomClass  *bwinClass;
extern struct MUI_CustomClass  *appClass;

extern struct Catalog          *g_cat;
extern struct Locale           *g_loc;
extern APTR                    *g_pool;
extern struct SignalSemaphore  g_poolSem;

extern struct WBStartup        *_WBenchMsg;
extern UBYTE                   __ver[];

#ifdef DEBUGMEM
extern ULONG g_tot;
#endif

/***********************************************************************/

#include "rss_protos.h"

/***********************************************************************/

#endif /* _RSS_H */
