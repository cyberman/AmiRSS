
#include "rss.h"
#include <dos/dostags.h>
#include <proto/CManager.h>

/* Mem debug */
#ifdef DEBUGMEM
extern ULONG g_tot;
#endif

/***********************************************************************/

#define _HELP(h) ((ULONG)(h)) ? MUIA_ShortHelp   : TAG_IGNORE, ((ULONG)(h)) ? (ULONG)getString((ULONG)(h)) : 0
#define _KEY(k)  ((ULONG)(k)) ? MUIA_ControlChar : TAG_IGNORE, ((ULONG)(k)) ? (ULONG)getKeyCharID((ULONG)(k)) : 0

/***********************************************************************/

#ifndef __MORPHOS__
ULONG STDARGS
DoSuperNew(struct IClass *cl,Object *obj,ULONG tag1,...)
{
    return DoSuperMethod(cl,obj,OM_NEW,&tag1,NULL);
}
#endif

/***********************************************************************/

ULONG
xget(Object *obj,ULONG attr)
{
    ULONG res;

    get(obj,attr,&res);

    return res;
}

/***********************************************************************/

Object *
ostring(ULONG maxlen,ULONG key,ULONG help)
{
    return rootStringObject,
        _HELP(help),
        _KEY(key),
        MUIA_CycleChain,            TRUE,
        StringFrame,
        MUIA_Textinput_AdvanceOnCR, TRUE,
        MUIA_Textinput_MaxLen,      maxlen,
    End;
}

/***********************************************************************/

Object *
oaestring(ULONG maxlen,ULONG key,ULONG help)
{
    return rootStringObject,
        _HELP(help),
        _KEY(key),
        MUIA_CycleChain,            TRUE,
        StringFrame,
        MUIA_Textinput_AdvanceOnCR, TRUE,
        MUIA_Textinput_MaxLen,      maxlen,
        MUIA_Textinput_AutoExpand,  TRUE,
    End;
}

/***********************************************************************/

Object *
ocheck(ULONG key,ULONG help)
{
    Object *obj;

    if (obj = MUI_MakeObject(MUIO_Checkmark,(ULONG)getString(key)))
        SetAttrs(obj,MUIA_CycleChain,TRUE,_HELP(help),TAG_DONE);

    return obj;
}

/***********************************************************************/

Object *
obutton(ULONG text,ULONG help)
{
    Object *obj;

    if (obj = MUI_MakeObject(MUIO_Button,(ULONG)getString(text)))
        SetAttrs(obj,MUIA_CycleChain,TRUE,_HELP(help),TAG_DONE);

    return obj;
}

/***********************************************************************/

Object *
ocycle(ULONG key,ULONG help,STRPTR *array)
{
    Object *obj;

    if (obj = MUI_MakeObject(MUIO_Cycle,(ULONG)getString(key),(ULONG)array))
        SetAttrs(obj,MUIA_CycleChain,TRUE,_HELP(help),TAG_DONE);

    return obj;
}

/***********************************************************************/

Object *
olabel(ULONG id)
{
    return Label((ULONG)getString(id));
}

/****************************************************************************/

Object *
ollabel(ULONG id)
{
    return LLabel((ULONG)getString(id));
}

/***********************************************************************/

Object *
olabel1(ULONG id)
{
    return Label1((ULONG)getString(id));
}

/***********************************************************************/

Object *
ollabel1(ULONG id)
{
    return LLabel1((ULONG)getString(id));
}

/***********************************************************************/

Object *
olabel2(ULONG id)
{
    return Label2((ULONG)getString(id));
}

/***********************************************************************/

Object *
opopbutton(ULONG img,ULONG help)
{
    Object *obj;

    if (obj = MUI_MakeObject(MUIO_PopButton,img))
        SetAttrs(obj,MUIA_CycleChain,TRUE,_HELP(help),TAG_DONE);

    return obj;
}

/****************************************************************************/

Object *
ofile(Object **obj,ULONG size,ULONG txt,ULONG help,ULONG title,ULONG drawersOnly)
{
    Object *o, *bt;

    o = PopaslObject,
        _HELP(help),
        MUIA_Popstring_String, *obj = ostring(size,txt,0),
        MUIA_Popstring_Button, bt = PopButton(drawersOnly ? MUII_PopDrawer : MUII_PopFile),
        MUIA_Popasl_Type,      ASL_FileRequest,
        ASLFR_DrawersOnly,     drawersOnly,
        title ? ASLFR_TitleText : TAG_IGNORE, title ? getString(title) : 0,
    End;

    return o;
}

/***********************************************************************/

Object *
oslider(ULONG key,ULONG help,LONG min,LONG max)
{
    Object *obj;

    if (obj = MUI_MakeObject(MUIO_Slider,(ULONG)getString(key),min,max))
        SetAttrs(obj,MUIA_CycleChain,TRUE,_HELP(help),TAG_DONE);

    return obj;
}

/***********************************************************************/

APTR
allocVecPooled(APTR pool,ULONG size)
{
    ULONG *mem;

    if (mem = AllocPooled(pool,size = size+sizeof(ULONG))) *mem++ = size;

    return mem;
}

/****************************************************************************/

void
freeVecPooled(APTR pool,APTR mem)
{
    FreePooled(pool,(ULONG *)mem-1,*((ULONG *)mem-1));
}

/****************************************************************************/

void *
reallocVecPooled(APTR pool,APTR mem,size_t size)
{
    APTR  new;
    ULONG sold = 0; //gcc

    if (size==0) return NULL;

    if (mem)
    {
        sold = *((ULONG *)mem-1);

        if (sold-sizeof(ULONG)>=size) return mem;
    }

    if (new = allocVecPooled(pool,size))
    {
        if (mem) copymem(new,mem,sold);
    }

    if (mem) freeVecPooled(pool,mem);

    return new;
}

/****************************************************************************/

void *
reallocVecPooledNC(APTR pool,APTR mem,size_t size)
{
    if (size==0) return NULL;

    if (mem)
    {
        if (*((ULONG *)mem-1)-sizeof(ULONG)>=size) return mem;
        freeVecPooled(pool,mem);
    }

    return allocVecPooled(pool,size);
}

/****************************************************************************/

APTR
allocArbitratePooled(ULONG size)
{
    APTR mem;

    ObtainSemaphore(&g_poolSem);

    mem = AllocPooled(g_pool,size);

#ifdef DEBUGMEM
    if (mem) g_tot += size;
    NewRawDoFmt("alloc %ld - %ld\n",1,1,size,g_tot);
#endif

    ReleaseSemaphore(&g_poolSem);

    return mem;
}

/***********************************************************************/

void
freeArbitratePooled(APTR mem,ULONG size)
{
    ObtainSemaphore(&g_poolSem);

    FreePooled(g_pool,mem,size);

#ifdef DEBUGMEM
    g_tot -= size;
    NewRawDoFmt("free %lx %lx - %ld\n",1,1,mem,size,g_tot);
#endif

    ReleaseSemaphore(&g_poolSem);
}

/***********************************************************************/

APTR
allocArbitrateVecPooled(ULONG size)
{
    APTR mem;

    ObtainSemaphore(&g_poolSem);

    mem = allocVecPooled(g_pool,size);

#ifdef DEBUGMEM
    if (mem) g_tot += size+4;
    NewRawDoFmt("allocVec %ld - %ld\n",1,1,size,g_tot);
#endif

    ReleaseSemaphore(&g_poolSem);

    return mem;
}

/***********************************************************************/

void
freeArbitrateVecPooled(APTR mem)
{
    ObtainSemaphore(&g_poolSem);

#ifdef DEBUGMEM
    g_tot -= *((ULONG *)mem-1);
    NewRawDoFmt("free vec %lx - %ld\n",1,1,mem,g_tot);
#endif

    freeVecPooled(g_pool,mem);

    ReleaseSemaphore(&g_poolSem);
}

/***********************************************************************/

APTR
reallocArbitrateVecPooled(APTR mem,size_t size)
{
    APTR  new;
    ULONG sold = 0; //gcc

    if (size==0) return NULL;

    if (mem)
    {
        sold = *((ULONG *)mem-1);

        if (sold-sizeof(ULONG)>=size) return mem;
    }

    if (new = allocArbitrateVecPooled(size))
    {
        if (mem) copymem(new,mem,sold);
    }

    if (mem) freeArbitrateVecPooled(mem);

    return new;
}

/***********************************************************************/

APTR
reallocArbitrateVecPooledNC(APTR mem,size_t size)
{
    if (size==0) return NULL;

    if (mem)
    {
        if (*((ULONG *)mem-1)-sizeof(ULONG)>=size) return mem;
        freeArbitrateVecPooled(mem);
    }

    return allocArbitrateVecPooled(size);
}

/***********************************************************************/

ULONG
openWindow(Object *app,Object *win)
{
    ULONG v;

    if (win)
    {
        set(win,MUIA_Window_Open,TRUE);
        get(win,MUIA_Window_Open,&v);
        if (!v) get(app,MUIA_Application_Iconified,&v);
    }
    else v = FALSE;

    if (!v) DisplayBeep(0);

    return v;
}

/***********************************************************************/

#ifndef __MORPHOS__
UWORD fmtfunc[] = { 0x16c0, 0x4e75 };

void
msprintf(STRPTR buf,STRPTR fmt,...)
{
    RawDoFmt(fmt,&fmt+1,(APTR)fmtfunc,buf);
}
#endif

/***********************************************************************/

struct stream
{
    UBYTE   *buf;
    int     size;
    int     counter;
    int     stop;
};

#ifdef __MORPHOS__
static void
snprintfStuff(void)
{
    struct stream *s = (struct stream *)REG_A3;
    UBYTE          c = (BYTE)REG_D0;
#else
static void ASM
snprintfStuff(REG(a3,struct stream *s),REG(d0,UBYTE c))
{
#endif
    if (!s->stop)
    {
        if (++s->counter>=s->size)
        {
            *(s->buf) = 0;
            s->stop   = 1;
        }
        else *(s->buf++) = c;
    }
}

#ifdef __MORPHOS__
static struct EmulLibEntry snprintfStuffTrap = {TRAP_LIB,0,(void *)&snprintfStuff};
#endif

int
msnprintf(STRPTR buf,int size,STRPTR fmt,...)
{
    struct stream s;
    #ifdef __MORPHOS__
    va_list       va;
    va_start(va,fmt);
    #endif

    s.buf     = buf;
    s.size    = size;
    s.counter = 0;
    s.stop    = 0;

    #ifdef __MORPHOS__
    RawDoFmt(fmt,va->overflow_arg_area,(APTR)&snprintfStuffTrap,&s);
    va_end(va);
    #else
    RawDoFmt(fmt,&fmt+1,(APTR)snprintfStuff,&s);
    #endif

    return s.counter-1;
}

/***********************************************************************/

#define SAMINSIZE   4096
#define SASMMINSIZE  256

#ifdef __MORPHOS__
static void
asnprintfStuff(void)
{
    struct astream *s = (struct astream *)REG_A3;
    UBYTE           c = (BYTE)REG_D0;
#else
static void ASM
asnprintfStuff(REG(a3,struct astream *s),REG(d0,UBYTE c))
{
#endif
    if (!(s->flags & ASFLG_Stop))
    {
        if (++s->counter>=s->size)
        {
            STRPTR new, old = s->buf;
            int    osize, nsize;

            osize = s->size;
            nsize = (s->flags & ASFLG_Small) ? (osize+osize+osize)>>1 : osize<<1;

            if (new = allocArbitrateVecPooled(nsize))
            {
                copymem(new,old,osize);

                s->buf   = new;
                s->ptr   = new+osize-1;
                s->size  = nsize;

                *(s->ptr++) = c;
            }
            else
            {
                s->buf     = NULL;
                s->counter = 0;
                s->flags  |= ASFLG_Stop;
            }

            freeArbitrateVecPooled(old);
        }
        else *(s->ptr++) = c;
    }
}

#ifdef __MORPHOS__
static struct EmulLibEntry asnprintfStuffTrap = {TRAP_LIB,0,(void *)&asnprintfStuff};
#endif

STRPTR STDARGS
asnprintf(struct astream *st,STRPTR fmt,...)
{
    if (!(st->flags & ASFLG_Stop))
    {
        #ifdef __MORPHOS__
        va_list va;
        va_start(va,fmt);
        #endif

        if (st->buf)
        {
            if (st->ptr)
            {
                st->ptr--;
                st->counter--;
            }
        }
        else
        {
            if (st->buf = allocArbitrateVecPooled((st->flags & ASFLG_Small) ? SASMMINSIZE : SAMINSIZE))
            {
                st->ptr     = st->buf;
                st->size    = (st->flags & ASFLG_Small) ? SASMMINSIZE : SAMINSIZE;
                st->counter = 0;
            }
            else st->flags |= ASFLG_Stop;
        }

        if (st->buf)
            #ifdef __MORPHOS__
            {
	        RawDoFmt(fmt,va->overflow_arg_area,(APTR)&asnprintfStuffTrap,st);
        	va_end(va);
	    }
            #else
            RawDoFmt(fmt,&fmt+1,(APTR)asnprintfStuff,st);
	    #endif
    }

    return st->buf;
}

void reset_asprint(struct astream *st)
{
    st->ptr     = st->buf+1;
    st->counter = 0;
}

/****************************************************************************/

ULONG
goURLFun(STRPTR URL)
{
    if (URL && *URL && OpenURLBase)
    {
        URL_OpenA(URL,NULL);

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/

ULONG
miniMailFun(Object *app,STRPTR URL)
{
    if (URL && !strnicmp(URL,"mailto:",7))
    {
        DoMethod(app,MUIM_App_MiniMailWin,(ULONG)(URL+7));

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************/

void
bookmarkFun(STRPTR URL)
{
    struct Library *CManagerBase;

    if (CManagerBase = OpenLibrary("CManager.library",0))
    {
        struct CMEntry *entry;
        ULONG          type;

        if (!strnicmp(URL,"mailto:",7))
        {
            type = CMEntry_User;
            URL += 7;
        }
        else type = CMEntry_WWW;

        if (entry = CM_AllocEntry(type))
        {
            switch (type)
            {
                case CMEntry_User:
                    stccpy(CMUSER(entry)->EMail,URL,sizeof(CMUSER(entry)->EMail));
                    break;

                default:
                    stccpy(CMWWW(entry)->WWW,URL,sizeof(CMWWW(entry)->WWW));
                    break;
            }

            stccpy(CMUSER(entry)->Name,URL,sizeof(CMUSER(entry)->Name));

            CM_AddEntry(entry);
            CM_FreeEntry(entry);
        }

        CloseLibrary(CManagerBase);
    }
}

/***********************************************************************/

struct entity
{
    STRPTR name;
    int    len;
    ULONG  ch;
};

static struct entity entities[] =
{
    "AElig",      sizeof("AElig"),    198,
    "Aacute",     sizeof("Aacute"),   193,
    "Acirc",      sizeof("Acirc"),    194,
    "Agrave",     sizeof("Agrave"),   192,
    "Aring",      sizeof("Aring"),    197,
    "Atilde",     sizeof("Atilde"),   195,
    "Auml",       sizeof("Auml"),     196,
    "Ccedil",     sizeof("Ccedil"),   199,
    "Dagger",     sizeof("Dagger"),   8225,
    "ETH",        sizeof("ETH"),      208,
    "Eacute",     sizeof("Eacute"),   201,
    "Ecirc",      sizeof("Ecirc"),    202,
    "Egrave",     sizeof("Egrave"),   200,
    "Euml",       sizeof("Euml"),     203,
    "Iacute",     sizeof("Iacute"),   205,
    "Icirc",      sizeof("Icirc"),    206,
    "Igrave",     sizeof("Igrave"),   204,
    "Iuml",       sizeof("Iuml"),     207,
    "Ntilde",     sizeof("Ntilde"),   209,
    "OElig",      sizeof("OElig"),    338,
    "Oacute",     sizeof("Oacute"),   211,
    "Ocirc",      sizeof("Ocirc"),    212,
    "Ograve",     sizeof("Ograve"),   210,
    "Oslash",     sizeof("Oslash"),   216,
    "Otilde",     sizeof("Otilde"),   213,
    "Ouml",       sizeof("Ouml"),     214,
    "Prime",      sizeof("Prime"),    8243,
    "Scaron",     sizeof("Scaron"),   352,
    "THORN",      sizeof("THORN"),    222,
    "Uacute",     sizeof("Uacute"),   218,
    "Ucirc",      sizeof("Ucirc"),    219,
    "Ugrave",     sizeof("Ugrave"),   217,
    "Uuml",       sizeof("Uuml"),     220,
    "Yacute",     sizeof("Yacute"),   221,
    "Yuml",       sizeof("Yuml"),     376,
    "aacute",     sizeof("aacute"),   225,
    "acirc",      sizeof("acirc"),    226,
    "acute",      sizeof("acute"),    180,
    "aelig",      sizeof("aelig"),    230,
    "agrave",     sizeof("agrave"),   224,
    "amp",        sizeof("amp"),      38,
    "apos",       sizeof("apos"),     39,
    "aring",      sizeof("aring"),    229,
    "atilde",     sizeof("atilde"),   227,
    "auml",       sizeof("auml"),     228,
    "bdquo",      sizeof("bdquo"),    8222,
    "brvbar",     sizeof("brvbar"),   166,
    "bull",       sizeof("bull"),     8226,
    "ccedil",     sizeof("ccedil"),   231,
    "cedil",      sizeof("cedil"),    184,
    "cent",       sizeof("cent"),     162,
    "circ",       sizeof("circ"),     710,
    "copy",       sizeof("copy"),     169,
    "curren",     sizeof("curren"),   164,
    "dagger",     sizeof("dagger"),   8224,
    "deg",        sizeof("deg"),      176,
    "divide",     sizeof("divide"),   247,
    "eacute",     sizeof("eacute"),   233,
    "ecirc",      sizeof("ecirc"),    234,
    "egrave",     sizeof("egrave"),   232,
    "empty",      sizeof("empty"),    8709,
    "emsp",       sizeof("emsp"),     8195,
    "ensp",       sizeof("ensp"),     8194,
    "eth",        sizeof("eth"),      240,
    "euml",       sizeof("euml"),     235,
    "euro",       sizeof("euro"),     8364,
    "fnof",       sizeof("fnof"),     402,
    "frac12",     sizeof("frac12"),   189,
    "frac14",     sizeof("frac14"),   188,
    "frac34",     sizeof("frac34"),   190,
    "frasl",      sizeof("frasl"),    8260,
    "ge",         sizeof("ge"),       8805,
    "gt",         sizeof("gt"),       62,
    "hellip",     sizeof("hellip"),   8230,
    "iacute",     sizeof("iacute"),   237,
    "icirc",      sizeof("icirc"),    238,
    "iexcl",      sizeof("iexcl"),    161,
    "igrave",     sizeof("igrave"),   236,
    "iquest",     sizeof("iquest"),   191,
    "iuml",       sizeof("iuml"),     239,
    "lang",       sizeof("lang"),     9001,
    "laquo",      sizeof("laquo"),    171,
    "ldquo",      sizeof("ldquo"),    8220,
    "le",         sizeof("le"),       8804,
    "lowast",     sizeof("lowast"),   8727,
    "loz",        sizeof("loz"),      9674,
    "lsaquo",     sizeof("lsaquo"),   8249,
    "lsquo",      sizeof("lsquo"),    8216,
    "lt",         sizeof("lt"),       60,
    "macr",       sizeof("macr"),     175,
    "mdash",      sizeof("mdash"),    8212,
    "micro",      sizeof("micro"),    181,
    "middot",     sizeof("middot"),   183,
    "minus",      sizeof("minus"),    8722,
    "nbsp",       sizeof("nbsp"),     160,
    "ndash",      sizeof("ndash"),    8211,
    "not",        sizeof("not"),      172,
    "ntilde",     sizeof("ntilde"),   241,
    "oacute",     sizeof("oacute"),   243,
    "ocirc",      sizeof("ocirc"),    244,
    "oelig",      sizeof("oelig"),    339,
    "ograve",     sizeof("ograve"),   242,
    "oline",      sizeof("oline"),    8254,
    "ordf",       sizeof("ordf"),     170,
    "ordm",       sizeof("ordm"),     186,
    "oslash",     sizeof("oslash"),   248,
    "otilde",     sizeof("otilde"),   245,
    "ouml",       sizeof("ouml"),     246,
    "para",       sizeof("para"),     182,
    "permil",     sizeof("permil"),   8240,
    "plusmn",     sizeof("plusmn"),   177,
    "pound",      sizeof("pound"),    163,
    "prime",      sizeof("prime"),    8242,
    "quot",       sizeof("quot"),     34,
    "rang",       sizeof("rang"),     9002,
    "raquo",      sizeof("raquo"),    187,
    "reg",        sizeof("reg"),      174,
    "rdquo",      sizeof("rdquo"),    8221,
    "rsaquo",     sizeof("rsaquo"),   8250,
    "rsquo",      sizeof("rsquo"),    8217,
    "sbquo",      sizeof("sbquo"),    8218,
    "scaron",     sizeof("scaron"),   353,
    "sdot",       sizeof("sdot"),     8901,
    "sect",       sizeof("sect"),     167,
    "shy",        sizeof("shy"),      173,
    "sim",        sizeof("sim"),      8764,
    "sup1",       sizeof("sup1"),     185,
    "sup2",       sizeof("sup2"),     178,
    "sup3",       sizeof("sup3"),     179,
    "szlig",      sizeof("szlig"),    223,
    "thinsp",     sizeof("thinsp"),   8201,
    "thorn",      sizeof("thorn"),    254,
    "tilde",      sizeof("tilde"),    732,
    "times",      sizeof("times"),    215,
    "trade",      sizeof("trade"),    8482,
    "uacute",     sizeof("uacute"),   250,
    "ucirc",      sizeof("ucirc"),    251,
    "ugrave",     sizeof("ugrave"),   249,
    "uml",        sizeof("uml"),      168,
    "uuml",       sizeof("uuml"),     252,
    "yacute",     sizeof("yacute"),   253,
    "yen",        sizeof("yen"),      165,
    "yuml",       sizeof("yuml"),     255,
    "zwj",        sizeof("zwj"),      8205,
    "zwnj",       sizeof("zwnj"),     8204
};

#define NENTITIES (sizeof(entities)/sizeof(struct entity))

static struct entity *
findEntity(UBYTE *name)
{
    int a = 0, b = NENTITIES-1, m, c;

    while (a<=b)
    {
        m = (a+b)/2;
        c = strncmp(entities[m].name,name,entities[m].len-1);

        if (c == 0) return &entities[m];

        if (c<0) a = m+1;
        else b = m-1;
    }

    return NULL;
}

/***********************************************************************/

/*static INLINE void
converToUTF8(ULONG n,STRPTR dest)
{
    UTF32  *src_ptr = &n;
    STRPTR d;

    d = &dest[1];
    CodesetsConvertUTF32toUTF8(&src_ptr,src_ptr+1,&d,d+6,CONVFLG_StrictConversion);
    *d = 0;
}*/

static INLINE void
converToUTF8(ULONG n,STRPTR dest)
{
    if ((n & 0x80)==0) *dest++ = n;
    else
    {
        *dest++ = 0xc0 | ((n >> 6) & 0x03);
        *dest++ = 0x80 | (n & 0x3f);
    }

    *dest = 0;
}

static LONG INLINE
substWinChars(ULONG n)
{
    switch (n)
    {
        case 0x85:                       return '.';
        case 0x91: case 0x92:            return '\'';
        case 0x93: case 0x94:            return '\"';
        case 0x95:                       return '>';
        case 0x96: case 0x97:            return '-';
        case 0xA0:                       return ' ';
        default:                         return -1;
    }
}

void
etranslate(STRPTR buffer,STRPTR to,int len,ULONG doSubstWinChars)
{
    STRPTR p = buffer;

    while (*p)
    {
        ULONG n;

        n = *p;

        if (n=='&')
        {
            UBYTE  ebuf[12];
            STRPTR q;

            *ebuf = 0;

            q = p;
            if (!*(++q)) break;

            if (*q=='#')
            {
                n = 0;
                if (isdigit(q[1]))
                {
                    while (*(++q) && isdigit(*q)) n = 10*n+(*q-'0');
                }
                else
                    if (toupper(q[1])=='X')
                    {
                        q++;

                        while (*(++q) && isxdigit(*q))
                        {
                            n = 16*n+((*q<='9') ? (*q-'0') : (toupper(*q)-'A'+10));
                        }
                    }

                if (*q)
                {
                    int l;

                    if (*q!=';') q--;
                    l = q-p+1;
                    if (l>11) l = 11;
                    strncpy(ebuf,p,l);
                    ebuf[l] = '\0';
                }
                else q--;
            }
            else
            {
                struct entity *cd;
                UBYTE         name[8];
                int           i;

                for (i = 0; *q && i<7 && (isalnum(*q) || *q=='.' || *q=='-'); q++, i++) name[i] = *q;
                name[i] = '\0';

                if (cd = findEntity(name))
                {
                    q = p+cd->len;
                    if (*q)
                    {
                        if (*q!=';') q--;
                    }
                    else q--;

                    n = cd->ch;
                }
                else q = p;
            }

            if (n<=127)
            {
                if (--len==0) goto end;
                *to++ = n;
            }
            else
            {
                STRPTR r = NULL;

                if (n>256)
                {
                    switch (n)
                    {
                        case 338:   r = "OE";             break;
                        case 339:   r = "oe";             break;
                        case 352:   n = (UBYTE)'S';       break;
                        case 353:   n = (UBYTE)'s';       break;
                        case 376:   n = (UBYTE)'Y';       break;
                        case 402:   n = (UBYTE)'f';       break;
                        case 710:   n = (UBYTE)'°';       break;
                        case 732:   n = (UBYTE)'~';       break;
                        case 8194:  n = (UBYTE)' ';       break;
                        case 8195:  n = (UBYTE)' ';       break;
                        case 8201:  n = (UBYTE)' ';       break;
                        case 8204:  n = (UBYTE)' ';       break;
                        case 8205:  n = (UBYTE)' ';       break;
                        case 8211:  n = (UBYTE)'-';       break;
                        case 8212:  n = (UBYTE)'-';       break;
                        case 8216:  n = (UBYTE)'`';       break;
                        case 8217:  n = (UBYTE)'\'';      break;
                        case 8218:  n = (UBYTE)'"';       break;
                        case 8220:  n = (UBYTE)'"';       break;
                        case 8221:  n = (UBYTE)'"';       break;
                        case 8222:  n = (UBYTE)'"';       break;
                        case 8224:  n = (UBYTE)'+';       break;
                        case 8225:  n = (UBYTE)'+';       break;
                        case 8226:  n = (UBYTE)'·';       break;
                        case 8230:  r = "...";            break;
                        case 8240:  r = "°/..";           break;
                        case 8242:  n = (UBYTE)'\'';      break;
                        case 8243:  n = (UBYTE)'"';       break;
                        case 8249:  n = (UBYTE)'<';       break;
                        case 8250:  n = (UBYTE)'>';       break;
                        case 8254:  n = (UBYTE)'¯';       break;
                        case 8260:  n = (UBYTE)'/';       break;
                        case 8482:  r = "TM";             break;
                        case 8364:  n = 128;              break;
                        case 8709:  n = (UBYTE)'ø';       break;
                        case 8722:  n = (UBYTE)'-';       break;
                        case 8727:  n = (UBYTE)'*';       break;
                        case 8764:  n = (UBYTE)'~';       break;
                        case 8804:  r = "<=";             break;
                        case 8805:  r = ">=";             break;
                        case 8901:  n = (UBYTE)'·';       break;
                        case 9001:  n = (UBYTE)'<';       break;
                        case 9002:  n = (UBYTE)'>';       break;
                        case 9674:  n = (UBYTE)'¤';       break;

                        default:    r = ebuf;
                    }

                    if (r)
                    {
                        STRPTR a;

                        for (a = r; *a; a++)
                        {
                            if (*a>127)
                            {
                                UBYTE  dest[8];
                                STRPTR b;

                                converToUTF8(*a,dest);

                                for (b = dest+1; *b; b++)
                                {
                                    if (--len==0) goto end;
                                    *to++ = *b;
                                }
                            }
                            else
                            {
                                if (--len==0) goto end;
                                *to++ = *a;
                            }
                        }
                    }
                    else
                    {
                        if (n>127)
                        {
                            UBYTE  dest[8];
                            STRPTR b;

                            converToUTF8(n,dest);

                            for (b = dest+1; *b; b++)
                            {
                                if (--len==0) goto end;
                                *to++ = *b;
                            }
                        }
                        else
                        {
                            if (--len==0) goto end;
                            *to++ = n;
                        }
                    }
                }
                else
                {
                    LONG r;

                    if (doSubstWinChars && (r = substWinChars(n))>0)
                    {
                        if (--len==0) goto end;
                        *to++ = r;
                    }
                    else
                    {
                        UBYTE  dest[8];
                        STRPTR b;

                        converToUTF8(n,dest);

                        for (b = dest+1; *b; b++)
                        {
                            if (--len==0) goto end;
                            *to++ = *b;
                        }
                    }
                }
            }

            p = q+1;
        }
        else
        {
            if (--len==0) goto end;
            *to++ = n;
            p++;
        }
    }

end:
    *to = 0;
}

/***********************************************************************/

void
stripHTML(STRPTR from,STRPTR to,BOOL remMUICodes)
{
    STRPTR f, t;
    int    ignore;

    for (f = from, t = to, ignore = 0; ; )
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
            f++;
            ignore--;
            continue;
        }

        if (ignore)
        {
            f++;
            continue;
        }

        if (remMUICodes && c=='\33') c = '_';
        *t++ = c;
        f++;

        /*if (c!='%')
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
        *t = a*16+b;

        if (remMUICodes && *t=='\33') *t = '_';
        t++;*/
    }

    *t = 0;
}

/***********************************************************************/

ULONG
executeCommand(APTR pool,STRPTR cmd,Object *win,STRPTR value,ULONG fileType)
{
    UBYTE       fname[DEF_FILESIZE];
    STRPTR      buf, f, d;
    STRPTR      screen;
    BPTR        fin, fout;
    ULONG       in, sd, fd;

    if (!value || !*value) return 0;

    if (fileType)
    {
        if (!(fin = Lock(value,SHARED_LOCK)))
            return 0;

        in = NameFromLock(fin,fname,sizeof(fname));
        UnLock(fin);
        if (!in) return 0;
    }
    else
    {
        stccpy(fname,value,sizeof(fname));
    }

    get(win,MUIA_Window_PublicScreen,&screen);
    if (!screen || !*screen) screen = "Workbench";

    if (!(buf = pool ? allocVecPooled(pool,strlen(fname)+strlen(screen)+strlen(cmd)+1) :
                       allocArbitrateVecPooled(strlen(fname)+strlen(screen)+strlen(cmd)+1)))
        return 0;

    in = sd = fd = FALSE;

    for (f = cmd, d = buf; *f; f++)
    {
        if (in)
        {
            switch (*f)
            {
                case 's':
                    if (sd)
                    {
                        if (pool) freeVecPooled(pool,buf);
                        else freeArbitrateVecPooled(buf);

                        return 0;
                    }
                    *d = 0;
                    strcat(d,fname);
                    d += strlen(fname);
                    sd = TRUE;
                    break;

                case 'f':
                    if (fd)
                    {
                        if (pool) freeVecPooled(pool,buf);
                        else freeArbitrateVecPooled(buf);

                        return 0;
                    }
                    *d = 0;
                    strcat(d,screen);
                    d += strlen(screen);
                    fd = TRUE;
                    break;

                default:
                    *d++ = '%';
                    *d++ = *f;
            }

            in = FALSE;
        }
        else
        {
            switch (*f)
            {
                case '%':
                    in = TRUE;
                    break;

                default:
                    *d++ = *f;
                    break;
            }
        }
    }

    *d = 0;

    //Printf("[%s] [%s] [%s] [%ld]\n",cmd,fname,screen,fileType);
    //Printf("%s\n",buf);

#ifdef __MORPHOS__
#define MOSSTACK NP_PPCStackSize, 48000,
#else
#define MOSSTACK
#endif

    if ((fin  = Open("NIL:",MODE_OLDFILE)) && (fout = Open("NIL:",MODE_OLDFILE)))
    {
        SystemTags(buf,SYS_Asynch,      TRUE,
                       SYS_Input,       fin,
                       SYS_Output,      fout,
                       NP_StackSize,    24000,
		       MOSSTACK
                       TAG_DONE);
    }
    else
    {
        if (fin) Close(fin);
    }

    if (pool) freeVecPooled(pool,buf);
    else freeArbitrateVecPooled(buf);

    return 0;
}

/***********************************************************************/

BPTR
openFile(STRPTR name,ULONG mode)
{
    if (AsyncIOBase)
    {
        if (mode==MODE_OLDFILE) mode = MODE_READ;
        else if (mode==MODE_NEWFILE) mode = MODE_WRITE;
             else return NULL;

        return (BPTR)OpenAsync(name,mode,2048);
    }
    else
    {
        return Open(name,mode);
    }
}

/***********************************************************************/

void
closeFile(BPTR file)
{
    if (AsyncIOBase) CloseAsync((struct AsyncFile *)file);
    else Close(file);
}

/***********************************************************************/

LONG
readFile(BPTR file,STRPTR buf,ULONG len)
{
    return AsyncIOBase ? ReadAsync((struct AsyncFile *)file,buf,len) : Read(file,buf,len);
}

/***********************************************************************/

LONG
writeFile(BPTR file,APTR buf,ULONG len)
{
    return (AsyncIOBase) ? WriteAsync((struct AsyncFile *)file,buf,len) : Write(file,buf,len);
}

/***********************************************************************/

