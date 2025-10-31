#ifndef PROTO_CMANAGER_H
#define PROTO_CMANAGER_H

#ifndef __NOLIBBASE__
extern struct Library *
#ifdef __CONSTLIBBASEDECL__
__CONSTLIBBASEDECL__
#endif /* __CONSTLIBBASEDECL__ */
CManagerBase;
#endif /* !__NOLIBBASE__ */

#include <clib/CManager_protos.h>

#ifdef __GNUC__
#ifdef __PPC__
#ifndef _NO_PPCINLINE
#include <ppcinline/CManager.h>
#endif /* _NO_PPCINLINE */
#else
#ifndef _NO_INLINE
#include <inline/CManager.h>
#endif /* _NO_INLINE */
#endif /* __PPC__ */
#else
#include <pragmas/CManager_pragmas.h>
#endif /* __GNUC__ */

#endif /* !PROTO_CMANAGER_H */

