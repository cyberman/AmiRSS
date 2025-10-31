
#include "rss.h"
#include <intuition/pointerclass.h>
#include <dos/dostags.h>
#include "http.h"
#define CATCOMP_NUMBERS
#include "loc.h"

/***********************************************************************/

static struct MUI_CustomClass *fixSpaceClass = NULL;
#define fsObject NewObject(fixSpaceClass->mcc_Class,NULL

static ULONG
mfsAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
    struct RastPort   rp;
    struct TextExtent info;
    UWORD 	      h;

    DoSuperMethodA(cl,obj,(Msg)msg);

    h = msg->MinMaxInfo->MinHeight;

    copymem(&rp,&_screen(obj)->RastPort,sizeof(rp));
    TextExtent(&rp,"X",1,&info);
    h = info.te_Height/6;
    if (h<2) h = 2;

    msg->MinMaxInfo->MinHeight = h;
    msg->MinMaxInfo->DefHeight = h;
    msg->MinMaxInfo->MaxHeight = h;

    return 0;
}

M_DISP(fsdispatcher)
{
    M_DISPSTART

    switch (msg->MethodID)
    {
        case MUIM_AskMinMax: return mfsAskMinMax(cl,obj,(APTR)msg);

        default:             return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(fsdispatcher)

/***********************************************************************/

static ULONG
initFSClass(void)
{
    return (ULONG)(fixSpaceClass = MUI_CreateCustomClass(NULL,MUIC_Rectangle,NULL,0,DISP(fsdispatcher)));
}

static void
disposeFSClass(void)
{
    MUI_DeleteCustomClass(fixSpaceClass);
}

/***********************************************************************/

static STRPTR g_title = NULL;
static ULONG  g_titleLen = 0;
static STRPTR g_date = NULL;
static ULONG  g_dateLen = 0;
static STRPTR g_author = NULL;
static ULONG  g_authorLen = 0;

/***********************************************************************/

struct data
{
    APTR                          pool;

    Object                        *app;
    Object                        *win;

    Object                        *bar;
    Object                        *superMain;
    Object                        *main;
    Object                        *groupslv;
    Object                        *hbal;
    Object                        *superEntries;
    Object                        *pager;
    Object                        *entriesGroup;
    Object                        *gentries;

    Object                        *groups;
    Object                        *date;
    Object                        *entries;
    Object                        *dgHTML;
    Object                        *vbal;
    Object                        *HTML;
    Object                        *sHTML;
    Object                        *info;

    Object                        *gdown;
    Object                        *bytes;
    Object                        *busy;

    ULONG                         len;
    ULONG                         current;

    Object                        *ginfo;
    Object                        *gbinfo;
    Object                        *search;

    Object                        *nbar;
    Object                        *nbarvgroup;
    Object                        *nbarhgroup;
    Object                        *gsHTML;
    Object                        *URL;

    ULONG                         bbar;

    struct bprefs                 bprefs;
    struct bprefs                 nbprefs;
    LONG                          GMTOffset;

    UBYTE                         codesetName[DEF_CODESETSIZE];
    struct codeset                *codeset;
    STRPTR                        proxy;
    int                           proxyPort;
    STRPTR                        agent;
    STRPTR                        editor;

    struct Window                 *window;
    Object                        *hand;

    ULONG			              wleft;
    ULONG			              wright;
    ULONG			              wup;
    ULONG			              wdown;

    ULONG			              bwleft;
    ULONG			              bwright;
    ULONG			              bwrightUp;
    ULONG			              bwrightDown;

    STRPTR			              descrBuf;
    ULONG                         flags;
};

enum
{
    FLG_Setup         = 1<<0,
    FLG_Task          = 1<<1,
    FLG_Delete        = 1<<2,
    FLG_Failure       = 1<<3,
    FLG_FeedAll       = 1<<4,
    FLG_SubstDate     = 1<<5,
    FLG_UseProxy      = 1<<6,
    FLG_Feeding       = 1<<8,
    FLG_HideTitleHTML = 1<<9,
    FLG_ShowFeedImage = 1<<10,
    FLG_SubstWinChars = 1<<11,
    FLG_HTMLContents  = 1<<12,
    FLG_AutoUpdate    = 1<<13,
    FLG_Queue         = 1<<14,
    FLG_Dispose	      = 1<<15,
    FLG_URLInArgs     = 1<<16,
    FLG_UseHandMouse  = 1<<17,
    FLG_HideGroups    = 1<<18,
    FLG_HideBar       = 1<<19,
    FLG_HidePreview   = 1<<20,
    FLG_Compact       = 1<<21,
    FLG_FirstPrefs    = 1<<22,
};

/***********************************************************************/

enum
{
    B_ADD,
    B_EDIT,
    B_ADDGROUP,
    B_SEARCH,
    B_DUMMY1,
    B_FEED,
    B_STOP,
    B_DUMMY2,
    B_REMOVE,
    B_DUMMY3,
    B_NEWS,
    B_HTML,
    B_DUMMY4,
    B_PREFS,
};

static struct MUIS_TheBar_Button buttons[] =
{
    {2,  B_ADDGROUP, (STRPTR)MSG_Gad_AddGroup,	(STRPTR)MSG_Gad_AddGroupHelp},
    {0,  B_ADD,      (STRPTR)MSG_Gad_Add,    	(STRPTR)MSG_Gad_AddHelp},
    {1,  B_EDIT,     (STRPTR)MSG_Gad_Edit,   	(STRPTR)MSG_Gad_EditHelp,     	MUIV_TheBar_ButtonFlag_Disabled},
    {MUIV_TheBar_ButtonSpacer,-1},
    {3,  B_FEED,     (STRPTR)MSG_Gad_Feed,   	(STRPTR)MSG_Gad_FeedHelp,     	MUIV_TheBar_ButtonFlag_Disabled},
    {4,  B_STOP,     (STRPTR)MSG_Gad_Stop,   	(STRPTR)MSG_Gad_StopHelp,     	MUIV_TheBar_ButtonFlag_Disabled},
    {5,  B_SEARCH,   (STRPTR)MSG_Gad_Search, 	(STRPTR)MSG_Gad_SearchHelp,   	MUIV_TheBar_ButtonFlag_Disabled},
    {MUIV_TheBar_ButtonSpacer,-1},
    {6,  B_REMOVE,   (STRPTR)MSG_Gad_Remove, 	(STRPTR)MSG_Gad_RemoveHelp,   	MUIV_TheBar_ButtonFlag_Disabled},
    {MUIV_TheBar_ButtonSpacer,-1},
    {7,  B_NEWS,     (STRPTR)MSG_Gad_Main,   	(STRPTR)MSG_Gad_MainHelp,     	MUIV_TheBar_ButtonFlag_Immediate|MUIV_TheBar_ButtonFlag_Selected, 1<<B_HTML},
    {8,  B_HTML,     (STRPTR)MSG_Gad_HTML,   	(STRPTR)MSG_Gad_HTMLHelp,     	MUIV_TheBar_ButtonFlag_Immediate,                                 1<<B_NEWS},
    {MUIV_TheBar_ButtonSpacer,-1},
    {9,  B_PREFS,    (STRPTR)MSG_Gad_Prefs,  	(STRPTR)MSG_Gad_PrefsHelp},
    {MUIV_TheBar_End}
};

static STRPTR pics[] =
{
    "Add",
    "Edit",
    "AddGroup",
    "Feed",
    "Stop",
    "Search",
    "Remove",
    "News",
    "HTML",
    "Prefs",
    NULL
};

enum
{
    B_BACK,
    B_FORWARD,
    B_RELOAD,
};

static struct MUIS_TheBar_Button nbuttons[] =
{
    {0,  B_BACK,    (STRPTR)MSG_Gad_Back,   (STRPTR)MSG_Gad_BackHelp,     MUIV_TheBar_ButtonFlag_Disabled},
    {1,  B_FORWARD, (STRPTR)MSG_Gad_Next,   (STRPTR)MSG_Gad_NextHelp,     MUIV_TheBar_ButtonFlag_Disabled},
    {2,  B_RELOAD,  (STRPTR)MSG_Gad_Reload, (STRPTR)MSG_Gad_ReloadHelp,   MUIV_TheBar_ButtonFlag_Disabled},
    {MUIV_TheBar_End}
};

static STRPTR npics[] =
{
    "Back",
    "Forward",
    "Reload",
    NULL
};

static UWORD handData[] =
{
    0x0400,0x0a00,0x0a00,0x0a00,0x0a00,0x0b80,0x0ae0,0x4ab8,
    0xa828,0x5808,0x2808,0x2808,0x1008,0x1008,0x0810,0x0ff0,
    
    0x0400,0x0e00,0x0e00,0x0e00,0x0e00,0x0f80,0x0fe0,0x4ff8,
    0xeff8,0x7ff8,0x3ff8,0x3ff8,0x1ff8,0x1ff8,0x0ff0,0x0ff0
};

#ifdef __MORPHOS__
static struct BitMap handBitMap = {2,16,0,2,0,(PLANEPTR)&handData[0],(PLANEPTR)&handData[16]};
#else
static struct BitMap handBitMap = {2,16,0,2,0,(PLANEPTR)&handData[16],(PLANEPTR)&handData[0]};
#endif

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct data temp;
    Object      *bpop, *rsg;
    ULONG  	    URLInArgs, compact;

    memset(&temp,0,sizeof(temp));

    URLInArgs = GetTagData(MUIA_RSS_URLInArgs,FALSE,msg->ops_AttrList);
    compact   = GetTagData(MUIA_RSS_Compact,FALSE,msg->ops_AttrList);

    if ((temp.pool = CreatePool(MEMF_ANY,8192,2048)) &&
        (obj = (Object *)DoSuperNew(cl,obj,
            MUIA_Group_VertSpacing, DEF_HARDSPACING,

            Child, (ULONG)(temp.bar = rootBarObject,
                MUIA_HelpNode,          "mainbar.html",
                MUIA_TheBar_Buttons,    buttons,
                MUIA_TheBar_PicsDrawer, "PROGDIR:etc/img",
                MUIA_TheBar_Pics,       pics,
            End),

            Child, (ULONG)(temp.superMain = HGroup,
                MUIA_Group_Spacing, DEF_HARDSPACING,

                Child, temp.main = HGroup,
                    MUIA_Group_Spacing, DEF_HARDSPACING,

                    Child, temp.groupslv = NListviewObject,
                        MUIA_HelpNode,   "groups.html",
                        MUIA_CycleChain, TRUE,
                        MUIA_HorizWeight, DEF_WLEFT,
                        MUIA_NListview_NList, temp.groups = groupListObject,
                            MUIA_NList_Pool,    temp.pool,
            			    MUIA_RSS_URLInArgs, URLInArgs,
                            MUIA_RSS_Pool, 	    temp.pool,
                        End,
                    End,

                    Child, temp.hbal = BalanceObject, End,

                    Child, temp.superEntries = VGroup,
                        MUIA_HorizWeight, DEF_WRIGHT,

                        Child, temp.pager = VGroup,
                            MUIA_Group_PageMode, TRUE,

                            Child, temp.entriesGroup = VGroup,
                                MUIA_Group_Spacing, DEF_HARDSPACING,

                                Child, temp.gentries = ListviewObject,
                                    MUIA_HelpNode,   "news.html",
                                    MUIA_VertWeight, DEF_WRIGHTUP,
                                    MUIA_CycleChain, TRUE,
                                    MUIA_Listview_List, temp.entries = entriesListObject,
                                    	MUIA_List_Pool, temp.pool,
                                    End,
                                End,

                                Child, temp.vbal = BalanceObject, End,

                                Child, temp.dgHTML = rootScrollgroupObject,
                                    MUIA_VertWeight, DEF_WRIGHTDOWN,
                                    MUIA_Scrollgroup_Contents, temp.HTML = HTMLObject,
                                        MUIA_Group_Spacing,  DEF_HARDSPACING,
                                        MUIA_Frame,       MUIV_Frame_Virtual,
                                        MUIA_HelpNode,    "descr.html",
                                        MUIA_CycleChain,  TRUE,
                                        MUIA_HTML_Simple, TRUE,
                                    End,
                                End,
                            End,

                            Child, temp.nbarvgroup = VGroup,
                                MUIA_Group_Spacing, DEF_HARDSPACING,

                                Child, temp.nbarhgroup = HGroup,
                                    MUIA_Group_Spacing, DEF_HARDSPACING,

                                    Child, temp.gsHTML = VGroup,
                                        MUIA_Group_PageMode, TRUE,
                                        Child, VGroup,
                                            MUIA_HelpNode, "HTML.html",
                                            Child, HGroup,
                                                GroupFrame,
                                                //MUIA_Background, MUII_GroupBack,
                                                MUIA_Group_HorizSpacing, DEF_HARDSPACING,
                                                Child, HGroup,
                                                    Child, olabel2(MSG_Gad_URL),
                                                    Child, temp.URL = oaestring(DEF_URLSIZE,MSG_Gad_URL,MSG_Gad_URLHelp),
                                                End,
                                                Child, bpop = MUI_MakeObject(MUIO_PopButton,MUII_PopUp),
                                            End,

                                            Child, rsg = rootScrollgroupObject,
                                                MUIA_Scrollgroup_UseWinBorder, !compact,
                                                MUIA_Scrollgroup_Contents, temp.sHTML = HTMLObject,
                                                    MUIA_Frame,      MUIV_Frame_Virtual,
                                                    MUIA_CycleChain, TRUE,
                                                End,
                                            End,
                                        End,
                                        Child, HVSpace,
                                    End,
                                End,

                                Child, temp.nbar = rootBarObject,
                                    MUIA_TheBar_Buttons,    nbuttons,
                                    MUIA_TheBar_PicsDrawer, "PROGDIR:etc/img",
                                    MUIA_TheBar_Pics,       npics,
                                    MUIA_RootBar_NBar,      TRUE,
                                    MUIA_HelpNode,          "navbar.html",
                                End,
                            End,
                        End,
                    End,
                End,
            End),

            Child, temp.ginfo = HGroup,
                MUIA_ShowMe,        !compact,
                MUIA_Group_Spacing, 0,
                MUIA_Background, MUII_TextBack,
                TextFrame,

                Child, VGroup,
                    MUIA_Group_Spacing, DEF_HARDSPACING,

                    Child, temp.date = TextObject,
                        MUIA_Text_SetMin,   FALSE,
                    End,
    
                    Child, fsObject, End,
    
                    Child, temp.info = TextObject,
    	                MUIA_Text_SetMin,   FALSE,
                    End,
            	End,

            End,

            TAG_MORE, (ULONG)msg->ops_AttrList)))
    {
        struct data    *data = INST_DATA(cl,obj);
        struct Library *CManagerBase;

        copymem(data,&temp,sizeof(*data));

        setDefaultBPrefs(&data->bprefs);
        setDefaultNBPrefs(&data->nbprefs);

    	data->bwleft      = DEF_WLEFT;
    	data->bwright	  = DEF_WRIGHT;
    	data->bwrightUp	  = DEF_WRIGHTUP;
    	data->bwrightDown = DEF_WRIGHTDOWN;

        data->gdown = VGroup,
	    MUIA_Group_Spacing, DEF_HARDSPACING,
            MUIA_Weight, 20,
            Child, data->bytes = GaugeObject, GaugeFrame, MUIA_Gauge_Horiz, TRUE, End,
            Child, data->busy = BusyObject, MUIA_Weight, 80, End,
        End;

        data->hand = NewObject(NULL,"pointerclass",
                        POINTERA_BitMap,      &handBitMap,
                        POINTERA_WordWidth,   handBitMap.BytesPerRow,
                        POINTERA_XOffset,     -5,
                        POINTERA_YOffset,     -1,
                        POINTERA_XResolution, POINTERXRESN_HIRES,
                        POINTERA_YResolution, POINTERYRESN_HIGH,
                        TAG_DONE);

        data->flags = FLG_FirstPrefs;
    	if (URLInArgs) data->flags |= FLG_URLInArgs;
        if (compact) data->flags |= FLG_Compact;
    	
        DoSuperMethod(cl,obj,MUIM_MultiSet,MUIA_RSS_Groups,
            (ULONG)obj,
            (ULONG)data->groups,
            (ULONG)data->sHTML,
            (ULONG)data->HTML,
            NULL);

        set(data->groups,MUIA_RSS_Entries,data->entries);

        DoMethod(_sbbut(data->bar,B_ADD),MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)data->groups,2,MUIM_GroupList_EditFeed,MUIV_GroupList_EditFeed_New|MUIV_GroupList_EditFeed_Sub);
        DoMethod(_sbbut(data->bar,B_ADDGROUP),MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)data->groups,2,MUIM_GroupList_EditFeed,MUIV_GroupList_EditFeed_New|MUIV_GroupList_EditFeed_Group);
        DoMethod(_sbbut(data->bar,B_EDIT),MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)data->groups,2,MUIM_GroupList_EditFeed,0);
        DoMethod(_sbbut(data->bar,B_REMOVE),MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)data->groups,1,MUIM_GroupList_Remove);
        DoMethod(_sbbut(data->bar,B_FEED),MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)data->groups,3,MUIM_GroupList_Feed,(ULONG)MUIV_GroupList_Feed_Remote,(ULONG)MUIV_GroupList_Feed_Active);
        DoMethod(_sbbut(data->bar,B_STOP),MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)obj,1,MUIM_MainGroup_Stop);
        DoMethod(_sbbut(data->bar,B_SEARCH),MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)obj,1,MUIM_MainGroup_Search);
        DoMethod(_sbbut(data->bar,B_PREFS),MUIM_Notify,MUIA_Pressed,FALSE,MUIV_Notify_Application,2,MUIM_App_PrefsWin,MUIV_Prefs_Page_None);
        DoMethod(data->bar,MUIM_Notify,MUIA_TheBar_Active,B_NEWS,(ULONG)obj,2,MUIM_MainGroup_ActivePage,MUIV_MainGroup_ActivePage_News);
        DoMethod(data->bar,MUIM_Notify,MUIA_TheBar_Active,B_HTML,(ULONG)obj,2,MUIM_MainGroup_ActivePage,MUIV_MainGroup_ActivePage_HTML);

        DoMethod(data->entries,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime,(ULONG)obj,1,MUIM_MainGroup_EntryChange);

        DoMethod(data->URL,MUIM_Notify,MUIA_Textinput_Acknowledge,MUIV_EveryTime,(ULONG)obj,3,MUIM_MainGroup_UserURL,MUIV_TriggerValue,FALSE);
        if (CManagerBase = OpenLibrary("CManager.library",32))
        {
            DoMethod(bpop,MUIM_Notify,MUIA_Pressed,FALSE,MUIV_Notify_Application,1,MUIM_App_CMWin);
            CloseLibrary(CManagerBase);
        }
        else set(bpop,MUIA_Disabled,TRUE);

        DoMethod(_sbbut(data->nbar,B_BACK),MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)data->sHTML,1,MUIM_HTML_GoBack);
        DoMethod(_sbbut(data->nbar,B_FORWARD),MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)data->sHTML,1,MUIM_HTML_GoSucc);
        DoMethod(_sbbut(data->nbar,B_RELOAD),MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)data->sHTML,1,MUIM_HTMLview_Reload);

        DoMethod(data->HTML,MUIM_Notify,MUIA_HTMLview_CurrentURL,MUIV_EveryTime,(ULONG)obj,3,MUIM_MainGroup_CurrentURL,MUIV_TriggerValue,TRUE);
        DoMethod(data->sHTML,MUIM_Notify,MUIA_HTMLview_CurrentURL,MUIV_EveryTime,(ULONG)obj,3,MUIM_MainGroup_CurrentURL,MUIV_TriggerValue,TRUE);

        DoMethod(data->HTML,MUIM_Notify,MUIA_HTMLview_Prop_VDeltaFactor,MUIV_EveryTime,(ULONG)rsg,3,MUIM_Set,MUIA_RootScrollGroup_VDeltaFactor,MUIV_TriggerValue);
        DoMethod(data->HTML,MUIM_Notify,MUIA_HTMLview_Prop_HDeltaFactor,MUIV_EveryTime,(ULONG)rsg,3,MUIM_Set,MUIA_RootScrollGroup_HDeltaFactor,MUIV_TriggerValue);

        //DoMethod(data->HTML,MUIM_Notify,MUIA_HTMLview_ImagesInDecodeQueue,MUIV_EveryTime,obj,4,MUIM_RSS_HTTPStatus,HSMTYPE_Busy,MUIV_TriggerValue,HSMFLG_Queue);
        //DoMethod(data->sHTML,MUIM_Notify,MUIA_HTMLview_ImagesInDecodeQueue,MUIV_EveryTime,obj,4,MUIM_RSS_HTTPStatus,HSMTYPE_Busy,MUIV_TriggerValue,HSMFLG_Queue);
    }
    else if (temp.pool) DeletePool(temp.pool);

    return (ULONG)obj;
}

/***********************************************************************/

static ULONG
mDispose(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);
    APTR        pool = data->pool;
    ULONG       res;

    data->flags |= FLG_Dispose;

    if (data->hand) DisposeObject(data->hand);

    /* data->bbar should always be 0 ! */
    if (!data->bbar && data->gdown)
        MUI_DisposeObject(data->gdown);

    if (data->flags & FLG_HideGroups)
    {
    	MUI_DisposeObject(data->groupslv);
        MUI_DisposeObject(data->hbal);
    }

    if (data->flags & FLG_HideBar)
    	MUI_DisposeObject(data->bar);

    if (data->flags & FLG_HidePreview)
    {
        MUI_DisposeObject(data->vbal);
        MUI_DisposeObject(data->dgHTML);
    }

    if (data->descrBuf) freeVecPooled(data->pool,data->descrBuf);

    res = DoSuperMethodA(cl,obj,msg);

    DeletePool(pool);

    return res;
}

/***********************************************************************/

static ULONG
mGet(struct IClass *cl,Object *obj,struct opGet *msg)
{
    struct data *data = INST_DATA(cl, obj);

    switch (msg->opg_AttrID)
    {
        case MUIA_RSS_Groups: *msg->opg_Storage = (ULONG)data->groups; return TRUE;
        default: return DoSuperMethodA(cl,obj,(Msg)msg);
    }
}

/***************************************************************************/

static void
setBarPos(struct IClass *cl,Object *obj,struct data *data,ULONG bPos)
{
    ULONG obPos = data->bprefs.bPos;

    if (!(data->flags & FLG_HideBar))
        if (obPos & BPOS_TopMode)
        {
            if (!(bPos & BPOS_TopMode)) DoSuperMethod(cl,obj,OM_REMMEMBER,(ULONG)data->bar);
        }
        else
            if (obPos & BPOS_LeftMode)
            {
                if (!(bPos & BPOS_LeftMode)) DoMethod(data->superMain,OM_REMMEMBER,(ULONG)data->bar);
            }
            else
                if (obPos & BPOS_InnerMode)
                {
                    if (!(bPos & BPOS_InnerMode)) DoMethod(data->superEntries,OM_REMMEMBER,(ULONG)data->bar);
                }

    switch (bPos)
    {
        case BPOS_Top:
            if (!(obPos & BPOS_TopMode)) DoSuperMethod(cl,obj,OM_ADDMEMBER,(ULONG)data->bar);
            DoSuperMethod(cl,obj,MUIM_Group_Sort,(ULONG)data->bar,(ULONG)data->superMain,(ULONG)data->ginfo,NULL);
            break;

        case BPOS_Bottom:
            if (!(obPos & BPOS_TopMode)) DoSuperMethod(cl,obj,OM_ADDMEMBER,(ULONG)data->bar);
            DoSuperMethod(cl,obj,MUIM_Group_Sort,(ULONG)data->superMain,(ULONG)data->ginfo,(ULONG)data->bar,NULL);
            break;

        case BPOS_Left:
            if (!(obPos & BPOS_LeftMode)) DoMethod(data->superMain,OM_ADDMEMBER,(ULONG)data->bar);
            DoMethod(data->superMain,MUIM_Group_Sort,(ULONG)data->bar,(ULONG)data->main,NULL);
            break;

        case BPOS_Right:
            if (!(obPos & BPOS_LeftMode)) DoMethod(data->superMain,OM_ADDMEMBER,(ULONG)data->bar);
            DoMethod(data->superMain,MUIM_Group_Sort,(ULONG)data->main,(ULONG)data->bar,NULL);
            break;

        case BPOS_InnerTop:
            if (!(obPos & BPOS_InnerMode)) DoMethod(data->superEntries,OM_ADDMEMBER,(ULONG)data->bar);
            DoMethod(data->superEntries,MUIM_Group_Sort,(ULONG)data->bar,(ULONG)data->pager,NULL);
            break;

        case BPOS_InnerBottom:
            if (!(obPos & BPOS_InnerMode)) DoMethod(data->superEntries,OM_ADDMEMBER,(ULONG)data->bar);
            DoMethod(data->superEntries,MUIM_Group_Sort,(ULONG)data->pager,(ULONG)data->bar,NULL);
            break;
    }

    if (!BOOLSAME(bPos & BPOS_Horiz,obPos & BPOS_Horiz))
        set(data->bar,MUIA_Group_Horiz,(bPos & BPOS_Horiz) ? TRUE : FALSE);
}

/***********************************************************************/

static void
applyBarPrefs(struct IClass *cl,Object *obj,struct data *data,struct bprefs *prefs)
{
    ULONG obPos = data->bprefs.bPos, bPos = prefs->bPos;

    if (data->flags & FLG_Setup)
    {
        set(data->bar,MUIA_TheBar_Limbo,TRUE);

        if ((bPos & BPOS_TopMode)   || (obPos & BPOS_TopMode))   DoSuperMethod(cl,obj,MUIM_Group_InitChange);
        if ((bPos & BPOS_LeftMode)  || (obPos & BPOS_LeftMode))  DoMethod(data->superMain,MUIM_Group_InitChange);
        if ((bPos & BPOS_InnerMode) || (obPos & BPOS_InnerMode)) DoMethod(data->superEntries,MUIM_Group_InitChange);
    }

    SetAttrs(data->bar,
        MUIA_Group_Forward,     FALSE,
        MUIA_TheBar_ViewMode,   prefs->viewMode,
        MUIA_TheBar_BarPos,     prefs->layout,
        MUIA_TheBar_LabelPos,   prefs->lPos,
        MUIA_TheBar_Borderless, prefs->bFlags & BFLG_Borderless,
        MUIA_TheBar_Sunny,      prefs->bFlags & BFLG_Sunny,
        MUIA_TheBar_Raised,     prefs->bFlags & BFLG_Raised,
        MUIA_TheBar_Scaled,     prefs->bFlags & BFLG_Scaled,
        MUIA_TheBar_EnableKeys, prefs->bFlags & BFLG_EnableKeys,
        MUIA_TheBar_BarSpacer,  prefs->bFlags & BFLG_BarSpacer,
        MUIA_TheBar_Frame,      prefs->bFlags & BFLG_Frame,
        MUIA_TheBar_DragBar,    prefs->bFlags & BFLG_DragBar,
        TAG_DONE);

    if (obPos!=bPos) setBarPos(cl,obj,data,bPos);

    if (data->flags & FLG_Setup)
    {
        if ((bPos & BPOS_InnerMode) || (obPos & BPOS_InnerMode)) DoMethod(data->superEntries,MUIM_Group_ExitChange);
        if ((bPos & BPOS_LeftMode)  || (obPos & BPOS_LeftMode))  DoMethod(data->superMain,MUIM_Group_ExitChange);
        if ((bPos & BPOS_TopMode)   || (obPos & BPOS_TopMode))   DoSuperMethod(cl,obj,MUIM_Group_ExitChange);

        set(data->bar,MUIA_TheBar_Limbo,FALSE);
    }

    copymem(&data->bprefs,prefs,sizeof(data->bprefs));
}

/***********************************************************************/

static void
setNBarPos(struct IClass *cl,Object *obj,struct data *data,ULONG bPos)
{
    ULONG obPos = data->nbprefs.bPos;

    if (obPos & BPOS_TopMode)
    {
        if (!(bPos & BPOS_TopMode)) DoMethod(data->nbarvgroup,OM_REMMEMBER,(ULONG)data->nbar);
    }
    else
        if (obPos & BPOS_LeftMode)
        {
            if (!(bPos & BPOS_LeftMode)) DoMethod(data->nbarhgroup,OM_REMMEMBER,(ULONG)data->nbar);
        }

    switch (bPos)
    {
        case BPOS_Top:
            if (!(obPos & BPOS_TopMode)) DoMethod(data->nbarvgroup,OM_ADDMEMBER,(ULONG)data->nbar);
            DoMethod(data->nbarvgroup,MUIM_Group_Sort,(ULONG)data->nbar,(ULONG)data->nbarhgroup,NULL);
            break;

        case BPOS_Bottom:
            if (!(obPos & BPOS_TopMode)) DoMethod(data->nbarvgroup,OM_ADDMEMBER,(ULONG)data->nbar);
            DoMethod(data->nbarvgroup,MUIM_Group_Sort,(ULONG)data->nbarhgroup,(ULONG)data->nbar,NULL);
            break;

        case BPOS_Left:
            if (!(obPos & BPOS_LeftMode)) DoMethod(data->nbarhgroup,OM_ADDMEMBER,(ULONG)data->nbar);
            DoMethod(data->nbarhgroup,MUIM_Group_Sort,(ULONG)data->nbar,(ULONG)data->gsHTML,NULL);
            break;

        case BPOS_Right:
            if (!(obPos & BPOS_LeftMode)) DoMethod(data->nbarhgroup,OM_ADDMEMBER,(ULONG)data->nbar);
            DoMethod(data->nbarhgroup,MUIM_Group_Sort,(ULONG)data->gsHTML,(ULONG)data->nbar,NULL);
            break;
    }

    if (!BOOLSAME(bPos & BPOS_Horiz,obPos & BPOS_Horiz))
        set(data->nbar,MUIA_Group_Horiz,(bPos & BPOS_Horiz) ? TRUE : FALSE);
}

/***********************************************************************/

static void
applyNBarPrefs(struct IClass *cl,Object *obj,struct data *data,struct bprefs *prefs)
{
    ULONG obPos = data->nbprefs.bPos, bPos = prefs->bPos;

    if (data->flags & FLG_Setup)
    {
        set(data->nbar,MUIA_TheBar_Limbo,TRUE);

        if ((bPos & BPOS_TopMode)   || (obPos & BPOS_TopMode))   DoMethod(data->nbarvgroup,MUIM_Group_InitChange);
        if ((bPos & BPOS_LeftMode)  || (obPos & BPOS_LeftMode))  DoMethod(data->nbarhgroup,MUIM_Group_InitChange);
    }

    SetAttrs(data->nbar,
        MUIA_Group_Forward,     FALSE,
        MUIA_TheBar_ViewMode,   prefs->viewMode,
        MUIA_TheBar_BarPos,     prefs->layout,
        MUIA_TheBar_LabelPos,   prefs->lPos,
        MUIA_TheBar_Borderless, prefs->bFlags & BFLG_Borderless,
        MUIA_TheBar_Sunny,      prefs->bFlags & BFLG_Sunny,
        MUIA_TheBar_Raised,     prefs->bFlags & BFLG_Raised,
        MUIA_TheBar_Scaled,     prefs->bFlags & BFLG_Scaled,
        MUIA_TheBar_EnableKeys, prefs->bFlags & BFLG_EnableKeys,
        MUIA_TheBar_BarSpacer,  prefs->bFlags & BFLG_BarSpacer,
        MUIA_TheBar_Frame,      prefs->bFlags & BFLG_Frame,
        MUIA_TheBar_DragBar,    prefs->bFlags & BFLG_DragBar,
        TAG_DONE);

    if (obPos!=bPos) setNBarPos(cl,obj,data,bPos);

    if (data->flags & FLG_Setup)
    {
        if ((bPos & BPOS_LeftMode)  || (obPos & BPOS_LeftMode))  DoMethod(data->nbarhgroup,MUIM_Group_ExitChange);
        if ((bPos & BPOS_TopMode)   || (obPos & BPOS_TopMode))   DoMethod(data->nbarvgroup,MUIM_Group_ExitChange);

        set(data->nbar,MUIA_TheBar_Limbo,FALSE);
    }

    copymem(&data->nbprefs,prefs,sizeof(data->nbprefs));
}

/***********************************************************************/

static void
hideBar(struct IClass *cl,Object *obj,struct data *data,ULONG hide)
{
    ULONG bPos = data->bprefs.bPos;

    if (hide)
    {
        if (data->flags & FLG_Setup)
        {
            if (bPos & BPOS_TopMode)   DoSuperMethod(cl,obj,MUIM_Group_InitChange);
            if (bPos & BPOS_LeftMode)  DoMethod(data->superMain,MUIM_Group_InitChange);
            if (bPos & BPOS_InnerMode) DoMethod(data->superEntries,MUIM_Group_InitChange);
        }

        DoMethod(data->main,OM_REMMEMBER,(ULONG)data->bar);

        if (data->flags & FLG_Setup)
        {
            if (bPos & BPOS_InnerMode) DoMethod(data->superEntries,MUIM_Group_ExitChange);
            if (bPos & BPOS_LeftMode)  DoMethod(data->superMain,MUIM_Group_ExitChange);
            if (bPos & BPOS_TopMode)   DoSuperMethod(cl,obj,MUIM_Group_ExitChange);
        }

    	data->flags |= FLG_HideBar;
    }
    else
    {
        data->bprefs.bPos = 0;

        if (data->flags & FLG_Setup)
        {
            if (bPos & BPOS_TopMode)   DoSuperMethod(cl,obj,MUIM_Group_InitChange);
            if (bPos & BPOS_LeftMode)  DoMethod(data->superMain,MUIM_Group_InitChange);
            if (bPos & BPOS_InnerMode) DoMethod(data->superEntries,MUIM_Group_InitChange);
        }

        setBarPos(cl,obj,data,bPos);

        if (data->flags & FLG_Setup)
        {
            if (bPos & BPOS_InnerMode) DoMethod(data->superEntries,MUIM_Group_ExitChange);
            if (bPos & BPOS_LeftMode)  DoMethod(data->superMain,MUIM_Group_ExitChange);
            if (bPos & BPOS_TopMode)   DoSuperMethod(cl,obj,MUIM_Group_ExitChange);
        }

        data->bprefs.bPos = bPos;
   	    data->flags &= ~FLG_HideBar;
    }
}

static void
hideGroups(struct IClass *cl,Object *obj,struct data *data,ULONG hide)
{
    if (hide)
    {
        get(data->groupslv,MUIA_HorizWeight,&data->wleft);
        get(data->superEntries,MUIA_HorizWeight,&data->wright);

        if (data->flags & FLG_Setup) DoMethod(data->main,MUIM_Group_InitChange);
        DoMethod(data->main,OM_REMMEMBER,(ULONG)data->groupslv);
        DoMethod(data->main,OM_REMMEMBER,(ULONG)data->hbal);
        if (data->flags & FLG_Setup) DoMethod(data->main,MUIM_Group_ExitChange);

    	data->flags |= FLG_HideGroups;
    }
    else
    {
        if (data->flags & FLG_Setup) DoMethod(data->main,MUIM_Group_InitChange);

        DoMethod(data->main,OM_ADDMEMBER,(ULONG)data->groupslv);
        DoMethod(data->main,OM_ADDMEMBER,(ULONG)data->hbal);
        DoMethod(data->main,MUIM_Group_Sort,(ULONG)data->groupslv,(ULONG)data->hbal,(ULONG)data->superEntries,NULL);

        set(data->groupslv,MUIA_HorizWeight,data->wleft);
        set(data->superEntries,MUIA_HorizWeight,data->wright);

        if (data->flags & FLG_Setup) DoMethod(data->main,MUIM_Group_ExitChange);

    	data->flags &= ~FLG_HideGroups;
    }
}

static void
hidePreview(struct IClass *cl,Object *obj,struct data *data,ULONG hide)
{
    if (hide)
    {
        get(data->gentries,MUIA_HorizWeight,&data->wup);
        get(data->dgHTML,MUIA_HorizWeight,&data->wdown);

        DoMethod(data->entriesGroup,MUIM_Group_InitChange);
        DoMethod(data->entriesGroup,OM_REMMEMBER,(ULONG)data->vbal);
        DoMethod(data->entriesGroup,OM_REMMEMBER,(ULONG)data->dgHTML);
        DoMethod(data->entriesGroup,MUIM_Group_ExitChange);

    	data->flags |= FLG_HidePreview;
    }
    else
    {
        DoMethod(data->entriesGroup,MUIM_Group_InitChange);
        DoMethod(data->entriesGroup,OM_ADDMEMBER,(ULONG)data->vbal);
        DoMethod(data->entriesGroup,OM_ADDMEMBER,(ULONG)data->dgHTML);

        set(data->gentries,MUIA_HorizWeight,data->wup);
        set(data->dgHTML,MUIA_HorizWeight,data->wdown);

        DoMethod(data->entriesGroup,MUIM_Group_ExitChange);

    	data->flags &= ~FLG_HidePreview;
        DoMethod(obj,MUIM_MainGroup_EntryChange);
    }
}

static ULONG
mSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct data    *data = INST_DATA(cl,obj);
    struct TagItem *tag;
    struct TagItem *tstate;

    for (tstate = msg->ops_AttrList; tag = NextTagItem(&tstate); )
    {
        ULONG tidata = tag->ti_Data;

        switch(tag->ti_Tag)
        {
            case MUIA_RSS_Prefs:
            {
                struct prefs *prefs = (struct prefs *)tidata;
                ULONG        feed = FALSE, descr = FALSE;

                if (memcmp(&data->bprefs,&prefs->bprefs,sizeof(data->bprefs)))
                    applyBarPrefs(cl,obj,data,&prefs->bprefs);

                if (memcmp(&data->nbprefs,&prefs->nbprefs,sizeof(data->nbprefs)))
                    applyNBarPrefs(cl,obj,data,&prefs->nbprefs);

                if (!GetTagData(MUIA_RSS_OneWay,FALSE,msg->ops_AttrList))
                {
                    set(data->groupslv,MUIA_HorizWeight,prefs->wprefs.left);
                    set(data->superEntries,MUIA_HorizWeight,prefs->wprefs.right);
                    set(data->gentries,MUIA_VertWeight,prefs->wprefs.rightUp);
                    set(data->dgHTML,MUIA_VertWeight,prefs->wprefs.rightDown);

        		    data->bwleft      = prefs->wprefs.left;
        		    data->bwright     = prefs->wprefs.right;
        		    data->bwrightUp   = prefs->wprefs.rightUp;
        		    data->bwrightDown = prefs->wprefs.rightDown;
                }

                if (stricmp(prefs->codeset,data->codesetName))
                {
                    strcpy(data->codesetName,prefs->codeset);
                    data->codeset = CodesetsFindA(data->codesetName,NULL);
                    set(data->groups,MUIA_RSS_Codeset,data->codeset);
                    feed = TRUE;
                }

                if (data->GMTOffset!=prefs->GMTOffset)
                {
                    data->GMTOffset = prefs->GMTOffset;
                    feed = TRUE;
                }

                data->proxy = prefs->proxy;
                data->proxyPort = prefs->proxyPort;
                data->agent = prefs->agent;
                data->editor = prefs->editor;

                if (prefs->httpFlags & PHFLG_UseProxy) data->flags |= FLG_UseProxy;
                else data->flags &= ~FLG_UseProxy;

                if (!BOOLSAME(data->flags & FLG_SubstDate,prefs->flags & PFLG_SubstDate))
                {
                    if (prefs->flags & PFLG_SubstDate) data->flags |= FLG_SubstDate;
                    else data->flags &= ~FLG_SubstDate;
                }

                if (!BOOLSAME(data->flags & FLG_ShowFeedImage,prefs->flags & PFLG_ShowFeedImage))
                {
                    if (prefs->flags & PFLG_ShowFeedImage) data->flags |= FLG_ShowFeedImage;
                    else data->flags &= ~FLG_ShowFeedImage;

                    descr = TRUE;
                }

                if (!BOOLSAME(data->flags & FLG_SubstWinChars,prefs->flags & PFLG_SubstWinChars))
                {
                    if (prefs->flags & PFLG_SubstWinChars) data->flags |= FLG_SubstWinChars;
                    else data->flags &= ~FLG_SubstWinChars;
                }

                if (!BOOLSAME(data->flags & FLG_UseHandMouse,prefs->flags & PFLG_UseHandMouse))
                {
                    if (prefs->flags & PFLG_UseHandMouse) data->flags |= FLG_UseHandMouse;
                    else data->flags &= ~FLG_UseHandMouse;
                }

                if ((data->flags & FLG_Compact) && (data->flags & FLG_FirstPrefs))
                {
                    hideBar(cl,obj,data,TRUE);
                    hideGroups(cl,obj,data,TRUE);
                    hidePreview(cl,obj,data,TRUE);
                }
                else
                {
                    if (!BOOLSAME(data->flags & FLG_HideBar,prefs->hideBar))
                        hideBar(cl,obj,data,prefs->hideBar);

                    if (!BOOLSAME(data->flags & FLG_HideGroups,prefs->hideGroups))
                        hideGroups(cl,obj,data,prefs->hideGroups);

                    if (!BOOLSAME(data->flags & FLG_HidePreview,prefs->hidePreview))
                        hidePreview(cl,obj,data,prefs->hidePreview);
                }

                if (data->flags & FLG_Setup)
                {
                    if (feed) DoMethod(data->groups,MUIM_GroupList_Feed,(ULONG)MUIV_GroupList_Feed_Local,(ULONG)MUIV_GroupList_Feed_Active);
                    else if (descr) DoMethod(obj,MUIM_MainGroup_EntryChange);
                }

                data->flags &= ~FLG_FirstPrefs;

                break;
            }

            case MUIA_MainGroup_Back:
                DoMethod(data->nbar,MUIM_TheBar_SetAttr,B_BACK,MUIV_TheBar_Attr_Disabled,!tidata);
                tag->ti_Tag = TAG_IGNORE;
                break;

            case MUIA_MainGroup_Succ:
                DoMethod(data->nbar,MUIM_TheBar_SetAttr,B_FORWARD,MUIV_TheBar_Attr_Disabled,!tidata);
                tag->ti_Tag = TAG_IGNORE;
                break;

            case MUIA_MainGroup_Reload:
                DoMethod(data->nbar,MUIM_TheBar_SetAttr,B_RELOAD,MUIV_TheBar_Attr_Disabled,!tidata);
                tag->ti_Tag = TAG_IGNORE;
                break;

            case MUIA_MainGroup_Status:
                switch (tidata)
                {
                    case MUIV_MainGroup_Status_None:
                        DoMethod(data->bar,MUIM_TheBar_SetAttr,B_EDIT,MUIV_TheBar_Attr_Disabled,TRUE);
                        DoMethod(data->bar,MUIM_TheBar_SetAttr,B_FEED,MUIV_TheBar_Attr_Disabled,TRUE);
                        DoMethod(data->bar,MUIM_TheBar_SetAttr,B_SEARCH,MUIV_TheBar_Attr_Disabled,TRUE);
                        DoMethod(data->bar,MUIM_TheBar_SetAttr,B_REMOVE,MUIV_TheBar_Attr_Disabled,TRUE);
                        break;

                    case MUIV_MainGroup_Status_Group:
                        DoMethod(data->bar,MUIM_TheBar_SetAttr,B_EDIT,MUIV_TheBar_Attr_Disabled,FALSE);
                        DoMethod(data->bar,MUIM_TheBar_SetAttr,B_FEED,MUIV_TheBar_Attr_Disabled,TRUE);
                        DoMethod(data->bar,MUIM_TheBar_SetAttr,B_SEARCH,MUIV_TheBar_Attr_Disabled,TRUE);
                        DoMethod(data->bar,MUIM_TheBar_SetAttr,B_REMOVE,MUIV_TheBar_Attr_Disabled,FALSE);
                        break;

                    case MUIV_MainGroup_Status_Feed:
	                if (data->flags & FLG_URLInArgs) data->flags &= ~FLG_URLInArgs;
            			else set(data->bar,MUIA_TheBar_Active,B_NEWS);
                        DoMethod(data->bar,MUIM_TheBar_SetAttr,B_EDIT,MUIV_TheBar_Attr_Disabled,FALSE);
                        DoMethod(data->bar,MUIM_TheBar_SetAttr,B_FEED,MUIV_TheBar_Attr_Disabled,FALSE);
                        DoMethod(data->bar,MUIM_TheBar_SetAttr,B_SEARCH,MUIV_TheBar_Attr_Disabled,FALSE);
                        DoMethod(data->bar,MUIM_TheBar_SetAttr,B_REMOVE,MUIV_TheBar_Attr_Disabled,FALSE);
                        break;

                    case MUIV_MainGroup_Status_Feeding:
                        DoMethod(data->bar,MUIM_TheBar_SetAttr,B_EDIT,MUIV_TheBar_Attr_Disabled,FALSE);
                        DoMethod(data->bar,MUIM_TheBar_SetAttr,B_FEED,MUIV_TheBar_Attr_Disabled,TRUE);
                        DoMethod(data->bar,MUIM_TheBar_SetAttr,B_SEARCH,MUIV_TheBar_Attr_Disabled,TRUE);
                        DoMethod(data->bar,MUIM_TheBar_SetAttr,B_REMOVE,MUIV_TheBar_Attr_Disabled,FALSE);
                        break;
                }
                tag->ti_Tag = TAG_IGNORE;
                break;

            case MUIA_RSS_App:
                data->app = (Object *)tidata;
                break;

            case MUIA_RSS_Win:
                data->win = (Object *)tidata;
                break;
        }
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static ULONG
mSetup(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    data->flags |= FLG_Setup;

    return TRUE;
}

/***********************************************************************/

static ULONG
mHTTPStatus(struct IClass *cl,Object *obj,struct MUIP_RSS_HTTPStatus *msg)
{
    struct data *data = INST_DATA(cl,obj);

    switch (msg->type)
    {
        case HSMTYPE_Busy:
            if (!data->gdown) break;

            if (msg->flags & HSMFLG_Queue)
            {
                if (msg->arg)
                {
                    if (data->flags & FLG_Queue) break;
                    data->flags |= FLG_Queue;
                }
                else data->flags &= ~FLG_Queue;
            }

            if (msg->arg)
            {
                if (data->bbar++>0) break;
            }
            else
            {
                if (data->bbar==0 || --data->bbar>0) break;
            }

            if (data->flags & FLG_Setup) DoMethod(data->ginfo,MUIM_Group_InitChange);

            if (data->bbar>0)
            {
                DoMethod(data->ginfo,OM_ADDMEMBER,(ULONG)data->gdown);
                DoMethod(data->bar,MUIM_TheBar_SetAttr,B_STOP,MUIV_TheBar_Attr_Disabled,FALSE);

                data->len = data->current = 0;
                SetAttrs(data->bytes,MUIA_Gauge_Max,1000000,MUIA_Gauge_Current,0,TAG_DONE);
            }
            else
            {
                DoMethod(data->ginfo,OM_REMMEMBER,(ULONG)data->gdown);
                DoMethod(data->bar,MUIM_TheBar_SetAttr,B_STOP,MUIV_TheBar_Attr_Disabled,TRUE);
            }

            if (data->flags & FLG_Setup) DoMethod(data->ginfo,MUIM_Group_ExitChange);
            break;

        case HSMTYPE_ToDo:
            if ((msg->flags & HSMFLG_NoLen) || (msg->arg==0))
            {
                SetAttrs(data->bytes,MUIA_Gauge_Max,1,MUIA_Gauge_Current,1,TAG_DONE);
            }
            else
            {
                data->len += msg->arg;
                set(data->bytes,MUIA_Gauge_Max,data->len);
            }
            break;

        case HSMTYPE_Done:
            data->current += msg->arg;
            set(data->bytes,MUIA_Gauge_Current,data->current);
            break;
    }

    return 0;
}

/***********************************************************************/

static ULONG
mCleanup(struct IClass *cl,Object *obj,Msg msg)
{
    struct data  *data = INST_DATA(cl,obj);
    struct prefs *prefs;

    get(data->app,MUIA_RSS_Prefs,&prefs);
    
    get(data->groupslv,MUIA_HorizWeight,&prefs->wprefs.left);
    get(data->superEntries,MUIA_HorizWeight,&prefs->wprefs.right);
    get(data->gentries,MUIA_VertWeight,&prefs->wprefs.rightUp);
    get(data->dgHTML,MUIA_VertWeight,&prefs->wprefs.rightDown);

    if (data->bwleft!=prefs->wprefs.left ||
        data->bwright!=prefs->wprefs.right ||
        data->bwrightUp!=prefs->wprefs.rightUp ||
        data->bwrightDown!=prefs->wprefs.rightDown)
    {
    	data->bwleft = prefs->wprefs.left;
        data->bwright = prefs->wprefs.right;
        data->bwrightUp = prefs->wprefs.rightUp;
        data->bwrightDown = prefs->wprefs.rightDown;

        set(data->app,MUIA_App_SavePrefs,TRUE);
    }

    DoMethod(data->HTML,MUIM_HTMLview_AbortAll);
    DoMethod(data->sHTML,MUIM_HTMLview_AbortAll);

    data->flags &= ~FLG_Setup;

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static ULONG
mShow(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (!(DoSuperMethodA(cl,obj,msg))) return FALSE;

    get(data->win,MUIA_Window_Window,&data->window);

    return TRUE;
}

/***************************************************************************/

static ULONG
mEntryChange(struct IClass *cl,Object *obj,Msg msg)
{
    struct data                   *data = INST_DATA(cl,obj);
    ULONG                         done = FALSE;
    struct MUI_NListtree_TreeNode *tn;

    if (data->flags & (FLG_Dispose|FLG_HidePreview)) return 0;

    DoMethod(data->HTML,MUIM_HTMLview_Abort);

    if (tn = (struct MUI_NListtree_TreeNode *)xget(data->groups,MUIA_NListtree_Active))
    {
        struct entry *entry;

        DoMethod(data->entries,MUIM_List_GetEntry,MUIV_List_GetEntry_Active,(ULONG)&entry);
        if (entry)
        {
            struct feed *feed = entry->feed;

            if ((feed->flags & FFLG_ShowLink) && entry->link)
            {
                set(data->HTML,MUIA_HTML_CurrentHost,NULL);

                if (data->flags & FLG_HTMLContents)
                {
                    DoMethod(data->HTML,MUIM_HTML_BlindGoURL,(ULONG)"about:blank",NULL);
                    data->flags &= ~FLG_HTMLContents;
                }
                DoMethod(data->HTML,MUIM_HTML_BlindGoURL,(ULONG)entry->link,NULL);
		        done = TRUE;
            }
            else
            {
                STRPTR b;
                ULONG  doImage, doTitle, doAuthor, doMailtoAuthor, doDate, doDescr;
                int    len;

                len = 512;

                if ((data->flags & FLG_ShowFeedImage) && feed->info.image)
                {
                    len += feed->info.imageLen + feed->info.imageLinkLen;
                    doImage = TRUE;
                }
                else doImage = FALSE;

                if (entry->title)
                {
                    len += entry->titleLen+entry->linkLen+g_titleLen;
                    doTitle = TRUE;
                }
                else doTitle = FALSE;

                if (entry->author && *entry->author)
                {
                    len += entry->authorLen+g_authorLen;
                    if (strchr(entry->author,'@') || strstr(entry->author,"AT") || strstr(entry->author," AT "))
                    {
                        len += 32+entry->authorLen;
                        doMailtoAuthor = TRUE;
                    }
                    else doMailtoAuthor = FALSE;
                    doAuthor = TRUE;
                }
                else doAuthor = doMailtoAuthor = FALSE;

                if (entry->date.ds_Days || entry->date.ds_Minute || entry->date.ds_Tick)
                {
                    len += LEN_DATSTRING*3+g_dateLen;
                    doDate = TRUE;
                }
                else doDate = FALSE;

                if (entry->descr)
                {
                    len += entry->descrLen;
                    doDescr = TRUE;
                }
                else doDescr = FALSE;

		if (b = reallocVecPooledNC(data->pool,data->descrBuf,len))
                {
                    int l;

                    l = msnprintf(b,len,"<html><body>");

                    if (doImage)
                    {
                        if (feed->info.imageLink) l += msnprintf(b+l,len-l,"<table border=\"0\" cellpadding=\"0\" cellspacing=\"8\" width=\"100%%\"><tr><td align=\"left\"><a href=\"%s\"><img border=\"0\" src=\"%s\"",feed->info.imageLink,feed->info.image);
                        else l += msnprintf(b+l,len-l,"<table border=\"0\" cellpadding=\"0\" cellspacing=\"8\" width=\"100%%\"><tr><td align=\"left\"><img border=\"0\" src=\"%s\"",feed->info.image);

                        if (feed->info.iwidth>0 && feed->info.iheight>0)
                            l += msnprintf(b+l,len-l," width=\"%ld\" height=\"%ld\"",feed->info.iwidth,feed->info.iheight);

                        if (feed->info.imageLink) l += msnprintf(b+l,len-l,"></a></td><td align=\"left\">");
                        else l += msnprintf(b+l,len-l,"></td><td align=\"left\">");
                    }
                    else l += msnprintf(b+l,len-l,"<p>");

                    if (doTitle)
                        if (entry->link) l += msnprintf(b+l,len-l,"<b>%s:</b> <a href=\"%s\">%s</a>",g_title,entry->link,entry->title);
                        else l += msnprintf(b+l,len-l,"<b>%s:</b> %s",g_title,entry->title);

                    if (doAuthor)
                    {
                        if (doMailtoAuthor) l += msnprintf(b+l,len-l,"<br><b>%s:</b> <a href=\"mailto:%s\">%s</a>",g_author,entry->author,entry->author);
                        else l += msnprintf(b+l,len-l,"<br><b>%s:</b> %s",g_author,entry->author);
                    }

                    if (doDate)
                    {
                        struct DateTime dt;
                        UBYTE  		bday[16], bdate[16], btime[16];

                        copymem(&dt.dat_Stamp,&entry->date,sizeof(dt.dat_Stamp));
                        dt.dat_Format  = FORMAT_DEF;
                        dt.dat_Flags   = 0;
                        dt.dat_StrDay  = bday;
                        dt.dat_StrDate = bdate;
                        dt.dat_StrTime = btime;

                        DateToStr(&dt);
                        l += msnprintf(b+l,len-l,"<br><b>%s:</b> %s %s",g_date,bdate,btime);
                    }

                    if (doImage)
                        l += msnprintf(b+l,len-l,"</td></tr></table>");

                    if (doDescr)
                    {
                        l += msnprintf(b+l,len-l,"<hr><p>%s</p>",entry->descr);

                        if (entry->link) set(data->HTML,MUIA_HTML_CurrentHost,entry->link);
                    }

                    msnprintf(b+l,len-l,"</body></html>");

                    set(data->HTML,MUIA_HTMLview_Contents,b);
                    data->flags |= FLG_HTMLContents;
                    done = TRUE;
                }
            }
        }
    }

    if (!done)
    {
        set(data->HTML,MUIA_HTML_CurrentHost,NULL);
        set(data->HTML,MUIA_HTMLview_Contents,"<html><body><p>&nbsp;<p></html></body>");
        data->flags |= FLG_HTMLContents;
    }

    return 0;
}

/***********************************************************************/

static ULONG
mStop(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    DoMethod(data->HTML,MUIM_HTMLview_AbortAll);
    DoMethod(data->sHTML,MUIM_HTMLview_AbortAll);

    DoMethod(data->groups,MUIM_GroupList_Stop);

    return 0;
}

/***********************************************************************/

static ULONG
mSearch(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (!data->search)
    {
        if (searchClass || initSearchClass())
        {
            if (data->search = searchObject,
                    MUIA_RSS_Pool,       data->pool,
                    MUIA_Search_ListObj, data->entries,
                    MUIA_Search_HTMLObj, data->HTML,
                End)
            {
                DoMethod(data->app,OM_ADDMEMBER,(ULONG)data->search);
            }
        }
    }

    openWindow(data->app,data->search);

    return 0;
}

/***********************************************************************/

static ULONG
mDragQuery(struct IClass *cl,Object *obj,struct MUIP_DragQuery *msg)
{
    struct data *data = INST_DATA(cl,obj);

    return (ULONG)(((msg->obj==data->bar) || (msg->obj==data->nbar)) ? MUIV_DragQuery_Accept : MUIV_DragQuery_Refuse);
}

/***********************************************************************/

static ULONG
mDragBegin(struct IClass *cl,Object *obj,struct MUIP_DragBegin *msg)
{
    return 0;
}

/***********************************************************************/

#define DD_FACT   6
#define DD_INFACT 4

static ULONG
findPos(struct data *data,Object *obj,int obPos,ULONG mx,ULONG my)
{
    ULONG bPos;
    LONG  l, t, r, b, w, h, il, it, ib, ih;

    l = _mleft(obj);
    t = _mtop(obj);
    r = _mright(obj);
    b = _mbottom(obj);
    w = _mwidth(obj)/DD_FACT;
    h = _mheight(obj)/DD_FACT;

    il = _mleft(data->superEntries);
    it = _mtop(data->superEntries);
    ib = _mbottom(data->superEntries);
    ih = _mheight(data->superEntries)/DD_INFACT;

    bPos = 0;

    switch (obPos)
    {
        case BPOS_Left:
            if (mx<l+w) bPos = BPOS_Left;
            else if (mx>r-w) bPos = BPOS_Right;
                 else if (my<t+h) bPos = BPOS_Top;
                      else if (my>b-h) bPos = BPOS_Bottom;
                           else if (my<it+ih) bPos = BPOS_InnerTop;
                                else if (my>ib-ih) bPos = BPOS_InnerBottom;
            break;

        case BPOS_Right:
            if (mx>r-w) bPos = BPOS_Right;
            else if (mx<l+w) bPos = BPOS_Left;
                 else if (my<t+h) bPos = BPOS_Top;
                      else if (my>b-h) bPos = BPOS_Bottom;
                           else if (my<it+ih) bPos = BPOS_InnerTop;
                                else if (my>ib-ih) bPos = BPOS_InnerBottom;
            break;

        case BPOS_Top:
            if (my<t+h) bPos = BPOS_Top;
            else if (mx<l+w) bPos = BPOS_Left;
                 else if (mx>r-w) bPos = BPOS_Right;
                      else if (my>b-h) bPos = BPOS_Bottom;
                           else if (my<it+ih) bPos = BPOS_InnerTop;
                                else if (my>ib-ih) bPos = BPOS_InnerBottom;
            break;

        case BPOS_Bottom:
            if (my>b-h) bPos = BPOS_Bottom;
            else if (mx<l+w) bPos = BPOS_Left;
                 else if (mx>r-w) bPos = BPOS_Right;
                      else if (my<t+h) bPos = BPOS_Top;
                           else if (my<it+ih) bPos = BPOS_InnerTop;
                                else if (my>ib-ih) bPos = BPOS_InnerBottom;
            break;

        case BPOS_InnerTop:
            if ((my<it+ih) && (mx>il)) bPos = BPOS_InnerTop;
            else if (my<t+h) bPos = BPOS_Top;
                 else if (mx<l+w) bPos = BPOS_Left;
                      else if (mx>r-w) bPos = BPOS_Right;
                           else if (my>b-h) bPos = BPOS_Bottom;
                                else if (my>ib-ih) bPos = BPOS_InnerBottom;
            break;

        case BPOS_InnerBottom:
            if ((my>ib-ih) && (mx>il)) bPos = BPOS_InnerBottom;
            else if (my>b-h) bPos = BPOS_Bottom;
                 else if (mx<l+w) bPos = BPOS_Left;
                      else if (mx>r-w) bPos = BPOS_Right;
                           else if (my<t+h) bPos = BPOS_Top;
                                else if (my<it+ih) bPos = BPOS_InnerTop;
            break;

    }

    return (bPos==obPos) ? 0 : bPos;
}

/***********************************************************************/

static ULONG
findNPos(struct data *data,Object *obj,int obPos,ULONG mx,ULONG my)
{
    ULONG bPos;
    LONG  l, t, r, b, w, h;

    l = _mleft(data->nbarvgroup);
    t = _mtop(data->nbarvgroup);
    r = _mright(data->nbarvgroup);
    b = _mbottom(data->nbarvgroup);
    w = _mwidth(data->nbarvgroup)/12;
    h = _mheight(data->nbarvgroup)/12;

    bPos = 0;

    switch (obPos)
    {
        case BPOS_Left:
            if (mx<l+w) bPos = BPOS_Left;
            else if (mx>r-w) bPos = BPOS_Right;
                 else if (my<t+h) bPos = BPOS_Top;
                      else if (my>b-h) bPos = BPOS_Bottom;
            break;

        case BPOS_Right:
            if (mx>r-w) bPos = BPOS_Right;
            else if (mx<l+w) bPos = BPOS_Left;
                 else if (my<t+h) bPos = BPOS_Top;
                      else if (my>b-h) bPos = BPOS_Bottom;
            break;

        case BPOS_Top:
            if (my<t+h) bPos = BPOS_Top;
            else if (mx<l+w) bPos = BPOS_Left;
                 else if (mx>r-w) bPos = BPOS_Right;
                      else if (my>b-h) bPos = BPOS_Bottom;
            break;

        case BPOS_Bottom:
            if (my>b-h) bPos = BPOS_Bottom;
            else if (mx<l+w) bPos = BPOS_Left;
                 else if (mx>r-w) bPos = BPOS_Right;
                      else if (my<t+h) bPos = BPOS_Top;
            break;
    }

    return (bPos==obPos) ? 0 : bPos;
}

/***********************************************************************/

static ULONG
mDragReport(struct IClass *cl,Object *obj,struct MUIP_DragReport *msg)
{
    struct data *data = INST_DATA(cl,obj);
    ULONG       bPos, obPos;

    if (!msg->update) return MUIV_DragReport_Refresh;

    if (msg->obj==data->bar)
    {
        obPos = data->bprefs.bPos;
        bPos = findPos(data,obj,obPos,msg->x,msg->y);
        if (bPos==0) return MUIV_DragReport_Continue;

        if (data->flags & FLG_HideGroups)
            if (bPos==BPOS_InnerTop) bPos = BPOS_Top;
            else if (bPos==BPOS_InnerBottom) bPos = BPOS_Bottom;

        set(data->bar,MUIA_TheBar_Limbo,TRUE);
        if ((bPos & BPOS_TopMode)   || (obPos & BPOS_TopMode))   DoSuperMethod(cl,obj,MUIM_Group_InitChange);
        if ((bPos & BPOS_LeftMode)  || (obPos & BPOS_LeftMode))  DoMethod(data->superMain,MUIM_Group_InitChange);
        if ((bPos & BPOS_InnerMode) || (obPos & BPOS_InnerMode)) DoMethod(data->superEntries,MUIM_Group_InitChange);

        setBarPos(cl,obj,data,bPos);

        if ((bPos & BPOS_InnerMode) || (obPos & BPOS_InnerMode)) DoMethod(data->superEntries,MUIM_Group_ExitChange);
        if ((bPos & BPOS_LeftMode)  || (obPos & BPOS_LeftMode))  DoMethod(data->superMain,MUIM_Group_ExitChange);
        if ((bPos & BPOS_TopMode)   || (obPos & BPOS_TopMode))   DoSuperMethod(cl,obj,MUIM_Group_ExitChange);
        set(data->bar,MUIA_TheBar_Limbo,FALSE);

        data->bprefs.bPos = bPos;
        set(data->app,MUIA_RSS_BarPos,bPos);
    }
    else
    {
        obPos = data->nbprefs.bPos;
        bPos = findNPos(data,obj,obPos,msg->x,msg->y);
        if (bPos==0) return MUIV_DragReport_Continue;

        set(data->nbar,MUIA_TheBar_Limbo,TRUE);
        if ((bPos & BPOS_TopMode)   || (obPos & BPOS_TopMode))   DoMethod(data->nbarvgroup,MUIM_Group_InitChange);
        if ((bPos & BPOS_LeftMode)  || (obPos & BPOS_LeftMode))  DoMethod(data->nbarhgroup,MUIM_Group_InitChange);

        setNBarPos(cl,obj,data,bPos);

        if ((bPos & BPOS_LeftMode)  || (obPos & BPOS_LeftMode))  DoMethod(data->nbarhgroup,MUIM_Group_ExitChange);
        if ((bPos & BPOS_TopMode)   || (obPos & BPOS_TopMode))   DoMethod(data->nbarvgroup,MUIM_Group_ExitChange);
        set(data->nbar,MUIA_TheBar_Limbo,FALSE);

        data->nbprefs.bPos = bPos;
        set(data->app,MUIA_RSS_NBarPos,bPos);
    }

    return MUIV_DragReport_Continue;
}

/***********************************************************************/

static ULONG
mDragFinish(struct IClass *cl,Object *obj,struct MUIP_DragFinish *msg)
{
    return 0;
}

/***********************************************************************/

static ULONG
mDragDrop(struct IClass *cl,Object *obj,struct MUIP_DragDrop *msg)
{
    return 0;
}

/***********************************************************************/

static ULONG
mGoURL(struct IClass *cl,Object *obj,struct MUIP_RSS_GotoURL *msg)
{
    struct data *data = INST_DATA(cl,obj);

    set(data->URL,MUIA_Textinput_Contents,msg->URL);
    set(data->bar,MUIA_TheBar_Active,B_HTML);
    DoMethodA(data->sHTML,(Msg)msg);

    return 0;
}

/***********************************************************************/

static ULONG
mSetInfo(struct IClass *cl,Object *obj,struct MUIP_MainGroup_SetInfo *msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (msg->string) set(data->info,MUIA_Text_Contents,msg->string);
    else set(data->info,MUIA_Text_Contents,getString(msg->id));

    return 0;
}

/***********************************************************************/

static ULONG
mSetFeedInfo(struct IClass *cl,Object *obj,struct MUIP_MainGroup_SetFeedInfo *msg)
{
    struct data     *data = INST_DATA(cl,obj);
    struct feedInfo *fi = msg->fi;
    UBYTE           buf[128];

    if (fi && (fi->date.ds_Days || fi->date.ds_Minute || fi->date.ds_Tick))
    {
        UBYTE   	 bday[32], bdate[32], btime[32];
        struct DateStamp ds;
        struct DateTime  dt;
        ULONG   	 id, a, b, c, d, e;
        LONG    	 ticks, secs, mins, hours, days;

        *bday  = 0;
        *bdate = 0;
        *btime = 0;

        copymem(&dt.dat_Stamp,&fi->date,sizeof(dt.dat_Stamp));
        dt.dat_Format  = FORMAT_DOS;
        dt.dat_Flags   = 0;
        dt.dat_StrDay  = bday;
        dt.dat_StrDate = bdate;
        dt.dat_StrTime = btime;
        DateToStr(&dt);

        DateStamp(&ds);

        days  = ds.ds_Days   - fi->date.ds_Days;
        mins  = ds.ds_Minute - fi->date.ds_Minute;
        ticks = ds.ds_Tick   - fi->date.ds_Tick;

        if (ticks<0)
        {
            mins--;
            ticks += 3000;
        }

        if (mins<0)
        {
            days--;
            mins += 1440;
        }

        secs  = ticks/50;
        hours = mins/60;
        mins  = mins%60;

        if (days>0)
        {
            if (days>1) id = MSG_Group_Date_DSHMS;
            else id = MSG_Group_Date_DHMS;

            a = days;
            b = hours;
            c = mins;
            d = secs;
            e = fi->news;
        }
        else
        {
            if (hours==0 && mins==0 && secs<8)
            {
                id = MSG_Group_Date_Now;

                a = fi->news;
                b = 0;
                c = 0;
                d = 0;
                e = 0;
            }
            else
            {
                id = MSG_Group_Date_HMS;

                a = hours;
                b = mins;
                c = secs;
                d = fi->news;
                e = 0;
            }
        }

        msnprintf(buf,sizeof(buf),getString(id),bdate,btime,a,b,c,d,e);
    }
    else *buf = 0;

    set(data->date,MUIA_Text_Contents,buf);

    return 0;
}

static ULONG
mActivePage(struct IClass *cl,Object *obj,struct MUIP_MainGroup_ActivePage *msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (msg->page==MUIV_MainGroup_ActivePage_News)
    {
        nnset(data->gsHTML,MUIA_Group_ActivePage,1);
        nnset(data->pager,MUIA_Group_ActivePage,0);
    }
    else
    {
        nnset(data->pager,MUIA_Group_ActivePage,1);
        nnset(data->gsHTML,MUIA_Group_ActivePage,0);
    }

    return 0;
}

/***********************************************************************/

static ULONG
mEditor(struct IClass *cl,Object *obj,struct MUIP_MainGroup_Editor *msg)
{
    struct data *data = INST_DATA(cl,obj);

    return executeCommand(data->pool,data->editor,data->win,msg->file,TRUE);
}

/***********************************************************************/

static ULONG
mCurrentURL(struct IClass *cl,Object *obj,struct MUIP_MainGroup_CurrentURL *msg)
{
    struct data *data = INST_DATA(cl,obj);

    if ((data->flags & FLG_UseHandMouse) && data->hand)
        if (msg->URL) SetWindowPointer(data->window,WA_Pointer,(ULONG)data->hand,TAG_DONE);
        else SetWindowPointer(data->window, WA_BusyPointer,xget(data->app,MUIA_Application_Sleep),TAG_DONE);

    if (msg->showString) DoMethod(obj,MUIM_MainGroup_SetInfo,(ULONG)msg->URL,0);

    return 0;
}

/***********************************************************************/

static ULONG
mUserURL(struct IClass *cl,Object *obj,struct MUIP_MainGroup_UserURL *msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (msg->URL && *msg->URL)
    {
        UBYTE  buf[1024];
        STRPTR URL;
        ULONG  freeURL = FALSE;

        if (!strchr(msg->URL,':'))
        {
            ULONG l = strlen(msg->URL)+8;

            if (l>sizeof(buf))
            {
                if (URL = allocVecPooled(data->pool,l)) freeURL = TRUE;
            }
            else URL = buf;

            if (URL) msprintf(URL,"http://%s",(ULONG)msg->URL);
        }
        else URL = msg->URL;

        if (URL)
        {
            set(data->URL,MUIA_Textinput_Contents,msg->URL);
            if (!msg->oneWay) DoMethod(data->sHTML,MUIM_RSS_GotoURL,(ULONG)URL,MUIV_RSS_GotoURL_IgnorePrefs|MUIV_RSS_GotoURL_NoTarget);
            set(data->bar,MUIA_TheBar_Active,B_HTML);
            if (freeURL) freeVecPooled(data->pool,URL);
        }
    }

    return 0;
}

/***********************************************************************/

static ULONG
mFlushImages(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    /* XXX hits otherwise */
    if (!(data->flags &FLG_HidePreview)) DoMethod(data->HTML,MUIM_HTMLview_FlushImage,MUIV_HTMLview_FlushImage_All);

    DoMethod(data->sHTML,MUIM_HTMLview_FlushImage,MUIV_HTMLview_FlushImage_All);

    return 0;
}

/***********************************************************************/

static ULONG
mFlushCookies(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    DoMethod(data->HTML,MUIM_HTML_FlushCookies);
    DoMethod(data->sHTML,MUIM_HTML_FlushCookies);

    return 0;
}

/***********************************************************************/

static ULONG
mAppMessage(struct IClass *cl,Object *obj,struct MUIP_MainGroup_AppMessage *msg)
{
    register int i;
    	
    for (i = 0; i<msg->amsg->am_NumArgs; i++)
    {
        STRPTR waname = msg->amsg->am_ArgList[i].wa_Name;
	BPTR   walock = msg->amsg->am_ArgList[i].wa_Lock;

        if (walock && waname && *waname)
        {
	    struct data *data = INST_DATA(cl,obj);
	    UBYTE       file[256], URL[256];

            if (NameFromLock(walock,file,sizeof(file)))
	    {
		if (AddPart(file,waname,sizeof(file)))
	        {
		    msnprintf(URL,sizeof(URL),"file://%s",file);
		    DoMethod(data->sHTML,MUIM_RSS_GotoURL,(ULONG)URL,MUIV_RSS_GotoURL_IgnorePrefs|MUIV_RSS_GotoURL_NoTarget);
                    break;
	        }
	    }
	}
    }

    return 0;
}

/***********************************************************************/


M_DISP(dispatcher)
{
    M_DISPSTART

    switch (msg->MethodID)
    {
        case OM_NEW:                      return mNew(cl,obj,(APTR)msg);
        case OM_DISPOSE:                  return mDispose(cl,obj,(APTR)msg);
        case OM_GET:                      return mGet(cl,obj,(APTR)msg);
        case OM_SET:                      return mSets(cl,obj,(APTR)msg);

        case MUIM_Setup:                  return mSetup(cl,obj,(APTR)msg);
        case MUIM_Cleanup:                return mCleanup(cl,obj,(APTR)msg);
        case MUIM_Show:                   return mShow(cl,obj,(APTR)msg);
        case MUIM_DragQuery:              return mDragQuery(cl,obj,(APTR)msg);
        case MUIM_DragBegin:              return mDragBegin(cl,obj,(APTR)msg);
        case MUIM_DragReport:             return mDragReport(cl,obj,(APTR)msg);
        case MUIM_DragFinish:             return mDragFinish(cl,obj,(APTR)msg);
        case MUIM_DragDrop:               return mDragDrop(cl,obj,(APTR)msg);

        case MUIM_RSS_GotoURL:            return mGoURL(cl,obj,(APTR)msg);
        case MUIM_RSS_HTTPStatus:         return mHTTPStatus(cl,obj,(APTR)msg);

        case MUIM_MainGroup_EntryChange:  return mEntryChange(cl,obj,(APTR)msg);
        case MUIM_MainGroup_Stop:         return mStop(cl,obj,(APTR)msg);
        case MUIM_MainGroup_Search:       return mSearch(cl,obj,(APTR)msg);
        case MUIM_MainGroup_SetInfo:      return mSetInfo(cl,obj,(APTR)msg);
        case MUIM_MainGroup_SetFeedInfo:  return mSetFeedInfo(cl,obj,(APTR)msg);
        case MUIM_MainGroup_ActivePage:   return mActivePage(cl,obj,(APTR)msg);
        case MUIM_MainGroup_Editor:       return mEditor(cl,obj,(APTR)msg);
        case MUIM_MainGroup_CurrentURL:   return mCurrentURL(cl,obj,(APTR)msg);
        case MUIM_MainGroup_UserURL:      return mUserURL(cl,obj,(APTR)msg);
        case MUIM_MainGroup_FlushImages:  return mFlushImages(cl,obj,(APTR)msg);
        case MUIM_MainGroup_FlushCookies: return mFlushCookies(cl,obj,(APTR)msg);
        case MUIM_MainGroup_AppMessage:   return mAppMessage(cl,obj,(APTR)msg);

        default:                          return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(dispatcher)

/***********************************************************************/

ULONG
initGroupClass(void)
{
    if (initFSClass())
    {
        if (groupClass = MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(struct data),DISP(dispatcher)))
        {
            localizeButtonsBar(buttons);
            localizeButtonsBar(nbuttons);

            g_title = getString(MSG_Title_Headline);
            g_titleLen = strlen(g_title);

            g_date = getString(MSG_Title_Date);
            g_dateLen = strlen(g_date);

            g_author = getString(MSG_Title_Author);
            g_authorLen = strlen(g_author);

            return TRUE;
        }

	disposeFSClass();
    }

    return FALSE;
}

/***********************************************************************/

void
disposeGroupClass(void)
{
    disposeFSClass();
    MUI_DeleteCustomClass(groupClass);
}

/***********************************************************************/
