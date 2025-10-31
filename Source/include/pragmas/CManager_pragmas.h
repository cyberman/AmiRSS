#ifndef PRAGMAS_CMANAGER_PRAGMAS_H
#define PRAGMAS_CMANAGER_PRAGMAS_H

/*
**  $VER: CManager_pragmas.h 30.0 (25.10.2004)
**  Includes Release 30.0
**
**  Written by Simone Tellini and Alfonso Ranieri <alforan@tin.it>.
**
**  Released under the terms of the
**  GNU Public Licence version 2
*/

#ifndef CLIB_CMANAGER_PROTOS_H
#include <clib/CManager_protos.h>
#endif

#if defined(AZTEC_C) || defined(__MAXON__) || defined(__STORM__)
#pragma amicall(CManagerBase,0x01e,CM_StartManager(a0,a1))
#pragma amicall(CManagerBase,0x036,CM_GetParent(a0,a1))
#pragma amicall(CManagerBase,0x03c,CM_AllocEntry(d0))
#pragma amicall(CManagerBase,0x042,CM_FreeEntry(a0))
#pragma amicall(CManagerBase,0x048,CM_FreeHandle(a0,d0))
#pragma amicall(CManagerBase,0x04e,CM_GetEntry(a0,d0))
#pragma amicall(CManagerBase,0x060,CM_AddEntry(a0))
#pragma amicall(CManagerBase,0x066,CM_FreeList(a0))
#pragma amicall(CManagerBase,0x07e,CM_FreeData(a0))
#pragma amicall(CManagerBase,0x084,CM_AllocCMData())
#pragma amicall(CManagerBase,0x08a,CM_GetString(d0))
#pragma amicall(CManagerBase,0x090,CM_GetOption(d0))
#pragma amicall(CManagerBase,0x096,CM_StartCManagerA(a0))
#pragma amicall(CManagerBase,0x09c,CM_AllocObject(d0))
#pragma amicall(CManagerBase,0x0a2,CM_FreeObject(a0))
#pragma amicall(CManagerBase,0x0a8,CM_LoadDataA(a0))
#pragma amicall(CManagerBase,0x0ae,CM_SaveDataA(a0))
#pragma amicall(CManagerBase,0x0b4,CM_CodesetsSupported())
#pragma amicall(CManagerBase,0x0ba,CM_CodesetsFind(a0))
#pragma amicall(CManagerBase,0x0c0,CM_UTF8tostr(a0,a1,a2,d0))
#endif
#if defined(_DCC) || defined(__SASC)
#pragma  libcall CManagerBase CM_StartManager        01e 9802
#pragma  libcall CManagerBase CM_GetParent           036 9802
#pragma  libcall CManagerBase CM_AllocEntry          03c 001
#pragma  libcall CManagerBase CM_FreeEntry           042 801
#pragma  libcall CManagerBase CM_FreeHandle          048 0802
#pragma  libcall CManagerBase CM_GetEntry            04e 0802
#pragma  libcall CManagerBase CM_AddEntry            060 801
#pragma  libcall CManagerBase CM_FreeList            066 801
#pragma  libcall CManagerBase CM_FreeData            07e 801
#pragma  libcall CManagerBase CM_AllocCMData         084 00
#pragma  libcall CManagerBase CM_GetString           08a 001
#pragma  libcall CManagerBase CM_GetOption           090 001
#pragma  libcall CManagerBase CM_StartCManagerA      096 801
#pragma  libcall CManagerBase CM_AllocObject         09c 001
#pragma  libcall CManagerBase CM_FreeObject          0a2 801
#pragma  libcall CManagerBase CM_LoadDataA           0a8 801
#pragma  libcall CManagerBase CM_SaveDataA           0ae 801
#pragma  libcall CManagerBase CM_CodesetsSupported   0b4 00
#pragma  libcall CManagerBase CM_CodesetsFind        0ba 801
#pragma  libcall CManagerBase CM_UTF8tostr           0c0 0a9804
#endif
#ifdef __STORM__
#pragma tagcall(CManagerBase,0x096,CM_StartCManager(a0))
#pragma tagcall(CManagerBase,0x0a8,CM_LoadData(a0))
#pragma tagcall(CManagerBase,0x0ae,CM_SaveData(a0))
#endif
#ifdef __SASC_60
#pragma  tagcall CManagerBase CM_StartCManager       096 801
#pragma  tagcall CManagerBase CM_LoadData            0a8 801
#pragma  tagcall CManagerBase CM_SaveData            0ae 801
#endif

#endif /* PRAGMAS_CMANAGER_PRAGMAS_H */
