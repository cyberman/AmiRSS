#ifndef MUI_CMANAGER_MCC_H
#define MUI_CMANAGER_MCC_H

#ifndef INTUITION_CLASSUSR_H
#include <intuition/classusr.h>
#endif

#if defined(__GNUC__)
# pragma pack(2)
#endif

/*
**  $VER: CManager_mcc.h 31.2 (16.11.2004)
**  Includes Release 31.2
**
**  CManager_mcc.h
**  Global bookmarks manager MUI class
**
**  Written by Simone Tellini and Alfonso Ranieri <alforan@tin.it>.
**
**  Released under the terms of the
**  GNU Public Licence version 2
*/

/***********************************************************************/
/*
** Tags
**
** These are Simone Tellini's tags:
** Don't use them in other projects!
*/

#define TAGBASE  ((int)0xF76B0010)
#define CMTAG(n) (TAGBASE+(int)(n))

#define MUIC_CManager  "CManager.mcc"
#define CManagerObject MUI_NewObject(MUIC_CManager

/***********************************************************************/
/*
** CManager preferences
*/

/* Columns */
#define COLS_User   4
#define COLS_WWW    5
#define COLS_FTP    5
#define COLS_Chat   3
#define COLS_Telnet 2

#define LP_NUMSECTIONS 16
#define LP_NUMCOLS     16

/* List sorting */
struct SortOrder
{
    ULONG SortCol0;
    ULONG SortCol1;
    BYTE  Order[LP_NUMCOLS];
    int   Width[LP_NUMCOLS];
};

/* Entry list preferences */
struct ListPrefs
{
    ULONG            TreeW;
    ULONG            ListW;
    struct SortOrder Sorts[LP_NUMSECTIONS];
    ULONG            Flags;
    UBYTE            UserCols[LP_NUMCOLS];
    UBYTE            UserReserved[LP_NUMCOLS];
    UBYTE            WWWCols[LP_NUMCOLS];
    UBYTE            WWWCReserved[LP_NUMCOLS];
    UBYTE            FTPCols[LP_NUMCOLS];
    UBYTE            FTPCReserved[LP_NUMCOLS];
    UBYTE            ChatCols[LP_NUMCOLS];
    UBYTE            ChatReserved[LP_NUMCOLS];
    UBYTE            TelnetCols[LP_NUMCOLS];
    UBYTE            TelnReserved[LP_NUMCOLS];
    ULONG            ListCW;
    ULONG            CardCW;

    ULONG            res0;
    ULONG            res1;
    ULONG            res2;
    ULONG            res3;
    ULONG            res4;
    ULONG            res5;
    ULONG            res6;
    ULONG            res7;
};

/* UserFreeCol */
enum
{
    USERCOL_Name,
    USERCOL_EMail,
    USERCOL_Comment,
    USERCOL_Alias,
    USERCOL_Country,
    USERCOL_City,
    USERCOL_Phone,
    USERCOL_Mobile,
    USERCOL_Fax,
    USERCOL_WWW,
    USERCOL_Company,
    USERCOL_BPhone,
    USERCOL_BMobile,
    USERCOL_BFax,
    USERCOL_Birthday,
    USERCOL_Anniversary,
};

/* WWWFreeCol */
enum
{
    WWWCOL_Name,
    WWWCOL_Site,
    WWWCOL_LastVisited,
    WWWCOL_LastModified,
    WWWCOL_Comment,
    WWWCOL_Alias,
    WWWCOL_EMail,
};

/* FTPFreeCol */
enum
{
    FTPCOL_Name,
    FTPCOL_Site,
    FTPCOL_LastVisited,
    FTPCOL_LastModified,
    FTPCOL_Comment,
    FTPCOL_Alias,
    FTPCOL_EMail,
};

/* ChatFreeCol */
enum
{
    CHATCOL_Channel,
    CHATCOL_Server,
    CHATCOL_Comment,
    CHATCOL_EMail,
    CHATCOL_WWW,
};

/* TelnetFreeCol */
enum
{
    TELNETCOL_Host,
    TELNETCOL_Comment,
    TELNETCOL_Port,
    TELNETCOL_EMail,
    TELNETCOL_Alias,
};

/* ButtonsBar preferences */
struct ButtonsBarPrefs
{
    ULONG ViewMode;
    ULONG LabelPosition;
    ULONG BarPosition;
    ULONG Flags;
    ULONG ButtonsPos;
    ULONG dummy[7];
};

/* Flags */
enum
{
    SBFLG_Borderless = 1<<0,
    SBFLG_Raised     = 1<<1,
    SBFLG_Scaled     = 1<<2,
    SBFLG_Sunny      = 1<<3,
    SBFLG_Underscore = 1<<4,
    SBFLG_BarSpacer  = 1<<5,
    SBFLG_Hide       = 1<<6,
    SBFLG_Frame      = 1<<7,
    SBFLG_DragBar    = 1<<8,
};

/* BarPosition */
enum
{
    SBPos_Horiz       = 1<<0,
    SBPos_TopMode     = 1<<1,
    SBPos_LeftMode    = 1<<2,
    SBPos_InnerMode   = 1<<3,

    SBPos_Top         = (1<<4)|SBPos_Horiz|SBPos_TopMode,
    SBPos_Bottom      = (1<<5)|SBPos_Horiz|SBPos_TopMode,
    SBPos_Left        = (1<<6)|SBPos_LeftMode,
    SBPos_Right       = (1<<7)|SBPos_LeftMode,
    SBPos_InnerTop    = (1<<8)|SBPos_Horiz|SBPos_InnerMode,
    SBPos_InnerBottom = (1<<9)|SBPos_Horiz|SBPos_InnerMode,
};

/* Visual quick preferences always saved */
struct VisualPrefs
{
    struct ListPrefs       ListPrefs;
    struct ButtonsBarPrefs ButtonsBarPrefs;
    ULONG                  Flags;
};

/* Flags */
enum
{
    VPFLG_NoQuick    = 1<<0,
    VPFLG_NoCard     = 1<<1,
};


/* Hotlists */
enum
{
    HL_IB,
    HL_V,
    HL_AWEB,
    HL_AMFTP,
    HL_MD2,
    HL_STFAX,
    HL_YAM,
    HL_AMIRC,
    HL_DOPUS,
    HL_DFA,
    HL_PINT,
    HL_GOODNEWS,
    HL_STRICQ,
    HL_NETSCAPE,

    NUM_HOTLISTS
};

enum
{
    IDX_User,
    IDX_WWW,
    IDX_FTP,
    IDX_Chat,
    IDX_Telnet,
};

/* Preferences */
struct CMPrefs
{
    /* Actual Version */
    ULONG                           Version;

    /* VisuaPrefs */
    struct VisualPrefs              VisualPrefs;

    /* Settings */
    char                            Country[80];
    char                            CountryCode[9];
    ULONG                           Action;
    ULONG                           DelWarning;
    ULONG                           Options;
    ULONG                           GroupsStatus;

    /* Programs */
    UBYTE                           MailProg;
    char                            MailScript[256];
    char                            MailPath[256];

    UBYTE                           WWWProg;
    char                            WWWScript[256];
    char                            WWWPath[256];

    UBYTE                           FTPProg;
    char                            FTPScript[256];
    char                            FTPPath[256];
    char                            Local[256];
    ULONG                           FTPOptions;

    UBYTE                           ChatProg;
    char                            ChatScript[256];
    char                            ChatPath[256];

    UBYTE                           FaxProg;
    char                            FaxScript[256];
    char                            FaxPath[256];

    UBYTE                           TelnetProg;
    char                            TelnetScript[256];
    char                            TelnetPath[256];

    char                            SavedProg[256];

    /* Hotlists */
    char                            Hotlists[NUM_HOTLISTS][256];
    struct DateStamp                Stamps[NUM_HOTLISTS];

    /* Dialer */
    ULONG                           DialOptions;
    char                            DialProg[256];
    char                            SerDevice[256];
    UBYTE                           SerUnit;
    ULONG                           ModemType;
    char                            SerInit[256];
    char                            DialPrefix[30];
    char                            DialSuffix[30];
    ULONG                           RedialDelay;
    ULONG                           RedialAttempts;

    char                            CodesetName[256];
    struct codeset                  *Codeset;
};

#define CMANAGER_PREFS_VERSION 4

/* Action */
enum
{
    ACTION_EDIT,
    ACTION_SCRIPT,
    ACTION_SCRIPT_CLOSE,
    ACTION_SCRIPT_ICONIFY
};

/* Groups Status */
enum
{
    GRSTATUS_REMEMBER,
    GRSTATUS_OPENED,
    GRSTATUS_CLOSED,
};

/* Options */
enum
{
    OPT_SilentScan        = 1<<0,
    OPT_UseOpenURL        = 1<<1,
    OPT_AutoReload        = 1<<2,
    OPT_NtCheckChanges    = 1<<3,
    OPT_SaveOnExit        = 1<<4,
    OPT_SilentExit        = 1<<5,
    OPT_SaveOnChanges     = 1<<6,
    OPT_EditNew           = 1<<7,
    OPT_BoldSections      = 1<<8,
    OPT_BoldTitles        = 1<<9,
    OPT_NoListBar         = 1<<10,
    OPT_RightMouseActive  = 1<<11,
    OPT_UseSavedProg      = 1<<12,
    OPT_EmptyNodes        = 1<<13,
    OPT_SectionsImages    = 1<<14,
    OPT_GroupsImages      = 1<<15,
    OPT_ManualSort        = 1<<16,
};

/* FTPOptions */
enum
{
    FTPOPT_UseLocal = 1<<0,
};

/* DialOptions */
enum
{
    DIALOPT_ExternalDialer = 1<<0,
    DIALOPT_SerShared      = 1<<1,
    DIALOPT_LogCalls       = 1<<2,
};

/***********************************************************************/
/*
** CManager.mcc
*/

#define MUIM_CManager_LoadData           CMTAG(1)
#define MUIM_CManager_SetGroup           CMTAG(2)
#define MUIM_CManager_HandleNotify       CMTAG(3)
#define MUIM_CManager_FreeData           CMTAG(5)
#define MUIM_CManager_SaveData           CMTAG(6)
#define MUIM_CManager_GetGroup           CMTAG(8)
#define MUIM_CManager_DisposeObj         CMTAG(9)
#define MUIM_CManager_Arrow              CMTAG(10)
#define MUIM_CManager_AddGroup           CMTAG(11)
#define MUIM_CManager_RemGroup           CMTAG(12)
#define MUIM_CManager_AddItem            CMTAG(13)
#define MUIM_CManager_GrabLists          CMTAG(14)
#define MUIM_CManager_ReinsertLists      CMTAG(15)
#define MUIM_CManager_EditSelected       CMTAG(16)
#define MUIM_CManager_OnDoubleClick      CMTAG(17)
#define MUIM_CManager_LoadPrefs          CMTAG(18)
#define MUIM_CManager_AddEntry           CMTAG(19)
#define MUIM_CManager_Import             CMTAG(20)
#define MUIM_CManager_Login              CMTAG(21)
#define MUIM_CManager_Export             CMTAG(22)
#define MUIM_CManager_Search             CMTAG(23)
#define MUIM_CManager_Cleanup            CMTAG(24)
#define MUIM_CManager_SetPrefs           CMTAG(25)
#define MUIM_CManager_ScanHotlists       CMTAG(26)
#define MUIM_CManager_WinClose           CMTAG(27)
#define MUIM_CManager_DoubleClick        CMTAG(28)
#define MUIM_CManager_PrefsWindow        CMTAG(30)
#define MUIM_CManager_LoginWindow        CMTAG(31)
#define MUIM_CManager_ChangeQuick        CMTAG(32)
#define MUIM_CManager_QuickChange        CMTAG(33)
#define MUIM_CManager_Reload             CMTAG(34)
#define MUIM_CManager_Close              CMTAG(35)
#define MUIM_CManager_Locate             CMTAG(36)
#define MUIM_CManager_RequesterResult    CMTAG(37)
#define MUIM_CManager_SetButtonsBarPrefs CMTAG(38)
#define MUIM_CManager_RemSelected        CMTAG(39)
#define MUIM_CManager_SetFreeCol         CMTAG(40)
#define MUIM_CManager_SetListPrefs       CMTAG(41)
#define MUIM_CManager_MiniMailWindow     CMTAG(42)
#define MUIM_CManager_GetMap             CMTAG(43)

/* Attributes */
#define MUIA_CManager_ListObject         CMTAG(0)  /* [..G.], Object *         */
#define MUIA_CManager_Changed            CMTAG(2)  /* [.SGN], BOOL             */
#define MUIA_CManager_Path               CMTAG(3)  /* [..G.], STRPTR           */
#define MUIA_CManager_NoMenu             CMTAG(4)  /* [IS..], BOOL             */
#define MUIA_CManager_HideUsers          CMTAG(5)  /* [I...], BOOL             */
#define MUIA_CManager_HideWWW            CMTAG(6)  /* [I...], BOOL             */
#define MUIA_CManager_HideFTP            CMTAG(7)  /* [I...], BOOL             */
#define MUIA_CManager_HideChat           CMTAG(8)  /* [I...], BOOL             */
#define MUIA_CManager_CMData             CMTAG(9)  /* [..G.], struct CMData *  */
#define MUIA_CManager_AppDoubleClick     CMTAG(10) /* [IS..], struct Hook *    */
#define MUIA_CManager_HideTelnet         CMTAG(11) /* [I...], BOOL             */
#define MUIA_CManager_TreeObject         CMTAG(12) /* [..G.], Object *         */
#define MUIA_CManager_AppObj             CMTAG(13) /* [IS..], Object *         */
#define MUIA_CManager_WinObj             CMTAG(14) /* [IS..], Object *         */
#define MUIA_CManager_CMObj              CMTAG(15) /* [IS..], Object *         */
#define MUIA_CManager_IsCMWin            CMTAG(16) /* [..G.], BOOL             */
#define MUIA_CManager_Prefs              CMTAG(17) /* [ISG.], struct CMPrefs * */
#define MUIA_CManager_AtHome             CMTAG(18) /* [I...], BOOL             */
#define MUIA_CManager_DoubleClickObject  CMTAG(19) /* [IS..], Object *         */
#define MUIA_CManager_NoBar              CMTAG(20) /* [I...], BOOL             */
#define MUIA_CManager_NoGroupsCtrl       CMTAG(21) /* [I...], BOOL             */
#define MUIA_CManager_SortCol0           CMTAG(22) /* [.S..], LONG             */
#define MUIA_CManager_Version            CMTAG(23) /* [..G.], STRPTR           */
#define MUIA_CManager_ButtonsBarPrefs    CMTAG(24) /* [..G.], struct VisualPrefs * */
#define MUIA_CManager_ButtonsBar         CMTAG(25) /* [..G.], Object *         */
#define MUIA_CManager_ActiveEditObject   CMTAG(26) /* [..G.], Object *         */
#define MUIA_CManager_NoQuick            CMTAG(27) /* [I...], BOOL             */
#define MUIA_CManager_Quit               CMTAG(28) /* [..G.], BOOL             */
#define MUIA_CManager_NoCard             CMTAG(29) /* [..G.], BOOL             */

#define MUIA_CManager_DisableQuick       CMTAG(30) /* [I...], BOOL             */
#define MUIA_CManager_DisableCard        CMTAG(31) /* [I...], BOOL             */

/* Structures */

struct MUIP_CManager_LoadData
{
    ULONG  MethodID;
    STRPTR User;    /* !User -> Current, User & Path are mutually exclusive */
    STRPTR Path;
};

struct MUIP_CManager_SaveData
{
    ULONG  MethodID;
    STRPTR Path;
};

struct MUIP_CManager_AddEntry
{
    ULONG          MethodID;
    struct CMEntry *Entry;
    ULONG          Flags;
};

struct MUIP_CManager_AddGroup
{
    ULONG  MethodID;
    STRPTR name;
};

/* Flags */
enum
{
    MUIV_CManager_AddEntry_CurrentGroup = 1<<0,
};

struct MUIP_CManager_SetGroup
{
    ULONG                         MethodID;
    struct MUI_NListtree_TreeNode *Node;
};

struct MUIP_CManager_ChangeSort
{
    ULONG MethodID;
    ULONG Column;
};

struct MUIP_CManager_DisposeObj
{
    ULONG  MethodID;
    Object *Object;
};

struct MUIP_CManager_Import
{
    ULONG  MethodID;
    ULONG  Type;
    STRPTR File;
    ULONG  Flags;
};
/* Import types */
enum
{
    MUIV_CManager_Import_Voyager,
    MUIV_CManager_Import_IB,
    MUIV_CManager_Import_AWeb,
    MUIV_CManager_Import_Users_CSV,
    MUIV_CManager_Import_AmFTP,
    MUIV_CManager_Import_THOR,
    MUIV_CManager_Import_MD,
    MUIV_CManager_Import_YAM,
    MUIV_CManager_Import_PINT,
    MUIV_CManager_Import_GoodNews,
    MUIV_CManager_Import_STRICQ,
    MUIV_CManager_Import_OldSTFax,
    MUIV_CManager_Import_STFax,
    MUIV_CManager_Import_AmIRC,
    MUIV_CManager_Import_DOpusFTP,
    MUIV_CManager_Import_DFA,
    MUIV_CManager_Import_Netscape,
    MUIV_CManager_Import_Users_CSVFullOE,
    MUIV_CManager_Import_XML,
    MUIV_CManager_Import_SimpleMail,
};

/* flags */
enum
{
    MUIV_CManager_Import_AvoidDuplicateGroups  = 1<<0, /* Avoid duplicate groups  */
    MUIV_CManager_Import_AvoidDuplicateEntries = 1<<1, /* Avoid duplicate entries */
    MUIV_CManager_Import_OverwriteEntries      = 1<<2, /* Overwrite entries       */

    MUIV_CManager_Import_UTF8                  = 1<<3,
    MUIV_CManager_Import_Skip                  = 1<<4,

    MUIV_CManager_Import_AvoidDuplicate        = MUIV_CManager_Import_AvoidDuplicateGroups|MUIV_CManager_Import_AvoidDuplicateEntries,
};

struct MUIP_CManager_Export
{
    ULONG  MethodID;
    ULONG  Type;
    STRPTR File;
    ULONG  Flags;
};

/* Export types */
enum
{
    MUIV_CManager_Export_HTML_URLs,
    MUIV_CManager_Export_HTML_WWW,
    MUIV_CManager_Export_HTML_FTP,
    MUIV_CManager_Export_CSV_Users,

    MUIV_CManager_Export_YAM,
    MUIV_CManager_Export_AWeb,
    MUIV_CManager_Export_IBrowse,

    MUIV_CManager_Export_Netscape,
    MUIV_CManager_Export_CSVFullOE_Users,
    MUIV_CManager_Export_XML,
    MUIV_CManager_Export_SimpleMail,
};

/* Flags */
enum
{
    MUIV_CManager_Export_NoHead = 1<<0,
};


struct MUIS_CManager_IO
{
    BPTR           file;
    struct codeset *codeset;
    ULONG          flags;
    char           buf[2048];
    int            added;
};

struct MUIP_CManager_Login
{
    ULONG  MethodID;
    STRPTR user;
};

struct MUIP_CManager_Cleanup
{
    ULONG MethodID;
    ULONG OnlyEdit;
};

struct MUIP_CManager_SetPrefs
{
    ULONG          MethodID;
    struct CMPrefs *prefs;
    ULONG          flags;
};

/* flags */
enum
{
    MUIV_CManager_SetPrefs_Save        = 1<<0,
    MUIV_CManager_SetPrefs_Full        = 1<<1,
    MUIV_CManager_SetPrefs_OneWay      = 1<<2,
    MUIV_CManager_SetPrefs_SaveCurrent = 1<<3,
};

struct MUIP_CManager_ScanHotlists
{
    ULONG MethodID;
    ULONG mode;
    ULONG flags;
};

/* mode */
enum
{
    MUIV_CManager_ScanHotlists_Check     = 1<<0,
    MUIV_CManager_ScanHotlists_Import    = 1<<1,
    MUIV_CManager_ScanHotlists_SaveDates = 1<<2,
};

/* flags */
enum
{
    MUIV_CManager_ScanHotlists_IB       = 1<<HL_IB,
    MUIV_CManager_ScanHotlists_V        = 1<<HL_V,
    MUIV_CManager_ScanHotlists_AWEB     = 1<<HL_AWEB,
    MUIV_CManager_ScanHotlists_AMFTP    = 1<<HL_AMFTP,
    MUIV_CManager_ScanHotlists_MD2      = 1<<HL_MD2,
    MUIV_CManager_ScanHotlists_STFAX    = 1<<HL_STFAX,
    MUIV_CManager_ScanHotlists_YAM      = 1<<HL_YAM,
    MUIV_CManager_ScanHotlists_AMIRC    = 1<<HL_AMIRC,
    MUIV_CManager_ScanHotlists_DOPUS    = 1<<HL_DOPUS,
    MUIV_CManager_ScanHotlists_DFA      = 1<<HL_DFA,
    MUIV_CManager_ScanHotlists_PINT     = 1<<HL_PINT,
    MUIV_CManager_ScanHotlists_GOODNEWS = 1<<HL_GOODNEWS,
    MUIV_CManager_ScanHotlists_STRICQ   = 1<<HL_STRICQ,
    MUIV_CManager_ScanHotlists_NETSCAPE = 1<<HL_NETSCAPE,
};

struct MUIP_CManager_DoubleClick
{
    ULONG          MethodID;
    struct CMEntry *entry;
};

struct MUIP_CManager_WinClose
{
    ULONG MethodID;
    ULONG mode;
};

struct MUIP_CManager_RequesterResult
{
    ULONG  MethodID;
    Object *win;
    ULONG  type;
    ULONG  res;
};

/* type */
enum
{
    MUIV_CManager_Requester_Reload,
    MUIV_CManager_Requester_SaveChanges,
    MUIV_CManager_Requester_Close,
    MUIV_CManager_Requester_Asl,
    MUIV_CManager_Requester_DelWarning,
    MUIV_CManager_Requester_GroupDelWarning
};

struct MUIP_CManager_Close
{
    ULONG MethodID;
    ULONG mode;
};

/* mode */
enum
{
    MUIV_CManager_Close_Quit,
    MUIV_CManager_Close_Check,
    MUIV_CManager_Close_Save,
};

struct MUIP_CManager_Locate
{
    ULONG  MethodID;
    STRPTR contents;
};

struct MUIP_CManager_PrefsWindow
{
    ULONG MethodID;
    ULONG page;
};

struct MUIP_CManager_SetButtonsBarPrefs
{
    ULONG                  MethodID;
    struct ButtonsBarPrefs *prefs;
};

struct MUIP_CManager_SetListPrefs
{
    ULONG            MethodID;
    struct ListPrefs *prefs;
};

struct MUIP_CManager_SetFreeCol
{
    ULONG MethodID;
    ULONG type;
    ULONG col;
    ULONG val;
};

struct MUIP_CManager_MiniMailWindow
{
    ULONG          MethodID;
    struct CMEntry *entry;
};

struct MUIP_CManager_GetMap
{
    ULONG MethodID;
    ULONG type;
};

enum
{
    MUIV_CManager_GetMap_Home,
    MUIV_CManager_GetMap_Job,
};

/***********************************************************************/
/*
** PrefsWindow
*/

/* Methods */
#define MUIM_PrefsWindow_Open           CMTAG(50)
#define MUIM_PrefsWindow_GetCountry     CMTAG(51)
#define MUIM_PrefsWindow_SetString      CMTAG(52)
#define MUIM_PrefsWindow_SetDialerGroup CMTAG(53)
#define MUIM_PrefsWindow_HotsFill       CMTAG(54)
#define MUIM_PrefsWindow_HotsClear      CMTAG(55)

/* Structures */

/* MUIM_Cmanager_WinClose PrefsWindow mode */
enum
{
    MUIV_PrefsWindow_Close_Cancel,
    MUIV_PrefsWindow_Close_Apply,
    MUIV_PrefsWindow_Close_Use,
    MUIV_PrefsWindow_Close_Save,
};

struct MUIP_PrefsWindow_SetString
{
    ULONG  MethodID;
    Object *Obj;
    STRPTR *Strings;
    ULONG  Index;
};

/* Attributes */
#define MUIA_PrefsWindow_ActivePage  CMTAG(50)
#define MUIA_PrefsWindow_BarPosition CMTAG(51)

enum
{
    MUIV_PrefsWindow_ActivePage_Options,
    MUIV_PrefsWindow_ActivePage_Locale,
    MUIV_PrefsWindow_ActivePage_ButtonsBar,
    MUIV_PrefsWindow_ActivePage_Programs,
    MUIV_PrefsWindow_ActivePage_Dialer,
    MUIV_PrefsWindow_ActivePage_Hotlists,

    MUIV_PrefsWindow_ActivePage_NoPage = 255,
};


/***********************************************************************/
/*
** Login
*/

/* Methods */
#define MUIM_LoginWindow_Acknowledge  CMTAG(60)
#define MUIM_LoginWindow_ChangeActive CMTAG(61)
#define MUIM_LoginWindow_ActiveChange CMTAG(62)

/* Structures */

struct MUIP_LoginWindow_Acknowledge
{
    ULONG MethodID;
    ULONG mode;
};

/* mode */
enum
{
    MUIV_LoginWindow_Acknowledge_String,
    MUIV_LoginWindow_Acknowledge_List,
};

struct MUIP_LoginWindow_ActiveChange
{
    ULONG  MethodID;
    STRPTR contents;
};

/* Attributes */
#define MUIA_LoginWindow_User CMTAG(60)

/***********************************************************************/
/*
** Search
*/

/* Methods */
#define MUIM_SearchWindow_Search CMTAG(70)

/* Structures */

struct MUIP_SearchWindow_Search
{
    ULONG MethodID;
    ULONG Type;
};

/* Attributes */
#define MUIA_SearchWindow_HideUsers  CMTAG(70)
#define MUIA_SearchWindow_HideWWW    CMTAG(71)
#define MUIA_SearchWindow_HideFTP    CMTAG(72)
#define MUIA_SearchWindow_HideChat   CMTAG(73)
#define MUIA_SearchWindow_HideTelnet CMTAG(74)

/***********************************************************************/
/*
** DialWindow
*/

/* Methods */
#define MUIM_DialWindow_Open  CMTAG(80)

/* Structures */

struct MUIP_DialWindow_Open
{
    ULONG  MethodID;
    STRPTR Number;
    Object *MainWnd;
};

/***********************************************************************/
/*
** Edit
*/

/* Methods */
#define MUIM_Edit_Update              CMTAG(90)
#define MUIM_Edit_URL                 CMTAG(91)
#define MUIM_Edit_Phone               CMTAG(92)
#define MUIM_Edit_GetImage            CMTAG(93)
#define MUIM_Edit_GetVisualInfo       CMTAG(94)
#define MUIM_Edit_ChangeVisualName    CMTAG(95)
#define MUIM_Edit_ChangeEMail         CMTAG(96)
#define MUIM_Edit_EMailChange         CMTAG(97)
#define MUIM_Edit_FTPAdvanced         CMTAG(98)
#define MUIM_Edit_GoURL               CMTAG(99)
#define MUIM_Edit_GetMap              CMTAG(100)

/* Structures */

struct MUIP_Edit_Update
{
    ULONG MethodID;
    ULONG Refresh;
};

struct MUIP_Edit_Phone
{
    ULONG  MethodID;
    Object *Gadget;
};

struct MUIP_Edit_URL
{
    ULONG  MethodID;
    Object *URL;
    ULONG  Type;
};

/* Type */
enum
{
    URL_WWW,
    URL_FTP,
};

struct MUIP_Edit_GetVisualInfo
{
    ULONG                       MethodID;
    struct MUIS_Edit_VisualInfo *info;
};

struct MUIP_Edit_ChangeVisualName
{
    ULONG  MethodID;
    ULONG  what;
    STRPTR val;
};

enum
{
    MUIV_Edit_ChangeVisualName_Name,
    MUIV_Edit_ChangeVisualName_LastName,
    MUIV_Edit_ChangeVisualName_VisualName,
};

struct MUIS_Edit_VisualInfo
{
    STRPTR VisualName;
    STRPTR Name;
    STRPTR LastName;
    STRPTR Alias;
};

struct MUIP_Edit_FTPAdvanced
{
    ULONG MethodID;
    ULONG val;
};

struct MUIP_Edit_GoURL
{
    ULONG  MethodID;
    Object *URL;
};

/* Attributes */
#define MUIA_Edit_List         CMTAG(90)
#define MUIA_Edit_Entry        CMTAG(91)
#define MUIA_Edit_PrefsCountry CMTAG(92)

/***********************************************************************/
/*
** Entrylist
*/

/* Methods */
#define MUIM_EntryList_Remove      CMTAG(100)
#define MUIM_EntryList_Clone       CMTAG(101)
#define MUIM_EntryList_StartScript CMTAG(102)
#define MUIM_EntryList_Edit        CMTAG(103)
#define MUIM_EntryList_DoubleClick CMTAG(104)
#define MUIM_EntryList_HandleARexx CMTAG(105)
#define MUIM_EntryList_CopyToClip  CMTAG(106)
#define MUIM_EntryList_Locate      CMTAG(36)
#define MUIM_EntryList_Sort        CMTAG(107)

/* Structures */

struct MUIP_EntryList_StartScript
{
    ULONG MethodID;
    ULONG Script;
};

struct MUIP_EntryList_Locate
{
    ULONG  MethodID;
    STRPTR contents;
};

/* Attributes */
#define MUIA_EntryList_Group          CMTAG(100)
#define MUIA_EntryList_Type           CMTAG(101)
#define MUIA_EntryList_NoMenu         CMTAG(102)
#define MUIA_EntryList_ListPrefs      CMTAG(103)

enum
{
    SCP_WWW = 0,
    SCP_FTP,
    SCP_EMAIL,
    SCP_PHONE,
    SCP_FAX,
    SCP_CHAT,
    SCP_DOPUSFTP,
    SCP_MOBILE,
    SCP_BUSINESS_EMAIL,
    SCP_BUSINESS_FAX,
    SCP_BUSINESS_PHONE,
    SCP_BUSINESS_MOBILE,
};

/***********************************************************************/
/*
** EditWindow
*/

/* Methods */
#define MUIM_EditWindow_Open CMTAG(110)

/* Structures */

struct MUIP_EditWindow_Open
{
    ULONG          MethodID;
    struct CMEntry *Entry;
};

/* Attributes */
#define MUIA_EditWindow_Title     CMTAG(110)
#define MUIA_EditWindow_Object    CMTAG(111)
#define MUIA_EditWindow_List      CMTAG(112)
#define MUIA_EditWindow_ID        CMTAG(113)
#define MUIA_EditWindow_Entry     CMTAG(114)
#define MUIA_EditWindow_Virtual   CMTAG(115)
#define MUIA_EditWindow_IsEditWin CMTAG(116)

/***********************************************************************/
/*
** Grouplist
*/

/* Methods */
#define MUIM_GroupList_Arrow        CMTAG(120)
#define MUIM_GroupList_InsertGroups CMTAG(121)
#define MUIM_GroupList_RebuildList  CMTAG(122)
#define MUIM_GroupList_RebuildTree  CMTAG(123)
#define MUIM_GroupList_Clear        CMTAG(124)
#define MUIM_GroupList_GetActive    CMTAG(125)
#define MUIM_GroupList_GetRoot      CMTAG(126)
#define MUIM_GroupList_Sort         CMTAG(127)

/* Structures */

struct MUIP_GroupList_Arrow
{
    ULONG MethodID;
    ULONG Up;
};

struct MUIP_GroupList_InsertGroups
{
    ULONG         MethodID;
    struct CMData *CMData;
    ULONG         Flags;
};

enum
{
    MUIV_GroupList_HideUsers  = 1<<0,
    MUIV_GroupList_HideWWW    = 1<<1,
    MUIV_GroupList_HideFTP    = 1<<2,
    MUIV_GroupList_HideChat   = 1<<3,
    MUIV_GroupList_HideTelnet = 1<<4,
};

struct MUIP_GroupList_RebuildTree
{
    ULONG         MethodID;
    struct CMData *CMData;
    ULONG         Flags;
};

enum
{
    MUIV_RebuildTree_HideUsers  = 1<<0,
    MUIV_RebuildTree_HideWWW    = 1<<1,
    MUIV_RebuildTree_HideFTP    = 1<<2,
    MUIV_RebuildTree_HideChat   = 1<<3,
    MUIV_RebuildTree_HideTelnet = 1<<4,
};

struct MUIP_GroupList_RebuildList
{
    ULONG                         MethodID;
    Object                        *List;
    struct MUI_NListtree_TreeNode *tn;
};

struct MUIP_GroupList_GetRoot
{
    ULONG                         MethodID;
    struct MUI_NListtree_TreeNode *tn;
};

/* Attributes */
#define MUIA_GroupList_CMData CMTAG(120)

/***********************************************************************/

struct CMCountry
{
    struct CMCountry *Succ;
    struct CMCountry *Pred;
    char             Country[80];
    char             Country2[80];
    char             Code[8];
};

/***********************************************************************/

#if defined(__GNUC__)
# pragma pack()
#endif

#endif /* MUI_CMANAGER_MCC_H */
