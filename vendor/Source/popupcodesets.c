
#include "rss.h"

/***********************************************************************/

#ifdef __MORPHOS__
static void
windowFun(void)
{
    //struct Hook *hook = (struct Hook *)REG_A0;
    Object      *pop = (Object *)REG_A2;
    Object      *win = (Object *)REG_A1;
#else
static void SAVEDS ASM
windowFun(REG(a0,struct Hook *hook),REG(a2,Object *pop),REG(a1,Object *win))
{
#endif
    set(win,MUIA_Window_DefaultObject,pop);
}

#ifdef __MORPHOS__
static struct EmulLibEntry windowTrap = {TRAP_LIB,0,(void (*)(void))windowFun};
static struct Hook windowHook = {0,0,(HOOKFUNC)&windowTrap};
#else
static struct Hook windowHook = {0,0,(HOOKFUNC)&windowFun};
#endif

/***********************************************************************/

#ifdef __MORPHOS__
static ULONG
openFun(void)
{
    //struct Hook *hook = (struct Hook *)REG_A0;
    Object      *list = (Object *)REG_A2;
    Object      *str = (Object *)REG_A1;
#else
static ULONG SAVEDS ASM
openFun(REG(a0,struct Hook *hook),REG(a2,Object *list),REG(a1,Object *str))
{
#endif
    STRPTR s;
    STRPTR x;
    int    i;

    s = (STRPTR)xget(str,MUIA_Textinput_Contents);

    for (i = 0; ;i++)
    {
        DoMethod(list,MUIM_List_GetEntry,i,(ULONG)&x);
        if (!x)
        {
            set(list,MUIA_List_Active,MUIV_List_Active_Off);
            break;
        }
        else
            if (!stricmp(x,s))
            {
                set(list,MUIA_List_Active,i);
                break;
            }
    }

    return TRUE;
}

#ifdef __MORPHOS__
static struct EmulLibEntry openTrap = {TRAP_LIB,0,(void (*)(void))openFun};
static struct Hook openHook = {0,0,(HOOKFUNC)&openTrap};
#else
static struct Hook openHook = {0,0,(HOOKFUNC)&openFun};
#endif

/***********************************************************************/

#ifdef __MORPHOS__
void closeFun(void)
{
    //struct Hook *hook = (struct Hook *)REG_A0;
    Object      *list = (Object *)REG_A2;
    Object      *str = (Object *)REG_A1;
#else
void SAVEDS ASM
closeFun(REG(a0,struct Hook *hook),REG(a2,Object *list),REG(a1,Object *str))
{
#endif
    STRPTR e;

    DoMethod(list,MUIM_List_GetEntry,MUIV_List_GetEntry_Active,(ULONG)&e);
    set(str,MUIA_Textinput_Contents,e);
    set(str,MUIA_String_Acknowledge,TRUE);
}

#ifdef __MORPHOS__
static struct EmulLibEntry closeTrap = {TRAP_LIB,0,(void (*)(void))closeFun};
static struct Hook closeHook = {0,0,(HOOKFUNC)&closeTrap};
#else
static struct Hook closeHook = {0,0,(HOOKFUNC)&closeFun};
#endif

/***********************************************************************/

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    Object *str, *bt, *lv, *l;

    if (obj = (Object *)DoSuperNew(cl,obj,

            MUIA_Popstring_String, (ULONG)(str = ostring(DEF_CODESETSIZE,GetTagData(MUIA_RSS_Key,0,msg->ops_AttrList),0)),
            MUIA_Popstring_Button, (ULONG)(bt = MUI_MakeObject(MUIO_PopButton,MUII_PopUp)),

            MUIA_Popobject_Object, (ULONG)(lv = ListviewObject,
                MUIA_Listview_List, l = ListObject,
                    MUIA_Frame,              MUIV_Frame_InputList,
                    MUIA_Background,         MUII_ListBack,
                    MUIA_List_AutoVisible,   TRUE,
                    MUIA_List_ConstructHook, MUIV_List_ConstructHook_String,
                    MUIA_List_DestructHook,  MUIV_List_DestructHook_String,
                End,
            End),
            MUIA_Popobject_WindowHook, (ULONG)&windowHook,
            MUIA_Popobject_StrObjHook, (ULONG)&openHook,
            MUIA_Popobject_ObjStrHook, (ULONG)&closeHook,


            TAG_MORE,(ULONG)msg->ops_AttrList))
    {
        STRPTR *array;

        set(bt,MUIA_CycleChain,TRUE);
        DoMethod(lv,MUIM_Notify,MUIA_Listview_DoubleClick,TRUE,(ULONG)obj,2,MUIM_Popstring_Close,TRUE);
        DoMethod(str,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,(ULONG)obj,3,MUIM_Set,MUIA_Popupcodesets_Acknowledge,TRUE);

        if (array = CodesetsSupportedA(NULL))
        {
            DoMethod(l,MUIM_List_Insert,(ULONG)array,-1,MUIV_List_Insert_Sorted);
            CodesetsFreeA(array,NULL);
        }
        else superset(cl,obj,MUIA_Disabled,TRUE);
    }

    return (ULONG)obj;
}

/***********************************************************************/

static ULONG
mGet(struct IClass *cl,Object *obj,struct opGet *msg)
{
    switch(msg->opg_AttrID)
    {
        case MUIA_Popupcodesets_Acknowledge: *msg->opg_Storage = TRUE; return TRUE;
        default: return DoSuperMethodA(cl,obj,(Msg)msg);
    }
}

/***********************************************************************/

M_DISP(dispatcher)
{
    M_DISPSTART

    switch (msg->MethodID)
    {
        case OM_NEW: return mNew(cl,obj,(APTR)msg);
        case OM_GET: return mGet(cl,obj,(APTR)msg);
        default:     return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(dispatcher)

/***********************************************************************/

ULONG
initPopupCodesetsClass(void)
{
    return (ULONG)(popupCodesetsClass = MUI_CreateCustomClass(NULL,MUIC_Popobject,NULL,0,DISP(dispatcher)));
}

/***********************************************************************/

void
disposePopupCodesetsClass(void)
{
    MUI_DeleteCustomClass(popupCodesetsClass);
}

/***********************************************************************/
