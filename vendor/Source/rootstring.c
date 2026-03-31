
#include "rss.h"
#define CATCOMP_NUMBERS
#include "loc.h"

/***********************************************************************/

struct data
{
    Object        *this;
    APTR          menu;
    ULONG         flags;
};

enum
{
    FLG_Disabled = 1<<0,
};

/***********************************************************************/

static struct NewMenu tnMenu[] =
{
    MTITLE(MSG_CMenu_Title_Edit),
        MITEM(MSG_CMenu_Cut,MSG_CMenu_Cut),
        MITEM(MSG_CMenu_Copy,MSG_CMenu_Copy),
        MITEM(MSG_CMenu_Paste,MSG_CMenu_Paste),
    MEND
};

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    if (obj = (Object *)DoSuperNew(cl,obj,
            MUIA_Textinput_MinVersion,   29,
            MUIA_Textinput_DefaultPopup, FALSE,
            TAG_MORE, (ULONG)msg->ops_AttrList))
    {
        struct data *data = INST_DATA(cl,obj);

        data->this = obj;

        if (GetTagData(MUIA_Disabled,FALSE,msg->ops_AttrList))
            data->flags |= FLG_Disabled;

        data->menu = MUI_MakeObject(MUIO_MenustripNM,(ULONG)tnMenu,0);

        if (data->menu) SetSuperAttrs(cl,obj,MUIA_ContextMenu,TRUE,TAG_DONE);
    }

    return (ULONG)obj;
}

/***********************************************************************/

static ULONG
mDispose(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (data->menu) MUI_DisposeObject(data->menu);

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static ULONG
mSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct data    *data = INST_DATA(cl,obj);
    struct TagItem *tag;

    if (tag = FindTagItem(MUIA_Disabled,msg->ops_AttrList))
        if (tag->ti_Data) data->flags |= FLG_Disabled;
        else data->flags &= ~FLG_Disabled;

    /* ?!?!?!?!?!? Texinput bug */
    if (tag = FindTagItem(MUIA_ContextMenu,msg->ops_AttrList))
        tag->ti_Tag = TAG_IGNORE;

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static ULONG
mContextMenuChoice(struct IClass *cl,Object *obj,struct MUIP_ContextMenuChoice *msg)
{
    switch (muiUserData(msg->item))
    {
        case MSG_CMenu_Cut:
            DoSuperMethod(cl,obj,MUIM_Textinput_DoCut);
            break;

        case MSG_CMenu_Copy:
            DoSuperMethod(cl,obj,MUIM_Textinput_DoCopy);
            break;

        case MSG_CMenu_Paste:
            DoSuperMethod(cl,obj,MUIM_Textinput_DoPaste);
            break;
    }

    return 0;
}

/***********************************************************************/

static ULONG
mContextMenuBuild(struct IClass *cl,Object *obj,struct MUIP_ContextMenuBuild *msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (data->flags & FLG_Disabled) return NULL;

    return (ULONG)data->menu;
}

/***********************************************************************/

M_DISP(dispatcher)
{
    M_DISPSTART

    switch(msg->MethodID)
    {
        case OM_NEW:                 return mNew(cl,obj,(APTR)msg);
        case OM_DISPOSE:             return mDispose(cl,obj,(APTR)msg);
        case OM_SET:                 return mSets(cl,obj,(APTR)msg);

        case MUIM_ContextMenuBuild:  return mContextMenuBuild(cl,obj,(APTR)msg);
        case MUIM_ContextMenuChoice: return mContextMenuChoice(cl,obj,(APTR)msg);

        default:                     return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(dispatcher)

/***********************************************************************/

ULONG
initRootStringClass(void)
{
    if (rootStringClass = MUI_CreateCustomClass(NULL,MUIC_Textinput,NULL,sizeof(struct data),DISP(dispatcher)))
    {
        localizeMenus(tnMenu);
        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/

void
disposeRootStringClass(void)
{
    MUI_DeleteCustomClass(rootStringClass);
}

/***********************************************************************/
