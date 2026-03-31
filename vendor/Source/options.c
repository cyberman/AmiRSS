
#include "rss.h"
#include <proto/iffparse.h>
#include <prefs/prefhdr.h>
#include "amrss_rev.h"

/***********************************************************************/

#define DEF_PrefsFile       "PROGDIR:etc/opts/amrss.prefs"
#define RSSPREFS_VERSION    1

#define ID_WPREFS           MAKE_ID('W','I','D','T')
#define ID_BPREFS           MAKE_ID('T','B','A','R')
#define ID_NBPREFS          MAKE_ID('T','N','B','A')
#define ID_NODRAGDROP       MAKE_ID('N','D','&','D')
#define ID_NOAUTOUPDATE     MAKE_ID('N','A','P','D')
#define ID_HIDEBAR          MAKE_ID('H','B','A','R')
#define ID_HIDEGROUPS       MAKE_ID('H','G','R','P')
#define ID_HIDEPREVIEW      MAKE_ID('H','P','R','W')

#define ID_GROUPDC          MAKE_ID('G','D','C','K')
#define ID_ENTRIESDC        MAKE_ID('E','D','C','K')
#define ID_DESCRC           MAKE_ID('D','E','C','K')
#define ID_EDITOR           MAKE_ID('E','D','I','T')
#define ID_USERITEM         MAKE_ID('U','I','T','M')
#define ID_USERITEMNAME     MAKE_ID('U','I','T','N')
#define ID_MAXIMAGESIZE     MAKE_ID('M','I','M','S')
#define ID_OPTIONS          MAKE_ID('O','P','T','S')

#define ID_CODESET          MAKE_ID('C','S','E','T')
#define ID_GMTOFFSET        MAKE_ID('G','M','T','O')

#define ID_PROXY            MAKE_ID('P','R','X','Y')
#define ID_PROXYPORT        MAKE_ID('P','R','X','P')
#define ID_HTTPFLAGS        MAKE_ID('H','F','L','G')
#define ID_AGENT            MAKE_ID('A','G','E','N')
#define ID_USEPROXY         MAKE_ID('U','P','R','X')    /* old */

#define ID_BWINFLAGS        MAKE_ID('B','F','L','G')

static ULONG ids[] =
{
    ID_WPREFS,
    ID_BPREFS,
    ID_NBPREFS,
    ID_NODRAGDROP,
    ID_NOAUTOUPDATE,
    ID_HIDEBAR,
    ID_HIDEGROUPS,
    ID_HIDEPREVIEW,

    ID_GROUPDC,
    ID_ENTRIESDC,
    ID_DESCRC,
    ID_EDITOR,
    ID_USERITEM,
    ID_USERITEMNAME,
    ID_MAXIMAGESIZE,
    ID_OPTIONS,

    ID_CODESET,
    ID_GMTOFFSET,

    ID_PROXY,
    ID_PROXYPORT,
    ID_AGENT,
    ID_HTTPFLAGS,
    ID_USEPROXY,

    ID_BWINFLAGS,

    0
};

/***********************************************************************/

void
setDefaultBPrefs(struct bprefs *prefs)
{
    prefs->viewMode = DEF_ViewMode;
    prefs->lPos     = DEF_LPos;
    prefs->bPos     = DEF_BPos;
    prefs->layout   = DEF_Layout;
    prefs->bFlags   = DEF_BFlags;
}

/***************************************************************************/

void
setDefaultNBPrefs(struct bprefs *prefs)
{
    prefs->viewMode = DEF_NViewMode;
    prefs->lPos     = DEF_NLPos;
    prefs->bPos     = DEF_NBPos;
    prefs->layout   = DEF_NLayout;
    prefs->bFlags   = DEF_NBFlags;
}

/***************************************************************************/

void
setDefaultPrefs(struct prefs *prefs)
{
    struct codeset *codeset;

    setDefaultBPrefs(&prefs->bprefs);
    setDefaultNBPrefs(&prefs->nbprefs);

    prefs->wprefs.left      = DEF_WLEFT;
    prefs->wprefs.right     = DEF_WRIGHT;
    prefs->wprefs.rightUp   = DEF_WRIGHTUP;
    prefs->wprefs.rightDown = DEF_WRIGHTDOWN;

    prefs->flags              = DEF_PFLAGS;
    prefs->groupDoubleClick   = DEF_GDC;
    prefs->entriesDoubleClick = DEF_EDC;
    prefs->descrClick         = DEF_DC;

    *prefs->userItem     = 0;
    *prefs->userItemName = 0;

    codeset = CodesetsFindA(NULL,NULL);
    stccpy(prefs->codeset,codeset->name,sizeof(prefs->codeset));

    prefs->GMTOffset = -g_loc->loc_GMTOffset;

    *prefs->proxy    = 0;
    prefs->proxyPort = DEF_PROXYPORT;

    strcpy(prefs->console,DEF_CONSOLE);
    strcpy(prefs->agent,DEF_AGENT);
    strcpy(prefs->editor,DEF_EDITOR);

    prefs->bwinFlags = PBWFLG_ShowDragBar|PBWFLG_ShowSize;
}

/***************************************************************************/

static  ULONG
simpleSave(struct IFFHandle *iffh,ULONG id,APTR ptr,ULONG size)
{
    return (ULONG)(!PushChunk(iffh,ID_PREF,id,size) &&
                   (WriteChunkBytes(iffh,ptr,size)==size) &&
                   !PopChunk(iffh));
}

/***************************************************************************/

static  ULONG
stringSave(struct IFFHandle *iffh,ULONG id,STRPTR string)
{
    ULONG l;

    return (ULONG)(!PushChunk(iffh,ID_PREF,id,IFFSIZE_UNKNOWN) &&
                   (WriteChunkBytes(iffh,string,l = strlen(string))==l) &&
                   !PopChunk(iffh));
}

/***************************************************************************/

ULONG
savePrefs(struct prefs *prefs)
{
    struct IFFHandle *iffh;
    ULONG            res = FALSE;

    if (iffh = AllocIFF())
    {
        if (iffh->iff_Stream = Open(DEF_PrefsFile,MODE_NEWFILE))
        {
            InitIFFasDOS(iffh);

            if (!OpenIFF(iffh,IFFF_WRITE))
            {
                struct PrefHeader prhd;

                if (PushChunk(iffh,ID_PREF,ID_FORM,IFFSIZE_UNKNOWN)) goto fail;
                if (PushChunk(iffh,ID_PREF,ID_PRHD,sizeof(struct PrefHeader))) goto fail;

                prhd.ph_Version = RSSPREFS_VERSION;
                prhd.ph_Type    = 0;
                prhd.ph_Flags   = 0;

                if (WriteChunkBytes(iffh,&prhd,sizeof(struct PrefHeader))!=sizeof(struct PrefHeader))
                    goto fail;

                if (PopChunk(iffh)) goto fail;

                if (!simpleSave(iffh,ID_WPREFS,&prefs->wprefs,sizeof(prefs->wprefs)) ||
                    !simpleSave(iffh,ID_BPREFS,&prefs->bprefs,sizeof(prefs->bprefs)) ||
                    !simpleSave(iffh,ID_NBPREFS,&prefs->nbprefs,sizeof(prefs->nbprefs)) ||
                    !simpleSave(iffh,ID_NODRAGDROP,&prefs->noDragDrop,sizeof(prefs->noDragDrop)) ||
                    !simpleSave(iffh,ID_NOAUTOUPDATE,&prefs->noAutoUpdate,sizeof(prefs->noAutoUpdate)) ||
                    !simpleSave(iffh,ID_HIDEBAR,&prefs->hideBar,sizeof(prefs->hideBar)) ||
                    !simpleSave(iffh,ID_HIDEGROUPS,&prefs->hideGroups,sizeof(prefs->hideGroups)) ||
                    !simpleSave(iffh,ID_HIDEPREVIEW,&prefs->hidePreview,sizeof(prefs->hidePreview)) ||

                    !simpleSave(iffh,ID_GROUPDC,&prefs->groupDoubleClick,sizeof(prefs->groupDoubleClick)) ||
                    !simpleSave(iffh,ID_ENTRIESDC,&prefs->entriesDoubleClick,sizeof(prefs->entriesDoubleClick)) ||
                    !simpleSave(iffh,ID_DESCRC,&prefs->descrClick,sizeof(prefs->descrClick)) ||
                    !(*prefs->editor ? stringSave(iffh,ID_EDITOR,prefs->editor) : TRUE) ||
                    !(*prefs->userItem ? stringSave(iffh,ID_USERITEM,prefs->userItem) : TRUE) ||
                    !(*prefs->userItemName ? stringSave(iffh,ID_USERITEMNAME,prefs->userItemName) : TRUE) ||
                    !simpleSave(iffh,ID_MAXIMAGESIZE,&prefs->maxImageSize,sizeof(prefs->maxImageSize)) ||
                    !simpleSave(iffh,ID_OPTIONS,&prefs->flags,sizeof(prefs->flags)) ||

                    !(*prefs->codeset ? stringSave(iffh,ID_CODESET,prefs->codeset) : TRUE) ||
                    !simpleSave(iffh,ID_GMTOFFSET,&prefs->GMTOffset,sizeof(prefs->GMTOffset)) ||

                    !(*prefs->proxy ? stringSave(iffh,ID_PROXY,prefs->proxy) : TRUE) ||
                    !simpleSave(iffh,ID_PROXYPORT,&prefs->proxyPort,sizeof(prefs->proxyPort)) ||
                    !simpleSave(iffh,ID_HTTPFLAGS,&prefs->httpFlags,sizeof(prefs->httpFlags)) ||
                    !(*prefs->agent ? stringSave(iffh,ID_AGENT,prefs->agent) : TRUE) ||

                    !simpleSave(iffh,ID_BWINFLAGS,&prefs->bwinFlags,sizeof(prefs->bwinFlags))) goto fail;

                res = TRUE;

                fail: CloseIFF(iffh);
            }

            Close(iffh->iff_Stream);
        }

        FreeIFF(iffh);
    }

    if (!res) DeleteFile(DEF_PrefsFile);

    return res;
}

/**************************************************************************/

ULONG
loadPrefs(struct prefs *prefs)
{
    struct IFFHandle *iffh;
    ULONG            res = FALSE;

    setDefaultPrefs(prefs);

    if (iffh = AllocIFF())
    {
        if (iffh->iff_Stream = Open(DEF_PrefsFile,MODE_OLDFILE))
        {
            InitIFFasDOS(iffh);

            if (!OpenIFF(iffh,IFFF_READ))
            {
                struct PrefHeader  prhd;
                struct ContextNode *cn;
                ULONG              *id, l;
                ULONG              useProxy = FALSE;

                if (StopChunk(iffh,ID_PREF,ID_PRHD)) goto fail;

                for(id = ids; *id; id++) if (StopChunk(iffh,ID_PREF,*id)) goto fail;

                if (ParseIFF(iffh,IFFPARSE_SCAN)) goto fail;

                if (!(cn = CurrentChunk(iffh))) goto fail;

                if ((cn->cn_Type!=ID_PREF) || (cn->cn_ID!=ID_PRHD) ||
                    (cn->cn_Size!=sizeof(struct PrefHeader))) goto fail;

                if (ReadChunkBytes(iffh,&prhd,cn->cn_Size)!=cn->cn_Size) goto fail;

                if (prhd.ph_Version>RSSPREFS_VERSION) goto fail;

                for (;;)
                {
                    ULONG error;

                    error = ParseIFF(iffh,IFFPARSE_SCAN);
                    if (error==IFFERR_EOF) break;
                    else if (error) goto fail;

                    if (!(cn = CurrentChunk(iffh))) goto fail;

                    if (cn->cn_Type!=ID_PREF) continue;

                    if ((cn->cn_ID==ID_WPREFS) && ((l = cn->cn_Size)==sizeof(prefs->wprefs)))
                    {
                        if (ReadChunkBytes(iffh,&prefs->wprefs,l)!=l) goto fail;
                        continue;
                    }

                    if ((cn->cn_ID==ID_BPREFS) && ((l = cn->cn_Size)==sizeof(prefs->bprefs)))
                    {
                        if (ReadChunkBytes(iffh,&prefs->bprefs,l)!=l) goto fail;
                        continue;
                    }

                    if ((cn->cn_ID==ID_NBPREFS) && ((l = cn->cn_Size)==sizeof(prefs->nbprefs)))
                    {
                        if (ReadChunkBytes(iffh,&prefs->nbprefs,l)!=l) goto fail;
                        continue;
                    }

                    if ((cn->cn_ID==ID_NODRAGDROP) && ((l = cn->cn_Size)==sizeof(prefs->noDragDrop)))
                    {
                        if (ReadChunkBytes(iffh,&prefs->noDragDrop,l)!=l) goto fail;
                        continue;
                    }

                    if ((cn->cn_ID==ID_NOAUTOUPDATE) && ((l = cn->cn_Size)==sizeof(prefs->noAutoUpdate)))
                    {
                        if (ReadChunkBytes(iffh,&prefs->noAutoUpdate,l)!=l) goto fail;
                        continue;
                    }

                    if ((cn->cn_ID==ID_HIDEBAR) && ((l = cn->cn_Size)==sizeof(prefs->hideBar)))
                    {
                        if (ReadChunkBytes(iffh,&prefs->hideBar,l)!=l) goto fail;
                        continue;
                    }

                    if ((cn->cn_ID==ID_HIDEGROUPS) && ((l = cn->cn_Size)==sizeof(prefs->hideGroups)))
                    {
                        if (ReadChunkBytes(iffh,&prefs->hideGroups,l)!=l) goto fail;
                        continue;
                    }

                    if ((cn->cn_ID==ID_HIDEPREVIEW) && ((l = cn->cn_Size)==sizeof(prefs->hidePreview)))
                    {
                        if (ReadChunkBytes(iffh,&prefs->hidePreview,l)!=l) goto fail;
                        continue;
                    }

                    if ((cn->cn_ID==ID_GROUPDC) && ((l = cn->cn_Size)==sizeof(prefs->groupDoubleClick)))
                    {
                        if (ReadChunkBytes(iffh,&prefs->groupDoubleClick,l)!=l) goto fail;
                        continue;
                    }

                    if ((cn->cn_ID==ID_ENTRIESDC) && ((l = cn->cn_Size)==sizeof(prefs->entriesDoubleClick)))
                    {
                        if (ReadChunkBytes(iffh,&prefs->entriesDoubleClick,l)!=l) goto fail;
                        continue;
                    }

                    if ((cn->cn_ID==ID_DESCRC) && ((l = cn->cn_Size)==sizeof(prefs->descrClick)))
                    {
                        if (ReadChunkBytes(iffh,&prefs->descrClick,l)!=l) goto fail;
                        continue;
                    }

                    if ((cn->cn_ID==ID_EDITOR) && ((l = cn->cn_Size)<sizeof(prefs->editor)))
                    {
                        if (ReadChunkBytes(iffh,prefs->editor,l)!=l) goto fail;
                        prefs->editor[l] = 0;
                        continue;
                    }

                    if ((cn->cn_ID==ID_USERITEM) && ((l = cn->cn_Size)<sizeof(prefs->userItem)))
                    {
                        if (ReadChunkBytes(iffh,prefs->userItem,l)!=l) goto fail;
                        prefs->userItem[l] = 0;
                        continue;
                    }

                    if ((cn->cn_ID==ID_USERITEMNAME) && ((l = cn->cn_Size)<sizeof(prefs->userItemName)))
                    {
                        if (ReadChunkBytes(iffh,prefs->userItemName,l)!=l) goto fail;
                        prefs->userItemName[l] = 0;
                        continue;
                    }

                    if ((cn->cn_ID==ID_MAXIMAGESIZE) && ((l = cn->cn_Size)==sizeof(prefs->maxImageSize)))
                    {
                        if (ReadChunkBytes(iffh,&prefs->maxImageSize,l)!=l) goto fail;
                        continue;
                    }

                    if ((cn->cn_ID==ID_OPTIONS) && ((l = cn->cn_Size)==sizeof(prefs->flags)))
                    {
                        if (ReadChunkBytes(iffh,&prefs->flags,l)!=l) goto fail;
                        continue;
                    }

                    if ((cn->cn_ID==ID_CODESET) && ((l = cn->cn_Size)<sizeof(prefs->codeset)))
                    {
                        if (ReadChunkBytes(iffh,prefs->codeset,l)!=l) goto fail;
                        prefs->codeset[l] = 0;
                        continue;
                    }

                    if ((cn->cn_ID==ID_GMTOFFSET) && ((l = cn->cn_Size)==sizeof(prefs->GMTOffset)))
                    {
                        if (ReadChunkBytes(iffh,&prefs->GMTOffset,l)!=l) goto fail;
                        continue;
                    }

                    if ((cn->cn_ID==ID_PROXY) && ((l = cn->cn_Size)<sizeof(prefs->proxy)))
                    {
                        if (ReadChunkBytes(iffh,prefs->proxy,l)!=l) goto fail;
                        prefs->proxy[l] = 0;
                        continue;
                    }

                    if ((cn->cn_ID==ID_PROXYPORT) && ((l = cn->cn_Size)==sizeof(prefs->proxyPort)))
                    {
                        if (ReadChunkBytes(iffh,&prefs->proxyPort,l)!=l) goto fail;
                        continue;
                    }

                    if ((cn->cn_ID==ID_HTTPFLAGS) && ((l = cn->cn_Size)==sizeof(prefs->httpFlags)))
                    {
                        if (ReadChunkBytes(iffh,&prefs->httpFlags,l)!=l) goto fail;
                        continue;
                    }

                    if ((cn->cn_ID==ID_AGENT) && ((l = cn->cn_Size)<sizeof(prefs->agent)))
                    {
                        if (ReadChunkBytes(iffh,prefs->agent,l)!=l) goto fail;
                        prefs->codeset[l] = 0;
                        continue;
                    }

                    if ((cn->cn_ID==ID_USEPROXY) && ((l = cn->cn_Size)==sizeof(useProxy)))
                    {
                        if (ReadChunkBytes(iffh,&useProxy,l)!=l) goto fail;
                        continue;
                    }

                    if ((cn->cn_ID==ID_BWINFLAGS) && ((l = cn->cn_Size)==sizeof(prefs->bwinFlags)))
                    {
                        if (ReadChunkBytes(iffh,&prefs->bwinFlags,l)!=l) goto fail;
                        continue;
                    }
                }

                if (useProxy) prefs->httpFlags |= PHFLG_UseProxy;
                res = TRUE;

                fail: CloseIFF(iffh);
            }

            Close(iffh->iff_Stream);
        }

        FreeIFF(iffh);
    }

    return res;
}

/**************************************************************************/
