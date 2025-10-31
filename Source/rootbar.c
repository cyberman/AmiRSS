
#include "rss.h"
#define CATCOMP_NUMBERS
#include "loc.h"

/***********************************************************************/

struct data
{
    Object          *app;
    Object          *this;
    APTR            menu;

    ULONG           prefsTag;
    ULONG           prefsPage;
    ULONG           flags;
};

enum
{
    FLG_TextOnly = 1<<0,
};

/***********************************************************************/

static struct NewMenu tnMenu[] =
{
    MTITLE(MSG_CMenu_Title_TheBar),
        MEITEM(MSG_CMenu_ViewMode_TextGfx,MSG_CMenu_ViewMode_TextGfx,2+4),
        MEITEM(MSG_CMenu_ViewMode_Gfx,MSG_CMenu_ViewMode_Gfx,1+4),
        MEITEM(MSG_CMenu_ViewMode_Text,MSG_CMenu_ViewMode_Text,1+2),
        MBAR,
        MTITEM(MSG_CMenu_Borderless,MSG_CMenu_Borderless),
        MTITEM(MSG_CMenu_Sunny,MSG_CMenu_Sunny),
        MTITEM(MSG_CMenu_Raised,MSG_CMenu_Raised),
        MBAR,
        MITEM(MSG_CMenu_Configure,MSG_CMenu_Configure),
    MEND
};

static  ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    if (obj = (Object *)DoSuperNew(cl,obj,
            MUIA_Group_Horiz,                TRUE,
            MUIA_TheBar_MinVer,              19,
            MUIA_TheBar_ViewMode,            DEF_ViewMode,
            MUIA_TheBar_LabelPos,            DEF_LPos,
            MUIA_TheBar_BarPos,              DEF_Layout,
            MUIA_TheBar_Borderless,          DEF_BFlags & BFLG_Borderless,
            MUIA_TheBar_Sunny,               DEF_BFlags & BFLG_Sunny,
            MUIA_TheBar_Raised,              DEF_BFlags & BFLG_Raised,
            MUIA_TheBar_Scaled,              DEF_BFlags & BFLG_Scaled,
            MUIA_TheBar_EnableKeys,          DEF_BFlags & BFLG_EnableKeys,
            MUIA_TheBar_DragBar,             DEF_BFlags & BFLG_DragBar,
            MUIA_TheBar_Frame,               DEF_BFlags & BFLG_Frame,
            MUIA_TheBar_BarSpacer,           DEF_BFlags & BFLG_BarSpacer,
	    MUIA_TheBar_ForceWindowActivity, TRUE,
            TAG_MORE, (ULONG)msg->ops_AttrList))
    {
        struct data *data = INST_DATA(cl,obj);
        ULONG       tonly;

        data->this = obj;

        superget(cl,obj,MUIA_TheBar_TextOnly,&tonly);
        if (tonly) data->flags |= FLG_TextOnly;

        if (GetTagData(MUIA_RootBar_NBar,FALSE,msg->ops_AttrList))
        {
            data->prefsTag  = MUIA_RSS_NBarPrefs;
            data->prefsPage = MUIV_Prefs_Page_NBar;
        }
        else
        {
            data->prefsTag  = MUIA_RSS_BarPrefs;
            data->prefsPage = MUIV_Prefs_Page_Bar;
        }

        if (data->menu = MUI_MakeObject(MUIO_MenustripNM,(ULONG)tnMenu,0))
        {
            if (tonly)
            {
                set((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_ViewMode_TextGfx),MUIA_Menuitem_Enabled,FALSE);
                set((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_ViewMode_Gfx),MUIA_Menuitem_Enabled,FALSE);
                set((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_ViewMode_Text),MUIA_Menuitem_Enabled,FALSE);
            }
        }

        if (data->menu) SetSuperAttrs(cl,obj,MUIA_ContextMenu,TRUE,TAG_DONE);
    }

    return (ULONG)obj;
}

/***********************************************************************/

static  ULONG
mDispose(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (data->menu) MUI_DisposeObject(data->menu);

    return DoSuperMethodA(cl,obj,msg);

}

/***********************************************************************/

static ULONG
mContextMenuChoice(struct IClass *cl,Object *obj,struct MUIP_ContextMenuChoice *msg)
{
    struct data *data = INST_DATA(cl,obj);
    ULONG       id = muiUserData(msg->item);

    if (id==MSG_CMenu_Configure) DoMethod(data->app,MUIM_App_PrefsWin,data->prefsPage);
    else
    {
        struct bprefs *oprefs, prefs;

        get(data->app,data->prefsTag,&oprefs);
        copymem(&prefs,oprefs,sizeof(prefs));

        switch (id)
        {
            case MSG_CMenu_ViewMode_TextGfx:
                prefs.viewMode = MUIV_TheBar_ViewMode_TextGfx;
                break;

            case MSG_CMenu_ViewMode_Gfx:
                prefs.viewMode = MUIV_TheBar_ViewMode_Gfx;
                break;

            case MSG_CMenu_ViewMode_Text:
                prefs.viewMode = MUIV_TheBar_ViewMode_Text;
                break;

            case MSG_CMenu_Borderless:
                if (prefs.bFlags & BFLG_Borderless) prefs.bFlags &= ~BFLG_Borderless;
                else prefs.bFlags |= BFLG_Borderless;
                break;

            case MSG_CMenu_Raised:
                if (prefs.bFlags & BFLG_Raised) prefs.bFlags &= ~BFLG_Raised;
                else prefs.bFlags |= BFLG_Raised;
                break;

            case MSG_CMenu_Sunny:
                if (prefs.bFlags & BFLG_Sunny) prefs.bFlags &= ~BFLG_Sunny;
                else prefs.bFlags |= BFLG_Sunny;
                break;
        }

        if (!(prefs.bFlags & BFLG_Borderless)) prefs.bFlags &= ~BFLG_Raised;

        set(data->app,data->prefsTag,&prefs);
    }

    return 0;
}

/***********************************************************************/

static ULONG
mContextMenuBuild(struct IClass *cl,Object *obj,struct MUIP_ContextMenuBuild *msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (data->menu)
    {
        struct bprefs  *prefs;
        ULONG  vm = 0, bl; // gcc
        ULONG  dm;

        data->app = _app(obj);
        get(data->app,data->prefsTag,&prefs);

        if (!(data->flags & FLG_TextOnly)) vm = prefs->viewMode;
        bl = prefs->bFlags & BFLG_Borderless;
        get(obj,MUIA_TheBar_DisMode,&dm);

        if (!(data->flags & FLG_TextOnly))
        {
            if (vm==MUIV_TheBar_ViewMode_TextGfx) vm = MSG_CMenu_ViewMode_TextGfx;
            else if (vm==MUIV_TheBar_ViewMode_Gfx) vm = MSG_CMenu_ViewMode_Gfx;
                 else vm = MSG_CMenu_ViewMode_Text;

            set((Object *)DoMethod(data->menu,MUIM_FindUData,vm),MUIA_Menuitem_Checked,TRUE);
        }

        set((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_Borderless),MUIA_Menuitem_Checked,prefs->bFlags & BFLG_Borderless);
        SetAttrs((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_Sunny),MUIA_Menuitem_Checked,prefs->bFlags & BFLG_Sunny,MUIA_Menuitem_Enabled,dm!=MUIV_TheBar_DisMode_Sunny,TAG_DONE);
        SetAttrs((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_Raised),MUIA_Menuitem_Enabled,bl,MUIA_Menuitem_Checked,bl && (prefs->bFlags & BFLG_Raised),TAG_DONE);

        return (ULONG)data->menu;
    }

    return NULL;
}

/***********************************************************************/

M_DISP(dispatcher)
{
    M_DISPSTART

    switch(msg->MethodID)
    {
        case OM_NEW:                 return mNew(cl,obj,(APTR)msg);
        case OM_DISPOSE:             return mDispose(cl,obj,(APTR)msg);

        case MUIM_ContextMenuBuild:  return mContextMenuBuild(cl,obj,(APTR)msg);
        case MUIM_ContextMenuChoice: return mContextMenuChoice(cl,obj,(APTR)msg);

        default:                     return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(dispatcher)

/***********************************************************************/

ULONG
initRootBarClass(void)
{
    if (rootBarClass = MUI_CreateCustomClass(NULL,MUIC_TheBarVirt,NULL,sizeof(struct data),DISP(dispatcher)))
    {
        localizeMenus(tnMenu);

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/

void
disposeRootBarClass(void)
{
    MUI_DeleteCustomClass(rootBarClass);
}

/***********************************************************************/
