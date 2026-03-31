
#include "rss.h"
#include <mui/CManager_mcc.h>
#include <libraries/CManager.h>
#define CATCOMP_NUMBERS
#include "loc.h"

/***********************************************************************/

struct data
{
    Object *app;
    Object *cm;

    ULONG  flags;
};

/***********************************************************************/

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    Object *cm;

    if (obj = (Object *)DoSuperNew(cl,obj,
            MUIA_Window_Title, (ULONG)getString(MSG_CM_Win_Title),
            MUIA_Window_ID,    MAKE_ID('W','C','M','A'),
            MUIA_HelpNode,     (ULONG)"CM.html",

            WindowContents, VGroup,
                Child, cm = CManagerObject,
                   MUIA_CManager_HideChat,       TRUE,
                   MUIA_CManager_HideTelnet,     TRUE,
                   MUIA_CManager_HideFTP,        TRUE,
                   MUIA_CManager_DisableCard,    TRUE,
                   MUIA_CManager_DisableQuick,   TRUE,
                   MUIA_CManager_NoGroupsCtrl,   TRUE,
                End,
            End,
            TAG_MORE,(ULONG)msg->ops_AttrList))
    {
        struct data *data = INST_DATA(cl,obj);

        data->cm = cm;
        set(cm,MUIA_CManager_DoubleClickObject,obj);

        data->app = (Object *)GetTagData(MUIA_RSS_App,NULL,msg->ops_AttrList);

        DoSuperMethod(cl,obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,(ULONG)data->app,5,
            MUIM_Application_PushMethod,(ULONG)data->app,2,MUIM_App_DisposeWin,(ULONG)obj);

        DoMethod(cm,MUIM_CManager_LoadData,NULL,NULL);
    }

    return (ULONG)obj;
}

/***********************************************************************/

static ULONG
mDoubleClick(struct IClass *cl,Object *obj,struct MUIP_CManager_DoubleClick *msg)
{
    struct data *data = INST_DATA(cl,obj);
    STRPTR      URL;

    switch (msg->entry->Type)
    {
        case CMEntry_User:
            URL = CMUSER(msg->entry)->WWW;
            break;

        case CMEntry_WWW:
            URL = CMWWW(msg->entry)->WWW;
            break;

        default:
            URL = "";
            break;
    }

    if (*URL) DoMethod(data->app,MUIM_RSS_GotoURL,(ULONG)URL,MUIV_RSS_GotoURL_IgnorePrefs|MUIV_RSS_GotoURL_NoTarget);

    return 0;
}

/***********************************************************************/

M_DISP(dispatcher)
{
    M_DISPSTART

    switch (msg->MethodID)
    {
        case OM_NEW:                    return mNew(cl,obj,(APTR)msg);
        case MUIM_CManager_DoubleClick: return mDoubleClick(cl,obj,(APTR)msg);
        default:                        return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(dispatcher)

/***********************************************************************/

ULONG
initCMClass(void)
{
    return (ULONG)(CMClass = MUI_CreateCustomClass(NULL,NULL,winClass,sizeof(struct data),DISP(dispatcher)));
}

/***********************************************************************/

void
disposeCMClass(void)
{
    MUI_DeleteCustomClass(CMClass);
}

/***********************************************************************/
