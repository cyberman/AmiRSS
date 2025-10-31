
#include "rss.h"

/***********************************************************************/

APTR
NewObject(struct IClass *classPtr,STRPTR classID,... )
{
    APTR    res;
    va_list va;

    va_start(va,classID);
    res = NewObjectA(classPtr,classID,(struct TagItem *)va->overflow_arg_area);
    va_end(va);

    return res;
}

/***********************************************************************/

APTR
MUI_NewObject(STRPTR classID,... )
{
    APTR    res;
    va_list va;

    va_start(va,classID);
    res = MUI_NewObjectA(classID,(struct TagItem *)va->overflow_arg_area);
    va_end(va);

    return res;
}

/***********************************************************************/

APTR
DoSuperNew(struct IClass *cl,Object *obj,...)
{
    APTR    res;
    va_list va;

    va_start(va,obj);
    res = (APTR)DoSuperMethod(cl,obj,OM_NEW,(ULONG)va->overflow_arg_area,NULL);
    va_end(va);

    return res;
}

/***********************************************************************/

