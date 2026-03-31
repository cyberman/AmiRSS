
#include "rss.h"
#include <mui/BWin_mcc.h>
#define CATCOMP_NUMBERS
#include "loc.h"
#include "amrss_rev.h"

/***********************************************************************/

struct data
{
    struct prefs                prefs;

    Object                      *strip;
    Object                      *win;
    Object                      *prefsWin;
    Object                      *aboutMUI;
    Object                      *CMWin;
    Object                      *miniMailWin;
    Object                      *group;
    Object                      *groups;

    struct DiskObject           *icon;
    struct DiskObject           *alertIcon;

    int                         sig;
    struct MUI_InputHandlerNode rxHandler;
    struct MsgPort              rxPort;
    ULONG                       rxOut;
    BPTR                        console;
    struct MsgPort              notifyPort;

    ULONG                       timMillis;

    UBYTE                       *base;
    UBYTE                       wtitle[DEF_WINTITLESIZE];
    UBYTE                       stitle[DEF_SCREENTITLESIZE];

    ULONG                       sleep;

    ULONG                       flags;
};

enum
{
    FLG_Disposing = 1<<0,
    FLG_AlertIcon = 1<<1,
    FLG_SavePrefs = 1<<2,
};

/***********************************************************************/

enum
{
    SSL_S, /* STRPTR */
    SSL_B, /* BOOL   */
    SSL_N, /* ULONG  */
};

static void
makeName(STRPTR buf,STRPTR stem,STRPTR field,ULONG bufSize)
{
    register STRPTR t, f;
    register int    i;

    for (t = buf, f = stem, i = 2; (i<bufSize) && (*t++=  toupper(*f)); i++, f++);

    if (i<bufSize)
    {
        *(t-1) = '.';
        i++;
    }

    for (f = field; (i<bufSize) && (*t++ = toupper(*f)); i++, f++);

    *t = 0;
}

static ULONG
setStemList(struct RexxMsg *msg,STRPTR stem,...)
{
    va_list ap;
    UBYTE   buf[512];
    STRPTR  field;

    va_start(ap,stem);

    while (field = va_arg(ap,STRPTR))
    {
        UBYTE  		num[16];
        ULONG  		res, type, value;
        register STRPTR v = NULL; //gcc

        type  = va_arg(ap,ULONG);
        value = va_arg(ap,ULONG);

        makeName(buf,stem,field,sizeof(buf));

        switch (type)
        {
            case SSL_S:
                v = value ? (STRPTR)value : (STRPTR)"";
                break;

            case SSL_B:
                v = value ? "1" : "0";
                break;

            case SSL_N:
                msprintf(num,"%ld",value);
                v = num;
                break;
        }

        if (res = setrxvar(msg,buf,v,strlen(v))) return res;
    }

    va_end(va);

    return 0;
}

/***********************************************************************/

static LONG
execMacro(Object *obj,struct data *data,STRPTR macro,ULONG useConsole)
{
    struct RexxMsg *rxMsg;
    STRPTR         myPort;

    get(obj,MUIA_Application_Base,&myPort);

    if ((data->sig>=0) && (rxMsg = CreateRexxMsg(&data->rxPort,"REXX",myPort)))
    {
        if (rxMsg->rm_Args[0] = CreateArgstring(macro,strlen(macro)))
        {
            struct MsgPort *dest;

            if (useConsole && !data->console)
            {
                struct Process  *this;
                APTR            win;
                BPTR            l, c = 0; //gcc

                this = (struct Process *)FindTask(NULL);
                win = this->pr_WindowPtr;
                this->pr_WindowPtr = (APTR)-1;

                if (!(l = Lock(data->prefs.console,SHARED_LOCK)))
                {
                    if (c = Open(data->prefs.console,MODE_NEWFILE))
                    {
                        if (!IsInteractive(c))
                        {
                            Close(c);
                            DeleteFile(data->prefs.console);
                            c = NULL;
                        }
                    }
                }
                else UnLock(l);

                this->pr_WindowPtr = win;

                data->console = c;
            }

            rxMsg->rm_Action  = RXCOMM;
            rxMsg->rm_FileExt = "amrss";
            rxMsg->rm_Stdin   = data->console;
            rxMsg->rm_Stdout  = data->console;

            Forbid();
            if (dest = FindPort("REXX")) PutMsg(dest,(struct Message *)rxMsg);
            Permit();

            if (dest)
            {
                data->rxOut++;
                return 0;
            }
            else
            {
                DeleteArgstring(rxMsg->rm_Args[0]);
                DeleteRexxMsg(rxMsg);
            }
        }
    }

    return 5;
}

/***********************************************************************/

#ifdef __MORPHOS__
static LONG
rxFun(void)
{
    struct Hook    *hook = (struct Hook *)REG_A0;
    Object         *app = (Object *)REG_A2;
    struct RexxMsg *msg = (struct RexxMsg *)REG_A1;
#else
static LONG SAVEDS ASM
rxfun(REG(a0,struct Hook *hook),REG(a2,Object *app),REG(a1,struct RexxMsg *msg))
{
#endif
    struct data *data = hook->h_Data;

    return execMacro(app,data,msg->rm_Args[0],FALSE);
}

#ifdef __MORPHOS__
static struct EmulLibEntry rxTrap = {TRAP_LIB,0,(void (*)(void))rxFun};
static struct Hook rxHook = {0,0,(HOOKFUNC)&rxTrap};
#else
static struct Hook rxHook = {0,0,(HOOKFUNC)&rxfun};
#endif

/***********************************************************************/

static LONG
setFeed(Object *app,struct feed *feed,STRPTR stem)
{
    ULONG res;

    if (stem)
    {
        struct RexxMsg *rxMsg;
        ULONG 	       group = feed->flags & FFLG_Group;

        get(app,MUIA_Application_RexxMsg,&rxMsg);

        res = setStemList(rxMsg,stem,
                    "NAME",        SSL_S, feed->name,
                    "GROUP",       SSL_B, group,
                    NULL);

        if (!res && !group)
        {
            res = setStemList(rxMsg,stem,
                    "URL",         SSL_S, feed->URL,
                    "AUTHOR",      SSL_S, feed->info.author,
                    "SUBJECT",     SSL_S, feed->info.subject,
                    "TITLE",       SSL_S, feed->info.title,
                    "DESCR",       SSL_S, feed->info.descr,
                    "LINK",        SSL_S, feed->info.link,
                    "DATE.DAYS",   SSL_N, feed->info.date.ds_Days,
                    "DATE.MINUTE", SSL_N, feed->info.date.ds_Minute,
                    "DATE.TICK",   SSL_N, feed->info.date.ds_Tick,
                    NULL);
        }
    }
    else res = 0;

    return (LONG)res;
}

/***********************************************************************/

#ifdef __MORPHOS__
static LONG
findByNameFun(void)
{
    struct Hook *hook = (struct Hook *)REG_A0;
    Object      *app = (Object *)REG_A2;
    ULONG       *args = (ULONG *)REG_A1;
#else
static LONG SAVEDS ASM
findByNameFun(REG(a0,struct Hook *hook),REG(a2,Object *app),REG(a1,ULONG *args))
{
#endif
    struct data                   *data = hook->h_Data;
    struct MUI_NListtree_TreeNode *tn;
    LONG                          res;

    if (tn = (struct MUI_NListtree_TreeNode *)DoMethod(data->groups,MUIM_GroupList_FindName,args[0]))
    {
        if (args[1]) set(data->groups,MUIA_NListtree_Active,tn);

        if (args[2]) res = setFeed(app,tn->tn_User,(STRPTR)args[2]);
        else res = 0;

        if (!(tn->tn_Flags & TNF_LIST) && args[3])
            DoMethod(data->groups,MUIM_GroupList_Feed,MUIV_GroupList_Feed_Remote,(ULONG)tn);
    }
    else res = 5;

    return res;
}

#ifdef __MORPHOS__
static struct EmulLibEntry findByNameTrap = {TRAP_LIB,0,(void (*)(void))findByNameFun};
static struct Hook findByNameHook = {0,0,(HOOKFUNC)&findByNameTrap};
#else
static struct Hook findByNameHook = {0,0,(HOOKFUNC)&findByNameFun};
#endif

/***********************************************************************/

#ifdef __MORPHOS__
static LONG
findByURLFun(void)
{
    struct Hook *hook = (struct Hook *)REG_A0;
    Object      *app = (Object *)REG_A2;
    ULONG       *args = (ULONG *)REG_A1;
#else
static LONG SAVEDS ASM
findByURLFun(REG(a0,struct Hook *hook),REG(a2,Object *app),REG(a1,ULONG *args))
{
#endif
    struct data                   *data = hook->h_Data;
    struct MUI_NListtree_TreeNode *tn;
    LONG                          res;

    if (tn = (struct MUI_NListtree_TreeNode *)DoMethod(data->groups,MUIM_GroupList_FindURL,args[0]))
    {
        if (args[1]) set(data->groups,MUIA_NListtree_Active,tn);

        if (args[2]) res = setFeed(app,tn->tn_User,(STRPTR)args[2]);
        else res = 0;

        if (!(tn->tn_Flags & TNF_LIST) && args[3])
            DoMethod(data->groups,MUIM_GroupList_Feed,MUIV_GroupList_Feed_Remote,(ULONG)tn);
    }
    else res = 5;

    return res;
}

#ifdef __MORPHOS__
static struct EmulLibEntry findByURLTrap = {TRAP_LIB,0,(void (*)(void))findByURLFun};
static struct Hook findByURLHook = {0,0,(HOOKFUNC)&findByURLTrap};
#else
static struct Hook findByURLHook = {0,0,(HOOKFUNC)&findByURLFun};
#endif

/***********************************************************************/

#ifdef __MORPHOS__
static LONG
getFeedFun(void)
{
    struct Hook *hook = (struct Hook *)REG_A0;
    Object      *app = (Object *)REG_A2;
    ULONG       *args = (ULONG *)REG_A1;
#else
static LONG SAVEDS ASM
getFeedFun(REG(a0,struct Hook *hook),REG(a2,Object *app),REG(a1,ULONG *args))
{
#endif
    struct data          	  *data = hook->h_Data;
    struct MUI_NListtree_TreeNode *tn;
    LONG                 	  res;

    DoMethod(data->groups,MUIM_NList_GetEntry,GETNUM(args[0]),(ULONG)&tn);
    if (tn)
    {
        if (args[1]) set(data->groups,MUIA_NListtree_Active,tn);

        if (args[2]) res = setFeed(app,tn->tn_User,(STRPTR)args[2]);
        else res = 0;

        if (!(tn->tn_Flags & TNF_LIST) && args[3])
            DoMethod(data->groups,MUIM_GroupList_Feed,MUIV_GroupList_Feed_Remote,(ULONG)tn);
    }
    else res = 5;

    return res;
}

#ifdef __MORPHOS__
static struct EmulLibEntry getFeedTrap = {TRAP_LIB,0,(void (*)(void))getFeedFun};
static struct Hook getFeedHook = {0,0,(HOOKFUNC)&getFeedTrap};
#else
static struct Hook getFeedHook = {0,0,(HOOKFUNC)&getFeedFun};
#endif

/***********************************************************************/

#ifdef __MORPHOS__
static LONG
addFeedFun(void)
{
    struct Hook *hook = (struct Hook *)REG_A0;
    //Object      *app = (Object *)REG_A2;
    ULONG       *args = (ULONG *)REG_A1;
#else
static LONG SAVEDS ASM
addFeedFun(REG(a0,struct Hook *hook),REG(a2,Object *app),REG(a1,ULONG *args))
{
#endif
    struct data                   *data = hook->h_Data;
    struct MUI_NListtree_TreeNode *tn;
    LONG                          res = 5;

    tn = (struct MUI_NListtree_TreeNode *)DoMethod(data->groups,MUIM_GroupList_AddFeed,args[0],args[1],args[2]);
    if (tn)
    {
        if (args[3]) DoMethod(data->groups,MUIM_GroupList_Feed,(ULONG)MUIV_GroupList_Feed_Remote,(ULONG)MUIV_GroupList_Feed_Active);
        else if (args[4]) DoMethod(data->groups,MUIM_GroupList_EditFeed,0);
        res = 0;
    }

    return res;
}

#ifdef __MORPHOS__
static struct EmulLibEntry addFeedTrap = {TRAP_LIB,0,(void (*)(void))addFeedFun};
static struct Hook addFeedHook = {0,0,(HOOKFUNC)&addFeedTrap};
#else
static struct Hook addFeedHook = {0,0,(HOOKFUNC)&addFeedFun};
#endif

/***********************************************************************/

#ifdef __MORPHOS__
static LONG
toFrontFun(void)
{
    struct Hook *hook = (struct Hook *)REG_A0;
    //Object      *app = (Object *)REG_A2;
    //ULONG       *args = (ULONG *)REG_A1;
#else
static LONG SAVEDS ASM
toFrontFun(REG(a0,struct Hook *hook),REG(a2,Object *app),REG(a1,ULONG *args))
{
#endif
    struct data *data = hook->h_Data;

    DoMethod(data->win,MUIM_Window_ScreenToFront);
    DoMethod(data->win,MUIM_Window_ToFront);
    set(data->win,MUIA_Window_Activate,TRUE);

    return 0;
}

#ifdef __MORPHOS__
static struct EmulLibEntry toFrontTrap = {TRAP_LIB,0,(void (*)(void))toFrontFun};
static struct Hook toFrontHook = {0,0,(HOOKFUNC)&toFrontTrap};
#else
static struct Hook toFrontHook = {0,0,(HOOKFUNC)&toFrontFun};
#endif

/***********************************************************************/

#ifdef __MORPHOS__
static LONG
goToURLFun(void)
{
    struct Hook *hook = (struct Hook *)REG_A0;
    //Object      *app = (Object *)REG_A2;
    ULONG       *args = (ULONG *)REG_A1;
#else
static LONG SAVEDS ASM
goToURLFun(REG(a0,struct Hook *hook),REG(a2,Object *app),REG(a1,ULONG *args))
{
#endif
    struct data  *data = hook->h_Data;

    DoMethod(data->group,MUIM_MainGroup_UserURL,args[0],FALSE);

    return 0;
}

#ifdef __MORPHOS__
static struct EmulLibEntry goToURLTrap = {TRAP_LIB,0,(void (*)(void))goToURLFun};
static struct Hook goToURLHook = {0,0,(HOOKFUNC)&goToURLTrap};
#else
static struct Hook goToURLHook = {0,0,(HOOKFUNC)&goToURLFun};
#endif

/***********************************************************************/

static struct MUI_Command ARexxCmds[] =
{
    {"ADDFEED",    "NAME/A,URL/A,IMAGE/K,FEED/S,EDIT/S", 4, &addFeedHook,    0},
    {"FINDBYNAME", "NAME/A,ACTIVATE/S,STEM/K,FEED/S",    4, &findByNameHook, 0},
    {"FINDBYURL",  "NAME/A,ACTIVATE/S,STEM/K,FEED/S",    4, &findByURLHook,  0},
    {"GETENTRY",   "NUM/A/N,ACTIVATE/S,STEM/K,FEED/S",   4, &getFeedHook,    0},
    {"GOTOURL",    "URL/A,",                             1, &goToURLHook,    0},
    {"TOFRONT",    "",                                   0, &toFrontHook,    0},

    NULL
};

static STRPTR usedClasses[] =
{
    "Busy.mcc",
    "BWin.mcc",
    "HTMLview.mcc",
    "Lamp.mcc ",
    "NListtree.mcc ",
    "NListviews.mcc ",
    "Textinput.mcc",
    "TheBar.mcc",

    NULL
};

static struct NewMenu appMenu[] =
{
    MTITLE(MSG_Menu_Project),
        MITEM(MSG_Menu_About,MSG_Menu_About),
        MITEM(MSG_Menu_AboutMUI,MSG_Menu_AboutMUI),
        MBAR,
        MITEM(MSG_Menu_Help,MSG_Menu_Help),
        MBAR,
        MITEM(MSG_Menu_Hide,MSG_Menu_Hide),
        MBAR,
        MITEM(MSG_Menu_Quit,MSG_Menu_Quit),
        MITEM(MSG_Menu_QuickQuit,MSG_Menu_QuickQuit),

    MTITLE(MSG_Menu_Feeds),
        MITEM(MSG_Menu_AddGroup,MSG_Menu_AddGroup),
        MITEM(MSG_Menu_AddFeed,MSG_Menu_AddFeed),
        MITEM(MSG_Menu_EditFeed,MSG_Menu_EditFeed),
        MITEM(MSG_Menu_Remove,MSG_Menu_Remove),
        MITEM(MSG_Menu_UpdateAll,MSG_Menu_UpdateAll),
        MBAR,
        MITEM(MSG_Menu_NewsPage,MSG_Menu_NewsPage),
        MITEM(MSG_Menu_HTMLPage,MSG_Menu_HTMLPage),
        MBAR,
        MTITEM(MSG_Menu_NoDragDrop,MSG_Menu_NoDragDrop),
        MTITEM(MSG_Menu_NoAutoUpdate,MSG_Menu_NoAutoUpdate),
        MBAR,
        MTITEM(MSG_Menu_HideBar,MSG_Menu_HideBar),
        MTITEM(MSG_Menu_HideGroups,MSG_Menu_HideGroups),
        MTITEM(MSG_Menu_HidePreview,MSG_Menu_HidePreview),
        MBAR,
        MITEM(MSG_Menu_FlushImages,MSG_Menu_FlushImages),
        MITEM(MSG_Menu_FlushCookies,MSG_Menu_FlushCookies),
        MBAR,
        MITEM(MSG_Menu_SaveFeeds,MSG_Menu_SaveFeeds),
        MITEM(MSG_Menu_EditFeeds,MSG_Menu_EditFeeds),
        MITEM(MSG_Menu_LastSaved,MSG_Menu_LastSaved),

    MTITLE(MSG_Menu_Settings),
        MITEM(MSG_Menu_EditPrefs,MSG_Menu_EditPrefs),
        MITEM(MSG_Menu_LastSavedPrefs,MSG_Menu_LastSavedPrefs),
        MBAR,
        MITEM(MSG_Menu_Snapshot,MSG_Menu_Snapshot),
        MITEM(MSG_Menu_UnSnapshot,MSG_Menu_UnSnapshot),
        MBAR,
        MITEM(MSG_Menu_ScreenMode,MSG_Menu_ScreenMode),
        MITEM(MSG_Menu_MUI,MSG_Menu_MUI),

    MEND
};

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    Object           *strip, *win, *group;
    struct parseArgs *pa;
    struct TagItem   *attrs = msg->ops_AttrList;
    int              sig;

    if ((sig = AllocSignal(-1))<0) return 0;

    pa = (struct parseArgs *)GetTagData(MUIA_App_ParseArgs,NULL,attrs);

    pa = (struct parseArgs *)GetTagData(MUIA_App_ParseArgs,NULL,attrs);

    if (pa->flags & PAFLG_BWin) initBWinClass();

    if (obj = (Object *)DoSuperNew(cl,obj,
            MUIA_Application_Title,       (ULONG)PRG,
            MUIA_Application_Base,        (ULONG)DEF_BASE,
            MUIA_Application_Version,     (ULONG)&__ver[1],
            MUIA_Application_Copyright,   (ULONG)getString(MSG_App_Copyright),
            MUIA_Application_Description, (ULONG)getString(MSG_App_Description),
            MUIA_Application_Menustrip,   (ULONG)(strip = MUI_MakeObject(MUIO_MenustripNM,(ULONG)appMenu,MUIO_MenustripNM_CommandKeyCheck)),
            MUIA_Application_Commands,    (ULONG)ARexxCmds,
            MUIA_Application_UsedClasses, (ULONG)usedClasses,

            SubWindow, win = NewObject(bwinClass ? bwinClass->mcc_Class : winClass->mcc_Class,NULL,
                MUIA_HelpNode,                       (ULONG)"win.html",
                MUIA_Window_ID,                      MAKE_ID('W','M','A','I'),
                (pa->flags & PAFLG_Pubscreen) ? MUIA_Window_PublicScreen : TAG_IGNORE, (ULONG)pa->pubscreen,
            	MUIA_Window_AppWindow,		         TRUE,
                MUIA_Window_Backdrop,                pa->flags & PAFLG_Backdrop,
                MUIA_Window_UseBottomBorderScroller, !(pa->flags & PAFLG_Compact),
                MUIA_Window_UseRightBorderScroller,  !(pa->flags & PAFLG_Compact),
                MUIA_Win_Forward,                    TRUE,

                WindowContents, group = groupObject,
                    MUIA_RSS_URLInArgs,  pa->flags & PAFLG_URL,
                    MUIA_RSS_Compact,    pa->flags & PAFLG_Compact,
                End,
            End,

            TAG_MORE,(ULONG)attrs))
    {
        struct data        *data = INST_DATA(cl,obj);
        struct MUI_Command *acmd;
        Object             *o;

        /* Setup istance data */

        data->strip = strip;
        data->win   = win;
        data->group = group;

        superget(cl,obj,MUIA_Application_Base,&data->base);
        msnprintf(data->wtitle,sizeof(data->wtitle),getString(MSG_Win_Title),data->base);
        msnprintf(data->stitle,sizeof(data->stitle),getString(MSG_Screen_Title),data->base);
        SetAttrs(win,MUIA_Window_Title,(ULONG)data->wtitle,MUIA_Window_ScreenTitle,(ULONG)data->stitle,TAG_DONE);

        if (data->icon = GetDiskObject(DEF_ICON))
            superset(cl,obj,MUIA_Application_DiskObject,data->icon);

        data->alertIcon = GetDiskObject(DEF_ALERTICON);

        data->sig = sig;

        INITPORT(&data->rxPort,sig);
        INITPORT(&data->notifyPort,sig);

        data->rxHandler.ihn_Object         = obj;
        data->rxHandler.ihn_stuff.ihn_sigs = 1L<<sig;
        data->rxHandler.ihn_Method         = MUIM_App_HandleMsg;

        rxHook.h_Data  = data;
        superset(cl,obj,MUIA_Application_RexxHook,&rxHook);

        DoSuperMethod(cl,obj,MUIM_Application_AddInputHandler,(ULONG)&data->rxHandler);
        set(data->group,MUIA_RSS_NotifyPort,&data->notifyPort);

        for (acmd = ARexxCmds; acmd->mc_Name; acmd++)
            acmd->mc_Hook->h_Data = data;

        /* Window notifies */

        DoMethod(win,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,MUIV_Notify_Application,2,
            MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

        DoMethod(win,MUIM_Notify,MUIA_AppMessage,MUIV_EveryTime,(ULONG)group,2,
    	    MUIM_MainGroup_AppMessage,MUIV_TriggerValue);

        /* Project notifies */

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_About),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,MUIV_Notify_Application,5,MUIM_Application_ShowHelp,(ULONG)win,NULL,(ULONG)"about.html",0);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_AboutMUI),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,MUIV_Notify_Application,2,MUIM_Application_AboutMUI,(ULONG)win);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_Hide),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,MUIV_Notify_Application,3,MUIM_Set,MUIA_Application_Iconified,TRUE);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_Help),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,MUIV_Notify_Application,5,MUIM_Application_ShowHelp,(ULONG)win,NULL,(ULONG)"index.html",0);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_QuickQuit),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,MUIV_Notify_Application,1,MUIM_App_QuickQuit);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_Quit),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,MUIV_Notify_Application,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

        /* Feeds notifies */

        get(group,MUIA_RSS_Groups,&o);
        data->groups = o;

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_AddGroup),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,(ULONG)o,3,MUIM_GroupList_EditFeed,MUIV_GroupList_EditFeed_New|MUIV_GroupList_EditFeed_Group);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_AddFeed),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,(ULONG)o,3,MUIM_GroupList_EditFeed,MUIV_GroupList_EditFeed_New);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_EditFeed),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,(ULONG)o,3,MUIM_GroupList_EditFeed,0);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_Remove),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,(ULONG)o,1,MUIM_GroupList_Remove);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_UpdateAll),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,(ULONG)o,1,MUIM_GroupList_HandleTimer);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_NewsPage),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,(ULONG)group,2,MUIM_MainGroup_ActivePage,MUIV_MainGroup_ActivePage_News);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_HTMLPage),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,(ULONG)group,2,MUIM_MainGroup_ActivePage,MUIV_MainGroup_ActivePage_HTML);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_NoDragDrop),MUIM_Notify,
            MUIA_Menuitem_Checked,MUIV_EveryTime,MUIV_Notify_Application,3,MUIM_Set,MUIA_App_NoDragDrop,MUIV_TriggerValue);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_NoAutoUpdate),MUIM_Notify,
            MUIA_Menuitem_Checked,MUIV_EveryTime,MUIV_Notify_Application,3,MUIM_Set,MUIA_App_NoAutoUpdate,MUIV_TriggerValue);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_HideBar),MUIM_Notify,
            MUIA_Menuitem_Checked,MUIV_EveryTime,MUIV_Notify_Application,3,MUIM_Set,MUIA_App_HideBar,MUIV_TriggerValue);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_HideGroups),MUIM_Notify,
            MUIA_Menuitem_Checked,MUIV_EveryTime,MUIV_Notify_Application,3,MUIM_Set,MUIA_App_HideGroups,MUIV_TriggerValue);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_HidePreview),MUIM_Notify,
            MUIA_Menuitem_Checked,MUIV_EveryTime,MUIV_Notify_Application,3,MUIM_Set,MUIA_App_HidePreview,MUIV_TriggerValue);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_SaveFeeds),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,(ULONG)data->groups,1,MUIM_GroupList_SaveFeeds);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_EditFeeds),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,(ULONG)data->group,2,MUIM_MainGroup_Editor,(ULONG)"PROGDIR:etc/db/feeds");

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_LastSaved),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,(ULONG)obj,1,MUIM_App_LoadFeeds);

        /* Settings notifies */

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_EditPrefs),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,MUIV_Notify_Application,2,MUIM_App_PrefsWin,MUIV_Prefs_Page_Options);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_LastSavedPrefs),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,MUIV_Notify_Application,1,MUIM_App_LoadPrefs);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_Snapshot),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,(ULONG)win,2,MUIM_Window_Snapshot,1);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_UnSnapshot),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,(ULONG)win,2,MUIM_Window_Snapshot,0);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_FlushImages),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,(ULONG)group,1,MUIM_MainGroup_FlushImages);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_FlushCookies),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,(ULONG)group,1,MUIM_MainGroup_FlushCookies);

        if (MUIMasterBase->lib_Version>=20)
            DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_ScreenMode),MUIM_Notify,
    	        MUIA_Menuitem_Trigger,MUIV_EveryTime,MUIV_Notify_Application,2,MUIM_Application_OpenConfigWindow,MUIV_Application_OCW_ScreenPage);

        DoMethod((Object *)DoMethod(strip,MUIM_FindUData,MSG_Menu_MUI),MUIM_Notify,
            MUIA_Menuitem_Trigger,MUIV_EveryTime,MUIV_Notify_Application,2,MUIM_Application_OpenConfigWindow,0);

        /* Setup online help */

        superset(cl,obj,MUIA_Application_HelpFile,TRUE);

        DoSuperMethod(cl,obj,MUIM_Notify,MUIA_Application_MenuHelp,MUIV_EveryTime,MUIV_Notify_Self,
            5,MUIM_Application_ShowHelp,(ULONG)win,NULL,(ULONG)"menus.html",0);

        /* Setup the world */
        SetAttrs(data->group,MUIA_RSS_App,(ULONG)obj,MUIA_RSS_Win,(ULONG)win,TAG_DONE);

        /* Load prefs */
        DoMethod(obj,MUIM_App_LoadPrefs);
    }
    else FreeSignal(sig);

    return (ULONG)obj;
}

/***********************************************************************/

static ULONG
mDispose(struct IClass *cl,Object *obj,Msg msg)
{
    struct data          *data = INST_DATA(cl,obj);
    struct DiskObject    *icon = data->icon, *alertIcon = data->alertIcon;
    struct HTTPStatusMsg *sm;
    ULONG                res;
    ULONG                fq;

    data->flags |= FLG_Disposing;

    superget(cl,obj,MUIA_Application_ForceQuit,&fq);

    if (!fq)
    {
        struct List     *l;
        Object          *mstate;
        Object 		    *win;

        for (superget(cl,obj,MUIA_Application_WindowList,&l), mstate = (Object *)l->lh_Head; win = NextObject(&mstate); )
            if (win!=data->win) set(win,MUIA_Window_Open,FALSE);

        set(data->win,MUIA_Window_Open,FALSE);

        if (bwinClass)
        {
            ULONG bwinFlags = 0;

            if (xget(data->win,MUIA_BWin_AutoShow))
            {
                bwinFlags |= PBWFLG_Autoshow|PBWFLG_ShowDragBar|PBWFLG_ShowSize;
            }
            else
            {
                if (xget(data->win,MUIA_BWin_ShowDragBar)) bwinFlags |= PBWFLG_ShowDragBar;
                if (xget(data->win,MUIA_BWin_ShowSize)) bwinFlags |= PBWFLG_ShowSize;
            }

            if (xget(data->win,MUIA_BWin_DragBarTop)) bwinFlags |= PBWFLG_DragBarTop;

            if (bwinFlags!=data->prefs.bwinFlags)
            {
                data->prefs.bwinFlags = bwinFlags;
                data->flags |= FLG_SavePrefs;
            }
        }

        if (data->flags & FLG_SavePrefs) DoMethod(obj,MUIM_App_SavePrefs);

        DoMethod(data->groups,MUIM_GroupList_SaveFeeds);
    }

    DoSuperMethod(cl,obj,MUIM_Application_RemInputHandler,(ULONG)&data->rxHandler);
    set(data->group,MUIA_RSS_NotifyPort,NULL);

    while (sm = (struct HTTPStatusMsg *)GetMsg(&data->notifyPort))
        freeArbitratePooled(sm,sizeof(*sm));

    while (data->rxOut)
    {
        struct RexxMsg *rxMsg;

        WaitPort(&data->rxPort);

        if (rxMsg = (struct RexxMsg *)GetMsg(&data->rxPort))
        {
            DeleteArgstring(rxMsg->rm_Args[0]);
            DeleteRexxMsg(rxMsg);

            data->rxOut--;
        }
    }

    FreeSignal(data->sig);

    res = DoSuperMethodA(cl,obj,msg);

    if (icon) FreeDiskObject(icon);
    if (alertIcon) FreeDiskObject(alertIcon);

    return res;
}

/***********************************************************************/

static ULONG
mGet(struct IClass *cl,Object *obj,struct opGet *msg)
{
    struct data *data = INST_DATA(cl, obj);

    switch (msg->opg_AttrID)
    {
        case MUIA_Application_Sleep:    *msg->opg_Storage = (ULONG)data->sleep;      	 return TRUE;
        case MUIA_RSS_Win:              *msg->opg_Storage = (ULONG)data->win;            return TRUE;
        case MUIA_RSS_Prefs:            *msg->opg_Storage = (ULONG)&data->prefs;         return TRUE;
        case MUIA_RSS_BarPrefs:         *msg->opg_Storage = (ULONG)&data->prefs.bprefs;  return TRUE;
        case MUIA_RSS_NBarPrefs:        *msg->opg_Storage = (ULONG)&data->prefs.nbprefs; return TRUE;
    	case MUIA_App_ScreenTitle:      *msg->opg_Storage = (ULONG)&data->stitle;        return TRUE;

        default: return DoSuperMethodA(cl,obj,(Msg)msg);
    }
}

/***************************************************************************/

static ULONG
mSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct data    *data = INST_DATA(cl,obj);
    struct TagItem *tag;
    struct TagItem *tstate;
    ULONG          oneWay = FALSE, prefs = FALSE, bPos = FALSE, nbPos = FALSE,
    			   noDragDrop = FALSE, noAutoUpdate = FALSE, hideBar = FALSE,
                   hideGroups = FALSE, hidePreview = FALSE;

    for (tstate = msg->ops_AttrList; tag = NextTagItem(&tstate); )
    {
        ULONG tidata = tag->ti_Data;

        switch(tag->ti_Tag)
        {
            case MUIA_Application_Sleep:
                data->sleep += tidata ? 1 : -1;
                break;

            case MUIA_App_Title:
            {
                UBYTE *fmt;

                fmt = getString(tidata ? MSG_Win_TitleFeed : MSG_Win_Title);
                msnprintf(data->wtitle,sizeof(data->wtitle),fmt,data->base,tidata);

                fmt = getString(tidata ? MSG_Screen_TitleFeed : MSG_Screen_Title);
                msnprintf(data->stitle,sizeof(data->stitle),fmt,data->base,tidata);

                SetAttrs(data->win,MUIA_Window_Title,(ULONG)data->wtitle,MUIA_Window_ScreenTitle,(ULONG)data->stitle,TAG_DONE);

                break;
            }

            case MUIA_RSS_Prefs:
                if ((struct prefs *)tidata!=&data->prefs)
                    copymem(&data->prefs,(struct prefs *)(tidata),sizeof(data->prefs));
                prefs = TRUE;
                data->flags |= FLG_SavePrefs;
                break;

            case MUIA_RSS_BarPrefs:
                if ((struct bprefs *)tidata!=&data->prefs.bprefs)
                    copymem(&data->prefs.bprefs,(struct bprefs *)(tidata),sizeof(data->prefs.bprefs));
                prefs = TRUE;
                data->flags |= FLG_SavePrefs;
                break;

            case MUIA_RSS_NBarPrefs:
                if ((struct bprefs *)tidata!=&data->prefs.nbprefs)
                    copymem(&data->prefs.nbprefs,(struct bprefs *)(tidata),sizeof(data->prefs.nbprefs));
                prefs = TRUE;
                data->flags |= FLG_SavePrefs;
                break;

            case MUIA_RSS_BarPos:
                data->prefs.bprefs.bPos = tidata;
                bPos = TRUE;
                data->flags |= FLG_SavePrefs;
                break;

            case MUIA_RSS_NBarPos:
                data->prefs.nbprefs.bPos = tidata;
                nbPos = TRUE;
                data->flags |= FLG_SavePrefs;
                break;

            case MUIA_App_NoDragDrop:
                data->prefs.noDragDrop = tidata;
                noDragDrop = TRUE;
                data->flags |= FLG_SavePrefs;
                break;

            case MUIA_App_NoAutoUpdate:
                data->prefs.noAutoUpdate = tidata;
                noAutoUpdate = TRUE;
                data->flags |= FLG_SavePrefs;
                break;

            case MUIA_App_HideBar:
                data->prefs.hideBar = tidata;
                hideBar = TRUE;
                data->flags |= FLG_SavePrefs;
                break;

            case MUIA_App_HideGroups:
                data->prefs.hideGroups = tidata;
                hideGroups = TRUE;
                data->flags |= FLG_SavePrefs;
                break;

            case MUIA_App_HidePreview:
                data->prefs.hidePreview = tidata;
                hidePreview = TRUE;
                data->flags |= FLG_SavePrefs;
                break;

            case MUIA_RSS_OneWay:
                oneWay = TRUE;
                break;

            case MUIA_App_SavePrefs:
                data->flags |= FLG_SavePrefs;
                break;

            case MUIA_Application_Iconified:
                if (tidata)
                {
                    struct List     *l;
                    Object          *mstate;
                    Object 	    *win;

                    superset(cl,obj,MUIA_Application_Iconified,TRUE);

                    for (superget(cl,obj,MUIA_Application_WindowList,&l), mstate = (Object *)l->lh_Head; win = NextObject(&mstate); )
                        if (win!=data->win) set(win,MUIA_Window_Open,FALSE);

                    tag->ti_Tag=TAG_IGNORE;

                    break;
                }

            case MUIA_RSS_YouHaveNews:
                if (!BOOLSAME(data->flags & FLG_AlertIcon,tidata))
                {
                    ULONG iconified;

                    superget(cl,obj,MUIA_Application_Iconified,&iconified);

                    if (iconified)
                    {
                        set(data->win,MUIA_Window_Open,FALSE);
                        superset(cl,obj,MUIA_Application_Iconified,FALSE);
                    }

                    if (tidata)
                    {
                        data->flags |= FLG_AlertIcon;
                        if (data->alertIcon) superset(cl,obj,MUIA_Application_DiskObject,data->alertIcon);
                    }
                    else
                    {
                        data->flags &= ~FLG_AlertIcon;
                        if (data->icon) superset(cl,obj,MUIA_Application_DiskObject,data->icon);
                    }

                    if (iconified)
                    {
                        superset(cl,obj,MUIA_Application_Iconified,TRUE);
                        set(data->win,MUIA_Window_Open,TRUE);
                    }
                }
                break;
        }
    }

    if (prefs)
    {
        struct List     *l;
        Object          *mstate;
        Object 		*win;

        for (superget(cl,obj,MUIA_Application_WindowList,&l), mstate = (Object *)l->lh_Head; win = NextObject(&mstate); )
            if (!oneWay || (win!=data->prefsWin)) SetAttrs(win,MUIA_RSS_Prefs,(ULONG)&data->prefs,MUIA_RSS_OneWay,oneWay,TAG_DONE);
    }
    else
    {
        if ((bPos || nbPos) && data->prefsWin)
	    SetAttrs(data->prefsWin,bPos  ? MUIA_RSS_BarPos  : TAG_IGNORE,data->prefs.bprefs.bPos,
                                    nbPos ? MUIA_RSS_NBarPos : TAG_IGNORE,data->prefs.nbprefs.bPos,
                                    TAG_DONE);

        if (hideBar || hideGroups || hidePreview) set(data->group,MUIA_RSS_Prefs,&data->prefs);
        else if (noDragDrop || noAutoUpdate) set(data->groups,MUIA_RSS_Prefs,&data->prefs);
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static ULONG
mAppDisposeWin(struct IClass *cl,Object *obj,struct MUIP_App_DisposeWin *msg)
{
    struct data *data = INST_DATA(cl,obj);
    Object      *win = msg->win;

    set(win,MUIA_Window_Open,FALSE);
    DoSuperMethod(cl,obj,OM_REMMEMBER,(ULONG)win);
    MUI_DisposeObject(win);

    if (win==data->prefsWin) data->prefsWin = NULL;
    else if (win==data->CMWin) data->CMWin = NULL;
         else if (win==data->miniMailWin) data->miniMailWin = NULL;
              else if (win==data->aboutMUI) data->aboutMUI = NULL;

    return 0;
}

/***********************************************************************/

static ULONG
mHandleMsg(struct IClass *cl,Object *obj,Msg msg)
{
    struct data          *data = INST_DATA(cl,obj);
    struct HTTPStatusMsg *sm;
    struct RexxMsg       *rxMsg;
    ULONG                res = FALSE;

    while (sm = (struct HTTPStatusMsg *)GetMsg(&data->notifyPort))
    {
        DoMethod(data->group,MUIM_RSS_HTTPStatus,sm->type,sm->arg,sm->flags);
        freeArbitratePooled(sm,sizeof(*sm));
        res = TRUE;
    }

    while (rxMsg = (struct RexxMsg *)GetMsg(&data->rxPort))
    {
        DeleteArgstring(rxMsg->rm_Args[0]);
        DeleteRexxMsg(rxMsg);

        data->rxOut--;
        res = TRUE;
    }

    return res;
}

/***********************************************************************/

static ULONG
mAboutMUI(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    set(obj,MUIA_Application_Sleep,TRUE);

    if (!data->aboutMUI)
    {
        if (data->aboutMUI = AboutmuiObject,
                MUIA_Aboutmui_Application, obj,
                MUIA_Window_RefWindow,     data->win,
                MUIA_HelpNode,             "about.html",
            End)
        {
	    set(data->aboutMUI,MUIA_Window_ScreenTitle,data->stitle);
            DoMethod(data->aboutMUI,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,(ULONG)obj,5,
                MUIM_Application_PushMethod,(ULONG)obj,2,MUIM_App_DisposeWin,(ULONG)data->aboutMUI);
        }
    }

    openWindow(obj,data->aboutMUI);

    set(obj,MUIA_Application_Sleep,FALSE);

    return 0;
}

/***********************************************************************/

static ULONG
mOpenMUIConfigWindow(struct IClass *cl,Object *obj,Msg msg)
{
    ULONG res;

    set(obj,MUIA_Application_Sleep,TRUE);
    res = DoSuperMethodA(cl,obj,msg);
    set(obj,MUIA_Application_Sleep,FALSE);

    return res;
}

/***********************************************************************/

static ULONG
mPrefsWin(struct IClass *cl,Object *obj,struct MUIP_App_PrefsWin *msg)
{
    struct data *data = INST_DATA(cl,obj);

    set(obj,MUIA_Application_Sleep,TRUE);

    if (!data->prefsWin)
    {
        if ((prefsClass || initPrefsClass()) &&
            (GMTClass || initGMTClass()) &&
            (agentClass || initAgentClass()) &&
            (popphClass || initPopphClass()) &&
            (popupCodesetsClass || initPopupCodesetsClass()))
        {
            if (data->prefsWin = prefsObject,
                    MUIA_Window_RefWindow, data->win,
                End)
            {
                DoSuperMethod(cl,obj,OM_ADDMEMBER,(ULONG)data->prefsWin);
	            SetAttrs(data->prefsWin,MUIA_Window_ScreenTitle, (ULONG)data->stitle,
			                            MUIA_RSS_Prefs,          (ULONG)&data->prefs,
                                        TAG_DONE);
            }
        }
    }

    if (data->prefsWin)
        set(data->prefsWin,MUIA_RSS_Prefs_Page,msg->page);

    openWindow(obj,data->prefsWin);

    set(obj,MUIA_Application_Sleep,FALSE);

    return 0;
}

/***********************************************************************/

static ULONG
mCMWin(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    set(obj,MUIA_Application_Sleep,TRUE);

    if (!data->CMWin)
    {
        if (CMClass || initCMClass())
        {
            if (data->CMWin = CMObject,
                    MUIA_Window_RefWindow, data->win,
                    MUIA_RSS_App,              obj,
                End)
            {
                DoSuperMethod(cl,obj,OM_ADDMEMBER,(ULONG)data->CMWin);
	        SetAttrs(data->CMWin,MUIA_Window_ScreenTitle,(ULONG)data->stitle,
			             MUIA_RSS_Prefs,(ULONG)&data->prefs,
                                     TAG_DONE);
            }
        }
    }

    openWindow(obj,data->CMWin);

    set(obj,MUIA_Application_Sleep,FALSE);

    return 0;
}

/***********************************************************************/

static ULONG
mMiniMailWin(struct IClass *cl,Object *obj,struct MUIP_App_MiniMailWin *msg)
{
    struct data *data = INST_DATA(cl,obj);

    set(obj,MUIA_Application_Sleep,TRUE);

    if (!data->miniMailWin)
    {
        if (miniMailClass || initMiniMailClass())
        {
            if (data->miniMailWin = miniMailObject,
                    MUIA_Window_RefWindow, data->win,
                    MUIA_RSS_App,          obj,
                End)
            {
                DoSuperMethod(cl,obj,OM_ADDMEMBER,(ULONG)data->miniMailWin);
	        SetAttrs(data->miniMailWin,MUIA_Window_ScreenTitle,(ULONG)data->stitle,
			                   MUIA_RSS_Prefs,(ULONG)&data->prefs,
                                           TAG_DONE);
            }
        }
    }

    if (data->miniMailWin) set(data->miniMailWin,MUIA_MiniMail_Addr,msg->addr);
    openWindow(obj,data->miniMailWin);

    set(obj,MUIA_Application_Sleep,FALSE);

    return 0;
}

/***********************************************************************/

static ULONG
mLoadPrefs(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    loadPrefs(&data->prefs);
    nnset((Object *)DoMethod(data->strip,MUIM_FindUData,MSG_Menu_NoDragDrop),MUIA_Menuitem_Checked,data->prefs.noDragDrop);
    nnset((Object *)DoMethod(data->strip,MUIM_FindUData,MSG_Menu_NoAutoUpdate),MUIA_Menuitem_Checked,data->prefs.noAutoUpdate);
    nnset((Object *)DoMethod(data->strip,MUIM_FindUData,MSG_Menu_HideBar),MUIA_Menuitem_Checked,data->prefs.hideBar);
    nnset((Object *)DoMethod(data->strip,MUIM_FindUData,MSG_Menu_HideGroups),MUIA_Menuitem_Checked,data->prefs.hideGroups);
    nnset((Object *)DoMethod(data->strip,MUIM_FindUData,MSG_Menu_HidePreview),MUIA_Menuitem_Checked,data->prefs.hidePreview);
    set(obj,MUIA_RSS_Prefs,&data->prefs);
    data->flags &= ~FLG_SavePrefs;

    return 0;
}

/***********************************************************************/

static ULONG
mSavePrefs(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (savePrefs(&data->prefs))
    {
     	data->flags &= ~FLG_SavePrefs;
	return TRUE;
    }

    return FALSE;
}

/***********************************************************************/

static ULONG
mGoURL(struct IClass *cl,Object *obj,struct MUIP_RSS_GotoURL *msg)
{
    struct data *data = INST_DATA(cl,obj);

    return DoMethodA(data->group,(Msg)msg);
}

/***********************************************************************/

static ULONG
mHTTPStatus(struct IClass *cl,Object *obj,struct MUIP_RSS_HTTPStatus *msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (!(data->flags & FLG_Disposing)) return DoMethodA(data->group,(Msg)msg);
    else return 0;
}

/***********************************************************************/

static ULONG
mShowHelp(struct IClass *cl,Object *obj,struct MUIP_Application_ShowHelp *msg)
{
    struct data *data = INST_DATA(cl,obj);
    UBYTE       buf[DEF_FILESIZE];

    if (msg->node && *msg->node)
    {
        msprintf(buf,DEF_HELPFMT,(ULONG)msg->node);
        DoMethod(data->group,MUIM_RSS_GotoURL,(ULONG)buf,MUIV_RSS_GotoURL_IgnorePrefs|MUIV_RSS_GotoURL_NoTarget);
    }

    return 0;
}

/***********************************************************************/

static ULONG
mLoadFeeds(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    DoMethod(data->groups,MUIM_GroupList_LoadFeeds);
    DoMethod(data->groups,MUIM_GroupList_SetDefaultFeed);

    return 0;
}

/***********************************************************************/

static ULONG
mQuickQuit(struct IClass *cl,Object *obj,Msg msg)
{
    superset(cl,obj,MUIA_Application_ForceQuit,TRUE);
    DoSuperMethod(cl,obj,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

    return 0;
}

/***********************************************************************/

M_DISP(dispatcher)
{
    M_DISPSTART

    switch(msg->MethodID)
    {
        case OM_NEW:                            return mNew(cl,obj,(APTR)msg);
        case OM_DISPOSE:                        return mDispose(cl,obj,(APTR)msg);
        case OM_GET:                            return mGet(cl,obj,(APTR)msg);
        case OM_SET:                            return mSets(cl,obj,(APTR)msg);

        case MUIM_Application_AboutMUI:         return mAboutMUI(cl,obj,(APTR)msg);
        case MUIM_Application_OpenConfigWindow: return mOpenMUIConfigWindow(cl,obj,(APTR)msg);
        case MUIM_Application_ShowHelp:         return mShowHelp(cl,obj,(APTR)msg);

        case MUIM_RSS_HTTPStatus:               return mHTTPStatus(cl,obj,(APTR)msg);
        case MUIM_RSS_GotoURL:                  return mGoURL(cl,obj,(APTR)msg);

        case MUIM_App_DisposeWin:               return mAppDisposeWin(cl,obj,(APTR)msg);
        case MUIM_App_PrefsWin:                 return mPrefsWin(cl,obj,(APTR)msg);
        case MUIM_App_CMWin:                    return mCMWin(cl,obj,(APTR)msg);
        case MUIM_App_MiniMailWin:              return mMiniMailWin(cl,obj,(APTR)msg);
        case MUIM_App_LoadPrefs:                return mLoadPrefs(cl,obj,(APTR)msg);
        case MUIM_App_SavePrefs:                return mSavePrefs(cl,obj,(APTR)msg);
        case MUIM_App_HandleMsg:                return mHandleMsg(cl,obj,(APTR)msg);
        case MUIM_App_LoadFeeds:                return mLoadFeeds(cl,obj,(APTR)msg);
        case MUIM_App_QuickQuit:                return mQuickQuit(cl,obj,(APTR)msg);

        default:                                return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(dispatcher)

/***********************************************************************/

ULONG
initAppClass(void)
{
    if (appClass = MUI_CreateCustomClass(NULL,MUIC_Application,NULL,sizeof(struct data),DISP(dispatcher)))
    {
        if (MUIMasterBase->lib_Version<20)
        {
    	    struct NewMenu *m = appMenu;

            while (m->nm_Type!=NM_END && m->nm_Label!=(STRPTR)MSG_Menu_ScreenMode) m++;
            if (m->nm_Type!=NM_END)
            {
	            m++;

                while (m->nm_Type!=NM_END)
                {
                    copymem(m-1,m,sizeof(*m));
                    m++;
                }

                (m-1)->nm_Type = NM_END;
    	    }
        }

        localizeMenus(appMenu);

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/

void
disposeAppClass(void)
{
    MUI_DeleteCustomClass(appClass);
}

/***********************************************************************/
