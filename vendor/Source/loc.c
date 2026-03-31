
#include "rss.h"
#define CATCOMP_ARRAY
#include "loc.h"

/***********************************************************************/

struct Catalog *
openCatalog(STRPTR name,ULONG minVer,ULONG minRev)
{
    struct Catalog *cat;

    if (cat = OpenCatalog(NULL,name,OC_BuiltInLanguage,(ULONG)"english",TAG_DONE))
    {
        ULONG ver = cat->cat_Version;

        if ((ver<minVer) ? TRUE : ((ver==minVer) ? (cat->cat_Revision<minRev) : FALSE))
        {
            CloseCatalog(cat);
            cat = NULL;
        }
    }

    return cat;
}

/***********************************************************************/

STRPTR
getStaticString(ULONG id)
{
    register int low, high;

    for (low = 0, high = (sizeof(CatCompArray)/sizeof(struct CatCompArrayType))-1; low<=high; )
    {
        register int                     mid = (low+high)>>1, cond;
        register struct CatCompArrayType *cca = (struct CatCompArrayType *)CatCompArray+mid;

        if ((cond = id-cca->cca_ID)==0)
            return cca->cca_Str;

        if (cond<0) high = mid-1;
        else low = mid+1;
    }

    return (STRPTR)"";
}

/****************************************************************************/

STRPTR
getString(ULONG id)
{
    register int low, high;

    for (low = 0, high = (sizeof(CatCompArray)/sizeof(struct CatCompArrayType))-1; low<=high; )
    {
        register int                     mid = (low+high)>>1, cond;
        register struct CatCompArrayType *cca = (struct CatCompArrayType *)CatCompArray+mid;

        if ((cond = id-cca->cca_ID)==0)
            return g_cat ? GetCatalogStr(g_cat,id,cca->cca_Str) : cca->cca_Str;

        if (cond<0) high = mid-1;
        else low = mid+1;
    }

    return (STRPTR)"";
}

/****************************************************************************/

void
localizeStrings(STRPTR *s)
{
    for (; *s; s++) *s = getString((ULONG)*s);
}

/***********************************************************************/

void
localizeMenus(struct NewMenu *menu)
{
    while (menu->nm_Type!=NM_END)
    {
        if (menu->nm_Label!=NM_BARLABEL)
            menu->nm_Label = getString((ULONG)menu->nm_Label);

        if (menu->nm_CommKey) menu->nm_CommKey = getString((ULONG)menu->nm_CommKey);

        menu++;
    }
}

/***********************************************************************/

void
localizeButtonsBar(struct MUIS_TheBar_Button *button)
{
    while (button->img!=MUIV_TheBar_End)
    {
        if (button->text) button->text = getString((ULONG)button->text);
        if (button->help) button->help = getString((ULONG)button->help);

        button++;
    }
}

/***********************************************************************/

ULONG
getKeyCharID(LONG id)
{
    STRPTR string;
    ULONG  res = 0;

    string = getString(id);

    for (; *string && *string!='_'; string++);
    if (*string++) res = ToLower(*string);

    return res;
}

/***********************************************************************/
