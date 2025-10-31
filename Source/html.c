
#include "rss.h"

#include <proto/socket.h>
#ifdef __MORPHOS__
#include <net/socketbasetags.h>
#else
#include <bsdsocket/socketbasetags.h>
#include <sys/socket.h>
#endif
#include <netdb.h>

#include <proto/iffparse.h>
#include <datatypes/textclass.h>
#include "lineread.h"
#include "URI.h"
#define CATCOMP_NUMBERS
#include "loc.h"
#include "amrss_rev.h"

/***********************************************************************/

//#ifndef __SASC
#define lm_Open  dummy1.lm_Open
#define lm_Read  dummy1.lm_Read
#define lm_Write dummy1.lm_Write
//#endif

/***********************************************************************/

struct where
{
    struct MinNode link;
    STRPTR         URL;
    STRPTR         target;
};

struct cookie
{
    struct MinNode link;
    ULONG          remove;
    STRPTR         name;
    STRPTR         value;
    STRPTR         domain;
    STRPTR         path;
};

struct data
{
    Object                              *app;
    Object                              *this;
    Object                              *group;

    APTR                                menu;
    struct MUIR_HTMLview_GetContextInfo *ci;
    struct Window                       *window;

    APTR                                userItemBar;
    APTR                                userItem;

    ULONG                               click;

    struct SignalSemaphore              countSem;
    ULONG                               count;

    struct SignalSemaphore              sem;
    Object                              *parent;

    struct data                         *data;
#ifdef __MORPHOS__
    struct EmulLibEntry             	loadTrap;
    struct EmulLibEntry             	imloadTrap;
#endif
    struct Hook                     	loadHook;
    struct Hook                     	imloadHook;

    struct MsgPort                      *notifyPort;
    STRPTR                              proxy;
    ULONG                               proxyPort;
    STRPTR                              agent;
    STRPTR                              currentHost;
    ULONG                               useProxy;
    ULONG                               ntLoadImages;

    struct MinList                      cookies;
    ULONG                               cnum;

    struct MinList                      history;
    struct where                        *current;
    ULONG                               num;

    ULONG                               flags;
};

enum
{
    FLG_Setup      = 1<<0,
    FLG_Simple     = 1<<1,
    FLG_MainObject = 1<<2,
    FLG_DisposeBar = 1<<3,
    FLG_IgnoreDC   = 1<<4,
};

#define TAG_MAINMENU   (TAG_USER+1)
#define TAG_USERMENU   (TAG_USER+2)

/***********************************************************************/

/*static void
cleanWhere(struct data *data)
{
    struct where *where;

    while (where = (struct where *)RemHead((struct List *)&data->history))
        freeArbitrateVecPooled(where);

    data->current = NULL;
    data->num = 0;
}*/

static struct where *
insertWhere(struct data *data,STRPTR URL,STRPTR target)
{
    register struct where *where;
    ULONG        	  lu, lt;

    if (!URL) return NULL;

    if (data->current)
    {
        if (!strcmp(URL,data->current->URL))
        {
            if (target)
            {
                if (data->current->target && !strcmp(target,data->current->target)) return NULL;
            }
            else
            {
                if (!data->current->target) return NULL;
            }
        }
    }

    lu = strlen(URL)+1;
    lt = target ? (strlen(target)+1) : 0;

    if (where = allocArbitrateVecPooled(sizeof(struct where)+lu+lt))
    {
        where->URL = (STRPTR)where+sizeof(struct where);
        copymem(where->URL,URL,lu);

        if (lt)
        {
            where->target = where->URL+lu;
            copymem(where->target,target,lt);
        }
        else where->target = NULL;

        if (data->num==16)
        {
            struct where *w;

            if (w = (struct where *)RemHead((struct List *)&data->history))
                freeArbitrateVecPooled(w);
        }
        else data->num++;

        AddTail((struct List *)&data->history,(struct Node *)where);
        data->current = where;
    }

    return where;
}

static struct where *
getBack(struct data *data)
{
    register struct where *where = NULL;

    if (data->current->link.mln_Pred!=(struct MinNode *)&data->history)
    {
        where = (struct where *)data->current->link.mln_Pred;
    }

    return where;
}

static struct where *
getSucc(struct data *data)
{
    register struct where *where = NULL;

    if (data->current->link.mln_Succ!=(struct MinNode *)&data->history.mlh_Tail)
    {
        where = (struct where *)data->current->link.mln_Succ;
    }

    return where;
}

/***********************************************************************/

#define LRBUFSIZE 4096
#define BUFSIZE   1024

struct handle
{
    ULONG           type;

    struct Process  *me;
    struct Window   *win;

    APTR            file;

    struct Library  *SocketBase;
    int             sock;
    struct URI      URI;
    UBYTE           lrBuf[sizeof(struct lineRead)+LRBUFSIZE];
    struct lineRead *lr;
    ULONG           status;
    ULONG           flags;

    Object          *app;

    STRPTR          URL;

    ULONG           len;
    ULONG           current;

    ULONG           moved;

    APTR	    stack;
};

enum
{
    HDTYPE_File,
    HDTYPE_Socket,
};

enum
{
    HFLG_Image     = 1<<0,
    HFLG_Post      = 1<<1,
    HFLG_Restored  = 1<<2,
    HFLG_Moved     = 1<<3,
    HFLG_NotifyLen = 1<<4,
    HFLG_Text      = 1<<5,
};

enum
{
    STATUS_Init,
    STATUS_Head,
    STATUS_ParseHead,
    STATUS_Result,
    STATUS_HTMLError,
    STATUS_Failure,
};

/***********************************************************************/

static ULONG
strtcmp(STRPTR a,STRPTR b)
{
    register int la, lb;

    la = strlen(a);
    lb = strlen(b);

    if (lb>la) return FALSE;
    return (ULONG)!strnicmp(a+la-lb,b,lb);
}

static STRPTR
findToken(STRPTR str,STRPTR token,STRPTR def,STRPTR *value)
{
    ULONG  len = 0;
    STRPTR res;

    if (str = strstr(str,token))
    {
        register STRPTR s;

        while (*str && *str!='=') str++;

        if (*str)
        {
            s = ++str;
            if (*s)
            {
                while (*str && *str!=';') str++;
                len = str-s;
                str = s;
            }
        }
    }

    if (!len) len = strlen(str = def);

    if (res = allocArbitrateVecPooled(len+1))
    {
        copymem(res,str,len);
	res[len] = 0;
    }

    return *value = res;
}

static void
freeCookie(struct cookie *cookie)
{
    if (cookie->remove) Remove(NODE(cookie));

    if (cookie->name)   freeArbitrateVecPooled(cookie->name);
    if (cookie->value)  freeArbitrateVecPooled(cookie->value);
    if (cookie->domain) freeArbitrateVecPooled(cookie->domain);
    if (cookie->path)   freeArbitrateVecPooled(cookie->path);

    freeArbitratePooled(cookie,sizeof(struct cookie));
}

static void
freeCookies(struct data *data)
{
    register struct cookie *cookie;

    ObtainSemaphore(&data->sem);

    while (cookie = (struct cookie *)RemHead(LIST(&data->cookies)))
    {
        cookie->remove = FALSE;
        freeCookie(cookie);
    }

    data->cnum = 0;

    ReleaseSemaphore(&data->sem);
}

static struct cookie *
findCookieDP(struct data *data,struct cookie *start,STRPTR domain,STRPTR path)
{
    register struct cookie *cookie, *succ;

    ObtainSemaphoreShared(&data->sem);

    if (start) cookie = (struct cookie *)start->link.mln_Succ;
    else cookie = (struct cookie *)data->cookies.mlh_Head;

    for (; succ = (struct cookie *)cookie->link.mln_Succ; cookie = succ)
    {
        STRPTR s;

        if (strtcmp(domain,cookie->domain) && (s = strstr(path,cookie->path)) && (s==path))
        {
            ReleaseSemaphore(&data->sem);
            return cookie;
        }
    }

    ReleaseSemaphore(&data->sem);

    return NULL;
}

static struct cookie *
findCookieNP(struct data *data,STRPTR name,STRPTR path)
{
    register struct cookie *cookie, *succ;

    for (cookie = (struct cookie *)data->cookies.mlh_Head; succ = (struct cookie *)cookie->link.mln_Succ; cookie = succ)
    {
        ULONG found;

        if (cookie->name) found = !strcmp(cookie->name,name);
        else found = FALSE;

        if (found)
        {
            if (cookie->path) found = strnicmp(cookie->path,path,strlen(path));
        }

        if (found) return cookie;
    }

    return NULL;
}

static void
addCookie(struct data *data,STRPTR str,STRPTR host,STRPTR path)
{
    register struct cookie *cookie;

    if (cookie = allocArbitratePooled(sizeof(struct cookie)))
    {
        STRPTR s;
        ULONG  len, l;

	cookie->remove = 0;
	cookie->name   = NULL;
    	cookie->value  = NULL;
    	cookie->domain = NULL;
    	cookie->path   = NULL;

        while (*str && *str==' ') str++;
        if (*str)
        {
            s = str;

            while (*str && *str!='=') str++;
            len = str-s;

            if (*str && (len>0) && (len<4096))
            {
                if (cookie->name = allocArbitrateVecPooled(len+1))
                {
                    copymem(cookie->name,s,len);
		    cookie->name[len] = 0;

                    s = ++str;

                    if (*s)
                    {
                        while (*str && *str!=';') str++;
                        len += (l = str-s);
                        if (l>0 && len<4096)
                        {
                            if (cookie->value = allocArbitrateVecPooled(l+1))
                            {
                                copymem(cookie->value,s,l);
			        cookie->value[l] = 0;

                                if (*str) str++;

                                if (findToken(str,"path",path,&cookie->path) &&  findToken(str,"domain",host,&cookie->domain))
                                {
                                    struct cookie *c;

                                    cookie->remove = TRUE;

                                    ObtainSemaphore(&data->sem);

                                    c = findCookieNP(data,cookie->name,cookie->path);
                                    AddTail(LIST(&data->cookies),NODE(cookie));
                                    if (c) freeCookie(c);

                                    if (data->cnum==100)
                                    {
                                        c = (struct cookie *)RemHead(LIST(&data->cookies));
                                        c->remove = FALSE;
                                        freeCookie(c);
                                    }
                                    else data->cnum++;

                                    ReleaseSemaphore(&data->sem);

                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }

        freeCookie(cookie);
    }
}

/***********************************************************************/

static INLINE void
notifyMessage(struct data *data,Object *app,ULONG type,LONG arg,ULONG flags)
{
    ObtainSemaphore(&data->sem);

    if (data->notifyPort)
    {
        struct HTTPStatusMsg *sm;

        if (sm = allocArbitratePooled(sizeof(struct HTTPStatusMsg)))
        {
            memset(sm,0,sizeof(*sm));
            sm->type  = type;
            sm->arg   = arg;

            PutMsg(data->notifyPort,(struct Message *)sm);
        }
        else
        {
            DoMethod(app,MUIM_Application_PushMethod,(ULONG)app,4,MUIM_RSS_HTTPStatus,type,arg,flags);
        }
    }

    ReleaseSemaphore(&data->sem);
}

/***********************************************************************/

#define MAXTASKS 8

static ULONG
openFun(struct data *data,struct HTMLview_LoadMsg *msg)
{
    register struct handle *handle;

    if (!strcmp("about:blank",msg->lm_Open.URL)) return NULL;

    if (handle = allocArbitratePooled(sizeof(struct handle)))
    {
        ULONG res = FALSE;

	memset(handle,0,sizeof(*handle));

        handle->status = STATUS_Failure;
        handle->app    = msg->lm_App;
        handle->me     = (struct Process *)FindTask(NULL);

        if (!strnicmp("file://",msg->lm_Open.URL,7))
        {
            if (AsyncIOBase) handle->file = OpenAsync(msg->lm_Open.URL+7,MODE_READ,2048);
            else handle->file = (APTR)Open(msg->lm_Open.URL+7,MODE_OLDFILE);

            if (handle->file)
            {
                handle->type = HDTYPE_File;
                res = TRUE;
            }
        }
        else
        {
            struct Library *SocketBase;

            handle->lr = (struct lineRead *)handle->lrBuf;

            if (handle->URL = allocArbitrateVecPooled(strlen(msg->lm_Open.URL)+1))
            {
                if (handle->SocketBase = SocketBase = OpenLibrary("bsdsocket.library",4))
                {
                    SocketBaseTags(SBTM_SETVAL(SBTC_BREAKMASK),SIGBREAKF_CTRL_C,TAG_DONE);

                    handle->sock = -1;

                    strcpy(handle->URL,msg->lm_Open.URL);
                    if (msg->lm_Open.Flags & MUIF_HTMLview_LoadMsg_Image) handle->flags |= HFLG_Image;
                    if (msg->lm_Open.Flags & MUIF_HTMLview_LoadMsg_Post) handle->flags |= HFLG_Post;

                    handle->type = HDTYPE_Socket;
                    res = TRUE;

                    notifyMessage(data,msg->lm_App,HSMTYPE_Busy,TRUE,0);
                }
            }
        }

        if (res)
        {
            ObtainSemaphore(&data->countSem);
            if (++data->count<MAXTASKS) ReleaseSemaphore(&data->countSem);

            handle->win = handle->me->pr_WindowPtr;
            handle->me->pr_WindowPtr = (APTR)-1;
            handle->status = STATUS_Init;
        }
        else
        {
            if (handle->URL) freeArbitrateVecPooled(handle->URL);
            freeArbitratePooled(handle,sizeof(struct handle));
            handle = NULL;
        }
    }

    return (ULONG)(msg->lm_Userdata = handle);
}

static ULONG
closeFun(struct data *data,struct handle *handle,struct HTMLview_LoadMsg *msg)
{
    if (handle)
    {
        switch (handle->type)
        {
            case HDTYPE_File:
                if (handle->file)
                {
                    if (AsyncIOBase) CloseAsync(handle->file);
                    else Close((BPTR)handle->file);
                }
                break;

            case HDTYPE_Socket:
            {
                struct Library *SocketBase;

                if (SocketBase = handle->SocketBase)
                {
                    if (handle->sock>=0)
                    {
                        shutdown(handle->sock,2);
                        CloseSocket(handle->sock);
                    }

                    freeURI(&handle->URI);

                    CloseLibrary(SocketBase);
                }

                notifyMessage(data,msg->lm_App,HSMTYPE_Busy,FALSE,0);
            }
            break;

            default:
                break;
        }

        handle->me->pr_WindowPtr = handle->win;

        if (handle->URL) freeArbitrateVecPooled(handle->URL);
        freeArbitratePooled(handle,sizeof(struct handle));

        msg->lm_Userdata = NULL;

        ObtainSemaphore(&data->countSem);
        if (data->count--==MAXTASKS) ReleaseSemaphore(&data->countSem);
        ReleaseSemaphore(&data->countSem);
    }

    return 0;
}

#ifndef __MORPHOS__
static INLINE ULONG
HTMLCheckStack(struct handle *handle)
{
    struct Task *me = (struct Process *)FindTask(NULL);

    if(me->pr_Task.tc_Node.ln.Type==NT_PROCESS && me->pr_CLI)
	lower = (STRPTR)me->pr_ReturnAddr+sizeof(ULONG)-*(ULONG *)me->pr_ReturnAddr;
    else lower = (STRPTR)me->pr_Task.tc_SPLower;

    if ((STRPTR)getreg(REG,A7)-lower<2048)
    {
        handle->status = STATUS_Failure;

        return FALSE;
    }

    return TRUE;
}
#endif

static LONG
readFun(struct data *data,struct handle *handle,struct HTMLview_LoadMsg *msg)
{
    if (!handle) return 0;

    if (handle->status==STATUS_Failure)
        return 0;

    switch (handle->type)
    {
        case HDTYPE_File:
            return AsyncIOBase ? ReadAsync(handle->file,msg->lm_Read.Buffer,msg->lm_Read.Size) :
                                 Read((BPTR)handle->file,msg->lm_Read.Buffer,msg->lm_Read.Size);
            break;

        case HDTYPE_Socket:
        {
            struct Library  	     *SocketBase = handle->SocketBase;
            register struct lineRead *lr = (struct lineRead *)handle->lr;
            LONG            	     code = 0; //gcc

            for (;;)
            {
                #ifndef __MORPHOS__
                if (!HTMLCheckStack(handle))
                {
                    handle->status = STATUS_Failure;

                    return 0;
                }
		#endif

                if (handle->status==STATUS_Init)
                {
                    struct URI      URI;
                    struct astream  st;
                    STRPTR host = NULL; //gcc
                    LONG   port = 0; //gcc
                    ULONG  success = FALSE;

                    handle->flags &= ~HFLG_Moved;
                    memset(&handle->URI,0,sizeof(struct URI));
                    memset(&URI,0,sizeof(struct URI));
                    memset(&st,0,sizeof(struct astream));

                    if (!parseURI(handle->URL,&handle->URI,URIFLG_SHORT))
                    {
                        if (handle->URI.schemeid==0)
                        {
                            if (!handle->URI.hostname || !*handle->URI.hostname)
                            {
                                ObtainSemaphoreShared(&data->sem);

                                if (data->currentHost && !parseURI(data->currentHost,&URI,URIFLG_SHORT) && (URI.schemeid==URISCHEME_HTTP))
                                {
                                    host = URI.hostname;
                                    port = URI.port;

                                    success = TRUE;
                                }

                                ReleaseSemaphore(&data->sem);
                            }
                        }
                        else
                        {
                            host = handle->URI.hostname;
                            port = handle->URI.port;

                            success = TRUE;
                        }
                    }

                    if (success)
                    {
                        if ((handle->sock = socket(AF_INET,SOCK_STREAM,IPPROTO_IP))>=0)
                        {
                            struct    sockaddr_in sin;
                            STRPTR    connectHost;
                            ULONG     useProxy;
                            LONG      connectPort;

                            memset(&sin,0,sizeof(sin));

                            ObtainSemaphoreShared(&data->sem);

                            if (useProxy = data->useProxy)
                            {
                                connectHost = data->proxy;
                                connectPort = data->proxyPort;
                            }
                            else
                            {
                                connectHost = host;
                                connectPort = port;
                            }

                            if ((long)(sin.sin_addr.s_addr = inet_addr(connectHost))==INADDR_NONE)
                            {
                                struct hostent *hostent;

                                if (hostent = gethostbyname(connectHost))
                                    copymem(&sin.sin_addr.s_addr,hostent->h_addr,4);
                                else success = FALSE;
                            }

                            ReleaseSemaphore(&data->sem);

                            if (success)
                            {
                                sin.sin_len    = sizeof(struct sockaddr_in);
                                sin.sin_family = AF_INET;
                                sin.sin_port   = connectPort;

                                if (connect(handle->sock,(struct sockaddr *)&sin,sizeof(sin))>=0)
                                {
                                    struct cookie *cookie;
                                    int len;

                                    if (useProxy)
                                        if (handle->flags & HFLG_Post) asnprintf(&st,"POST %s",handle->URL);
                                        else asnprintf(&st,"GET %s",handle->URL);
                                    else
                                    {
                                        if (handle->flags & HFLG_Post) asnprintf(&st,"POST %s",handle->URI.path);
                                        else asnprintf(&st,"GET %s",handle->URI.path);
                                    }

                                    asnprintf(&st," HTTP/1.0\r\n");

                                    if (handle->flags & HFLG_Post)
                                        asnprintf(&st,"Content-Type: %s\r\nContent-length: %ld\r\n",msg->EncodingType,msg->PostLength);

                                    if (handle->URI.user && handle->URI.password)
                                    {
                                        UBYTE  auth[256];
                                        STRPTR dest;

                                        msnprintf(auth,sizeof(auth),"%s:%s",handle->URI.user,handle->URI.password);

                                        if (CodesetsEncodeB64(CODESETSA_B64SourceString, (ULONG)auth,
                                                              CODESETSA_B64SourceLen,    strlen(auth),
                                                              CODESETSA_B64DestPtr,      (ULONG)&dest,
                                                              TAG_DONE)==B64_ERROR_OK)
                                        {

                                            asnprintf(&st,"Authorization: Basic %s\r\n",dest);
                                            CodesetsFreeA(dest,NULL);
                                        }
                                        else success = FALSE;
                                    }

                                    if (success)
                                    {
                                        ObtainSemaphoreShared(&data->sem);

                                        if (data->agent && *data->agent) asnprintf(&st,"User-Agent: %s\r\nHost: %s\r\n",data->agent,host);
                                        else asnprintf(&st,"User-Agent: "PRG"/"VRSTRING " Amiga RSS client\r\nHost: %s\r\n",host);

                                        ReleaseSemaphore(&data->sem);

                                        if (handle->flags & HFLG_Image) asnprintf(&st,"Accept: image/*\r\n");
                                        else asnprintf(&st,"Accept: text/html;level=3, text/html;version=3.0, */*;q=1\r\n");

                                        if (!st.buf) break;

                                        if (send(handle->sock,st.buf,strlen(st.buf),0)>=0)
                                        {
                                            reset_asprint(&st);

                                            if (cookie = findCookieDP(data,NULL,handle->URI.hostname,handle->URI.path))
                                            {
                                                len = 0;

                                                asnprintf(&st,"Cookie: %s=%s",cookie->name,cookie->value);

                                                if (!st.buf) break;

                                                if (send(handle->sock,st.buf,strlen(st.buf),0)>=0)
                                                {
                                                    reset_asprint(&st);

                                                    while (cookie = findCookieDP(data,cookie,handle->URI.hostname,handle->URI.path))
                                                    {
                                                        asnprintf(&st,"; %s=%s",cookie->name,cookie->value);

                                                        if (!st.buf) break;

                                                        if (send(handle->sock,st.buf,strlen(st.buf),0)<0)
                                                        {
                                                            success = FALSE;
                                                            break;
                                                        }
                                                        reset_asprint(&st);
                                                    }

                                                    if (send(handle->sock,"\r\n",2,0)<0) success = FALSE;
                                                }
                                                else success = FALSE;
                                            }

                                            if (success)
                                            {
                                                if (send(handle->sock,"\r\n",2,0)<0) break;

                                                initLineRead(lr,SocketBase,handle->sock,LRV_Type_NotReq,LRBUFSIZE);

                                                if (handle->flags & HFLG_Post)
                                                {
                                                    if (send(handle->sock,msg->lm_Write.Buffer,msg->lm_Write.Size,0)>=0)
                                                        handle->status = STATUS_Head;
                                                }
                                                else handle->status = STATUS_Head;
                                           }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    freeURI(&URI);
                    if (st.buf) freeArbitrateVecPooled(st.buf);

                    if (handle->status!=STATUS_Head)
                    {
                        handle->status = STATUS_Failure;
                        return 0;
                    }
                }

                if (handle->status==STATUS_Head)
                {
                    register STRPTR line, s;
                    register int    l, i;

                    l = lineRead(lr);
                    if (l<=0)
                    {
                        handle->status = STATUS_Failure;
                        return 0;
                    }
                    line = lr->line;

                    if (strnicmp("HTTP/",line,5))
                    {
                        handle->status = STATUS_Failure;
                        return 0;
                    }

                    for (s = line+5; *s && *s!=' '; s++);

                    if (!*s || *s=='\r' || *s=='\n')
                    {
                        handle->status = STATUS_Failure;
                        return 0;
                    }

                    for (; *s && *s==' '; s++);

                    if (!*s || *s=='\r' || *s=='\n')
                    {
                        handle->status = STATUS_Failure;
                        return 0;
                    }

                    for (code = 0, i = 0; *s; s++)
                    {
                        if (*s==' ' || *s=='\r' || *s=='\n') break;

                        if (*s<'0' || *s>'9' || (++i==12))
                        {
                            handle->status = STATUS_Failure;
                            return 0;
                        }

                        code = code*10 + *s-'0';
                    }

                    handle->status = STATUS_ParseHead;
                }

                if (handle->status==STATUS_ParseHead)
                {
                    register STRPTR line;
                    register int    l;

                    l = lineRead(lr);
                    if (l<=0)
                    {
                        handle->status = STATUS_Failure;
                        return 0;
                    }
                    line = lr->line;

                    if (!strnicmp("Location: ",line,10))
                    {
                        line += 10;

                        if (strchr(line,':'))
                        {
                            if (handle->URL = reallocArbitrateVecPooledNC(handle->URL,strlen(line)+1))
                                strcpy(handle->URL,line);
                        }
                        else
                        {
                            if (handle->URL = reallocArbitrateVecPooledNC(handle->URL,16+strlen(handle->URI.hostinfo)+strlen(line)+1))
                                msnprintf(handle->URL,sizeof(handle->URL),"http://%s/%s",handle->URI.hostinfo,line);
                        }

                        freeURI(&handle->URI);

                        if (handle->URL)
                        {
                            STRPTR s;

                            for (s = handle->URL; *s && *s!='\r' && *s!='\n'; s++);
                            *s = 0;

                            handle->flags |= HFLG_Moved;
                        }
                    }
                    else
                        if (!strnicmp("Content-Length: ",line,16))
                        {
                            LONG v;

			    StrToLong(line+16,&v);

                            if (v>0)
                            {
                                handle->flags |= HFLG_NotifyLen;
                                notifyMessage(data,msg->lm_App,HSMTYPE_ToDo,v,0);
                            }
                        }
                        else
                            if (!strnicmp("Content-Type: ",line,14))
                            {
                                if (!strnicmp("text/plain",line+14,10) ||
                                    !strnicmp("text/xml",line+14,8)) handle->flags |= HFLG_Text;
                            }
                            else
                                if (!strnicmp("Set-Cookie: ",line,12))
                                {
                                    if (handle->URI.hostname && handle->URI.path)
                                    {
                                        register STRPTR s;

                                        for (s = line+12; *s && *s!='\r' && *s!='\n'; s++);
                                        *s = 0;

                                        addCookie(data,line+12,handle->URI.hostname,handle->URI.path);
                                    }
                                }
                                else
                                    if (line[0]=='\r' && line[1]=='\n')
                                    {
                                        if (code==200)
                                        {
                                            handle->status = STATUS_Result;
                                        }
                                        else
                                            if ((handle->flags & HFLG_Moved) && (code/10==30) && (handle->moved++<5))
                                            {
                                                shutdown(handle->sock,2);
                                                CloseSocket(handle->sock);
                                                handle->sock = -1;
                                                handle->status = STATUS_Init;
                                            }
                                            else
                                            {
                                                if (handle->flags & HFLG_Image)
                                                {
                                                    handle->status = STATUS_Failure;
                                                    return 0;
                                                }
                                                else handle->status = STATUS_Result;
                                            }
                                    }
                }

                if (handle->status==STATUS_Result)
                {
                    register int l;

                    if (handle->flags & HFLG_Text)
                    {
                        if (msg->lm_Read.Size>=17)
                        {
                            strcpy(msg->lm_Read.Buffer,"<html><body><pre>");
                            handle->flags &= ~HFLG_Text;
                            return 17;
                        }
            		else
                        {
                	    handle->status = STATUS_Failure;
                            return 0;
                        }
                    }

                    if (lr->bufPtr<lr->howLong)
                    {
                        if (!(handle->flags & HFLG_Restored))
                        {
                            lr->buffer[lr->bufPtr] = lr->saved;
                            handle->flags |= HFLG_Restored;
                        }

                        for (l= 0; (lr->bufPtr<lr->howLong) && (l<msg->lm_Read.Size); l++)
                            msg->lm_Read.Buffer[l] = lr->buffer[lr->bufPtr++];

                        if (handle->flags & HFLG_NotifyLen)
                            notifyMessage(data,msg->lm_App,HSMTYPE_Done,l,0);

                        return l;
                    }

                    l = recv(handle->sock,msg->lm_Read.Buffer,msg->lm_Read.Size,0);
                    if (l<=0)
                    {
                        handle->status = STATUS_Failure;
                        return 0;
                    }

                    if (handle->flags & HFLG_NotifyLen)
                        notifyMessage(data,msg->lm_App,HSMTYPE_Done,l,0);

                    return l;
                }

                if (handle->status==STATUS_HTMLError)
                {
                    register int l;

                    if (lr->bufPtr<lr->howLong)
                    {
                        if (!(handle->flags & HFLG_Restored))
                        {
                            lr->buffer[lr->bufPtr] = lr->saved;
                            handle->flags |= HFLG_Restored;
                        }

                        for (l= 0; (lr->bufPtr<lr->howLong) && (l<msg->lm_Read.Size); l++)
                            msg->lm_Read.Buffer[l] = lr->buffer[lr->bufPtr++];

                        return l;
                    }

                    return 0;
                }
            }

            break;
        }
    }

    return 0;
}

#ifdef __MORPHOS__
#define PPC_STACK_SIZE 64000

static ULONG loadFun(void)
{
    struct Hook             *hook = (struct Hook *)REG_A0;
    struct HTMLview_LoadMsg *msg  = (struct HTMLview_LoadMsg *)REG_A1;
    struct data    	    *data = hook->h_Data;
    struct StackSwapStruct  stack;
    struct PPCStackSwapArgs args;
    ULONG          	    res, swap, free = FALSE;

    if (msg->lm_Type!=HTMLview_Open)
    {
	if (stack.stk_Lower = ((struct handle *)msg->lm_Userdata)->stack)
        {
	    stack.stk_Upper   = (ULONG)stack.stk_Lower+PPC_STACK_SIZE;
            stack.stk_Pointer = (APTR)stack.stk_Upper;
            swap = TRUE;
	}
    	else swap = FALSE;
    }
    else
    {
        struct Task *me = FindTask(0);
        ULONG 	    size;

        NewGetTaskAttrsA(me,&size,sizeof(size),TASKINFOTYPE_STACKSIZE,NULL);

        if (size<PPC_STACK_SIZE)
        {
	    if (!(stack.stk_Lower = allocArbitratePooled(PPC_STACK_SIZE))) return 0;

	    stack.stk_Upper   = (ULONG)stack.stk_Lower+PPC_STACK_SIZE;
            stack.stk_Pointer = (APTR)stack.stk_Upper;
            swap = TRUE;
        }
        else swap = FALSE;
    }

    switch (msg->lm_Type)
    {
        case HTMLview_Open:
	    if (swap)
            {
                args.Args[0] = (ULONG)data;
                args.Args[1] = (ULONG)msg;
            	res = NewPPCStackSwap(&stack,openFun,&args);
		if (res) ((struct handle *)res)->stack = stack.stk_Lower;
	    }
            else res = openFun(data,msg);
            break;

        case HTMLview_Close:
	    if (swap)
            {
                args.Args[0] = (ULONG)data;
                args.Args[1] = (ULONG)msg->lm_Userdata;
                args.Args[2] = (ULONG)msg;
            	res = NewPPCStackSwap(&stack,closeFun,&args);

	    	free = TRUE;
            }
            else res = closeFun(data,msg->lm_Userdata,msg);
            break;

        case HTMLview_Write:
	    if (swap)
            {
                args.Args[0] = (ULONG)data;
                args.Args[1] = (ULONG)msg->lm_Userdata;
                args.Args[2] = (ULONG)msg;
            	res = NewPPCStackSwap(&stack,readFun,&args);
	    }
            else res = readFun(data,msg->lm_Userdata,msg);
            break;

        case HTMLview_Read:
	    if (swap)
            {
                args.Args[0] = (ULONG)data;
                args.Args[1] = (ULONG)msg->lm_Userdata;
                args.Args[2] = (ULONG)msg;
            	res = NewPPCStackSwap(&stack,readFun,&args);
	    }
            else res = readFun(data,msg->lm_Userdata,msg);
            break;

        default:
            res = 0;
            break;
    }

    if (free) freeArbitratePooled(stack.stk_Lower,PPC_STACK_SIZE);

    return res;
}

static ULONG imloadFun(void)
{
    struct Hook             *hook = (struct Hook *)REG_A0;
    struct HTMLview_LoadMsg *msg  = (struct HTMLview_LoadMsg *)REG_A1;
    struct data    	    *data = hook->h_Data;
    struct StackSwapStruct  stack;
    struct PPCStackSwapArgs args;
    ULONG          	    stop, res, swap, free = FALSE;

    if (msg->lm_Type!=HTMLview_Open)
    {
	if (stack.stk_Lower = ((struct handle *)msg->lm_Userdata)->stack)
        {
	    stack.stk_Upper   = (ULONG)stack.stk_Lower+PPC_STACK_SIZE;
            stack.stk_Pointer = (APTR)stack.stk_Upper;
            swap = TRUE;
	}
    	else swap = FALSE;
    }
    else
    {
        struct Task *me = FindTask(0);
        ULONG 	    size;

        ObtainSemaphoreShared(&data->sem);
    	stop = data->ntLoadImages;
    	ReleaseSemaphore(&data->sem);
    	if (stop) return 0;

        NewGetTaskAttrsA(me,&size,sizeof(size),TASKINFOTYPE_STACKSIZE,NULL);

        if (size<PPC_STACK_SIZE)
        {
	    if (!(stack.stk_Lower = allocArbitratePooled(PPC_STACK_SIZE))) return 0;

	    stack.stk_Upper   = (ULONG)stack.stk_Lower+PPC_STACK_SIZE;
            stack.stk_Pointer = (APTR)stack.stk_Upper;
            swap = TRUE;
        }
        else swap = FALSE;
    }

    switch (msg->lm_Type)
    {
        case HTMLview_Open:
	    if (swap)
            {
                args.Args[0] = (ULONG)data;
                args.Args[1] = (ULONG)msg;
            	res = NewPPCStackSwap(&stack,openFun,&args);
		if (res) ((struct handle *)res)->stack = stack.stk_Lower;
	    }
            else res = openFun(data,msg);
            break;

        case HTMLview_Close:
	    if (swap)
            {
                args.Args[0] = (ULONG)data;
                args.Args[1] = (ULONG)msg->lm_Userdata;
                args.Args[2] = (ULONG)msg;
            	res = NewPPCStackSwap(&stack,closeFun,&args);

	    	free = TRUE;
            }
            else res = closeFun(data,msg->lm_Userdata,msg);
            break;

        case HTMLview_Write:
	    if (swap)
            {
                args.Args[0] = (ULONG)data;
                args.Args[1] = (ULONG)msg->lm_Userdata;
                args.Args[2] = (ULONG)msg;
            	res = NewPPCStackSwap(&stack,readFun,&args);
	    }
            else res = readFun(data,msg->lm_Userdata,msg);
            break;

        case HTMLview_Read:
	    if (swap)
            {
                args.Args[0] = (ULONG)data;
                args.Args[1] = (ULONG)msg->lm_Userdata;
                args.Args[2] = (ULONG)msg;
            	res = NewPPCStackSwap(&stack,readFun,&args);
	    }
            else res = readFun(data,msg->lm_Userdata,msg);
            break;

        default:
            res = 0;
            break;
    }

    if (free) freeArbitratePooled(stack.stk_Lower,PPC_STACK_SIZE);

    return res;
}
#else
static ULONG SAVEDS ASM
loadFun(REG(a0,struct Hook *hook),REG(a2,Object *obj),REG(a1,struct HTMLview_LoadMsg *msg))
{
    switch (msg->lm_Type)
    {
        case HTMLview_Open:
            return openFun(hook->h_Data,msg);

        case HTMLview_Close:
            return closeFun(hook->h_Data,msg->lm_Userdata,msg);

        case HTMLview_Write:
            return (ULONG)readFun(hook->h_Data,msg->lm_Userdata,msg);

        case HTMLview_Read:
            return (ULONG)readFun(hook->h_Data,msg->lm_Userdata,msg);

        default:
            return 0;
    }
}

static ULONG SAVEDS ASM
imloadFun(REG(a0,struct Hook *hook),REG(a2,Object *obj),REG(a1,struct HTMLview_LoadMsg *msg))
{
    struct data *data = hook->h_Data;
    ULONG       stop;

    ObtainSemaphoreShared(&data->sem);
    stop = data->ntLoadImages;
    ReleaseSemaphore(&data->sem);
    if (stop) return 0;

    switch (msg->lm_Type)
    {
        case HTMLview_Open:
            return openFun(hook->h_Data,msg);

        case HTMLview_Close:
            return closeFun(hook->h_Data,msg->lm_Userdata,msg);

        case HTMLview_Write:
            return (ULONG)readFun(hook->h_Data,msg->lm_Userdata,msg);

        case HTMLview_Read:
            return (ULONG)readFun(hook->h_Data,msg->lm_Userdata,msg);

        default:
            return 0;
    }
}
#endif

/***********************************************************************/

static struct NewMenu cMenu[] =
{
    MTITLE(MSG_CMenu_Title_HTML),
        MITEM(MSG_CMenu_Browser,MSG_CMenu_Browser),
        MITEM(MSG_CMenu_Link,MSG_CMenu_Link),
        MITEM(MSG_CMenu_CopyLink,MSG_CMenu_CopyLink),
        MITEM(MSG_CMenu_Bookmark,MSG_CMenu_Bookmark),
        MITEM(MSG_CMenu_AddQuickEMail,MSG_CMenu_AddQuickEMail),
        MBAR,
        MITEM(MSG_CMenu_ImBrowser,MSG_CMenu_ImBrowser),
        MBAR,
        MITEM(MSG_CMenu_Configure,MSG_CMenu_Configure),
    MEND
};

struct HTMLview_SharedData
{
    Object *o;
};

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct TagItem *attrs = msg->ops_AttrList;

    if (obj = (Object *)DoSuperNew(cl,obj,
            MUIA_ContextMenu, TRUE,
            TAG_MORE,(ULONG)attrs))
    {
        struct data                *data = INST_DATA(cl,obj);
        struct HTMLview_SharedData *sd;

        set(obj,MUIA_CycleChain,TRUE);

        data->this  = obj;
        data->flags = GetTagData(MUIA_HTML_Simple,FALSE,attrs) ? FLG_Simple : 0;

        DoMethod(obj,MUIM_Notify,MUIA_HTMLview_ClickedURL,MUIV_EveryTime,MUIV_Notify_Self,3,MUIM_RSS_GotoURL,MUIV_TriggerValue,0);

        if (sd = (struct HTMLview_SharedData *)GetTagData(MUIA_HTMLview_SharedData,NULL,attrs))
        {
            data->parent = sd->o;
            data->data   = (struct data *)xget(sd->o,MUIA_HTML_Data);
        }
        else
        {
            InitSemaphore(&data->sem);
            InitSemaphore(&data->countSem);
            QUICKNEWLIST(&data->history);
            QUICKNEWLIST(&data->cookies);

            data->menu = MUI_MakeObject(MUIO_MenustripNM,(ULONG)cMenu,0);

            data->data = data;
            data->flags |= FLG_MainObject;
        }

#ifdef __MORPHOS__
        data->loadTrap.Trap        = TRAP_LIB;
        data->loadTrap.Extension   = 0;
        data->loadTrap.Func        = (APTR)loadFun;
        data->loadHook.h_Entry     = (HOOKFUNC)&data->loadTrap;

        data->imloadTrap.Trap      = TRAP_LIB;
        data->imloadTrap.Extension = 0;
        data->imloadTrap.Func      = (APTR)imloadFun;
        data->imloadHook.h_Entry   = (HOOKFUNC)&data->imloadTrap;
#else
        data->loadHook.h_Entry     = (HOOKFUNC)loadFun;
        data->imloadHook.h_Entry   = (HOOKFUNC)imloadFun;
#endif
        data->loadHook.h_Data      = data->data;
        data->imloadHook.h_Data    = data->data;

        SetSuperAttrs(cl,obj,MUIA_HTMLview_LoadHook,      (ULONG)&data->loadHook,
                             MUIA_HTMLview_ImageLoadHook, (ULONG)&data->imloadHook,
                             TAG_DONE);
    }

    return (ULONG)obj;
}

/***********************************************************************/

static ULONG
mDispose(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (data->flags & FLG_MainObject)
    {
        ObtainSemaphore(&data->sem);
        data->notifyPort = NULL;
        ReleaseSemaphore(&data->sem);

        if (data->menu)
        {
            MUI_DisposeObject(data->menu);
            if (data->flags & FLG_DisposeBar) MUI_DisposeObject(data->userItemBar);
        }
    }

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static ULONG
mGet(struct IClass *cl,Object *obj,struct opGet *msg)
{
    struct data *data = INST_DATA(cl, obj);

    switch (msg->opg_AttrID)
    {
        case MUIA_HTML_Data:
            if (data->flags & FLG_MainObject)
            {
                *msg->opg_Storage = (ULONG)data;
                return TRUE;
            }
            else return get(data->parent,MUIA_HTML_Data,msg->opg_Storage);

        default:
            return DoSuperMethodA(cl,obj,(Msg)msg);
    }
}

/***************************************************************************/

#define MINSETSTACKSIZE 2048

static ULONG
mSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct data    	    *data = INST_DATA(cl,obj);
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

                if (data->flags & FLG_MainObject)
                {
                    ObtainSemaphore(&data->sem);

                    data->proxy     = prefs->proxy;
                    data->proxyPort = prefs->proxyPort;
                    data->agent     = prefs->agent;
                    data->click     = prefs->descrClick;

                    data->useProxy     = prefs->httpFlags & PHFLG_UseProxy;
                    data->ntLoadImages = prefs->httpFlags & PHFLG_NTLoadImages;

                    if (data->menu)
                    {
                        if (data->userItemBar)
                        {
                            DoMethod(data->menu,MUIM_Family_Remove,(ULONG)data->userItemBar);
                            data->flags |= FLG_DisposeBar;
                        }

                        if (data->userItem)
                        {
                            DoMethod(data->menu,MUIM_Family_Remove,(ULONG)data->userItem);
                            MUI_DisposeObject(data->userItem);
                            data->userItem = NULL;
                        }

                        if (*prefs->userItem && *prefs->userItemName)
                        {
                            if (!data->userItemBar)
                                if (data->userItemBar = MenuitemObject,MUIA_Menuitem_Title,NM_BARLABEL,End)
                                    data->flags |= FLG_DisposeBar;

                            if (data->userItem = MenuitemObject,MUIA_Menuitem_Title,prefs->userItemName,MUIA_UserData,TAG_USERMENU,End)
                            {
                                Object *o;

                                o = (Object *)DoMethod(data->menu,MUIM_FindUData,MSG_CMenu_Title_HTML);

                                if (data->userItemBar) DoMethod(o,MUIM_Family_AddTail,(ULONG)data->userItemBar);
                                DoMethod(o,MUIM_Family_AddTail,(ULONG)data->userItem);

                                data->flags &= ~FLG_DisposeBar;
                            }
                        }
                    }

                    ReleaseSemaphore(&data->sem);
                }
                break;
            }

            case MUIA_RSS_Groups:
                data->group = (Object *)tidata;
                break;

            case MUIA_HTML_CurrentHost:
                if (data->flags & FLG_MainObject)
                {
                    STRPTR ch = (STRPTR)tidata;

                    ObtainSemaphore(&data->sem);

                    if (data->currentHost)
                    {
                        freeArbitrateVecPooled(data->currentHost);
                        data->currentHost = NULL;
                    }

                    if (ch && *ch && (data->currentHost = allocArbitrateVecPooled(strlen(ch)+1)))
                        strcpy(data->currentHost,ch);

                    ReleaseSemaphore(&data->sem);
                }
                break;

            case MUIA_RSS_NotifyPort:
                if (data->flags & FLG_MainObject)
                {
                    ObtainSemaphore(&data->sem);
                    data->notifyPort = (struct MsgPort *)tidata;
                    ReleaseSemaphore(&data->sem);
                }
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

    if (!DoSuperMethodA(cl,obj,msg)) return FALSE;

    data->app   = _app(obj);
    data->flags |= FLG_Setup;

    return TRUE;
}

/***********************************************************************/

static ULONG
mCleanup(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);
    ULONG       res;

    data->flags &= ~FLG_Setup;

    res = DoSuperMethodA(cl,obj,msg);

    return res;
}

/***********************************************************************/

static ULONG
mHide(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    DoMethod(data->data->group,MUIM_MainGroup_CurrentURL,NULL,FALSE);

    return DoSuperMethodA(cl,obj,msg);
}

/***********************************************************************/

static ULONG
mContextMenuChoice(struct IClass *cl,Object *obj,struct MUIP_ContextMenuChoice *msg)
{
    struct data *data = INST_DATA(cl,obj);

    switch (muiUserData(msg->item))
    {
        case MSG_CMenu_ImBrowser:
            if (data->ci) goURLFun(data->ci->Img);
            break;

        case MSG_CMenu_Browser:
            if (data->ci) goURLFun(data->ci->URL);
            break;

        case MSG_CMenu_Link:
            if (data->ci && data->ci->URL)
                DoMethod(data->app,MUIM_RSS_GotoURL,(ULONG)data->ci->URL,MUIV_RSS_GotoURL_IgnorePrefs);
            break;

        case MSG_CMenu_AddQuickEMail:
            if (data->ci) miniMailFun(data->app,data->ci->URL);
            break;

        case MSG_CMenu_CopyLink:
            if (data->ci && data->ci->URL)
            {
                UBYTE  		 buf[512];
                struct IFFHandle *iff;

                stccpy(buf,data->ci->URL,sizeof(buf));

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
            break;

        case MSG_CMenu_Bookmark:
            if (data->ci && data->ci->URL) bookmarkFun(data->ci->URL);
            break;

        case MSG_CMenu_Configure:
            DoMethod(data->app,MUIM_App_PrefsWin,(data->flags & FLG_Simple) ? MUIV_Prefs_Page_Options : MUIV_Prefs_Page_HTTP);
            break;

        case TAG_USERMENU:
            if (data->ci && data->ci->URL)
            {
                struct prefs *prefs;

                get(data->app,MUIA_RSS_Prefs,&prefs);

                executeCommand(NULL,prefs->userItem,_win(data->this),data->ci->URL,FALSE);
            }
            break;
    }

    return 0;
}

/***********************************************************************/

static ULONG
mContextMenuBuild(struct IClass *cl,Object *obj,struct MUIP_ContextMenuBuild *msg)
{
    struct data *data = INST_DATA(cl,obj);
    APTR        menu;

    if (menu = data->data->menu)
    {
        APTR  o;
        ULONG URL, quickEMail, im;

        if (data->ci = (struct MUIR_HTMLview_GetContextInfo *)DoMethod(obj,MUIM_HTMLview_GetContextInfo,msg->mx,msg->my))
        {
            URL        = (ULONG)data->ci->URL;
            quickEMail = URL && !strnicmp(data->ci->URL,"mailto:",7);
            im         = (ULONG)data->ci->Img;
        }
        else URL = quickEMail = im = FALSE;

        set((Object *)DoMethod(menu,MUIM_FindUData,MSG_CMenu_ImBrowser),MUIA_Menuitem_Enabled,im && OpenURLBase);
        set((Object *)DoMethod(menu,MUIM_FindUData,MSG_CMenu_Browser),MUIA_Menuitem_Enabled,URL && OpenURLBase);
        set((Object *)DoMethod(menu,MUIM_FindUData,MSG_CMenu_Link),MUIA_Menuitem_Enabled,URL);
        set((Object *)DoMethod(menu,MUIM_FindUData,MSG_CMenu_CopyLink),MUIA_Menuitem_Enabled,URL);
        set((Object *)DoMethod(menu,MUIM_FindUData,MSG_CMenu_Bookmark),MUIA_Menuitem_Enabled,URL);
        set((Object *)DoMethod(menu,MUIM_FindUData,MSG_CMenu_AddQuickEMail),MUIA_Menuitem_Enabled,quickEMail);
        if (o = (Object *)DoMethod(menu,MUIM_FindUData,TAG_USERMENU)) set(o,MUIA_Menuitem_Enabled,URL);

        return (ULONG)menu;
    }

    return NULL;
}

/***********************************************************************/

static STRPTR imime[] =
{
    "gif",
    "ham",
    "ham8",
    "ief",
    "iff",
    "ilbm",
    "jfif",
    "jpe",
    "jpeg",
    "jpg",
    "pbm",
    "pgm",
    "pnm",
    "ppm",
    "rgb",
    "tif",
    "tiff",
    "xbm",
    "xpm",
    "xwd",
};

#define IMIMENUM (sizeof(imime)/sizeof(STRPTR))

static ULONG
isImage(STRPTR name)
{
    register int  cond, mid, low, high;

    low  = 0;
    high = IMIMENUM-1;

    while (low<=high)
    {
        mid = (low+high)>>1;
        if (!(cond = stricmp(name,imime[mid]))) return TRUE;
        else
            if (cond<0) high = mid-1;
            else low = mid+1;
    }

    return FALSE;
}

static STRPTR bmime[] =
{
    "8vsx",
    "ai",
    "aif",
    "aifc",
    "aiff",
    "au",
    "avi",
    "bin",
    "class",
    "dms",
    "dvi",
    "eps",
    "exe",
    "gsm",
    "gsd",
    "gtar",
    "gz",
    "hdf",
    "latex",
    "lha",
    "lzh",
    "mid",
    "midi",
    "mod",
    "mov",
    "movie",
    "mpe",
    "mpeg",
    "mpg",
    "muad",
    "pdf",
    "ps",
    "qt",
    "ra",
    "snd",
    "svx",
    "tar",
    "voc",
    "wav",
    "zip",
};

#define BMIMENUM (sizeof(bmime)/sizeof(STRPTR))

static ULONG
needBrowser(STRPTR name)
{
    register int cond, mid, low, high;

    low  = 0;
    high = BMIMENUM-1;

    while (low<=high)
    {
        mid = (low+high)>>1;
        if (!(cond = stricmp(name,bmime[mid]))) return TRUE;
        else
            if (cond<0) high = mid-1;
            else low = mid+1;
    }

    return FALSE;
}

/****************************************************************************/

enum
{
    U_HTTP,
    U_MAILTO,
    U_XMAILTO,
    U_FILE,
};

struct uscheme
{
    STRPTR name;
    int    len;
    int    id;
};

struct uscheme uschemes[] =
{
    "file://",  7,  U_FILE,
    "http://",  7,  U_HTTP,
    "mailto:",  7,  U_MAILTO,
    "xmailto",  7,  U_XMAILTO
};

static INLINE int
findUScheme(STRPTR name)
{
    register int low, high;

    for (low = 0, high = (sizeof(uschemes)/sizeof(struct uscheme))-1; low<=high; )
    {
        int mid = (low+high)>>1, cond;

        if (!(cond = strncmp(name,(uschemes+mid)->name,(uschemes+mid)->len)))
            return (uschemes+mid)->id;

        if (cond<0) high = mid-1;
        else low = mid+1;
    }

    return 0;
}

/***********************************************************************/

static ULONG
mGoToURL(struct IClass *cl,Object *obj,struct MUIP_HTMLview_GotoURL *msg)
{
    struct data *data = INST_DATA(cl,obj);
    STRPTR      URL;
    ULONG       us, dc, flags;

    URL   = msg->URL;
    us    = findUScheme(URL);
    dc    = data->data->click;
    flags = data->data->flags;

    if (!(flags & FLG_IgnoreDC) && (dc==DC_Nothing) && (us!=U_FILE) && (us!=U_XMAILTO) && (us!=U_MAILTO)) return 0;
    if (us==U_XMAILTO) return miniMailFun(data->app,URL+1);
    if (us==U_MAILTO) return goURLFun(URL);

    if (flags & FLG_Simple) return DoMethod(data->app,MUIM_RSS_GotoURL,(ULONG)msg->URL,0);
    else
    {
        register STRPTR s, d;

        for (d = NULL, s = msg->URL; *s; s++)
            if (*s=='.') d = s+1;

        if (d && *d)
        {
            if (isImage(d))
            {
                STRPTR buf;

                if (buf = allocArbitrateVecPooled(strlen(msg->URL)+64))
                {
                    msprintf(buf,"<html><body><img src=\"%s\"></body></html>",(ULONG)msg->URL);
                    superset(cl,obj,MUIA_HTMLview_Contents,buf);
                    freeArbitrateVecPooled(buf);
                }

                return 0;
            }

            if (needBrowser(d)) return goURLFun(msg->URL);
        }

        if (/*(flags & FLG_MainObject) &&*/ !msg->Target || !stricmp(msg->Target,"_New"))
        {
            if (insertWhere(data->data,msg->URL,msg->Target))
            {
                SetAttrs(data->data->group,
                    MUIA_MainGroup_Back,    (ULONG)getBack(data->data),
                    MUIA_MainGroup_Succ,    (ULONG)getSucc(data->data),
                    MUIA_MainGroup_Reload,  TRUE,
                    TAG_DONE);
            }
        }
    }

    if (!msg->Target || !stricmp(msg->Target,"_new"))
        DoMethod(data->data->group,MUIM_MainGroup_UserURL,(ULONG)msg->URL,TRUE);

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

static ULONG
mGoURL(struct IClass *cl,Object *obj,struct MUIP_RSS_GotoURL *msg)
{
    struct data *data = INST_DATA(cl,obj);
    STRPTR      target = NULL;
    ULONG       res;

    DoMethod(data->data->group,MUIM_MainGroup_CurrentURL,NULL,FALSE);

    if (!(msg->flags & MUIV_RSS_GotoURL_NoTarget))
        superget(cl,obj,MUIA_HTMLview_Target,&target);

    if (msg->flags & MUIV_RSS_GotoURL_IgnorePrefs)
        data->data->flags |= FLG_IgnoreDC;

    res = DoMethod(obj,MUIM_HTMLview_GotoURL,(ULONG)msg->URL,(ULONG)target);

    if (msg->flags & MUIV_RSS_GotoURL_IgnorePrefs)
        data->data->flags &= ~FLG_IgnoreDC;

    return res;
}

/***********************************************************************/

static ULONG
mBlindGoURL(struct IClass *cl,Object *obj,struct MUIP_HTMLview_GotoURL *msg)
{
    ULONG res;

    msg->MethodID = MUIM_HTMLview_GotoURL;
    res = DoSuperMethodA(cl,obj,(Msg)msg);
    msg->MethodID = MUIM_HTML_BlindGoURL;

    return res;
}

/***********************************************************************/

static ULONG
mGoBack(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);
    struct where *where;

    if (where = getBack(data->data))
    {
        data->data->current = where;

        SetAttrs(data->data->group,
            MUIA_MainGroup_Back,    (ULONG)getBack(data->data),
            MUIA_MainGroup_Succ,    (ULONG)getSucc(data->data),
            MUIA_MainGroup_Reload,  TRUE,
            TAG_DONE);

        if (!where->target || !stricmp(where->target,"_new"))
            DoMethod(data->data->group,MUIM_MainGroup_UserURL,(ULONG)where->URL,TRUE);

        return DoSuperMethod(cl,obj,MUIM_HTMLview_GotoURL,(ULONG)where->URL,(ULONG)where->target);
    }

    return 0;
}

/***********************************************************************/

static ULONG
mGoSucc(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);
    struct where *where;

    if (where = getSucc(data->data))
    {
        data->data->current = where;

        SetAttrs(data->data->group,
            MUIA_MainGroup_Back,    (ULONG)getBack(data->data),
            MUIA_MainGroup_Succ,    (ULONG)getSucc(data->data),
            MUIA_MainGroup_Reload,  TRUE,
            TAG_DONE);

        if (!where->target || !stricmp(where->target,"_new"))
            DoMethod(data->data->group,MUIM_MainGroup_UserURL,(ULONG)where->URL,TRUE);

        return DoSuperMethod(cl,obj,MUIM_HTMLview_GotoURL,(ULONG)where->URL,(ULONG)where->target);
    }

    return 0;
}

/***********************************************************************/

static ULONG
mFlushCookies(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (data->flags & FLG_MainObject)
        freeCookies(data);

    return 0;
}

/***********************************************************************/

M_DISP(dispatcher)
{
    M_DISPSTART

    switch (msg->MethodID)
    {
        case OM_NEW:                 return mNew(cl,obj,(APTR)msg);
        case OM_DISPOSE:             return mDispose(cl,obj,(APTR)msg);
        case OM_GET:                 return mGet(cl,obj,(APTR)msg);
        case OM_SET:                 return mSets(cl,obj,(APTR)msg);

        case MUIM_Setup:             return mSetup(cl,obj,(APTR)msg);
        case MUIM_Cleanup:           return mCleanup(cl,obj,(APTR)msg);
        case MUIM_Hide:              return mHide(cl,obj,(APTR)msg);
        case MUIM_ContextMenuBuild:  return mContextMenuBuild(cl,obj,(APTR)msg);
        case MUIM_ContextMenuChoice: return mContextMenuChoice(cl,obj,(APTR)msg);

        case MUIM_HTMLview_GotoURL:  return mGoToURL(cl,obj,(APTR)msg);

        case MUIM_HTML_BlindGoURL:   return mBlindGoURL(cl,obj,(APTR)msg);
        case MUIM_RSS_GotoURL:       return mGoURL(cl,obj,(APTR)msg);
        case MUIM_HTML_GoBack:       return mGoBack(cl,obj,(APTR)msg);
        case MUIM_HTML_GoSucc:       return mGoSucc(cl,obj,(APTR)msg);
        case MUIM_HTML_FlushCookies: return mFlushCookies(cl,obj,(APTR)msg);

        default:                     return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(dispatcher)

/***********************************************************************/

ULONG
initHTMLClass(void)
{
    if (HTMLClass = MUI_CreateCustomClass(NULL,MUIC_HTMLview,NULL,sizeof(struct data),DISP(dispatcher)))
    {
        localizeMenus(cMenu);

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/

void
disposeHTMLClass(void)
{
    MUI_DeleteCustomClass(HTMLClass);
}

/***********************************************************************/
