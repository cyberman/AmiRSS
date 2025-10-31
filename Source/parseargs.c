
#include "rss.h"
#include "amrss_rev.h"

/***********************************************************************/

#define ARG_PUBSCREEN   "PUBSCREEN"
#define ARG_URL	        "URL"
#define ARG_COMPACT     "COMPACT"
#define ARG_BWIN        "BWIN"
#define ARG_BACKDROP    "BACKDROP"
#define TEMPLATE        ARG_PUBSCREEN"/K,"ARG_URL"/K,"ARG_COMPACT"/S,"ARG_BWIN"/S,"ARG_BACKDROP"/S"

/***********************************************************************/

LONG
parseArgs(struct parseArgs *pa)
{
    LONG res = FALSE;

    if (_WBenchMsg)
    {
        struct DiskObject *icon;
        struct WBArg      *args;
        BPTR              oldDir;

        args = _WBenchMsg->sm_ArgList;
        oldDir = CurrentDir(args[0].wa_Lock);

        if (icon = GetDiskObject(args[0].wa_Name))
        {
            STRPTR value;

            if (value = FindToolType((STRPTR *)icon->do_ToolTypes,ARG_PUBSCREEN))
            {
                stccpy(pa->pubscreen,value,sizeof(pa->pubscreen));
                pa->flags |= PAFLG_Pubscreen;
            }

            if (value = FindToolType((STRPTR *)icon->do_ToolTypes,ARG_URL))
            {
                if (pa->URL = allocArbitrateVecPooled(strlen(value)+1))
                {
                    strcpy(pa->URL,value);
                    pa->flags |= PAFLG_URL;
                }
    	    }

            if (value = FindToolType((STRPTR *)icon->do_ToolTypes,ARG_BWIN))
                pa->flags |= PAFLG_Compact|PAFLG_BWin;
            else
                if (value = FindToolType((STRPTR *)icon->do_ToolTypes,ARG_COMPACT))
                    pa->flags |= PAFLG_Compact;

            if (value = FindToolType((STRPTR *)icon->do_ToolTypes,ARG_BACKDROP))
                pa->flags |= PAFLG_Backdrop;

            FreeDiskObject(icon);
        }

        CurrentDir(oldDir);

        res = TRUE;
    }
    else
    {
        struct RDArgs *ra;
        APTR          arg[16] = {0};

        if (ra = ReadArgs(TEMPLATE,(LONG *)arg,NULL))
        {
            if (arg[0])
            {
                stccpy(pa->pubscreen,(STRPTR)arg[0],sizeof(pa->pubscreen));
                pa->flags |= PAFLG_Pubscreen;
            }

            if (arg[1])
            {
                if (pa->URL = allocArbitrateVecPooled(strlen((STRPTR)arg[1])+1))
                {
                    strcpy(pa->URL,(STRPTR)arg[1]);
                    pa->flags |= PAFLG_URL;
                }
    	    }

            if (arg[3])
                pa->flags |= PAFLG_Compact|PAFLG_BWin;
            else
                if (arg[2])
                    pa->flags |= PAFLG_Compact;

            if (arg[4])
                pa->flags |= PAFLG_Backdrop;

            FreeArgs(ra);

            res = TRUE;
        }
        else PrintFault(IoErr(),PRG);
    }

    return res;
}

/****************************************************************************/
