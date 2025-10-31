
#include "rss.h"
#define CATCOMP_NUMBERS
#include "loc.h"

/***********************************************************************/

struct data
{
    UBYTE buf[32];
};

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    return (ULONG)DoSuperNew(cl,obj,
            MUIA_Numeric_Min, -24,
            MUIA_Numeric_Max,  24,
            TAG_MORE, (ULONG)msg->ops_AttrList);
}

/***********************************************************************/

static ULONG
mStringify(struct IClass *cl,Object *obj,struct MUIP_Numeric_Stringify *msg)
{
    struct data *data = INST_DATA(cl,obj);
    ULONG       s, m, h;

    if (msg->value==0) return (ULONG)"GMT";

    if (msg->value<0)
    {
        s = TRUE;
        m = -msg->value*30;
    }
    else
    {
        s = FALSE;
        m = msg->value*30;
    }

    h = m/60;
    m = m%60;

    msprintf(data->buf,"\33c %lc%02ld:%02ld ",s ? '-' : '+',h,m);

    return (ULONG)data->buf;
}

/***********************************************************************/

M_DISP(dispatcher)
{
    M_DISPSTART

    switch (msg->MethodID)
    {
        case OM_NEW:                 return mNew(cl,obj,(APTR)msg);

        case MUIM_Numeric_Stringify: return mStringify(cl,obj,(APTR)msg);

        default:                     return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(dispatcher)

/***********************************************************************/

ULONG
initGMTClass(void)
{
    if (GMTClass = MUI_CreateCustomClass(NULL,MUIC_Slider,NULL,sizeof(struct data),DISP(dispatcher)))
    {
        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/

void
disposeGMTClass(void)
{
    MUI_DeleteCustomClass(GMTClass);
}

/***********************************************************************/
