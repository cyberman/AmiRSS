
#ifdef __MORPHOS__
/* stubs.c */
#undef NewObject
#undef MUI_NewObject
#undef DoSuperNew
APTR NewObject ( struct IClass *classPtr , STRPTR classID , ...) __attribute((varargs68k));
APTR MUI_NewObject ( STRPTR classID , ...) __attribute((varargs68k));
APTR DoSuperNew ( struct IClass *cl , Object *obj , ...) __attribute((varargs68k));
#endif

/* parseargs.c */
LONG parseArgs ( struct parseArgs *pa );

/* loc.c */
STRPTR getStaticString ( ULONG id );
struct Catalog *openCatalog ( STRPTR name , ULONG minVer , ULONG minRev );
STRPTR getString ( ULONG id );
void localizeStrings ( STRPTR *s );
void localizeMenus ( struct NewMenu *menu );
void localizeButtonsBar ( struct MUIS_TheBar_Button *button );
ULONG getKeyCharID ( LONG id );

/* utils.c */
#ifndef __MORPHOS__
ULONG STDARGS DoSuperNew ( struct IClass *cl , Object *obj , ULONG tag1 , ...);
void STDARGS msprintf ( STRPTR to , STRPTR fmt , ...);
#else
#define msprintf(to, fmt, ...) ({ ULONG _tags[] = { __VA_ARGS__ }; RawDoFmt(fmt, _tags, (void (*)(void)) 0, to); })
#endif
int STDARGS msnprintf ( STRPTR buf , int size , STRPTR fmt , ...) __attribute((varargs68k));
STRPTR STDARGS asnprintf ( struct astream *st , STRPTR fmt , ...) __attribute((varargs68k));
APTR allocVecPooled ( APTR pool , ULONG size );
APTR reallocVecPooled ( APTR pool , APTR mem , size_t size );
APTR reallocVecPooledNC ( APTR pool , APTR mem , size_t size );
void freeVecPooled ( APTR pool , APTR mem );
ULONG xget ( Object *obj , ULONG attr );
Object *ostring ( ULONG maxlen , ULONG key , ULONG help );
Object *oaestring ( ULONG maxlen , ULONG key , ULONG help );
Object *ocheck ( ULONG key , ULONG help );
Object *obutton ( ULONG text , ULONG help );
Object *ocycle ( ULONG key , ULONG help , STRPTR *array );
Object *olabel ( ULONG id );
Object *ollabel ( ULONG id );
Object *olabel1 ( ULONG id );
Object *ollabel1 ( ULONG id );
Object *olabel2 ( ULONG id );
Object *opopbutton ( ULONG img , ULONG help );
Object *ofile ( Object **obj , ULONG size , ULONG txt , ULONG help , ULONG title , ULONG drawersOnly );
Object *oslider ( ULONG key , ULONG help , LONG min , LONG max );
APTR reallocArbitrateVecPooled ( APTR mem , size_t size );
APTR reallocArbitrateVecPooledNC ( APTR mem , size_t size );
ULONG openWindow ( Object *app , Object *win );
void reset_asprint ( struct astream *st );
ULONG goURLFun ( STRPTR URL );
ULONG miniMailFun ( Object *app , STRPTR URL );
void bookmarkFun ( STRPTR URL );
void etranslate ( STRPTR buffer , STRPTR to , int len , ULONG doSubstWinChars );
void stripHTML ( STRPTR from , STRPTR to , BOOL remMUICodes );
ULONG executeCommand ( APTR pool , STRPTR cmd , Object *win , STRPTR value , ULONG fileType );
BPTR openFile ( STRPTR name , ULONG mode );
void closeFile ( BPTR file );
LONG readFile ( BPTR file , STRPTR buf , ULONG len );
LONG writeFile ( BPTR file , APTR buf , ULONG len );
APTR allocArbitratePooled ( ULONG size );
void freeArbitratePooled ( APTR mem , ULONG size );
APTR allocArbitrateVecPooled ( ULONG size );
void freeArbitrateVecPooled ( APTR mem );

/* date.c */
void getdate ( struct DateStamp *ds , STRPTR date , int GMTOffset );

/* options.c */
void setDefaultBPrefs ( struct bprefs *prefs );
void setDefaultNBPrefs ( struct bprefs *prefs );
void setDefaultPrefs ( struct prefs *prefs );
ULONG savePrefs ( struct prefs *prefs );
ULONG loadPrefs ( struct prefs *prefs );

/* lineread.c */
void initLineRead ( struct lineRead *lr , struct Library *socketBase , int fd , int type , int bufferSize );
int lineRead ( struct lineRead *lr );

/* uri.c */
int parseURI ( STRPTR URI , struct URI *uptr , ULONG flags );
void freeURI ( struct URI *uptr );

/* app.c */
ULONG initAppClass ( void );
void disposeAppClass ( void );

/* win.c */
ULONG initWinClass ( void );
void disposeWinClass ( void );

/* bwin.c */
ULONG initBWinClass ( void );
void disposeBWinClass ( void );

/* about.c */
Object *olabel ( ULONG id );
Object *ollabel ( ULONG id );
Object *ourltext ( STRPTR url , STRPTR text );
ULONG initAboutClass ( void );
void disposeAboutClass ( void );

/* prefs.c */
ULONG initPrefsClass ( void );
void disposePrefsClass ( void );

/* search.c */
ULONG initSearchClass ( void );
void disposeSearchClass ( void );

/* editfeed.c */
ULONG initEditFeedClass ( void );
void disposeEditFeedClass ( void );

/* group.c */
ULONG initGroupClass ( void );
void disposeGroupClass ( void );

/* rootbar.c */
ULONG initRootBarClass ( void );
void disposeRootBarClass ( void );

/* rootstring.c */
ULONG initRootStringClass ( void );
void disposeRootStringClass ( void );

/* rootscrollgroup.c */
ULONG initRootScrollgroupClass ( void );
void disposeRootScrollgroupClass ( void );

///* rootlist.c */
//ULONG initRootListClass ( void );
//void disposeRootListClass ( void );

/* grouplist.c */
ULONG initGroupListClass ( void );
void disposeGroupListClass ( void );

/* HTML.c */
ULONG initHTMLClass ( void );
void disposeHTMLClass ( void );

/* entrieslist.c */
void freeNew ( APTR pool , struct entry *entry );
ULONG initEntriesListClass ( void );
void disposeEntriesListClass ( void );

/* popupcodesets.c */
ULONG initPopupCodesetsClass ( void );
void disposePopupCodesetsClass ( void );

/* agentClass.c */
ULONG initAgentClass ( void );
void disposeAgentClass ( void );

/* gmt.c */
ULONG initGMTClass ( void );
void disposeGMTClass ( void );

/* cm.c */
ULONG initCMClass ( void );
void disposeCMClass ( void );

/* minimail.c */
ULONG initMiniMailClass ( void );
void disposeMiniMailClass ( void );

/* pic.c */
ULONG initPicClass ( void );
void disposePicClass ( void );

/* popph.c */
ULONG initPopphClass ( void );
void disposePopphClass ( void );

/* http.c */
#ifdef __MORPHOS__
void httpProc ( void );
#else
void SAVEDS httpProc ( void );
#endif

/* favIcon.c */
#ifdef __MORPHOS__
void favIconProc ( void );
#else
void SAVEDS favIconProc ( void );
#endif

