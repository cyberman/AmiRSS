#ifndef _PROC_H
#define _PROC_H

/***********************************************************************/

struct taskMsg
{
    struct Message                link;
    ULONG                         type;

    struct SignalSemaphore        *sem;
    struct MUI_NListtree_TreeNode *tn;
    struct MsgPort                *resPort;
    struct MsgPort                *notifyPort;
    Object                        *app;

    STRPTR                        URL;

    STRPTR                        errorBuf;
    ULONG                         errorBufSize;

    STRPTR                        proxy;
    int                           proxyPort;
    ULONG                         useProxy;

    STRPTR                        agent;
    LONG                          GMTOffset;

    struct MsgPort                *procPort;

    LONG                          err;
    ULONG                         flags;
};

enum
{
    TMTYPE_File,
    TMTYPE_Socket,
};

enum
{
    TMFLG_ErrorBuf = 1<<0,
    TMFLG_UseProxy = 1<<1,
};

/***********************************************************************/

struct resultMsg
{
    struct Message                link;
    ULONG                         type;
    struct MUI_NListtree_TreeNode *tn;
};

enum
{
    MTYPE_Status,
    MTYPE_Line,
    MTYPE_Date,
};

/***********************************************************************/

struct statusMsg
{
    struct Message                link;
    ULONG                         type;
    struct MUI_NListtree_TreeNode *tn;

    ULONG                         status;
    ULONG                         err;
};

struct lineMsg
{
    struct Message                link;
    ULONG                         type;
    struct MUI_NListtree_TreeNode *tn;

    STRPTR                        line;
    ULONG                         len;
};

struct dateMsg
{
    struct Message                link;
    ULONG                         type;
    struct MUI_NListtree_TreeNode *tn;

    struct DateStamp              ds;
};

/***********************************************************************/

#endif /* _PROC_H */
