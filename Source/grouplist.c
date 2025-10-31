
#include "rss.h"
#include <dos/dostags.h>
#include <mui/Lamp_mcc.h>
#define CATCOMP_NUMBERS
#include "loc.h"

/***********************************************************************/

#define SACTIVE(cl,obj) ((struct MUI_NListtree_TreeNode *)DoSuperMethod(cl,obj,MUIM_NListtree_GetEntry,MUIV_NListtree_GetEntry_ListNode_Active,MUIV_NListtree_GetEntry_Position_Active,0))
#define ACTIVE(obj)     ((struct MUI_NListtree_TreeNode *)DoMethod(obj,MUIM_NListtree_GetEntry,MUIV_NListtree_GetEntry_ListNode_Active,MUIV_NListtree_GetEntry_Position_Active,0))

/***********************************************************************/

struct data
{
    APTR                          pool;
    Object                        *app;
    Object                        *win;
    Object                        *this;
    Object                        *group;
    Object                        *list;
    APTR                          menu;

    struct MUI_NListtree_TreeNode *tn;
    struct MUI_NListtree_TreeNode *def;

    struct MinList                images;
    struct image                  *groupIm;
    APTR                          alertIm;
    Object                        *alertObj;

    ULONG                         editID;

    ULONG                         doubleClick;
    LONG                          GMTOffset;
    ULONG                         maxImageSize;

    struct SignalSemaphore        sem;
    struct MsgPort                *notifyPort;
    int                           sig;
    struct MUI_InputHandlerNode   ih;
    struct MinList                feeds;

    ULONG                         localTasks;
    ULONG                         remoteTasks;
    ULONG                         entryPos;
    ULONG                         news;
    struct MUI_InputHandlerNode   th;

    struct codeset                *codeset;
    STRPTR                        proxy;
    int                           proxyPort;
    STRPTR                        agent;

    UBYTE                         name[DEF_NAMESIZE+32];

    ULONG                         flags;
};

enum
{
    FLG_Setup                = 1<<0,
    FLG_RightMouse           = 1<<1,
    FLG_SkipDelFile          = 1<<2,
    FLG_BoldGroups           = 1<<3,
    FLG_NoDragDrop           = 1<<4,
    FLG_UseFeedsListImages   = 1<<5,
    FLG_UseDefaultGroupImage = 1<<6,
    FLG_Disposing            = 1<<7,
    FLG_SubstWinChars        = 1<<8,
    FLG_UseProxy             = 1<<9,
    FLG_Timer                = 1<<10,
    FLG_NoAutoUpdate         = 1<<11,
    FLG_AutoUpdate           = 1<<12,
    FLG_PushedUpdate         = 1<<13,
    FLG_ScaleImages          = 1<<14,
    FLG_ShowGroupNews        = 1<<15,
    FLG_URLInArgs            = 1<<16,
    FLG_SavePrefs            = 1<<17,
};

/***********************************************************************/

static Object *
opic(struct data *data,STRPTR name)
{
    ULONG scale = data->flags & FLG_ScaleImages;

    return picObject,
        MUIA_Pic_File,        name,
        MUIA_Pic_Transparent, TRUE,
        scale ? MUIA_Pic_ScaledMaxSize : TAG_IGNORE, data->maxImageSize,
    End;
}

/***********************************************************************/

static struct image *
findImage(struct data *data,STRPTR pic)
{
    BPTR lock;

    if (lock = Lock(pic,SHARED_LOCK))
    {
        register struct image *image, *succ;

        for (image = (struct image *)data->images.mlh_Head; succ = (struct image *)image->link.mln_Succ; image = succ)
            if (SameLock(image->lock,lock)==LOCK_SAME) break;

        if (succ)
        {
            UnLock(lock);
            image->count++;

            return image;
        }

        if (image = AllocPooled(data->pool,sizeof(struct image)))
        {
            if (image->picObj = opic(data,pic))
            {
                if ((data->flags & FLG_Setup) && (data->flags & FLG_UseFeedsListImages))
                    image->picIm = (APTR)DoMethod(data->this,MUIM_NList_CreateImage,(ULONG)image->picObj,0L);
                else image->picIm = NULL;

                image->lock  = lock;
                image->count = 1;

                AddHead(LIST(&data->images),NODE(image));

                return image;
            }

            FreePooled(data->pool,image,sizeof(struct image));
        }

        UnLock(lock);
    }

    return NULL;
}

/***********************************************************************/

static void
disposeImage(struct data *data,struct image *image)
{
    if (--image->count==0)
    {
        Remove(NODE(image));
        if (image->picIm) DoMethod(data->this,MUIM_NList_DeleteImage,(ULONG)image->picIm);
        UnLock(image->lock);
        MUI_DisposeObject(image->picObj);
        FreePooled(data->pool,image,sizeof(struct image));
    }
}

/***********************************************************************/

static void
setupImages(struct data *data)
{
    register struct image *image, *succ;

    for (image = (struct image *)data->images.mlh_Head; succ = (struct image *)image->link.mln_Succ; image = succ)
        image->picIm = (APTR)DoMethod(data->this,MUIM_NList_CreateImage,(ULONG)image->picObj,0L);
}

/***********************************************************************/

static void
cleanupImages(struct data *data)
{
    register struct image *image, *succ;

    for (image = (struct image *)data->images.mlh_Head; succ = (struct image *)image->link.mln_Succ; image = succ)
        if (image->picIm)
        {
            DoMethod(data->this,MUIM_NList_DeleteImage,(ULONG)image->picIm);
            image->picIm = NULL;
        }
}

/***********************************************************************/

static void
disposeImages(struct data *data)
{
    struct image *image;

    while (image = (struct image *)RemHead(LIST(&data->images)))
    {
        MUI_DisposeObject(image->picObj);
        UnLock(image->lock);
        FreePooled(data->pool,image,sizeof(struct image));
    }
}

/***********************************************************************/

static void
scaleImages(struct data *data)
{
    struct image *image, *succ;

    for (image = (struct image *)data->images.mlh_Head; succ = (struct image *)image->link.mln_Succ; image = succ)
    {
        if (image->picIm) DoMethod(data->this,MUIM_NList_DeleteImage,(ULONG)image->picIm);
        set(image->picObj,MUIA_Pic_ScaledMaxSize,(data->flags & FLG_ScaleImages) ? data->maxImageSize : 0);
        image->picIm = (APTR)DoMethod(data->this,MUIM_NList_CreateImage,(ULONG)image->picObj,0L);
    }
}

/***********************************************************************/

static APTR
makeImage(struct data *data,Object *obj,struct feed *feed)
{
    if (feed->picIm)
    {
        disposeImage(data,feed->picIm);
        feed->picIm = NULL;
    }

    if (feed->pic && *feed->pic)
        feed->picIm = findImage(data,feed->pic);

    return feed->picIm;
}

/***********************************************************************/

static ULONG
parseMsg(struct data *data,struct resultMsg *rm)
{
    struct feed *feed = rm->tn->tn_User;
    ULONG       stop = FALSE;

    switch (rm->type)
    {
        case MTYPE_Status:
        {
            struct statusMsg *sm = (struct statusMsg *)rm;
            ULONG            id;

            if (sm->status==MSG_Status_Done)
            {
                stop = TRUE;

                id = sm->err ? sm->err : MSG_Status_Done;

                if (!(feed->flags & FFLG_FavIcon))
                    MD5Final(feed->digest,&feed->pdata.ctx);
            }
            else id = sm->status;

            if (!(feed->flags & FFLG_Failure) && ((feed->flags & FFLG_Local) || (ACTIVE(data->this)==feed->tn)))
            {
                if (stop)
                {
		    if (data->flags & FLG_URLInArgs) data->flags &= ~FLG_URLInArgs;
		    else set(data->group,MUIA_MainGroup_Status,MUIV_MainGroup_Status_Feed);
                }

                if (feed->tmsg.flags & TMFLG_ErrorBuf)
                {
                    UBYTE buf[256];

                    msnprintf(buf,sizeof(buf),"%s \"%s\"",getString(id),feed->errorBuf);
                    DoMethod(data->group,MUIM_MainGroup_SetInfo,(ULONG)buf,0);
                }
                else
                {
                    if ((feed->flags & FFLG_Local))
                    {
                        if (!(feed->pdata.flags & PFLG_Group))
                        {
                            UBYTE buf[256];

                            msnprintf(buf,sizeof(buf),getString(MSG_Group_LocalFeed),feed->name);
                            DoMethod(data->group,MUIM_MainGroup_SetInfo,(ULONG)buf,0);
                        }
                    }
                    else DoMethod(data->group,MUIM_MainGroup_SetInfo,NULL,id);
                }
            }

            if (stop) feed->flags &= ~FFLG_Failure;

            break;
        }

        case MTYPE_Line:
        {
            struct lineMsg *lm = (struct lineMsg *)rm;

            if (feed->fh)
            {
                if (feed->flags & FFLG_FavIcon) feed->flags &= ~FFLG_Delete;
                writeFile(feed->fh,lm->line,lm->len);
            }

	    if (!(feed->flags & FFLG_FavIcon) && feed->parser && (XML_Parse(feed->parser,lm->line,lm->len,0)==XML_STATUS_ERROR))
                stop = TRUE;

            break;
        }

        case MTYPE_Date:
        {
            struct dateMsg *dm = (struct dateMsg *)rm;

            copymem(&feed->pdata.info.date,&dm->ds,sizeof(struct DateStamp));

            break;
        }
    }

    return stop;
}

/***********************************************************************/

static void
updateInfo(struct data *data,struct feed *feed,struct feedInfo *info)
{
    if (!(feed->flags & FFLG_Group))
    {
        APTR pool = data->pool;

        if (feed->info.author)
        {
            freeVecPooled(pool,feed->info.author);
            feed->info.author = NULL;
        }

        if (feed->info.subject)
        {
            freeVecPooled(pool,feed->info.subject);
            feed->info.subject = NULL;
        }

        if (feed->info.title)
        {
            freeVecPooled(pool,feed->info.title);
            feed->info.title = NULL;
        }

        if (feed->info.descr)
        {
            freeVecPooled(pool,feed->info.descr);
            feed->info.descr = NULL;
        }

        if (feed->info.link)
        {
            freeVecPooled(pool,feed->info.link);
            feed->info.link = NULL;
        }

        if (feed->info.image)
        {
            freeVecPooled(pool,feed->info.image);
            feed->info.image = NULL;
        }

        if (feed->info.imageLink)
        {
            freeVecPooled(pool,feed->info.imageLink);
            feed->info.imageLink = NULL;
        }

        if (feed->info.author = info->author)
            feed->info.authorLen = strlen(info->author);
        else feed->info.authorLen = 0;

        if (feed->info.subject = info->subject)
            feed->info.subjectLen = strlen(info->subject);
        else feed->info.subjectLen = 0;

        if (feed->info.title = info->title)
            feed->info.titleLen = strlen(info->title);
        else feed->info.titleLen = 0;

        if (feed->info.descr = info->descr)
            feed->info.descrLen = strlen(info->descr);
        else feed->info.descrLen = 0;

        if (feed->info.link = info->link)
            feed->info.linkLen = strlen(info->link);
        else feed->info.linkLen = 0;

        if (feed->info.image = info->image)
            feed->info.imageLen = strlen(info->image);
        else feed->info.imageLen = 0;

        if (feed->info.imageLink = info->imageLink)
            feed->info.imageLinkLen = strlen(info->imageLink);
        else feed->info.imageLinkLen = 0;

        feed->info.iwidth  = info->iwidth;
        feed->info.iheight = info->iheight;
        feed->info.news    = info->news;

        copymem(&feed->info.date,&info->date,sizeof(feed->info.date));
    }
}

/***********************************************************************/

static void
checkNew(struct data *data,struct feed *feed)
{
    /*int i;
    printf("--- %s ---\n",feed->name);
    printf("MD5: ");
    for (i = 0; i<sizeof(feed->digestBack); i++) printf("%02lx",feed->digestBack[i]);
    printf(" . ");
    for (i = 0; i<sizeof(feed->digest); i++) printf("%02lx",feed->digest[i]);
    printf("  -> %ld\n\n",memcmp(feed->digest,feed->digestBack,sizeof(feed->digest)));*/

    if (memcmp(feed->digest,feed->digestBack,sizeof(feed->digest)))
    {
        if (!(feed->flags & FFLG_News))
        {
            feed->flags |= FFLG_News;

            if (data->news++==0)
            {
                set(data->app,MUIA_RSS_YouHaveNews,TRUE);
                set(data->this,MUIA_RSS_YouHaveNews,TRUE);
            }

            DoMethod(data->this,MUIM_NListtree_Redraw,(ULONG)feed->tn,0);
        }
    }
}

/***********************************************************************/

static void
breakProc(struct data *data,struct feed *feed,ULONG wait)
{
    struct MUI_NListtree_TreeNode *tn = feed->tn;
    ULONG                         update = FALSE, reload = FALSE, active = ACTIVE(data->this)==tn;
    ULONG                         disposing = data->flags & FLG_Disposing;

    if (feed->flags & FFLG_Task)
    {
        register struct resultMsg *rm;

        Remove(NODE(feed));

        if (wait)
        {
            ObtainSemaphore(&feed->sem);
            if (feed->proc) Signal((struct Task *)feed->proc,SIGBREAKF_CTRL_C);
            ReleaseSemaphore(&feed->sem);

            WaitPort(&feed->port);
            GetMsg(&feed->port);
        }

        feed->flags &= ~FFLG_Task;

        while (rm = (struct resultMsg *)GetMsg(&feed->resPort))
            freeArbitrateVecPooled(rm);

        if (!disposing)
        {
            if (!(feed->flags & (FFLG_Delete|FFLG_Local)))
                checkNew(data,feed);

            if (feed->errorBuf)
            {
                freeVecPooled(data->pool,feed->errorBuf);
                feed->errorBuf = NULL;
            }

            if (feed->flags & FFLG_Local)
            {
                feed->flags &= ~FFLG_Local;

                if (--data->localTasks==0)
                {
                    if (feed->pdata.flags & PFLG_Group)
                    {
                        DoMethod(data->group,MUIM_MainGroup_SetInfo,NULL,MSG_Status_GroupRead);
                        active = TRUE;
                    }
                }

                set(data->app,MUIA_Application_Sleep,FALSE);
            }
            else data->remoteTasks--;

            update = TRUE;
        }
    }

    if (!(feed->flags & FFLG_FavIcon) && feed->parser)
    {
        ULONG err, line;

        err = XML_GetErrorCode(feed->parser);
        line = XML_GetCurrentLineNumber(feed->parser);

        XML_Parse(feed->parser,"",0,1);
        XML_ParserFree(feed->parser);
        feed->parser = NULL;

        if (!disposing)
        {
            if (!feed->pdata.info.date.ds_Days && !feed->pdata.info.date.ds_Minute && !feed->pdata.info.date.ds_Tick)
                DateStamp(&feed->pdata.info.date);
            updateInfo(data,feed,&feed->pdata.info);

            if (active)
            {
                if (err)
                {
                    UBYTE buf[1024];

                    if (err==XML_ERROR_UNKNOWN_ENCODING) msnprintf(buf,sizeof(buf),getString(MSG_Group_UnknownEncoding),feed->pdata.encoding ? feed->pdata.encoding : (STRPTR)"");
                    else msnprintf(buf,sizeof(buf),getString(MSG_Group_CantParseXML),err,line,XML_ErrorString(err));
                    DoMethod(data->group,MUIM_MainGroup_SetInfo,(ULONG)buf,0);
                }

                DoMethod(data->group,MUIM_MainGroup_SetFeedInfo,(ULONG)&feed->info);
            }

            if (feed->pdata.encoding) freeVecPooled(data->pool,feed->pdata.encoding);
        }
    }

    if (feed->fh)
    {
        closeFile(feed->fh);
        feed->fh = NULL;

        if (feed->flags & FFLG_Delete)
        {
	    BPTR lock;

            feed->flags &= ~FFLG_Delete;

            if (lock = Lock(feed->tempFile,SHARED_LOCK))
            {
            	UnLock(lock);
            	DeleteFile(feed->tempFile);
	    }

            if (feed->favIconFile)
            {
                freeVecPooled(data->pool,feed->favIconFile);
                feed->favIconFile = NULL;
            }

            if (!(feed->flags & FFLG_FavIcon)) reload = active;
        }
        else
        {
            STRPTR file = (feed->flags & FFLG_FavIcon) ? feed->favIconFile : feed->file;
	    BPTR   lock;

            if (lock = Lock(file,SHARED_LOCK))
	    {
            	UnLock(lock);
            	DeleteFile(file);
            }

            Rename(feed->tempFile,file);

            if (feed->flags & FFLG_FavIcon)
            {
                if (feed->pic) freeVecPooled(data->pool,feed->pic);
                feed->pic = feed->favIconFile;
                feed->favIconFile = NULL;
                if (makeImage(data,data->this,feed)) DoMethod(data->this,MUIM_NListtree_Redraw,(ULONG)feed->tn,0);
            }
        }

        feed->flags &= ~FFLG_FavIcon;
    }
    else feed->flags &= ~FFLG_Delete;

    if ((feed->flags & FFLG_Local) && (feed->flags & FFLG_Delete))
    {
        if (feed->file)
        {
            freeVecPooled(data->pool,feed->file);
            feed->file = NULL;
        }
    }

    if (!disposing)
    {
        if (reload)
        {
            feed->flags |= FFLG_Failure;
            DoMethod(data->this,MUIM_GroupList_Feed,MUIV_GroupList_Feed_Local,(ULONG)tn);
        }
        else if (active) set(data->list,MUIA_List_Active,MUIV_List_Active_Top);

        if (update)
        {
            if (data->flags & FLG_AutoUpdate)
            {
                if (data->remoteTasks<8)
                {
                    if (!(data->flags & FLG_PushedUpdate))
                    {
                        DoMethod(data->app,MUIM_Application_PushMethod,(ULONG)data->this,1,MUIM_GroupList_HandleTimer);
                        data->flags |= FLG_PushedUpdate;
                    }
                }
            }
        }
    }
}

/***********************************************************************/

static void
freeFeed(struct data *data,APTR pool,struct feed *feed,ULONG doBreakProc)
{
    if (!(feed->flags & FFLG_Group) && doBreakProc) breakProc(data,feed,TRUE);

    if (feed->ewin) DoMethod(feed->ewin,MUIM_EditFeed_Close,FALSE);

    if (feed->name) freeVecPooled(pool,feed->name);

    if (!(data->flags & FLG_Disposing) && feed->picIm)
        disposeImage(data,feed->picIm);

    if (feed->pic) freeVecPooled(pool,feed->pic);

    if (!(feed->flags & FFLG_Group))
    {
        if (feed->URL)          freeVecPooled(pool,feed->URL);
        if (feed->info.author)  freeVecPooled(pool,feed->info.author);
        if (feed->info.subject) freeVecPooled(pool,feed->info.subject);
        if (feed->info.title)   freeVecPooled(pool,feed->info.title);
        if (feed->info.descr)   freeVecPooled(pool,feed->info.descr);
        if (feed->info.link)    freeVecPooled(pool,feed->info.link);

        if (feed->file)
        {
            if (!(data->flags & FLG_SkipDelFile))
            {
                BPTR lock;

                if (lock = Lock(feed->file,SHARED_LOCK))
                {
                    UnLock(lock);
                    DeleteFile(feed->file);
                }
            }

            freeVecPooled(pool,feed->file);
        }

        if (feed->tempFile) freeVecPooled(pool,feed->tempFile);
    }

    FreePooled(pool,feed,sizeof(struct feed));
}

/***************************************************************************/

static INLINE void
parseString(STRPTR str)
{
    register STRPTR f, t;

    if (!str || !*str) return;

    f = t = str;
    for (;;)
    {
        register UBYTE a, b;

        if (*f==0)
            break;

        if (*f!='%')
        {
            *t++ = *f++;
            continue;
        }

        a = *(++f);

        if (a>='a' && a<='f')
        {
            a -= 'a';
            a += 10;
        }
        else
            if (a>='A' && a<='F')
            {
                a -= 'A';
                a += 10;
            }
            else
                if (a>='0' && a<='9')
                {
                    a -= '0';
                }
                else break;

        b = *(++f);

        if (b>='a' && b<='f')
        {
            b -= 'a';
            b += 10;
        }
        else
            if (b>='A' && b<='F')
            {
                b -= 'A';
                b += 10;
            }
            else
                if (b>='0' && b<='9')
                {
                    b -= '0';
                }
                else break;

        f++;
        *t++ = a*16+b;
    }

    *t = 0;
}

/***************************************************************************/

#ifdef __MORPHOS__
static struct feed *
conFun(void)
{
    //struct Hook 		             *hook = (struct Hook *)REG_A0;
    //Object      		             *obj = (Object *)REG_A2;
    struct MUIP_NListtree_ConstructMessage *msg = (struct MUIP_NListtree_ConstructMessage *)REG_A1;
#else
static struct feed * SAVEDS ASM
conFun(REG(a0,struct Hook *hook),REG(a2,Object *obj),REG(a1,struct MUIP_NListtree_ConstructMessage *msg))
{
#endif
    struct feed *feed = msg->UserData;

    if (feed->flags & FFLG_SkipUTF8) return feed;

    parseString(feed->name);

    if (!(feed->flags & FFLG_Group))
    {
        parseString(feed->URL);
        parseString(feed->file);
    }

    return feed;
}

#ifdef __MORPHOS__
static struct EmulLibEntry conTrap = {TRAP_LIB,0,(void (*)(void))conFun};
static struct Hook conHook = {0,0,(HOOKFUNC)&conTrap};
#else
static struct Hook conHook = {0,0,(HOOKFUNC)&conFun};
#endif

/***************************************************************************/

#ifdef __MORPHOS__
static void
destFun(void)
{
    struct Hook 		          *hook = (struct Hook *)REG_A0;
    //Object      		            *obj = (Object *)REG_A2;
    struct MUIP_NListtree_DestructMessage *msg = (struct MUIP_NListtree_DestructMessage *)REG_A1;
#else
static void SAVEDS ASM
destFun(REG(a0,struct Hook *hook),REG(a2,Object *obj),REG(a1,struct MUIP_NListtree_DestructMessage *msg))
{
#endif
    struct data *data = hook->h_Data;

    freeFeed(data,data->pool,msg->UserData,TRUE);
}

#ifdef __MORPHOS__
static struct EmulLibEntry destTrap = {TRAP_LIB,0,(void (*)(void))destFun};
static struct Hook destHook = {0,0,(HOOKFUNC)&destTrap};
#else
static struct Hook destHook = {0,0,(HOOKFUNC)&destFun};
#endif

/***************************************************************************/

#ifdef __MORPHOS__
static void
dispFun(void)
{
    struct Hook 		         *hook = (struct Hook *)REG_A0;
    //Object      		           *obj = (Object *)REG_A2;
    struct MUIP_NListtree_DisplayMessage *msg = (struct MUIP_NListtree_DisplayMessage *)REG_A1;
#else
static void SAVEDS ASM
dispFun(REG(a0,struct Hook *hook),REG(a2,Object *obj),REG(a1,struct MUIP_NListtree_DisplayMessage *msg))
{
#endif
    struct data *data = hook->h_Data;
    struct feed *feed = msg->TreeNode->tn_User;
    STRPTR      name = feed->name ? feed->name : (STRPTR)"";
    APTR        im = NULL;

    *msg->Array = name;

    if (data->flags & FLG_UseFeedsListImages)
    {
        if (feed->picIm) im = feed->picIm->picIm;
        else if ((feed->flags & FFLG_Group) && (data->flags & FLG_UseDefaultGroupImage)) im = data->groupIm->picIm;
    }

    if (im)
    {
        if ((feed->flags & FFLG_News) && data->alertIm) msprintf(data->name,"\33O[%08lx] \33O[%08lx] %s",(ULONG)im,(ULONG)data->alertIm,(ULONG)name);
        else msprintf(data->name,"\33O[%08lx] %s",(ULONG)im,(ULONG)name);

        *msg->Array = data->name;
    }
    else
    {
        if ((feed->flags & FFLG_News) && data->alertIm)
        {
            msprintf(data->name,"\33O[%08lx] %s",(ULONG)data->alertIm,(ULONG)name);
            *msg->Array = data->name;
        }
        else *msg->Array = name;
    }

    if ((data->flags & FLG_BoldGroups) && (feed->flags & FFLG_Group)) *msg->Preparse = MUIX_B;
}

#ifdef __MORPHOS__
static struct EmulLibEntry dispTrap = {TRAP_LIB,0,(void (*)(void))dispFun};
static struct Hook dispHook = {0,0,(HOOKFUNC)&dispTrap};
#else
static struct Hook dispHook = {0,0,(HOOKFUNC)&dispFun};
#endif

/***************************************************************************/

#ifdef __MORPHOS__
static void
openFun(void)
{
    struct Hook    	              *hook = (struct Hook *)REG_A0;
    //Object   		                *obj = (Object *)REG_A2;
    struct MUIP_NListtree_OpenMessage *msg = (struct MUIP_NListtree_OpenMessage *)REG_A1;
#else
static void SAVEDS ASM
openFun(REG(a0,struct Hook *hook),REG(a2,Object *obj),REG(a1,struct MUIP_NListtree_OpenMessage *msg))
{
#endif
    struct data *data = hook->h_Data;
    struct feed *feed;

    if (msg->TreeNode && (feed = (struct feed *)msg->TreeNode->tn_User))
    {
	feed->flags |= FFLG_Opened;
    	data->flags |= FLG_SavePrefs;
    }
}

#ifdef __MORPHOS__
static struct EmulLibEntry openTrap = {TRAP_LIB,0,(void (*)(void))openFun};
static struct Hook openHook = {0,0,(HOOKFUNC)&openTrap};
#else
static struct Hook openHook = {0,0,(HOOKFUNC)&openFun};
#endif

/***************************************************************************/

#ifdef __MORPHOS__
static void
closeFun(void)
{
    struct Hook    	               *hook = (struct Hook *)REG_A0;
    //Object   		                 *obj = (Object *)REG_A2;
    struct MUIP_NListtree_CloseMessage *msg = (struct MUIP_NListtree_CloseMessage *)REG_A1;
#else
static void SAVEDS ASM
closeFun(REG(a0,struct Hook *hook),REG(a2,Object *obj),REG(a1,struct MUIP_NListtree_CloseMessage *msg))
{
#endif
    struct data *data = hook->h_Data;
    struct feed *feed;

    if (msg->TreeNode && (feed = (struct feed *)msg->TreeNode->tn_User))
    {
        feed->flags &= ~FFLG_Opened;
    	data->flags |= FLG_SavePrefs;
    }
}

#ifdef __MORPHOS__
static struct EmulLibEntry closeTrap = {TRAP_LIB,0,(void (*)(void))closeFun};
static struct Hook closeHook = {0,0,(HOOKFUNC)&closeTrap};
#else
static struct Hook closeHook = {0,0,(HOOKFUNC)&closeFun};
#endif

/***************************************************************************/

#ifdef __MORPHOS__
static LONG
compFun(void)
{
    //struct Hook    	                   *hook = (struct Hook *)REG_A0;
    //Object   		                   *obj = (Object *)REG_A2;
    struct MUIP_NListtree_CompareMessage *msg = (struct MUIP_NListtree_CompareMessage *)REG_A1;
#else
static LONG SAVEDS ASM
compFun(REG(a0,struct Hook *hook),REG(a2,Object *obj),REG(a1,struct MUIP_NListtree_CompareMessage *msg))
{
#endif
    return StrnCmp(g_loc,FEED(msg->TreeNode1->tn_User)->name,FEED(msg->TreeNode2->tn_User)->name,-1,SC_COLLATE2);
}

#ifdef __MORPHOS__
static struct EmulLibEntry compTrap = {TRAP_LIB,0,(void (*)(void))compFun};
static struct Hook compHook = {0,0,(HOOKFUNC)&compTrap};
#else
static struct Hook compHook = {0,0,(HOOKFUNC)&compFun};
#endif

/***************************************************************************/

static struct NewMenu menu[] =
{
    MTITLE(MSG_CMenu_Title_Feed),
        MITEM(MSG_CMenu_AddGroup,MSG_CMenu_AddGroup),
        MITEM(MSG_CMenu_AddFeed,MSG_CMenu_AddFeed),
        MITEM(MSG_CMenu_AddGroupHere,MSG_CMenu_AddGroupHere),
        MITEM(MSG_CMenu_AddFeedHere,MSG_CMenu_AddFeedHere),
        MBAR,
        MITEM(MSG_CMenu_Edit,MSG_CMenu_Edit),
        MITEM(MSG_CMenu_Remove,MSG_CMenu_Remove),
        MTITEM(MSG_CMenu_SetAsDefault,MSG_CMenu_SetAsDefault),
        MBAR,
        MITEM(MSG_CMenu_Feed,MSG_CMenu_Feed),
        MITEM(MSG_CMenu_Browser,MSG_CMenu_Browser),
        MITEM(MSG_CMenu_Link,MSG_CMenu_Link),
        MITEM(MSG_CMenu_Editor,MSG_CMenu_Editor),
        MITEM(MSG_CMenu_Validate,MSG_CMenu_Validate),
        MITEM(MSG_CMenu_DownloadFavIcon,MSG_CMenu_DownloadFavIcon),
        MBAR,
        MITEM(MSG_CMenu_Sort,MSG_CMenu_Sort),
        MITEM(MSG_CMenu_OpenAll,MSG_CMenu_OpenAll),
        MITEM(MSG_CMenu_CloseAll,MSG_CMenu_CloseAll),
        MBAR,
        MITEM(MSG_CMenu_Configure,MSG_CMenu_Configure),
    MEND
};

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct TagItem *attrs = msg->ops_AttrList;
    int            sig;

    if (((sig = AllocSignal(-1))>=0) &&
        (obj = (Object *)DoSuperNew(cl,obj,
            MUIA_ContextMenu,            TRUE,
            MUIA_NListtree_DupNodeName,  FALSE,
            TAG_MORE, (ULONG)attrs)))
    {
        struct data *data = INST_DATA(cl,obj);

        data->pool = (APTR)GetTagData(MUIA_RSS_Pool,NULL,attrs);
        data->this = obj;
        data->sig  = sig;

        QUICKNEWLIST(&data->images);
        QUICKNEWLIST(&data->feeds);
        InitSemaphore(&data->sem);

        if (GetTagData(MUIA_RSS_URLInArgs,FALSE,msg->ops_AttrList))
            data->flags |= FLG_URLInArgs;

        conHook.h_Data   = data;
        destHook.h_Data  = data;
        dispHook.h_Data  = data;
        openHook.h_Data  = data;
        closeHook.h_Data = data;
        compHook.h_Data  = data;

        SetSuperAttrs(cl,obj,MUIA_NListtree_ConstructHook, (ULONG)&conHook,
                             MUIA_NListtree_DestructHook,  (ULONG)&destHook,
                             MUIA_NListtree_DisplayHook,   (ULONG)&dispHook,
                             MUIA_NListtree_OpenHook,      (ULONG)&openHook,
                             MUIA_NListtree_CloseHook,     (ULONG)&closeHook,
                             MUIA_NListtree_CompareHook,   (ULONG)&compHook,
                             TAG_DONE);

        data->alertObj = LampObject, MUIA_Lamp_Color, MUIV_Lamp_Color_Warning, End;
        data->menu = MUI_MakeObject(MUIO_MenustripNM,(ULONG)menu,0);

        DoSuperMethod(cl,obj,MUIM_Notify,MUIA_NListtree_Active,MUIV_EveryTime,MUIV_Notify_Self,3,
            MUIM_GroupList_Feed,MUIV_GroupList_Feed_Local,MUIV_TriggerValue);
    }
    else if (sig>=0) FreeSignal(sig);

    return (ULONG)obj;
}

/***********************************************************************/

static void
addRemTimer(struct data *data,ULONG add)
{
    if (add)
    {
        if (!(data->flags & FLG_Timer) && !(data->flags & FLG_NoAutoUpdate))
        {
            data->th.ihn_Object = data->this;
            data->th.ihn_Method = MUIM_GroupList_HandleTimer;
            data->th.ihn_Flags  = MUIIHNF_TIMER;
            data->th.ihn_Millis = 60000;
            DoMethod(data->app,MUIM_Application_AddInputHandler,(ULONG)&data->th);

            data->flags |= FLG_Timer;
        }
    }
    else
    {
        if (data->flags & FLG_Timer)
        {
            DoMethod(data->app,MUIM_Application_RemInputHandler,(ULONG)&data->th);
            data->flags &= ~FLG_Timer;
        }
    }
}

/***********************************************************************/

static void
breakAll(struct data *data)
{
    register struct feed *feed, *succ;

    for (feed = FEED(data->feeds.mlh_Head); succ = FEED(feed->link.mln_Succ); feed = succ)
        breakProc(data,feed,TRUE);
}

/***********************************************************************/

static ULONG
mDispose(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);
    ULONG       res;
    int         sig = data->sig;

    data->flags |= FLG_Disposing;

    addRemTimer(data,FALSE);
    DoMethod(data->app,MUIM_Application_RemInputHandler,(ULONG)&data->ih);

    disposeImages(data);
    if (data->alertObj) MUI_DisposeObject(data->alertObj);

    data->flags |= FLG_SkipDelFile;
    DoSuperMethod(cl,obj,MUIM_NListtree_Clear,NULL,0);
    //breakAll(data);

    if (data->menu) MUI_DisposeObject(data->menu);

    //superset(cl,obj,MUIA_NListtree_DestructHook,NULL);

    res = DoSuperMethodA(cl,obj,msg);

    FreeSignal(sig);

    return res;
}

/***********************************************************************/

static ULONG
mSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct data             *data = INST_DATA(cl,obj);
    register struct TagItem *tag;
    struct TagItem 	    *tstate;

    for (tstate = msg->ops_AttrList; tag = NextTagItem(&tstate); )
    {
        register ULONG tidata = tag->ti_Data;

        switch(tag->ti_Tag)
        {
            case MUIA_RSS_Prefs:
            {
                struct prefs *prefs = (struct prefs *)tidata;
                ULONG        v, redraw = FALSE, feed = FALSE, scaleIms = FALSE;

                if (!BOOLSAME(data->flags & FLG_RightMouse,v = (prefs->flags & PFLG_RightMouse)))
                {
                    if (v) data->flags |= FLG_RightMouse;
                    else data->flags &= ~FLG_RightMouse;
                }

                if (!BOOLSAME(data->flags & FLG_BoldGroups,v = (prefs->flags & PFLG_BoldGroups)))
                {
                    if (v) data->flags |= FLG_BoldGroups;
                    else data->flags &= ~FLG_BoldGroups;

                    redraw = TRUE;
                }

                if (!BOOLSAME(data->flags & FLG_UseFeedsListImages,v = (prefs->flags & PFLG_UseFeedsListImages)))
                {
                    if (v)
                    {
                        data->flags |= FLG_UseFeedsListImages;
                        if (data->flags & FLG_Setup) setupImages(data);
                    }
                    else
                    {
                        data->flags &= ~FLG_UseFeedsListImages;
                        if (data->flags & FLG_Setup) cleanupImages(data);
                    }

                    redraw = TRUE;
                }

                if (!BOOLSAME(data->flags & FLG_UseDefaultGroupImage,v = (prefs->flags & PFLG_UseDefaultGroupImage)))
                {
                    if (v)
                    {
                        data->flags |= FLG_UseDefaultGroupImage;
                        data->groupIm = findImage(data,DEF_GROUPPIC);
                    }
                    else
                    {
                        data->flags &= ~FLG_UseDefaultGroupImage;
                        if (data->groupIm)
                        {
                            disposeImage(data,data->groupIm);
                            data->groupIm = NULL;
                        }
                    }

                    redraw = TRUE;
                }

                if (!BOOLSAME(data->flags & FLG_ShowGroupNews,v = (prefs->flags & PFLG_ShowGroupNews)))
                {
                    struct MUI_NListtree_TreeNode *active = SACTIVE(cl,obj);

                    if (v) data->flags |= FLG_ShowGroupNews;
                    else data->flags &= ~FLG_ShowGroupNews;

                    if (active && (active->tn_Flags & TNF_LIST)) feed = TRUE;
                }

                if (prefs->noDragDrop)
                {
                    set(obj,MUIA_NList_DragType,MUIV_NList_DragType_None);
                    data->flags |= FLG_NoDragDrop;
                }
                else
                {
                    set(obj,MUIA_NList_DragType,MUIV_NList_DragType_Default);
                    data->flags &= ~FLG_NoDragDrop;
                }

                if (data->GMTOffset!=prefs->GMTOffset)
                {
                    data->GMTOffset = prefs->GMTOffset;
                    feed = TRUE;
                }

                data->proxy       = prefs->proxy;
                data->proxyPort   = prefs->proxyPort;
                data->agent       = prefs->agent;
                data->doubleClick = prefs->groupDoubleClick;
                data->GMTOffset   = prefs->GMTOffset;

                if (prefs->httpFlags & PHFLG_UseProxy) data->flags |= FLG_UseProxy;
                else data->flags &= ~FLG_UseProxy;

                if (data->maxImageSize!=prefs->maxImageSize)
                {
                    data->maxImageSize = prefs->maxImageSize;
                    scaleIms = TRUE;
                }

                if (!BOOLSAME(data->flags & FLG_ScaleImages,v = (prefs->flags & PFLG_ScaleImages)))
                {
                    if (v) data->flags |= FLG_ScaleImages;
                    else data->flags &= ~FLG_ScaleImages;

                    scaleIms = TRUE;
                }

                if (!BOOLSAME(data->flags & FLG_SubstWinChars,v = (prefs->flags & PFLG_SubstWinChars)))
                {
                    if (v) data->flags |= FLG_SubstWinChars;
                    else data->flags &= ~FLG_SubstWinChars;
                }

                if (!BOOLSAME(data->flags & FLG_NoAutoUpdate,v = prefs->noAutoUpdate))
                {
                    if (v)
                    {
                        data->flags |= FLG_NoAutoUpdate;
                        data->flags &= ~(FLG_AutoUpdate|FLG_PushedUpdate);
                        addRemTimer(data,FALSE);
                    }
                    else
                    {
                        data->flags &= ~FLG_NoAutoUpdate;
                        if (!(data->flags & FLG_AutoUpdate)) addRemTimer(data,TRUE);
                    }
                }

                if (scaleIms)
                {
                    scaleImages(data);
                    redraw = TRUE;
                }

                if (redraw) DoSuperMethod(cl,obj,MUIM_NListtree_Redraw,MUIV_NListtree_Redraw_All,0);

                if ((data->flags & FLG_Setup) && feed)
                    DoMethod(obj,MUIM_GroupList_Feed,(ULONG)MUIV_GroupList_Feed_Local,(ULONG)MUIV_GroupList_Feed_Active);
            }
            break;

            case MUIA_RSS_Groups:
                data->group = (Object *)tidata;
                break;

            case MUIA_RSS_Entries:
                data->list = (Object *)tidata;
                break;

            case MUIA_RSS_NotifyPort:
                data->notifyPort = (struct MsgPort *)tidata;
                break;

            case MUIA_RSS_Codeset:
                data->codeset = (struct codeset *)tidata;
                break;

            case MUIA_RSS_App:
                data->app = (Object *)tidata;

                memset(&data->ih,0,sizeof(data->ih));
                data->ih.ihn_Object  = obj;
                data->ih.ihn_Signals = 1<<data->sig;
                data->ih.ihn_Method  = MUIM_GroupList_HandleEvent;
                DoMethod(data->app,MUIM_Application_AddInputHandler,(ULONG)&data->ih);

                addRemTimer(data,TRUE);

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
mSetup(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (!(DoSuperMethodA(cl,obj,msg))) return FALSE;

    data->flags |= FLG_Setup;

    if (data->flags & FLG_NoDragDrop) set(obj,MUIA_NList_DragType,MUIV_NList_DragType_None);
    else set(obj,MUIA_NList_DragType,MUIV_NList_DragType_Default);

    if (data->flags & FLG_UseFeedsListImages)
        setupImages(data);

    if (data->alertObj)
        data->alertIm = (APTR)DoSuperMethod(cl,obj,MUIM_NList_CreateImage,(ULONG)data->alertObj,0);

    return TRUE;
}

/***************************************************************************/

static ULONG
mCleanup(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    cleanupImages(data);

    if (data->alertIm)
    {
        DoSuperMethod(cl,obj,MUIM_NList_DeleteImage,(ULONG)data->alertIm);
        data->alertIm = NULL;
    }

    data->flags &= ~FLG_Setup;

    return DoSuperMethodA(cl,obj,msg);
}

/***************************************************************************/

static UBYTE rfc1738_unsafe_chars[] =
{
    (UBYTE)0x3C,        /* < */
    (UBYTE)0x3E,        /* > */
    (UBYTE)0x22,        /* " */
    (UBYTE)0x23,        /* # */
    (UBYTE)0x25,        /* % */
    (UBYTE)0x7B,        /* { */
    (UBYTE)0x7D,        /* } */
    (UBYTE)0x7C,        /* | */
    (UBYTE)0x5C,        /* \ */
    (UBYTE)0x5E,        /* ^ */
    (UBYTE)0x7E,        /* ~ */
    (UBYTE)0x5B,        /* [ */
    (UBYTE)0x5D,        /* ] */
    (UBYTE)0x60,        /* ` */
    (UBYTE)0x27,        /* ' */
    (UBYTE)0x20         /* space */
};

static void
rfc1738Escape(STRPTR from,STRPTR to)
{
    register STRPTR p, q;

    for (p = from, q = to; *p; p++, q++)
    {
        register int   i;
        register UBYTE c = *p;

        /* RFC 1738 defines these chars as unsafe */
        for (i = 0; i<sizeof(rfc1738_unsafe_chars); i++)
            if (c==rfc1738_unsafe_chars[i]) goto escape;

        /* RFC 1738 says any control chars (0x00-0x1F) are encoded */
        if (c<=0x1F) goto escape;

        /* RFC 1738 says 0x7f is encoded */
        if (c==0x7F)  goto escape;

        /* RFC 1738 says any non-US-ASCII are encoded */
        if (c>=0x80) goto escape;

        *q = c;
        continue;

escape:
        msprintf(q,"%%%02lx",c);
        q += sizeof(UBYTE)*2;
    }

    *q = '\0';
}

/****************************************************************************/

static ULONG
mContextMenuChoice(struct IClass *cl,Object *obj,struct MUIP_ContextMenuChoice *msg)
{
    struct data                   *data = INST_DATA(cl,obj);
    struct MUI_NListtree_TreeNode *active = SACTIVE(cl,obj);

    switch (muiUserData(msg->item))
    {
        case MSG_CMenu_AddGroup:
            DoMethod(obj,MUIM_GroupList_EditFeed,MUIV_GroupList_EditFeed_New|MUIV_GroupList_EditFeed_Group);
            break;

        case MSG_CMenu_AddGroupHere:
            DoMethod(obj,MUIM_GroupList_EditFeed,MUIV_GroupList_EditFeed_New|MUIV_GroupList_EditFeed_Group|MUIV_GroupList_EditFeed_Sub);
            break;

        case MSG_CMenu_AddFeed:
            DoMethod(obj,MUIM_GroupList_EditFeed,MUIV_GroupList_EditFeed_New);
            break;

        case MSG_CMenu_AddFeedHere:
            DoMethod(obj,MUIM_GroupList_EditFeed,MUIV_GroupList_EditFeed_New|MUIV_GroupList_EditFeed_Sub);
            break;

        case MSG_CMenu_Edit:
            DoMethod(obj,MUIM_GroupList_EditFeed,0);
            break;

        case MSG_CMenu_Remove:
            DoMethod(obj,MUIM_GroupList_Remove);
            break;

        case MSG_CMenu_Browser:
            if (active) goURLFun(FEED(active->tn_User)->info.link);
            break;

        case MSG_CMenu_Link:
        {
            STRPTR link = NULL;

            if (active)
            {
                struct feed *feed = active->tn_User;

                link = feed->info.link;
            }

            if (link) DoMethod(data->app,MUIM_RSS_GotoURL,(ULONG)link,MUIV_RSS_GotoURL_IgnorePrefs|MUIV_RSS_GotoURL_NoTarget);
            break;
        }

        case MSG_CMenu_Feed:
            DoMethod(obj,MUIM_GroupList_Feed,(ULONG)MUIV_GroupList_Feed_Remote,(ULONG)MUIV_GroupList_Feed_Active);
            break;

        case MSG_CMenu_SetAsDefault:
            data->def = (data->def==active) ? NULL : active;
            data->flags |= FLG_SavePrefs;
            break;

        case MSG_CMenu_Editor:
            if (active)
            {
                struct feed *feed = active->tn_User;

                if (feed->file) DoMethod(data->group,MUIM_MainGroup_Editor,(ULONG)feed->file);
            }
            break;

        case MSG_CMenu_Validate:
            if (active)
            {
                struct feed *feed = active->tn_User;

                if (feed->URL)
                {
                    UBYTE  buf[2048];
                    STRPTR URL;
                    ULONG  l = (strlen(feed->URL)+64)*3, freeURL = FALSE;

                    if (l>sizeof(buf))
                    {
                        if (URL = allocVecPooled(data->pool,l)) freeURL = TRUE;
                    }
                    else URL = buf;

                    if (URL)
                    {
                        STRPTR s = URL;

                        if (!strchr(feed->URL,':'))
                        {
                            strcpy(URL,"http://feedvalidator.org/check.cgi?url=http://");
                            s += 46;
                        }
                        else
                        {
                            msprintf(URL,"http://feedvalidator.org/check.cgi?url=");
                            s += 39;
                        }

                        rfc1738Escape(feed->URL,s);

                        DoMethod(data->group,MUIM_MainGroup_UserURL,(ULONG)URL,FALSE);

                        if (freeURL) freeVecPooled(data->pool,URL);
                    }
                }

            }
            break;

        case MSG_CMenu_Sort:
            superset(cl,obj,MUIA_NListtree_Quiet,TRUE);
            DoSuperMethod(cl,obj,MUIM_NListtree_Sort,MUIV_NListtree_Sort_ListNode_Active,0);
            SetSuperAttrs(cl,obj,MUIA_NListtree_Active,(ULONG)active,MUIA_NoNotify,TRUE,MUIA_NListtree_Quiet,FALSE,TAG_DONE);
            data->flags |= FLG_SavePrefs;
            break;

        case MSG_CMenu_OpenAll:
            DoSuperMethod(cl,obj,MUIM_NListtree_Open,MUIV_NListtree_Open_ListNode_Root,MUIV_NListtree_Open_TreeNode_All,0);
            if (active) superset(cl,obj,MUIA_NListtree_Active,active);
            break;

        case MSG_CMenu_CloseAll:
            superset(cl,obj,MUIA_NListtree_Active,MUIV_NListtree_Active_Off);
            DoSuperMethod(cl,obj,MUIM_NListtree_Close,MUIV_NListtree_Close_ListNode_Root,MUIV_NListtree_Close_TreeNode_All,0);
            break;

        case MSG_CMenu_Configure:
            DoMethod(data->app,MUIM_App_PrefsWin,MUIV_Prefs_Page_Options);
            break;

        case MSG_CMenu_DownloadFavIcon:
            DoMethod(obj,MUIM_GroupList_DownloadFavIcon,(ULONG)active);
            data->flags |= FLG_SavePrefs;
            break;
    }

    return 0;
}

/***********************************************************************/

static ULONG
mContextMenuBuild(struct IClass *cl,Object *obj,struct MUIP_ContextMenuBuild *msg)
{
    struct data                   	   *data = INST_DATA(cl,obj);
    register struct MUI_NListtree_TreeNode *tn = NULL;
    struct MUI_NListtree_TreeNode 	   *active = SACTIVE(cl,obj);

    /*if (data->flags & FLG_RightMouse)
    {
        struct MUI_NListtree_TestPos_Result tp;

        DoSuperMethod(cl,obj,MUIM_NListtree_TestPos,msg->mx,msg->my,&tp);
        if (tp.tpr_TreeNode)
        {
            struct MUI_NListtree_TreeNode *tn;

            tn = active;
            if (tn!=tp.tpr_TreeNode)
                SetSuperAttrs(cl,obj,MUIA_NListtree_Active,tp.tpr_TreeNode,TAG_DONE);
        }
    }*/

    if (data->menu)
    {
        ULONG link = FALSE, xlink = FALSE, group = TRUE, file = FALSE, act = FALSE;

        if (!tn) tn = active;

        if (tn)
        {
            struct feed *feed = tn->tn_User;

            group  = feed->flags & FFLG_Group;
            link   = (ULONG)feed->info.link;
            xlink  = (ULONG)feed->URL;
            file   = (ULONG)feed->file;
            act    = data->def==tn;
        }

        set((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_AddGroupHere),MUIA_Menuitem_Enabled,tn);
        set((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_AddFeedHere),MUIA_Menuitem_Enabled,tn);
        set((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_Edit),MUIA_Menuitem_Enabled,tn);
        set((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_Remove),MUIA_Menuitem_Enabled,tn);
        set((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_Sort),MUIA_Menuitem_Enabled,tn);
        set((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_Browser),MUIA_Menuitem_Enabled,link);
        set((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_Link),MUIA_Menuitem_Enabled,link);
        set((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_Feed),MUIA_Menuitem_Enabled,!group);
        set((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_SetAsDefault),MUIA_Menuitem_Enabled,!group);
        set((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_SetAsDefault),MUIA_Menuitem_Checked,act);
        set((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_Editor),MUIA_Menuitem_Enabled,!group && file);
        set((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_Validate),MUIA_Menuitem_Enabled,xlink);
        set((Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_DownloadFavIcon),MUIA_Menuitem_Enabled,!group && xlink);

        return (ULONG)data->menu;
    }

    return NULL;
}

/***********************************************************************/

static INLINE LONG
putPre(BPTR file,int pre)
{
    register int p;

    for (p = pre<<2; p--;)
        if (FPutC(file,' ')==EOF) return EOF;

    return 0;
}

static INLINE LONG
putChar(BPTR file,int c)
{
    return (c<128 && c!='%') ? FPutC(file,c) : FPrintf(file,"%%%02lx",c);
}

static LONG
putElement(BPTR file,STRPTR element,STRPTR str,int pre)
{
    register STRPTR c;

    if (putPre(file,pre)==EOF) return EOF;

    if (FPrintf(file,"<%s>",(ULONG)element)==EOF) return EOF;

    for (c = str;*c; c++)
        if (putChar(file,*c)==EOF) return EOF;

    return FPrintf(file,"</%s>\n",(ULONG)element);
}

static LONG INLINE
saveFeed(BPTR file,struct feed *feed,ULONG def,int pre)
{
    if (putPre(file,pre)==EOF) return EOF;

    if (!(feed->flags & FFLG_ShowLink) && !def && !feed->updateMin)
    {
        if (FPuts(file,"<feed>\n")==EOF) return EOF;
    }
    else
    {
        if (FPuts(file,"<feed")==EOF) return EOF;
        if (def && (FPuts(file," active=\"1\"")==EOF)) return EOF;
        if ((feed->flags & FFLG_ShowLink) && (FPuts(file," showLink=\"1\"")==EOF)) return EOF;
        if ((feed->updateMin) && (FPrintf(file," updateMin=\"%ld\"",feed->updateMin)==EOF)) return EOF;
        if (FPuts(file,">\n")==EOF) return EOF;
    }

    if (feed->name && (putElement(file,"name",feed->name,pre+1)==EOF)) return EOF;
    if (feed->pic  && (putElement(file,"pic",feed->pic,pre+1)==EOF))   return EOF;
    if (feed->URL  && (putElement(file,"url",feed->URL,pre+1)==EOF))   return EOF;
    if (feed->file && (putElement(file,"file",feed->file,pre+1)==EOF)) return EOF;

    if (putPre(file,pre)==EOF) return EOF;

    return FPrintf(file,"</feed>\n");
}

static LONG
saveGroup(BPTR file,struct IClass *cl,Object *obj,struct MUI_NListtree_TreeNode *list,struct MUI_NListtree_TreeNode *def,int pre)
{
    register struct MUI_NListtree_TreeNode *tn;
    register struct feed                   *feed;

    feed = list->tn_User;

    if (putPre(file,pre)==EOF) return EOF;

    if (FPrintf(file,"<group")==EOF) return EOF;

    if (FPrintf(file,(feed->flags & FFLG_Opened) ? " open=\"1\">\n" : ">\n")==EOF) return EOF;

    if (feed->name && (putElement(file,"name",feed->name,pre+1)==EOF)) return EOF;

    if (feed->pic && (putElement(file,"pic",feed->pic,pre+1)==EOF)) return EOF;

    tn = (struct MUI_NListtree_TreeNode *)DoSuperMethod(cl,obj,MUIM_NListtree_GetEntry,(ULONG)list,MUIV_NListtree_GetEntry_Position_Head,0);
    while (tn)
    {
        if (((tn->tn_Flags & TNF_LIST) ? saveGroup(file,cl,obj,tn,def,pre+1) : saveFeed(file,tn->tn_User,def==tn,pre+1))==EOF)
            return EOF;

        tn = (struct MUI_NListtree_TreeNode *)DoMethod(obj,MUIM_NListtree_GetEntry,(ULONG)tn,MUIV_NListtree_GetEntry_Position_Next,MUIV_NListtree_GetEntry_Flag_SameLevel);
    }

    if (putPre(file,pre)==EOF) return EOF;

    if (FPrintf(file,"</group>\n")==EOF) return EOF;

    return 0;
}

static ULONG
mSaveFeeds(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);
    BPTR        file;
    ULONG       res = FALSE;

    if (!(data->flags & FLG_SavePrefs)) return 0;

    if (file = Open(DEF_FEEDS".tmp",MODE_NEWFILE))
    {
        struct MUI_NListtree_TreeNode *tn;

        if (FPrintf(file,"<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<rssfeeds version=\"1.0\">\n")==EOF)
            goto fail;

        tn = (struct MUI_NListtree_TreeNode *)DoMethod(obj,MUIM_NListtree_GetEntry,MUIV_NListtree_GetEntry_ListNode_Root,MUIV_NListtree_GetEntry_Position_Head,0);
        while (tn)
        {

            if (((tn->tn_Flags & TNF_LIST) ? saveGroup(file,cl,obj,tn,data->def,1) : saveFeed(file,tn->tn_User,data->def==tn,1))==EOF)
                goto fail;

            tn = (struct MUI_NListtree_TreeNode *)DoMethod(obj,MUIM_NListtree_GetEntry,(ULONG)tn,MUIV_NListtree_GetEntry_Position_Next,0);
        }

        if (FPrintf(file,"</rssfeeds>\n")==EOF) goto fail;
        res = TRUE;

fail:
        Close(file);

        if (res)
        {
            DeleteFile(DEF_FEEDS);
            if (Rename(DEF_FEEDS".tmp",DEF_FEEDS)) res = MUIV_GroupList_SaveFeeds_OK;
            else res = MUIV_GroupList_SaveFeeds_Rename;
        }
        else
        {
            DeleteFile(DEF_FEEDS".tmp");
            res = MUIV_GroupList_SaveFeeds_Failure;
        }

    	data->flags &= ~FLG_SavePrefs;
    }
    else res = MUIV_GroupList_SaveFeeds_Failure;

    return res;
}

/***************************************************************************/

struct lfdata
{
    struct IClass 		  *cl;
    Object        		  *obj;
    struct data   		  *data;
    APTR          		  *pool;
    struct MUI_NListtree_TreeNode *activeGroup;
    struct MUI_NListtree_TreeNode *def;
    struct feed   		  *feed;
    struct feed   		  *group;
    STRPTR        		  curr;
    UBYTE         		  buf[256];
    ULONG         		  clen;
    ULONG         		  groups;
    ULONG         		  flags;
};

enum
{
    LFFLG_Group     = 1<<0,
    LFFLG_Feed      = 1<<1,
    LFFLG_Valid     = 1<<2,
    LFFLG_Allocated = 1<<3,
    LFFLG_Active    = 1<<4,
};

/***********************************************************************/

enum
{
    LFT_GROUP = 1,
    LFT_FEED,
    LFT_NAME,
    LFT_URL,
    LFT_FILE,
    LFT_DATE,
    LFT_PIC,
};

struct item
{
    STRPTR name;
    int    id;
};

static struct item lfitems[] =
{
    "date",      LFT_DATE,
    "feed",      LFT_FEED,
    "file",      LFT_FILE,
    "group",     LFT_GROUP,
    "name",      LFT_NAME,
    "pic",       LFT_PIC,
    "url",       LFT_URL,
};

static INLINE int
findLFItem(STRPTR name)
{
    register int low, high;

    for (low = 0, high = (sizeof(lfitems)/sizeof(struct item))-1; low<=high; )
    {
        register int         mid = (low+high)>>1, cond;
        register struct item *item = lfitems+mid;

        if (!(cond = stricmp(name,item->name))) return item->id;

        if (cond<0) high = mid-1;
        else low = mid+1;
    }

    return 0;
}

/***********************************************************************/

static void
#ifndef __MORPHOS__
SAVEDS STDARGS
#endif
xlfstart(void *data,const XML_Char *name,const XML_Char **atts)
{
    register struct lfdata *lfdata = data;
    register int           id;

    if (!(id = findLFItem((STRPTR)name))) return;

    switch (id)
    {
        case LFT_GROUP:
            if (lfdata->group = AllocPooled(lfdata->pool,sizeof(struct feed)))
            {
                struct MUI_NListtree_TreeNode *tn;
                ULONG                         flags = TNF_LIST;

                memset(lfdata->group,0,sizeof(struct feed));

                if (atts[0] && !stricmp(atts[0],"open") && atts[1] && !strcmp(atts[1],"1"))
                {
                    lfdata->group->flags |= FFLG_Opened;
                    flags |= TNF_OPEN;
                }

                lfdata->group->flags |= FFLG_Group;

                if (tn = (struct MUI_NListtree_TreeNode *)DoSuperMethod(lfdata->cl,lfdata->obj,MUIM_NListtree_Insert,
                		                                        (ULONG)"M",
                                                                        (ULONG)lfdata->group,
                                                                        (ULONG)lfdata->activeGroup,
                                                                        MUIV_NListtree_Insert_PrevNode_Tail,
                                                                        flags))
                {
                    INITPORT(&lfdata->group->port,lfdata->data->sig);
                    INITPORT(&lfdata->group->resPort,lfdata->data->sig);
                    InitSemaphore(&lfdata->group->sem);
                    lfdata->group->tn = tn;

                    lfdata->activeGroup = tn;
                    lfdata->groups++;
                }
                else
                {
                    freeFeed(lfdata->data,lfdata->pool,lfdata->group,FALSE);
                    lfdata->group = NULL;
            	}
            }
            break;

        case LFT_FEED:
            if (lfdata->feed = AllocPooled(lfdata->pool,sizeof(struct feed)))
            {
            	STRPTR *args;
                int    i;

                memset(lfdata->feed,0,sizeof(struct feed));

                for (args = (STRPTR *)atts, i = 0; args[i]; i += 2)
                {
                    if (!stricmp(args[i],"active") && args[i+1] && !strcmp(args[i+1],"1"))
                    {
                        lfdata->flags |= LFFLG_Active;
                        continue;
                    }

                    if (!stricmp(args[i],"showLink") && args[i+1] && !strcmp(args[i+1],"1"))
                    {
                        lfdata->feed->flags = FFLG_ShowLink;
                        continue;
                    }

                    if (!stricmp(args[i],"updateMin") && args[i+1])
                    {
                        LONG v;

            		StrToLong(args[i+1],&v);

                        if (v>=10 && v<=300) lfdata->feed->updateMin = v;
                        continue;
                    }
                }

                INITPORT(&lfdata->feed->port,lfdata->data->sig);
                INITPORT(&lfdata->feed->resPort,lfdata->data->sig);
                InitSemaphore(&lfdata->feed->sem);
            	lfdata->flags |= LFFLG_Feed;
            }
            break;

        default:
            if ((lfdata->flags & LFFLG_Feed) || (lfdata->groups && ((id==LFT_NAME) || (id==LFT_PIC))))
                lfdata->flags |= LFFLG_Valid;
            break;
    }
}

/***********************************************************************/

static void
#ifndef __MORPHOS__
SAVEDS STDARGS
#endif
xlfend(void *data,const XML_Char *name)
{
    register struct lfdata *lfdata = data;
    register int           id;

    if (!(lfdata->flags & LFFLG_Feed) && !lfdata->groups) return;
    if (!(id = findLFItem((STRPTR)name))) return;

    if (lfdata->flags & LFFLG_Feed)
    {
        switch (id)
        {
            case LFT_FEED:
            {
                struct MUI_NListtree_TreeNode *tn;

                makeImage(lfdata->data,lfdata->obj,lfdata->feed);

                if (tn = (struct MUI_NListtree_TreeNode *)DoSuperMethod(lfdata->cl,lfdata->obj,MUIM_NListtree_Insert,
                                            				(ULONG)"M",
                                                                        (ULONG)lfdata->feed,
                                                                        (ULONG)lfdata->activeGroup,
                                                                        MUIV_NListtree_Insert_PrevNode_Tail,
                                                                        0))
                {
                    lfdata->feed->tn = tn;
                    if (lfdata->flags & LFFLG_Active) lfdata->def = tn;
                }
                else freeFeed(lfdata->data,lfdata->pool,lfdata->feed,FALSE);

                lfdata->feed = NULL;
                lfdata->flags &= ~(LFFLG_Feed|LFFLG_Active);
                break;
            }

            default:
            {
                STRPTR value;

                if (lfdata->curr && (lfdata->curr==lfdata->buf) && (id!=LFT_DATE))
                {
                    STRPTR s;

                    if (s = allocVecPooled(lfdata->pool,lfdata->clen+1)) strcpy(s,lfdata->curr);
                    lfdata->curr = s;
                }

		value = lfdata->curr;

                switch (id)
                {
                    case LFT_NAME:
                        lfdata->feed->name = value;
                        break;

                    case LFT_PIC:
                        lfdata->feed->pic = value;
                        break;

                    case LFT_URL:
                        lfdata->feed->URL = value;
                        break;

                    case LFT_FILE:
                        lfdata->feed->file = value;
                        break;

                    case LFT_DATE:
                        if (value)
                        {
                            getdate(&lfdata->feed->info.date,value,lfdata->data->GMTOffset);
                            if (value!=lfdata->buf) freeVecPooled(lfdata->pool,value);
                        }
                        break;
                }

                lfdata->flags &= ~LFFLG_Valid;

                break;
            }
        }
    }
    else
    {
        switch (id)
        {
            case LFT_GROUP:
                lfdata->activeGroup = (struct MUI_NListtree_TreeNode *)DoSuperMethod(lfdata->cl,lfdata->obj,MUIM_NListtree_GetEntry,
                                                     				     (ULONG)lfdata->activeGroup,
                                                                                     MUIV_NListtree_GetEntry_Position_Parent,
                                                                                     0);
                lfdata->group = NULL;
                lfdata->groups--;
                break;

            default:
                lfdata->flags &= ~LFFLG_Valid;

                if (lfdata->curr && (lfdata->curr==lfdata->buf))
                {
                    STRPTR s;

                    if (s = allocVecPooled(lfdata->pool,lfdata->clen+1)) strcpy(s,lfdata->curr);
                    lfdata->curr = s;
                }

                switch (id)
                {
                    case LFT_NAME:
                        lfdata->group->name = lfdata->curr;
		        parseString(lfdata->group->name);
                        break;

                    case LFT_PIC:
                        lfdata->group->pic = lfdata->curr;
		        parseString(lfdata->group->pic);
                        if (makeImage(lfdata->data,lfdata->obj,lfdata->group))
                            DoSuperMethod(lfdata->cl,lfdata->obj,MUIM_NListtree_Redraw,MUIV_NListtree_Redraw_All,0);
                        break;
                }
                break;
        }
    }

    lfdata->flags &= ~LFFLG_Allocated;
    lfdata->clen   = 0;
    lfdata->curr   = NULL;
}

/***********************************************************************/

static void
#ifndef __MORPHOS__
SAVEDS STDARGS
#endif
xlfdata(void *data,const XML_Char *s,int len)
{
    register struct lfdata *lfdata = data;
    register STRPTR        o, n;
    register int           l, lo, cl;

    if (!(lfdata->flags & LFFLG_Valid)) return;

    if (o = lfdata->curr)
    {
        lo = strlen(o);
        l = lo+len+1;
    }
    else
    {
        lo = 0;
        l = len+1;
    }

    cl = lfdata->clen;

    if (!(lfdata->flags & LFFLG_Allocated) && (cl+len<sizeof(lfdata->buf)))
    {
        copymem(lfdata->buf+cl,s,len);
        lfdata->clen += len;
        lfdata->buf[lfdata->clen] = 0;
        lfdata->curr = lfdata->buf;
    }
    else
    {
        if (n = allocVecPooled(lfdata->pool,l))
        {
            STRPTR p = n;

            if (o)
            {
                strcpy(p,o);
                if (o!=lfdata->buf) freeVecPooled(lfdata->pool,o);
                p += lo;
    	    }

            copymem(p,s,len);
            p[len] = 0;

            lfdata->curr = n;
            lfdata->flags |= LFFLG_Allocated;
        }
    }
}

/***********************************************************************/

static ULONG
mLoadFeeds(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);
    BPTR        file;

    DoMethod(data->group,MUIM_MainGroup_SetInfo,0,MSG_Status_ReadingFeeds);

    if (file = openFile("PROGDIR:etc/db/feeds",MODE_OLDFILE))
    {
        XML_Parser parser;

        superset(cl,obj,MUIA_NListtree_Quiet,TRUE);
        data->flags |= FLG_SkipDelFile;
        DoSuperMethod(cl,obj,MUIM_NListtree_Clear,NULL,0);
        data->flags &= ~FLG_SkipDelFile;
        if (data->flags & FLG_Setup) set(data->app,MUIA_Application_Sleep,TRUE);

        if (parser = XML_ParserCreate(NULL))
        {
            struct lfdata lfdata;
            UBYTE 	  buf[1024];

            memset(&lfdata,0,sizeof(lfdata));
            lfdata.cl          = cl;
            lfdata.obj         = obj;
            lfdata.data        = data;
            lfdata.pool        = data->pool;
            lfdata.activeGroup = MUIV_NListtree_Insert_ListNode_Root;

            XML_SetUserData(parser,&lfdata);
            XML_SetElementHandler(parser,xlfstart,xlfend);
            XML_SetCharacterDataHandler(parser,xlfdata);

            for (;;)
            {
                register int len = readFile(file,buf,sizeof(buf));

                if (len<=0) break;

                if (XML_Parse(parser,buf,len,0)==XML_STATUS_ERROR)
                    break;
            }

            XML_Parse(parser,"",0,1);
            XML_ParserFree(parser);

            if (lfdata.feed) freeFeed(data,data->pool,lfdata.feed,FALSE);

            data->def = lfdata.def;
        }

        closeFile(file);

        superset(cl,obj,MUIA_NListtree_Quiet,FALSE);
        if (data->flags & FLG_Setup) set(data->app,MUIA_Application_Sleep,FALSE);

        /* XXX - NListtree bug */
        if ((data->flags & FLG_UseFeedsListImages) && (data->flags & FLG_Setup))
            MUI_Redraw(obj,MADF_DRAWOBJECT);

    	data->flags &= ~FLG_SavePrefs;
    }

    DoMethod(data->group,MUIM_MainGroup_SetInfo,0,MSG_Group_Welcome);

    return 0;
}

/***************************************************************************/

static ULONG
mSetDefaultFeed(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (data->def)
    {
        register struct MUI_NListtree_TreeNode *tn;

        for (tn = data->def; ;)
        {
            tn = (struct MUI_NListtree_TreeNode *)DoMethod(obj,MUIM_NListtree_GetEntry,(ULONG)tn,MUIV_NListtree_GetEntry_Position_Parent,0);
            if (!tn) break;
            DoSuperMethod(cl,obj,MUIM_NListtree_Open,MUIV_NListtree_Open_ListNode_Parent,(ULONG)tn,0);
        }

        superset(cl,obj,MUIA_NListtree_Active,data->def);
    }

    return 0;
}

/***********************************************************************/

static ULONG
mAddFeed(struct IClass *cl,Object *obj,struct MUIP_GroupList_AddFeed *msg)
{
    struct data                   *data = INST_DATA(cl,obj);
    struct MUI_NListtree_TreeNode *tn = NULL; //gcc
    struct feed                   *feed;

    if (!msg->URL || !*msg->URL || !msg->name || !*msg->name) return 0;

    if (feed = AllocPooled(data->pool,sizeof(struct feed)))
    {
	memset(feed,0,sizeof(struct feed));

        feed->flags = FFLG_SkipUTF8;

        if (feed->URL = allocVecPooled(data->pool,strlen(msg->URL)+1))
        {
            strcpy(feed->URL,msg->URL);

            if (feed->name = allocVecPooled(data->pool,strlen(msg->name)+1))
            {
                strcpy(feed->name,msg->name);

                if (msg->pic && *msg->pic && (feed->pic = allocVecPooled(data->pool,strlen(msg->pic)+1)))
                   strcpy(feed->pic,msg->pic);

                tn = (struct MUI_NListtree_TreeNode *)DoSuperMethod(cl,obj,MUIM_NListtree_Insert,
                                            (ULONG)"M",
                                                                    (ULONG)feed,
                                                                    MUIV_NListtree_Insert_ListNode_Root,
                                                                    MUIV_NListtree_Insert_PrevNode_Tail,
                                                                    MUIV_NListtree_Insert_Flag_Active);
            }
        }

        if (!tn) freeFeed(data,data->pool,feed,FALSE);
    }

    return (ULONG)tn;
}

/***********************************************************************/

static ULONG
mEditFeed(struct IClass *cl,Object *obj,struct MUIP_GroupList_EditFeed *msg)
{
    struct data                   *data = INST_DATA(cl,obj);
    struct MUI_NListtree_TreeNode *list, *tn;
    struct feed                   *feed;
    struct MUI_NListtree_TreeNode *active = SACTIVE(cl,obj);

    list = tn = NULL;

    if (msg->flags & MUIV_GroupList_EditFeed_New)
    {
        struct MUI_NListtree_TreeNode *a;

        a = active;

        if (!(feed = AllocPooled(data->pool,sizeof(struct feed))))
            return 0;
	
        memset(feed,0,sizeof(struct feed));

        feed->flags = FFLG_SkipUTF8;

        if (msg->flags & MUIV_GroupList_EditFeed_Group) feed->flags |= FFLG_Group;

        if ((msg->flags & MUIV_GroupList_EditFeed_Sub) && a)
        {
            if (a->tn_Flags & TNF_LIST) list = a;
            else list = (struct MUI_NListtree_TreeNode *)DoSuperMethod(cl,obj,MUIM_NListtree_GetEntry,(ULONG)a,MUIV_NListtree_GetEntry_Position_Parent,0);
        }
    }
    else
    {
        tn = active;

        if (tn)
        {
            feed = tn->tn_User;
        }
        else return 0;
    }

    if (!feed->ewin)
    {
        ULONG ID = ('W'<<24) | ('E'<<16) | (data->editID & 0xFF);

        if ((editFeedClass || initEditFeedClass()) &&
            (feed->ewin = editFeedObject,
                    MUIA_Window_ID,          ID,
                    MUIA_Window_RefWindow,   data->win,
                MUIA_Window_ScreenTitle, xget(data->app,MUIA_App_ScreenTitle),
                    MUIA_EditFeed_ListObj,   obj,
                    MUIA_EditFeed_List,      list,
                    MUIA_EditFeed_Node,      tn,
                    MUIA_EditFeed_Feed,      feed,
                    End))
        {
            DoMethod(data->app,OM_ADDMEMBER,(ULONG)feed->ewin);
            data->editID++;
        }
        else
        {
            if (!tn)
            {
                freeFeed(data,data->pool,feed,FALSE);
                feed = NULL;
            }
        }
    }

    if (feed && feed->ewin)
        openWindow(data->app,feed->ewin);

    return 0;
}

/***********************************************************************/

static ULONG
mUpdateFeed(struct IClass *cl,Object *obj,struct MUIP_GroupList_UpdateFeed *msg)
{
    struct data          	  *data = INST_DATA(cl,obj);
    struct MUI_NListtree_TreeNode *list = msg->list, *tn = msg->node;
    struct feed          	  *feed = msg->feed;

    feed->ewin = NULL;
    data->editID--;

    if (tn)
    {
        if (msg->success)
        {
            data->flags |= FLG_SavePrefs;

            if (msg->name)
            {
                STRPTR s;

                if (s = allocVecPooled(data->pool,strlen(msg->name)+1))
                {
                    strcpy(s,msg->name);
                    if (feed->name) freeVecPooled(data->pool,feed->name);
                    feed->name = s;
                }
            }

            if (msg->URL)
            {
                STRPTR s;

                if (s = allocVecPooled(data->pool,strlen(msg->URL)+1))
                {
                    strcpy(s,msg->URL);
                    if (feed->URL) freeVecPooled(data->pool,feed->URL);
                    feed->URL = s;
                }
            }

            if (msg->pic)
            {
                STRPTR s;

                if (s = allocVecPooled(data->pool,strlen(msg->pic)+1))
                {
                    strcpy(s,msg->pic);
                    if (feed->pic) freeVecPooled(data->pool,feed->pic);
                    feed->pic = s;

                    makeImage(data,obj,feed);
                }
            }

            feed->updateMin = msg->updateMin;

            DoSuperMethod(cl,obj,MUIM_NListtree_Redraw,(ULONG)tn,0);

            if (!BOOLSAME(msg->flags & MUIV_GroupList_UpdateFeed_ShowLink,feed->flags & FFLG_ShowLink))
            {
                if (msg->flags & MUIV_GroupList_UpdateFeed_ShowLink) feed->flags |= FFLG_ShowLink;
                else feed->flags &= ~FFLG_ShowLink;

                DoMethod(data->group,MUIM_MainGroup_EntryChange);
            }
        }
    }
    else
    {
        if (msg->success)
        {
            if (msg->name && *msg->name)
            {
                if (feed->name = allocVecPooled(data->pool,strlen(msg->name)+1))
                {
                    strcpy(feed->name,msg->name);
                }
                else
                {
                    freeFeed(data,data->pool,feed,FALSE);
                    return 0;
                }
            }

            if (msg->URL && *msg->URL)
            {
                if (feed->URL = allocVecPooled(data->pool,strlen(msg->URL)+1))
                {
                    strcpy(feed->URL,msg->URL);
                }
                else
                {
                    freeFeed(data,data->pool,feed,FALSE);
                    return 0;
                }
            }

            if (msg->pic && *msg->pic)
            {
                if (feed->pic = allocVecPooled(data->pool,strlen(msg->pic)+1))
                    strcpy(feed->pic,msg->pic);

                makeImage(data,obj,feed);
            }

            feed->updateMin = msg->updateMin;

            if (msg->flags & MUIV_GroupList_UpdateFeed_ShowLink) feed->flags |= FFLG_ShowLink;

            INITPORT(&feed->port,data->sig);
            INITPORT(&feed->resPort,data->sig);
            InitSemaphore(&feed->sem);

            if (!(feed->tn = (struct MUI_NListtree_TreeNode *)DoSuperMethod(cl,obj,MUIM_NListtree_Insert,(ULONG)"M",(ULONG)feed,list ? (ULONG)list : (ULONG)MUIV_NListtree_Insert_ListNode_Root,MUIV_NListtree_Insert_PrevNode_Tail,MUIV_NListtree_Insert_Flag_Active|((feed->flags & FFLG_Group) ? TNF_LIST|TNF_OPEN : 0))))
                freeFeed(data,data->pool,feed,FALSE);
        }
        else
        {
            freeFeed(data,data->pool,feed,FALSE);
        }
    }

    return 0;
}

/***********************************************************************/

#ifdef __MORPHOS__
static LONG
findNameFun(void)
{
    //struct Hook 		            *hook = (struct Hook *)REG_A0;
    //Object      		            *obj = (Object *)REG_A2;
    struct MUIP_NListtree_FindNameMessage *msg = (struct MUIP_NListtree_FindNameMessage *)REG_A1;
#else
static LONG SAVEDS ASM
findNameFun(REG(a0,struct Hook *hook),REG(a2,Object *obj),REG(a1,struct MUIP_NListtree_FindNameMessage *msg))
{
#endif
    struct feed *feed = msg->UserData;

    return feed->name ? StrnCmp(g_loc,msg->Name,feed->name,-1,SC_COLLATE1) : -1;
}

#ifdef __MORPHOS__
static struct EmulLibEntry findNameTrap = {TRAP_LIB,0,(void (*)(void))findNameFun};
static struct Hook findNameHook = {0,0,(HOOKFUNC)&findNameTrap};
#else
static struct Hook findNameHook = {0,0,(HOOKFUNC)&findNameFun};
#endif

#ifdef __MORPHOS__
static LONG
findURLFun(void)
{
    //struct Hook 		            *hook = (struct Hook *)REG_A0;
    //Object      		            *obj = (Object *)REG_A2;
    struct MUIP_NListtree_FindNameMessage *msg = (struct MUIP_NListtree_FindNameMessage *)REG_A1;
#else
static LONG SAVEDS ASM
findURLFun(REG(a0,struct Hook *hook),REG(a2,Object *obj),REG(a1,struct MUIP_NListtree_FindNameMessage *msg))
{
#endif
    struct feed *feed = msg->UserData;

    return feed->URL ? strcmp(msg->Name,feed->URL) : -1;
}

#ifdef __MORPHOS__
static struct EmulLibEntry findURLTrap = {TRAP_LIB,0,(void (*)(void))findURLFun};
static struct Hook findURLHook = {0,0,(HOOKFUNC)&findURLTrap};
#else
static struct Hook findURLHook = {0,0,(HOOKFUNC)&findURLFun};
#endif

static ULONG
mFindName(struct IClass *cl,Object *obj,struct MUIP_GroupList_Find *msg)
{
    superset(cl,obj,MUIA_NListtree_FindNameHook,&findNameHook);

    return DoSuperMethod(cl,obj,MUIM_NListtree_FindName,MUIV_NListtree_FindName_ListNode_Root,(ULONG)msg->string,0);
}

static ULONG
mFindURL(struct IClass *cl,Object *obj,struct MUIP_GroupList_Find *msg)
{
    superset(cl,obj,MUIA_NListtree_FindNameHook,&findURLHook);

    return DoSuperMethod(cl,obj,MUIM_NListtree_FindName,MUIV_NListtree_FindName_ListNode_Root,(ULONG)msg->string,0);
}

/***********************************************************************/

static ULONG
mDoubleClick(struct IClass *cl,Object *obj,struct MUIP_NListtree_DoubleClick *msg)
{
    struct data *data = INST_DATA(cl,obj);

    switch (data->doubleClick)
    {
        case GDC_Edit:
            DoMethod(obj,MUIM_GroupList_EditFeed,0);
            break;

        case GDC_Feed:
            DoMethod(obj,MUIM_GroupList_Feed,(ULONG)MUIV_GroupList_Feed_Remote,(ULONG)MUIV_GroupList_Feed_Active);
            break;

        default:
            break;
    }

    return 0;
}

/***********************************************************************/

enum
{
    T_CHANNEL = 1,
    T_FEED,
    T_IMAGE,
    T_IWIDTH,
    T_IHEIGHT,
    T_ITEM,
    T_TITLE,
    T_LINK,
    T_URL,
    T_AUTHOR,
    T_DATE,
    T_DESCR,
    T_SUBJECT,
};

static struct item items[] =
{
    "author",        T_AUTHOR,
    "category",      T_SUBJECT,
    "channel",       T_CHANNEL,
    "content",       T_DESCR,
    "created",       T_DATE,
    "dc:creator",    T_AUTHOR,
    "dc:date",       T_DATE,
    "description",   T_DESCR,
    "entry",         T_ITEM,
    "feed",          T_FEED,
    "height",        T_IHEIGHT,
    "image",         T_IMAGE,
    "item",          T_ITEM,
    "lastBuildDate", T_DATE,
    "link",          T_LINK,
    "pubdate",       T_DATE,
    "slash:section", T_SUBJECT,
    "subject",       T_SUBJECT,
    "title",         T_TITLE,
    "url",           T_URL,
    "width",         T_IWIDTH
};

static INLINE int
findItem(STRPTR name)
{
    register int low, high;

    for (low = 0, high = (sizeof(items)/sizeof(struct item))-1; low<=high; )
    {
        int mid = (low+high)>>1, cond;

        if (!(cond = stricmp(name,(items+mid)->name))) return (items+mid)->id;

        if (cond<0) high = mid-1;
        else low = mid+1;
    }

    return 0;
}

/***********************************************************************/

static void INLINE
substWinChars(STRPTR str)
{
    register STRPTR s;

    for (s = str; *s; s++)
    {
        switch (*s)
        {
            case 0x85:                       *s = '.';  break;
            case 0x91: case 0x92:            *s = '\''; break;
            case 0x93: case 0x94:            *s = '\"'; break;
            case 0x95:                       *s = '>';  break;
            case 0x96: case 0x97:            *s = '-';  break;
            case 0xA0:                       *s = ' ';  break;
        }
    }
}

/***********************************************************************/

static STRPTR
allocAndDecode(struct pdata *pdata,STRPTR str,STRPTR oldStr,ULONG freeCurrent,ULONG doTraslate)
{
    register STRPTR s, res = NULL;
    ULONG  	    l = strlen(str)+1;

    if (s = AllocPooled(pdata->pool,l))
    {
        if (doTraslate) etranslate(str,s,l,pdata->data->flags & FLG_SubstWinChars);
        else copymem(s,str,l);

        res = CodesetsUTF8ToStr(CODESETSA_Codeset, (ULONG)pdata->data->codeset,
                                CODESETSA_Source,  (ULONG)s,
                                CODESETSA_Pool,    (ULONG)pdata->pool,
                                TAG_DONE);

        FreePooled(pdata->pool,s,l);

        if (res && oldStr)
            freeVecPooled(pdata->pool,oldStr);
    }

    if (freeCurrent && (pdata->curr!=pdata->buf))
        freeVecPooled(pdata->pool,pdata->curr);

    return res;
}

/***********************************************************************/

static void
#ifndef __MORPHOS__
SAVEDS STDARGS
#endif
xstart(void *data,const XML_Char *name,const XML_Char **atts)
{
    register struct pdata *pdata = data;
    register int          id;

    if (!(id = findItem((STRPTR)name))) return;

    switch (id)
    {
        case T_CHANNEL:
            pdata->flags |= PFLG_Channel;
            break;

        case T_FEED:
            pdata->flags |= PFLG_Channel|PFLG_Atom;
            break;

        case T_IMAGE:
            pdata->flags |= PFLG_Image;
            break;

        case T_ITEM:
            if ((pdata->flags & PFLG_Group) || (ACTIVE(pdata->data->this)==pdata->feed->tn))
            {
                if (pdata->entry = AllocPooled(pdata->pool,sizeof(struct entry)))
                {
                    memset(pdata->entry,0,sizeof(struct entry));

                    pdata->entry->feed = pdata->feed;
                    pdata->flags |= PFLG_Item;
                    if (pdata->flags & PFLG_Group) pdata->entry->flags |= EFLG_Group;
                }
            }
            else pdata->feed->flags &= ~FFLG_Delete;
            pdata->flags |= PFLG_UpdateMD5;
            break;

        case T_LINK:
            if (pdata->flags & (PFLG_Channel|PFLG_Image|PFLG_Item))
            {
                if (pdata->flags & PFLG_Atom)
                {
                    if (atts[0] && !stricmp(atts[0],"href") && atts[1])
                    {
                        if (pdata->flags & PFLG_Item)
                        {
                            pdata->entry->link = allocAndDecode(pdata,(STRPTR)atts[1],pdata->entry->link,FALSE,TRUE);
                        }
                        else
                        {
                            pdata->info.link = allocAndDecode(pdata,(STRPTR)atts[1],pdata->info.link,FALSE,TRUE);
                        }
                    }
                }
                else pdata->flags |= PFLG_Valid;
            }
            break;

        default:
            if (pdata->flags & (PFLG_Channel|PFLG_Image|PFLG_Item))
                pdata->flags |= PFLG_Valid;
            break;
    }
}

/***********************************************************************/

static void
#ifndef __MORPHOS__
SAVEDS STDARGS
#endif
xend(void *data,const XML_Char *name)
{
    register struct pdata *pdata = data;
    register int 	  id;

    if (!(pdata->flags & (PFLG_Channel|PFLG_Image|PFLG_Item))) return;
    if (!(id = findItem((STRPTR)name))) return;

    if ((pdata->flags & PFLG_Item) && pdata->entry)
    {
        switch (id)
        {
            case T_ITEM:
                DoMethod(pdata->list,MUIM_List_InsertSingle,(ULONG)pdata->entry,MUIV_List_Insert_Sorted);
                if (!(pdata->flags & PFLG_Active))
                {
                    pdata->feed->flags &= ~FFLG_Delete;
                    pdata->flags |= PFLG_Active;

                    //set(pdata->list,MUIA_NList_SortType,2);

                    if (pdata->flags & PFLG_Group)
                    {
                        pdata->flags |= PFLG_FSubject;
                        set(pdata->list,MUIA_Entries_Format,((pdata->flags & PFLG_FSubject) ? MUIV_Entries_Subject : 0)|((pdata->flags & PFLG_FDate) ? MUIV_Entries_Date : 0));
                    }
                }

                pdata->entry = NULL;
                pdata->flags &= ~(PFLG_Item|PFLG_Valid);
                pdata->info.news++;
                break;

            default:
            {
                STRPTR value;

                pdata->flags &= ~PFLG_Valid;

                if (pdata->curr && (id!=T_DATE))
                {
                    pdata->curr = allocAndDecode(pdata,pdata->curr,NULL,TRUE,TRUE);
                    if ((pdata->data->flags & FLG_SubstWinChars) && (id==T_TITLE) || (id==T_DESCR))
                        substWinChars(pdata->curr);
                }

		value = pdata->curr;

                switch (id)
                {
                    case T_TITLE:
                        if (value) stripHTML(value,value,TRUE);
                        pdata->entry->title = value;
                        break;

                    case T_SUBJECT:
                        if (value) stripHTML(value,value,TRUE);
                        pdata->entry->subject = value;
                        if (!(pdata->flags & PFLG_FSubject))
                        {
                            pdata->flags |= PFLG_FSubject;
                            set(pdata->list,MUIA_Entries_Format,((pdata->flags & PFLG_FSubject) ? MUIV_Entries_Subject : 0)|((pdata->flags & PFLG_FDate) ? MUIV_Entries_Date : 0));
                        }
                        break;

                    case T_LINK:
                        if (!(pdata->flags & PFLG_Atom))
                        {
                            if (value) stripHTML(value,value,TRUE);
                            pdata->entry->link = value;
                        }
                        break;

                    case T_AUTHOR:
                        if (value)
                        {
                            if (pdata->flags & PFLG_Atom)
                            {
                                STRPTR s;

                                for (s = value; *s; s++)
                                {
                                    UBYTE c = *s;

                                    if (c!=' ' && c!='\r' && c!='\n')
                                        break;
                                }

                                if (!*s)
                                {
                                    freeVecPooled(pdata->pool,value);
                                    value = NULL;
                                }
                            }
                        }

                        if (value) stripHTML(value,value,TRUE);
                        pdata->entry->author = value;
                        break;

                    case T_DATE:
                    {
                        if (value)
                        {
                            if (!(pdata->flags & PFLG_FDate))
                            {
                                pdata->flags |= PFLG_FDate;
                                set(pdata->list,MUIA_Entries_Format,((pdata->flags & PFLG_FSubject) ? MUIV_Entries_Subject : 0)|((pdata->flags & PFLG_FDate) ? MUIV_Entries_Date : 0));
                            }

                            getdate(&pdata->entry->date,value,pdata->data->GMTOffset);

                            if (value!=pdata->buf)
                                freeVecPooled(pdata->pool,value);
                        }
                        break;
                    }

                    case T_DESCR:
                        pdata->entry->descr = value;
                        break;

                    default:
                        if (value && (value!=pdata->buf))
                            freeVecPooled(pdata->pool,value);
                        break;
                }

            	break;
            }
        }
    }
    else
        if (pdata->flags & PFLG_Image)
        {
            switch (id)
            {
                case T_IMAGE:
                    pdata->flags &= ~(PFLG_Image|PFLG_Valid);
                    break;

                default:
                {
                    STRPTR value;

		    pdata->flags &= ~PFLG_Valid;

                    if (pdata->curr && (id!=T_DATE) && (id!=T_IWIDTH) && (id!=T_IHEIGHT))
                        pdata->curr = allocAndDecode(pdata,pdata->curr,NULL,TRUE,TRUE);

                    value = pdata->curr;

                    switch (id)
                    {
                        case T_URL:
                            if (!(pdata->flags & PFLG_Atom))
                            {
                                if (value) stripHTML(value,value,TRUE);
                                pdata->info.image = value;
                            }
                            break;

                        case T_IWIDTH:
                        case T_IHEIGHT:
                        {
                            if (value)
                            {
                                LONG v;

	            		StrToLong(value,&v);
                                if (v<0 || v>256) v = -1;

                                if (id==T_IWIDTH) pdata->info.iwidth = v;
                                else pdata->info.iheight = v;

                                if (value!=pdata->buf)
                                    freeVecPooled(pdata->pool,value);
                            }
                            break;
                        }


                    case T_LINK:
                        if (!(pdata->flags & PFLG_Atom))
                        {
                            if (value) stripHTML(value,value,TRUE);
                            pdata->info.imageLink = value;
                        }
                        break;

                        default:
                            if (value && (value!=pdata->buf))
                                freeVecPooled(pdata->pool,value);
                            break;
                    }

                    break;
            	}
            }
        }
        else
        {
            switch (id)
            {
                case T_CHANNEL:
                    pdata->flags &= ~(PFLG_Channel|PFLG_Atom|PFLG_Valid);
                    break;

                default:
                {
                    STRPTR value;

		    pdata->flags &= ~PFLG_Valid;

                    if (pdata->curr && (id!=T_DATE))
                        pdata->curr = allocAndDecode(pdata,pdata->curr,NULL,TRUE,TRUE);

                    value = pdata->curr;

                    switch (id)
                    {
                        case T_TITLE:
                            if (value) stripHTML(value,value,TRUE);
                            pdata->info.title = value;
                            break;

                        case T_SUBJECT:
                            if (value) stripHTML(value,value,TRUE);
                            pdata->info.subject = value;
                            break;

                        case T_LINK:
                            if (!(pdata->flags & PFLG_Atom))
                            {
                                if (value) stripHTML(value,value,TRUE);
                                pdata->info.link = value;
                            }
                            break;

                        case T_AUTHOR:
                            if (value) stripHTML(value,value,TRUE);
                            pdata->info.author = value;
                            break;

                        case T_DATE:
                        {
                            if (value)
                            {
                                getdate(&pdata->info.date,value,pdata->data->GMTOffset);
                                if (value!=pdata->buf)
                                    freeVecPooled(pdata->pool,value);
                            }
                            pdata->flags |= PFLG_UpdateMD5;
                            break;
                        }

                        case T_DESCR:
                            pdata->info.descr = value;
                            break;

                        default:
                            if (value && (value!=pdata->buf))
                                freeVecPooled(pdata->pool,value);
                            break;
                    }

                    break;
                }
            }
        }

    pdata->flags &= ~PFLG_Allocated;
    pdata->clen   = 0;
    pdata->curr   = NULL;
}

/***********************************************************************/

static void
#ifndef __MORPHOS__
SAVEDS STDARGS
#endif
xdata(void *data,const XML_Char *s,int len)
{
    register struct pdata *pdata = data;
    register STRPTR       o, n;
    register int          l, lo, cl;

    if (pdata->flags & PFLG_UpdateMD5)
        MD5Update(&pdata->ctx,(STRPTR)s,len);

    if (!(pdata->flags & PFLG_Valid)) return;

    if (o = pdata->curr)
    {
        lo = strlen(o);
        l = lo+len+1;
    }
    else
    {
        lo = 0;
        l = len+1;
    }

    cl = pdata->clen;

    if (!(pdata->flags & PFLG_Allocated) && (cl+len<sizeof(pdata->buf)))
    {
        copymem(pdata->buf+cl,s,len);
        pdata->clen += len;
        pdata->buf[pdata->clen] = 0;
        pdata->curr = pdata->buf;
    }
    else
    {
        if (n = allocVecPooled(pdata->pool,l))
        {
            STRPTR p = n;

            if (o)
            {
                strcpy(p,o);
                if (o!=pdata->buf) freeVecPooled(pdata->pool,o);
                p += lo;
            }

            copymem(p,s,len);
            p[len] = 0;

            pdata->curr = n;
            pdata->flags |= PFLG_Allocated;
        }
    }
}

/***********************************************************************/

/*static int
#ifndef __MORPHOS__
SAVEDS STDARGS
#endif
convert(void *data,const char *s)
{
    return ((struct codeset *)data)->table[*s].ucs4;
}*/

static int
#ifndef __MORPHOS__
SAVEDS STDARGS
#endif
xunknownEncoding(void *data,const XML_Char *name,XML_Encoding *info)
{
    struct codeset *codeset;

    if (codeset = CodesetsFind((STRPTR)name,CODESETSA_NoFail,FALSE,TAG_DONE))
    {
        register int i;

        for (i = 0; i<256; i++)
        {
            if (i<128) info->map[i] = i;
            else info->map[i] = codeset->table[i].ucs4;
        }

        /*info->convert = convert;*/
        info->data = codeset;
    }
    else
    {
        int i;

        for (i = 0; i<256; i++)
            info->map[i] = i;

        /*int l = strlen(name)+1;

        if (l>128) l = 128;

        if (pdata->encoding = allocVecPooled(pdata->pool,l))
            stccpy(pdata->encoding,name,l);*/
    }

    return 1;
}

/***********************************************************************/

static STRPTR forbidden = "#?[]()<>!/\\*:";

static ULONG
createFile(APTR pool,struct feed *feed,ULONG favIcon)
{
    STRPTR file;
    ULONG  l;

    if (file = favIcon ? feed->favIconFile : feed->file)
    {
        ULONG l = strlen(file)+1;

        if (feed->tempFile = reallocVecPooledNC(pool,feed->tempFile,l+32))
        {
            msprintf(feed->tempFile,favIcon ? "%s.ico.temp" : "%s.temp",(ULONG)file);

            if (feed->fh = openFile(feed->tempFile,MODE_NEWFILE))
                return TRUE;
        }

        if (feed->tempFile)
        {
            freeVecPooled(pool,feed->tempFile);
            feed->tempFile = NULL;
	}

        return FALSE;
    }

    l = strlen(feed->name)+1;

    if (file = allocVecPooled(pool,l+32))
    {
        if (feed->tempFile = allocVecPooled(pool,l+64))
        {
            UBYTE  	    buf[DEF_FILESIZE];
            register STRPTR f, t;
            register int    i;

            f = feed->name;
            if (!strnicmp(f,"http://",7)) f += 7;

            for (t = file; *f; )
            {
                STRPTR s;

                if (*f<31)
                {
                    f++;
                    continue;
                }

                if (*f==' ')
                {
                    f++;
                    continue;
                }

                for (s = forbidden; *s; s++)
                    if (*s==*f) break;

                if (*s)
                {
                    f++;
                    continue;
                }

                *t++ = *f++;
            }

            *t = 0;

            if (*file==0) strcpy(file,favIcon ? "feed.ico" : "feed");
            else if (favIcon) strcat(file,".ico");

            for (i = 0; i<1024; i++)
            {
                BPTR lock;

                if (i==0) msnprintf(buf,sizeof(buf),favIcon ? "PROGDIR:etc/img/%s" : "PROGDIR:etc/db/%s",file);
                else if (favIcon) msnprintf(buf,sizeof(buf),"PROGDIR:etc/img/%lx_%s",i,file);
                     else msnprintf(buf,sizeof(buf),"PROGDIR:etc/db/%s_%lx",file,i);

                if (lock = Lock(buf,SHARED_LOCK)) UnLock(lock);
                else break;
            }

            if (i<1024)
            {
                msprintf(feed->tempFile,favIcon ? "%s.ico.temp" : "%s.temp",(ULONG)buf);

                if (feed->fh = openFile(feed->tempFile,MODE_NEWFILE))
                {
                    strcpy(file,buf);
                    if (favIcon) feed->favIconFile = file;
                    else feed->file = file;

                    return TRUE;
                }
            }

            freeVecPooled(pool,feed->tempFile);
            feed->tempFile = NULL;
        }

    	freeVecPooled(pool,file);
    }

    return FALSE;
}

/***********************************************************************/

static void
clearList(struct data *data)
{
    set(data->list,MUIA_List_Quiet,TRUE);
    DoMethod(data->group,MUIM_MainGroup_SetFeedInfo,NULL);
    DoMethod(data->list,MUIM_List_Clear);
    set(data->list,MUIA_Entries_Format,0);
    set(data->list,MUIA_List_Quiet,FALSE);
}

/***********************************************************************/

static ULONG
feedRemote(struct IClass *cl,Object *obj,struct data *data,struct MUI_NListtree_TreeNode *tn,LONG pri)
{
    struct feed *feed = tn->tn_User;

    if (!(feed->flags & FFLG_Group) && feed->URL)
    {
        if (!createFile(data->pool,feed,FALSE)) return FALSE;
    	data->flags |= FLG_SavePrefs;

        if (feed->parser = XML_ParserCreate(NULL))
        {
            struct TagItem ptag[] = {NP_Entry,        0,
                                     NP_Priority,     0,
    	                             #ifdef __MORPHOS__
                                     NP_CodeType,     CODETYPE_PPC,
                                     NP_PPCStackSize, 64000,
                                     #endif
                                     NP_StackSize,    48000,
                                     NP_Name,         (ULONG)"amrss - remote feedder",
                                     NP_CopyVars,     FALSE,
                                     NP_Input,        NULL,
                                     NP_CloseInput,   FALSE,
                                     NP_Output,       NULL,
                                     NP_CloseOutput,  FALSE,
                                     NP_Error,        NULL,
                                     NP_CloseError,   FALSE,
                                     TAG_DONE};

            memset(&feed->pdata,0,sizeof(feed->pdata));
            feed->pdata.cl    = cl;
            feed->pdata.obj   = obj;
            feed->pdata.data  = data;
            feed->pdata.pool  = data->pool;
            feed->pdata.list  = data->list;
            feed->pdata.feed  = feed;

            XML_SetUserData(feed->parser,&feed->pdata);
            XML_SetElementHandler(feed->parser,xstart,xend);
            XML_SetCharacterDataHandler(feed->parser,xdata);
            XML_SetUnknownEncodingHandler(feed->parser,xunknownEncoding,NULL);

            MD5Init(&feed->pdata.ctx);

            if (SACTIVE(cl,obj)==feed->tn)
            {
                DoMethod(data->list,MUIM_List_Clear);
                set(data->list,MUIA_Entries_Format,0);
            }

	    ptag[0].ti_Data = (ULONG)httpProc;
	    ptag[1].ti_Data = pri;
            if (feed->proc = CreateNewProcTagList(ptag))
            {
                struct MsgPort *procPort;

                memset(&feed->tmsg,0,sizeof(feed->tmsg));
                INITMESSAGE(&feed->tmsg,&feed->port,sizeof(struct taskMsg));

                feed->tmsg.type         = TMTYPE_Socket;
                feed->tmsg.sem          = &feed->sem;
                feed->tmsg.tn           = tn;
                feed->tmsg.resPort      = &feed->resPort;
                feed->tmsg.notifyPort   = data->notifyPort;
                feed->tmsg.app          = data->app;
                feed->tmsg.URL          = feed->URL;
                feed->tmsg.errorBuf     = feed->errorBuf = allocVecPooled(data->pool,128);
                feed->tmsg.errorBufSize = 128;
                feed->tmsg.proxy        = data->proxy;
                feed->tmsg.proxyPort    = data->proxyPort;
                feed->tmsg.useProxy     = data->flags & FLG_UseProxy;
                feed->tmsg.agent        = data->agent;
                feed->tmsg.GMTOffset    = data->GMTOffset;

                PutMsg(&feed->proc->pr_MsgPort,(struct Message *)&feed->tmsg);
                WaitPort(&feed->port);
                GetMsg(&feed->port);
                procPort = feed->tmsg.procPort;

                memset(&feed->tmsg,0,sizeof(feed->tmsg));
                INITMESSAGE(&feed->tmsg,&feed->port,sizeof(struct taskMsg));
                PutMsg(procPort,(struct Message *)&feed->tmsg);

                copymem(feed->digestBack,feed->digest,sizeof(feed->digestBack));

                feed->flags |= FFLG_Task|FFLG_Delete;
                AddTail(LIST(&data->feeds),NODE(feed));

                set(data->group,MUIA_MainGroup_Status,MUIV_MainGroup_Status_Feeding);

                data->remoteTasks++;
                return TRUE;
            }
        }

        breakProc(data,feed,FALSE);
    }

    return FALSE;
}

/***************************************************************************/

static ULONG
feedLocal(struct IClass *cl,Object *obj,struct data *data,struct MUI_NListtree_TreeNode *tn,LONG pri)
{
    struct feed *feed = tn->tn_User;

    if (feed->flags & FFLG_Task)
    	breakProc(data,feed,TRUE);

    if (SACTIVE(cl,obj)==feed->tn)
        clearList(data);

    if (!(feed->flags & FFLG_Group))
    {
        if (feed->flags & FFLG_News)
        {
            feed->flags &= ~FFLG_News;

            if (--data->news==0)
            {
                set(data->app,MUIA_RSS_YouHaveNews,FALSE);
                set(obj,MUIA_RSS_YouHaveNews,FALSE);
            }
        }

        set(data->group,MUIA_MainGroup_Status,MUIV_MainGroup_Status_Feed);

        if (feed->file)
        {
            memset(&feed->pdata,0,sizeof(feed->pdata));

            if (feed->parser = XML_ParserCreate(NULL))
            {
                struct TagItem ptag[] = {NP_Entry,        0,
                                         NP_Priority,     0,
                                         #ifdef __MORPHOS__
                                         NP_CodeType,     CODETYPE_PPC,
                                         NP_PPCStackSize, 64000,
                                         #endif
                                         NP_StackSize,    48000,
                                         NP_Name,         (ULONG)"amrss - local feedder",
                                         NP_CopyVars,     FALSE,
                                         NP_Input,        NULL,
                                         NP_CloseInput,   FALSE,
                                         NP_Output,       NULL,
                                         NP_CloseOutput,  FALSE,
                                         NP_Error,        NULL,
                                         NP_CloseError,   FALSE,
                                         TAG_DONE};

                feed->pdata.cl    = cl;
                feed->pdata.obj   = obj;
                feed->pdata.data  = data;
                feed->pdata.pool  = data->pool;
                feed->pdata.list  = data->list;
                feed->pdata.feed  = feed;

                XML_SetUserData(feed->parser,&feed->pdata);
                XML_SetElementHandler(feed->parser,xstart,xend);
                XML_SetCharacterDataHandler(feed->parser,xdata);
                XML_SetUnknownEncodingHandler(feed->parser,xunknownEncoding,NULL);

                MD5Init(&feed->pdata.ctx);

		ptag[0].ti_Data = (ULONG)httpProc;
		ptag[1].ti_Data = pri;
                if (feed->proc = CreateNewProcTagList(ptag))
                {
                    struct MsgPort *procPort;

                    memset(&feed->tmsg,0,sizeof(feed->tmsg));
                    INITMESSAGE(&feed->tmsg,&feed->port,sizeof(struct taskMsg));

                    feed->tmsg.type         = TMTYPE_File;
                    feed->tmsg.sem          = &feed->sem;
                    feed->tmsg.tn           = tn;
                    feed->tmsg.resPort      = &feed->resPort;
                    feed->tmsg.notifyPort   = data->notifyPort;
                    feed->tmsg.app          = data->app;
                    feed->tmsg.URL          = feed->file;
                    feed->tmsg.errorBuf     = feed->errorBuf = allocVecPooled(data->pool,128);
                    feed->tmsg.errorBufSize = 128;

                    PutMsg(&feed->proc->pr_MsgPort,(struct Message *)&feed->tmsg);
                    WaitPort(&feed->port);
                    GetMsg(&feed->port);
                    procPort = feed->tmsg.procPort;

                    memset(&feed->tmsg,0,sizeof(feed->tmsg));
                    INITMESSAGE(&feed->tmsg,&feed->port,sizeof(struct taskMsg));
                    PutMsg(procPort,(struct Message *)&feed->tmsg);

                    copymem(feed->digestBack,feed->digest,sizeof(feed->digestBack));
                    feed->flags |= FFLG_Task|FFLG_Delete|FFLG_Local;
                    AddTail(LIST(&data->feeds),NODE(feed));
                    data->localTasks++;
                    set(data->app,MUIA_Application_Sleep,TRUE);

                    return TRUE;
                }
            }

            breakProc(data,feed,FALSE);
        }
        else
        {
            UBYTE buf[512];

            msnprintf(buf,sizeof(buf),getString(MSG_Group_NoLocalFeed),feed->name);
            DoMethod(data->group,MUIM_MainGroup_SetInfo,(ULONG)buf,0);
        }
    }
    else
    {
        set(data->group,MUIA_MainGroup_Status,MUIV_MainGroup_Status_Group);
        DoMethod(data->group,MUIM_MainGroup_SetInfo,NULL,MSG_Group_Welcome);
    }

    return 0;
}

/***********************************************************************/

static ULONG
feedLocalGroup(struct IClass *cl,Object *obj,struct data *data,struct MUI_NListtree_TreeNode *tn,LONG pri)
{
    struct feed *feed = tn->tn_User;

    clearList(data);

    if (feed->flags & FFLG_Group)
    {
        struct MUI_NListtree_TreeNode *t;

        set(data->group,MUIA_MainGroup_Status,MUIV_MainGroup_Status_Feed);
        DoMethod(data->group,MUIM_MainGroup_SetInfo,NULL,MSG_Status_ReadingGroup);

        t = (struct MUI_NListtree_TreeNode *)DoSuperMethod(cl,obj,MUIM_NListtree_GetEntry,(ULONG)tn,MUIV_NListtree_GetEntry_Position_Head,0);
        while (t)
        {
            if (!(t->tn_Flags & TNF_LIST))
            {
                struct feed *f = t->tn_User;

                if (f->flags & FFLG_News)
                {
                    f->flags &= ~FFLG_News;

                    if (--data->news==0)
                    {
                        set(data->app,MUIA_RSS_YouHaveNews,FALSE);
                        set(obj,MUIA_RSS_YouHaveNews,FALSE);
                    }
                }

                if (f->flags & FFLG_Task)
                    breakProc(data,f,TRUE);

                if (f->file)
                {
                    ULONG ok = FALSE;

                    f->flags &= ~FFLG_Failure;
                    memset(&f->pdata,0,sizeof(f->pdata));

	            if (f->parser = XML_ParserCreate(NULL))
                    {
	                struct TagItem ptag[] = {NP_Entry,        0,
	                                         NP_Priority,     0,
	                                         #ifdef __MORPHOS__
	                                         NP_CodeType,     CODETYPE_PPC,
	                                         NP_PPCStackSize, 64000,
	                                         #endif
	                                         NP_StackSize,    48000,
	                                         NP_Name,         (ULONG)"amrss - remote group feedder",
	                                         NP_CopyVars,     FALSE,
	                                         NP_Input,        NULL,
	                                         NP_CloseInput,   FALSE,
	                                         NP_Output,       NULL,
	                                         NP_CloseOutput,  FALSE,
	                                         NP_Error,        NULL,
	                                         NP_CloseError,   FALSE,
	                                         TAG_DONE};

                        f->pdata.cl    = cl;
                        f->pdata.obj   = obj;
                        f->pdata.data  = data;
                        f->pdata.pool  = data->pool;
                        f->pdata.list  = data->list;
                        f->pdata.flags |= PFLG_Group;
                        f->pdata.feed  = f;

                        XML_SetUserData(f->parser,&f->pdata);
                        XML_SetElementHandler(f->parser,xstart,xend);
                        XML_SetCharacterDataHandler(f->parser,xdata);
                        XML_SetUnknownEncodingHandler(f->parser,xunknownEncoding,NULL);

                        MD5Init(&f->pdata.ctx);

			ptag[0].ti_Data = (ULONG)httpProc;
			ptag[1].ti_Data = pri;
                        if (f->proc = CreateNewProcTagList(ptag))
                        {
                            struct MsgPort *procPort;

                            memset(&f->tmsg,0,sizeof(f->tmsg));
                            INITMESSAGE(&f->tmsg,&f->port,sizeof(struct taskMsg));

                            f->tmsg.type         = TMTYPE_File;
                            f->tmsg.sem          = &f->sem;
                            f->tmsg.tn           = t;
                            f->tmsg.resPort      = &f->resPort;
                            f->tmsg.notifyPort   = data->notifyPort;
                            f->tmsg.app          = data->app;
                            f->tmsg.URL          = f->file;
                            f->tmsg.errorBuf     = f->errorBuf = allocVecPooled(data->pool,128);
                            f->tmsg.errorBufSize = 128;
                            f->tmsg.proxy        = data->proxy;
                            f->tmsg.proxyPort    = data->proxyPort;
                            f->tmsg.useProxy     = data->flags & FLG_UseProxy;
                            f->tmsg.agent        = data->agent;
                            f->tmsg.GMTOffset    = data->GMTOffset;

                            PutMsg(&f->proc->pr_MsgPort,(struct Message *)&f->tmsg);
                            WaitPort(&f->port);
                            GetMsg(&f->port);
                            procPort = f->tmsg.procPort;

                            memset(&f->tmsg,0,sizeof(f->tmsg));
                            INITMESSAGE(&f->tmsg,&f->port,sizeof(struct taskMsg));
                            PutMsg(procPort,(struct Message *)&f->tmsg);

                            copymem(f->digestBack,f->digest,sizeof(f->digestBack));

                            f->flags |= FFLG_Task|FFLG_Delete|FFLG_Local;
                            data->localTasks++;
                            AddTail(LIST(&data->feeds),NODE(f));

                            set(data->app,MUIA_Application_Sleep,TRUE);
                            ok = TRUE;
                        }
                    }

                    if (!ok) breakProc(data,f,FALSE);
                }
            }

            t = (struct MUI_NListtree_TreeNode *)DoMethod(obj,MUIM_NListtree_GetEntry,(ULONG)t,MUIV_NListtree_GetEntry_Position_Next,MUIV_NListtree_GetEntry_Flag_SameLevel);
        }

        if (data->localTasks==0)
        {
            DoMethod(data->group,MUIM_MainGroup_SetInfo,NULL,MSG_Status_GroupRead);
        }
    }

    return 0;
}

/***********************************************************************/

static ULONG
mFeed(struct IClass *cl,Object *obj,struct MUIP_GroupList_Feed *msg)
{
    struct data                   *data = INST_DATA(cl,obj);
    struct MUI_NListtree_TreeNode *tn;
    ULONG                         res = FALSE;

    if (data->flags & FLG_Disposing) return 0;

    if (msg->tn==(struct MUI_NListtree_TreeNode *)MUIV_NListtree_Active_Off)
    {
        clearList(data);
        set(_app(obj),MUIA_App_Title,NULL);

        return 0;
    }

    if (msg->tn==MUIV_GroupList_Feed_Active)
    {
        if (!(tn = (struct MUI_NListtree_TreeNode *)xget(obj,MUIA_NListtree_Active)))
        {
            set(data->group,MUIA_MainGroup_Status,MUIV_MainGroup_Status_None);
            return FALSE;
        }
    }
    else tn = msg->tn;

    if (tn)
    {
        struct feed *feed = tn->tn_User;

        SetAttrs(data->app,MUIA_Application_Sleep,TRUE,MUIA_App_Title,(ULONG)feed->name,TAG_DONE);

        if (!(feed->flags & FFLG_Task))
        {
            if (feed->flags & FFLG_Group)
            {
                if (data->flags & FLG_ShowGroupNews) res = feedLocalGroup(cl,obj,data,tn,-1);
                else
                {
            	    set(data->list,MUIA_List_Quiet,TRUE);
    	            DoMethod(data->group,MUIM_MainGroup_SetFeedInfo,NULL);
	                DoMethod(data->list,MUIM_List_Clear);
	                set(data->list,MUIA_Entries_Format,0);
	                set(data->list,MUIA_List_Quiet,FALSE);
	                set(data->group,MUIA_MainGroup_Status,MUIV_MainGroup_Status_Group);
                }
            }
            else
                if (msg->mode==MUIV_GroupList_Feed_Local)
                {
                    res = feedLocal(cl,obj,data,tn,0);
                }
                else
                {
                    res = feedRemote(cl,obj,data,tn,0);
                }
        }
        else res = TRUE;

        set(data->app,MUIA_Application_Sleep,FALSE);
    }
    else clearList(data);

    return res;
}

/***********************************************************************/

static ULONG
mDownloadFavIcon(struct IClass *cl,Object *obj,struct MUIP_GroupList_DownloadFavIcon *msg)
{
    struct data                   *data = INST_DATA(cl,obj);
    struct MUI_NListtree_TreeNode *tn;
    ULONG                         res = FALSE;

    if (data->flags & FLG_Disposing) return 0;

    if (msg->tn==MUIV_GroupList_DownloadFavIcon_Active)
    {
        if (!(tn = SACTIVE(cl,obj)))
        {
            set(data->group,MUIA_MainGroup_Status,MUIV_MainGroup_Status_None);
            return FALSE;
        }
    }
    else tn = msg->tn;

    if (tn)
    {
        struct feed *feed = tn->tn_User;

        set(data->app,MUIA_Application_Sleep,TRUE);

        if (!(feed->flags & (FFLG_Task|FFLG_Group)) && feed->URL)
        {
            if (createFile(data->pool,feed,TRUE))
            {
                struct TagItem ptag[] = {NP_Entry,        0,
                                         NP_Priority,     -1,
                                         #ifdef __MORPHOS__
                                         NP_CodeType,     CODETYPE_PPC,
                                         NP_PPCStackSize, 64000,
                                         #endif
                                         NP_StackSize,    48000,
                                         NP_Name,         (ULONG)"amrss - favicon downloader",
                                         NP_CopyVars,     FALSE,
                                         NP_Input,        NULL,
                                         NP_CloseInput,   FALSE,
                                         NP_Output,       NULL,
                                         NP_CloseOutput,  FALSE,
                                         NP_Error,        NULL,
                                         NP_CloseError,   FALSE,
                                         TAG_DONE};

	    	data->flags |= FLG_SavePrefs;
		
                ptag[0].ti_Data = (ULONG)favIconProc;
                if (feed->proc = CreateNewProcTagList(ptag))
                {
                    struct MsgPort *procPort;

                    memset(&feed->tmsg,0,sizeof(feed->tmsg));
                    INITMESSAGE(&feed->tmsg,&feed->port,sizeof(struct taskMsg));

                    feed->tmsg.type         = TMTYPE_Socket;
                    feed->tmsg.sem          = &feed->sem;
                    feed->tmsg.tn           = tn;
                    feed->tmsg.resPort      = &feed->resPort;
                    feed->tmsg.notifyPort   = data->notifyPort;
                    feed->tmsg.app          = data->app;
                    feed->tmsg.URL          = feed->URL;
                    feed->tmsg.errorBuf     = feed->errorBuf = allocVecPooled(data->pool,128);
                    feed->tmsg.errorBufSize = 128;
                    feed->tmsg.proxy        = data->proxy;
                    feed->tmsg.proxyPort    = data->proxyPort;
                    feed->tmsg.useProxy     = data->flags & FLG_UseProxy;
                    feed->tmsg.agent        = data->agent;
                    feed->tmsg.GMTOffset    = data->GMTOffset;

                    PutMsg(&feed->proc->pr_MsgPort,(struct Message *)&feed->tmsg);
                    WaitPort(&feed->port);
                    GetMsg(&feed->port);
                    procPort = feed->tmsg.procPort;

                    memset(&feed->tmsg,0,sizeof(feed->tmsg));
                    INITMESSAGE(&feed->tmsg,&feed->port,sizeof(struct taskMsg));
                    PutMsg(procPort,(struct Message *)&feed->tmsg);

                    feed->flags |= FFLG_Task|FFLG_Delete|FFLG_FavIcon;
                    AddTail(LIST(&data->feeds),NODE(feed));

                    set(data->group,MUIA_MainGroup_Status,MUIV_MainGroup_Status_Feeding);

                    data->remoteTasks++;
                    res = TRUE;
                }
            }

            if (!res) breakProc(data,feed,FALSE);
        }

        set(data->app,MUIA_Application_Sleep,FALSE);
    }

    return res;
}

/***********************************************************************/

static ULONG
mHandleEvent(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);
    struct feed *feed, *succ;
    ULONG       res = 0;

    if (data->flags & FLG_Setup) set(data->app,MUIA_Application_Sleep,TRUE);

    for (feed = (struct feed *)data->feeds.mlh_Head; succ = (struct feed *)feed->link.mln_Succ; feed = succ)
    {
        struct taskMsg   *tm;
        struct resultMsg *rm;

        while (rm = (struct resultMsg *)GetMsg(&feed->resPort))
        {
            parseMsg(data,rm);
            freeArbitrateVecPooled(rm);
            res++;
        }

        if (tm = (struct taskMsg *)GetMsg(&feed->port))
        {
            breakProc(data,feed,FALSE);
            res++;
        }
    }

    if (data->flags & FLG_Setup) set(data->app,MUIA_Application_Sleep,FALSE);

    return res;
}

/***********************************************************************/

static ULONG
mStop(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    breakAll(data);

    return 0;
}

/***********************************************************************/

static void
updateMD5(struct IClass *cl,Object *obj,struct data *data,struct feed *feed)
{
    if (feed->file)
    {
        BPTR file;

        if (file = openFile(feed->file,MODE_OLDFILE))
        {
            if (feed->parser = XML_ParserCreate(NULL))
            {
                UBYTE buf[2048];

                memset(&feed->pdata,0,sizeof(feed->pdata));
                feed->pdata.cl    = cl;
                feed->pdata.obj   = obj;
                feed->pdata.data  = data;
                feed->pdata.pool  = data->pool;
                feed->pdata.list  = data->list;
                feed->pdata.feed  = feed;

                XML_SetUserData(feed->parser,&feed->pdata);
                XML_SetElementHandler(feed->parser,xstart,xend);
                XML_SetCharacterDataHandler(feed->parser,xdata);
                XML_SetUnknownEncodingHandler(feed->parser,xunknownEncoding,NULL);

                MD5Init(&feed->pdata.ctx);

                for (;;)
                {
                    register int len = readFile(file,buf,sizeof(buf));

                    if (len<=0) break;
                    if (XML_Parse(feed->parser,buf,len,0)==XML_STATUS_ERROR) break;
                }

                XML_Parse(feed->parser,"",0,1);
	        XML_ParserFree(feed->parser);
        	feed->parser = NULL;

                if (feed->pdata.entry)
                    freeNew(data->pool,feed->pdata.entry);

                MD5Final(feed->digest,&feed->pdata.ctx);
                feed->flags |= FFLG_Read;
            }

            closeFile(file);
        }
    }
}

/***********************************************************************/

static ULONG
mHandleTimer(struct IClass *cl,Object *obj,Msg msg)
{
    struct data  *data = INST_DATA(cl,obj);
    register int i;

    set(data->app,MUIA_Application_Sleep,TRUE);

    DoMethod(data->group,MUIM_MainGroup_SetInfo,NULL,MSG_Status_Updating);
    addRemTimer(data,FALSE);
    data->flags |= FLG_AutoUpdate;
    data->flags &= ~FLG_PushedUpdate;

    for (i = 0; i<16; )
    {
        struct MUI_NListtree_TreeNode *tn;

        DoSuperMethod(cl,obj,MUIM_NList_GetEntry,(ULONG)data->entryPos++,(ULONG)&tn);
        if (!tn)
        {
            data->flags &= ~FLG_AutoUpdate;
            data->entryPos = 0;
            addRemTimer(data,TRUE);
            DoMethod(data->group,MUIM_MainGroup_SetInfo,NULL,MSG_Status_Updated);
            set(data->app,MUIA_Application_Sleep,FALSE);
            return TRUE;
        }

        if (!(tn->tn_Flags & TNF_LIST))
        {
            struct feed *feed = tn->tn_User;

            if (!(feed->flags & FFLG_Task) && feed->updateMin)
            {
                struct DateStamp ds, now;

                copymem(&ds,&feed->info.date,sizeof(struct DateStamp));

                if ((ds.ds_Minute += feed->updateMin)>=1440)
                //if ((ds.ds_Minute += 1)>=1440)
                {
                    ds.ds_Days   += ds.ds_Minute/1440;
                    ds.ds_Minute  = ds.ds_Minute%1440;
                }

                DateStamp(&now);

                if (CompareDates(&ds,&now)>=0)
                {
                    if (!(feed->flags & FFLG_Read)) updateMD5(cl,obj,data,feed);
                    feedRemote(cl,obj,data,tn,0);
                    i++;
                }
            }
        }
    }

    set(data->app,MUIA_Application_Sleep,FALSE);

    return TRUE;
}

/***********************************************************************/

static ULONG
mRemove(struct IClass *cl,Object *obj,Msg msg)
{
    struct 			  data *data = INST_DATA(cl,obj);
    struct MUI_NListtree_TreeNode *tn;


    if (tn = SACTIVE(cl,obj))
    {
        set(data->list,MUIA_List_Quiet,TRUE);

        /* XXX NListtree bug!
        ** If you Quiet the tree you get corrupted draw
        ** If it is a very deep tree it is very slow on 68k
        ** That is the best solution
        **/

        if (tn->tn_Flags & TNF_LIST)
            DoSuperMethod(cl,obj,MUIM_NListtree_Close,MUIV_NListtree_Close_ListNode_Active,MUIV_NListtree_Close_TreeNode_Active,0);

        DoSuperMethod(cl,obj,MUIM_NListtree_Remove,MUIV_NListtree_Remove_ListNode_Active,MUIV_NListtree_Remove_TreeNode_Active,0);


        set(data->list,MUIA_List_Quiet,FALSE);
        DoMethod(obj,MUIM_GroupList_Feed,MUIV_GroupList_Feed_Local,(ULONG)MUIV_GroupList_Feed_Active);
    }

    return 0;
}

/***********************************************************************/

M_DISP(dispatcher)
{
    M_DISPSTART

    switch (msg->MethodID)
    {
        case OM_NEW:                           return mNew(cl,obj,(APTR)msg);
        case OM_DISPOSE:                       return mDispose(cl,obj,(APTR)msg);
        case OM_SET:                           return mSets(cl,obj,(APTR)msg);

        case MUIM_Setup:                       return mSetup(cl,obj,(APTR)msg);
        case MUIM_Cleanup:                     return mCleanup(cl,obj,(APTR)msg);
        case MUIM_ContextMenuBuild:            return mContextMenuBuild(cl,obj,(APTR)msg);
        case MUIM_ContextMenuChoice:           return mContextMenuChoice(cl,obj,(APTR)msg);

        case MUIM_NListtree_DoubleClick:       return mDoubleClick(cl,obj,(APTR)msg);

        case MUIM_GroupList_LoadFeeds:         return mLoadFeeds(cl,obj,(APTR)msg);
        case MUIM_GroupList_SaveFeeds:         return mSaveFeeds(cl,obj,(APTR)msg);
        case MUIM_GroupList_EditFeed:          return mEditFeed(cl,obj,(APTR)msg);
        case MUIM_GroupList_UpdateFeed:        return mUpdateFeed(cl,obj,(APTR)msg);
        case MUIM_GroupList_FindName:          return mFindName(cl,obj,(APTR)msg);
        case MUIM_GroupList_FindURL:           return mFindURL(cl,obj,(APTR)msg);
        case MUIM_GroupList_AddFeed:           return mAddFeed(cl,obj,(APTR)msg);
        case MUIM_GroupList_SetDefaultFeed:    return mSetDefaultFeed(cl,obj,(APTR)msg);
        case MUIM_GroupList_Feed:              return mFeed(cl,obj,(APTR)msg);
        case MUIM_GroupList_HandleEvent:       return mHandleEvent(cl,obj,(APTR)msg);
        case MUIM_GroupList_Stop:              return mStop(cl,obj,(APTR)msg);
        case MUIM_GroupList_HandleTimer:       return mHandleTimer(cl,obj,(APTR)msg);
        case MUIM_GroupList_Remove:            return mRemove(cl,obj,(APTR)msg);
        case MUIM_GroupList_DownloadFavIcon:   return mDownloadFavIcon(cl,obj,(APTR)msg);

        default:                               return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(dispatcher)

/***********************************************************************/

ULONG
initGroupListClass(void)
{
    if (groupListClass = MUI_CreateCustomClass(NULL,MUIC_NListtree,NULL,sizeof(struct data),DISP(dispatcher)))
    {
        localizeMenus(menu);

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/

void
disposeGroupListClass(void)
{
    MUI_DeleteCustomClass(groupListClass);
}

/***********************************************************************/
