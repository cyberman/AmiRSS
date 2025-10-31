
#include "rss.h"
#include "URI.h"
#define CATCOMP_NUMBERS
#include "loc.h"

/****************************************************************************/

static STRPTR
mstrdup(APTR pool,STRPTR s)
{
    STRPTR res;
    size_t len;

    if (!s) return NULL;

    len = strlen(s)+1;

    if (res = pool ? allocVecPooled(pool,len) : allocArbitrateVecPooled(len))
        copymem(res,s,len);

    return res;
}

/****************************************************************************/

static STRPTR
mstrndup(APTR pool,STRPTR s,int n)
{
    STRPTR res;

    if (!s) return NULL;

    if (res = pool ? allocVecPooled(pool,n+1) : allocArbitrateVecPooled(n+1))
    {
        copymem(res,s,n);
        res[n] = '\0';
    }

    return res;
}

/****************************************************************************/

struct scheme
{
    const STRPTR    name;
    ULONG           schemeid;
    unsigned short  port;
};

static struct scheme schemes[] =
{
    "http",     URISCHEME_HTTP,     DEFAULT_HTTP_PORT,
    "mailto",   URISCHEME_MAILTO,   0xFFFF,
    "file",     URISCHEME_FILE,     0xFFFF,
    "amrss",    URISCHEME_AMRSS,    0xFFFF,
    "ftp",      URISCHEME_HTTPS,    DEFAULT_FTP_PORT,
    "https",    URISCHEME_FTP_DATA, DEFAULT_HTTPS_PORT,
    "gopher",   URISCHEME_FTP,      DEFAULT_GOPHER_PORT,
    "wais",     URISCHEME_GOPHER,   DEFAULT_WAIS_PORT,
    "snews",    URISCHEME_WAIS,     DEFAULT_SNEWS_PORT,
    "prospero", URISCHEME_SNEWS,    DEFAULT_PROSPERO_PORT,
    "finger",   URISCHEME_PROSPERO, DEFAULT_FINGER_PORT,
    "daytime",  URISCHEME_FINGER,   DEFAULT_DAYTIME_PORT,
    "smtp",     URISCHEME_DAYTIME,  DEFAULT_SMTP_PORT,
    "pop",      URISCHEME_SMTP,     DEFAULT_POP_PORT,
    "nntp",     URISCHEME_POP,      DEFAULT_NNTP_PORT,

    NULL,       0,                  0xFFFF
};

/****************************************************************************/

static unsigned short
defaultPort(STRPTR name,ULONG *id)
{
    struct scheme *scheme;

    for (scheme = schemes; scheme->name; ++scheme)
        if (!stricmp(scheme->name,name))
        {
            *id = scheme->schemeid;
            return scheme->port;
        }

    return 0;
}

/****************************************************************************/

#define T_COLON     0x01    /* ':'  */
#define T_SLASH     0x02    /* '/'  */
#define T_QUESTION  0x04    /* '?'  */
#define T_HASH      0x08    /* '#'  */
#define T_NUL       0x80    /* '\0' */

static UBYTE URIDelims[256] =
{
    T_NUL,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,T_HASH,0,0,0,0,
    0,0,0,0,0,0,0,T_SLASH,0,0,0,0,0,0,0,0,0,0,T_COLON,0,
    0,0,0,T_QUESTION,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/* it works like this:
    if (URIDelims[ch] & NOTEND_foobar) {
    then we're not at a delimiter for foobar
    }
*/

/* Note that we optimize the scheme scanning here, we cheat and let the
 * compiler know that it doesn't have to do the & masking.
 */
#define NOTEND_SCHEME   (0xff)
#define NOTEND_HOSTINFO (T_SLASH | T_QUESTION | T_HASH | T_NUL)
#define NOTEND_PATH     (T_QUESTION | T_HASH | T_NUL)

/*
 * parse_uri_components():
 * Parse a given URI, fill in all supplied fields of a URI
 * structure. This eliminates the necessity of extracting host, port,
 * path, query info repeatedly in the modules.
 * Side effects:
 *  - fills in fields of URI *uptr
 *  - none on any of the r->* fields
 */

int
parseURI(STRPTR URI,struct URI *uptr,ULONG flags)
{
    STRPTR s, s1, hostinfo;
    STRPTR endstr;

    /*
     * We assume the processor has a branch predictor like most --
     * it assumes forward branches are untaken and backwards are taken.
     *  That's the reason for the gotos.  -djg
     */
    if (URI[0]=='/')
    {

deal_with_path:
        /*
         * we expect URI to point to first character of path ... remember
         * that the path could be empty -- http://foobar?query for example
         */

        if (flags & URIFLG_SHORT) uptr->path = mstrdup(uptr->pool,URI);
        else
        {
            for (s = URI; !(URIDelims[*(STRPTR)s] & NOTEND_PATH); s++);

            if (s!=URI)
            {
                if (!(uptr->path = mstrndup(uptr->pool,URI,s-URI)))
                    return MSG_Error_NoMem;
            }

            if (!*s) return 0;

            if (*s=='?')
            {
                s++;

                if (s1 = strchr(s,'#'))
                {
                    if (!(uptr->fragment = mstrdup(uptr->pool,s1+1)))
                        return MSG_Error_NoMem;

                    if (!(uptr->query = mstrndup(uptr->pool,s,s1-s)))
                        return MSG_Error_NoMem;
                }
                else
                {
                    if (!(uptr->query = mstrdup(uptr->pool,s)))
                        return MSG_Error_NoMem;
                }

                return 0;
            }

            /* otherwise it's a fragment */
            if (!(uptr->fragment = mstrdup(uptr->pool,s+1)))
                return MSG_Error_NoMem;
        }

        return 0;
    }

    /* find the scheme: */
    for (s = URI; !(URIDelims[*(STRPTR)s] & NOTEND_SCHEME); s++);

    if (s==URI || s[0]!=':' || s[1]!='/' || s[2]!='/')
    {
        /* alfie: no scheme, http:// is the default */

        if (!(uptr->scheme = mstrndup(uptr->pool,"http",4)))
            return MSG_Error_NoMem;

        s = URI;
    }
    else
    {
        if (!(uptr->scheme = mstrndup(uptr->pool,URI,s-URI)))
            return MSG_Error_NoMem;

        s += 3;
    }

    hostinfo = s;
    for (; !(URIDelims[*(STRPTR)s] & NOTEND_HOSTINFO); s++);

    URI = s;    /* whatever follows hostinfo is start of URI */
    if (!(uptr->hostinfo = mstrndup(uptr->pool,hostinfo,URI-hostinfo)))
        return MSG_Error_NoMem;

    /*
     * If there's a username:password@host:port, the @ we want is the last @...
     * too bad there's no memrchr()... For the C purists, note that hostinfo
     * is definately not the first character of the original URI so therefore
     * &hostinfo[-1] < &hostinfo[0] ... and this loop is valid C.
     */
    do
    {
        --s;
    } while (s>=hostinfo && *s!='@');

    if (s<hostinfo)
    {
        /* again we want the common case to be fall through */
deal_with_host:
        /*
         * We expect hostinfo to point to the first character of
         * the hostname.  If there's a port it is the first colon.
         */
        s = memchr(hostinfo,':',URI-hostinfo);
        if (!s)
        {
            /* we expect the common case to have no port */
            if (!(uptr->hostname = mstrndup(uptr->pool,hostinfo,URI-hostinfo)))
                return MSG_Error_NoMem;

            uptr->port = defaultPort(uptr->scheme,&uptr->schemeid);
            goto deal_with_path;
        }
        else defaultPort(uptr->scheme,&uptr->schemeid);

        if (!(uptr->hostname = mstrndup(uptr->pool,hostinfo,s-hostinfo)))
            return MSG_Error_NoMem;

        s++;

        if (URI!=s)
        {
            UBYTE pbuf[32], *pb;

            if (flags & URIFLG_SHORT)
            {
                int l = URI-s+1;

                if (l+1>sizeof(pbuf)) return MSG_Error_InvalidURL;
                stccpy(pbuf,s,l);
                pb = pbuf;
            }
            else
            {
                if (!(uptr->port_str = mstrndup(uptr->pool,s,URI-s)))
                    return MSG_Error_NoMem;

                pb = uptr->port_str;
            }

            uptr->port = strtol(pb,(char **)&endstr,10);
            if (!*endstr) goto deal_with_path;

            /* Invalid characters after ':' found */
            return MSG_Error_InvalidURL;
        }

        uptr->port = defaultPort(uptr->scheme,&uptr->schemeid);
        goto deal_with_path;
    }

    /* first colon delimits username:password */
    s1 = memchr(hostinfo,':',s-hostinfo);
    if (s1)
    {
        if (!(uptr->user = mstrndup(uptr->pool,hostinfo,s1-hostinfo)))
            return MSG_Error_NoMem;

        s1++;

        if (!(uptr->password = mstrndup(uptr->pool,s1,s-s1)))
            return MSG_Error_NoMem;
    }
    else
    {
        if (!(uptr->user = mstrndup(uptr->pool,hostinfo,s-hostinfo)))
            return MSG_Error_NoMem;
    }

    hostinfo = s+1;
    goto deal_with_host;
}

/****************************************************************************/

void
freeURI(struct URI *uptr)
{
    APTR pool = uptr->pool;

    if (pool)
    {
        if (uptr->scheme)   freeVecPooled(pool,uptr->scheme);
        if (uptr->hostinfo) freeVecPooled(pool,uptr->hostinfo);
        if (uptr->user)     freeVecPooled(pool,uptr->user);
        if (uptr->password) freeVecPooled(pool,uptr->password);
        if (uptr->hostname) freeVecPooled(pool,uptr->hostname);
        if (uptr->port_str) freeVecPooled(pool,uptr->port_str);
        if (uptr->path)     freeVecPooled(pool,uptr->path);
        if (uptr->query)    freeVecPooled(pool,uptr->query);
        if (uptr->fragment) freeVecPooled(pool,uptr->fragment);
    }
    else
    {
        if (uptr->scheme)   freeArbitrateVecPooled(uptr->scheme);
        if (uptr->hostinfo) freeArbitrateVecPooled(uptr->hostinfo);
        if (uptr->user)     freeArbitrateVecPooled(uptr->user);
        if (uptr->password) freeArbitrateVecPooled(uptr->password);
        if (uptr->hostname) freeArbitrateVecPooled(uptr->hostname);
        if (uptr->port_str) freeArbitrateVecPooled(uptr->port_str);
        if (uptr->path)     freeArbitrateVecPooled(uptr->path);
        if (uptr->query)    freeArbitrateVecPooled(uptr->query);
        if (uptr->fragment) freeArbitrateVecPooled(uptr->fragment);
    }

    memset(uptr,0,sizeof(struct URI));
    uptr->pool = pool;
}

/****************************************************************************/
