
#include "rss.h"
#define CATCOMP_NUMBERS
#include "loc.h"

/***********************************************************************/

static struct MUI_CustomClass *updateClass;
#define updateObject NewObject(updateClass->mcc_Class,NULL

/***********************************************************************/

struct updateData
{
    UBYTE buf[32];
};

static ULONG
mUpdateNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    return (ULONG)DoSuperNew(cl,obj,
            MUIA_Numeric_Min,  0,
            MUIA_Numeric_Max,  18,
            MUIA_ControlChar,  getKeyCharID(MSG_EditFeed_Update),
            MUIA_ShortHelp,    (ULONG)getString(MSG_EditFeed_UpdateHelp),
            TAG_MORE, (ULONG)msg->ops_AttrList);
}

/***********************************************************************/

static ULONG
mUpdateStringify(struct IClass *cl,Object *obj,struct MUIP_Numeric_Stringify *msg)
{
    struct updateData *data = INST_DATA(cl,obj);
    ULONG             v = 0, id; //gcc

    if (msg->value==0) id = MSG_EditFeed_Update_Never;
    else
    {
        v = msg->value*10;
        id = MSG_EditFeed_Update_Min;
    }

    msprintf(data->buf,getString(id),v);

    return (ULONG)data->buf;
}

/***********************************************************************/

M_DISP(updateDispatcher)
{
    M_DISPSTART

    switch (msg->MethodID)
    {
        case OM_NEW:                 return mUpdateNew(cl,obj,(APTR)msg);
        case MUIM_Numeric_Stringify: return mUpdateStringify(cl,obj,(APTR)msg);
        default:                     return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(updateDispatcher)

/***********************************************************************/

static ULONG
initUpdateClass(void)
{
    return (ULONG)(updateClass = MUI_CreateCustomClass(NULL,MUIC_Slider,NULL,sizeof(struct updateData),DISP(updateDispatcher)));
}

/***********************************************************************/

static void
disposeUpdateClass(void)
{
    MUI_DeleteCustomClass(updateClass);
}

/***********************************************************************/

struct data
{
    struct MUI_NListtree_TreeNode *list;
    struct MUI_NListtree_TreeNode *node;
    struct feed                   *feed;

    Object                        *listObj;
    Object                        *name;
    Object                        *pic;
    Object                        *URL;
    Object                        *update;
    Object                        *goURL;
    Object                        *showLink;
    Object                        *ok;

    ULONG                         flags;
};

enum
{
    FLG_Group = 1<<0,
};

/***********************************************************************/

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct data                   temp;
    struct TagItem                *attrs = msg->ops_AttrList;
    struct MUI_NListtree_TreeNode *node;
    struct feed                   *feed;
    Object                        *file = NULL , *cancel; //gcc
    ULONG                         flags, id;

    memset(&temp,0,sizeof(temp));

    temp.node = node  = (struct MUI_NListtree_TreeNode *)GetTagData(MUIA_EditFeed_Node,NULL,attrs);
    temp.feed = feed  = (struct feed *)GetTagData(MUIA_EditFeed_Feed,NULL,attrs);
    flags = (feed->flags & FFLG_Group) ? FLG_Group : 0;

    id = node ? ((flags & FLG_Group) ? MSG_EditFeed_WinTitleEditGroup : MSG_EditFeed_WinTitleEditFeed) :
                ((flags & FLG_Group) ? MSG_EditFeed_WinTitleNewGroup  : MSG_EditFeed_WinTitleNewFeed);

    if (obj = (Object *)DoSuperNew(cl,obj,
        MUIA_Window_Title, (ULONG)getString(id),
        MUIA_HelpNode,     (ULONG)"editfeed.html",
        MUIA_Window_Width, MUIV_Window_Width_MinMax(30),

        WindowContents, (ULONG)(VGroup,
            Child, VGroup,
                GroupFrame,
                MUIA_Background, MUII_GroupBack,

                Child, ColGroup(2),

                    (flags & FLG_Group) ? TAG_IGNORE : Child, (flags & FLG_Group) ? 0 : olabel2(MSG_EditFeed_URL),
                    (flags & FLG_Group) ? TAG_IGNORE : Child, (flags & FLG_Group) ? 0 : (HGroup,
                        MUIA_Group_Spacing, 1,
                        Child, temp.URL = oaestring(DEF_URLSIZE,MSG_EditFeed_URL,MSG_EditFeed_URLHelp),
                        Child, temp.goURL = MUI_MakeObject(MUIO_PopButton,MUII_PopUp),
                        End),

                    Child, olabel2(MSG_EditFeed_Name),
                    Child, temp.name = ostring(DEF_NAMESIZE,MSG_EditFeed_Name,MSG_EditFeed_NameHelp),

                    Child, olabel2(MSG_EditFeed_Pic),
                    Child, ofile(&temp.pic,DEF_FILESIZE,MSG_EditFeed_Pic,MSG_EditFeed_PicHelp,MSG_EditFeed_PicReqTitle,FALSE),

                    (flags & FLG_Group) ? TAG_IGNORE : Child, (flags & FLG_Group) ? 0 : olabel2(MSG_EditFeed_Update),
                    (flags & FLG_Group) ? TAG_IGNORE : Child, (flags & FLG_Group) ? 0 : (temp.update = updateObject,End),

                    ((flags & FLG_Group) || !feed->file) ? TAG_IGNORE : Child, (flags & FLG_Group) ? 0 : olabel2(MSG_EditFeed_File),
                    ((flags & FLG_Group) || !feed->file) ? TAG_IGNORE : Child, (flags & FLG_Group) ? 0 : (file = TextObject,MUIA_Frame,MUIV_Frame_Text, MUIA_Background, MUII_TextBack, End),

                End,

                (flags & FLG_Group) ? TAG_IGNORE : Child, (flags & FLG_Group) ? 0 : (rootScrollgroupObject,
                    MUIA_Scrollgroup_FreeHoriz, FALSE,
                    MUIA_Scrollgroup_Contents, VirtgroupObject,
                        MUIA_Frame, MUIV_Frame_Virtual,

                        Child, HGroup,
                            Child, ColGroup(2),
                                Child, temp.showLink = ocheck(MSG_EditFeed_ShowLink,MSG_EditFeed_ShowLinkHelp),
                                Child, ollabel1(MSG_EditFeed_ShowLink),
                            End,
                            Child, HSpace(0),
                        End,
                        Child, VSpace(0),
                    End,
                End),
            End,

            Child, HGroup,
                Child, temp.ok = obutton(Msg_EditFeed_OK,Msg_EditFeed_OKHelp),
                Child, wspace(300),
                Child, cancel = obutton(Msg_EditFeed_Cancel,Msg_EditFeed_CancelHelp),
            End,
        End),
        TAG_MORE,(ULONG)attrs))
    {
        struct data *data = INST_DATA(cl,obj);

        *data = temp;
        data->flags = flags;

        data->list    = (struct MUI_NListtree_TreeNode *)GetTagData(MUIA_EditFeed_List,NULL,attrs);
        data->listObj = (Object *)GetTagData(MUIA_EditFeed_ListObj,NULL,attrs);

        if (feed->name) set(data->name,MUIA_Textinput_Contents,feed->name);
        if (feed->pic) set(data->pic,MUIA_Textinput_Contents,feed->pic);

        if (!(flags & FLG_Group))
        {
            if (feed->URL)  set(data->URL,MUIA_Textinput_Contents,feed->URL);
            if (feed->file) set(file,MUIA_Text_Contents,feed->file);
            set(data->showLink,MUIA_Selected,feed->flags & FFLG_ShowLink);
            set(data->update,MUIA_Numeric_Value,feed->updateMin/10);
        }

        superset(cl,obj,MUIA_Window_ActiveObject,(flags & FLG_Group) ? data->name : data->URL);

        DoMethod(obj,MUIM_EditFeed_EditChange);

        DoSuperMethod(cl,obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,(ULONG)obj,2,MUIM_EditFeed_Close,FALSE);

        DoMethod(data->name,MUIM_Notify,MUIA_Textinput_Contents,MUIV_EveryTime,(ULONG)obj,1,MUIM_EditFeed_EditChange);

        if (!(flags & FLG_Group))
        {
            DoMethod(data->URL,MUIM_Notify,MUIA_Textinput_Contents,MUIV_EveryTime,(ULONG)obj,1,MUIM_EditFeed_EditChange);
            DoMethod(data->goURL,MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)obj,1,MUIM_EditFeed_GoURL);
        }

        DoMethod(data->ok,MUIM_Notify,MUIA_Pressed,0,(ULONG)obj,2,MUIM_EditFeed_Close,TRUE);
        DoMethod(cancel,MUIM_Notify,MUIA_Pressed,0,(ULONG)obj,2,MUIM_EditFeed_Close,FALSE);
    }

    return (ULONG)obj;
}

/***********************************************************************/

static ULONG
mClose(struct IClass *cl,Object *obj,struct MUIP_EditFeed_Close *msg)
{
    struct data *data = INST_DATA(cl,obj);
    UBYTE       buf[DEF_URLSIZE];
    STRPTR      URLPtr = NULL; //gcc
    STRPTR      name = NULL, pic = NULL;
    ULONG       flags = 0, mode, update = 0, freeURL = FALSE; // gcc

    SetSuperAttrs(cl,obj,MUIA_Window_Open,FALSE,TAG_DONE);

    if (mode = msg->mode)
    {
        get(data->name,MUIA_Textinput_Contents,&name);
        get(data->pic,MUIA_Textinput_Contents,&pic);

        if (!(data->flags & FLG_Group))
        {
            STRPTR URL = NULL;

            get(data->URL,MUIA_Textinput_Contents,&URL);

            if (strnicmp(URL,"http://",7))
            {
                ULONG l = strlen(URL)+8;

                if (l>sizeof(buf))
                {
                    if (URLPtr = allocArbitrateVecPooled(l)) freeURL = TRUE;
                }
                else URLPtr = buf;

                if (URLPtr) msprintf(URLPtr,"http://%s",(ULONG)URL);
                else mode = FALSE;
            }
            else URLPtr = URL;

            update = xget(data->update,MUIA_Numeric_Value)*10;
            flags = xget(data->showLink,MUIA_Selected) ? MUIV_GroupList_UpdateFeed_ShowLink : 0;
        }
    }

    DoMethod(data->listObj,MUIM_GroupList_UpdateFeed,mode,(ULONG)data->list,(ULONG)data->node,(ULONG)data->feed,(ULONG)name,(ULONG)URLPtr,(ULONG)pic,update,flags);
    if (freeURL) freeArbitrateVecPooled(URLPtr);
    DoMethod(_app(obj),MUIM_Application_PushMethod,(ULONG)_app(obj),2,MUIM_App_DisposeWin,(ULONG)obj);

    return 0;
}

/***********************************************************************/

static ULONG
mEditChange(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);
    STRPTR      name, URL;

    get(data->name,MUIA_Textinput_Contents,&name);

    if (data->flags & FLG_Group) URL = " ";
    else get(data->URL,MUIA_Textinput_Contents,&URL);

    set(data->ok,MUIA_Disabled,!*name || !*URL);

    if (!(data->flags & FLG_Group))
        set(data->goURL,MUIA_Disabled,!*URL);

    return 0;
}

/***********************************************************************/

static ULONG
mGoURL(struct IClass *cl,Object *obj,struct MUIP_HTML_GoURL *msg)
{
    struct data *data = INST_DATA(cl,obj);
    STRPTR      URL;

    get(data->URL,MUIA_Textinput_Contents,&URL);
    return goURLFun(URL);
}

/***********************************************************************/

M_DISP(dispatcher)
{
    M_DISPSTART

    switch (msg->MethodID)
    {
        case OM_NEW:                   return mNew(cl,obj,(APTR)msg);
        case MUIM_EditFeed_Close:      return mClose(cl,obj,(APTR)msg);
        case MUIM_EditFeed_EditChange: return mEditChange(cl,obj,(APTR)msg);
        case MUIM_EditFeed_GoURL:      return mGoURL(cl,obj,(APTR)msg);
        default:                       return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(dispatcher)

/***********************************************************************/

ULONG
initEditFeedClass(void)
{
    if (initUpdateClass())
    {
        if (editFeedClass = MUI_CreateCustomClass(NULL,NULL,winClass,sizeof(struct data),DISP(dispatcher)))
            return TRUE;

        disposeUpdateClass();
    }

    return FALSE;
}

/***********************************************************************/

void
disposeEditFeedClass(void)
{
    disposeUpdateClass();
    MUI_DeleteCustomClass(editFeedClass);
}

/***********************************************************************/
