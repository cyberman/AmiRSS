
#include "rss.h"

/***********************************************************************/

struct data
{
    struct MUI_EventHandlerNode eh;

    Object                      *vg;
    Object                      *horiz;
    Object                      *vert;
    Object                      *last;
    struct Window               *win;

    ULONG                       vdelta;
    ULONG                       hdelta;
    ULONG                       lastDelta;

    ULONG                       flags;
};

enum
{
    FLG_UseWinBorder = 1<<0,
};

/***********************************************************************/

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    if (obj = (Object *)DoSuperMethodA(cl,obj,(Msg)msg))
    {
        register struct data *data = INST_DATA(cl,obj);

        get(obj,MUIA_Scrollgroup_Contents,&data->vg);
        get(obj,MUIA_Scrollgroup_HorizBar,&data->horiz);
        get(obj,MUIA_Scrollgroup_VertBar,&data->vert);

        if (GetTagData(MUIA_Scrollgroup_UseWinBorder,FALSE,msg->ops_AttrList))
            data->flags |= FLG_UseWinBorder;

        data->vdelta = data->hdelta = data->lastDelta = 8;
    }

    return (ULONG)obj;
}

/***********************************************************************/

static ULONG
mShow(struct IClass *cl,Object *obj,Msg msg)
{
    register struct data *data = INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    if (MUIMasterBase->lib_Version<20)
    {
        memset(&data->eh,0,sizeof(data->eh));
        data->eh.ehn_Class  = cl;
        data->eh.ehn_Object = obj;
        data->eh.ehn_Events = IDCMP_RAWKEY;
        DoMethod(_win(obj),MUIM_Window_AddEventHandler,(ULONG)&data->eh);

        if (data->flags & FLG_UseWinBorder)
            get(_win(obj),MUIA_Window_Window,&data->win);
    }

    return TRUE;
}

/***********************************************************************/

static ULONG
mHide(struct IClass *cl,Object *obj,Msg msg)
{
    register struct data *data = INST_DATA(cl,obj);

    if (MUIMasterBase->lib_Version<20)
        DoMethod(_win(obj),MUIM_Window_RemEventHandler,(ULONG)&data->eh);

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static ULONG
mSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    register struct data    *data = INST_DATA(cl,obj);
    register struct TagItem *tag;
    struct TagItem          *tstate;

    for (tstate = msg->ops_AttrList; tag = NextTagItem(&tstate); )
    {
        register ULONG tidata = tag->ti_Data;

        switch(tag->ti_Tag)
        {
            case MUIA_RootScrollGroup_HDeltaFactor:
                data->hdelta = tidata;
                break;

            case MUIA_RootScrollGroup_VDeltaFactor:
                data->vdelta = tidata;
                break;
        }
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static ULONG
mHandleEvent(struct IClass *cl,Object *obj,struct MUIP_HandleEvent *msg)
{
    register struct data *data = INST_DATA(cl,obj);
    Object               *o;

    /*get(_win(obj),MUIA_Window_ActiveObject,&o);
    if (!o || o==obj || o==data->vg)*/
    {
        register ULONG overobj, overwin;
        register WORD  mx, my;

        mx = msg->imsg->MouseX;
        my = msg->imsg->MouseY;

        overobj = _left(obj)<mx && _right(obj)>mx && _top(obj)<my && _bottom(obj)>my;

        if (data->flags & FLG_UseWinBorder) overwin = mx>=0 && mx<=data->win->Width && my>=0 && my<=data->win->Height;
        else overwin = FALSE;

        if (overobj || overwin)
        {
            if (msg->imsg->Class==IDCMP_RAWKEY)
            {
                register UWORD code = msg->imsg->Code;

                if (code==NM_WHEEL_UP || code==NM_WHEEL_DOWN)
                {
                    struct List    *l;
                    Object         *mstate;
                    register ULONG delta = 0; // gcc

                    for (get(data->vg,MUIA_Group_ChildList,&l), mstate = (Object *)l->lh_Head; o = NextObject(&mstate);)
                    {
                        ULONG res;

                        res = DoMethodA(o,(Msg)msg);
                        if (res & MUI_EventHandlerRC_Eat) return res;
                    }

                    if (data->horiz || data->vert)
                    {
                        o = NULL;

                        if (data->horiz)
                        {
                            if (overwin)
                            {
                                    WORD bh = -(data->win->BorderBottom-data->win->BorderTop);

                                    if (mx>=0 && mx<=data->win->Width && my>=data->win->Height-bh && my<=data->win->Height)
                                        o = data->horiz;
                            }
                            else
                            {
                                if (_left(data->horiz)<mx && _right(data->horiz)>mx && _top(data->horiz)<my && _bottom(data->horiz)>my)
                                    o = data->horiz;
                            }

                            if (o) delta = data->hdelta;
                        }

                        if (!o)
                            if (data->vert)
                            {
                                if (overwin)
                                {
                                    WORD bw = data->win->BorderRight-data->win->BorderLeft;

                                    if (mx>=data->win->Width-bw && mx<=data->win->Width && my>=0  && my<=data->win->Height)
                                        o = data->vert;
                                }
                                else
                                {
                                    if (_left(data->vert)<mx && _right(data->vert)>mx && _top(data->vert)<my && _bottom(data->vert)>my)
                                        o = data->vert;
                                }

                                if (o) delta = data->vdelta;
                            }

                        if (!o)
                        {
                            o = data->last;
                            delta = data->lastDelta;
                        }

                        if (!o)
                        {
                            if (data->vert)
                            {
                                o = data->vert;
                                delta = data->vdelta;
                            }
                            else
                            {
                                if (data->horiz)
                                {
                                    o = data->horiz;
                                    delta = data->hdelta;
                                }
                            }
                        }

                        data->last      = o;
                        data->lastDelta = delta;

                        if (o)
                        {
                            if (code==NM_WHEEL_UP) delta = -delta;
                            DoMethod(o,MUIM_Prop_Increase,delta);
                        }

                        return MUI_EventHandlerRC_Eat;
                    }
                }
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
        case OM_NEW:           return mNew(cl,obj,(APTR)msg);
        case OM_SET:           return mSets(cl,obj,(APTR)msg);

        case MUIM_Show:        return mShow(cl,obj,(APTR)msg);
        case MUIM_Hide:        return mHide(cl,obj,(APTR)msg);
        case MUIM_HandleEvent: return mHandleEvent(cl,obj,(APTR)msg);

        default:               return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(dispatcher)

/***********************************************************************/

ULONG
initRootScrollgroupClass(void)
{
    return (ULONG)(rootScrollgroupClass = MUI_CreateCustomClass(NULL,MUIC_Scrollgroup,NULL,sizeof(struct data),DISP(dispatcher)));
}

/***********************************************************************/

void
disposeRootScrollgroupClass(void)
{
    MUI_DeleteCustomClass(rootScrollgroupClass);
}

/***********************************************************************/

