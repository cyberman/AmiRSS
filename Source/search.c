
#include "rss.h"
#define CATCOMP_NUMBERS
#include "loc.h"

/***********************************************************************/

struct data
{
    Object *str;
    Object *scase;
    Object *info;
    Object *top;
    Object *succ;
    Object *back;

    Object *list;
    Object *HTML;

    APTR   pool;
};

/***********************************************************************/

enum
{
    SFLG_Case = 1<<0,
};

/***********************************************************************/

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct data    temp;
    Object         *cancel;
    struct TagItem *attrs = msg->ops_AttrList;

    memset(&temp,0,sizeof(temp));

    if (obj = (Object *)DoSuperNew(cl,obj,
            MUIA_HelpNode,         (ULONG)"search.html",
            MUIA_Window_Title,     (ULONG)getString(MSG_SearchWin_Title),
            MUIA_Window_ID,        MAKE_ID('w','s','e','r'),

            WindowContents, (ULONG)(VGroup,

                Child, VGroup,
                    MUIA_Frame,      MUIV_Frame_Group,
                    MUIA_Background, MUII_GroupBack,

                    Child, HGroup,
                        Child, olabel2(MSG_SearchWin_Pattern),
                        Child, (ULONG)(temp.str = ostring(DEF_SEARCHSIZE,MSG_SearchWin_Pattern,MSG_SearchWin_PatternHelp)),
                    End,

                    Child, HGroup,
                        Child, (ULONG)HSpace(0),
                        Child, olabel1(MSG_SearchWin_Case),
                        Child, (ULONG)(temp.scase = ocheck(MSG_SearchWin_Case,MSG_SearchWin_CaseHelp)),
                        Child, (ULONG)HSpace(0),
                    End,
                End,

                Child, (ULONG)(temp.info = TextObject,
                    MUIA_Frame,         MUIV_Frame_Text,
                    MUIA_Background,    MUII_TextBack,
                    MUIA_Text_PreParse, MUIX_C,
                    MUIA_Text_Contents, getString(MSG_SearchWin_Welcome),
                End),

                Child, HGroup,
                    Child, (ULONG)(temp.top = obutton(MSG_SearchWin_First,MSG_SearchWin_FirstHelp)),
                    Child, (ULONG)wspace(25),
                    Child, (ULONG)(temp.succ = obutton(MSG_SearchWin_Succ,MSG_SearchWin_SuccHelp)),
                    Child, (ULONG)wspace(25),
                    Child, (ULONG)(temp.back   = obutton(MSG_SearchWin_Back,MSG_SearchWin_BackHelp)),
                    Child, (ULONG)wspace(25),
                    Child, cancel = obutton(MSG_SearchWin_Cancel,MSG_SearchWin_CancelHelp),
                End,
            End),
            TAG_MORE,(ULONG)attrs))
    {
        struct data *data = INST_DATA(cl,obj);

        *data = temp;

        data->list    = (Object *)GetTagData(MUIA_Search_ListObj,NULL,attrs);
        data->HTML    = (Object *)GetTagData(MUIA_Search_HTMLObj,NULL,attrs);
        data->pool    = (APTR)GetTagData(MUIA_RSS_Pool,NULL,attrs);

        set(data->str,MUIA_Textinput_ResetMarkOnCursor,TRUE);
        set(data->scase,MUIA_CycleChain,FALSE);

        DoSuperMethod(cl,obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,(ULONG)obj,3,MUIM_Set,MUIA_Window_Open,FALSE);

        DoMethod(data->str,MUIM_Notify,MUIA_Textinput_Acknowledge,MUIV_EveryTime,(ULONG)obj,2,MUIM_Search_Search,MUIV_Search_Top);
        DoMethod(data->top,MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)obj,2,MUIM_Search_Search,MUIV_Search_Top);
        DoMethod(data->succ,MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)obj,2,MUIM_Search_Search,MUIV_Search_Succ);
        DoMethod(data->back,MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)obj,2,MUIM_Search_Search,MUIV_Search_Back);
        DoMethod(cancel,MUIM_Notify,MUIA_Pressed,FALSE,(ULONG)obj,3,MUIM_Set,MUIA_Window_Open,FALSE);
    }

    return (ULONG)obj;
}

/***********************************************************************/

static  ULONG
mSets( struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct data *data = INST_DATA(cl,obj);
    ULONG       res;

    res = DoSuperMethodA(cl,obj,(Msg)msg);

    if (GetTagData(MUIA_Window_Open,FALSE,msg->ops_AttrList))
    {
        set(data->info,MUIA_Text_Contents,getString(MSG_SearchWin_Welcome));
        DoMethod(data->str,MUIM_Textinput_DoMarkAll);
        SetSuperAttrs(cl,obj,MUIA_Window_ActiveObject,(ULONG)data->str,TAG_DONE);
    }

    return res;
}

/***********************************************************************/

static STRPTR
mstrstr(STRPTR s1,STRPTR s2,ULONG scase)
{
    ULONG len2 = strlen(s2);
    int   c2 = scase ? *s2 : ToLower(*s2);

    if (!len2) return s1;

    for (; *s1; s1++)
    {
        int c1 = scase ? *s1 : ToLower(*s1);

        if (c2==c1 && !(scase ? strncmp(s1,s2,len2) : strnicmp(s1,s2,len2)))
            return s1;
    }

    return NULL;
}

/***********************************************************************/

static ULONG INLINE
checkString(STRPTR patt,STRPTR string,ULONG flags)
{
    return (ULONG)mstrstr(string,patt,flags & SFLG_Case);
}

/***********************************************************************/

static ULONG INLINE
checkHTML(APTR pool, STRPTR patt,STRPTR string,ULONG flags)
{
    UBYTE  buf[2048];
    STRPTR s;
    ULONG  res = FALSE, len;

    len = strlen(string)+1;

    if (len>sizeof(buf)) s = AllocPooled(pool,len);
    else s = buf;

    if (s)
    {
        STRPTR f, t;
        int    ignore;

        for (f = string, t = s, ignore = 0; ; )
        {
            UBYTE c;

            c = *f;

            if (c==0)
                break;

            if (c=='<')
            {
                f++;
                ignore++;
                continue;
            }

            if (c=='>')
            {
                *t++ = '\1';
                f++;
                ignore--;
                continue;
            }

            if (ignore)
            {
                f++;
                continue;
            }

            *t++ = *f++;
        }

        if (!*f && !ignore)
        {
            *t = 0;
            res = checkString(patt,s,flags);
        }

        if (s!=buf) FreePooled(pool,s,len);
    }

    return res;
}

/***********************************************************************/

static ULONG
mSearch(struct IClass *cl,Object *obj,struct MUIP_Search_Search *msg)
{
    struct data                   *data = INST_DATA(cl,obj);
    struct entry                  *entry;
    UBYTE                         buf[DEF_SEARCHSIZE*2];
    STRPTR                        str;
    ULONG                         flags, res = FALSE, HTML = FALSE;
    LONG                          pos;

    flags = 0;

    switch (msg->mode)
    {
        case MUIV_Search_Top:
            pos = 0;
            break;

        case MUIV_Search_Succ:
            get(data->list,MUIA_List_Active,&pos);
            pos++;
            break;

        case MUIV_Search_Back:
            get(data->list,MUIA_List_Active,&pos);
            if (pos==0) goto done;
            pos--;
            break;

        default:
            goto done;
            break;
    }

    get(data->str,MUIA_Textinput_Contents,&str);
    if (!*str) goto done;
    strcpy(buf,str);

    set(data->info,MUIA_Text_Contents,getString(MSG_SearchWin_Searching));

    if (xget(data->scase,MUIA_Selected)) flags |= SFLG_Case;

    for (;;)
    {
        DoMethod(data->list,MUIM_List_GetEntry,(ULONG)pos,(ULONG)&entry);
        if (!entry) goto done;

        if ((entry->author && checkString(buf,entry->author,flags)) ||
            (entry->subject && checkString(buf,entry->subject,flags)) ||
            (entry->title && checkString(buf,entry->title,flags)) ||
            (entry->link && checkString(buf,entry->link,flags)))
        {
            res = TRUE;
            goto done;
        }

        if (entry->descr)
        {
            if (checkHTML(data->pool,buf,entry->descr,flags))
            {
                res = HTML = TRUE;
                goto done;
            }
        }

        if (msg->mode==MUIV_Search_Back)
        {
            pos--;
            if (pos<0) goto done;
        }
        else pos++;
    }

done:
    if (res)
    {
        set(data->list,MUIA_List_Active,pos);
        if (HTML) DoMethod(data->HTML,MUIM_HTMLview_Search,(ULONG)buf,MUIF_HTMLview_Search_FromTop|((flags & SFLG_Case) ? MUIF_HTMLview_Search_CaseSensitive : 0));
        set(data->info,MUIA_Text_Contents,getString(MSG_SearchWin_Found));
    }
    else
    {
        set(data->info,MUIA_Text_Contents,getString(MSG_SearchWin_NotFound));
    }

    return 0;
}

/***********************************************************************/

M_DISP(dispatcher)
{
    M_DISPSTART

    switch(msg->MethodID)
    {
        case OM_NEW:             return mNew(cl,obj,(APTR)msg);
        case OM_SET:             return mSets(cl,obj,(APTR)msg);

        case MUIM_Search_Search: return mSearch(cl,obj,(APTR)msg);

        default:                 return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(dispatcher)

/***********************************************************************/

ULONG
initSearchClass(void)
{
    return (ULONG)(searchClass = MUI_CreateCustomClass(NULL,NULL,winClass,sizeof(struct data),DISP(dispatcher)));
}

/***********************************************************************/

void
disposeSearchClass(void)
{
    MUI_DeleteCustomClass(searchClass);
    searchClass = NULL;
}

/***********************************************************************/
