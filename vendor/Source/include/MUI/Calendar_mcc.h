#ifndef CALENDAR_MCC_H
#define CALENDAR_MCC_H

/*
**  $VER: Calendar_mcc.h 15.2 (9.2.2004)
**  Includes Release 15.2
**
**  (C) Copyright 2004 Alfonso [alfie] Ranieri <alforan@tin.it>
**      All rights reserved
*/

#ifndef LIBRARIES_MUI_H
#include <libraries/mui.h>
#endif

#if defined(__GNUC__)
# pragma pack(2)
#endif

/***********************************************************************/

#define MUIC_Calendar  "Calendar.mcc"
#define CalendarObject MUI_NewObject(MUIC_Calendar

#define CALENDAR_VERSION 15

/***********************************************************************/

#define CALENDARBASE 0xFEC90160

/***********************************************************************/
/*
** Methods
*/

#define MUIM_Calendar_FormatDate    (CALENDARBASE+5)    /* v14  */
#define MUIM_Calendar_Today         (CALENDARBASE+7)    /* v14  */
#define MUIM_Calendar_AddDay        (CALENDARBASE+8)    /* v14  */
#define MUIM_Calendar_AddMonth      (CALENDARBASE+9)    /* v14  */
#define MUIM_Calendar_AddYear       (CALENDARBASE+10)   /* v14  */
#define MUIM_Calendar_GetInfo       (CALENDARBASE+11)   /* v15  */

/***********************************************************************/
/*
** Methods structures
*/

struct MUIP_Calendar_FormatDate      { ULONG MethodID; STRPTR fmt; STRPTR buf; int bufLen; };
struct MUIP_Calendar_AddDay          { ULONG MethodID; ULONG delta; };
struct MUIP_Calendar_AddMonth        { ULONG MethodID; ULONG delta; };
struct MUIP_Calendar_AddYear         { ULONG MethodID; ULONG delta; };
struct MUIP_Calendar_GetInfo         { ULONG MethodID; ULONG greg; ULONG info; ULONG *storage[1]; };

#define MUIV_Calendar_FormatDate_Long  ((STRPTR)(0))
#define MUIV_Calendar_FormatDate_Short ((STRPTR)(1))

enum
{
    MUIV_Calendar_GetInfo_Greg,
    MUIV_Calendar_GetInfo_WeekDay,
    MUIV_Calendar_GetInfo_JulianDay,
    MUIV_Calendar_GetInfo_Today,
    MUIV_Calendar_GetInfo_Easter,
};

#define MUIV_Calendar_GetInfo_Greg_Default 0
#define MUIV_Calendar_GetInfo_Greg_Today   1

/***********************************************************************/
/*
** Attributes
*/

#define MUIA_Calendar_MinVer              (CALENDARBASE+5)   /* v14 ULONG, [I...]   */
#define MUIA_Calendar_Short               (CALENDARBASE+6)   /* v14 ULONG, [I...]   */
#define MUIA_Calendar_Date                (CALENDARBASE+7)   /* v14 ULONG, [ISGN]   */
#define MUIA_Calendar_Day                 (CALENDARBASE+8)   /* v14 ULONG, [ISGN]   */
#define MUIA_Calendar_Month               (CALENDARBASE+9)   /* v14 ULONG, [ISGN]   */
#define MUIA_Calendar_Year                (CALENDARBASE+10)  /* v14 ULONG, [ISGN]   */
#define MUIA_Calendar_Greg                (CALENDARBASE+11)  /* v14 ULONG, [ISGN]   */
#define MUIA_Calendar_Click               (CALENDARBASE+12)  /* v14 ULONG, [..GN]   */
#define MUIA_Calendar_DoubleClick         (CALENDARBASE+13)  /* v14 ULONG, [..GN]   */
#define MUIA_Calendar_FixedHoriz          (CALENDARBASE+14)  /* v14 BOOL,  [I...]   */
#define MUIA_Calendar_ShowToday           (CALENDARBASE+15)  /* v14 BOOL,  [I...]   */
#define MUIA_Calendar_OnlyDays            (CALENDARBASE+16)  /* v14 BOOL,  [I...]   */
#define MUIA_Calendar_TodayClick          (CALENDARBASE+17)  /* v14 ULONG, [I...]   */
#define MUIA_Calendar_SetDaysAsDefault    (CALENDARBASE+18)  /* v14 BOOL,  [I...]   */
#define MUIA_Calendar_Input               (CALENDARBASE+19)  /* v15 BOOL,  [I...]   */

enum
{
    MUIV_Calendar_TodayClick_None,
    MUIV_Calendar_TodayClick_Single,
    MUIV_Calendar_TodayClick_Double,
};

/***********************************************************************/

#if defined(__GNUC__)
# pragma pack()
#endif

#endif /* CALENDAR_MCC_H */
