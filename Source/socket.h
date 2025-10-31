
/***********************************************************************/

#ifndef _SYS_TYPES_H_
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned long   u_int;
typedef unsigned long   u_long;
#endif

/***********************************************************************/

#define AF_INET     2

#define IPPROTO_IP  0

#define SOCK_STREAM 1       /* stream socket */
#define SOCK_DGRAM  2       /* datagram socket */
#define SOCK_RAW    3       /* raw-protocol interface */
#define SOCK_RDM    4       /* reliably-delivered message */
#define SOCK_SEQPACKET  5       /* sequenced packet stream */

struct in_addr
{
    u_long s_addr;
};

struct sockaddr
{
    u_char  sa_len;
    u_char  sa_family;
    char    sa_data[14];
};

struct sockaddr_in
{
    u_char  sin_len;
    u_char  sin_family;
    u_short sin_port;
    struct  in_addr sin_addr;
    char    sin_zero[8];
};

/***********************************************************************/

struct hostent
{
    char *h_name;
    char **h_aliases;
    long h_addrtype;
    long h_length;
    char **h_addr_list;
};

#define h_addr h_addr_list[0]

#define INADDR_NONE     0xffffffff

#ifndef __MORPHOS__
typedef APTR fd_set ;
#endif

/***********************************************************************/

#define SBTF_VAL 0x0000
#define SBTF_REF 0x8000

#define SBTB_CODE 1
#define SBTS_CODE 0x3FFF
#define SBTM_CODE(tag) ((((UWORD)(tag))>>SBTB_CODE) & SBTS_CODE)

#define SBTF_GET  0x0
#define SBTF_SET  0x1

#define SBTM_GETREF(code) (TAG_USER | SBTF_REF | (((code) & SBTS_CODE) << SBTB_CODE))
#define SBTM_GETVAL(code) (TAG_USER | (((code) & SBTS_CODE) << SBTB_CODE))
#define SBTM_SETREF(code) (TAG_USER | SBTF_REF | (((code) & SBTS_CODE) << SBTB_CODE) | SBTF_SET)
#define SBTM_SETVAL(code) (TAG_USER | (((code) & SBTS_CODE) << SBTB_CODE) | SBTF_SET)

#define SBTC_BREAKMASK      1
#define SBTC_SIGIOMASK      2
#define SBTC_SIGURGMASK     3
#define SBTC_SIGEVENTMASK   4
#define SBTC_ERRNO          6
#define SBTC_HERRNO         7
#define SBTC_DTABLESIZE     8
#define SBTC_FDCALLBACK     9

#define FDCB_FREE  0
#define FDCB_ALLOC 1
#define FDCB_CHECK 2

#define SBTC_LOGSTAT        10
#define SBTC_LOGTAGPTR      11
#define SBTC_LOGFACILITY    12
#define SBTC_LOGMASK        13

#define SBTC_ERRNOSTRPTR    14
#define SBTC_HERRNOSTRPTR   15
#define SBTC_IOERRNOSTRPTR  16
#define SBTC_S2ERRNOSTRPTR  17
#define SBTC_S2WERRNOSTRPTR 18

#define SBTC_ERRNOBYTEPTR   21
#define SBTC_ERRNOWORDPTR   22
#define SBTC_ERRNOLONGPTR   24

#define SBTC_ERRNOPTR(size)    ((size == sizeof(long)) ? SBTC_ERRNOLONGPTR :\
 ((size == sizeof(short)) ? SBTC_ERRNOWORDPTR :\
 ((size == sizeof(char)) ? SBTC_ERRNOBYTEPTR :\
 0)))

#define SBTC_HERRNOLONGPTR  25

#define SBTC_RELEASESTRPTR  29

/***********************************************************************/

#ifndef __MORPHOS__
long accept(long,struct sockaddr *,long *);
long bind(long,const struct sockaddr *,long);
long CloseSocket(long);
long connect(long,const struct sockaddr *,long);
long Dup2Socket(long,long);
long Errno(void);
long getdtablesize(void);
struct hostent *gethostbyaddr(const unsigned char *,long,long);
struct hostent *gethostbyname(const unsigned char *);
long gethostname(char *,long);
unsigned long gethostid(void);
struct netent *getnetbyaddr(long,long);
struct netent *getnetbyname(const unsigned char *);
long getpeername(long,struct sockaddr *,long *);
struct protoent *getprotobyname(const unsigned char *);
struct protoent *getprotobynumber(long);
struct servent *getservbyname(const unsigned char *,const unsigned char *);
struct servent *getservbyport(long,const unsigned char *);
long GetSocketEvents(unsigned long *);
long getsockname(long,struct sockaddr *,long *);
long getsockopt(long,long,long,void *,long *);
unsigned long inet_addr(const unsigned char *);
unsigned long Inet_LnaOf(long);
unsigned long inet_lnaof(struct in_addr);
unsigned long Inet_MakeAddr(unsigned long,unsigned long);
struct in_addr inet_makeaddr(unsigned long,unsigned long);
unsigned long Inet_NetOf(long);
unsigned long inet_netof(struct in_addr);
unsigned long inet_network(const unsigned char *);
char *Inet_NtoA(unsigned long);
char *inet_ntoa(struct in_addr);
long IoctlSocket(long,unsigned long,char *);
long listen(long,long);
long ObtainSocket(long,long,long,long);
long recv(long,unsigned char *,long,long);
long recvfrom(long,unsigned char *,long,long,struct sockaddr *,long *);
long ReleaseCopyOfSocket(long,long);
long ReleaseSocket(long,long);
long select(long,fd_set *,fd_set *,fd_set *,void *);
long send(long,const unsigned char *,long,long);
long sendto(long,const unsigned char *,long,long, const struct sockaddr *,long);
long SetErrnoPtr(void *,long);
void SetSocketSignals(unsigned long,unsigned long,unsigned long);
long setsockopt(long,long,long,const void *,long);
long shutdown(long,long);
long socket(long,long,long);
long SocketBaseTagList(struct TagItem *);
long SocketBaseTags(long,...);
void syslog(unsigned long,const char *, ...);
void vsyslog(unsigned long,const char *,long *);
long WaitSelect(long,fd_set *,fd_set *,fd_set *,void *, unsigned long *);
#else
LONG socket(LONG domain, LONG type, LONG protocol);
LONG bind(LONG s, const struct sockaddr *name, LONG namelen);
LONG listen(LONG s, LONG backlog);
LONG accept(LONG s, struct sockaddr *addr, LONG *addrlen);
LONG connect(LONG s, const struct sockaddr *name, LONG namelen);
LONG send(LONG s, const UBYTE *msg, LONG len, LONG flags);
LONG sendto(LONG s, const UBYTE *msg, LONG len, LONG flags,
		  const struct sockaddr *to, LONG tolen);
LONG recv(LONG s, UBYTE *buf, LONG len, LONG flags);
LONG recvfrom(LONG s, UBYTE *buf, LONG len, LONG flags,
		    struct sockaddr *from, LONG *fromlen);
LONG shutdown(LONG s, LONG how);
LONG setsockopt(LONG s, LONG level, LONG optname,
		     const void *optval, LONG optlen);
LONG getsockopt(LONG s, LONG level, LONG optname,
		     void *optval, LONG *optlen);
LONG getsockname(LONG s, struct sockaddr *name, LONG *namelen);
LONG getpeername(LONG s, struct sockaddr *name, LONG *namelen);

LONG IoctlSocket(LONG d, ULONG request, char *argp);
LONG CloseSocket(LONG d);
LONG WaitSelect(LONG nfds, fd_set *readfds, fd_set *writefds, fd_set *exeptfds,
		struct timeval *timeout, ULONG *maskp);

LONG Dup2Socket(LONG fd1, LONG fd2);

LONG getdtablesize(void);
void SetSocketSignals(ULONG _SIGINTR, ULONG _SIGIO, ULONG _SIGURG);
LONG SetErrnoPtr(void *errno_p, LONG size);
LONG SocketBaseTagList(struct TagItem *tagList);
LONG SocketBaseTags(LONG tag, ...);

LONG GetSocketEvents(ULONG *eventmaskp);

LONG Errno(void);

LONG gethostname(STRPTR hostname, LONG size);
ULONG gethostid(void);

LONG ObtainSocket(LONG id, LONG domain, LONG type, LONG protocol);
LONG ReleaseSocket(LONG fd, LONG id);
LONG ReleaseCopyOfSocket(LONG fd, LONG id);

ULONG inet_addr(const UBYTE *);
ULONG inet_network(const UBYTE *);
char *Inet_NtoA(ULONG s_addr);
ULONG Inet_MakeAddr(ULONG net, ULONG lna);
ULONG Inet_LnaOf(LONG s_addr);
ULONG Inet_NetOf(LONG s_addr);

struct hostent  *gethostbyname(const UBYTE *name);
struct hostent  *gethostbyaddr(const UBYTE *addr, LONG len, LONG type);
struct netent   *getnetbyname(const UBYTE *name);
struct netent   *getnetbyaddr(LONG net, LONG type);
struct servent  *getservbyname(const UBYTE *name, const UBYTE *proto);
struct servent  *getservbyport(LONG port, const UBYTE *proto);
struct protoent *getprotobyname(const UBYTE *name);
struct protoent *getprotobynumber(LONG proto);

void vsyslog(ULONG pri, const char *fmt, LONG *);
void syslog(ULONG pri, const char *fmt, ...);
#endif

/***********************************************************************/

#ifndef __MORPHOS__
#pragma libcall SocketBase socket 1e 21003
#pragma libcall SocketBase bind 24 18003
#pragma libcall SocketBase listen 2a 1002
#pragma libcall SocketBase accept 30 98003
#pragma libcall SocketBase connect 36 18003
#pragma libcall SocketBase sendto 3c 39218006
#pragma libcall SocketBase send 42 218004
#pragma libcall SocketBase recvfrom 48 A9218006
#pragma libcall SocketBase recv 4e 218004
#pragma libcall SocketBase shutdown 54 1002
#pragma libcall SocketBase setsockopt 5a 3821005
#pragma libcall SocketBase getsockopt 60 9821005
#pragma libcall SocketBase getsockname 66 98003
#pragma libcall SocketBase getpeername 6c 98003
#pragma libcall SocketBase IoctlSocket 72 81003
#pragma libcall SocketBase CloseSocket 78 001
#pragma libcall SocketBase WaitSelect 7e 1BA98006
#pragma libcall SocketBase SetSocketSignals 84 21003
#pragma libcall SocketBase getdtablesize 8a 0
#pragma libcall SocketBase ObtainSocket 90 321004
#pragma libcall SocketBase ReleaseSocket 96 1002
#pragma libcall SocketBase ReleaseCopyOfSocket 9c 1002
#pragma libcall SocketBase Errno a2 0
#pragma libcall SocketBase SetErrnoPtr a8 0802
#pragma libcall SocketBase Inet_NtoA ae 001
#pragma libcall SocketBase inet_addr b4 801
#pragma libcall SocketBase Inet_LnaOf ba 001
#pragma libcall SocketBase Inet_NetOf c0 001
#pragma libcall SocketBase Inet_MakeAddr c6 1002
#pragma libcall SocketBase inet_network cc 801
#pragma libcall SocketBase gethostbyname d2 801
#pragma libcall SocketBase gethostbyaddr d8 10803
#pragma libcall SocketBase getnetbyname de 801
#pragma libcall SocketBase getnetbyaddr e4 1002
#pragma libcall SocketBase getservbyname ea 9802
#pragma libcall SocketBase getservbyport f0 8002
#pragma libcall SocketBase getprotobyname f6 801
#pragma libcall SocketBase getprotobynumber fc 001
#pragma libcall SocketBase vsyslog 102 98003
#pragma tagcall SocketBase syslog 102 98003
#pragma libcall SocketBase Dup2Socket 108 1002
#pragma libcall SocketBase sendmsg 10e 18003
#pragma libcall SocketBase recvmsg 114 18003
#pragma libcall SocketBase gethostname 11a 0802
#pragma libcall SocketBase gethostid 120 0
#pragma libcall SocketBase SocketBaseTagList 126 801
#pragma tagcall SocketBase SocketBaseTags 126 801
#pragma libcall SocketBase GetSocketEvents 12c 801
#else
#ifndef __PPCINLINE_MACROS_H
#include <ppcinline/macros.h>
#endif /* !__PPCINLINE_MACROS_H */

#ifndef SOCKET_BASE_NAME
#define SOCKET_BASE_NAME SocketBase
#endif /* !SOCKET_BASE_NAME */

#define SetErrnoPtr(__p0, __p1) \
	LP2(168, LONG , SetErrnoPtr, \
		void *, __p0, a0, \
		LONG , __p1, d0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define recv(__p0, __p1, __p2, __p3) \
	LP4(78, LONG , recv, \
		LONG , __p0, d0, \
		UBYTE *, __p1, a0, \
		LONG , __p2, d1, \
		LONG , __p3, d2, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define gethostbyaddr(__p0, __p1, __p2) \
	LP3(216, struct hostent *, gethostbyaddr, \
		const UBYTE *, __p0, a0, \
		LONG , __p1, d0, \
		LONG , __p2, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define SocketBaseTagList(__p0) \
	LP1(294, LONG , SocketBaseTagList, \
		struct TagItem *, __p0, a0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define setsockopt(__p0, __p1, __p2, __p3, __p4) \
	LP5(90, LONG , setsockopt, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		LONG , __p2, d2, \
		const void *, __p3, a0, \
		LONG , __p4, d3, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getprotobyname(__p0) \
	LP1(246, struct protoent *, getprotobyname, \
		const UBYTE *, __p0, a0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define WaitSelect(__p0, __p1, __p2, __p3, __p4, __p5) \
	LP6(126, LONG , WaitSelect, \
		LONG , __p0, d0, \
		fd_set *, __p1, a0, \
		fd_set *, __p2, a1, \
		fd_set *, __p3, a2, \
		struct timeval *, __p4, a3, \
		ULONG *, __p5, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define gethostname(__p0, __p1) \
	LP2(282, LONG , gethostname, \
		STRPTR , __p0, a0, \
		LONG , __p1, d0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getdtablesize() \
	LP0(138, LONG , getdtablesize, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define listen(__p0, __p1) \
	LP2(42, LONG , listen, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getsockname(__p0, __p1, __p2) \
	LP3(102, LONG , getsockname, \
		LONG , __p0, d0, \
		struct sockaddr *, __p1, a0, \
		LONG *, __p2, a1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define gethostbyname(__p0) \
	LP1(210, struct hostent *, gethostbyname, \
		const UBYTE *, __p0, a0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define shutdown(__p0, __p1) \
	LP2(84, LONG , shutdown, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getservbyname(__p0, __p1) \
	LP2(234, struct servent *, getservbyname, \
		const UBYTE *, __p0, a0, \
		const UBYTE *, __p1, a1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define bind(__p0, __p1, __p2) \
	LP3(36, LONG , bind, \
		LONG , __p0, d0, \
		const struct sockaddr *, __p1, a0, \
		LONG , __p2, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getnetbyaddr(__p0, __p1) \
	LP2(228, struct netent *, getnetbyaddr, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getservbyport(__p0, __p1) \
	LP2(240, struct servent *, getservbyport, \
		LONG , __p0, d0, \
		const UBYTE *, __p1, a0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define recvfrom(__p0, __p1, __p2, __p3, __p4, __p5) \
	LP6(72, LONG , recvfrom, \
		LONG , __p0, d0, \
		UBYTE *, __p1, a0, \
		LONG , __p2, d1, \
		LONG , __p3, d2, \
		struct sockaddr *, __p4, a1, \
		LONG *, __p5, a2, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define SetSocketSignals(__p0, __p1, __p2) \
	LP3NR(132, SetSocketSignals, \
		ULONG , __p0, d0, \
		ULONG , __p1, d1, \
		ULONG , __p2, d2, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define socket(__p0, __p1, __p2) \
	LP3(30, LONG , socket, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		LONG , __p2, d2, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define connect(__p0, __p1, __p2) \
	LP3(54, LONG , connect, \
		LONG , __p0, d0, \
		const struct sockaddr *, __p1, a0, \
		LONG , __p2, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Inet_NetOf(__p0) \
	LP1(192, ULONG , Inet_NetOf, \
		LONG , __p0, d0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define ReleaseCopyOfSocket(__p0, __p1) \
	LP2(156, LONG , ReleaseCopyOfSocket, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Inet_NtoA(__p0) \
	LP1(174, char *, Inet_NtoA, \
		ULONG , __p0, d0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Errno() \
	LP0(162, LONG , Errno, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define inet_addr(__p0) \
	LP1(180, ULONG , inet_addr, \
		const UBYTE *, __p0, a0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getsockopt(__p0, __p1, __p2, __p3, __p4) \
	LP5(96, LONG , getsockopt, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		LONG , __p2, d2, \
		void *, __p3, a0, \
		LONG *, __p4, a1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getnetbyname(__p0) \
	LP1(222, struct netent *, getnetbyname, \
		const UBYTE *, __p0, a0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Dup2Socket(__p0, __p1) \
	LP2(264, LONG , Dup2Socket, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define ObtainSocket(__p0, __p1, __p2, __p3) \
	LP4(144, LONG , ObtainSocket, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		LONG , __p2, d2, \
		LONG , __p3, d3, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define sendto(__p0, __p1, __p2, __p3, __p4, __p5) \
	LP6(60, LONG , sendto, \
		LONG , __p0, d0, \
		const UBYTE *, __p1, a0, \
		LONG , __p2, d1, \
		LONG , __p3, d2, \
		const struct sockaddr *, __p4, a1, \
		LONG , __p5, d3, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Inet_MakeAddr(__p0, __p1) \
	LP2(198, ULONG , Inet_MakeAddr, \
		ULONG , __p0, d0, \
		ULONG , __p1, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define send(__p0, __p1, __p2, __p3) \
	LP4(66, LONG , send, \
		LONG , __p0, d0, \
		const UBYTE *, __p1, a0, \
		LONG , __p2, d1, \
		LONG , __p3, d2, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define CloseSocket(__p0) \
	LP1(120, LONG , CloseSocket, \
		LONG , __p0, d0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define IoctlSocket(__p0, __p1, __p2) \
	LP3(114, LONG , IoctlSocket, \
		LONG , __p0, d0, \
		ULONG , __p1, d1, \
		char *, __p2, a0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define ReleaseSocket(__p0, __p1) \
	LP2(150, LONG , ReleaseSocket, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define gethostid() \
	LP0(288, ULONG , gethostid, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getpeername(__p0, __p1, __p2) \
	LP3(108, LONG , getpeername, \
		LONG , __p0, d0, \
		struct sockaddr *, __p1, a0, \
		LONG *, __p2, a1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define accept(__p0, __p1, __p2) \
	LP3(48, LONG , accept, \
		LONG , __p0, d0, \
		struct sockaddr *, __p1, a0, \
		LONG *, __p2, a1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define vsyslog(__p0, __p1, __p2) \
	LP3NR(258, vsyslog, \
		ULONG , __p0, d0, \
		const char *, __p1, a0, \
		LONG *, __p2, a1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getprotobynumber(__p0) \
	LP1(252, struct protoent *, getprotobynumber, \
		LONG , __p0, d0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define inet_network(__p0) \
	LP1(204, ULONG , inet_network, \
		const UBYTE *, __p0, a0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define GetSocketEvents(__p0) \
	LP1(300, LONG , GetSocketEvents, \
		ULONG *, __p0, a0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Inet_LnaOf(__p0) \
	LP1(186, ULONG , Inet_LnaOf, \
		LONG , __p0, d0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#ifdef USE_INLINE_STDARG

#include <stdarg.h>

#define syslog(__p0, __p1, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; \
	vsyslog(__p0, __p1, (LONG *)_tags);})

#define SocketBaseTags(...) \
	({ULONG _tags[] = { __VA_ARGS__ }; \
	SocketBaseTagList((struct TagItem *)_tags);})

#endif


#endif

/***********************************************************************/
