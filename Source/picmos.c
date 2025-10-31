
#include "rss.h"
#include <proto/datatypes.h>
#include <proto/cybergraphics.h>
#include <datatypes/pictureclass.h>

/****************************************************************************/

#define RAWIDTH(w) ((((UWORD)(w))+15)>>3 & 0xFFFE)

ULONG WritePixelArrayAlpha(APTR, UWORD, UWORD, UWORD, struct RastPort *, UWORD, UWORD, UWORD, UWORD, ULONG);
#define WritePixelArrayAlpha(__p0, __p1, __p2, __p3, __p4, __p5, __p6, __p7, __p8, __p9) \
	LP10(216, ULONG , WritePixelArrayAlpha, \
		APTR ,             __p0, a0, \
		UWORD ,            __p1, d0, \
		UWORD ,            __p2, d1, \
		UWORD , 	   __p3, d2, \
		struct RastPort *, __p4, a1, \
		UWORD , 	   __p5, d3, \
		UWORD , 	   __p6, d4, \
		UWORD ,            __p7, d5, \
		UWORD ,            __p8, d6, \
		ULONG ,            __p9, d7, \
		, CyberGfxBase, 0, 0, 0, 0, 0, 0)

/****************************************************************************/

struct data
{
    UBYTE               file[256];
    Object              *dto;
    STRPTR		chunky;
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
    FLG_EmpytAlpha    = 1<<5,
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

    if (data->chunky)
    {
        freeArbitrateVecPooled(data->chunky);
        data->chunky = NULL;
    }

    if (data->dto)
    {
        DisposeDTObject(data->dto);
        data->dto = NULL;
    }
}

/***********************************************************************/

void
checkAlpha(struct data *data)
{
    register UBYTE *src;
    register ULONG reallyHasAlpha = FALSE;
    register int   x, y;

    src = data->chunky;

    for (y = 0; y<data->height; y++)
    {
        for (x = 0; x<data->width; x++)
        {
            if (*src) reallyHasAlpha = TRUE;
            src += 4;
        }
    }

    if (!reallyHasAlpha) data->flags |= FLG_EmpytAlpha;
    else data->flags &= ~FLG_EmpytAlpha;
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
    register APTR dest;

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
    register struct BitMap *src, *bm;
    register UWORD         x, y, w, h;

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
            scalebm.dw  = scale.bsa_DestWidth;  //w
            scalebm.dh  = scale.bsa_DestHeight; //h
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

struct scale
{
    UWORD w;
    UWORD tw;
    UWORD sl;
    UWORD st;
    UWORD sw;
    UWORD sh;
    UWORD dw;
    UWORD dh;
};

struct scaleData
{
    LONG  cy;
    float sourcey;
    float deltax;
    float deltay;
};

static void
scaleLineRGB(struct scale *sce,struct scaleData *data,ULONG *src,ULONG *dst)
{
    LONG w8 = (sce->dw>>3)+1, cx = 0, dx = data->deltax*65536;

    src = (ULONG *)((UBYTE *)src+4*sce->sl+(data->cy+sce->st)*sce->tw);

    switch (sce->dw & 7)
    {
        do
        {
                    *dst++ = src[cx>>16]; cx += dx;
            case 7: *dst++ = src[cx>>16]; cx += dx;
            case 6: *dst++ = src[cx>>16]; cx += dx;
            case 5: *dst++ = src[cx>>16]; cx += dx;
            case 4: *dst++ = src[cx>>16]; cx += dx;
            case 3: *dst++ = src[cx>>16]; cx += dx;
            case 2: *dst++ = src[cx>>16]; cx += dx;
            case 1: *dst++ = src[cx>>16]; cx += dx;
            case 0: w8--;

        } while (w8);
    }

    data->cy = data->sourcey += data->deltay;
}

void
scaleRGB(struct scale *sce,ULONG *src,ULONG *dst)
{
    if (sce && src && dst && sce->dw-1>0 && sce->dh-1>0)
    {
        struct scaleData scdata;
        LONG             y;

        scdata.cy       = 0;
        scdata.sourcey  = 0;

        scdata.deltax   = sce->sw-1;
        scdata.deltax  /= (sce->dw-1);

        scdata.deltay   = sce->sh-1;
        scdata.deltay  /= (sce->dh-1);

        for (y = 0; y<sce->dh; y++)
        {
            scaleLineRGB(sce,&scdata,src,dst);
            dst += sce->dw;
        }
    }
}

/***********************************************************************/

static ULONG
scaleChunky(struct data *data)
{
    struct scale    sce;
    register STRPTR chunky;
    register UWORD  x, y, tw, w, h;
    register ULONG  size;

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

    tw   = ((w+15)>>4)<<4;
    size = tw*h;
    size += size+size+size;

    if (!(chunky = allocArbitrateVecPooled(size)))
    	return FALSE;

    sce.w  = data->width;
    sce.tw = data->width*4;
    sce.sl = 0;
    sce.st = 0;
    sce.sw = data->width;
    sce.sh = data->height;
    sce.dw = w;
    sce.dh = h;

    scaleRGB(&sce,(ULONG *)data->chunky,(ULONG *)chunky);

    freeArbitrateVecPooled(data->chunky);
    data->chunky = chunky;
    data->width  = w;
    data->height = h;

    return TRUE;
}

/***********************************************************************/

static ULONG
build(struct IClass *cl,Object *obj,struct data *data)
{
    register ULONG cyber = data->flags & FLG_Cyber;

    freeDTO(data);
    if (!cyber) return FALSE;

    if ((data->flags & FLG_Setup) && *data->file)
    {
        register struct Process *me = (struct Process *)FindTask(NULL);
        register APTR           win = me->pr_WindowPtr;

        me->pr_WindowPtr = (APTR)-1;

        data->dto = NewDTObject(data->file,DTA_GroupID,    GID_PICTURE,
                               		   DTA_SourceType, DTST_FILE,
                               		   PDTA_Remap,     FALSE,
                               		   PDTA_DestMode,  PMODE_V43,
                               		   TAG_DONE);

        me->pr_WindowPtr = win;

        if (data->dto)
        {
	    struct BitMapHeader *bmhd;

            get(data->dto,PDTA_BitMapHeader,&bmhd);

            if (bmhd)
            {
            	register ULONG width, height, depth, size;

                width  = bmhd->bmh_Width;
                height = bmhd->bmh_Height;
                depth  = bmhd->bmh_Depth;

		if (depth>8)
                {
            	    size  = width*height;
                    size += size+size+size;

                    if (data->chunky = allocArbitrateVecPooled(size))
	            {
                    	DoMethod(data->dto,PDTM_READPIXELARRAY,(ULONG)data->chunky,PBPAFMT_ARGB,width<<2,0,0,width,height);

                        data->width     = width;
                        data->height    = height;
                        data->depth     = depth;

			if (((data->flags & FLG_Scaled) && scaleChunky(data)) || !(data->flags & FLG_Scaled))
                        {
                            checkAlpha(data);

        		    DisposeDTObject(data->dto);
		            data->dto = NULL;

                            return TRUE;
                        }
                    }
                }
		else
                {
                    struct FrameInfo fri;

		    memset(&fri,0,sizeof(fri));

        	    SetDTAttrs(data->dto,NULL,NULL,PDTA_Screen,           (ULONG)_screen(obj),
                                         	   PDTA_FreeSourceBitMap, TRUE,
                                         	   PDTA_UseFriendBitMap,  TRUE,
                                         	   TAG_DONE);

                    DoMethod(data->dto,DTM_FRAMEBOX,NULL,(ULONG)&fri,(ULONG)&fri,sizeof(struct FrameInfo),0);

                    if (fri.fri_Dimensions.Depth>0)
	            {
	                if (DoMethod(data->dto,DTM_PROCLAYOUT,NULL,TRUE))
	                {
                            data->width  = width;
                            data->height = height;
                            data->depth  = depth;

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
        register struct data    *data = INST_DATA(cl, obj);
        register struct TagItem *attrs = msg->ops_AttrList;
        register STRPTR         file;

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
    register struct data    *data = INST_DATA(cl, obj);
    struct TagItem          *tags;
    register struct TagItem *tag;
    register ULONG          res, rebuild = FALSE;

    for (tags = msg->ops_AttrList; tag = NextTagItem(&tags); )
    {
        register ULONG tidata = tag->ti_Data;

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
        Object                 *parent;
        register struct BitMap *bm = data->bitmap;

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
    register struct data *data = INST_DATA(cl,obj);

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
    register struct data *data = INST_DATA(cl,obj);

    freeDTO(data);
    data->flags &= ~(FLG_Setup|FLG_Cyber);

    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/***********************************************************************/

static ULONG
mAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
    register struct data       *data = INST_DATA(cl,obj);
    register struct MUI_MinMax *mi;
    register UWORD	       w, h;

    DoSuperMethodA(cl,obj,(Msg)msg);

    w = data->width;
    h = data->height;

    mi = msg->MinMaxInfo;

    if (data->bitmap||data->chunky)
    {
        mi->MinWidth  += w;
        mi->MinHeight += h;
        mi->DefWidth  += w;
        mi->DefHeight += h;
    }

    if (data->flags & FLG_Scaled)
    {
        mi->MaxWidth  += w;
        mi->MaxHeight += h;
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
    register struct data *data = INST_DATA(cl,obj);

    DoSuperMethodA(cl,obj,(Msg)msg);

    if ((msg->flags & MADF_DRAWOBJECT) && (data->chunky || data->bitmap))
    {
        register ULONG done = FALSE;
        register WORD  l, t, w, h, bw, bh;

        w = _mwidth(obj);
        h = _mheight(obj);
        l = _mleft(obj);
        t = _mtop(obj);

        bw = data->width;
        bh = data->height;

        if (bw<w) w = bw;
        if (bh<h) h = bh;

        if (data->chunky)
        {
	    if (data->flags & FLG_EmpytAlpha) WritePixelArray(data->chunky,0,0,data->width*4,_rp(obj),l,t,w,h,PBPAFMT_ARGB);
    	    else WritePixelArrayAlpha(data->chunky,0,0,data->width*4,_rp(obj),l,t,w,h,0xFFFFFFFF);
	}
    	else
        {
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

