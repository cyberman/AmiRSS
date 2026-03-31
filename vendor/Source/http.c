
#include "rss.h"

#include <proto/socket.h>
#ifdef __MORPHOS__
#include <net/socketbasetags.h>
#else
#include <bsdsocket/socketbasetags.h>
#include <sys/socket.h>
#endif
#include <netdb.h>

#include "lineread.h"
#include "URI.h"
#define CATCOMP_NUMBERS
#include "loc.h"
#include "amrss_rev.h"

/****************************************************************************/

enum
{
    STATUS_Init = 0,
    STATUS_Head,
    STATUS_ParseHead,
    STATUS_Result,
};

/***********************************************************************/

#define GETNUM(a) (*((LONG *)a))

#define BUFSIZE     4096
#define LR_BUFSIZE  2048

/****************************************************************************/

#define MAXTASKS 8

/****************************************************************************/

static APTR
createStatusMsg(ULONG type,struct MUI_NListtree_TreeNode *tn,ULONG more)
{
    struct resultMsg *rm;
    ULONG            len;

    switch (type)
    {
        case MTYPE_Status:
            len = sizeof(struct statusMsg);
            break;

        case MTYPE_Line:
            len = sizeof(struct lineMsg);
            break;

        case MTYPE_Date:
            len = sizeof(struct dateMsg);
            break;

        default:
            return NULL;
    }

    if (rm = allocArbitrateVecPooled(len+more))
    {
        memset(rm,0,len+more);
        rm->type = type;
        rm->tn   = tn;
    }

    return rm;
}

/****************************************************************************/

static void
notifyStatus(struct SignalSemaphore *sem,struct MsgPort *port,struct MUI_NListtree_TreeNode *tn,ULONG status,ULONG err)
{
    struct statusMsg *sm;

    if (sm = createStatusMsg(MTYPE_Status,tn,0))
    {
        sm->status = status;
        sm->err    = err;

        PutMsg(port,(struct Message *)sm);
    }
}

/****************************************************************************/

static INLINE void
notifyMessage(struct MsgPort *port,Object *app,ULONG type,LONG arg,ULONG flags)
{
    struct HTTPStatusMsg *sm;

    if (sm = allocArbitratePooled(sizeof(struct HTTPStatusMsg)))
    {
        memset(sm,0,sizeof(*sm));
        sm->type = type;
        sm->arg  = arg;

        PutMsg(port,(struct Message *)sm);
    }
    else
    {
        DoMethod(app,MUIM_Application_PushMethod,(ULONG)app,4,MUIM_RSS_HTTPStatus,sm->type,sm->arg,sm->flags);
    }
}

/***********************************************************************/

enum
{
    HFLG_Restored  = 1<<0,
    HFLG_NotifyLen = 1<<1,
};

#ifdef __MORPHOS__
void httpProc(void)
#else
void SAVEDS httpProc(void)
#endif
{
    struct Process                *me = (struct Process *)FindTask(NULL);
    struct taskMsg                *msg;
    struct MsgPort                port;
    struct SignalSemaphore        *sem;
    struct MsgPort                *resPort, *notifyPort;
    Object                        *app;
    struct MUI_NListtree_TreeNode *tn;
    UBYTE                         proxy[DEF_URLSIZE], agent[DEF_AGENTSIZE];
    STRPTR                        location, errorBuf;
    ULONG                         type, errorBufSize, useProxy = 0; //gcc
    LONG                          err = 0, GMTOffset;
    int                           sig, proxyPort = 0; //gcc

    WaitPort(&me->pr_MsgPort);
    msg = (struct taskMsg *)GetMsg(&me->pr_MsgPort);

    sig = AllocSignal(-1);
    INITPORT(&port,sig);

    type          = msg->type;
    sem           = msg->sem;
    resPort       = msg->resPort;
    notifyPort    = msg->notifyPort;
    app           = msg->app;
    tn            = msg->tn;
    errorBuf      = msg->errorBuf;
    errorBufSize  = msg->errorBufSize;
    GMTOffset     = msg->GMTOffset;

    if (location = allocArbitrateVecPooled(strlen(msg->URL)+1))
        strcpy(location,msg->URL);

    if (type==TMTYPE_Socket)
    {
        proxyPort     = msg->proxyPort;
        useProxy      = msg->useProxy;

        if (msg->proxy && *msg->proxy) stccpy(proxy,msg->proxy,sizeof(proxy));
        else *proxy   = 0;

        if (msg->agent && *msg->agent) stccpy(agent,msg->agent,sizeof(agent));
        else strcpy(agent,PRG"/"VRSTRING " Amiga RSS client");
    }

    msg->procPort = &port;
    ReplyMsg((struct Message *)msg);

    WaitPort(&port);
    msg = (struct taskMsg *)GetMsg(&port);

    if (location)
    {
        if (type==TMTYPE_File)
        {
            BPTR file;

            if (file = openFile(location,MODE_OLDFILE))
            {
                for (;;)
                {
                    UBYTE          buf[BUFSIZE];
                    struct lineMsg *lm;
                    register int   l;

                    if (CheckSignal(SIGBREAKF_CTRL_C)) break;

                    l = readFile(file,buf,sizeof(buf));
                    if (l<=0) break;

                    if (lm = createStatusMsg(MTYPE_Line,tn,l))
                    {
                        lm->line = (UBYTE *)lm+sizeof(struct lineMsg);
                        copymem(lm->line,buf,l);
                        lm->len = l;
                        PutMsg(resPort,(struct Message *)lm);
                    }
                    else
                    {
                        err = MSG_Error_NoMem;
                        break;
                    }
                }

                closeFile(file);
            }
        }
        else
        {
            struct Library *SocketBase;

            if (SocketBase = OpenLibrary("bsdsocket.library",4))
            {
                UBYTE  		buf[BUFSIZE], lrBuf[sizeof(struct lineRead)+LR_BUFSIZE+1];
                struct lineRead *lr = NULL; //gcc
                ULONG  		status, code, ccode = 0, flags, moved; //gcc
                int    		sock;

		#ifdef __MORPHOS__
		code = 0; //gcc
		#endif

    	        SocketBaseTags(SBTM_SETVAL(SBTC_BREAKMASK),SIGBREAKF_CTRL_C,TAG_DONE);

                sock   = -1;
                status = STATUS_Init;
                flags  = 0;
                moved  = 0;

                notifyMessage(notifyPort,app,HSMTYPE_Busy,TRUE,0);

                for (;;)
                {
                    if (status==STATUS_Init)
                    {
                        struct URI URI;

                        memset(&URI,0,sizeof(struct URI));

                        if (!(err = parseURI(location,&URI,URIFLG_SHORT)))
                        {
                            if ((sock = socket(AF_INET,SOCK_STREAM,IPPROTO_IP))>=0)
                            {
                                struct sockaddr_in sin;
                                STRPTR    hostName;
                                ULONG     found = TRUE;
                                int       port;

                                lr = (struct lineRead *)lrBuf;
                                initLineRead(lr,SocketBase,sock,LRV_Type_NotReq,LR_BUFSIZE);

                                notifyStatus(sem,resPort,tn,MSG_Status_ResolvingHost,0);

                                if (useProxy)
                                {
                                    hostName = proxy;
                                    port     = proxyPort;
                                }
                                else
                                {
                                    hostName = URI.hostname;
                                    port     = URI.port;
                                }

                                if ((long)(sin.sin_addr.s_addr = inet_addr(hostName))==INADDR_NONE)
                                {
                                    struct hostent *host;

                                    if (host = gethostbyname(hostName))
                                        copymem(&sin.sin_addr.s_addr,host->h_addr,4);
                                    else found = FALSE;
                                }

                                if (found)
                                {
                                    sin.sin_port   = port;
                                    sin.sin_family = AF_INET;
                                    sin.sin_len    = sizeof(struct sockaddr_in);

                                    notifyStatus(sem,resPort,tn,MSG_Status_Connecting,0);

                                    if (connect(sock,(struct sockaddr *)&sin,sizeof(sin))>=0)
                                    {
                                        int len;

                                        if (useProxy)
                                        {
                                            len = msnprintf(buf,BUFSIZE,"GET %s HTTP/1.0\r\n",location);
                                            msnprintf(buf+len,BUFSIZE-len,"User-Agent: %s\r\nHost: %s\r\nCache-Control: no-cache\r\n\r\n",agent,URI.hostname);
                                        }
                                        else
                                        {
                                            len = msnprintf(buf,BUFSIZE,"GET %s",URI.path);
                                            len += msnprintf(buf+len,BUFSIZE-len," HTTP/1.0\r\n");

                                            if (URI.user && URI.password)
                                            {
                                                UBYTE  auth[256];
                                                STRPTR dest;

                                                msnprintf(auth,sizeof(auth),"%s:%s",URI.user,URI.password);

                                                if (CodesetsEncodeB64(CODESETSA_B64SourceString, (ULONG)auth,
                                                                      CODESETSA_B64SourceLen,    strlen(auth),
                                                                      CODESETSA_B64DestPtr,      (ULONG)&dest,
                                                                      TAG_DONE)==B64_ERROR_OK)
                                                {
                                                    len += msnprintf(buf+len,BUFSIZE-len,"Authorization: Basic %s\r\n",dest);
                                                    CodesetsFreeA(dest,NULL);
                                                }
                                                else
                                                {
                                                    err = MSG_Error_NoMem;
                                                    break;
                                                }
                                            }

                                            msnprintf(buf+len,BUFSIZE-len,"User-Agent: %s\r\nHost: %s\r\n\r\n",agent,URI.hostname);
                                        }

                                        notifyStatus(sem,resPort,tn,MSG_Status_Sending,0);

                                        if (send(sock,buf,strlen(buf),0)>=0)
                                        {
                                            notifyStatus(sem,resPort,tn,MSG_Status_Receiving,0);
                                            status = STATUS_Head;
                                        }
                                        else
                                        {
                                            shutdown(sock,2);
                                            err = MSG_Error_Send;
                                        }
                                    }
                                    else err = MSG_Error_CantConnect;
                                }
                                else err = MSG_Error_NoHost;
                            }
                            else err = MSG_Error_NoSocket;
                        }

                        freeURI(&URI);

                        if (err)
                        {
                            CloseSocket(sock);
                            sock = -1;
                            break;
                        }
                    }

                    if (status==STATUS_Head)
                    {
                        struct RDArgs   ra;
                        STRPTR line;
                        LONG   parms[16];
                        int    l;

                        l = lineRead(lr);
                        if (l<=0)
                        {
                            err = MSG_Error_Recv;
                            break;
                        }
                        line = lr->line;

                        ra.RDA_Source.CS_Buffer  = line;
                        ra.RDA_Source.CS_Length  = l;
                        ra.RDA_Source.CS_CurChr  = 0;
                        ra.RDA_DAList            = NULL;
                        ra.RDA_Buffer            = buf;
                        ra.RDA_BufSiz            = BUFSIZE;
                        ra.RDA_Flags             = RDAF_NOALLOC|RDAF_NOPROMPT;
                        if (!ReadArgs("HTTP/A,CODE/A/N,REST/A/F",parms,&ra))
                        {
                            if (errorBuf)
                            {
                                STRPTR s;

                                stccpy(errorBuf,line,errorBufSize);

                                for (s = errorBuf; *s && *s!='\r' && *s!='\n'; s++);
                                *s = 0;

                                msg->flags |= TMFLG_ErrorBuf;
                            }

                            err = MSG_Error_ProtoError;
                            break;
                        }

                        if (strnicmp("HTTP/",(STRPTR)parms[0],5))
                        {
                            if (errorBuf)
                            {
                                STRPTR s;

                                stccpy(errorBuf,line,errorBufSize);

                                for (s = errorBuf; *s && *s!='\r' && *s!='\n'; s++);
                                *s = 0;

                                msg->flags |= TMFLG_ErrorBuf;
                            }

                            err = MSG_Error_ProtoError;
                            break;
                        }

                        code  = GETNUM(parms[1]);
                        ccode = code/10;

                        if (ccode==20 || ((ccode==30) && (moved++<2))) status = STATUS_ParseHead;
                        else
                        {
                            if (errorBuf)
                            {
                                STRPTR s;

                                stccpy(errorBuf,line,errorBufSize);

                                for (s = errorBuf; *s && *s!='\r' && *s!='\n'; s++);
                                *s = 0;

                                msg->flags |= TMFLG_ErrorBuf;
                            }

                            err = MSG_Error_ServerError;
                            break;
                        }
                    }

                    if (status==STATUS_ParseHead)
                    {
                        STRPTR line;
                        int    l;

                        l = lineRead(lr);
                        if (l<=0)
                        {
                            err = MSG_Error_Recv;
                            break;
                        }
                        line = lr->line;

                        if (!strnicmp("Last-Modified: ",line,15))
                        {
                            struct dateMsg *dm;

                            if (dm = createStatusMsg(MTYPE_Date,tn,0))
                            {
                                getdate(&dm->ds,line+15,GMTOffset);
                                PutMsg(resPort,(struct Message *)dm);
                            }
                        }
                        else
                            if (!strnicmp("Location: ",line,10))
                            {
                                if (location = reallocArbitrateVecPooledNC(location,strlen(line+10)+1))
                                {
                                    STRPTR s;

                                    strcpy(location,line+10);
                                    for (s = location; *s && *s!='\r' && *s!='\n'; s++);
                                    *s = 0;
                                    moved = 1;
                                }
                            }
                            else
                                if (!strnicmp("Content-Length: ",line,16))
                                {
                                    LONG len;

	                	    StrToLong(line+16,&len);

                                    if (len>0)
                                    {
                                        flags |= HFLG_NotifyLen;
                                        notifyMessage(notifyPort,app,HSMTYPE_ToDo,len,0);
                                    }
                                }
                                else
                                    if (line[0]=='\r' && line[1]=='\n')
                                    {
                                        if (ccode==20)
                                        {
                                            status = STATUS_Result;

                                            if (!(flags & HFLG_NotifyLen))
                                                notifyMessage(notifyPort,app,HSMTYPE_ToDo,0,HSMFLG_NoLen);

                                        }
                                        else
                                            if (ccode==30)
                                            {
                                                shutdown(sock,2);
                                                CloseSocket(sock);
                                                sock = -1;
                                                status = STATUS_Init;
                                            }
                                    }
                    }

                    if (status==STATUS_Result)
                    {
                        struct lineMsg *lm;
                        int            l;

                        if (lr->bufPtr<lr->howLong)
                        {
                            if (lm = createStatusMsg(MTYPE_Line,tn,lr->howLong-lr->bufPtr))
                            {
                                if (!(flags & HFLG_Restored))
                                {
                                    lr->buffer[lr->bufPtr] = lr->saved;
                                    flags |= HFLG_Restored;
                                }

                                lm->line = (UBYTE *)lm+sizeof(struct lineMsg);

                                for (l = 0; lr->bufPtr<lr->howLong; l++)
                                    lm->line[l] = lr->buffer[lr->bufPtr++];

                                lm->len = l;
                                PutMsg(resPort,(struct Message *)lm);

                                if (flags & HFLG_NotifyLen)
                                    notifyMessage(notifyPort,app,HSMTYPE_Done,l,0);
                            }
                            else
                            {
                                err = MSG_Error_NoMem;
                                break;
                            }
                        }
                        else
                        {
                            l = recv(sock,lr->buffer,LR_BUFSIZE,0);
                            if (l<0)
                            {
                                err = MSG_Error_Recv;
                                break;
                            }

                            if (l==0) break;

                            if (lm = createStatusMsg(MTYPE_Line,tn,l+1))
                            {
                                lm->line = (UBYTE *)lm+sizeof(struct lineMsg);
                                copymem(lm->line,lr->buffer,l);
                                lm->line[l] = 0;
                                lm->len = l;
                                PutMsg(resPort,(struct Message *)lm);

                                if (flags & HFLG_NotifyLen)
                                    notifyMessage(notifyPort,app,HSMTYPE_Done,l,0);
                            }
                            else
                            {
                                err = MSG_Error_NoMem;
                                break;
                            }
                        }
                    }
                }

                if (err && (Errno()==4))
                    err = MSG_Error_Aborted;

                if (sock>=0)
                {
                    shutdown(sock,2);
                    CloseSocket(sock);
                }

                CloseLibrary(SocketBase);

                notifyMessage(notifyPort,app,HSMTYPE_Busy,FALSE,0);
            }
            else err = MSG_Error_NoSocketBase;
        }

        if (location) freeArbitrateVecPooled(location);
    }
    else err = MSG_Error_NoMem;

    FreeSignal(sig);

    notifyStatus(sem,resPort,tn,MSG_Status_Done,err);

    ObtainSemaphore(sem);
    FEED(tn->tn_User)->proc = NULL;
    ReleaseSemaphore(sem);

    msg->err = err;

    Forbid();
    ReplyMsg((struct Message *)msg);
}

/****************************************************************************/
