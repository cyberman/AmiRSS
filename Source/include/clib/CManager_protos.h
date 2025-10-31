#ifndef CLIB_CMANAGER_PROTOS_H
#define CLIB_CMANAGER_PROTOS_H

/*
**  $VER: CManager_protos.h 30.0 (25.10.2004)
**  Includes Release 30.0
**
**  C prototypes. For use with 32 bit integers only.
**
**  Written by Simone Tellini <wiz@vapor.com> and
**             Alfonso Ranieri <alforan@tin.it>.
**
**  Released under the terms of the
**  GNU Public Licence version 2
*/

#ifndef LIBRARIES_CMANAGER_H
#include <libraries/CManager.h>
#endif

/* Database */
ULONG CM_LoadDataA ( struct TagItem *attrs );
ULONG CM_LoadData ( Tag , ... );
void CM_SaveDataA ( struct TagItem *attrs );
void CM_SaveData ( Tag , ... );
struct CMData *CM_AllocCMData( void );
struct CMGroup  *CM_GetParent( struct CMGroup *, struct CMGroup * );
void CM_FreeData( struct CMData * );
APTR CM_AllocEntry( ULONG );
void CM_FreeEntry( APTR );
APTR CM_GetEntry( APTR, ULONG );
BOOL CM_AddEntry( APTR );
void CM_FreeList( struct MinList * );

/* Application */
APTR CM_StartManager( STRPTR, STRPTR );
void CM_FreeHandle( APTR, BOOL );
APTR CM_StartCManagerA( struct TagItem *);
APTR CM_StartCManager( Tag , ... );

/* Various */
STRPTR CM_GetString( ULONG ID );
ULONG CM_GetOption(ULONG option);
APTR CM_AllocObject( ULONG type);
void CM_FreeObject( APTR object );

/* Codesets */
STRPTR *CM_CodesetsSupported ( void );
int CM_UTF8tostr ( struct codeset *codeset , char *str , char *dest , int dest_size );
struct codeset *CM_CodesetsFind ( STRPTR name );

#endif /* CLIB_CMANAGER_PROTOS_H */
