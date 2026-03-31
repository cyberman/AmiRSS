/*
**
** amrss - Amiga RSS Client
**
** Copyright 2004-2005 by Alfonso [alfie] Ranieri <alforan@tin.it>
** All Rights Are Reserved.
**
** Destributed Under The Terms Of The GPL II
**
**/

#include "rss.h"
#include <proto/datatypes.h>
#include <proto/cybergraphics.h>
#include <datatypes/pictureclass.h>

/****************************************************************************/

#define RAWIDTH(w) ((((UWORD)(w))+15)>>3 & 0xFFFE)

/****************************************************************************/

struct data
{
    UBYTE               file[DEF_FILESIZE];
    Object              *dto;
    struct BitMap       *bitmap;
    APTR                plane;
    struct BitMap       *scaledMask;
    ULONG               flags;

    UWORD               width;
    UWORD               height;
    UWORD               depth;
    UWORD               scaleds;
};

enum
{
    FLG_Setup         = 1<<0,
    FLG_Transparent   = 1<<1,
    FLG_Scaled        = 1<<2,
    FLG_FreeBitMap    = 1<<3,
    FLG_Cyber         = 1<<4,
};

/***********************************************************************/

static void
freeDTO(struct data *data)
{
    if (data->flags & FLG_FreeBitMap)
    {
        if (data->scaledMask)
        {
            FreeBitMap(data->scaledMask);
            data->scaledMask = NULL;
        }

        FreeBitMap(data->bitmap);
        data->flags &= ~FLG_FreeBitMap;
    }

    data->bitmap = NULL;

    if (data->dto)
    {
        DisposeDTObject(data->dto);
        data->dto = NULL;
    }
}

/***********************************************************************/

struct scaleBitMap
{
    UWORD sw;
    UWORD sh;

    UWORD dw;
    UWORD dh;

    UWORD fw;
    UWORD fh;

    UWORD tsw;
    UWORD tsh;
};

#define FACTOR 16383

static struct BitMap *
scaleMask(struct data *data,APTR src,struct scaleBitMap *scale)
{
    APTR dest;

    if (dest = AllocBitMap(scale->dw,scale->dh,1,0,NULL))
    {
        struct BitScaleArgs bsa;
        struct BitMap       sbm;

        memset(&sbm,0,sizeof(sbm));
        sbm.BytesPerRow = RAWIDTH(scale->tsw);
        sbm.Rows        = scale->tsh;
        sbm.Depth       = 1;
        sbm.Planes[0]   = src;

        memset(&bsa,0,sizeof(bsa));

        bsa.bsa_SrcBitMap   = &sbm;
        bsa.bsa_DestBitMap  = dest;

        bsa.bsa_SrcWidth    = scale->sw;
        bsa.bsa_SrcHeight   = scale->sh;

        bsa.bsa_XSrcFactor  = FACTOR;
        bsa.bsa_XDestFactor = scale->fw;

        bsa.bsa_YSrcFactor  = FACTOR;
        bsa.bsa_YDestFactor = scale->fh;

        BitMapScale(&bsa);
        WaitBlit();
    }

    return dest;
}

static ULONG
scaleBitMap(struct data *data,Object *obj)
{
    struct BitMap *src, *bm;
    UWORD         x, y, w, h;

    if (data->width==0 || data->width>16383 || data->height==0 || data->height>16383)
        return FALSE;

    if (data->width<=data->scaleds && data->height<=data->scaleds)
        return TRUE;

    if (data->width>data->height)
    {
        w = data->scaleds;
        h = data->scaleds*data->height/data->width;
    }
    else
    {
        w = data->scaleds*data->width/data->height;
        h = data->scaleds;
    }

    if (w==0 || w>16383 || h==0 || h>16383)
        return FALSE;

    x = ScalerDiv(w,FACTOR,data->width);
    y = ScalerDiv(h,FACTOR,data->height);

    if (x==0 || x>16383 || y==0 || y>16383)
        return FALSE;

    w = ScalerDiv(x,data->width,FACTOR);
    h = ScalerDiv(y,data->height,FACTOR);

    if (w==0 || w>16383 || h==0 || h>16383)
        return FALSE;

    src = data->bitmap;

    if (bm = AllocBitMap(w,h,GetBitMapAttr(src,BMA_DEPTH),(data->flags & FLG_Cyber) ? BMF_MINPLANES : 0,(data->flags & FLG_Cyber) ? src : NULL))
    {
        struct BitScaleArgs scale;

        memset(&scale,0,sizeof(struct BitScaleArgs));

        scale.bsa_SrcBitMap   = src;
        scale.bsa_DestBitMap  = bm;

        scale.bsa_SrcWidth    = data->width;
        scale.bsa_SrcHeight   = data->height;

        scale.bsa_XSrcFactor  = FACTOR;
        scale.bsa_XDestFactor = x;

        scale.bsa_YSrcFactor  = FACTOR;
        scale.bsa_YDestFactor = y;

        BitMapScale(&scale);

        if (data->plane)
        {
            struct scaleBitMap scalebm;

            scalebm.sw  = data->width;
            scalebm.sh  = data->height;
            scalebm.dw  = scale.bsa_DestWidth;
            scalebm.dh  = scale.bsa_DestHeight;
            scalebm.fw  = x;
            scalebm.fh  = y;
            scalebm.tsw = data->width;
            scalebm.tsh = data->height;

            data->scaledMask = scaleMask(data,data->plane,&scalebm);
        }

        data->width  = w;
        data->height = h;

        data->bitmap = bm;
        data->flags |= FLG_FreeBitMap;
    }

    return (ULONG)bm;
}

/***********************************************************************/

static ULONG
build(struct IClass *cl,Object *obj,struct data *data)
{
    freeDTO(data);

    if ((data->flags & FLG_Setup) && *data->file)
    {
        ULONG cyber = data->flags & FLG_Cyber;
        struct Process *me = (struct Process *)FindTask(NULL);
        APTR           win = me->pr_WindowPtr;

        me->pr_WindowPtr = (APTR)-1;

        data->dto = NewDTObject(data->file,DTA_GroupID,                        	   GID_PICTURE,
                                               PDTA_Screen,                        (ULONG)_screen(obj),
                                               PDTA_FreeSourceBitMap,              TRUE,
                                               PDTA_UseFriendBitMap,               cyber,
                                               cyber ? PDTA_DestMode : TAG_IGNORE, PMODE_V43,
                                               TAG_DONE);

        me->pr_WindowPtr = win;

        if (data->dto)
        {
            struct FrameInfo fri = {NULL};

            DoMethod(data->dto,DTM_FRAMEBOX,NULL,(ULONG)&fri,(ULONG)&fri,sizeof(struct FrameInfo),0);

            if (fri.fri_Dimensions.Depth>0)
            {
                if (DoMethod(data->dto,DTM_PROCLAYOUT,NULL,TRUE))
                {
                    struct BitMapHeader *bmhd;

                    get(data->dto,PDTA_BitMapHeader,&bmhd);

                    if (bmhd)
                    {
                        data->width  = bmhd->bmh_Width;
                        data->height = bmhd->bmh_Height;
                        data->depth  = bmhd->bmh_Depth;

                        GetDTAttrs(data->dto,PDTA_DestBitMap,(ULONG)&data->bitmap,TAG_DONE);
                        if (!data->bitmap) GetDTAttrs(data->dto,PDTA_BitMap,(ULONG)&data->bitmap,TAG_DONE);

                        if (data->bitmap)
                        {
                            GetDTAttrs(data->dto,PDTA_MaskPlane,(ULONG)&data->plane,TAG_DONE);

                            if (((data->flags & FLG_Scaled) && scaleBitMap(data,obj)) ||
                                !(data->flags & FLG_Scaled))
                                return TRUE;
                        }
                    }
                }
            }
        }
    }

    freeDTO(data);

    return FALSE;
}

/***********************************************************************/

static ULONG
mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
    if (obj = (Object *)DoSuperMethodA(cl,obj,(Msg)msg))
    {
        struct data    *data = INST_DATA(cl, obj);
        struct TagItem *attrs = msg->ops_AttrList;
        STRPTR         file;

        file = (STRPTR)GetTagData(MUIA_Pic_File,0,attrs);
        if (file && *file) stccpy(data->file,file,sizeof(data->file));

        if (GetTagData(MUIA_Pic_Transparent,FALSE,attrs))
            data->flags |= FLG_Transparent;

        data->scaleds = (UWORD)GetTagData(MUIA_Pic_ScaledMaxSize,0,attrs);
        if (data->scaleds!=0) data->flags |= FLG_Scaled;
    }

    return (ULONG)obj;
}

/***********************************************************************/

static ULONG
mSets(struct IClass *cl,Object *obj,struct opSet *msg)
{
    struct data    *data = INST_DATA(cl, obj);
    struct TagItem *tags;
    struct TagItem *tag;
    ULONG          res, rebuild = FALSE;

    for (tags = msg->ops_AttrList; tag = NextTagItem(&tags); )
    {
        ULONG tidata = tag->ti_Data;

        switch (tag->ti_Tag)
        {
            case MUIA_Pic_File:
                if (tidata && *((STRPTR)tidata)) stccpy(data->file,(STRPTR)tidata,sizeof(data->file));
                else *data->file = 0;
                rebuild = TRUE;
                break;

            case MUIA_Pic_Transparent:
                if (!BOOLSAME(data->flags & FLG_Transparent,tidata))
                {
                    if (tidata) data->flags |= FLG_Transparent;
                    else data->flags &= ~FLG_Transparent;

                    rebuild = TRUE;
                }
                break;

            case MUIA_Pic_ScaledMaxSize:
                if (data->scaleds!=(UWORD)tidata)
                {
                    data->scaleds = (UWORD)tidata;
                    if (data->scaleds==0) data->flags &= ~FLG_Scaled;
                    else data->flags |= FLG_Scaled;

                    rebuild = TRUE;
                }
                break;
        }
    }

    res = DoSuperMethodA(cl,obj,(Msg)msg);

    if ((data->flags & FLG_Setup) && rebuild)
    {
        Object        *parent;
        struct BitMap *bm = data->bitmap;

        if (bm)
        {
            data->bitmap = NULL;
            MUI_Redraw(obj,MADF_DRAWOBJECT);
            data->bitmap = bm;
        }

        superget(cl,obj,MUIA_Parent,&parent);
        if (parent) DoMethod(parent,MUIM_Group_InitChange);

        build(cl,obj,data);

        if (parent) DoMethod(parent,MUIM_Group_ExitChange);

        MUI_Redraw(obj,MADF_DRAWOBJECT);
    }

    return res;
}

/***************************************************************************/

static ULONG
mSetup(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    if (!DoSuperMethodA(cl,obj,(Msg)msg)) return FALSE;

    if (CyberGfxBase && IsCyberModeID(GetVPModeID(&_screen(obj)->ViewPort)))
        data->flags |= FLG_Cyber;

    data->flags |= FLG_Setup;

    build(cl,obj,data);

    return TRUE;
}

/***********************************************************************/

static ULONG
mCleanup(struct IClass *cl,Object *obj,Msg msg)
{
    struct data *data = INST_DATA(cl,obj);

    freeDTO(data);
    data->flags &= ~(FLG_Setup|FLG_Cyber);

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static ULONG
mAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
    struct data       *data = INST_DATA(cl,obj);
    struct MUI_MinMax *mi;

    DoSuperMethodA(cl,obj,(Msg)msg);

    mi = msg->MinMaxInfo;

    if (data->bitmap)
    {
        mi->MinWidth  += data->width;
        mi->MinHeight += data->height;
        mi->DefWidth  += data->width;
        mi->DefHeight += data->height;
    }

    if (data->flags & FLG_Scaled)
    {
        mi->MaxWidth  += data->width;
        mi->MaxHeight += data->height;
    }
    else
    {
        mi->MaxWidth  += MBQ_MUI_MAXMAX;
        mi->MaxHeight += MBQ_MUI_MAXMAX;
    }

    return 0;
}

/***********************************************************************/

static ULONG
mDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
    struct data *data = INST_DATA(cl,obj);

    DoSuperMethodA(cl,obj,(Msg)msg);

    if ((msg->flags & MADF_DRAWOBJECT) && data->bitmap)
    {
        ULONG done = FALSE;
        WORD  l, t, w, h, bw, bh;

        w = _mwidth(obj);
        h = _mheight(obj);
        l = _mleft(obj);
        t = _mtop(obj);

        bw = data->width;
        bh = data->height;

        if (bw<w) w = bw;
        if (bh<h) h = bh;

        if (data->flags & FLG_Transparent)
        {
            if (data->flags & FLG_FreeBitMap)
            {
                if (data->scaledMask)
                {
                    BltMaskBitMapRastPort(data->bitmap,0,0,_rp(obj),l,t,w,h,(ABC|ABNC|ANBC),data->scaledMask->Planes[0]);
                    done = TRUE;
                }
            }
            else
                if (data->plane)
                {
                    BltMaskBitMapRastPort(data->bitmap,0,0,_rp(obj),l,t,w,h,(ABC|ABNC|ANBC),data->plane);
                    done = TRUE;
                }
        }

        if (!done) BltBitMapRastPort(data->bitmap,0,0,_rp(obj),l,t,w,h,0xc0);
    }

    return 0;
}

/***********************************************************************/

M_DISP(dispatcher)
{
    M_DISPSTART

    switch(msg->MethodID)
    {
        case OM_NEW:         return mNew(cl,obj,(APTR)msg);
        case OM_SET:         return mSets(cl,obj,(APTR)msg);

        case MUIM_Draw:      return mDraw(cl,obj,(APTR)msg);
        case MUIM_AskMinMax: return mAskMinMax(cl,obj,(APTR)msg);
        case MUIM_Setup:     return mSetup(cl,obj,(APTR)msg);
        case MUIM_Cleanup:   return mCleanup(cl,obj,(APTR)msg);

        default:             return DoSuperMethodA(cl,obj,msg);
    }
}

M_DISPEND(dispatcher)

/***********************************************************************/

ULONG
initPicClass(void)
{
    return (ULONG)(picClass = MUI_CreateCustomClass(NULL,MUIC_Area,NULL,sizeof(struct data),DISP(dispatcher)));
}

/***********************************************************************/

void
disposePicClass(void)
{
    MUI_DeleteCustomClass(picClass);
}

/***********************************************************************/
