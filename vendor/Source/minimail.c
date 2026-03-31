
#include "rss.h"
#include <mui/MiniMailer_mcc.h>
#define CATCOMP_NUMBERS
#include "loc.h"

/***********************************************************************/

struct data
{
    Object *miniMail;
    STRPTR codesetName;
};

/***********************************************************************/

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    Object         *miniMail;
    struct TagItem *attrs = msg->ops_AttrList;

    if (obj = (Object *)DoSuperNew(cl,obj,
            MUIA_HelpNode,      (ULONG)"minimail.html",
            MUIA_Window_Title,  (ULONG)getString(MSG_MiniMail_Win_Title),
            MUIA_Window_ID,     MAKE_ID('c','m','m','m'),
            MUIA_Window_Height, MUIV_Window_Height_Scaled,

            WindowContents, (ULONG)(miniMail = MiniMailerObject,
                MUIA_MiniMailer_MinVer,       18,
                MUIA_MiniMailer_TranslateObj, obj,
            End),

            TAG_MORE,(ULONG)attrs))
    {
        struct data *data = INST_DATA(cl,obj);
        Object      *app;

        data->miniMail = miniMail;

        set(miniMail,MUIA_MiniMailer_TranslateObj,obj);

        DoMethod(miniMail,MUIM_MiniMailer_LoadPrefs,(ULONG)"PROGDIR:etc/opts/minimail.opts");

        app = (Object *)GetTagData(MUIA_RSS_App,NULL,attrs);
        DoSuperMethod(cl,obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,(ULONG)app,5,
            MUIM_Application_PushMethod,(ULONG)app,2,MUIM_App_DisposeWin,(ULONG)obj);
    }

    return (ULONG)obj;
}

/***********************************************************************/

static ULONG
mDispose(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    DoMethod(data->miniMail,MUIM_MiniMailer_SavePrefs,(ULONG)"PROGDIR:etc/opts/minimail.opts");

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static ULONG
mSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct data    *data = INST_DATA(cl,obj);
    struct TagItem *tags;
    struct TagItem *tag;

    for (tags = msg->ops_AttrList; tag = NextTagItem(&tags); )
    {
        ULONG tidata = tag->ti_Data;

        switch (tag->ti_Tag)
        {
            case MUIA_RSS_Prefs:
                data->codesetName = PREFS(tidata)->codeset;
                break;

            case MUIA_MiniMail_Addr:
                set(data->miniMail,MUIA_MiniMailer_To,tidata);
                break;
        }
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static ULONG
mTranslate(struct IClass *cl,Object *obj,struct MUIP_MiniMailer_Translate *msg)
{
    struct data *data = INST_DATA(cl,obj);
    STRPTR      codesetName;

    if (data->codesetName && *data->codesetName) codesetName = data->codesetName;
    else
    {
        struct codeset *codeset = CodesetsFindA(NULL,NULL);

        codesetName = codeset->name;
    }

    stccpy(msg->codesetName,codesetName,msg->codesetNameLen);

    return (ULONG)msg->text;
}

/***********************************************************************/

static ULONG
mFreeTranslated(struct IClass *cl,Object *obj,Msg msg)
{
    return 0;
}

/***********************************************************************/

M_DISP(dispatcher)
{
    M_DISPSTART

    switch(msg->MethodID)
    {
        case OM_NEW:                         return mNew(cl,obj,(APTR)msg);
        case OM_DISPOSE:                     return mDispose(cl,obj,(APTR)msg);
        case OM_SET:                         return mSets(cl,obj,(APTR)msg);
        case MUIM_MiniMailer_Translate:      return mTranslate(cl,obj,(APTR)msg);
        case MUIM_MiniMailer_FreeTranslated: return mFreeTranslated(cl,obj,(APTR)msg);
        default:                             return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(dispatcher)

/***********************************************************************/

ULONG
initMiniMailClass(void)
{
    return (ULONG)(miniMailClass = MUI_CreateCustomClass(NULL,NULL,winClass,sizeof(struct data),DISP(dispatcher)));
}

/***********************************************************************/

void
disposeMiniMailClass(void)
{
    MUI_DeleteCustomClass(miniMailClass);
}

/***********************************************************************/
