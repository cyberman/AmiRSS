
#include "rss.h"
#include <mui/BWin_mcc.h>

/***********************************************************************/

struct data
{
    Object *root;
    ULONG  flags;
};

enum
{
    FLG_Forward = 1<<0,
};

/***********************************************************************/

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct TagItem *attrs = msg->ops_AttrList;
    Object         *root = (Object *)GetTagData(WindowContents,NULL,attrs);

    if (!root) return 0;

    if (obj = (Object *)DoSuperMethodA(cl,obj,(Msg)msg))
    {
        struct data *data = INST_DATA(cl,obj);

        data->root  = root;
        data->flags = GetTagData(MUIA_Win_Forward,FALSE,attrs) ? FLG_Forward : 0;
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
                if (data->flags & FLG_Forward)
                    SetAttrs(data->root,
                        MUIA_RSS_Prefs,  tidata,
                        MUIA_RSS_OneWay, GetTagData(MUIA_RSS_OneWay,FALSE,msg->ops_AttrList),
                        TAG_DONE);
                break;

            case MUIA_Window_Activate:
            {
                struct Window *win = NULL;

                if (!tidata && get(obj,MUIA_Window_Window,&win) && win)
                    SetWindowPointerA(win,NULL);

                break;
            }
        }
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

M_DISP(dispatcher)
{
    M_DISPSTART

    switch(msg->MethodID)
    {
        case OM_NEW: return mNew(cl,obj,(APTR)msg);
        case OM_SET: return mSets(cl,obj,(APTR)msg);

        default:     return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(dispatcher)

/***********************************************************************/

ULONG
initWinClass(void)
{
    return (ULONG)(winClass = MUI_CreateCustomClass(NULL,MUIC_Window,NULL,sizeof(struct data),DISP(dispatcher)));
}

/***********************************************************************/

void
disposeWinClass(void)
{
    MUI_DeleteCustomClass(winClass);
}

/***********************************************************************/
