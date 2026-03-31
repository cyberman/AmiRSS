
#include "rss.h"

/***********************************************************************/

struct data
{
    struct MUI_EventHandlerNode eh;

    Object                      *horiz;
    Object                      *vert;
    Object                      *last;
    struct Window               *win;

    ULONG                       flags;
};

/***********************************************************************/

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    if (obj = (Object *)DoSuperMethodA(cl,obj,(Msg)msg))
    {
        struct data *data = INST_DATA(cl,obj);

        data->flags = 0;
    }

    return (ULONG)obj;
}

/***********************************************************************/

static ULONG
mShow(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    if (MUIMasterBase->lib_Version<20)
    {
        memset(&data->eh,0,sizeof(data->eh));
        data->eh.ehn_Class  = cl;
        data->eh.ehn_Object = obj;
        data->eh.ehn_Events = IDCMP_RAWKEY;
        DoMethod(_win(obj),MUIM_Window_AddEventHandler,&data->eh);

        get(_win(obj),MUIA_Window_Window,&data->win);
    }

    return TRUE;
}

/***********************************************************************/

static ULONG
mHide(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (MUIMasterBase->lib_Version<20)
        DoMethod(_win(obj),MUIM_Window_RemEventHandler,&data->eh);

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static ULONG
mHandleEvent(struct IClass *cl,Object *obj,struct MUIP_HandleEvent *msg)
{
    struct data *data = INST_DATA(cl,obj);
    Object      *o;

    get(_win(obj),MUIA_Window_ActiveObject,&o);
    if (!o || o==obj)
    {
        WORD  mx, my;
        ULONG overobj;

        mx = msg->imsg->MouseX;
        my = msg->imsg->MouseY;

        overobj = _left(obj)<mx && _right(obj)>mx && _top(obj)<my && _bottom(obj)>my;

        if (overobj)
        {
            if (msg->imsg->Class==IDCMP_RAWKEY)
            {
                LONG delta = 0;

                if (msg->imsg->Code==NM_WHEEL_UP) delta = -8;
                else if (msg->imsg->Code==NM_WHEEL_DOWN) delta = 8;

                if (delta)
                {
                    ULONG f;

                    superset(cl,obj,MUIA_NList_First,(delta<0) ? MUIV_NList_First_PageUp : MUIV_NList_First_PageDown);

                    /*if (data->horiz || data->vert)
                    {
                        o = NULL;

                        if (data->horiz)
                        {
                            if (_left(data->horiz)<mx && _right(data->horiz)>mx && _top(data->horiz)<my && _bottom(data->horiz)>my)
                                o = data->horiz;
                        }

                        if (!o)
                            if (data->vert)
                            {
                                if (_left(data->vert)<mx && _right(data->vert)>mx && _top(data->vert)<my && _bottom(data->vert)>my)
                                    o = data->vert;
                            }

                        if (!o) o = data->vert ? data->vert : data->horiz;

                        data->last = o;

                        DoMethod(o,MUIM_Prop_Increase,delta);

                        return MUI_EventHandlerRC_Eat;
                    }*/
                }
            }
        }
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static SAVEDS ASM ULONG
dispatcher(REG(a0,struct IClass *cl),REG(a2,Object *obj),REG(a1,Msg msg))
{
    switch(msg->MethodID)
    {
        case OM_NEW:           return mNew(cl,obj,(APTR)msg);

        case MUIM_Show:        return mShow(cl,obj,(APTR)msg);
        case MUIM_Hide:        return mHide(cl,obj,(APTR)msg);
        case MUIM_HandleEvent: return mHandleEvent(cl,obj,(APTR)msg);

        default:               return DoSuperMethodA(cl,obj,msg);
    }
}

/***********************************************************************/

ULONG
initRootListClass(void)
{
    return (ULONG)(rootListClass = MUI_CreateCustomClass(NULL,MUIC_NListview,NULL,sizeof(struct data),dispatcher));
}

/***********************************************************************/

void
disposeRootListClass(void)
{
    MUI_DeleteCustomClass(rootListClass);
}

/***********************************************************************/
