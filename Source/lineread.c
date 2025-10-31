/*
**      lineread.c - functions to read lines from sockets effectively
**
**      Copyright © 1994 AmiTCP/IP Group,
**                       Network Solutions Development Inc.
**                       All rights reserved.
**
**      Modified by Alfonso Ranieri for freedb.library
**
*/

/***************************************************************************/

#include "rss.h"

#include <proto/socket.h>
#ifdef __MORPHOS__
#include <net/socketbasetags.h>
#else
#include <bsdsocket/socketbasetags.h>
#include <sys/socket.h>
#endif
#include <netdb.h>

#include "lineread.h"

/***************************************************************************/

void
initLineRead(struct lineRead *lr,
             struct Library *socketBase,
             int fd,
             int type,
             int bufferSize)
{
    lr->socketBase = socketBase;
    lr->sock       = fd;
    lr->type       = type;
    lr->bufPtr     = lr->howLong = 0;
    lr->selected   = lr->lineCompleted = TRUE;
    lr->bufferSize = bufferSize;
}

/***************************************************************************/

int
lineRead(struct lineRead *lr)
{
    struct Library *SocketBase = lr->socketBase;
    register int   i;

    if (lr->bufPtr==lr->howLong)
        if (lr->selected)
        {
            if (lr->lineCompleted)
                lr->startp = lr->bufPtr = 0;

            if ((i = recv(lr->sock,lr->buffer+lr->bufPtr,lr->bufferSize-lr->bufPtr,0))<=0)
            {
                /*
                 * here if end-of-file or on error. set Howlong == Bufpointer
                 * so if non-blocking I/O is in use next call will go to READ()
                 */
                lr->howLong = lr->bufPtr;
                lr->line = NULL;
                return i;
            }
            else lr->howLong = lr->bufPtr + i;
        }
        else
        {
            /* Inform user that next call may block (unless select()ed) */
            lr->selected = TRUE;
            return 0;
        }
    else /* Bufpointer has not reached Howlong yet. */
    {
        lr->buffer[lr->bufPtr] = lr->saved;
        lr->startp = lr->bufPtr;
    }

    /*
    * Scan read string for next newline.
    */
    while (lr->bufPtr<lr->howLong)
        if (lr->buffer[lr->bufPtr++]=='\n') goto Skip;

    /*
    * Here if Bufpointer == Howlong.
    */
    if (lr->type!=LRV_Type_NotReq)
    {
        lr->selected = TRUE;

        if (lr->bufPtr==lr->bufferSize)
        {
            /*
            * Here if Bufpointer reaches end-of-buffer.
            */
            if (lr->startp==0)
            {
                /* (buffer too short for whole string) */
                lr->lineCompleted = TRUE;
                lr->line = lr->buffer;
                lr->buffer[lr->bufPtr] = '\0';
                return -1;
            }
            /*
            * Copy partial string to start-of-buffer and make control ready for
            * filling rest of buffer when next call to lineRead() is made
            * (perhaps after select()).
            */
            for (i = 0; i<lr->bufferSize-lr->startp; i++)
                lr->buffer[i] = lr->buffer[lr->startp+i];
            lr->howLong -= lr->startp;
            lr->bufPtr = lr->howLong;
            lr->startp = 0;
        }

        lr->lineCompleted = FALSE;
        return 0;
    }

Skip:
    lr->lineCompleted = TRUE;
    if (lr->type==LRV_Type_ReqNul) lr->buffer[lr->bufPtr-1] = '\0';
    lr->saved = lr->buffer[lr->bufPtr];
    lr->buffer[lr->bufPtr] = '\0';
    lr->selected = FALSE;
    lr->line = lr->buffer+lr->startp;

    return (lr->bufPtr-lr->startp);
}

/***************************************************************************/
