
#ifndef _MACROS_H
#define _MACROS_H

/****************************************************************************/

#ifdef __MORPHOS__

#define SAVEDS   __saveds
#define ASM
#define REGARGS
#define STDARGS
#define INLINE   __inline
#define REG(x,p)
#define REGARRAY

#define setrxvar(msg,name,value,len) SetRexxVar((struct RexxMsg *)msg,name,value,len)

#define M_HOOK(n,y,z) \
    static LONG n##_GATE(void); \
    static LONG n##_GATE2(struct Hook *hook,y,z); \
    static struct EmulLibEntry n = {TRAP_LIB,0,(void (*)(void))n##_GATE}; \
    static LONG n##_GATE(void) {return (n##_GATE2((void *)REG_A0,(void *)REG_A2,(void *)REG_A1));} \
    static struct Hook n##_hook = { 0, 0, (void *)&n }; \
    static LONG n##_GATE2(struct Hook *hook, y, z)

#define M_DISP(n) static ULONG _##n(void)
#define M_DISPSTART \
    struct IClass *cl = (struct IClass *)REG_A0; \
    Object        *obj = (Object *)REG_A2; \
    Msg            msg  = (Msg)REG_A1;
#define M_DISPEND(n) static struct EmulLibEntry n = {TRAP_LIB,0,(void *)_##n};
#define DISP(n) ((APTR)&n)

#define copymem(to,from,len) CopyMem((APTR)(from),(APTR)(to),(ULONG)(len))

#else /* __MORPHOS__ */

#define setrxvar(msg,name,value,len) SetRexxVar((struct RexxMsg *)msg,name,value,len)

#define SAVEDS   __saveds
#define ASM      __asm
#define REGARGS  __regargs
#define STDARGS  __stdargs
#define INLINE   __inline
#define REG(x,p) register __ ## x p
#define REGARRAY register
#define __attribute(a)

#define M_HOOK(n,y,z) \
    static LONG SAVEDS ASM n##_func(REG(a0,struct Hook *hook),REG(a2,y),REG(a1,z)); \
    static struct Hook n##_hook = {0,0,(HOOKFUNC)n##_func}; \
    static LONG ASM SAVEDS n##_func(REG(a0,struct Hook *hook),REG(a2,y),REG(a1,z))

#define M_DISP(n) static ULONG ASM SAVEDS n(REG(a0,struct IClass *cl),REG(a2,Object *obj),REG(a1,Msg msg))
#define M_DISPSTART
#define M_DISPEND(n)
#define DISP(n) (n)

#define copymem(to,from,len) memcpy((APTR)(to),(APTR)(from),(ULONG)(len))

#endif /* __MORPHOS__ */

#undef NODE
#define NODE(a) ((struct Node *)(a))

#undef MINNODE
#define MINNODE(a) ((struct MinNode *)(a))

#undef LIST
#define LIST(a) ((struct List *)(a))

#undef MINLIST
#define MINLIST(a) ((struct MinList *)(a))

#undef MESSAGE
#define MESSAGE(m) ((struct Message *)(m))

#undef NEWLIST
#define NEWLIST(l) (LIST(l)->lh_Head = NODE(&LIST(l)->lh_Tail), \
                    LIST(l)->lh_Tail = NULL, \
                    LIST(l)->lh_TailPred = NODE(&LIST(l)->lh_Head))

#ifndef QUICKNEWLIST
#define QUICKNEWLIST(l) (LIST(l)->lh_Head = NODE(&LIST(l)->lh_Tail), \
                         LIST(l)->lh_TailPred = NODE(&LIST(l)->lh_Head))
#endif /* QUICKNEWLIST */

#undef ADDTAIL
#define ADDTAIL(l,n) AddTail(LIST(l),NODE(n))

#undef PORT
#define PORT(p) ((struct MsgPort *)(p))

#undef INITPORT
#define INITPORT(p,s) (PORT(p)->mp_Flags = PA_SIGNAL, \
                       PORT(p)->mp_SigBit = (UBYTE)(s), \
                       PORT(p)->mp_SigTask = FindTask(NULL), \
                       NEWLIST(&(PORT(p)->mp_MsgList)))

#undef QUICKINITPORT
#define QUICKINITPORT(p,s,t) (PORT(p)->mp_Flags = PA_SIGNAL, \
                              PORT(p)->mp_SigBit = (UBYTE)(s), \
                              PORT(p)->mp_SigTask = (t), \
                              QUICKNEWLIST(&(PORT(p)->mp_MsgList)))

#undef INITMESSAGE
#define INITMESSAGE(m,p,l) (MESSAGE(m)->mn_Node.ln_Type = NT_MESSAGE, \
                            MESSAGE(m)->mn_ReplyPort = PORT(p), \
                            MESSAGE(m)->mn_Length = ((UWORD)l))

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif /* MAKE_ID */

#undef MIN
#define MIN(a,b) ((a<b) ? (a) : (b))

#undef MAX
#define MAX(a,b) ((a>b) ? (a) : (b))

#undef ABS
#define ABS(a) (((a)>0) ? (a) : -(a))

#undef BOOLSAME
#define BOOLSAME(a,b) (((a) ? TRUE : FALSE)==((b) ? TRUE : FALSE))

/****************************************************************************/

#endif /* _MACROS_H */
