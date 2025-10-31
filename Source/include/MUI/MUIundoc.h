/***************************************************************************
** The Hacker's include to MUI v1.8 :-)
**
** Copyright 1997-98 by Alessandro Zummo
** azummo@ita.flashnet.it
**
** This include is unofficial, use at your own risk!
**
** You can also find other undocumented tags in libraries/mui.h :-)
**
****************************************************************************
** Class Tree
****************************************************************************
**
** rootclass                   (BOOPSI's base class)
** +--Notify                   (implements notification mechanism)
** !  +--Area                  (base class for all GUI elements)
** !     +--Framedisplay       (displays frame specification)
** !     !  \--Popframe        (popup button to adjust a frame spec)
** !     +--Imagedisplay       (displays image specification)
** !     !  \--Popimage        (popup button to adjust an image spec)
** !     +--Pendisplay         (displays a pen specification)
** !     !  \--Poppen          (popup button to adjust a pen spec)
** !     +--Group              (groups other GUI elements)
** !        +--Register        (handles page groups with titles)
** !        !  \--Penadjust    (group to adjust a pen)
** !        +--Frameadjust     (group to adjust a frame)
** !        +--Imageadjust     (group to adjust an image)
**
*/


#ifndef MUI_UNDOC_H
#define MUI_UNDOC_H

#if defined(__GNUC__)
# pragma pack(2)
#endif


//Uncomment this if you want be able to use all the undocumented features
//But remember to modify your libraries/mui.h include

//#define UNDOC_HACK


/*************************************************************************
** Black box specification structures for images, pens, frames
*************************************************************************/

/* Defined in mui.h
struct MUI_PenSpec
{
    char buf[32];
};
*/

struct MUI_ImageSpec
{
  char buf[64];
};

struct MUI_FrameSpec
{
  char buf[32];
};


// I'm not sure if MUI_ImageSpec and MUI_FrameSpec are 32 or 64 bytes wide.

/*************************************************************************
** The real MUI_NotifyData structure
*************************************************************************/

#ifdef UNDOC_HACK

struct MUI_NotifyData
{
    struct MUI_GlobalInfo *mnd_GlobalInfo;
    ULONG                  mnd_UserData;
    ULONG                  mnd_ObjectID;
    ULONG priv1;
    Object                *mnd_ParentObject; // The name may not be the real one
    ULONG priv3;
    ULONG priv4;
};

#ifndef _parent
#define _parent(obj)    (muiNotifyData(obj)->mnd_ParentObject) /* valid between MUIM_Setup/Cleanup */
#endif

#else

#ifndef _parent
#define _parent(obj)    xget(obj,MUIA_Parent)
#endif

#endif


// The use of _parent(obj) macro is strictly forbidden! Use xget(obj,MUIA_Parent) instead.


/****************************************************************************/
/** Flags                                                                  **/
/****************************************************************************/

#ifndef MADF_OBJECTVISIBLE
#define MADF_OBJECTVISIBLE     (1<<14) // The object is visible
#endif

#ifndef MUIMRI_INVIRTUALGROUP
#define MUIMRI_INVIRTUALGROUP  (1<<29) // The object is inside a virtual group
#endif

#ifndef MUIMRI_ISVIRTUALGROUP
#define MUIMRI_ISVIRTUALGROUP  (1<<30) // The object is a virtual group
#endif

/****************************************************************************/
/** Crawling                                                               **/
/****************************************************************************/

#ifdef _DCC
extern char MUIC_Crawling[];
#else
#ifndef MUIC_Crawling
#define MUIC_Crawling "Crawling.mcc"
#endif
#endif

#ifndef CrawlingObject
#define CrawlingObject MUI_NewObject(MUIC_Crawling
#endif

/****************************************************************************/
/** Application                                                            **/
/****************************************************************************/

/* Attributes */

#ifndef MUIA_Application_UsedClasses
#define MUIA_Application_UsedClasses    0x8042E9A7 /* V20 (!) */
#endif

/****************************************************************************/
/** Window                                                                 **/
/****************************************************************************/

/* Methods */

#ifndef MUIM_Window_ActionIconify
#define MUIM_Window_ActionIconify 0x80422cc0 /* V18 */
#endif

#ifndef MUIM_Window_Cleanup
#define MUIM_Window_Cleanup       0x8042ab26 /* Custom Class */ /* V18 */
struct  MUIP_Window_Cleanup       { ULONG MethodID; }; /* Custom Class */
#endif

#ifndef MUIM_Window_Setup
#define MUIM_Window_Setup         0x8042c34c /* Custom Class */ /* V18 */
struct  MUIP_Window_Setup         { ULONG MethodID; }; /* Custom Class */
#endif

/* Attributes */

#define MUIA_Window_DisableKeys   0x80424c36 /* V15 isg ULONG */


/****************************************************************************/
/** Area                                                                   **/
/****************************************************************************/

/* Methods */

#ifndef MUIM_DoDrag
#define MUIM_DoDrag          0x804216bb /* V18 */ /* Custom Class */
struct  MUIP_DoDrag          { ULONG MethodID; LONG touchx; LONG touchy; ULONG flags; }; /* Custom Class */
#endif

#ifndef MUIM_CreateDragImage
#define MUIM_CreateDragImage 0x8042eb6f /* V18 */ /* Custom Class */
struct  MUIP_CreateDragImage { ULONG MethodID; LONG touchx; LONG touchy; ULONG flags; }; /* Custom Class */
#endif

#ifndef MUIM_DeleteDragImage
#define MUIM_DeleteDragImage 0x80423037 /* V18 */ /* Custom Class */
struct  MUIP_DeleteDragImage { ULONG MethodID; struct MUI_DragImage *di; };              /* Custom Class */
#endif

#ifndef MUIM_GoActive
#define MUIM_GoActive        0x8042491a
#endif

#ifndef MUIM_GoInactive
#define MUIM_GoInactive      0x80422c0c
#endif

#ifndef MUIM_CustomBackfill
#define MUIM_CustomBackfill  0x80428d73
struct  MUIP_CustomBackfill  { ULONG MethodID; LONG left; LONG top; LONG right; LONG bottom; LONG xoffset; LONG yoffset; };
#endif

/* Attributes */

#ifndef MUIA_CustomBackfill
#define MUIA_CustomBackfill  0x80420a63
#endif

#ifndef MUIV_CreateBubble_DontHidePointer
#define MUIV_CreateBubble_DontHidePointer (1<<0)
#endif

struct MUI_DragImage
{
    struct BitMap *bm;
    WORD width;  /* exact width and height of bitmap */
    WORD height;
    WORD touchx; /* position of pointer click relative to bitmap */
    WORD touchy;
    ULONG flags; /* must be set to 0 */
};


/****************************************************************************/
/** Imagedisplay                                                           **/
/****************************************************************************/

/* Attributes */

#ifndef MUIA_Imagedisplay_Spec
#define MUIA_Imagedisplay_Spec 0x8042a547 /* V11 isg struct MUI_ImageSpec * */
#endif

/****************************************************************************/
/** Imageadjust                                                            **/
/****************************************************************************/

/* Attributes */

#ifndef MUIA_Imageadjust_Type
#define MUIA_Imageadjust_Type  0x80422f2b /* V11 i.. LONG */
#endif

/****************************************************************************/
/** Framedisplay                                                           **/
/****************************************************************************/

/* Attributes */

#ifndef MUIA_Framedisplay_Spec
#define MUIA_Framedisplay_Spec 0x80421794 /* isg struct MUI_FrameSpec * */
#endif

/****************************************************************************/
/** Prop                                                                   **/
/****************************************************************************/

/* Attributes */

#ifndef MUIA_Prop_DeltaFactor
#define MUIA_Prop_DeltaFactor 0x80427c5e /* V4 .s. LONG */
#endif

#ifndef MUIA_Prop_DoSmooth
#define MUIA_Prop_DoSmooth    0x804236ce /* V4 i.. LONG */
#endif

#ifndef MUIA_Prop_Release
#define MUIA_Prop_Release     0x80429839 /* V? g BOOL */ /* private */
#endif

#ifndef MUIA_Prop_Pressed
#define MUIA_Prop_Pressed     0x80422cd7 /* V6 g BOOL */ /* private */
#endif

/****************************************************************************/
/** Group                                                                  **/
/****************************************************************************/

/* Attributes */

#ifndef MUIA_Group_Forward
#define MUIA_Group_Forward    0x80421422 /* V11 .s. BOOL */
#endif

/****************************************************************************/
/** List                                                                   **/
/****************************************************************************/

/* Attributes */

#ifndef MUIA_List_Prop_Entries
#define MUIA_List_Prop_Entries  0x8042a8f5 /* V? ??? */
#endif

#ifndef MUIA_List_Prop_Visible
#define MUIA_List_Prop_Visible  0x804273e9 /* V? ??? */
#endif

#ifndef MUIA_List_Prop_First
#define MUIA_List_Prop_First    0x80429df3 /* V? ??? */
#endif

/****************************************************************************/
/** Text                                                                   **/
/****************************************************************************/

/* Attributes */

#ifndef MUIA_Text_HiCharIdx
#define MUIA_Text_HiCharIdx   0x804214f5
#endif

/****************************************************************************/
/** Dtpic                                                                  **/
/****************************************************************************/

/* Attributes */

#ifndef MUIA_Dtpic_Name
#define MUIA_Dtpic_Name 0x80423d72
#endif

#ifndef MUIV_Application_OCW_ScreenPage
#define MUIV_Application_OCW_ScreenPage (1<<1) /* show just the screen page of the config window */
#endif

#ifndef MUIA_Window_MenuGadget
#define MUIA_Window_MenuGadget       0x8042324E
#endif

#ifndef MUIA_Window_SnapshotGadget
#define MUIA_Window_SnapshotGadget   0x80423C55
#endif

#ifndef MUIA_Window_ConfigGadget
#define MUIA_Window_ConfigGadget     0x8042E262
#endif

#ifndef MUIA_Window_IconifyGadget
#define MUIA_Window_IconifyGadget    0x8042BC26
#endif

#ifndef MUIIHNF_TIMER_SCALE10
#define MUIIHNF_TIMER_SCALE10   (1<<1) /* ihn_Millis is in 1/100 seconds instead */
#endif

#ifndef MUIIHNF_TIMER_SCALE100
#define MUIIHNF_TIMER_SCALE100  (1<<2) /* ihn_Millis is in 1/10 seconds instead */
#endif

#ifndef MUIM_TheBar_DeActivate
#define MUIM_TheBar_DeActivate 0xF76B022E   /* v11 PRIVATE */
#endif

#ifndef MUIA_Balance_Quiet
#define MUIA_Balance_Quiet                  0x80427486 /* V20 i.. LONG              */
#endif

#ifndef MUIA_Bitmap_Alpha
#define MUIA_Bitmap_Alpha                   0x80423e71 /* V20 isg ULONG             */
#endif

#ifndef MUIA_Dirlist_ExAllType
#define MUIA_Dirlist_ExAllType              0x8042cd7c /* V20 i.g ULONG             */
#endif

#ifndef MUIA_Volumelist_ExampleMode
#define MUIA_Volumelist_ExampleMode         0x804246a5 /* V20 i.. BOOL              */
#endif

#ifndef MUIA_Window_PanelWindow
#define MUIA_Window_PanelWindow 0x80429528
#endif

#if defined(__GNUC__)
# pragma pack()
#endif

#endif /* MUI_UNDOC_H */

