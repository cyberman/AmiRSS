#ifndef URI_H
#define URI_H

/****************************************************************************/

enum
{
    URISCHEME_HTTP = 1,
    URISCHEME_HTTPS,
    URISCHEME_FTP_DATA,
    URISCHEME_FTP,
    URISCHEME_GOPHER,
    URISCHEME_WAIS,
    URISCHEME_SNEWS,
    URISCHEME_PROSPERO,
    URISCHEME_FINGER,
    URISCHEME_DAYTIME,
    URISCHEME_SMTP,
    URISCHEME_POP,
    URISCHEME_NNTP,

    URISCHEME_MAILTO,
    URISCHEME_FILE,
    URISCHEME_AMRSS,
};

#define DEFAULT_HTTP_PORT         80
#define DEFAULT_HTTPS_PORT       443
#define DEFAULT_FTP_DATA_PORT     20
#define DEFAULT_FTP_PORT          21
#define DEFAULT_GOPHER_PORT       70
#define DEFAULT_NNTP_PORT        119
#define DEFAULT_WAIS_PORT        210
#define DEFAULT_SNEWS_PORT       563
#define DEFAULT_PROSPERO_PORT   1525
#define DEFAULT_FINGER_PORT       79
#define DEFAULT_DAYTIME_PORT      13
#define DEFAULT_SMTP_PORT         25
#define DEFAULT_POP_PORT         110

/****************************************************************************/

struct URI
{
    APTR            pool;
    ULONG           schemeid;
    STRPTR          scheme;            /* scheme ("http"/"ftp"/...) */
    STRPTR          hostinfo;          /* combined [user[:password]@]host[:port] */
    STRPTR          user;              /* user name, as in http://user:passwd@host:port/ */
    STRPTR          password;          /* password, as in http://user:passwd@host:port/ */
    STRPTR          hostname;          /* hostname from URI (or from Host: header) */
    STRPTR          port_str;          /* port string (integer representation is in "port") */
    STRPTR          path;              /* the request path (or "/" if only scheme://host was given) */
    STRPTR          query;             /* Everything after a '?' in the path, if present */
    STRPTR          fragment;          /* Trailing "#fragment" string, if present */
    USHORT          port;              /* The port number, numeric, valid only if port_str != NULL */
};

enum
{
    URIFLG_SHORT = 1<<0,
};

/****************************************************************************/

#endif /* URI_H */
