
#include "rss.h"
#define CATCOMP_NUMBERS
#include "loc.h"

/***********************************************************************/

#ifdef __MORPHOS__

static struct MUI_CustomClass *listerClass = NULL;
#define listerObject   NewObject(listerClass->mcc_Class,NULL

struct imageID
{
    STRPTR file;
    ULONG  id;
};

#define IMAGES 6

static struct imageID images[IMAGES] =
{
    DEF_IMGDIR"/Options",   MSG_Prefs_Title_Options,
    DEF_IMGDIR"/Locale",    MSG_Prefs_Title_Locale,
    DEF_IMGDIR"/HTTP",      MSG_Prefs_Title_HTTP,
    DEF_IMGDIR"/ButBar",    MSG_Prefs_Title_Bar,
    DEF_IMGDIR"/NavBar",    MSG_Prefs_Title_NBar,
    NULL
};

struct listerData
{
    Object *objs[IMAGES];
    APTR   *imgs[IMAGES];
    UBYTE  title[256];
    ULONG  flags;
};

enum
{
    LFLG_NoImages = 1<<0,
};

static ULONG
mListerNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    if (obj = (Object *)DoSuperNew(cl,obj,
            MUIA_Frame,             MUIV_Frame_InputList,
            MUIA_ContextMenu,       NULL,
            MUIA_CycleChain,        TRUE,
            MUIA_List_AdjustWidth,  TRUE,
            MUIA_List_AutoVisible,  TRUE,
            0x8042bc08, 1, /* Auto Line Height */

            //0x8042408a, 1, /* ?!? */
            //0x804234c4, 1, /* RMB changes active */

            TAG_MORE, msg->ops_AttrList))
    {
        struct listerData *data = INST_DATA(cl,obj);
        register ULONG    noImages = FALSE;
    	register int	  i;

    	for (i = 0; images[i].file; i++)
        {
            DoSuperMethod(cl,obj,MUIM_List_InsertSingle,i+1,MUIV_List_Insert_Bottom);

	    if (!noImages)
                if (!(data->objs[i] = picObject, MUIA_Pic_File, images[i].file, MUIA_Pic_Transparent, TRUE, End))
                    noImages = TRUE;
    	}

        if (noImages) data->flags |= LFLG_NoImages;
    }        

    return (ULONG)obj;
}

static ULONG
mListerDispose(struct IClass *cl,Object *obj,Msg msg)
{
    struct listerData *data = INST_DATA(cl,obj);
    register int      i;

    for (i = 0; i<IMAGES; i++)
	if (data->objs[i]) MUI_DisposeObject(data->objs[i]);
        else break;

    return DoSuperMethodA(cl,obj,msg);
}

static ULONG
mListerSetup(struct IClass *cl,Object *obj,Msg msg)
{
    struct listerData *data = INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    if (!(data->flags & LFLG_NoImages))
    {
    	register int i;

        for (i = 0; images[i].file; i++)
        {
            data->imgs[i] = (APTR)DoSuperMethod(cl,obj,MUIM_List_CreateImage,(ULONG)data->objs[i],0);

            if (!data->imgs[i])
            {
            	data->flags |= LFLG_NoImages;
                break;
            }
        }
    }

    return TRUE;
}

static ULONG
mListerCleanup(struct IClass *cl,Object *obj,Msg msg)
{
    struct listerData *data = INST_DATA(cl,obj);
    register int      i;

    for (i = 0; i<IMAGES; i++)
        if (data->imgs[i])
        {
            DoSuperMethod(cl,obj,MUIM_List_DeleteImage,(ULONG)data->imgs[i]);
            data->imgs[i] = NULL;
	}
    	else break;

    data->flags &= ~LFLG_NoImages;

    return DoSuperMethodA(cl,obj,msg);
}

/*static ULONG
mListerAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
    register struct MUI_MinMax *mi;

    DoSuperMethodA(cl,obj,(Msg)msg);

    mi = msg->MinMaxInfo;

    mi->MinWidth  += 8;
    mi->DefWidth  += 8;
    mi->MaxWidth  += 8;

    return 0;
}*/

/***********************************************************************/

static ULONG
mListerConstruct(struct IClass *cl,Object *obj,struct MUIP_List_Construct *msg)
{
    return (ULONG)msg->entry;
}

static ULONG
mListerDestruct(struct IClass *cl,Object *obj,struct MUIP_List_Destruct *msg)
{
    return 0;
}

static ULONG
mListerDisplay(struct IClass *cl,Object *obj,struct MUIP_List_Display *msg)
{
    struct listerData *data = INST_DATA(cl,obj);
    ULONG 	      id = (ULONG)msg->entry;
    STRPTR	      s;

    id--;
    s = getString(images[id].id);

    if (data->flags & LFLG_NoImages) stccpy(data->title,s,sizeof(data->title));
    else msnprintf(data->title,sizeof(data->title),"\33O[%08lx] %s",(ULONG)data->imgs[id],(ULONG)s);

    *msg->array = data->title;

    return 0;
}

M_DISP(listerDispatcher)
{
    M_DISPSTART

    switch (msg->MethodID)
    {
        case OM_NEW: 	   	       return mListerNew(cl,obj,(APTR)msg);
        case OM_DISPOSE:       	   return mListerDispose(cl,obj,(APTR)msg);

        case MUIM_Setup:   	       return mListerSetup(cl,obj,(APTR)msg);
        case MUIM_Cleanup: 	       return mListerCleanup(cl,obj,(APTR)msg);
        //case MUIM_AskMinMax:       return mListerAskMinMax(cl,obj,(APTR)msg);

        case MUIM_List_Construct:  return mListerConstruct(cl,obj,(APTR)msg);
        case MUIM_List_Destruct:   return mListerDestruct(cl,obj,(APTR)msg);
        case MUIM_List_Display:    return mListerDisplay(cl,obj,(APTR)msg);

        default:     	   	       return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(listerDispatcher)

static ULONG
initListerClass(void)
{
    return (ULONG)(listerClass = MUI_CreateCustomClass(NULL,MUIC_List,NULL,sizeof(struct listerData),DISP(listerDispatcher)));
}

static void
disposeListerClass(void)
{
    MUI_DeleteCustomClass(listerClass);
}
#endif

/**************************************************************************/

struct page
{
    Object *obj;
    ULONG  index;
};

struct data
{
    struct prefs                 backPrefs;
    struct prefs                 prefs;

    Object                       *lister;
    Object                       *pager;
    struct page                  pages[16];
    ULONG                        pagesIndexCounter;

    Object                       *groupDoubleClick;
    Object                       *entriesDoubleClick;
    Object                       *descrClick;
    Object                       *editor;
    Object                       *userItem;
    Object                       *userItemName;

    //Object                       *noListBar;
    //Object                       *boldTitles;
    Object                       *rightMouse;
    Object                       *boldGroups;
    Object                       *substDate;
    Object                       *substWinChars;
    Object                       *showFeedImage;
    Object                       *useFeedsListImages;
    Object                       *useDefaultGroupImage;
    Object                       *scaleImages;
    Object                       *showGroupNews;
    Object                       *maxImageSize;
    Object			             *useHandMouse;

    Object                       *codeset;
    Object                       *GMT;

    Object                       *proxy;
    Object                       *proxyPort;
    Object                       *agent;
    Object                       *useProxy;
    Object                       *ntLoadImages;

    Object                       *viewMode;
    Object                       *bPos;
    Object                       *layout;
    Object                       *lPos;
    Object                       *borderless;
    Object                       *sunny;
    Object                       *raising;
    Object                       *scaled;
    Object                       *barSpacer;
    Object                       *underscore;
    #ifndef __MORPHOS__
    Object                       *frame;
    #endif
    Object                       *dragbar;

    Object                       *nviewMode;
    Object                       *nbPos;
    Object                       *nlayout;
    Object                       *nlPos;
    Object                       *nborderless;
    Object                       *nsunny;
    Object                       *nraising;
    Object                       *nscaled;
    Object                       *nbarSpacer;
    Object                       *nunderscore;
    #ifndef __MORPHOS__
    Object                       *nframe;
    #endif
    Object                       *ndragbar;

    ULONG                        flags;
};

enum
{
    FLG_Test = 1<<0,
    FLG_Page = 1<<1,
    FLG_TestPrefs = 1<<2,
};

/***********************************************************************/

static void
copyPrefs(struct prefs *to,struct prefs *from)
{
    if (to!=from) copymem(to,from,sizeof(*to));
}

/***********************************************************************/

static void
prefsToGadgets(struct data *data,struct prefs *prefs,ULONG page,ULONG all)
{
    ULONG v;

    copyPrefs(&data->prefs,prefs);

    if ((all || page==MUIV_Prefs_Page_Options) && data->pages[MUIV_Prefs_Page_Options].obj)
    {
        nnset(data->groupDoubleClick,MUIA_Cycle_Active,prefs->groupDoubleClick);
        nnset(data->entriesDoubleClick,MUIA_Cycle_Active,prefs->entriesDoubleClick);
        nnset(data->descrClick,MUIA_Cycle_Active,prefs->descrClick);
        nnset(data->editor,MUIA_Textinput_Contents,prefs->editor);
        nnset(data->userItem,MUIA_Textinput_Contents,prefs->userItem);
        nnset(data->userItemName,MUIA_Textinput_Contents,prefs->userItemName);

        nnset(data->scaleImages,MUIA_Selected,prefs->flags & PFLG_ScaleImages);
        nnset(data->maxImageSize,MUIA_Numeric_Value,prefs->maxImageSize);

        nnset(data->rightMouse,MUIA_Selected,prefs->flags & PFLG_RightMouse);
        //nnset(data->noListBar,MUIA_Selected,prefs->flags & PFLG_NoListBar);
        //nnset(data->boldTitles,MUIA_Selected,prefs->flags & PFLG_BoldTitles);
        nnset(data->boldGroups,MUIA_Selected,prefs->flags & PFLG_BoldGroups);
        nnset(data->substDate,MUIA_Selected,prefs->flags & PFLG_SubstDate);
        nnset(data->substWinChars,MUIA_Selected,prefs->flags & PFLG_SubstWinChars);
        nnset(data->showFeedImage,MUIA_Selected,prefs->flags & PFLG_ShowFeedImage);
        nnset(data->useFeedsListImages,MUIA_Selected,prefs->flags & PFLG_UseFeedsListImages);
        nnset(data->useDefaultGroupImage,MUIA_Selected,prefs->flags & PFLG_UseDefaultGroupImage);
        nnset(data->showGroupNews,MUIA_Selected,prefs->flags & PFLG_ShowGroupNews);
        nnset(data->useHandMouse,MUIA_Selected,prefs->flags & PFLG_UseHandMouse);
    }

    if ((all || page==MUIV_Prefs_Page_Locale) && data->pages[MUIV_Prefs_Page_Locale].obj)
    {
        nnset(data->codeset,MUIA_Textinput_Contents,prefs->codeset);
        nnset(data->GMT,MUIA_Numeric_Value,prefs->GMTOffset/30);
    }

    if ((all || page==MUIV_Prefs_Page_HTTP) && data->pages[MUIV_Prefs_Page_HTTP].obj)
    {
        nnset(data->proxy,MUIA_Textinput_Contents,prefs->proxy);
        nnset(data->proxyPort,MUIA_Textinput_Integer,prefs->proxyPort);
        nnset(data->agent,MUIA_Textinput_Contents,prefs->agent);

        nnset(data->useProxy,MUIA_Selected,prefs->httpFlags & PHFLG_UseProxy);
        nnset(data->ntLoadImages,MUIA_Selected,prefs->httpFlags & PHFLG_NTLoadImages);
    }

    if ((all || page==MUIV_Prefs_Page_Bar) && data->pages[MUIV_Prefs_Page_Bar].obj)
    {
        nnset(data->viewMode,MUIA_Cycle_Active,prefs->bprefs.viewMode);
        nnset(data->lPos,MUIA_Cycle_Active,prefs->bprefs.lPos);
        switch (prefs->bprefs.bPos)
        {
            case BPOS_Top:         v = 0; break;
            case BPOS_Bottom:      v = 1; break;
            case BPOS_Left:        v = 2; break;
            case BPOS_Right:       v = 3; break;
            case BPOS_InnerTop:    v = 4; break;
            case BPOS_InnerBottom: v = 5; break;
            default:               v = 0; break;
        }
        nnset(data->bPos,MUIA_Cycle_Active,v);

        nnset(data->layout,MUIA_Cycle_Active,prefs->bprefs.layout);
        nnset(data->borderless,MUIA_Selected,prefs->bprefs.bFlags & BFLG_Borderless);
        nnset(data->sunny,MUIA_Selected,prefs->bprefs.bFlags & BFLG_Sunny);
        nnset(data->raising,MUIA_Selected,prefs->bprefs.bFlags & BFLG_Raised);
        nnset(data->scaled,MUIA_Selected,prefs->bprefs.bFlags & BFLG_Scaled);
        nnset(data->barSpacer,MUIA_Selected,prefs->bprefs.bFlags & BFLG_BarSpacer);
        nnset(data->underscore,MUIA_Selected,prefs->bprefs.bFlags & BFLG_EnableKeys);
        #ifndef __MORPHOS__
        nnset(data->frame,MUIA_Selected,prefs->bprefs.bFlags & BFLG_Frame);
        #endif
        nnset(data->dragbar,MUIA_Selected,prefs->bprefs.bFlags & BFLG_DragBar);
    }

    if ((all || page==MUIV_Prefs_Page_NBar) && data->pages[MUIV_Prefs_Page_NBar].obj)
    {
        nnset(data->nviewMode,MUIA_Cycle_Active,prefs->nbprefs.viewMode);
        nnset(data->nlPos,MUIA_Cycle_Active,prefs->nbprefs.lPos);
        switch (prefs->nbprefs.bPos)
        {
            case BPOS_Top:         v = 0; break;
            case BPOS_Bottom:      v = 1; break;
            case BPOS_Left:        v = 2; break;
            case BPOS_Right:       v = 3; break;
            case BPOS_InnerTop:    v = 4; break;
            case BPOS_InnerBottom: v = 5; break;
            default:               v = 0; break;
        }
        nnset(data->nbPos,MUIA_Cycle_Active,v);

        nnset(data->nlayout,MUIA_Cycle_Active,prefs->nbprefs.layout);
        nnset(data->nborderless,MUIA_Selected,prefs->nbprefs.bFlags & BFLG_Borderless);
        nnset(data->nsunny,MUIA_Selected,prefs->nbprefs.bFlags & BFLG_Sunny);
        nnset(data->nraising,MUIA_Selected,prefs->nbprefs.bFlags & BFLG_Raised);
        nnset(data->nscaled,MUIA_Selected,prefs->nbprefs.bFlags & BFLG_Scaled);
        nnset(data->nbarSpacer,MUIA_Selected,prefs->nbprefs.bFlags & BFLG_BarSpacer);
        nnset(data->nunderscore,MUIA_Selected,prefs->nbprefs.bFlags & BFLG_EnableKeys);
        #ifndef __MORPHOS__
        nnset(data->nframe,MUIA_Selected,prefs->nbprefs.bFlags & BFLG_Frame);
        #endif
        nnset(data->ndragbar,MUIA_Selected,prefs->nbprefs.bFlags & BFLG_DragBar);
    }
}

/***********************************************************************/

static void
gadgetsToPrefs(struct data *data,struct prefs *prefs)
{
    ULONG  v = 0; //gcc
    STRPTR c;

    if (data->pages[MUIV_Prefs_Page_Options].obj)
    {
        prefs->flags = 0;

        prefs->groupDoubleClick = xget(data->groupDoubleClick,MUIA_Cycle_Active);
        prefs->entriesDoubleClick = xget(data->entriesDoubleClick,MUIA_Cycle_Active);
        prefs->descrClick = xget(data->descrClick,MUIA_Cycle_Active);

        c = (STRPTR)xget(data->editor,MUIA_Textinput_Contents);
        if (*c) stccpy(prefs->editor,c,sizeof(prefs->agent));
        else *prefs->editor = 0;

        c = (STRPTR)xget(data->userItem,MUIA_Textinput_Contents);
        if (*c) stccpy(prefs->userItem,c,sizeof(prefs->userItem));
        else *prefs->userItem = 0;

        c = (STRPTR)xget(data->userItemName,MUIA_Textinput_Contents);
        if (*c) stccpy(prefs->userItemName,c,sizeof(prefs->userItemName));
        else *prefs->userItemName = 0;

        if (xget(data->scaleImages,MUIA_Selected)) prefs->flags |= PFLG_ScaleImages;
        prefs->maxImageSize = xget(data->maxImageSize,MUIA_Numeric_Value);

        //if (xget(data->noListBar,MUIA_Selected))     prefs->flags |= PFLG_NoListBar;
        //if (xget(data->boldTitles,MUIA_Selected))    prefs->flags |= PFLG_BoldTitles;
        if (xget(data->rightMouse,MUIA_Selected))    prefs->flags |= PFLG_RightMouse;
        if (xget(data->boldGroups,MUIA_Selected))    prefs->flags |= PFLG_BoldGroups;
        if (xget(data->substDate,MUIA_Selected))     prefs->flags |= PFLG_SubstDate;
        if (xget(data->showFeedImage,MUIA_Selected)) prefs->flags |= PFLG_ShowFeedImage;
        if (xget(data->substWinChars,MUIA_Selected)) prefs->flags |= PFLG_SubstWinChars;
        if (xget(data->showFeedImage,MUIA_Selected)) prefs->flags |= PFLG_ShowFeedImage;
        if (xget(data->useFeedsListImages  ,MUIA_Selected)) prefs->flags |= PFLG_UseFeedsListImages;
        if (xget(data->useDefaultGroupImage,MUIA_Selected)) prefs->flags |= PFLG_UseDefaultGroupImage;
        if (xget(data->showGroupNews,MUIA_Selected)) prefs->flags |= PFLG_ShowGroupNews;
        if (xget(data->useHandMouse,MUIA_Selected)) prefs->flags |= PFLG_UseHandMouse;
    }

    if (data->pages[MUIV_Prefs_Page_Locale].obj)
    {
        c = (STRPTR)xget(data->codeset,MUIA_Textinput_Contents);
        if (*c) stccpy(prefs->codeset,c,sizeof(prefs->codeset));
        else *prefs->codeset = 0;

        prefs->GMTOffset = xget(data->GMT,MUIA_Numeric_Value)*30;
    }

    if (data->pages[MUIV_Prefs_Page_HTTP].obj)
    {
        c = (STRPTR)xget(data->proxy,MUIA_Textinput_Contents);
        if (*c) stccpy(prefs->proxy,c,sizeof(prefs->proxy));
        else *prefs->proxy = 0;

        prefs->proxyPort = xget(data->proxyPort,MUIA_Textinput_Integer);

        c = (STRPTR)xget(data->agent,MUIA_Textinput_Contents);
        if (*c) stccpy(prefs->agent,c,sizeof(prefs->agent));
        else *prefs->agent = 0;

        prefs->httpFlags = 0;
        if (xget(data->useProxy,MUIA_Selected)) prefs->httpFlags |= PHFLG_UseProxy;
        if (xget(data->ntLoadImages,MUIA_Selected)) prefs->httpFlags |= PHFLG_NTLoadImages;
    }

    if (data->pages[MUIV_Prefs_Page_Bar].obj)
    {
        prefs->bprefs.viewMode = xget(data->viewMode,MUIA_Cycle_Active);
        prefs->bprefs.lPos = xget(data->lPos,MUIA_Cycle_Active);
        switch (xget(data->bPos,MUIA_Cycle_Active))
        {
            case 0: v = BPOS_Top;         break;
            case 1: v = BPOS_Bottom;      break;
            case 2: v = BPOS_Left;        break;
            case 3: v = BPOS_Right;       break;
            case 4: v = BPOS_InnerTop;    break;
            case 5: v = BPOS_InnerBottom; break;
        }
        prefs->bprefs.bPos = v;
        prefs->bprefs.layout = xget(data->layout,MUIA_Cycle_Active);

        prefs->bprefs.bFlags = 0;
        if (xget(data->borderless,MUIA_Selected)) prefs->bprefs.bFlags |= BFLG_Borderless;
        if (xget(data->sunny,MUIA_Selected))      prefs->bprefs.bFlags |= BFLG_Sunny;
        if (xget(data->raising,MUIA_Selected))    prefs->bprefs.bFlags |= BFLG_Raised;
        if (xget(data->scaled,MUIA_Selected))     prefs->bprefs.bFlags |= BFLG_Scaled;
        if (xget(data->barSpacer,MUIA_Selected))  prefs->bprefs.bFlags |= BFLG_BarSpacer;
        if (xget(data->underscore,MUIA_Selected)) prefs->bprefs.bFlags |= BFLG_EnableKeys;
        #ifndef __MORPHOS__
        if (xget(data->frame,MUIA_Selected))      prefs->bprefs.bFlags |= BFLG_Frame;
        #endif
        if (xget(data->dragbar,MUIA_Selected))    prefs->bprefs.bFlags |= BFLG_DragBar;
    }

    if (data->pages[MUIV_Prefs_Page_NBar].obj)
    {
        prefs->nbprefs.viewMode = xget(data->nviewMode,MUIA_Cycle_Active);
        prefs->nbprefs.lPos = xget(data->nlPos,MUIA_Cycle_Active);
        switch (xget(data->nbPos,MUIA_Cycle_Active))
        {
            case 0: v = BPOS_Top;         break;
            case 1: v = BPOS_Bottom;      break;
            case 2: v = BPOS_Left;        break;
            case 3: v = BPOS_Right;       break;
            case 4: v = BPOS_InnerTop;    break;
            case 5: v = BPOS_InnerBottom; break;
        }
        prefs->nbprefs.bPos = v;
        prefs->nbprefs.layout = xget(data->nlayout,MUIA_Cycle_Active);

        prefs->nbprefs.bFlags = 0;
        if (xget(data->nborderless,MUIA_Selected)) prefs->nbprefs.bFlags |= BFLG_Borderless;
        if (xget(data->nsunny,MUIA_Selected))      prefs->nbprefs.bFlags |= BFLG_Sunny;
        if (xget(data->nraising,MUIA_Selected))    prefs->nbprefs.bFlags |= BFLG_Raised;
        if (xget(data->nscaled,MUIA_Selected))     prefs->nbprefs.bFlags |= BFLG_Scaled;
        if (xget(data->nbarSpacer,MUIA_Selected))  prefs->nbprefs.bFlags |= BFLG_BarSpacer;
        if (xget(data->nunderscore,MUIA_Selected)) prefs->nbprefs.bFlags |= BFLG_EnableKeys;
        #ifndef __MORPHOS__
        if (xget(data->nframe,MUIA_Selected))      prefs->nbprefs.bFlags |= BFLG_Frame;
        #endif
        if (xget(data->ndragbar,MUIA_Selected))    prefs->nbprefs.bFlags |= BFLG_DragBar;
    }
}

/***********************************************************************/

static STRPTR gdcs[] =
{
    (STRPTR)MSG_Prefs_GroupDoubleClick_Nothing,
    (STRPTR)MSG_Prefs_GroupDoubleClick_Edit,
    (STRPTR)MSG_Prefs_GroupDoubleClick_Feed,
    0
};

static STRPTR edcs[] =
{
    (STRPTR)MSG_Prefs_EntriesDoubleClick_Nothing,
    (STRPTR)MSG_Prefs_EntriesDoubleClick_Browser,
    (STRPTR)MSG_Prefs_EntriesDoubleClick_GoToLink,
    0
};

static STRPTR ddcs[] =
{
    (STRPTR)MSG_Prefs_DescrClick_Nothing,
    (STRPTR)MSG_Prefs_DescrClick_Browser,
    (STRPTR)MSG_Prefs_DescrClick_GoToLink,
    0
};

static STRPTR editorSyms[] =
{
    "%s",
    "%f",
    NULL
};

static STRPTR editorNames[] =
{
    (STRPTR)MSG_Prefs_Popph_File,
    (STRPTR)MSG_Prefs_Popph_Screen,
    0
};

static STRPTR userItemSyms[] =
{
    "%s",
    "%f",
    NULL
};

static STRPTR userItemNames[] =
{
    (STRPTR)MSG_Prefs_Popph_Link,
    (STRPTR)MSG_Prefs_Popph_Screen,
    0
};

static STRPTR viewModes[] =
{
    (STRPTR)MSG_Prefs_ViewMode_TextGfx,
    (STRPTR)MSG_Prefs_ViewMode_Gfx,
    (STRPTR)MSG_Prefs_ViewMode_Text,
    0
};

static STRPTR lPoss[] =
{
    (STRPTR)MSG_Prefs_LPos_Bottom,
    (STRPTR)MSG_Prefs_LPos_Top,
    (STRPTR)MSG_Prefs_LPos_Right,
    (STRPTR)MSG_Prefs_LPos_Left,
    0
};

static STRPTR bPoss[] =
{
    (STRPTR)MSG_Prefs_BPos_Top,
    (STRPTR)MSG_Prefs_BPos_Bottom,
    (STRPTR)MSG_Prefs_BPos_Left,
    (STRPTR)MSG_Prefs_BPos_Right,
    (STRPTR)MSG_Prefs_BPos_InnerTop,
    (STRPTR)MSG_Prefs_BPos_InnerBottom,
    0
};

static STRPTR layouts[] =
{
    (STRPTR)MSG_Prefs_Layout_Up,
    (STRPTR)MSG_Prefs_Layout_Center,
    (STRPTR)MSG_Prefs_Layout_Down,
    0
};

static STRPTR nbPoss[] =
{
    (STRPTR)MSG_Prefs_BPos_Top,
    (STRPTR)MSG_Prefs_BPos_Bottom,
    (STRPTR)MSG_Prefs_BPos_Left,
    (STRPTR)MSG_Prefs_BPos_Right,
    0
};

#define INNER 4

static Object *
makePage(Object *obj,struct data *data,ULONG page)
{
    Object *o;

    if (o = data->pages[page].obj)
    {
        set(data->pager,MUIA_Group_ActivePage,data->pages[page].index);

        return o;
    }

    DoMethod(data->pager,MUIM_Group_InitChange);

    switch (page)
    {
        case MUIV_Prefs_Page_Options:
            o = VGroup,
                MUIA_HelpNode,    "prefs.html#Options",
                MUIA_InnerLeft,   INNER,
                MUIA_InnerTop,    INNER,
                MUIA_InnerRight,  INNER,
                MUIA_InnerBottom, INNER,
                VirtualFrame,
                MUIA_Background,  MUII_PageBack,

                Child, ColGroup(2),
                    GroupFrame,
                    MUIA_Background, MUII_GroupBack,
                    Child, olabel2(MSG_Prefs_GroupDoubleClick),
                    Child, data->groupDoubleClick = ocycle(MSG_Prefs_GroupDoubleClick,MSG_Prefs_GroupDoubleClickHelp,gdcs),
                    Child, olabel2(MSG_Prefs_EntriesDoubleClick),
                    Child, data->entriesDoubleClick = ocycle(MSG_Prefs_EntriesDoubleClick,MSG_Prefs_EntriesDoubleClickHelp,edcs),
                    Child, olabel2(MSG_Prefs_DescrClick),
                    Child, data->descrClick = ocycle(MSG_Prefs_DescrClick,MSG_Prefs_DescrClickHelp,ddcs),
                End,

                Child, ColGroup(2),
                    GroupFrame,
                    MUIA_Background, MUII_GroupBack,
                    Child, olabel2(MSG_Prefs_Editor),
                    Child, data->editor = popphObject,
                        MUIA_ShortHelp,      getString(MSG_Prefs_EditorHelp),
                        MUIA_Popph_Syms,     editorSyms,
                        MUIA_Popph_Names,    editorNames,
                        MUIA_Popph_MaxLen,   DEF_EDITORSIZE,
                        MUIA_Popph_Key,      MSG_Prefs_Editor,
                    End,
                    Child, olabel2(MSG_Prefs_UserItem),
                    Child, data->userItem = popphObject,
                        MUIA_ShortHelp,      getString(MSG_Prefs_UserItemHelp),
                        MUIA_Popph_Syms,     userItemSyms,
                        MUIA_Popph_Names,    userItemNames,
                        MUIA_Popph_MaxLen,   DEF_USERITEM,
                        MUIA_Popph_Key,      MSG_Prefs_UserItem,
                    End,
                    Child, olabel2(MSG_Prefs_UserItemName),
                    Child, data->userItemName = ostring(DEF_USERITEMNAME,MSG_Prefs_UserItemName,MSG_Prefs_UserItemNameHelp),
                    Child, olabel2(MSG_Prefs_MaxImageSize),
                    Child, HGroup,
                        MUIA_Group_HorizSpacing, 1,
                        Child, data->scaleImages = ocheck(0,MSG_Prefs_MaxImageSizeHelp),
                        Child, data->maxImageSize = oslider(MSG_Prefs_MaxImageSize,MSG_Prefs_MaxImageSizeHelp,8,64),
                    End,
                End,

                Child, rootScrollgroupObject,
                    MUIA_Scrollgroup_FreeHoriz, FALSE,
                    MUIA_Scrollgroup_Contents, VirtgroupObject,
                        MUIA_Frame, MUIV_Frame_Virtual,
                        Child, HGroup,
                            Child, ColGroup(2),
                                //Child, data->noListBar = ocheck(MSG_Prefs_NoListBar,MSG_Prefs_NoListBarHelp),
                                //Child, ollabel1(MSG_Prefs_NoListBar),
                                //Child, data->boldTitles = ocheck(MSG_Prefs_BoldTitles,MSG_Prefs_BoldTitlesHelp),
                                //Child, ollabel1(MSG_Prefs_BoldTitles),
                                Child, data->substDate = ocheck(MSG_Prefs_SubstDate,MSG_Prefs_SubstDateHelp),
                                Child, ollabel1(MSG_Prefs_SubstDate),
                                Child, data->rightMouse = ocheck(MSG_Prefs_RightMouse,MSG_Prefs_RightMouseHelp),
                                Child, ollabel1(MSG_Prefs_RightMouse),
                                Child, data->boldGroups = ocheck(MSG_Prefs_BoldGroups,MSG_Prefs_BoldGroupsHelp),
                                Child, ollabel1(MSG_Prefs_BoldGroups),
                                Child, data->substWinChars = ocheck(MSG_Prefs_SobstituteWinChars,MSG_Prefs_SobstituteWinCharsHelp),
                                Child, ollabel1(MSG_Prefs_SobstituteWinChars),
                                Child, data->showFeedImage = ocheck(MSG_Prefs_ShowFeedImage,MSG_Prefs_ShowFeedImageHelp),
                                Child, ollabel1(MSG_Prefs_ShowFeedImage),
                                Child, data->useFeedsListImages = ocheck(MSG_Prefs_UseFeedsListImages,MSG_Prefs_UseFeedsListImagesHelp),
                                Child, ollabel1(MSG_Prefs_UseFeedsListImages),
                                Child, data->useDefaultGroupImage = ocheck(MSG_Prefs_UseDefaultGroupImage,MSG_Prefs_UseDefaultGroupImageHelp),
                                Child, ollabel1(MSG_Prefs_UseDefaultGroupImage),
                                Child, data->showGroupNews = ocheck(MSG_Prefs_ShowGroupNews,MSG_Prefs_ShowGroupNewsHelp),
                                Child, ollabel1(MSG_Prefs_ShowGroupNews),
                                Child, data->useHandMouse = ocheck(MSG_Prefs_UseHandMouse,MSG_Prefs_UseHandMouseHelp),
                                Child, ollabel1(MSG_Prefs_UseHandMouse),
                            End,
                            Child, HSpace(0),
                        End,
                        Child, VSpace(0),
                    End,
                End,
            End;

            if (o)
            {
                DoMethod(data->groupDoubleClick,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->entriesDoubleClick,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->descrClick,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);

                DoMethod(data->editor,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->userItem,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->userItemName,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->scaleImages,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->maxImageSize,MUIM_Notify,MUIA_Slider_Level,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);

                DoMethod(data->substDate,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->rightMouse,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->boldGroups,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->substWinChars,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->showFeedImage,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->useFeedsListImages,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->useDefaultGroupImage,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->showGroupNews,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->useHandMouse,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
            }

            break;

        case MUIV_Prefs_Page_Locale:
        {
            Object *get;

            o = VGroup,
                MUIA_HelpNode,    "prefs.html#Locale",
                MUIA_InnerLeft,   INNER,
                MUIA_InnerTop,    INNER,
                MUIA_InnerRight,  INNER,
                MUIA_InnerBottom, INNER,
                VirtualFrame,
                MUIA_Background,  MUII_PageBack,

                Child, ColGroup(2),
                    Child, olabel2(MSG_Prefs_Codeset),
                    Child, data->codeset = popupCodesetsObject,
                        MUIA_ShortHelp, getString(MSG_Prefs_CodesetHelp),
                        MUIA_RSS_Key, MSG_Prefs_Codeset,
                    End,
                    Child, olabel2(MSG_Prefs_GMTOffset),
                    Child, HGroup,
                        Child, data->GMT = GMTObject, MUIA_ControlChar, getKeyCharID(MSG_Prefs_GMTOffset), MUIA_ShortHelp, getString(MSG_Prefs_GMTOffsetHelp), End,
                        Child, get = obutton(MSG_Prefs_GetFromSystem,MSG_Prefs_GetFromSystemHelp),
                    End,
                End,
                Child, VSpace(0),
            End;

            if (o)
            {
                set(get,MUIA_Weight,10);
                DoMethod(get,MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)obj,1,MUIM_Prefs_System);

                DoMethod(data->codeset,MUIM_Notify,MUIA_Popupcodesets_Acknowledge,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->GMT,MUIM_Notify,MUIA_Slider_Level,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
            }

            break;
        }

        case MUIV_Prefs_Page_HTTP:
            o = VGroup,
                MUIA_HelpNode,    "prefs.html#HTTP",
                MUIA_InnerLeft,   INNER,
                MUIA_InnerTop,    INNER,
                MUIA_InnerRight,  INNER,
                MUIA_InnerBottom, INNER,
                VirtualFrame,
                MUIA_Background,  MUII_PageBack,

                Child, ColGroup(2),
                    Child, olabel2(MSG_Prefs_Proxy),
                    Child, HGroup,
                        Child, data->proxy = ostring(DEF_URLSIZE,MSG_Prefs_Proxy,MSG_Prefs_ProxyHelp),
                        Child, olabel2(MSG_Prefs_ProxyPort),
                        Child, data->proxyPort = rootStringObject,
                            MUIA_Weight,                50,
                            MUIA_ShortHelp,             getString(MSG_Prefs_ProxyPortHelp),
                            MUIA_ControlChar,           getKeyCharID(MSG_Prefs_ProxyPort),
                            MUIA_CycleChain,            TRUE,
                            StringFrame,
                            MUIA_Textinput_AdvanceOnCR, TRUE,
                            MUIA_Textinput_MaxLen,      7,
                            MUIA_Textinput_AcceptChars, "0123456789",
                            MUIA_Textinput_Format,      MUIV_Textinput_Format_Right,
                            MUIA_Textinput_AdvanceOnCR, TRUE,
                        End,
                    End,
                    Child, olabel2(MSG_Prefs_Agent),
                    Child, data->agent = agentObject,
                        MUIA_ShortHelp, getString(MSG_Prefs_AgentHelp),
                        MUIA_RSS_Key, MSG_Prefs_Agent,
                    End,
                End,
                Child, rootScrollgroupObject,
                    MUIA_Scrollgroup_FreeHoriz, FALSE,
                    MUIA_Scrollgroup_Contents, VirtgroupObject,
                        MUIA_Frame, MUIV_Frame_Virtual,
                        Child, HGroup,
                            Child, ColGroup(2),
                                Child, data->useProxy = ocheck(MSG_Prefs_UseProxy,MSG_Prefs_UseProxyHelp),
                                Child, ollabel1(MSG_Prefs_UseProxy),
                                Child, data->ntLoadImages = ocheck(MSG_Prefs_NTLoadImages,MSG_Prefs_NTLoadImagesHelp),
                                Child, ollabel1(MSG_Prefs_NTLoadImages),
                            End,
                            Child, HSpace(0),
                        End,
                        Child, VSpace(0),
                    End,
                End,
            End;

            if (o)
            {
                DoMethod(data->proxy,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->proxyPort,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->agent,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);

                DoMethod(data->useProxy,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->ntLoadImages,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
            }
            break;

            break;

        case MUIV_Prefs_Page_Bar:
            o = VGroup,
                MUIA_HelpNode,    "prefs.html#MainBar",
                MUIA_InnerLeft,   INNER,
                MUIA_InnerTop,    INNER,
                MUIA_InnerRight,  INNER,
                MUIA_InnerBottom, INNER,
                VirtualFrame,
                MUIA_Background,  MUII_PageBack,

                Child, VGroup,
                    Child, ColGroup(4),
                        Child, olabel2(MSG_Prefs_BPos),
                        Child, data->bPos = ocycle(MSG_Prefs_BPos,MSG_Prefs_BPosHelp,bPoss),

                        Child, olabel2(MSG_Prefs_Layout),
                        Child, data->layout = ocycle(MSG_Prefs_Layout,MSG_Prefs_LayoutHelp,layouts),

                        Child, olabel2(MSG_Prefs_LPos),
                        Child, data->lPos = ocycle(MSG_Prefs_LPos,MSG_Prefs_LPosHelp,lPoss),

                        Child, olabel2(MSG_Prefs_ViewMode),
                        Child, data->viewMode = ocycle(MSG_Prefs_ViewMode,MSG_Prefs_ViewModeHelp,viewModes),
                    End,

                    Child, rootScrollgroupObject,
                        MUIA_Scrollgroup_FreeHoriz, FALSE,
                        MUIA_Scrollgroup_Contents, VirtgroupObject,
                            MUIA_Frame, MUIV_Frame_Virtual,
                            Child, HGroup,
                                Child, ColGroup(2),
                                    Child, data->borderless = ocheck(MSG_Prefs_Borderless,MSG_Prefs_BorderlessHelp),
                                    Child, ollabel1(MSG_Prefs_Borderless),
                                    Child, data->sunny = ocheck(MSG_Prefs_Sunny,MSG_Prefs_SunnyHelp),
                                    Child, ollabel1(MSG_Prefs_Sunny),
                                    Child, data->raising = ocheck(MSG_Prefs_Raised,MSG_Prefs_RaisedHelp),
                                    Child, ollabel1(MSG_Prefs_Raised),
                                    Child, data->scaled = ocheck(MSG_Prefs_Scaled,MSG_Prefs_ScaledHelp),
                                    Child, ollabel1(MSG_Prefs_Scaled),
                                    Child, data->underscore = ocheck(MSG_Prefs_Underscore,MSG_Prefs_UnderscoreHelp),
                                    Child, ollabel1(MSG_Prefs_Underscore),
                				    #ifndef __MORPHOS__
                                    Child, data->frame = ocheck(MSG_Prefs_Frame,MSG_Prefs_FrameHelp),
                                    Child, ollabel1(MSG_Prefs_Frame),
                                    #endif
                                    Child, data->barSpacer = ocheck(MSG_Prefs_BarSpacer,MSG_Prefs_BarSpacerHelp),
                                    Child, ollabel1(MSG_Prefs_BarSpacer),
                                    Child, data->dragbar = ocheck(MSG_Prefs_DragBar,MSG_Prefs_DragBarHelp),
                                    Child, ollabel1(MSG_Prefs_DragBar),
                                End,
                                Child, HSpace(0),
                            End,
                            Child, VSpace(0),
                        End,
                    End,
                End,
            End;

            if (o)
            {
                DoMethod(data->bPos,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->layout,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->lPos,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->viewMode,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);

                DoMethod(data->borderless,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->sunny,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->raising,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->scaled,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->underscore,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->barSpacer,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->dragbar,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
            }
            break;

        case MUIV_Prefs_Page_NBar:
            o = VGroup,
                MUIA_HelpNode,    "prefs.html#NavBar",
                MUIA_InnerLeft,   INNER,
                MUIA_InnerTop,    INNER,
                MUIA_InnerRight,  INNER,
                MUIA_InnerBottom, INNER,
                VirtualFrame,
                MUIA_Background,  MUII_PageBack,

                Child, VGroup,
                    Child, ColGroup(4),
                        Child, olabel2(MSG_Prefs_BPos),
                        Child, data->nbPos = ocycle(MSG_Prefs_BPos,MSG_Prefs_BPosHelp,nbPoss),

                        Child, olabel2(MSG_Prefs_Layout),
                        Child, data->nlayout = ocycle(MSG_Prefs_Layout,MSG_Prefs_LayoutHelp,layouts),

                        Child, olabel2(MSG_Prefs_LPos),
                        Child, data->nlPos = ocycle(MSG_Prefs_LPos,MSG_Prefs_LPosHelp,lPoss),

                        Child, olabel2(MSG_Prefs_ViewMode),
                        Child, data->nviewMode = ocycle(MSG_Prefs_ViewMode,MSG_Prefs_ViewModeHelp,viewModes),
                    End,

                    Child, rootScrollgroupObject,
                        MUIA_Scrollgroup_FreeHoriz, FALSE,
                        MUIA_Scrollgroup_Contents, VirtgroupObject,
                            MUIA_Frame, MUIV_Frame_Virtual,
                            Child, HGroup,
                                Child, ColGroup(2),
                                    Child, data->nborderless = ocheck(MSG_Prefs_Borderless,MSG_Prefs_BorderlessHelp),
                                    Child, ollabel1(MSG_Prefs_Borderless),
                                    Child, data->nsunny = ocheck(MSG_Prefs_Sunny,MSG_Prefs_SunnyHelp),
                                    Child, ollabel1(MSG_Prefs_Sunny),
                                    Child, data->nraising = ocheck(MSG_Prefs_Raised,MSG_Prefs_RaisedHelp),
                                    Child, ollabel1(MSG_Prefs_Raised),
                                    Child, data->nscaled = ocheck(MSG_Prefs_Scaled,MSG_Prefs_ScaledHelp),
                                    Child, ollabel1(MSG_Prefs_Scaled),
                                    Child, data->nunderscore = ocheck(MSG_Prefs_Underscore,MSG_Prefs_UnderscoreHelp),
                                    Child, ollabel1(MSG_Prefs_Underscore),
				                    #ifndef __MORPHOS__
                                    Child, data->nframe = ocheck(MSG_Prefs_Frame,MSG_Prefs_FrameHelp),
                                    Child, ollabel1(MSG_Prefs_Frame),
                                    #endif
                                    Child, data->nbarSpacer = ocheck(MSG_Prefs_BarSpacer,MSG_Prefs_BarSpacerHelp),
                                    Child, ollabel1(MSG_Prefs_BarSpacer),
                                    Child, data->ndragbar = ocheck(MSG_Prefs_DragBar,MSG_Prefs_DragBarHelp),
                                    Child, ollabel1(MSG_Prefs_DragBar),
                                End,
                                Child, HSpace(0),
                            End,
                            Child, VSpace(0),
                        End,
                    End,
                End,
            End;

            if (o)
            {
                DoMethod(data->nbPos,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->nlayout,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->nlPos,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->nviewMode,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);

                DoMethod(data->nborderless,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->nsunny,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->nraising,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->nscaled,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->nunderscore,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->nbarSpacer,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
                DoMethod(data->ndragbar,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(ULONG)obj,1,MUIM_Prefs_TestPrefs);
            }
            break;

            break;

        default:
            o = NULL;
            break;
    }

    if (o)
    {
        data->pages[page].obj   = o;
        data->pages[page].index = data->pagesIndexCounter++;
        prefsToGadgets(data,&data->prefs,page,0);

        DoMethod(data->pager,OM_ADDMEMBER,(ULONG)o);
        set(data->pager,MUIA_Group_ActivePage,data->pages[page].index);
    }

    DoMethod(data->pager,MUIM_Group_ExitChange);

    return o;
}

/***********************************************************************/

#ifndef __MORPHOS__
static STRPTR prefss[] =
{
    (STRPTR)MSG_Prefs_Title_Options,
    (STRPTR)MSG_Prefs_Title_Locale,
    (STRPTR)MSG_Prefs_Title_HTTP,
    (STRPTR)MSG_Prefs_Title_Bar,
    (STRPTR)MSG_Prefs_Title_NBar,
    0
};
#endif

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct data    temp;
    Object         *use, *apply, *cancel;
    struct TagItem *attrs = msg->ops_AttrList;

    memset(&temp,0,sizeof(temp));

    if (obj = (Object *)DoSuperNew(cl,obj,
            MUIA_Window_Title, (ULONG)getString(MSG_Prefs_Win_Title),
            MUIA_Window_ID,    MAKE_ID('W','P','R','E'),
            MUIA_HelpNode,     (ULONG)"prefs.html",

            WindowContents, (ULONG)(VGroup,

                Child, HGroup,
                    MUIA_Group_Spacing, 4,

                    Child, temp.lister = listerObject, End,

                    Child, temp.pager = VGroup,
                        MUIA_Group_PageMode, TRUE,
                    End,
                End,

                Child, HGroup,
                    Child, use = obutton(MSG_Prefs_Use,MSG_Prefs_UseHelp),
                    Child, wspace(80),
                    Child, apply = obutton(MSG_Prefs_Apply,MSG_Prefs_ApplyHelp),
                    Child, wspace(80),
                    Child, cancel = obutton(MSG_Prefs_Cancel,MSG_Prefs_CancelHelp),
                End,
            End),
            TAG_MORE,(ULONG)attrs))
    {
        struct data *data = INST_DATA(cl,obj);

        *data = temp;

        DoSuperMethod(cl,obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,MUIV_Notify_Self,2,MUIM_Prefs_UsePrefs,MUIV_Prefs_UsePrefs_Cancel);

        DoMethod(data->lister,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime,(ULONG)obj,3,MUIM_Set,MUIA_RSS_Prefs_Page,MUIV_TriggerValue);

        DoMethod(use,MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)obj,2,MUIM_Prefs_UsePrefs,MUIV_Prefs_UsePrefs_Use);
        DoMethod(apply,MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)obj,2,MUIM_Prefs_UsePrefs,MUIV_Prefs_UsePrefs_Apply);
        DoMethod(cancel,MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)obj,2,MUIM_Prefs_UsePrefs,MUIV_Prefs_UsePrefs_Cancel);

        SetSuperAttrs(cl,obj,MUIA_Window_DefaultObject,(ULONG)data->lister,TAG_DONE);
    }

    return (ULONG)obj;
}

/***********************************************************************/

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
                prefsToGadgets(data,(struct prefs *)tidata,0,TRUE);
                break;

            case MUIA_RSS_Prefs_Page:
                if (tidata==MUIV_Prefs_Page_None)
                    if (!(data->flags & FLG_Page)) tidata = MUIV_Prefs_Page_Options;
                    else break;

                if (makePage(obj,data,tidata))
                {
                    nnset(data->lister,MUIA_List_Active,tidata);
                    data->flags |= FLG_Page;
                }

                break;

            case MUIA_RSS_BarPos:
                data->prefs.bprefs.bPos = tidata;

                if (data->pages[MUIV_Prefs_Page_Bar].obj)
                {
                    ULONG v;

                    switch (tidata)
                    {
                        case BPOS_Top:         v = 0; break;
                        case BPOS_Bottom:      v = 1; break;
                        case BPOS_Left:        v = 2; break;
                        case BPOS_Right:       v = 3; break;
                        case BPOS_InnerTop:    v = 4; break;
                        case BPOS_InnerBottom: v = 5; break;
                        default:               v = 0; break;
                    }
                    set(data->bPos,MUIA_Cycle_Active,v);
                }
                break;

            case MUIA_RSS_NBarPos:
                data->prefs.nbprefs.bPos = tidata;

                if (data->pages[MUIV_Prefs_Page_NBar].obj)
                {
                    ULONG v;

                    switch (tidata)
                    {
                        case BPOS_Top:         v = 0; break;
                        case BPOS_Bottom:      v = 1; break;
                        case BPOS_Left:        v = 2; break;
                        case BPOS_Right:       v = 3; break;
                        case BPOS_InnerTop:    v = 4; break;
                        case BPOS_InnerBottom: v = 5; break;
                        default:               v = 0; break;
                    }
                    set(data->nbPos,MUIA_Cycle_Active,v);
                }
                break;
        }
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static ULONG
mUsePrefs(struct IClass *cl,Object *obj,struct MUIP_Prefs_UsePrefs *msg)
{
    struct data *data = INST_DATA(cl,obj);
    ULONG       close;

    if (close = msg->mode!=MUIV_Prefs_UsePrefs_Apply) superset(cl,obj,MUIA_Window_Open,FALSE);

    switch (msg->mode)
    {
        case MUIV_Prefs_UsePrefs_Use:
            gadgetsToPrefs(data,&data->prefs);
            SetAttrs(_app(obj),MUIA_RSS_Prefs,(ULONG)&data->prefs,MUIA_RSS_OneWay,TRUE,TAG_DONE);
            break;

        case MUIV_Prefs_UsePrefs_Apply:
            gadgetsToPrefs(data,&data->prefs);
            SetAttrs(_app(obj),MUIA_RSS_Prefs,(ULONG)&data->prefs,MUIA_RSS_OneWay,TRUE,TAG_DONE);
            CopyMem(&data->prefs,&data->backPrefs,sizeof(data->backPrefs));
            break;

        case MUIV_Prefs_UsePrefs_Cancel:
            if (data->flags & FLG_TestPrefs) SetAttrs(_app(obj),MUIA_RSS_Prefs,(ULONG)&data->backPrefs,MUIA_RSS_OneWay,TRUE,TAG_DONE);
            break;
    }

    if (close) DoMethod(_app(obj),MUIM_Application_PushMethod,(ULONG)_app(obj),2,MUIM_App_DisposeWin,(ULONG)obj);

    return 0;
}

/***********************************************************************/

static ULONG
mTestPrefs(struct IClass *cl,Object *obj,Msg msg)
{
    register struct data *data = INST_DATA(cl,obj);
    struct prefs         prefs;

    if (!(data->flags & FLG_TestPrefs))
    {
        CopyMem(&data->prefs,&data->backPrefs,sizeof(data->backPrefs));
        data->flags |= FLG_TestPrefs;
    }

    CopyMem(&data->prefs,&prefs,sizeof(prefs));
    gadgetsToPrefs(data,&prefs);
    SetAttrs(_app(obj),MUIA_RSS_Prefs,(ULONG)&prefs,MUIA_RSS_OneWay,TRUE,TAG_DONE);

    return 0;
}

/***********************************************************************/

static ULONG
mSystem(struct IClass *cl,Object *obj,struct MUIP_Prefs_UsePrefs *msg)
{
    struct data    *data = INST_DATA(cl,obj);
    struct codeset *codeset;

    set(data->GMT,MUIA_Numeric_Value,-g_loc->loc_GMTOffset/30);

    codeset = CodesetsFindA(NULL,NULL);
    set(data->codeset,MUIA_Textinput_Contents,codeset->name);

    return 0;
}

/***********************************************************************/

M_DISP(dispatcher)
{
    M_DISPSTART

    switch(msg->MethodID)
    {
        case OM_NEW:                  return mNew(cl,obj,(APTR)msg);
        case OM_SET:                  return mSets(cl,obj,(APTR)msg);

        case MUIM_Prefs_UsePrefs:     return mUsePrefs(cl,obj,(APTR)msg);
        case MUIM_Prefs_System:       return mSystem(cl,obj,(APTR)msg);
        case MUIM_Prefs_TestPrefs:    return mTestPrefs(cl,obj,(APTR)msg);

        default:                      return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(dispatcher)

/***********************************************************************/

ULONG
initPrefsClass(void)
{
    #ifdef __MORPHOS__
    if (initListerClass())
    {
    #endif
        if (prefsClass = MUI_CreateCustomClass(NULL,NULL,winClass,sizeof(struct data),DISP(dispatcher)))
        {
            #ifndef __MORPHOS__
            localizeStrings(prefss);
    	    #endif

            localizeStrings(gdcs);
            localizeStrings(edcs);
            localizeStrings(ddcs);
            localizeStrings(editorNames);
            localizeStrings(userItemNames);

            localizeStrings(viewModes);
            localizeStrings(lPoss);
            localizeStrings(bPoss);
            localizeStrings(nbPoss);
            localizeStrings(layouts);

            return TRUE;
        }

    #ifdef __MORPHOS__
        disposeListerClass();
    }
    #endif

    return FALSE;
}

/***********************************************************************/

void
disposePrefsClass(void)
{
    MUI_DeleteCustomClass(prefsClass);
    #ifdef __MORPHOS__
    disposeListerClass();
    #endif
}

/**********************************************************************/
