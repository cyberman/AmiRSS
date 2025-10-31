
#include "rss.h"

/****************************************************************************/

static int
words(STRPTR word)
{
    STRPTR c = word;
    int    w = 0;

    if (c)
    {
        while (*c)
        {
            while (*c && (*c==' ')) c++;

            if (*c)
            {
                w++;
                while (*c && (*c!=' ')) c++;
            }
        }
    }

    return w;
}

/***********************************************************************/

static int
cutWords(STRPTR word,int n)
{
    STRPTR c = word;
    int    w = 0;

    if (c)
    {
        while (*c)
        {
            while (*c && (*c==' ')) c++;

            if (*c)
            {
                w++;
                while (*c && (*c!=' ')) c++;

                if (w==n)
                {
                    return c-word;
                }
            }
        }
    }

    return NULL;
}

/***********************************************************************/

struct hdata
{
    STRPTR buf;
    int    len;
};

#ifdef __MORPHOS__
static ULONG
getCharFun(void)
{
    struct Hook *hook = (struct Hook *)REG_A0;
    //Object      *list = (Object *)REG_A2;
    //struct Locale *loc = (struct Locale *)REG_A1;
#else
static ULONG SAVEDS ASM
getCharFun(REG(a0,struct Hook *hook),REG(a2,ULONG dummy),REG(a1,struct Locale *loc))
{
#endif
    struct hdata *hdata = hook->h_Data;

    if (hdata->len--==0) return 0L;
    return *hdata->buf++;
}

static ULONG
parseDate(struct DateStamp *ds,STRPTR sfmt,STRPTR sdate,STRPTR *rest)
{
    UBYTE  		buf[256], *s;
#ifdef __MORPHOS__
    struct EmulLibEntry getCharTrap;
#endif
    struct Hook         getCharHook;
    struct hdata    	hdata;
    ULONG           	res;
    int             	len;

    if (strlen(sdate)>sizeof(buf)) return FALSE;
    if (!sfmt || !*sfmt) return FALSE;

    for (s = sdate; *s && *s==' '; s++);
    if (!*s) return FALSE;
    strcpy(buf,s);

    hdata.buf = buf;
    hdata.len = len = cutWords(buf,words(sfmt));

#ifdef __MORPHOS__
    getCharTrap.Trap        = TRAP_LIB;
    getCharTrap.Extension   = 0;
    getCharTrap.Func        = (APTR)getCharFun;
    getCharHook.h_Entry     = (HOOKFUNC)&getCharTrap;
#else
    getCharHook.h_Entry     = (HOOKFUNC)getCharFun;
#endif

    getCharHook.h_Data = &hdata;

    res = ParseDate(g_loc,ds,sfmt,&getCharHook);

    if (!res && hdata.buf!=buf)
    {
        UBYTE c = *(hdata.buf-1);

        *(hdata.buf-1) = 0;
        hdata.buf = buf;
    	hdata.len = len;
        res = ParseDate(g_loc,ds,sfmt,&getCharHook);
        *(hdata.buf-1) = c;
    }
    if (res && rest) *rest = hdata.buf;

    return res;
}

/***********************************************************************/

struct tzone
{
    STRPTR name;
    int    len;
    int    offset;
};

static struct tzone zones[] =
{
    "GMT",      3,         0,
    "UT",       2,         0,
    "PST",      3,      -480,

    "NST",      3,      -210,
    "NDT",      3,      -150,
    "AST",      3,      -240,
    "ADT",      3,      -180,
    "EST",      3,      -300,
    "EDT",      3,      -240,
    "CST",      3,      -360,
    "CDT",      3,      -300,
    "MST",      3,      -420,
    "MDT",      3,      -360,
    "PDT",      3,      -420,
    "AKST",     4,      -540,
    "AKDT",     4,      -480,
    "HAST",     4,      -600,
    "HADT",     4,      -540,

    "NFT",      3,       690,
    "EDT",      3,       660,
    "CST",      3,       570,
    "CDT",      3,       630,
    "WST",      3,       480,
    "CXT",      3,       420,

    "BST",      3,        60,
    "IST",      3,        60,
    "WET",      3,         0,
    "WEST",     4,        60,
    "CET",      3,        60,
    "CEST",     4,       120,
    "EET",      3,       120,
    "EEST",     4,       180,

    "Z",        1,         0,
    "Y",        1,       720 ,
    "X",        1,       660,
    "W",        1,       600,
    "V",        1,       540,
    "U",        1,       480,
    "T",        1,       420,
    "S",        1,       360,
    "R",        1,       300,
    "Q",        1,       240,
    "P",        1,       180,
    "O",        1,       120,
    "N",        1,       -60,
    "A",        1,        60,
    "B",        1,       120,
    "C",        1,       180,
    "D",        1,       240,
    "E",        1,       300,
    "F",        1,       360,
    "G",        1,       420,
    "H",        1,       480,
    "I",        1,       540,
    "K",        1,       600,
    "L",        1,       660,
    "M",        1,       720,

    NULL
};

static int
findTZone(STRPTR str)
{
    STRPTR  s;
    LONG    v;
    int     offset = 0;

    if (s = strchr(str,'+'))
    {
        STRPTR c;

        if (c = strchr(str,':')) *c = 0;

        if (strlen(s)<=3)
        {
            StrToLong(s,&v);
            offset = 60*v;
        }
        else
        {
            s++;
            offset  = (*s++-'0')*24*60 + (*s++-'0')*60 + (*s-'0');
        }

        if (c)
        {
            StrToLong(c+1,&v);
            offset += v;
        }
    }
    else
        if (s = strchr(str,'-'))
        {
            STRPTR c;

            if (c = strchr(str,':')) *c = 0;

            if (strlen(s)<=3)
            {
                StrToLong(s,&v);
                offset = 60*v;
            }
            else
            {
                s++;
                offset  = -((*s++-'0')*24*60 + (*s++-'0')*60 + (*s-'0'));
            }

            if (c)
            {
                StrToLong(c+1,&v);
            	offset -= v;
            }
        }
        else
        {
            int i;

            for (i = 0; zones[i].name; i++)
            {
                if (s = strstr(str,zones[i].name))
                {
                    offset = zones[i].offset;
                    break;
                }
            }
        }

    return offset;
}

/****************************************************************************/

static void
normalizeDate(struct DateStamp *ds,int offset,int GMTOffset)
{
    LONG days, mins, ticks;

    days  = ds->ds_Days;
    mins  = ds->ds_Minute-offset+GMTOffset;
    ticks = ds->ds_Tick;

    if (mins<0)
    {
        if (--days<0) days = mins = ticks = 0;
        else mins += 1440;
    }
    else
        if (mins>=1440)
        {
            days += mins/1440;
            mins = mins%1440;
        }

    ds->ds_Days   = days;
    ds->ds_Minute = mins;
    ds->ds_Tick   = ticks;
}

/****************************************************************************/

struct str
{
    STRPTR name;
    UBYTE  len;
    UBYTE  decCode;
    UBYTE  code;
};

static struct str sdays[] =
{
    "fri", 3, 0, 5,
    "mon", 3, 0, 1,
    "sat", 3, 0, 6,
    "sun", 3, 0, 0,
    "thu", 3, 0, 4,
    "tue", 3, 0, 2,
    "wed", 3, 0, 3
};

static struct str smonths[] =
{
    "apr", 3,  0, 4,
    "aug", 3,  0, 8,
    "dec", 3,  1, 2,
    "feb", 3,  0, 2,
    "jan", 3,  0, 1,
    "jul", 3,  0, 7,
    "jun", 3,  0, 6,
    "mar", 3,  0, 3,
    "may", 3,  0, 5,
    "nov", 3,  1, 1,
    "oct", 3,  1, 0,
    "sep", 3,  0, 9
};

static struct str *
findStr(STRPTR month,struct str *strs,int num)
{
    int low, high;

    for (low = 0, high = num-1; low<=high; )
    {
        int        mid = (low+high)>>1, cond;
        struct str *str = strs+mid;

        if (!(cond = strnicmp(month,str->name,str->len))) return str;

        if (cond<0) high = mid-1;
        else low = mid+1;
    }

    return NULL;
}

void
getdate(struct DateStamp *ds,STRPTR date,int GMTOffset)
{
    UBYTE   buf[256];
    STRPTR  f, t;
    STRPTR  r;
    ULONG   res;
    int     l, offset;

    memset(ds,0,sizeof(*ds));

    if (parseDate(ds,"%Y-%m-%dT%H:%M:%S",date,NULL))
    {
        offset = findTZone(date+19);
        normalizeDate(ds,offset,GMTOffset);
        return;
    }

    if (parseDate(ds,"%Y-%m-%d",date,NULL))
        return;

    for (f = date, t = buf, l = sizeof(buf); ; )
    {
        struct str *str;
        UBYTE      c = *f;

        if (!c) break;
        if (c=='+' || c=='-') break;

        if ((c==' ') || isdigit(c))
        {
            if (--l==0) break;
            *t++ = c;
            f++;
            continue;
        }

        if (str = findStr(f,sdays,sizeof(sdays)/sizeof(struct str)))
        {
            f += str->len;
            continue;
        }

        if (str = findStr(f,smonths,sizeof(smonths)/sizeof(struct str)))
        {
            if (--l==0) break;
            *t++ = str->decCode+'0';

            if (--l==0) break;
            *t++ = str->code+'0';

            f += str->len;
            continue;
        }

        if (c!=',')
        {
            if (--l==0) break;
            *t++ = c;
        }

        f++;
    }
    *t = 0;

    if (!(res = parseDate(ds,"%d %m %Y %H:%M:%S",buf,&r)))
        if (!(res = parseDate(ds,"%d %m %y %H:%M:%S",buf,&r)))
            if (!(res = parseDate(ds,"%d %m %Y",buf,&r)))
                res = parseDate(ds,"%d %m %y",buf,&r);

    if (res)
    {
        offset = findTZone(r);
        normalizeDate(ds,offset,GMTOffset);
    }
}

/****************************************************************************/
