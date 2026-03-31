
#include "rss.h"
#include <proto/iffparse.h>
#include <datatypes/textclass.h>
#define CATCOMP_NUMBERS
#include "loc.h"

/***********************************************************************/

struct data
{
    UBYTE          bday[32];
    UBYTE          bdate[32];
    UBYTE          btime[32];
    UBYTE          date[128];

    Object         *app;
    Object         *win;

    APTR           cmenu;

    struct entry   *active;

    ULONG          dispCol;
    ULONG          flags;

    ULONG          doubleClick;
    ULONG          format;

    ULONG          sortCol;
    ULONG          revSort;

    UBYTE          titleBuf0[64];
    UBYTE          titleBuf1[64];
    UBYTE          titleBuf2[64];

};

enum
{
    FLG_Disposing  = 1<<0,
    FLG_RightMouse = 1<<1,
    FLG_NoListBar  = 1<<2,
    FLG_BoldTitles = 1<<3,
    FLG_SubstDate  = 1<<4,
};

/***********************************************************************/

void
freeNew(APTR pool,struct entry *entry)
{
    if (entry->author) freeVecPooled(pool,entry->author);
    if (!(entry->flags & EFLG_NtFreeTitle) && entry->title) freeVecPooled(pool,entry->title);
    if (entry->descr) freeVecPooled(pool,entry->descr);
    if (entry->link) freeVecPooled(pool,entry->link);

    FreePooled(pool,entry,sizeof(struct entry));
}

/***********************************************************************/

static struct NewMenu cMenu[] =
{
    MTITLE(MSG_CMenu_Title_New),
        MITEM(MSG_CMenu_Browser,MSG_CMenu_Browser),
        MITEM(MSG_CMenu_Link,MSG_CMenu_Link),
        MITEM(MSG_CMenu_CopyLink,MSG_CMenu_CopyLink),
        MBAR,
        MITEM(MSG_CMenu_CopyTitle,MSG_CMenu_CopyTitle),
        MBAR,
        MITEM(MSG_CMenu_Configure,MSG_CMenu_Configure),
    MEND
};

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct TagItem * attrs = msg->ops_AttrList;

    if (obj = (Object *)DoSuperNew(cl,obj,
            InputListFrame,
            MUIA_List_Title,  TRUE,
            MUIA_List_Format, (ULONG)"COL=0,COL=1,COL=2 P=\33r",
            TAG_MORE, (ULONG)attrs))
    {
        struct data *data = INST_DATA(cl,obj);

        data->format = MUIV_Entries_Subject|MUIV_Entries_Date;

        data->cmenu = MUI_MakeObject(MUIO_MenustripNM,(ULONG)cMenu,0);
        superset(cl,obj,MUIA_ContextMenu,data->cmenu);

        DoSuperMethod(cl,obj,MUIM_Notify,MUIA_Listview_DoubleClick,MUIV_EveryTime,(ULONG)obj,1,MUIM_Entries_DoubleClick);

        set(obj,MUIA_Entries_Format,0);
    }

    return (ULONG)obj;
}

/***********************************************************************/

static ULONG
mDispose(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);
    ULONG       res;

    if (data->cmenu) MUI_DisposeObject(data->cmenu);

    data->flags |= FLG_Disposing;

    res = DoSuperMethodA(cl,obj,msg);

    return res;
}

/***********************************************************************/

static ULONG
mConstruct(struct IClass *cl,Object *obj,struct MUIP_List_Construct *msg)
{
    struct entry *entry = msg->entry;
    APTR         pool = msg->pool;

    if (!entry->title)
    {
        STRPTR s;

        if ((s = entry->descr) || (s = entry->link))
        {
            if (entry->title = allocVecPooled(pool,strlen(s)+1))
            {
                strcpy(entry->title,s);
                stripHTML(entry->title,entry->title,TRUE);
            }
        }

        if (!entry->title)
        {
            entry->title = "-";
            entry->flags |= EFLG_NtFreeTitle;
        }
    }

    if (entry->author)  entry->authorLen  = strlen(entry->author);
    if (entry->subject) entry->subjectLen = strlen(entry->subject);
    if (entry->title)   entry->titleLen   = strlen(entry->title);
    if (entry->descr)   entry->descrLen   = strlen(entry->descr);
    if (entry->link)    entry->linkLen    = strlen(entry->link);

    return (ULONG)entry;
}

/***********************************************************************/

static ULONG
mDestruct(struct IClass *cl,Object *obj,struct MUIP_List_Destruct *msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (!(data->flags & FLG_Disposing)) freeNew(msg->pool,msg->entry);

    return 0;
}

/***********************************************************************/

static ULONG
mDisplay(struct IClass *cl,Object *obj,struct MUIP_List_Display *msg)
{
    struct data  *data = INST_DATA(cl,obj);
    struct entry *entry = msg->entry;
    UBYTE        **a = msg->array;

    if (entry)
    {
        *a++ = entry->title;
        *a++ = (entry->flags & EFLG_Group) ? entry->feed->name : (entry->subject ? entry->subject : (STRPTR)"");

        if (!entry->date.ds_Days && !entry->date.ds_Minute && !entry->date.ds_Tick)
        {
            *a = "";
        }
        else
        {
            struct DateTime dt;

            *data->bday  = 0;
            *data->bdate = 0;
            *data->btime = 0;

            copymem(&dt.dat_Stamp,&entry->date,sizeof(dt.dat_Stamp));
            dt.dat_Format  = FORMAT_DOS;
            dt.dat_Flags   = (data->flags & FLG_SubstDate) ? DTF_SUBST : 0;
            dt.dat_StrDay  = data->bday;
            dt.dat_StrDate = data->bdate;
            dt.dat_StrTime = data->btime;

            DateToStr(&dt);
            msprintf(data->date,"%s %s",(ULONG)data->bdate,(ULONG)data->btime);

            *a = data->date;
        }
    }
    else
    {
        STRPTR img;
        ULONG  sortCol = data->sortCol, revSort = data->revSort;

        if (sortCol!=0) msnprintf(data->titleBuf0,sizeof(data->titleBuf0),"%s    ",getString(MSG_Title_Headline));
        if (sortCol!=1) msnprintf(data->titleBuf1,sizeof(data->titleBuf0),"%s    ",getString(MSG_Title_Subject));
        if (sortCol!=2) msnprintf(data->titleBuf2,sizeof(data->titleBuf0),"%s    ",getString(MSG_Title_Date));

        img = revSort ? "\33I[6:38]" : "\33I[6:39]";

        switch (sortCol)
        {
            case 0:
                msnprintf(data->titleBuf0,sizeof(data->titleBuf0),"%s %s",getString(MSG_Title_Headline),img);
                break;

            case 1:
                msnprintf(data->titleBuf1,sizeof(data->titleBuf1),"%s %s",getString(MSG_Title_Subject),img);
                break;

            case 2:
                msnprintf(data->titleBuf2,sizeof(data->titleBuf2),"%s %s",getString(MSG_Title_Date),img);
                break;
        };

        *a++ = data->titleBuf0;
        *a++ = data->titleBuf1;
        *a   = data->titleBuf2;
    }

    return 0;
}

/***********************************************************************/

static LONG
compare(struct entry *e1,struct entry *e2,LONG col,ULONG rev)
{
    LONG res = 0; //gcc

    switch (col)
    {
        case 0:
    	    res = StrnCmp(g_loc,e1->title,e2->title,-1,SC_COLLATE2);
            break;

        case 1:
            if (e1->flags & EFLG_Group)
            {
        		res = StrnCmp(g_loc,e1->feed->name,e2->feed->name,-1,SC_COLLATE2);
            }
            else
            {
                STRPTR a, b;

                if (!(a = e1->subject)) a = "";
                if (!(b = e2->subject)) b = "";
	            res = StrnCmp(g_loc,a,b,-1,SC_COLLATE2);
            }
            break;

        case 2:
            res = CompareDates(&e1->date,&e2->date);
            break;
    }

    return rev ? -res : res;
}

static ULONG
mCompare(struct IClass *cl,Object *obj,struct MUIP_List_Compare *msg)
{
    struct data  *data = INST_DATA(cl,obj);
    struct entry *e1 = msg->entry1, *e2 = msg->entry2;

    return (ULONG)compare(e1,e2,data->sortCol,data->revSort);
}

/***************************************************************************/


static STRPTR
getFormat(struct data *data)
{
    STRPTR format;
    ULONG  flags = data->format;

    if (data->flags & FLG_NoListBar)
        if (flags & MUIV_Entries_Date)
            if (flags & MUIV_Entries_Subject) format = "C=0,C=1,C=2 P=\33r";
            else format = "C=0,C=2 P=\33r";
        else
            if (flags & MUIV_Entries_Subject) format = "C=0,C=1";
            else format = "C=0";
    else
        if (flags & MUIV_Entries_Date)
            if (flags & MUIV_Entries_Subject) format = "C=0,C=1,C=2 P=\33r";
            else format = "C=0,C=2 P=\33r";
        else
            if (flags & MUIV_Entries_Subject) format = "C=0,C=1";
            else format = "C=0";

    return format;
}

/***********************************************************************/

#define MUIA_List_TitleClick 0x80422fd9

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
            case MUIA_List_TitleClick:
            {
                ULONG sortCol = data->sortCol, revSort = data->revSort;

                if (sortCol==tidata) revSort = !revSort;
                else
                {
                    sortCol = tidata;
                    revSort = FALSE;
                }

                data->sortCol = sortCol;
                data->revSort = revSort;

                DoSuperMethod(cl,obj,MUIM_List_Sort);

                break;
            }

            case MUIA_RSS_Prefs:
            {
                struct prefs *prefs = (struct prefs *)tidata;
                ULONG        redraw = FALSE, redrawMode = 0; // gcc

                if (!BOOLSAME(data->flags & FLG_RightMouse,prefs->flags & PFLG_RightMouse))
                {
                    if (prefs->flags & PFLG_RightMouse) data->flags |= FLG_RightMouse;
                    else data->flags &= ~FLG_RightMouse;
                }

                if (!BOOLSAME(data->flags & FLG_NoListBar,prefs->flags & PFLG_NoListBar))
                {
                    if (prefs->flags & PFLG_NoListBar) data->flags |= FLG_NoListBar;
                    else data->flags &= ~FLG_NoListBar;

                    superset(cl,obj,MUIA_List_Format,getFormat(data));
                }

                if (!BOOLSAME(data->flags & FLG_BoldTitles,prefs->flags & PFLG_BoldTitles))
                {
                    if (prefs->flags & PFLG_BoldTitles) data->flags |= FLG_BoldTitles;
                    else data->flags &= ~FLG_BoldTitles;

                    redraw = TRUE;
                    redrawMode = MUIV_List_Redraw_All;
                }

                if (!BOOLSAME(data->flags & FLG_SubstDate,prefs->flags & PFLG_SubstDate))
                {
                    if (prefs->flags & PFLG_SubstDate) data->flags |= FLG_SubstDate;
                    else data->flags &= ~FLG_SubstDate;

                    redraw = TRUE;
                    redrawMode = MUIV_List_Redraw_All;
                }

                data->doubleClick = prefs->entriesDoubleClick;

                if (redraw) DoSuperMethod(cl,obj,MUIM_List_Redraw,redrawMode);

                break;
            }

            case MUIA_Entries_Format:
                if (data->format!=tidata)
                {
                    data->format = tidata;
                    superset(cl,obj,MUIA_List_Format,getFormat(data));
                }
                break;

            case MUIA_RSS_App:
                data->app = (Object *)tidata;
                break;

            case MUIA_RSS_Win:
                data->win = (Object *)tidata;
                break;
        }
    }

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static ULONG
mContextMenuChoice(struct IClass *cl,Object *obj,struct MUIP_ContextMenuChoice *msg)
{
    struct data *data = INST_DATA(cl,obj);
    ULONG       id = muiUserData(msg->item);

    switch (id)
    {
        case MSG_CMenu_Browser:
            if (data->active && data->active->link) goURLFun(data->active->link);
            break;

        case MSG_CMenu_Link:
            if (data->active && data->active->link)
                DoMethod(data->app,MUIM_RSS_GotoURL,(ULONG)data->active->link,MUIV_RSS_GotoURL_IgnorePrefs|MUIV_RSS_GotoURL_NoTarget);
            break;

        case MSG_CMenu_CopyTitle:
        case MSG_CMenu_CopyLink:
            if (data->active)
            {
                UBYTE buf[512];

                *buf = 0;

                if (id==MSG_CMenu_CopyLink)
                {
                    if (data->active->link) stccpy(buf,data->active->link,sizeof(buf));
                }
                else
                {
                    if (data->active->title) stccpy(buf,data->active->title,sizeof(buf));
                }

                if (*buf)
                {
                    struct IFFHandle *iff;

                    if (iff = AllocIFF())
                    {
                        if (iff->iff_Stream = (ULONG)OpenClipboard(0))
                        {
                            InitIFFasClip(iff);

                            if (!(OpenIFF(iff,IFFF_WRITE)))
                            {
                                ULONG len = strlen(buf);

                                if (!PushChunk(iff,ID_FTXT,ID_FORM,IFFSIZE_UNKNOWN) &&
                                    !PushChunk(iff,ID_FTXT,ID_CHRS,len) &&
                                    (WriteChunkBytes(iff,buf,len)==len))
                                    PopChunk(iff);

                                CloseIFF(iff);
                            }

                            CloseClipboard((struct ClipboardHandle *)iff->iff_Stream);
                        }

                        FreeIFF(iff);
                    }
                }
            }
            break;

        case MSG_CMenu_Configure:
            DoMethod(data->app,MUIM_App_PrefsWin,MUIV_Prefs_Page_Options);
            break;
    }

    return 0;
}

/***********************************************************************/

enum
{
    CFLG_WidthAll  = 1<<0,
    CFLG_WidthThis = 1<<1,
    CFLG_OrderAll  = 1<<2,
    CFLG_OrderThis = 1<<3,
};

static ULONG
mContextMenuBuild(struct IClass *cl,Object *obj,struct MUIP_ContextMenuBuild *msg)
{
    struct data *data = INST_DATA(cl,obj);
    APTR        menu;

    if (data->flags & FLG_RightMouse)
    {
        struct MUI_List_TestPos_Result tp;

        DoSuperMethod(cl,obj,MUIM_List_TestPos,msg->mx,msg->my,(ULONG)&tp);
        if (tp.entry>=0) SetSuperAttrs(cl,obj,MUIA_List_Active,tp.entry,TAG_DONE);
    }

    if (menu = data->cmenu)
    {
        struct entry *active;
        ULONG        link, title;

        DoSuperMethod(cl,obj,MUIM_List_GetEntry,MUIV_List_GetEntry_Active,(ULONG)&active);

        if (active)
        {
            data->active = active;

            link  = (ULONG)active->link;
            title = (ULONG)active->title;
        }
        else
        {
            data->active = NULL;

            link = title = NULL;
        }

        set((Object *)DoMethod(menu,MUIM_FindUData,MSG_CMenu_Browser),MUIA_Menuitem_Enabled,link);
        set((Object *)DoMethod(menu,MUIM_FindUData,MSG_CMenu_Link),MUIA_Menuitem_Enabled,link);
        set((Object *)DoMethod(menu,MUIM_FindUData,MSG_CMenu_CopyLink),MUIA_Menuitem_Enabled,link);
        set((Object *)DoMethod(menu,MUIM_FindUData,MSG_CMenu_CopyTitle),MUIA_Menuitem_Enabled,title);

        return (ULONG)menu;
    }

    return NULL;
}

/***********************************************************************/

static ULONG
mDoubleClick(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    switch (data->doubleClick)
    {
        case EDC_Browser:
            DoSuperMethod(cl,obj,MUIM_List_GetEntry,MUIV_List_GetEntry_Active,(ULONG)&data->active);
            if (data->active && data->active->link) goURLFun(data->active->link);
            break;

        case EDC_GoToLink:
            DoSuperMethod(cl,obj,MUIM_List_GetEntry,MUIV_List_GetEntry_Active,(ULONG)&data->active);
            if (data->active && data->active->link)
                DoMethod(data->app,MUIM_RSS_GotoURL,(ULONG)data->active->link,MUIV_RSS_GotoURL_IgnorePrefs|MUIV_RSS_GotoURL_NoTarget);
            break;

        default:
            break;
    }

    return 0;
}

/***********************************************************************/

static ULONG
mClear(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    data->sortCol = 2;
    data->revSort = FALSE;

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

M_DISP(dispatcher)
{
    M_DISPSTART

    switch (msg->MethodID)
    {
        case OM_NEW:                      return mNew(cl,obj,(APTR)msg);
        case OM_DISPOSE:                  return mDispose(cl,obj,(APTR)msg);
        case OM_SET:                      return mSets(cl,obj,(APTR)msg);

        case MUIM_ContextMenuChoice:      return mContextMenuChoice(cl,obj,(APTR)msg);

        case MUIM_List_Construct:         return mConstruct(cl,obj,(APTR)msg);
        case MUIM_List_Destruct:          return mDestruct(cl,obj,(APTR)msg);
        case MUIM_List_Display:           return mDisplay(cl,obj,(APTR)msg);
        case MUIM_List_Compare:           return mCompare(cl,obj,(APTR)msg);
        case MUIM_List_Clear:             return mClear(cl,obj,(APTR)msg);

        case MUIM_ContextMenuBuild:       return mContextMenuBuild(cl,obj,(APTR)msg);
        case MUIM_Entries_DoubleClick:    return mDoubleClick(cl,obj,(APTR)msg);

        default:                          return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(dispatcher)

/***********************************************************************/

ULONG
initEntriesListClass(void)
{
    if (entriesListClass = MUI_CreateCustomClass(NULL,MUIC_List,NULL,sizeof(struct data),DISP(dispatcher)))
    {
        localizeMenus(cMenu);

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/

void
disposeEntriesListClass(void)
{
    MUI_DeleteCustomClass(entriesListClass);
}

/***********************************************************************/
