
#include "rss.h"
#define CATCOMP_NUMBERS
#include "loc.h"
#include "amrss_rev.h"
#include <graphics/gfxbase.h>

/***********************************************************************/

/* Stack and ver */
long                    __stack = 128000;
UBYTE                   __ver[] = VERSTAG;

/* Libraries */

#ifdef __MORPHOS__
struct IntuitionBase    *IntuitionBase = NULL;
#endif

struct Library          *UtilityBase = NULL;
struct GfxBase          *GfxBase = NULL;
struct Library          *DataTypesBase = NULL;
struct Library          *CyberGfxBase = NULL;
struct Library          *IFFParseBase = NULL;
struct RxsLib           *RexxSysBase = NULL;
struct LocaleBase       *LocaleBase = NULL;
struct Library          *MUIMasterBase = NULL;
struct Library          *OpenURLBase = NULL;
struct Library          *CodesetsBase = NULL;
struct Library          *IconBase = NULL;
struct Library          *ExpatBase = NULL;
struct Library          *AsyncIOBase = NULL;

/* MUI Classes */
struct MUI_CustomClass  *picClass = NULL;
struct MUI_CustomClass  *CMClass = NULL;
struct MUI_CustomClass  *miniMailClass = NULL;
struct MUI_CustomClass  *popphClass = NULL;
struct MUI_CustomClass  *groupClass = NULL;
struct MUI_CustomClass  *groupListClass = NULL;
struct MUI_CustomClass  *entriesListClass = NULL;
struct MUI_CustomClass  *HTMLClass = NULL;
struct MUI_CustomClass  *rootBarClass = NULL;
struct MUI_CustomClass  *rootStringClass = NULL;
struct MUI_CustomClass  *rootScrollgroupClass = NULL;
struct MUI_CustomClass  *agentClass = NULL;
struct MUI_CustomClass  *popupCodesetsClass = NULL;
struct MUI_CustomClass  *GMTClass = NULL;
struct MUI_CustomClass  *editFeedClass = NULL;
struct MUI_CustomClass  *prefsClass = NULL;
struct MUI_CustomClass  *searchClass = NULL;
struct MUI_CustomClass  *winClass = NULL;
struct MUI_CustomClass  *bwinClass = NULL;
struct MUI_CustomClass  *appClass = NULL;

/* Various */
struct Catalog          *g_cat = NULL;
struct Locale           *g_loc = NULL;
APTR                    *g_pool = NULL;
struct SignalSemaphore  g_poolSem = {0};

/* Mem debug */
#ifdef DEBUGMEM
ULONG g_tot = 0;
#endif

/***********************************************************************/

static void
requester(ULONG err)
{
    STRPTR title, text, gads;

    if (LocaleBase)
    {
        title = getString(MSG_Error_ReqTitle);
        text  = getString(err);
        gads  = getString(MSG_Error_ReqGadget);
    }
    else
    {
        title = getStaticString(MSG_Error_ReqTitle);
        text  = getStaticString(err);
        gads  = getStaticString(MSG_Error_ReqGadget);
    }

    if (MUIMasterBase)
    {
        Object *app = ApplicationObject,
            MUIA_Application_UseCommodities, FALSE,
            MUIA_Application_UseRexx,        FALSE,
        End;

        MUI_RequestA(app,NULL,0,title,gads,text,NULL);

        if (app) MUI_DisposeObject(app);
    }
    else
    {
        struct EasyStruct es;
        UBYTE    	  tbuf[1024], gbuf[32];
        STRPTR   	  f, t;

        for (f = text, t = tbuf; *f; )
        {
            if (*f=='\33')
            {
                f++;
                if (*f) f++;
            }
            else *t++ = *f++;
        }
        *t = 0;

        for (f = gads, t = gbuf; *f; )
        {
            if (*f=='\33')
            {
                f++;
                if (*f) f++;
                continue;
            }

            if (*f!='*' && *f!='_') *t++ = *f;
            f++;
        }
        *t = 0;

        es.es_StructSize   = sizeof(struct EasyStruct);
        es.es_Flags        = 0;
        es.es_Title        = title;
        es.es_TextFormat   = tbuf;
        es.es_GadgetFormat = gbuf;

        EasyRequestArgs(NULL,&es,NULL,NULL);
    }
}

/***********************************************************************/

static ULONG
createDirs(void)
{
    BPTR lock;

    lock = Lock(DEF_ETCDIR,SHARED_LOCK);
    if (!lock) lock = CreateDir(DEF_ETCDIR);
    if (lock) UnLock(lock);
    else return FALSE;

    lock = Lock(DEF_DBDIR,SHARED_LOCK);
    if (!lock) lock = CreateDir(DEF_DBDIR);
    if (lock) UnLock(lock);
    else return FALSE;

    lock = Lock(DEF_OPTSDIR,SHARED_LOCK);
    if (!lock) lock = CreateDir(DEF_OPTSDIR);
    if (lock) UnLock(lock);
    else return FALSE;

    return TRUE;
}

/***************************************************************************/

static void
closeStuff(void)
{
    if (MUIMasterBase)
    {
        if (HTMLClass) disposeHTMLClass();
        if (groupListClass) disposeGroupListClass();
        if (entriesListClass) disposeEntriesListClass();
        if (groupClass) disposeGroupClass();
        if (picClass) disposePicClass();
        if (rootStringClass) disposeRootStringClass();
        if (rootScrollgroupClass) disposeRootScrollgroupClass();
        if (rootBarClass) disposeRootBarClass();
        if (editFeedClass) disposeEditFeedClass();
        if (popupCodesetsClass) disposePopupCodesetsClass();
        if (agentClass) disposeAgentClass();
        if (GMTClass) disposeGMTClass();
        if (CMClass) disposeCMClass();
        if (miniMailClass) disposeMiniMailClass();
        if (popphClass) disposePopphClass();
        if (prefsClass) disposePrefsClass();
        if (searchClass) disposeSearchClass();
        if (winClass) disposeWinClass();
        if (bwinClass) disposeBWinClass();
        if (appClass) disposeAppClass();

        CloseLibrary(MUIMasterBase);
    }

    if (AsyncIOBase) CloseLibrary(AsyncIOBase);
    if (CyberGfxBase) CloseLibrary(CyberGfxBase);
    if (OpenURLBase) CloseLibrary(OpenURLBase);
    if (ExpatBase) CloseLibrary(ExpatBase);
    if (CodesetsBase) CloseLibrary(CodesetsBase);
    if (IconBase) CloseLibrary(IconBase);

    if (LocaleBase)
    {
        if (g_loc) CloseLocale(g_loc);
        if (g_cat) CloseCatalog(g_cat);
        CloseLibrary((struct Library *)LocaleBase);
    }

    if (RexxSysBase) CloseLibrary((struct Library *)RexxSysBase);
    if (IFFParseBase) CloseLibrary((struct Library *)IFFParseBase);
    if (DataTypesBase) CloseLibrary(DataTypesBase);
    if (GfxBase) CloseLibrary((struct Library *)GfxBase);
    if (UtilityBase) CloseLibrary(UtilityBase);


#ifdef __MORPHOS__
    if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
#endif

    if (g_pool) DeletePool(g_pool);
}

/***********************************************************************/

static ULONG
openStuff(void)
{
#ifdef __MORPHOS__
    if (!(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",37)))
        return ~0;
#endif

    if (!(LocaleBase = (struct LocaleBase *)OpenLibrary("locale.library",37)))
    	return MSG_Error_NoLocale;

    g_cat = openCatalog(DEF_CATNAME,5,0);

    if (!(MUIMasterBase = OpenLibrary("muimaster.library",20)))
        return MSG_Error_NoMUIMaster;
    if (MUIMasterBase->lib_Revision<5341)
        return MSG_Error_NoMUIMaster;

    if (!createDirs())
    	return MSG_Error_CantCreateDirs;

    if (!(UtilityBase = OpenLibrary("utility.library",37)))
    	return MSG_Error_NoUtility;

    if (!(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",37)))
    	return MSG_Error_NoGfx;

    if (!(DataTypesBase = OpenLibrary("datatypes.library",37)))
    	return MSG_Error_DataTypes;

    if (!(IFFParseBase = OpenLibrary("iffparse.library",37)))
    	return MSG_Error_NoIFFParse;

    if (!(RexxSysBase = (struct RxsLib *)OpenLibrary("rexxsyslib.library",36)))
    	return MSG_Error_NoRexxSysBase;

    if (!(IconBase = OpenLibrary("icon.library",37)))
    	return MSG_Error_Icon;

    if (!(CodesetsBase = OpenLibrary(CODESETSNAME,CODESETSVER)))
    	return MSG_Error_NoCodesets;

    if (!(ExpatBase = OpenLibrary("PROGDIR:etc/libs/expat.library",4)))
    	return MSG_Error_Expat;

    if (!(g_loc = OpenLocale(NULL)))
    	return MSG_Error_NoLoc;

    if (!(g_pool = CreatePool(MEMF_ANY,8192,2048)))
    	return MSG_Error_NoMem;

    InitSemaphore(&g_poolSem);

#ifdef __MORPHOS__
#define _OPENURLVER 3
#else
#define _OPENURLVER 4
#endif
    OpenURLBase = OpenLibrary("openurl.library",_OPENURLVER);
    AsyncIOBase = OpenLibrary("asyncio.library",0);
    if (GfxBase->LibNode.lib_Version>=39)
    	CyberGfxBase = OpenLibrary("cybergraphics.library",41);

    if (!initAppClass()) 	         return MSG_Error_NoAppClass;
    if (!initWinClass()) 	         return MSG_Error_NoWinClass;
    if (!initRootScrollgroupClass()) return MSG_Error_NoRootScrollgroupClass;
    if (!initRootStringClass())      return MSG_Error_NoRootStringClass;
    if (!initRootBarClass()) 	     return MSG_Error_NoRootBarClass;
    if (!initGroupClass()) 	         return MSG_Error_NoGroupClass;
    if (!initEntriesListClass())     return MSG_Error_NoNewsClass;
    if (!initGroupListClass()) 	     return MSG_Error_NoGrouplistClass;
    if (!initHTMLClass()) 	         return MSG_Error_NoHTMLClass;
    if (!initPicClass()) 	         return MSG_Error_NoPicClass;

    return 0;
}

/***********************************************************************/

#ifdef __MORPHOS__
int
realMain(void)
#else
int
main(void)
#endif
{
    ULONG err;
    int   res = RETURN_ERROR;

    if (!(err = openStuff()))
    {
        struct parseArgs pa;

        memset(&pa,0,sizeof(pa));

        if (parseArgs(&pa))
        {
            Object *app;

            if (app = appObject, MUIA_App_ParseArgs, &pa, End)
            {
                Object *win;

                get(app,MUIA_RSS_Win,&win);

                if (openWindow(app,win))
                {
                    ULONG sigs = 0;

                    DoMethod(app,MUIM_App_LoadFeeds);

                    if (pa.flags & PAFLG_URL)
                    {
                        DoMethod(app,MUIM_RSS_GotoURL,(ULONG)pa.URL,MUIV_RSS_GotoURL_NoTarget);
                        freeArbitrateVecPooled(pa.URL);
                    }

                    while (DoMethod(app,MUIM_Application_NewInput,(ULONG)&sigs)!=MUIV_Application_ReturnID_Quit)
                    {
                        if (sigs)
                        {
                            sigs = Wait(sigs | SIGBREAKF_CTRL_C);
                            if (sigs & SIGBREAKF_CTRL_C) break;
                        }
                    }

                    res = RETURN_OK;
                }
                else
                {
                    res = RETURN_FAIL;
                    err = MSG_Error_NoWin;
                }

                MUI_DisposeObject(app);
            }
            else
            {
                res = RETURN_FAIL;
                err = MSG_Error_NoApp;
            }
        }
        else res = RETURN_FAIL;
    }
    if (err && (err!=~0)) requester(err);

    closeStuff();

#ifdef DEBUGMEM
    printf("Memory: %ld\n",g_tot);
#endif

    return res;
}

#ifdef __MORPHOS__
#define MIN68KSTACKSIZE 64000

int
main(void)
{
    struct Task *me = FindTask(NULL);
    ULONG       size;

    if (!NewGetTaskAttrsA(me,&size,sizeof(size),TASKINFOTYPE_STACKSIZE_M68K,NULL))
        size = (ULONG)me->tc_SPUpper-(ULONG)me->tc_SPLower;

    if (size<MIN68KSTACKSIZE)
    {
        struct StackSwapStruct *sss;

        if (sss = AllocMem(sizeof(*sss)+MIN68KSTACKSIZE,MEMF_PUBLIC))
        {
            int res;

            sss->stk_Lower   = sss+1;
            sss->stk_Upper   = (ULONG)(((UBYTE *)(sss+1))+MIN68KSTACKSIZE);
            sss->stk_Pointer = (APTR)sss->stk_Upper;

            StackSwap(sss);
            res = realMain();
            StackSwap(sss);

            FreeMem(sss,sizeof(*sss)+MIN68KSTACKSIZE);

            return res;
        }
    }

    return realMain();
}

#endif

/***********************************************************************/
