             
             
       
       
#pragma pack(push,_CRT_PACKING)
  typedef __builtin_va_list __gnuc_va_list;
  typedef __gnuc_va_list va_list;
#pragma pack(pop)
void __attribute__((__cdecl__)) __debugbreak(void);
extern __inline__ __attribute__((__always_inline__,__gnu_inline__)) void __attribute__((__cdecl__)) __debugbreak(void)
{
  __asm__ __volatile__("int {$}3":);
}
void __attribute__((__cdecl__)) __attribute__ ((__noreturn__)) __fastfail(unsigned int code);
extern __inline__ __attribute__((__always_inline__,__gnu_inline__)) void __attribute__((__cdecl__)) __attribute__ ((__noreturn__)) __fastfail(unsigned int code)
{
  __asm__ __volatile__("int {$}0x29"::"c"(code));
  __builtin_unreachable();
}
const char *__mingw_get_crt_info (void);
#pragma pack(push,_CRT_PACKING)
__extension__ typedef unsigned long long size_t;
__extension__ typedef long long ssize_t;
typedef size_t rsize_t;
__extension__ typedef long long intptr_t;
__extension__ typedef unsigned long long uintptr_t;
__extension__ typedef long long ptrdiff_t;
typedef unsigned short wchar_t;
typedef unsigned short wint_t;
typedef unsigned short wctype_t;
typedef int errno_t;
typedef long __time32_t;
__extension__ typedef long long __time64_t;
typedef __time64_t time_t;
struct threadlocaleinfostruct;
struct threadmbcinfostruct;
typedef struct threadlocaleinfostruct *pthreadlocinfo;
typedef struct threadmbcinfostruct *pthreadmbcinfo;
struct __lc_time_data;
typedef struct localeinfo_struct {
  pthreadlocinfo locinfo;
  pthreadmbcinfo mbcinfo;
} _locale_tstruct,*_locale_t;
typedef struct tagLC_ID {
  unsigned short wLanguage;
  unsigned short wCountry;
  unsigned short wCodePage;
} LC_ID,*LPLC_ID;
typedef struct threadlocaleinfostruct {
  int refcount;
  unsigned int lc_codepage;
  unsigned int lc_collate_cp;
  unsigned long lc_handle[6];
  LC_ID lc_id[6];
  struct {
    char *locale;
    wchar_t *wlocale;
    int *refcount;
    int *wrefcount;
  } lc_category[6];
  int lc_clike;
  int mb_cur_max;
  int *lconv_intl_refcount;
  int *lconv_num_refcount;
  int *lconv_mon_refcount;
  struct lconv *lconv;
  int *ctype1_refcount;
  unsigned short *ctype1;
  const unsigned short *pctype;
  const unsigned char *pclmap;
  const unsigned char *pcumap;
  struct __lc_time_data *lc_time_curr;
} threadlocinfo;
#pragma pack(pop)
  __attribute__ ((__dllimport__)) extern int *__attribute__((__cdecl__)) _errno(void);
  errno_t __attribute__((__cdecl__)) _set_errno(int _Value);
  errno_t __attribute__((__cdecl__)) _get_errno(int *_Value);
  __attribute__ ((__dllimport__)) extern unsigned long __attribute__((__cdecl__)) __threadid(void);
  __attribute__ ((__dllimport__)) extern uintptr_t __attribute__((__cdecl__)) __threadhandle(void);
typedef struct {
  long long __max_align_ll __attribute__((__aligned__(__alignof__(long long))));
  long double __max_align_ld __attribute__((__aligned__(__alignof__(long double))));
} max_align_t;
typedef enum err_type {
    PEGGY_SUCCESS = 0,
    PEGGY_FAILURE,
    PEGGY_MALLOC_FAILURE,
    PEGGY_INIT_FAILURE,
    PEGGY_NOT_IMPLEMENTED,
    PEGGY_INDEX_OUT_OF_BOUNDS,
    PEGGY_MALLOC_MAP_FAILURE,
    PEGGY_MALLOC_PAIR_FAILURE,
    PEGGY_EMPTY_STACK,
    PEGGY_EMPTY_DEQUE,
    PEGGY_REGEX_FAILURE,
    PEGGY_FILE_IO_ERROR,
    PEGGY_NO_ROOT_PRODUCTION_FOUND,
    PEGGY_NO_TOKENIZER_FOUND,
} err_type;
typedef enum iter_status {
    PEGGY_ITER_GO = 0,
    PEGGY_ITER_STOP
} iter_status;
typedef int rule_id_type;
typedef unsigned long long hash_type;
typedef struct Rule Rule;
typedef struct Parser Parser;
_Bool is_prime(size_t x);
size_t next_prime(size_t x);
typedef unsigned short _ino_t;
typedef unsigned short ino_t;
typedef unsigned int _dev_t;
typedef unsigned int dev_t;
__extension__
typedef long long _pid_t;
typedef _pid_t pid_t;
typedef unsigned short _mode_t;
typedef _mode_t mode_t;
  typedef long _off_t;
  typedef long off32_t;
  __extension__ typedef long long _off64_t;
  __extension__ typedef long long off64_t;
typedef off32_t off_t;
typedef unsigned int useconds_t;
struct timespec {
  time_t tv_sec;
  long tv_nsec;
};
struct itimerspec {
  struct timespec it_interval;
  struct timespec it_value;
};
__extension__
typedef unsigned long long _sigset_t;
typedef int regoff_t;
typedef struct {
  size_t re_nsub;
  void *value;
} regex_t;
typedef struct {
  regoff_t rm_so;
  regoff_t rm_eo;
} regmatch_t;
typedef enum {
  REG_OK = 0,
  REG_NOMATCH,
  REG_BADPAT,
  REG_ECOLLATE,
  REG_ECTYPE,
  REG_EESCAPE,
  REG_ESUBREG,
  REG_EBRACK,
  REG_EPAREN,
  REG_EBRACE,
  REG_BADBR,
  REG_ERANGE,
  REG_ESPACE,
  REG_BADRPT,
  REG_BADMAX,
} reg_errcode_t;
extern int
tre_regcomp(regex_t *preg, const char *regex, int cflags);
extern int
tre_regexec(const regex_t *preg, const char *string, size_t nmatch,
 regmatch_t pmatch[], int eflags);
extern int
tre_regcompb(regex_t *preg, const char *regex, int cflags);
extern int
tre_regexecb(const regex_t *preg, const char *string, size_t nmatch,
 regmatch_t pmatch[], int eflags);
extern size_t
tre_regerror(int errcode, const regex_t *preg, char *errbuf,
  size_t errbuf_size);
extern void
tre_regfree(regex_t *preg);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _itow_s (int _Val,wchar_t *_DstBuf,size_t _SizeInWords,int _Radix);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _ltow_s (long _Val,wchar_t *_DstBuf,size_t _SizeInWords,int _Radix);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _ultow_s (unsigned long _Val,wchar_t *_DstBuf,size_t _SizeInWords,int _Radix);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wgetenv_s(size_t *_ReturnSize,wchar_t *_DstBuf,size_t _DstSizeInWords,const wchar_t *_VarName);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wdupenv_s(wchar_t **_Buffer,size_t *_BufferSizeInWords,const wchar_t *_VarName);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _i64tow_s(long long _Val,wchar_t *_DstBuf,size_t _SizeInWords,int _Radix);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _ui64tow_s(unsigned long long _Val,wchar_t *_DstBuf,size_t _SizeInWords,int _Radix);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wmakepath_s(wchar_t *_PathResult,size_t _SizeInWords,const wchar_t *_Drive,const wchar_t *_Dir,const wchar_t *_Filename,const wchar_t *_Ext);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wputenv_s(const wchar_t *_Name,const wchar_t *_Value);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wsearchenv_s(const wchar_t *_Filename,const wchar_t *_EnvVar,wchar_t *_ResultPath,size_t _SizeInWords);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wsplitpath_s(const wchar_t *_FullPath,wchar_t *_Drive,size_t _DriveSizeInWords,wchar_t *_Dir,size_t _DirSizeInWords,wchar_t *_Filename,size_t _FilenameSizeInWords,wchar_t *_Ext,size_t _ExtSizeInWords);
 
#pragma pack(push,_CRT_PACKING)
  struct _iobuf {
    char *_ptr;
    int _cnt;
    char *_base;
    int _flag;
    int _file;
    int _charbuf;
    int _bufsiz;
    char *_tmpfname;
  };
  typedef struct _iobuf FILE;
__attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) __acrt_iob_func(unsigned index);
  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) __iob_func(void);
  typedef unsigned long _fsize_t;
  struct _wfinddata32_t {
    unsigned attrib;
    __time32_t time_create;
    __time32_t time_access;
    __time32_t time_write;
    _fsize_t size;
    wchar_t name[260];
  };
  struct _wfinddata32i64_t {
    unsigned attrib;
    __time32_t time_create;
    __time32_t time_access;
    __time32_t time_write;
    __extension__ long long size;
    wchar_t name[260];
  };
  struct _wfinddata64i32_t {
    unsigned attrib;
    __time64_t time_create;
    __time64_t time_access;
    __time64_t time_write;
    _fsize_t size;
    wchar_t name[260];
  };
  struct _wfinddata64_t {
    unsigned attrib;
    __time64_t time_create;
    __time64_t time_access;
    __time64_t time_write;
    __extension__ long long size;
    wchar_t name[260];
  };
  extern unsigned short ** __imp__pctype;
  extern unsigned short ** __imp__wctype;
  extern unsigned short ** __imp__pwctype;
  int __attribute__((__cdecl__)) iswalpha(wint_t _C);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _iswalpha_l(wint_t _C,_locale_t _Locale);
  int __attribute__((__cdecl__)) iswupper(wint_t _C);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _iswupper_l(wint_t _C,_locale_t _Locale);
  int __attribute__((__cdecl__)) iswlower(wint_t _C);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _iswlower_l(wint_t _C,_locale_t _Locale);
  int __attribute__((__cdecl__)) iswdigit(wint_t _C);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _iswdigit_l(wint_t _C,_locale_t _Locale);
  int __attribute__((__cdecl__)) iswxdigit(wint_t _C);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _iswxdigit_l(wint_t _C,_locale_t _Locale);
  int __attribute__((__cdecl__)) iswspace(wint_t _C);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _iswspace_l(wint_t _C,_locale_t _Locale);
  int __attribute__((__cdecl__)) iswpunct(wint_t _C);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _iswpunct_l(wint_t _C,_locale_t _Locale);
  int __attribute__((__cdecl__)) iswalnum(wint_t _C);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _iswalnum_l(wint_t _C,_locale_t _Locale);
  int __attribute__((__cdecl__)) iswprint(wint_t _C);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _iswprint_l(wint_t _C,_locale_t _Locale);
  int __attribute__((__cdecl__)) iswgraph(wint_t _C);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _iswgraph_l(wint_t _C,_locale_t _Locale);
  int __attribute__((__cdecl__)) iswcntrl(wint_t _C);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _iswcntrl_l(wint_t _C,_locale_t _Locale);
  int __attribute__((__cdecl__)) iswascii(wint_t _C);
  int __attribute__((__cdecl__)) isleadbyte(int _C);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _isleadbyte_l(int _C,_locale_t _Locale);
  wint_t __attribute__((__cdecl__)) towupper(wint_t _C);
  __attribute__ ((__dllimport__)) wint_t __attribute__((__cdecl__)) _towupper_l(wint_t _C,_locale_t _Locale);
  wint_t __attribute__((__cdecl__)) towlower(wint_t _C);
  __attribute__ ((__dllimport__)) wint_t __attribute__((__cdecl__)) _towlower_l(wint_t _C,_locale_t _Locale);
  int __attribute__((__cdecl__)) iswctype(wint_t _C,wctype_t _Type);
  int __attribute__((__cdecl__)) is_wctype(wint_t _C,wctype_t _Type);
  int __attribute__((__cdecl__)) iswblank(wint_t _C);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wgetcwd(wchar_t *_DstBuf,int _SizeInWords);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wgetdcwd(int _Drive,wchar_t *_DstBuf,int _SizeInWords);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wchdir(const wchar_t *_Path);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wmkdir(const wchar_t *_Path);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wrmdir(const wchar_t *_Path);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _waccess(const wchar_t *_Filename,int _AccessMode);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wchmod(const wchar_t *_Filename,int _Mode);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wcreat(const wchar_t *_Filename,int _PermissionMode) ;
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wfindfirst32(const wchar_t *_Filename,struct _wfinddata32_t *_FindData);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wfindnext32(intptr_t _FindHandle,struct _wfinddata32_t *_FindData);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wunlink(const wchar_t *_Filename);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wrename(const wchar_t *_OldFilename,const wchar_t *_NewFilename);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wmktemp(wchar_t *_TemplateName) ;
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wfindfirst32i64(const wchar_t *_Filename,struct _wfinddata32i64_t *_FindData);
  intptr_t __attribute__((__cdecl__)) _wfindfirst64i32(const wchar_t *_Filename,struct _wfinddata64i32_t *_FindData);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wfindfirst64(const wchar_t *_Filename,struct _wfinddata64_t *_FindData);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wfindnext32i64(intptr_t _FindHandle,struct _wfinddata32i64_t *_FindData);
  int __attribute__((__cdecl__)) _wfindnext64i32(intptr_t _FindHandle,struct _wfinddata64i32_t *_FindData);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wfindnext64(intptr_t _FindHandle,struct _wfinddata64_t *_FindData);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wsopen_s(int *_FileHandle,const wchar_t *_Filename,int _OpenFlag,int _ShareFlag,int _PermissionFlag);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wopen(const wchar_t *_Filename,int _OpenFlag,...) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wsopen(const wchar_t *_Filename,int _OpenFlag,int _ShareFlag,...) ;
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wsetlocale(int _Category,const wchar_t *_Locale);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wexecl(const wchar_t *_Filename,const wchar_t *_ArgList,...);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wexecle(const wchar_t *_Filename,const wchar_t *_ArgList,...);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wexeclp(const wchar_t *_Filename,const wchar_t *_ArgList,...);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wexeclpe(const wchar_t *_Filename,const wchar_t *_ArgList,...);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wexecv(const wchar_t *_Filename,const wchar_t *const *_ArgList);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wexecve(const wchar_t *_Filename,const wchar_t *const *_ArgList,const wchar_t *const *_Env);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wexecvp(const wchar_t *_Filename,const wchar_t *const *_ArgList);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wexecvpe(const wchar_t *_Filename,const wchar_t *const *_ArgList,const wchar_t *const *_Env);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wspawnl(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wspawnle(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wspawnlp(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wspawnlpe(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wspawnv(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wspawnve(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList,const wchar_t *const *_Env);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wspawnvp(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wspawnvpe(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList,const wchar_t *const *_Env);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wsystem(const wchar_t *_Command);
  struct _stat32 {
    _dev_t st_dev;
   _ino_t st_ino;
    unsigned short st_mode;
    short st_nlink;
    short st_uid;
    short st_gid;
    _dev_t st_rdev;
    _off_t st_size;
    __time32_t st_atime;
    __time32_t st_mtime;
    __time32_t st_ctime;
  };
  struct stat {
    _dev_t st_dev;
    _ino_t st_ino;
    unsigned short st_mode;
    short st_nlink;
    short st_uid;
    short st_gid;
    _dev_t st_rdev;
    _off_t st_size;
    time_t st_atime;
    time_t st_mtime;
    time_t st_ctime;
  };
  struct _stat32i64 {
    _dev_t st_dev;
    _ino_t st_ino;
    unsigned short st_mode;
    short st_nlink;
    short st_uid;
    short st_gid;
    _dev_t st_rdev;
    __extension__ long long st_size;
    __time32_t st_atime;
    __time32_t st_mtime;
    __time32_t st_ctime;
  };
  struct _stat64i32 {
    _dev_t st_dev;
    _ino_t st_ino;
    unsigned short st_mode;
    short st_nlink;
    short st_uid;
    short st_gid;
    _dev_t st_rdev;
    _off_t st_size;
    __time64_t st_atime;
    __time64_t st_mtime;
    __time64_t st_ctime;
  };
  struct _stat64 {
    _dev_t st_dev;
    _ino_t st_ino;
    unsigned short st_mode;
    short st_nlink;
    short st_uid;
    short st_gid;
    _dev_t st_rdev;
    __extension__ long long st_size;
    __time64_t st_atime;
    __time64_t st_mtime;
    __time64_t st_ctime;
  };
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wstat32(const wchar_t *_Name,struct _stat32 *_Stat);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wstat32i64(const wchar_t *_Name,struct _stat32i64 *_Stat);
  int __attribute__((__cdecl__)) _wstat64i32(const wchar_t *_Name,struct _stat64i32 *_Stat);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wstat64(const wchar_t *_Name,struct _stat64 *_Stat);
  __attribute__ ((__dllimport__)) wchar_t *_cgetws(wchar_t *_Buffer) ;
  __attribute__ ((__dllimport__)) wint_t __attribute__((__cdecl__)) _getwch(void);
  __attribute__ ((__dllimport__)) wint_t __attribute__((__cdecl__)) _getwche(void);
  __attribute__ ((__dllimport__)) wint_t __attribute__((__cdecl__)) _putwch(wchar_t _WCh);
  __attribute__ ((__dllimport__)) wint_t __attribute__((__cdecl__)) _ungetwch(wint_t _WCh);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _cputws(const wchar_t *_String);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _cwprintf(const wchar_t * __restrict__ _Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _cwscanf(const wchar_t * __restrict__ _Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _cwscanf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vcwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _cwprintf_p(const wchar_t * __restrict__ _Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vcwprintf_p(const wchar_t * __restrict__ _Format,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _cwprintf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vcwprintf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _cwprintf_p_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vcwprintf_p_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
                                                     __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_swscanf(const wchar_t * __restrict__ _Src,const wchar_t * __restrict__ _Format,...);
                                                     __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_vswscanf (const wchar_t * __restrict__ _Str,const wchar_t * __restrict__ Format,va_list argp);
                                                     __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __mingw_wscanf(const wchar_t * __restrict__ _Format,...);
                                                     __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __mingw_vwscanf(const wchar_t * __restrict__ Format, va_list argp);
                                                     __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_fwscanf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
                                                     __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_vfwscanf (FILE * __restrict__ fp, const wchar_t * __restrict__ Format,va_list argp);
                                                      __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_fwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
                                                      __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __mingw_wprintf(const wchar_t * __restrict__ _Format,...);
                                                      __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_vfwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,va_list _ArgList);
                                                     __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __mingw_vwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
                                                      __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) __mingw_snwprintf (wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, ...);
                                                      __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) __mingw_vsnwprintf (wchar_t * __restrict__ , size_t, const wchar_t * __restrict__ , va_list);
                                                      __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_swprintf(wchar_t * __restrict__ , const wchar_t * __restrict__ , ...);
                                                      __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_vswprintf(wchar_t * __restrict__ , const wchar_t * __restrict__ ,va_list);
                                                    __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_swscanf(const wchar_t * __restrict__ _Src,const wchar_t * __restrict__ _Format,...);
                                                    __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __ms_wscanf(const wchar_t * __restrict__ _Format,...);
                                                    __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_fwscanf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
                                                     __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_fwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
                                                     __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __ms_wprintf(const wchar_t * __restrict__ _Format,...);
                                                    __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_vfwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,va_list _ArgList);
                                                    __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __ms_vwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
                                                     __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_swprintf(wchar_t * __restrict__ , const wchar_t * __restrict__ , ...);
                                                     __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_vswprintf(wchar_t * __restrict__ , const wchar_t * __restrict__ ,va_list);
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
                                                     __attribute__ ((__nonnull__ (2)))
int swscanf(const wchar_t *__source, const wchar_t *__format, ...)
{
  int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vswscanf( __source, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
                                                     __attribute__ ((__nonnull__ (1)))
int wscanf(const wchar_t *__format, ...)
{
  int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfwscanf( (__acrt_iob_func(0)), __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
                                                     __attribute__ ((__nonnull__ (2)))
int fwscanf(FILE *__stream, const wchar_t *__format, ...)
{
  int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfwscanf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
                                                     __attribute__ ((__nonnull__ (2)))
int vswscanf (const wchar_t *__source, const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vswscanf( __source, __format, __local_argv );
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
                                                     __attribute__ ((__nonnull__ (1)))
int vwscanf(const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfwscanf( (__acrt_iob_func(0)), __format, __local_argv );
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
                                                     __attribute__ ((__nonnull__ (2)))
int vfwscanf (FILE *__stream, const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfwscanf( __stream, __format, __local_argv );
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
                                                      __attribute__ ((__nonnull__ (2)))
int fwprintf (FILE *__stream, const wchar_t *__format, ...)
{
  int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfwprintf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
                                                      __attribute__ ((__nonnull__ (1)))
int wprintf (const wchar_t *__format, ...)
{
  int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfwprintf( (__acrt_iob_func(1)), __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
                                                      __attribute__ ((__nonnull__ (2)))
int vfwprintf (FILE *__stream, const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfwprintf( __stream, __format, __local_argv );
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
                                                      __attribute__ ((__nonnull__ (1)))
int vwprintf (const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfwprintf( (__acrt_iob_func(1)), __format, __local_argv );
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
                                                      __attribute__ ((__nonnull__ (3)))
int snwprintf (wchar_t *__stream, size_t __n, const wchar_t *__format, ...)
{
  int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vsnwprintf( __stream, __n, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
                                                      __attribute__ ((__nonnull__ (3)))
int vsnwprintf (wchar_t *__stream, size_t __n, const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vsnwprintf( __stream, __n, __format, __local_argv );
}
  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) _wfsopen(const wchar_t *_Filename,const wchar_t *_Mode,int _ShFlag);
  wint_t __attribute__((__cdecl__)) fgetwc(FILE *_File);
  __attribute__ ((__dllimport__)) wint_t __attribute__((__cdecl__)) _fgetwchar(void);
  wint_t __attribute__((__cdecl__)) fputwc(wchar_t _Ch,FILE *_File);
  __attribute__ ((__dllimport__)) wint_t __attribute__((__cdecl__)) _fputwchar(wchar_t _Ch);
  wint_t __attribute__((__cdecl__)) getwc(FILE *_File);
  wint_t __attribute__((__cdecl__)) getwchar(void);
  wint_t __attribute__((__cdecl__)) putwc(wchar_t _Ch,FILE *_File);
  wint_t __attribute__((__cdecl__)) putwchar(wchar_t _Ch);
  wint_t __attribute__((__cdecl__)) ungetwc(wint_t _Ch,FILE *_File);
  wchar_t *__attribute__((__cdecl__)) fgetws(wchar_t * __restrict__ _Dst,int _SizeInWords,FILE * __restrict__ _File);
  int __attribute__((__cdecl__)) fputws(const wchar_t * __restrict__ _Str,FILE * __restrict__ _File);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _getws(wchar_t *_String) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _putws(const wchar_t *_Str);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _scwprintf(const wchar_t * __restrict__ _Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _swprintf_l(wchar_t * __restrict__ ,size_t _SizeInWords,const wchar_t * __restrict__ _Format,_locale_t _Locale,... ) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _swprintf_c(wchar_t * __restrict__ _DstBuf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vswprintf_c(wchar_t * __restrict__ _DstBuf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snwprintf(wchar_t * __restrict__ _Dest,size_t _Count,const wchar_t * __restrict__ _Format,...) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vsnwprintf(wchar_t * __restrict__ _Dest,size_t _Count,const wchar_t * __restrict__ _Format,va_list _Args) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fwprintf_p(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wprintf_p(const wchar_t * __restrict__ _Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vfwprintf_p(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vwprintf_p(const wchar_t * __restrict__ _Format,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _swprintf_p(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,...);
  __attribute__((dllimport)) int __attribute__((__cdecl__)) _vswprintf_p(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _scwprintf_p(const wchar_t * __restrict__ _Format,...);
  __attribute__((dllimport)) int __attribute__((__cdecl__)) _vscwprintf_p(const wchar_t * __restrict__ _Format,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wprintf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wprintf_p_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vwprintf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vwprintf_p_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fwprintf_l(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fwprintf_p_l(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vfwprintf_l(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vfwprintf_p_l(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _swprintf_c_l(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _swprintf_p_l(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vswprintf_c_l(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vswprintf_p_l(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _scwprintf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _scwprintf_p_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vscwprintf_p_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snwprintf_l(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vsnwprintf_l(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _swprintf(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vswprintf(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,va_list _Args);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) __swprintf_l(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,_locale_t _Plocinfo,...) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vswprintf_l(wchar_t * __restrict__ _Dest,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) __vswprintf_l(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,_locale_t _Plocinfo,va_list _Args) ;
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
                                                      __attribute__ ((__nonnull__ (3)))
int vswprintf (wchar_t *__stream, size_t __count, const wchar_t *__format, __builtin_va_list __local_argv)
{
  return vsnwprintf( __stream, __count, __format, __local_argv );
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
                                                      __attribute__ ((__nonnull__ (3)))
int swprintf (wchar_t *__stream, size_t __count, const wchar_t *__format, ...)
{
  int __retval;
  __builtin_va_list __local_argv;
  __builtin_va_start( __local_argv, __format );
  __retval = vswprintf( __stream, __count, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wtempnam(const wchar_t *_Directory,const wchar_t *_FilePrefix);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vscwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vscwprintf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fwscanf_l(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,_locale_t _Locale,...) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _swscanf_l(const wchar_t * __restrict__ _Src,const wchar_t * __restrict__ _Format,_locale_t _Locale,...) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snwscanf(const wchar_t * __restrict__ _Src,size_t _MaxCount,const wchar_t * __restrict__ _Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snwscanf_l(const wchar_t * __restrict__ _Src,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wscanf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...) ;
  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) _wfdopen(int _FileHandle ,const wchar_t *_Mode);
  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) _wfopen(const wchar_t * __restrict__ _Filename,const wchar_t * __restrict__ _Mode) ;
  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) _wfreopen(const wchar_t * __restrict__ _Filename,const wchar_t * __restrict__ _Mode,FILE * __restrict__ _OldFile) ;
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _wperror(const wchar_t *_ErrMsg);
  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) _wpopen(const wchar_t *_Command,const wchar_t *_Mode);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wremove(const wchar_t *_Filename);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wtmpnam(wchar_t *_Buffer);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _itow(int _Value,wchar_t *_Dest,int _Radix) ;
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _ltow(long _Value,wchar_t *_Dest,int _Radix) ;
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _ultow(unsigned long _Value,wchar_t *_Dest,int _Radix) ;
  __attribute__ ((__dllimport__)) double __attribute__((__cdecl__)) _wcstod_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,_locale_t _Locale);
  double __attribute__((__cdecl__)) __mingw_wcstod(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr);
  float __attribute__((__cdecl__)) __mingw_wcstof(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr);
  long double __attribute__((__cdecl__)) __mingw_wcstold(const wchar_t * __restrict__, wchar_t ** __restrict__);
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
  double __attribute__((__cdecl__)) wcstod(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr){
    return __mingw_wcstod(_Str,_EndPtr);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
  float __attribute__((__cdecl__)) wcstof(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr){
    return __mingw_wcstof(_Str,_EndPtr);
  }
  long double __attribute__((__cdecl__)) wcstold (const wchar_t * __restrict__, wchar_t ** __restrict__);
  long __attribute__((__cdecl__)) wcstol(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix);
  __attribute__ ((__dllimport__)) long __attribute__((__cdecl__)) _wcstol_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
  unsigned long __attribute__((__cdecl__)) wcstoul(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix);
  __attribute__ ((__dllimport__)) unsigned long __attribute__((__cdecl__)) _wcstoul_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wgetenv(const wchar_t *_VarName) ;
  __attribute__ ((__dllimport__)) double __attribute__((__cdecl__)) _wtof(const wchar_t *_Str);
  __attribute__ ((__dllimport__)) double __attribute__((__cdecl__)) _wtof_l(const wchar_t *_Str,_locale_t _Locale);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wtoi(const wchar_t *_Str);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wtoi_l(const wchar_t *_Str,_locale_t _Locale);
  __attribute__ ((__dllimport__)) long __attribute__((__cdecl__)) _wtol(const wchar_t *_Str);
  __attribute__ ((__dllimport__)) long __attribute__((__cdecl__)) _wtol_l(const wchar_t *_Str,_locale_t _Locale);
  __extension__ __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _i64tow(long long _Val,wchar_t *_DstBuf,int _Radix) ;
  __extension__ __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _ui64tow(unsigned long long _Val,wchar_t *_DstBuf,int _Radix);
  __extension__ __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _wtoi64(const wchar_t *_Str);
  __extension__ __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _wtoi64_l(const wchar_t *_Str,_locale_t _Locale);
  __extension__ __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _wcstoi64(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix);
  __extension__ __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _wcstoi64_l(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix,_locale_t _Locale);
  __extension__ __attribute__ ((__dllimport__)) unsigned long long __attribute__((__cdecl__)) _wcstoui64(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix);
  __extension__ __attribute__ ((__dllimport__)) unsigned long long __attribute__((__cdecl__)) _wcstoui64_l(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix,_locale_t _Locale);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wfullpath(wchar_t *_FullPath,const wchar_t *_Path,size_t _SizeInWords);
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _wmakepath(wchar_t *_ResultPath,const wchar_t *_Drive,const wchar_t *_Dir,const wchar_t *_Filename,const wchar_t *_Ext);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wputenv(const wchar_t *_EnvString);
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _wsearchenv(const wchar_t *_Filename,const wchar_t *_EnvVar,wchar_t *_ResultPath) ;
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _wsplitpath(const wchar_t *_FullPath,wchar_t *_Drive,wchar_t *_Dir,wchar_t *_Filename,wchar_t *_Ext) ;
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wcsdup(const wchar_t *_Str);
  wchar_t *__attribute__((__cdecl__)) wcscat(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source) ;
  wchar_t *__attribute__((__cdecl__)) wcschr(const wchar_t *_Str,wchar_t _Ch);
  int __attribute__((__cdecl__)) wcscmp(const wchar_t *_Str1,const wchar_t *_Str2);
  wchar_t *__attribute__((__cdecl__)) wcscpy(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source) ;
  size_t __attribute__((__cdecl__)) wcscspn(const wchar_t *_Str,const wchar_t *_Control);
  size_t __attribute__((__cdecl__)) wcslen(const wchar_t *_Str);
  size_t __attribute__((__cdecl__)) wcsnlen(const wchar_t *_Src,size_t _MaxCount);
  wchar_t *__attribute__((__cdecl__)) wcsncat(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _Count) ;
  int __attribute__((__cdecl__)) wcsncmp(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  wchar_t *__attribute__((__cdecl__)) wcsncpy(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _Count) ;
  wchar_t *__attribute__((__cdecl__)) _wcsncpy_l(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _Count,_locale_t _Locale) ;
  wchar_t *__attribute__((__cdecl__)) wcspbrk(const wchar_t *_Str,const wchar_t *_Control);
  wchar_t *__attribute__((__cdecl__)) wcsrchr(const wchar_t *_Str,wchar_t _Ch);
  size_t __attribute__((__cdecl__)) wcsspn(const wchar_t *_Str,const wchar_t *_Control);
  wchar_t *__attribute__((__cdecl__)) wcsstr(const wchar_t *_Str,const wchar_t *_SubStr);
  wchar_t *__attribute__((__cdecl__)) wcstok(wchar_t * __restrict__ _Str,const wchar_t * __restrict__ _Delim) ;
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wcserror(int _ErrNum) ;
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) __wcserror(const wchar_t *_Str) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wcsicmp(const wchar_t *_Str1,const wchar_t *_Str2);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wcsicmp_l(const wchar_t *_Str1,const wchar_t *_Str2,_locale_t _Locale);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wcsnicmp(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wcsnicmp_l(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount,_locale_t _Locale);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wcsnset(wchar_t *_Str,wchar_t _Val,size_t _MaxCount) ;
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wcsrev(wchar_t *_Str);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wcsset(wchar_t *_Str,wchar_t _Val) ;
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wcslwr(wchar_t *_String) ;
  __attribute__ ((__dllimport__)) wchar_t *_wcslwr_l(wchar_t *_String,_locale_t _Locale) ;
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wcsupr(wchar_t *_String) ;
  __attribute__ ((__dllimport__)) wchar_t *_wcsupr_l(wchar_t *_String,_locale_t _Locale) ;
  size_t __attribute__((__cdecl__)) wcsxfrm(wchar_t * __restrict__ _Dst,const wchar_t * __restrict__ _Src,size_t _MaxCount);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _wcsxfrm_l(wchar_t * __restrict__ _Dst,const wchar_t * __restrict__ _Src,size_t _MaxCount,_locale_t _Locale);
  int __attribute__((__cdecl__)) wcscoll(const wchar_t *_Str1,const wchar_t *_Str2);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wcscoll_l(const wchar_t *_Str1,const wchar_t *_Str2,_locale_t _Locale);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wcsicoll(const wchar_t *_Str1,const wchar_t *_Str2);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wcsicoll_l(const wchar_t *_Str1,const wchar_t *_Str2,_locale_t _Locale);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wcsncoll(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wcsncoll_l(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount,_locale_t _Locale);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wcsnicoll(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wcsnicoll_l(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount,_locale_t _Locale);
  wchar_t *__attribute__((__cdecl__)) wcsdup(const wchar_t *_Str) ;
  int __attribute__((__cdecl__)) wcsicmp(const wchar_t *_Str1,const wchar_t *_Str2) ;
  int __attribute__((__cdecl__)) wcsnicmp(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount) ;
  wchar_t *__attribute__((__cdecl__)) wcsnset(wchar_t *_Str,wchar_t _Val,size_t _MaxCount) ;
  wchar_t *__attribute__((__cdecl__)) wcsrev(wchar_t *_Str) ;
  wchar_t *__attribute__((__cdecl__)) wcsset(wchar_t *_Str,wchar_t _Val) ;
  wchar_t *__attribute__((__cdecl__)) wcslwr(wchar_t *_Str) ;
  wchar_t *__attribute__((__cdecl__)) wcsupr(wchar_t *_Str) ;
  int __attribute__((__cdecl__)) wcsicoll(const wchar_t *_Str1,const wchar_t *_Str2) ;
  struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
  };
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wasctime(const struct tm *_Tm);
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _wasctime_s (wchar_t *_Buf,size_t _SizeInWords,const struct tm *_Tm);
  wchar_t *__attribute__((__cdecl__)) _wctime32(const __time32_t *_Time) ;
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _wctime32_s (wchar_t *_Buf,size_t _SizeInWords,const __time32_t *_Time);
  size_t __attribute__((__cdecl__)) wcsftime(wchar_t * __restrict__ _Buf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,const struct tm * __restrict__ _Tm);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _wcsftime_l(wchar_t * __restrict__ _Buf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,const struct tm * __restrict__ _Tm,_locale_t _Locale);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wstrdate(wchar_t *_Buffer) ;
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _wstrdate_s (wchar_t *_Buf,size_t _SizeInWords);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wstrtime(wchar_t *_Buffer) ;
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _wstrtime_s (wchar_t *_Buf,size_t _SizeInWords);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wctime64(const __time64_t *_Time) ;
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _wctime64_s (wchar_t *_Buf,size_t _SizeInWords,const __time64_t *_Time);
  wchar_t *__attribute__((__cdecl__)) _wctime(const time_t *_Time) ;
  errno_t __attribute__((__cdecl__)) _wctime_s(wchar_t *, size_t, const time_t *);
  typedef int mbstate_t;
  typedef wchar_t _Wint_t;
  wint_t __attribute__((__cdecl__)) btowc(int);
  size_t __attribute__((__cdecl__)) mbrlen(const char * __restrict__ _Ch,size_t _SizeInBytes,mbstate_t * __restrict__ _State);
  size_t __attribute__((__cdecl__)) mbrtowc(wchar_t * __restrict__ _DstCh,const char * __restrict__ _SrcCh,size_t _SizeInBytes,mbstate_t * __restrict__ _State);
  size_t __attribute__((__cdecl__)) mbsrtowcs(wchar_t * __restrict__ _Dest,const char ** __restrict__ _PSrc,size_t _Count,mbstate_t * __restrict__ _State) ;
  size_t __attribute__((__cdecl__)) wcrtomb(char * __restrict__ _Dest,wchar_t _Source,mbstate_t * __restrict__ _State) ;
  size_t __attribute__((__cdecl__)) wcsrtombs(char * __restrict__ _Dest,const wchar_t ** __restrict__ _PSource,size_t _Count,mbstate_t * __restrict__ _State) ;
  int __attribute__((__cdecl__)) wctob(wint_t _WCh);
  wchar_t *__attribute__((__cdecl__)) wmemset(wchar_t *s, wchar_t c, size_t n);
  wchar_t *__attribute__((__cdecl__)) wmemchr(const wchar_t *s, wchar_t c, size_t n);
  int __attribute__((__cdecl__)) wmemcmp(const wchar_t *s1, const wchar_t *s2,size_t n);
  wchar_t *__attribute__((__cdecl__)) wmemcpy(wchar_t * __restrict__ s1,const wchar_t * __restrict__ s2,size_t n) ;
  wchar_t * __attribute__((__cdecl__)) wmempcpy (wchar_t *_Dst, const wchar_t *_Src, size_t _Size);
  wchar_t *__attribute__((__cdecl__)) wmemmove(wchar_t *s1, const wchar_t *s2, size_t n) ;
  int __attribute__((__cdecl__)) fwide(FILE *stream,int mode);
  int __attribute__((__cdecl__)) mbsinit(const mbstate_t *ps);
  __extension__ long long __attribute__((__cdecl__)) wcstoll(const wchar_t * __restrict__ nptr,wchar_t ** __restrict__ endptr, int base);
  __extension__ unsigned long long __attribute__((__cdecl__)) wcstoull(const wchar_t * __restrict__ nptr,wchar_t ** __restrict__ endptr, int base);
  void *__attribute__((__cdecl__)) memmove(void *_Dst,const void *_Src,size_t _MaxCount);
  void *__attribute__((__cdecl__)) memcpy(void * __restrict__ _Dst,const void * __restrict__ _Src,size_t _MaxCount) ;
int __attribute__((__cdecl__)) __mingw_str_wide_utf8 (const wchar_t * const wptr, char **mbptr, size_t * buflen);
int __attribute__((__cdecl__)) __mingw_str_utf8_wide (const char *const mbptr, wchar_t ** wptr, size_t * buflen);
void __attribute__((__cdecl__)) __mingw_str_free(void *ptr);
#pragma pack(pop)
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _waccess_s (const wchar_t *_Filename,int _AccessMode);
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _wmktemp_s (wchar_t *_TemplateName,size_t _SizeInWords);
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _cgetws_s (wchar_t *_Buffer,size_t _SizeInWords,size_t *_SizeRead);
  __attribute__((dllimport)) int __attribute__((__cdecl__)) _cwprintf_s (const wchar_t *_Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _cwscanf_s(const wchar_t *_Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _cwscanf_s_l(const wchar_t *_Format,_locale_t _Locale,...);
  __attribute__((dllimport)) int __attribute__((__cdecl__)) _vcwprintf_s (const wchar_t *_Format,va_list _ArgList);
  __attribute__((dllimport)) int __attribute__((__cdecl__)) _cwprintf_s_l (const wchar_t *_Format,_locale_t _Locale,...);
  __attribute__((dllimport)) int __attribute__((__cdecl__)) _vcwprintf_s_l (const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _getws_s(wchar_t *_Str,size_t _SizeInWords);
 
  int __attribute__((__cdecl__)) fwprintf_s(FILE *_File,const wchar_t *_Format,...);
  int __attribute__((__cdecl__)) wprintf_s(const wchar_t *_Format,...);
  int __attribute__((__cdecl__)) vfwprintf_s(FILE *_File,const wchar_t *_Format,va_list _ArgList);
  int __attribute__((__cdecl__)) vwprintf_s(const wchar_t *_Format,va_list _ArgList);
  int __attribute__((__cdecl__)) vswprintf_s(wchar_t *_Dst,size_t _SizeInWords,const wchar_t *_Format,va_list _ArgList);
  int __attribute__((__cdecl__)) swprintf_s(wchar_t *_Dst,size_t _SizeInWords,const wchar_t *_Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vsnwprintf_s(wchar_t *_DstBuf,size_t _DstSizeInWords,size_t _MaxCount,const wchar_t *_Format,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snwprintf_s(wchar_t *_DstBuf,size_t _DstSizeInWords,size_t _MaxCount,const wchar_t *_Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wprintf_s_l(const wchar_t *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vwprintf_s_l(const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fwprintf_s_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vfwprintf_s_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _swprintf_s_l(wchar_t *_DstBuf,size_t _DstSize,const wchar_t *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vswprintf_s_l(wchar_t *_DstBuf,size_t _DstSize,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snwprintf_s_l(wchar_t *_DstBuf,size_t _DstSize,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vsnwprintf_s_l(wchar_t *_DstBuf,size_t _DstSize,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fwscanf_s_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) fwscanf_s(FILE *_File, const wchar_t *_Format, ...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _swscanf_s_l(const wchar_t *_Src,const wchar_t *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) swscanf_s(const wchar_t *_Src,const wchar_t *_Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snwscanf_s(const wchar_t *_Src,size_t _MaxCount,const wchar_t *_Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snwscanf_s_l(const wchar_t *_Src,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wscanf_s_l(const wchar_t *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) wscanf_s(const wchar_t *_Format, ...);
 
 
 
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wfopen_s(FILE **_File,const wchar_t *_Filename,const wchar_t *_Mode);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wfreopen_s(FILE **_File,const wchar_t *_Filename,const wchar_t *_Mode,FILE *_OldFile);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wtmpnam_s(wchar_t *_DstBuf,size_t _SizeInWords);
 
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) wcstok_s(wchar_t *_Str,const wchar_t *_Delim,wchar_t **_Context);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wcserror_s(wchar_t *_Buf,size_t _SizeInWords,int _ErrNum);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) __wcserror_s(wchar_t *_Buffer,size_t _SizeInWords,const wchar_t *_ErrMsg);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wcsnset_s(wchar_t *_Dst,size_t _DstSizeInWords,wchar_t _Val,size_t _MaxCount);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wcsset_s(wchar_t *_Str,size_t _SizeInWords,wchar_t _Val);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wcslwr_s(wchar_t *_Str,size_t _SizeInWords);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wcslwr_s_l(wchar_t *_Str,size_t _SizeInWords,_locale_t _Locale);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wcsupr_s(wchar_t *_Str,size_t _Size);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wcsupr_s_l(wchar_t *_Str,size_t _Size,_locale_t _Locale);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) wcscat_s(wchar_t *_Dst, rsize_t _DstSize, const wchar_t *_Src);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) wcscpy_s(wchar_t *_Dst, rsize_t _DstSize, const wchar_t *_Src);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) wcsncat_s(wchar_t *_Dst,size_t _DstSizeInChars,const wchar_t *_Src,size_t _MaxCount);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wcsncat_s_l(wchar_t *_Dst,size_t _DstSizeInChars,const wchar_t *_Src,size_t _MaxCount,_locale_t _Locale);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) wcsncpy_s(wchar_t *_Dst,size_t _DstSizeInChars,const wchar_t *_Src,size_t _MaxCount);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wcsncpy_s_l(wchar_t *_Dst,size_t _DstSizeInChars,const wchar_t *_Src,size_t _MaxCount,_locale_t _Locale);
 
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wcstok_s_l(wchar_t *_Str,const wchar_t *_Delim,wchar_t **_Context,_locale_t _Locale);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wcsset_s_l(wchar_t *_Str,size_t _SizeInChars,wchar_t _Val,_locale_t _Locale);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wcsnset_s_l(wchar_t *_Str,size_t _SizeInChars,wchar_t _Val, size_t _Count,_locale_t _Locale);
 
  extern __inline__ __attribute__((__always_inline__,__gnu_inline__)) size_t __attribute__((__cdecl__)) wcsnlen_s(const wchar_t * _src, size_t _count) {
    return _src ? wcsnlen(_src, _count) : 0;
  }
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _wasctime_s (wchar_t *_Buf,size_t _SizeInWords,const struct tm *_Tm);
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _wctime32_s (wchar_t *_Buf,size_t _SizeInWords,const __time32_t *_Time);
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _wstrdate_s (wchar_t *_Buf,size_t _SizeInWords);
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _wstrtime_s (wchar_t *_Buf,size_t _SizeInWords);
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _wctime64_s (wchar_t *_Buf,size_t _SizeInWords,const __time64_t *_Time);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) mbsrtowcs_s(size_t *_Retval,wchar_t *_Dst,size_t _SizeInWords,const char **_PSrc,size_t _N,mbstate_t *_State);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) wcrtomb_s(size_t *_Retval,char *_Dst,size_t _SizeInBytes,wchar_t _Ch,mbstate_t *_State);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) wcsrtombs_s(size_t *_Retval,char *_Dst,size_t _SizeInBytes,const wchar_t **_Src,size_t _Size,mbstate_t *_State);
 
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) wmemcpy_s (wchar_t *_dest,size_t _numberOfElements,const wchar_t *_src,size_t _count);
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) wmemmove_s(wchar_t *_dest,size_t _numberOfElements,const wchar_t *_src,size_t _count);
extern int
tre_regwcomp(regex_t *preg, const wchar_t *regex, int cflags);
extern int
tre_regwexec(const regex_t *preg, const wchar_t *string,
  size_t nmatch, regmatch_t pmatch[], int eflags);
extern int
tre_regncomp(regex_t *preg, const char *regex, size_t len, int cflags);
extern int
tre_regnexec(const regex_t *preg, const char *string, size_t len,
  size_t nmatch, regmatch_t pmatch[], int eflags);
extern int
tre_regncompb(regex_t *preg, const char *regex, size_t n, int cflags);
extern int
tre_regnexecb(const regex_t *preg, const char *str, size_t len,
   size_t nmatch, regmatch_t pmatch[], int eflags);
extern int
tre_regwncomp(regex_t *preg, const wchar_t *regex, size_t len, int cflags);
extern int
tre_regwnexec(const regex_t *preg, const wchar_t *string, size_t len,
   size_t nmatch, regmatch_t pmatch[], int eflags);
typedef struct {
  int cost_ins;
  int cost_del;
  int cost_subst;
  int max_cost;
  int max_ins;
  int max_del;
  int max_subst;
  int max_err;
} regaparams_t;
typedef struct {
  size_t nmatch;
  regmatch_t *pmatch;
  int cost;
  int num_ins;
  int num_del;
  int num_subst;
} regamatch_t;
extern int
tre_regaexec(const regex_t *preg, const char *string,
  regamatch_t *match, regaparams_t params, int eflags);
extern int
tre_reganexec(const regex_t *preg, const char *string, size_t len,
   regamatch_t *match, regaparams_t params, int eflags);
extern int
tre_regaexecb(const regex_t *preg, const char *string,
   regamatch_t *match, regaparams_t params, int eflags);
extern int
tre_regawexec(const regex_t *preg, const wchar_t *string,
   regamatch_t *match, regaparams_t params, int eflags);
extern int
tre_regawnexec(const regex_t *preg, const wchar_t *string, size_t len,
    regamatch_t *match, regaparams_t params, int eflags);
extern void
tre_regaparams_default(regaparams_t *params);
typedef wchar_t tre_char_t;
typedef struct {
  int (*get_next_char)(tre_char_t *c, unsigned int *pos_add, void *context);
  void (*rewind)(size_t pos, void *context);
  int (*compare)(size_t pos1, size_t pos2, size_t len, void *context);
  void *context;
} tre_str_source;
extern int
tre_reguexec(const regex_t *preg, const tre_str_source *string,
  size_t nmatch, regmatch_t pmatch[], int eflags);
extern char *
tre_version(void);
extern int
tre_config(int query, void *result);
enum {
  TRE_CONFIG_APPROX,
  TRE_CONFIG_WCHAR,
  TRE_CONFIG_MULTIBYTE,
  TRE_CONFIG_SYSTEM_ABI,
  TRE_CONFIG_VERSION
};
extern int
tre_have_backrefs(const regex_t *preg);
extern int
tre_have_approx(const regex_t *preg);
int regcomp(regex_t *preg, const char *pattern,
       int cflags);
size_t regerror(int errcode, const regex_t *preg,
       char *errbuf, size_t errbuf_size);
int regexec(const regex_t *preg, const char *string,
       size_t nmatch, regmatch_t pmatch[], int eflags);
void regfree(regex_t *preg);
typedef struct Type Type;
struct Type {
    Type const * _class;
    char const * type_name;
    Type * (*new)(char const *);
    err_type (*init)(Type *, char const *);
    void (*del)(Type *);
};
Type * Type_new(char const * type_name);
err_type Type_init(Type * self, char const * type_name);
void Type_del(Type * self);
extern Type Type_class;
_Bool isinstance(Type const * type, unsigned short ntypes, Type const * types[ntypes]);
typedef struct ASTNode ASTNode, * pASTNode;
struct ASTNode {
    struct ASTNodeType * _class;
    ASTNode ** children;
    Rule * rule;
    size_t str_length;
    size_t nchildren;
    size_t token_key;
    size_t ntokens;
};
extern ASTNode ASTNode_fail;
extern ASTNode ASTNode_lookahead;
typedef struct ASTNodeIterator ASTNodeIterator;
struct ASTNodeIterator {
    struct ASTNodeIteratorType * _class;
    ASTNode * parent_node;
    size_t start;
    size_t stop;
    ptrdiff_t step;
    size_t cur;
};
extern struct ASTNodeType {
    Type const * _class;
    ASTNode * (*new)(Rule * rule, size_t token_key, size_t ntokens, size_t str_length, size_t nchildren, ASTNode * children[nchildren]);
    err_type (*init)(ASTNode * self, Rule * rule, size_t token_key, size_t ntokens, size_t str_length, size_t nchildren, ASTNode * children[nchildren]);
    void (*dest)(ASTNode * self);
    void (*del)(ASTNode * self);
    err_type (*iter)(ASTNode * self, ASTNodeIterator * node_iter);
    err_type (*reverse)(ASTNode * self, ASTNodeIterator * node_iter);
    err_type (*get)(ASTNode * self, size_t key, ASTNode ** value);
    size_t (*len)(ASTNode * self);
    int (*str)(ASTNode * self, char * buffer, size_t buf_size);
} ASTNode_class;
ASTNode * ASTNode_new(Rule * rule, size_t token_key, size_t ntokens, size_t str_length, size_t nchildren, ASTNode * children[nchildren]);
err_type ASTNode_init(ASTNode * self, Rule * rule, size_t token_key, size_t ntokens, size_t str_length, size_t nchildren, ASTNode * children[nchildren]);
void ASTNode_dest(ASTNode * self);
void ASTNode_del(ASTNode * self);
err_type ASTNode_iter(ASTNode * self, ASTNodeIterator * node_iter);
err_type ASTNode_reverse(ASTNode * self, ASTNodeIterator * node_iter);
err_type ASTNode_get(ASTNode * self, size_t key, ASTNode ** value);
size_t ASTNode_len(ASTNode * self);
int ASTNode_str(ASTNode * self, char * buffer, size_t buf_size);
extern struct ASTNodeIteratorType {
    Type const * _class;
    ASTNodeIterator * (*new)(ASTNode * parent_node, size_t start, size_t stop, ptrdiff_t step);
    err_type (*init)(ASTNodeIterator * self, ASTNode * parent_node, size_t start, size_t stop, ptrdiff_t step);
    void (*del)(ASTNodeIterator * self);
    iter_status (*next)(ASTNodeIterator * self, ASTNode ** val);
    err_type (*iter)(ASTNodeIterator * self, ASTNodeIterator * node_iter);
    err_type (*reverse)(ASTNodeIterator * self, ASTNodeIterator * node_iter);
} ASTNodeIterator_class;
ASTNodeIterator * ASTNodeIterator_new(ASTNode * parent_node, size_t start, size_t stop, ptrdiff_t step);
err_type ASTNodeIterator_init(ASTNodeIterator * self, ASTNode * parent_node, size_t start, size_t stop, ptrdiff_t step);
void ASTNodeIterator_del(ASTNodeIterator * self);
iter_status ASTNodeIterator_next(ASTNodeIterator * self, ASTNode ** val);
err_type ASTNodeIterator_iter(ASTNodeIterator * self, ASTNodeIterator * other);
err_type ASTNodeIterator_reverse(ASTNodeIterator * self, ASTNodeIterator * other);
ASTNode * make_skip_node(ASTNode * node);
_Bool is_skip_node(ASTNode * node);
typedef struct Token Token, * pToken;
typedef struct TokenCoords {
    unsigned int line;
    unsigned int col;
} TokenCoords;
struct Token {
    struct TokenCoords coords;
    struct TokenType * _class;
    char const * const string;
    size_t const start;
    size_t end;
};
extern struct TokenType {
    Type const * _class;
    Token * (*new)(char const * string, size_t start, size_t end, unsigned int line, unsigned int col);
    err_type (*init)(Token * self, char const * string, size_t start, size_t end, unsigned int line, unsigned int col);
    void (*del)(Token * self);
    size_t (*len)(Token * self);
    struct TokenCoords (*coords)(Token * self);
    int (*str)(Token * self, char * buffer, size_t buf_size);
    err_type (*get)(Token * self, size_t key, char * chr);
    _Bool (*equal)(Token * self, Token * other);
    _Bool (*equal_value)(Token * self, Token * other);
} Token_class;
Token * Token_new(char const * string, size_t start, size_t end, unsigned int line, unsigned int col);
err_type Token_init(Token * self, char const * string, size_t start, size_t end, unsigned int line, unsigned int col);
void Token_del(Token * self);
size_t Token_len(Token * self);
struct TokenCoords Token_coords(Token * self);
int Token_str(Token * self, char * buffer, size_t buf_size);
err_type Token_get(Token * self, size_t key, char * chr);
_Bool Token_equal(Token * self, Token * other);
_Bool Token_equal_value(Token * self, Token * other);
void Token_print(Token * self);
#pragma pack(push,_CRT_PACKING)
  typedef int (__attribute__((__cdecl__)) *_onexit_t)(void);
  typedef struct _div_t {
    int quot;
    int rem;
  } div_t;
  typedef struct _ldiv_t {
    long quot;
    long rem;
  } ldiv_t;
#pragma pack(4)
  typedef struct {
    unsigned char ld[10];
  } _LDOUBLE;
#pragma pack()
  typedef struct {
    double x;
  } _CRT_DOUBLE;
  typedef struct {
    float f;
  } _CRT_FLOAT;
       
  typedef struct {
    long double x;
  } _LONGDOUBLE;
       
#pragma pack(4)
  typedef struct {
    unsigned char ld12[12];
  } _LDBL12;
#pragma pack()
  extern int * __imp___mb_cur_max;
__attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) ___mb_cur_max_func(void);
  typedef void (__attribute__((__cdecl__)) *_purecall_handler)(void);
  __attribute__ ((__dllimport__)) _purecall_handler __attribute__((__cdecl__)) _set_purecall_handler(_purecall_handler _Handler);
  __attribute__ ((__dllimport__)) _purecall_handler __attribute__((__cdecl__)) _get_purecall_handler(void);
  typedef void (__attribute__((__cdecl__)) *_invalid_parameter_handler)(const wchar_t *,const wchar_t *,const wchar_t *,unsigned int,uintptr_t);
  __attribute__ ((__dllimport__)) _invalid_parameter_handler __attribute__((__cdecl__)) _set_invalid_parameter_handler(_invalid_parameter_handler _Handler);
  __attribute__ ((__dllimport__)) _invalid_parameter_handler __attribute__((__cdecl__)) _get_invalid_parameter_handler(void);
  __attribute__ ((__dllimport__)) unsigned long *__attribute__((__cdecl__)) __doserrno(void);
  errno_t __attribute__((__cdecl__)) _set_doserrno(unsigned long _Value);
  errno_t __attribute__((__cdecl__)) _get_doserrno(unsigned long *_Value);
  extern __attribute__((dllimport)) char *_sys_errlist[1];
  extern __attribute__((dllimport)) int _sys_nerr;
  __attribute__ ((__dllimport__)) char ***__attribute__((__cdecl__)) __p___argv(void);
  __attribute__ ((__dllimport__)) int *__attribute__((__cdecl__)) __p__fmode(void);
  errno_t __attribute__((__cdecl__)) _get_pgmptr(char **_Value);
  errno_t __attribute__((__cdecl__)) _get_wpgmptr(wchar_t **_Value);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _set_fmode(int _Mode);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _get_fmode(int *_PMode);
  extern int * __imp___argc;
  extern char *** __imp___argv;
  extern wchar_t *** __imp___wargv;
  extern char *** __imp__environ;
  extern wchar_t *** __imp__wenviron;
  extern char ** __imp__pgmptr;
  extern wchar_t ** __imp__wpgmptr;
  extern unsigned int * __imp__osplatform;
  extern unsigned int * __imp__osver;
  extern unsigned int * __imp__winver;
  extern unsigned int * __imp__winmajor;
  extern unsigned int * __imp__winminor;
  errno_t __attribute__((__cdecl__)) _get_osplatform(unsigned int *_Value);
  errno_t __attribute__((__cdecl__)) _get_osver(unsigned int *_Value);
  errno_t __attribute__((__cdecl__)) _get_winver(unsigned int *_Value);
  errno_t __attribute__((__cdecl__)) _get_winmajor(unsigned int *_Value);
  errno_t __attribute__((__cdecl__)) _get_winminor(unsigned int *_Value);
  void __attribute__((__cdecl__)) __attribute__ ((__nothrow__)) exit(int _Code) __attribute__ ((__noreturn__));
  void __attribute__((__cdecl__)) __attribute__ ((__nothrow__)) _exit(int _Code) __attribute__ ((__noreturn__));
  void __attribute__((__cdecl__)) _Exit(int) __attribute__ ((__noreturn__));
       
  void __attribute__((__cdecl__)) __attribute__ ((__noreturn__)) abort(void);
       
  __attribute__ ((__dllimport__)) unsigned int __attribute__((__cdecl__)) _set_abort_behavior(unsigned int _Flags,unsigned int _Mask);
  int __attribute__((__cdecl__)) abs(int _X);
  long __attribute__((__cdecl__)) labs(long _X);
  __extension__ long long __attribute__((__cdecl__)) _abs64(long long);
  extern __inline__ __attribute__((__always_inline__,__gnu_inline__)) long long __attribute__((__cdecl__)) _abs64(long long x) {
    return __builtin_llabs(x);
  }
  int __attribute__((__cdecl__)) atexit(void (__attribute__((__cdecl__)) *)(void));
  double __attribute__((__cdecl__)) atof(const char *_String);
  double __attribute__((__cdecl__)) _atof_l(const char *_String,_locale_t _Locale);
  int __attribute__((__cdecl__)) atoi(const char *_Str);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _atoi_l(const char *_Str,_locale_t _Locale);
  long __attribute__((__cdecl__)) atol(const char *_Str);
  __attribute__ ((__dllimport__)) long __attribute__((__cdecl__)) _atol_l(const char *_Str,_locale_t _Locale);
  void *__attribute__((__cdecl__)) bsearch(const void *_Key,const void *_Base,size_t _NumOfElements,size_t _SizeOfElements,int (__attribute__((__cdecl__)) *_PtFuncCompare)(const void *,const void *));
  void __attribute__((__cdecl__)) qsort(void *_Base,size_t _NumOfElements,size_t _SizeOfElements,int (__attribute__((__cdecl__)) *_PtFuncCompare)(const void *,const void *));
  unsigned short __attribute__((__cdecl__)) _byteswap_ushort(unsigned short _Short);
  unsigned long __attribute__((__cdecl__)) _byteswap_ulong (unsigned long _Long);
  __extension__ unsigned long long __attribute__((__cdecl__)) _byteswap_uint64(unsigned long long _Int64);
  div_t __attribute__((__cdecl__)) div(int _Numerator,int _Denominator);
  char *__attribute__((__cdecl__)) getenv(const char *_VarName) ;
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _itoa(int _Value,char *_Dest,int _Radix);
  __extension__ __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _i64toa(long long _Val,char *_DstBuf,int _Radix) ;
  __extension__ __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _ui64toa(unsigned long long _Val,char *_DstBuf,int _Radix) ;
  __extension__ __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _atoi64(const char *_String);
  __extension__ __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _atoi64_l(const char *_String,_locale_t _Locale);
  __extension__ __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _strtoi64(const char *_String,char **_EndPtr,int _Radix);
  __extension__ __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _strtoi64_l(const char *_String,char **_EndPtr,int _Radix,_locale_t _Locale);
  __extension__ __attribute__ ((__dllimport__)) unsigned long long __attribute__((__cdecl__)) _strtoui64(const char *_String,char **_EndPtr,int _Radix);
  __extension__ __attribute__ ((__dllimport__)) unsigned long long __attribute__((__cdecl__)) _strtoui64_l(const char *_String,char **_EndPtr,int _Radix,_locale_t _Locale);
  ldiv_t __attribute__((__cdecl__)) ldiv(long _Numerator,long _Denominator);
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _ltoa(long _Value,char *_Dest,int _Radix) ;
  int __attribute__((__cdecl__)) mblen(const char *_Ch,size_t _MaxCount);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _mblen_l(const char *_Ch,size_t _MaxCount,_locale_t _Locale);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _mbstrlen(const char *_Str);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _mbstrlen_l(const char *_Str,_locale_t _Locale);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _mbstrnlen(const char *_Str,size_t _MaxCount);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _mbstrnlen_l(const char *_Str,size_t _MaxCount,_locale_t _Locale);
  int __attribute__((__cdecl__)) mbtowc(wchar_t * __restrict__ _DstCh,const char * __restrict__ _SrcCh,size_t _SrcSizeInBytes);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _mbtowc_l(wchar_t * __restrict__ _DstCh,const char * __restrict__ _SrcCh,size_t _SrcSizeInBytes,_locale_t _Locale);
  size_t __attribute__((__cdecl__)) mbstowcs(wchar_t * __restrict__ _Dest,const char * __restrict__ _Source,size_t _MaxCount);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _mbstowcs_l(wchar_t * __restrict__ _Dest,const char * __restrict__ _Source,size_t _MaxCount,_locale_t _Locale);
  int __attribute__((__cdecl__)) mkstemp(char *template_name);
  int __attribute__((__cdecl__)) rand(void);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _set_error_mode(int _Mode);
  void __attribute__((__cdecl__)) srand(unsigned int _Seed);
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
double __attribute__((__cdecl__)) __attribute__ ((__nothrow__)) strtod(const char * __restrict__ _Str,char ** __restrict__ _EndPtr)
{
  double __attribute__((__cdecl__)) __mingw_strtod (const char * __restrict__, char ** __restrict__);
  return __mingw_strtod( _Str, _EndPtr);
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
float __attribute__((__cdecl__)) __attribute__ ((__nothrow__)) strtof(const char * __restrict__ _Str,char ** __restrict__ _EndPtr)
{
  float __attribute__((__cdecl__)) __mingw_strtof (const char * __restrict__, char ** __restrict__);
  return __mingw_strtof( _Str, _EndPtr);
}
  long double __attribute__((__cdecl__)) __attribute__ ((__nothrow__)) strtold(const char * __restrict__ , char ** __restrict__ );
  extern double __attribute__((__cdecl__)) __attribute__ ((__nothrow__))
  __strtod (const char * __restrict__ , char ** __restrict__);
  float __attribute__((__cdecl__)) __mingw_strtof (const char * __restrict__, char ** __restrict__);
  double __attribute__((__cdecl__)) __mingw_strtod (const char * __restrict__, char ** __restrict__);
  long double __attribute__((__cdecl__)) __mingw_strtold(const char * __restrict__, char ** __restrict__);
  __attribute__ ((__dllimport__)) float __attribute__((__cdecl__)) _strtof_l(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,_locale_t _Locale);
  __attribute__ ((__dllimport__)) double __attribute__((__cdecl__)) _strtod_l(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,_locale_t _Locale);
  long __attribute__((__cdecl__)) strtol(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,int _Radix);
  __attribute__ ((__dllimport__)) long __attribute__((__cdecl__)) _strtol_l(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
  unsigned long __attribute__((__cdecl__)) strtoul(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,int _Radix);
  __attribute__ ((__dllimport__)) unsigned long __attribute__((__cdecl__)) _strtoul_l(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
  int __attribute__((__cdecl__)) system(const char *_Command);
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _ultoa(unsigned long _Value,char *_Dest,int _Radix) ;
  int __attribute__((__cdecl__)) wctomb(char *_MbCh,wchar_t _WCh) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wctomb_l(char *_MbCh,wchar_t _WCh,_locale_t _Locale) ;
  size_t __attribute__((__cdecl__)) wcstombs(char * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _MaxCount) ;
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _wcstombs_l(char * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _MaxCount,_locale_t _Locale) ;
  void *__attribute__((__cdecl__)) calloc(size_t _NumOfElements,size_t _SizeOfElements);
  void __attribute__((__cdecl__)) free(void *_Memory);
  void *__attribute__((__cdecl__)) malloc(size_t _Size);
  void *__attribute__((__cdecl__)) realloc(void *_Memory,size_t _NewSize);
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _aligned_free(void *_Memory);
  __attribute__ ((__dllimport__)) void *__attribute__((__cdecl__)) _aligned_malloc(size_t _Size,size_t _Alignment);
  __attribute__ ((__dllimport__)) void *__attribute__((__cdecl__)) _aligned_offset_malloc(size_t _Size,size_t _Alignment,size_t _Offset);
  __attribute__ ((__dllimport__)) void *__attribute__((__cdecl__)) _aligned_realloc(void *_Memory,size_t _Size,size_t _Alignment);
  __attribute__ ((__dllimport__)) void *__attribute__((__cdecl__)) _aligned_offset_realloc(void *_Memory,size_t _Size,size_t _Alignment,size_t _Offset);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _putenv(const char *_EnvString);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wputenv(const wchar_t *_EnvString);
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _fullpath(char *_FullPath,const char *_Path,size_t _SizeInBytes);
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _ecvt(double _Val,int _NumOfDigits,int *_PtDec,int *_PtSign) ;
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _fcvt(double _Val,int _NumOfDec,int *_PtDec,int *_PtSign) ;
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _gcvt(double _Val,int _NumOfDigits,char *_DstBuf) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _atodbl(_CRT_DOUBLE *_Result,char *_Str);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _atoldbl(_LDOUBLE *_Result,char *_Str);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _atoflt(_CRT_FLOAT *_Result,char *_Str);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _atodbl_l(_CRT_DOUBLE *_Result,char *_Str,_locale_t _Locale);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _atoldbl_l(_LDOUBLE *_Result,char *_Str,_locale_t _Locale);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _atoflt_l(_CRT_FLOAT *_Result,char *_Str,_locale_t _Locale);
unsigned long __attribute__((__cdecl__)) _lrotl(unsigned long,int);
unsigned long __attribute__((__cdecl__)) _lrotr(unsigned long,int);
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _makepath(char *_Path,const char *_Drive,const char *_Dir,const char *_Filename,const char *_Ext);
  _onexit_t __attribute__((__cdecl__)) _onexit(_onexit_t _Func);
  void __attribute__((__cdecl__)) perror(const char *_ErrMsg);
       
       
  __extension__ unsigned long long __attribute__((__cdecl__)) _rotl64(unsigned long long _Val,int _Shift);
  __extension__ unsigned long long __attribute__((__cdecl__)) _rotr64(unsigned long long Value,int Shift);
       
       
       
       
  unsigned int __attribute__((__cdecl__)) _rotr(unsigned int _Val,int _Shift);
  unsigned int __attribute__((__cdecl__)) _rotl(unsigned int _Val,int _Shift);
       
       
  __extension__ unsigned long long __attribute__((__cdecl__)) _rotr64(unsigned long long _Val,int _Shift);
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _searchenv(const char *_Filename,const char *_EnvVar,char *_ResultPath) ;
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _splitpath(const char *_FullPath,char *_Drive,char *_Dir,char *_Filename,char *_Ext) ;
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _swab(char *_Buf1,char *_Buf2,int _SizeInBytes);
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _beep(unsigned _Frequency,unsigned _Duration) __attribute__ ((__deprecated__));
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _seterrormode(int _Mode) __attribute__ ((__deprecated__));
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _sleep(unsigned long _Duration) __attribute__ ((__deprecated__));
  char *__attribute__((__cdecl__)) ecvt(double _Val,int _NumOfDigits,int *_PtDec,int *_PtSign) ;
  char *__attribute__((__cdecl__)) fcvt(double _Val,int _NumOfDec,int *_PtDec,int *_PtSign) ;
  char *__attribute__((__cdecl__)) gcvt(double _Val,int _NumOfDigits,char *_DstBuf) ;
  char *__attribute__((__cdecl__)) itoa(int _Val,char *_DstBuf,int _Radix) ;
  char *__attribute__((__cdecl__)) ltoa(long _Val,char *_DstBuf,int _Radix) ;
  int __attribute__((__cdecl__)) putenv(const char *_EnvString) ;
  void __attribute__((__cdecl__)) swab(char *_Buf1,char *_Buf2,int _SizeInBytes) ;
  char *__attribute__((__cdecl__)) ultoa(unsigned long _Val,char *_Dstbuf,int _Radix) ;
  _onexit_t __attribute__((__cdecl__)) onexit(_onexit_t _Func);
  typedef struct { __extension__ long long quot, rem; } lldiv_t;
  __extension__ lldiv_t __attribute__((__cdecl__)) lldiv(long long, long long);
  __extension__ long long __attribute__((__cdecl__)) llabs(long long);
  __extension__ long long __attribute__((__cdecl__)) strtoll(const char * __restrict__, char ** __restrict, int);
  __extension__ unsigned long long __attribute__((__cdecl__)) strtoull(const char * __restrict__, char ** __restrict__, int);
  __extension__ long long __attribute__((__cdecl__)) atoll (const char *);
  __extension__ long long __attribute__((__cdecl__)) wtoll (const wchar_t *);
  __extension__ char *__attribute__((__cdecl__)) lltoa (long long, char *, int);
  __extension__ char *__attribute__((__cdecl__)) ulltoa (unsigned long long , char *, int);
  __extension__ wchar_t *__attribute__((__cdecl__)) lltow (long long, wchar_t *, int);
  __extension__ wchar_t *__attribute__((__cdecl__)) ulltow (unsigned long long, wchar_t *, int);
#pragma pack(pop)
  __attribute__ ((__dllimport__)) void * __attribute__((__cdecl__)) bsearch_s(const void *_Key,const void *_Base,rsize_t _NumOfElements,rsize_t _SizeOfElements,int (__attribute__((__cdecl__)) * _PtFuncCompare)(void *, const void *, const void *), void *_Context);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _dupenv_s(char **_PBuffer,size_t *_PBufferSizeInBytes,const char *_VarName);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) getenv_s(size_t *_ReturnSize,char *_DstBuf,rsize_t _DstSize,const char *_VarName);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _itoa_s(int _Value,char *_DstBuf,size_t _Size,int _Radix);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _i64toa_s(long long _Val,char *_DstBuf,size_t _Size,int _Radix);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _ui64toa_s(unsigned long long _Val,char *_DstBuf,size_t _Size,int _Radix);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _ltoa_s(long _Val,char *_DstBuf,size_t _Size,int _Radix);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) mbstowcs_s(size_t *_PtNumOfCharConverted,wchar_t *_DstBuf,size_t _SizeInWords,const char *_SrcBuf,size_t _MaxCount);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _mbstowcs_s_l(size_t *_PtNumOfCharConverted,wchar_t *_DstBuf,size_t _SizeInWords,const char *_SrcBuf,size_t _MaxCount,_locale_t _Locale);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _ultoa_s(unsigned long _Val,char *_DstBuf,size_t _Size,int _Radix);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) wctomb_s(int *_SizeConverted,char *_MbCh,rsize_t _SizeInBytes,wchar_t _WCh);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wctomb_s_l(int *_SizeConverted,char *_MbCh,size_t _SizeInBytes,wchar_t _WCh,_locale_t _Locale);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) wcstombs_s(size_t *_PtNumOfCharConverted,char *_Dst,size_t _DstSizeInBytes,const wchar_t *_Src,size_t _MaxCountInBytes);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wcstombs_s_l(size_t *_PtNumOfCharConverted,char *_Dst,size_t _DstSizeInBytes,const wchar_t *_Src,size_t _MaxCountInBytes,_locale_t _Locale);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _ecvt_s(char *_DstBuf,size_t _Size,double _Val,int _NumOfDights,int *_PtDec,int *_PtSign);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _fcvt_s(char *_DstBuf,size_t _Size,double _Val,int _NumOfDec,int *_PtDec,int *_PtSign);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _gcvt_s(char *_DstBuf,size_t _Size,double _Val,int _NumOfDigits);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _makepath_s(char *_PathResult,size_t _Size,const char *_Drive,const char *_Dir,const char *_Filename,const char *_Ext);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _putenv_s(const char *_Name,const char *_Value);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _searchenv_s(const char *_Filename,const char *_EnvVar,char *_ResultPath,size_t _SizeInBytes);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _splitpath_s(const char *_FullPath,char *_Drive,size_t _DriveSize,char *_Dir,size_t _DirSize,char *_Filename,size_t _FilenameSize,char *_Ext,size_t _ExtSize);
 
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) qsort_s(void *_Base,size_t _NumOfElements,size_t _SizeOfElements,int (__attribute__((__cdecl__)) *_PtFuncCompare)(void *,const void *,const void *),void *_Context);
#pragma pack(push,_CRT_PACKING)
  typedef struct _heapinfo {
    int *_pentry;
    size_t _size;
    int _useflag;
  } _HEAPINFO;
  extern unsigned int _amblksiz;
void * __mingw_aligned_malloc (size_t _Size, size_t _Alignment);
void __mingw_aligned_free (void *_Memory);
void * __mingw_aligned_offset_realloc (void *_Memory, size_t _Size, size_t _Alignment, size_t _Offset);
void * __mingw_aligned_realloc (void *_Memory, size_t _Size, size_t _Offset);
static __inline__ void *
_mm_malloc (size_t __size, size_t __align)
{
  void * __malloc_ptr;
  void * __aligned_ptr;
  if (__align & (__align - 1))
    {
      (*_errno()) = 22;
      return ((void *) 0);
    }
  if (__size == 0)
    return ((void *) 0);
    if (__align < 2 * sizeof (void *))
      __align = 2 * sizeof (void *);
  __malloc_ptr = malloc (__size + __align);
  if (!__malloc_ptr)
    return ((void *) 0);
  __aligned_ptr = (void *) (((size_t) __malloc_ptr + __align)
       & ~((size_t) (__align) - 1));
  ((void **) __aligned_ptr)[-1] = __malloc_ptr;
  return __aligned_ptr;
}
static __inline__ void
_mm_free (void *__aligned_ptr)
{
  if (__aligned_ptr)
    free (((void **) __aligned_ptr)[-1]);
}
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _resetstkoflw (void);
  __attribute__ ((__dllimport__)) unsigned long __attribute__((__cdecl__)) _set_malloc_crt_max_wait(unsigned long _NewValue);
  __attribute__ ((__dllimport__)) void *__attribute__((__cdecl__)) _expand(void *_Memory,size_t _NewSize);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _msize(void *_Memory);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _get_sbh_threshold(void);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _set_sbh_threshold(size_t _NewValue);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _set_amblksiz(size_t _Value);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _get_amblksiz(size_t *_Value);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _heapadd(void *_Memory,size_t _Size);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _heapchk(void);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _heapmin(void);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _heapset(unsigned int _Fill);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _heapwalk(_HEAPINFO *_EntryInfo);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _heapused(size_t *_Used,size_t *_Commit);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _get_heap_handle(void);
  static __inline void *_MarkAllocaS(void *_Ptr,unsigned int _Marker) {
    if(_Ptr) {
      *((unsigned int*)_Ptr) = _Marker;
      _Ptr = (char*)_Ptr + 16;
    }
    return _Ptr;
  }
  static __inline void __attribute__((__cdecl__)) _freea(void *_Memory) {
    unsigned int _Marker;
    if(_Memory) {
      _Memory = (char*)_Memory - 16;
      _Marker = *(unsigned int *)_Memory;
      if(_Marker==0xDDDD) {
 free(_Memory);
      }
    }
  }
#pragma pack(pop)
  __attribute__ ((__dllimport__)) void *__attribute__((__cdecl__)) _memccpy(void *_Dst,const void *_Src,int _Val,size_t _MaxCount);
  void *__attribute__((__cdecl__)) memchr(const void *_Buf ,int _Val,size_t _MaxCount);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _memicmp(const void *_Buf1,const void *_Buf2,size_t _Size);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _memicmp_l(const void *_Buf1,const void *_Buf2,size_t _Size,_locale_t _Locale);
  int __attribute__((__cdecl__)) memcmp(const void *_Buf1,const void *_Buf2,size_t _Size);
  void * __attribute__((__cdecl__)) memcpy(void * __restrict__ _Dst,const void * __restrict__ _Src,size_t _Size) ;
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) memcpy_s (void *_dest,size_t _numberOfElements,const void *_src,size_t _count);
  void * __attribute__((__cdecl__)) mempcpy (void *_Dst, const void *_Src, size_t _Size);
  void * __attribute__((__cdecl__)) memset(void *_Dst,int _Val,size_t _Size);
  void * __attribute__((__cdecl__)) memccpy(void *_Dst,const void *_Src,int _Val,size_t _Size) ;
  int __attribute__((__cdecl__)) memicmp(const void *_Buf1,const void *_Buf2,size_t _Size) ;
  char * __attribute__((__cdecl__)) _strset(char *_Str,int _Val) ;
  char * __attribute__((__cdecl__)) _strset_l(char *_Str,int _Val,_locale_t _Locale) ;
  char * __attribute__((__cdecl__)) strcpy(char * __restrict__ _Dest,const char * __restrict__ _Source);
  char * __attribute__((__cdecl__)) strcat(char * __restrict__ _Dest,const char * __restrict__ _Source);
  int __attribute__((__cdecl__)) strcmp(const char *_Str1,const char *_Str2);
  size_t __attribute__((__cdecl__)) strlen(const char *_Str);
  size_t __attribute__((__cdecl__)) strnlen(const char *_Str,size_t _MaxCount);
  void *__attribute__((__cdecl__)) memmove(void *_Dst,const void *_Src,size_t _Size) ;
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _strdup(const char *_Src);
  char *__attribute__((__cdecl__)) strchr(const char *_Str,int _Val);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _stricmp(const char *_Str1,const char *_Str2);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _strcmpi(const char *_Str1,const char *_Str2);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _stricmp_l(const char *_Str1,const char *_Str2,_locale_t _Locale);
  int __attribute__((__cdecl__)) strcoll(const char *_Str1,const char *_Str2);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _strcoll_l(const char *_Str1,const char *_Str2,_locale_t _Locale);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _stricoll(const char *_Str1,const char *_Str2);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _stricoll_l(const char *_Str1,const char *_Str2,_locale_t _Locale);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _strncoll (const char *_Str1,const char *_Str2,size_t _MaxCount);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _strncoll_l(const char *_Str1,const char *_Str2,size_t _MaxCount,_locale_t _Locale);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _strnicoll (const char *_Str1,const char *_Str2,size_t _MaxCount);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _strnicoll_l(const char *_Str1,const char *_Str2,size_t _MaxCount,_locale_t _Locale);
  size_t __attribute__((__cdecl__)) strcspn(const char *_Str,const char *_Control);
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _strerror(const char *_ErrMsg) ;
  char *__attribute__((__cdecl__)) strerror(int) ;
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _strlwr(char *_String) ;
  char *strlwr_l(char *_String,_locale_t _Locale) ;
  char *__attribute__((__cdecl__)) strncat(char * __restrict__ _Dest,const char * __restrict__ _Source,size_t _Count) ;
  int __attribute__((__cdecl__)) strncmp(const char *_Str1,const char *_Str2,size_t _MaxCount);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _strnicmp(const char *_Str1,const char *_Str2,size_t _MaxCount);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _strnicmp_l(const char *_Str1,const char *_Str2,size_t _MaxCount,_locale_t _Locale);
  char *strncpy(char * __restrict__ _Dest,const char * __restrict__ _Source,size_t _Count) ;
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _strnset(char *_Str,int _Val,size_t _MaxCount) ;
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _strnset_l(char *str,int c,size_t count,_locale_t _Locale) ;
  char *__attribute__((__cdecl__)) strpbrk(const char *_Str,const char *_Control);
  char *__attribute__((__cdecl__)) strrchr(const char *_Str,int _Ch);
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _strrev(char *_Str);
  size_t __attribute__((__cdecl__)) strspn(const char *_Str,const char *_Control);
  char *__attribute__((__cdecl__)) strstr(const char *_Str,const char *_SubStr);
  char *__attribute__((__cdecl__)) strtok(char * __restrict__ _Str,const char * __restrict__ _Delim) ;
       
  char *strtok_r(char * __restrict__ _Str, const char * __restrict__ _Delim, char ** __restrict__ __last);
       
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _strupr(char *_String) ;
  __attribute__ ((__dllimport__)) char *_strupr_l(char *_String,_locale_t _Locale) ;
  size_t __attribute__((__cdecl__)) strxfrm(char * __restrict__ _Dst,const char * __restrict__ _Src,size_t _MaxCount);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _strxfrm_l(char * __restrict__ _Dst,const char * __restrict__ _Src,size_t _MaxCount,_locale_t _Locale);
  char *__attribute__((__cdecl__)) strdup(const char *_Src) ;
  int __attribute__((__cdecl__)) strcmpi(const char *_Str1,const char *_Str2) ;
  int __attribute__((__cdecl__)) stricmp(const char *_Str1,const char *_Str2) ;
  char *__attribute__((__cdecl__)) strlwr(char *_Str) ;
  int __attribute__((__cdecl__)) strnicmp(const char *_Str1,const char *_Str,size_t _MaxCount) ;
  int __attribute__((__cdecl__)) strncasecmp (const char *, const char *, size_t);
  int __attribute__((__cdecl__)) strcasecmp (const char *, const char *);
  char *__attribute__((__cdecl__)) strnset(char *_Str,int _Val,size_t _MaxCount) ;
  char *__attribute__((__cdecl__)) strrev(char *_Str) ;
  char *__attribute__((__cdecl__)) strset(char *_Str,int _Val) ;
  char *__attribute__((__cdecl__)) strupr(char *_Str) ;
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _strset_s(char *_Dst,size_t _DstSize,int _Value);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _strerror_s(char *_Buf,size_t _SizeInBytes,const char *_ErrMsg);
 
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) strerror_s(char *_Buf,size_t _SizeInBytes,int _ErrNum);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _strlwr_s(char *_Str,size_t _Size);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _strlwr_s_l(char *_Str,size_t _Size,_locale_t _Locale);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _strnset_s(char *_Str,size_t _Size,int _Val,size_t _MaxCount);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _strupr_s(char *_Str,size_t _Size);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _strupr_s_l(char *_Str,size_t _Size,_locale_t _Locale);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) strncat_s(char *_Dst,size_t _DstSizeInChars,const char *_Src,size_t _MaxCount);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _strncat_s_l(char *_Dst,size_t _DstSizeInChars,const char *_Src,size_t _MaxCount,_locale_t _Locale);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) strcpy_s(char *_Dst, rsize_t _SizeInBytes, const char *_Src);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) strncpy_s(char *_Dst, size_t _DstSizeInChars, const char *_Src, size_t _MaxCount);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _strncpy_s_l(char *_Dst, size_t _DstSizeInChars, const char *_Src, size_t _MaxCount, _locale_t _Locale);
 
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) strtok_s(char *_Str,const char *_Delim,char **_Context);
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _strtok_s_l(char *_Str,const char *_Delim,char **_Context,_locale_t _Locale);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) strcat_s(char *_Dst, rsize_t _SizeInBytes, const char * _Src);
 
  extern __inline__ __attribute__((__always_inline__,__gnu_inline__)) size_t __attribute__((__cdecl__)) strnlen_s(const char * _src, size_t _count) {
    return _src ? strnlen(_src, _count) : 0;
  }
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) memmove_s(void *_dest,size_t _numberOfElements,const void *_src,size_t _count);
#pragma pack(push,_CRT_PACKING)
       
       
       
       
__attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) __acrt_iob_func(unsigned index);
  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) __iob_func(void);
  __extension__ typedef long long fpos_t;
extern
  __attribute__((__format__ (gnu_scanf, 2, 3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_sscanf(const char * __restrict__ _Src,const char * __restrict__ _Format,...);
extern
  __attribute__((__format__ (gnu_scanf, 2, 0))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_vsscanf (const char * __restrict__ _Str,const char * __restrict__ Format,va_list argp);
extern
  __attribute__((__format__ (gnu_scanf, 1, 2))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __mingw_scanf(const char * __restrict__ _Format,...);
extern
  __attribute__((__format__ (gnu_scanf, 1, 0))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __mingw_vscanf(const char * __restrict__ Format, va_list argp);
extern
  __attribute__((__format__ (gnu_scanf, 2, 3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_fscanf(FILE * __restrict__ _File,const char * __restrict__ _Format,...);
extern
  __attribute__((__format__ (gnu_scanf, 2, 0))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_vfscanf (FILE * __restrict__ fp, const char * __restrict__ Format,va_list argp);
extern
  __attribute__((__format__ (gnu_printf, 3, 0))) __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) __mingw_vsnprintf(char * __restrict__ _DstBuf,size_t _MaxCount,const char * __restrict__ _Format,
                               va_list _ArgList);
extern
  __attribute__((__format__ (gnu_printf, 3, 4))) __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) __mingw_snprintf(char * __restrict__ s, size_t n, const char * __restrict__ format, ...);
extern
  __attribute__((__format__ (gnu_printf, 1, 2))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __mingw_printf(const char * __restrict__ , ... ) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__ (gnu_printf, 1, 0))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __mingw_vprintf (const char * __restrict__ , va_list) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__ (gnu_printf, 2, 3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_fprintf (FILE * __restrict__ , const char * __restrict__ , ...) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__ (gnu_printf, 2, 0))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_vfprintf (FILE * __restrict__ , const char * __restrict__ , va_list) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__ (gnu_printf, 2, 3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_sprintf (char * __restrict__ , const char * __restrict__ , ...) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__ (gnu_printf, 2, 0))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_vsprintf (char * __restrict__ , const char * __restrict__ , va_list) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__ (gnu_printf, 2, 3))) __attribute__((nonnull (1,2)))
  int __attribute__((__cdecl__)) __mingw_asprintf(char ** __restrict__ , const char * __restrict__ , ...) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__ (gnu_printf, 2, 0))) __attribute__((nonnull (1,2)))
  int __attribute__((__cdecl__)) __mingw_vasprintf(char ** __restrict__ , const char * __restrict__ , va_list) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__ (ms_scanf, 2, 3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_sscanf(const char * __restrict__ _Src,const char * __restrict__ _Format,...);
extern
  __attribute__((__format__ (ms_scanf, 1, 2))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __ms_scanf(const char * __restrict__ _Format,...);
extern
  __attribute__((__format__ (ms_scanf, 2, 3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_fscanf(FILE * __restrict__ _File,const char * __restrict__ _Format,...);
extern
  __attribute__((__format__ (ms_printf, 1, 2))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __ms_printf(const char * __restrict__ , ... ) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__ (ms_printf, 1, 0))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __ms_vprintf (const char * __restrict__ , va_list) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__ (ms_printf, 2, 3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_fprintf (FILE * __restrict__ , const char * __restrict__ , ...) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__ (ms_printf, 2, 0))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_vfprintf (FILE * __restrict__ , const char * __restrict__ , va_list) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__ (ms_printf, 2, 3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_sprintf (char * __restrict__ , const char * __restrict__ , ...) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__ (ms_printf, 2, 0))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_vsprintf (char * __restrict__ , const char * __restrict__ , va_list) __attribute__ ((__nothrow__));
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
__attribute__((__format__ (gnu_scanf, 2, 3))) __attribute__ ((__nonnull__ (2)))
int sscanf(const char *__source, const char *__format, ...)
{
  int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vsscanf( __source, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
__attribute__((__format__ (gnu_scanf, 1, 2))) __attribute__ ((__nonnull__ (1)))
int scanf(const char *__format, ...)
{
  int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfscanf( (__acrt_iob_func(0)), __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
__attribute__((__format__ (gnu_scanf, 2, 3))) __attribute__ ((__nonnull__ (2)))
int fscanf(FILE *__stream, const char *__format, ...)
{
  int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfscanf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
__attribute__((__format__ (gnu_scanf, 2, 0))) __attribute__ ((__nonnull__ (2)))
int vsscanf (const char *__source, const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vsscanf( __source, __format, __local_argv );
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
__attribute__((__format__ (gnu_scanf, 1, 0))) __attribute__ ((__nonnull__ (1)))
int vscanf(const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfscanf( (__acrt_iob_func(0)), __format, __local_argv );
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
__attribute__((__format__ (gnu_scanf, 2, 0))) __attribute__ ((__nonnull__ (2)))
int vfscanf (FILE *__stream, const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfscanf( __stream, __format, __local_argv );
}
#pragma GCC diagnostic pop
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
__attribute__((__format__ (gnu_printf, 2, 3))) __attribute__ ((__nonnull__ (2)))
int fprintf (FILE *__stream, const char *__format, ...)
{
  int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfprintf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
__attribute__((__format__ (gnu_printf, 1, 2))) __attribute__ ((__nonnull__ (1)))
int printf (const char *__format, ...)
{
  int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfprintf( (__acrt_iob_func(1)), __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
__attribute__((__format__ (gnu_printf, 2, 3))) __attribute__ ((__nonnull__ (2)))
int sprintf (char *__stream, const char *__format, ...)
{
  int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vsprintf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
__attribute__((__format__ (gnu_printf, 2, 0))) __attribute__ ((__nonnull__ (2)))
int vfprintf (FILE *__stream, const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfprintf( __stream, __format, __local_argv );
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
__attribute__((__format__ (gnu_printf, 1, 0))) __attribute__ ((__nonnull__ (1)))
int vprintf (const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfprintf( (__acrt_iob_func(1)), __format, __local_argv );
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
__attribute__((__format__ (gnu_printf, 2, 0))) __attribute__ ((__nonnull__ (2)))
int vsprintf (char *__stream, const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vsprintf( __stream, __format, __local_argv );
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
__attribute__((__format__ (gnu_printf, 3, 4))) __attribute__ ((__nonnull__ (3)))
int snprintf (char *__stream, size_t __n, const char *__format, ...)
{
  int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vsnprintf( __stream, __n, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}
static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__))
__attribute__((__format__ (gnu_printf, 3, 0))) __attribute__ ((__nonnull__ (3)))
int vsnprintf (char *__stream, size_t __n, const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vsnprintf( __stream, __n, __format, __local_argv );
}
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _filbuf(FILE *_File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _flsbuf(int _Ch,FILE *_File);
  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) _fsopen(const char *_Filename,const char *_Mode,int _ShFlag);
  void __attribute__((__cdecl__)) clearerr(FILE *_File);
  int __attribute__((__cdecl__)) fclose(FILE *_File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fcloseall(void);
  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) _fdopen(int _FileHandle,const char *_Mode);
  int __attribute__((__cdecl__)) feof(FILE *_File);
  int __attribute__((__cdecl__)) ferror(FILE *_File);
  int __attribute__((__cdecl__)) fflush(FILE *_File);
  int __attribute__((__cdecl__)) fgetc(FILE *_File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fgetchar(void);
  int __attribute__((__cdecl__)) fgetpos(FILE * __restrict__ _File ,fpos_t * __restrict__ _Pos);
  int __attribute__((__cdecl__)) fgetpos64(FILE * __restrict__ _File ,fpos_t * __restrict__ _Pos);
  char *__attribute__((__cdecl__)) fgets(char * __restrict__ _Buf,int _MaxCount,FILE * __restrict__ _File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fileno(FILE *_File);
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _tempnam(const char *_DirName,const char *_FilePrefix);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _flushall(void);
  FILE *__attribute__((__cdecl__)) fopen(const char * __restrict__ _Filename,const char * __restrict__ _Mode) ;
  FILE *fopen64(const char * __restrict__ filename,const char * __restrict__ mode);
  int __attribute__((__cdecl__)) fputc(int _Ch,FILE *_File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fputchar(int _Ch);
  int __attribute__((__cdecl__)) fputs(const char * __restrict__ _Str,FILE * __restrict__ _File);
  size_t __attribute__((__cdecl__)) fread(void * __restrict__ _DstBuf,size_t _ElementSize,size_t _Count,FILE * __restrict__ _File);
  FILE *__attribute__((__cdecl__)) freopen(const char * __restrict__ _Filename,const char * __restrict__ _Mode,FILE * __restrict__ _File) ;
  int __attribute__((__cdecl__)) fsetpos(FILE *_File,const fpos_t *_Pos);
  int __attribute__((__cdecl__)) fsetpos64(FILE *_File,const fpos_t *_Pos);
  int __attribute__((__cdecl__)) fseek(FILE *_File,long _Offset,int _Origin);
  long __attribute__((__cdecl__)) ftell(FILE *_File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fseeki64(FILE *_File,long long _Offset,int _Origin);
  __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _ftelli64(FILE *_File);
  int fseeko64(FILE* stream, _off64_t offset, int whence);
  int fseeko(FILE* stream, _off_t offset, int whence);
  _off_t ftello(FILE * stream);
  _off64_t ftello64(FILE * stream);
  size_t __attribute__((__cdecl__)) fwrite(const void * __restrict__ _Str,size_t _Size,size_t _Count,FILE * __restrict__ _File);
  int __attribute__((__cdecl__)) getc(FILE *_File);
  int __attribute__((__cdecl__)) getchar(void);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _getmaxstdio(void);
  char *__attribute__((__cdecl__)) gets(char *_Buffer)
    __attribute__((__warning__("Using gets() is always unsafe - use fgets() instead")));
  int __attribute__((__cdecl__)) _getw(FILE *_File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _pclose(FILE *_File);
  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) _popen(const char *_Command,const char *_Mode);
  int __attribute__((__cdecl__)) putc(int _Ch,FILE *_File);
  int __attribute__((__cdecl__)) putchar(int _Ch);
  int __attribute__((__cdecl__)) puts(const char *_Str);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _putw(int _Word,FILE *_File);
  int __attribute__((__cdecl__)) remove(const char *_Filename);
  int __attribute__((__cdecl__)) rename(const char *_OldFilename,const char *_NewFilename);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _unlink(const char *_Filename);
  int __attribute__((__cdecl__)) unlink(const char *_Filename) ;
  void __attribute__((__cdecl__)) rewind(FILE *_File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _rmtmp(void);
  void __attribute__((__cdecl__)) setbuf(FILE * __restrict__ _File,char * __restrict__ _Buffer) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _setmaxstdio(int _Max);
  __attribute__ ((__dllimport__)) unsigned int __attribute__((__cdecl__)) _set_output_format(unsigned int _Format);
  __attribute__ ((__dllimport__)) unsigned int __attribute__((__cdecl__)) _get_output_format(void);
  int __attribute__((__cdecl__)) setvbuf(FILE * __restrict__ _File,char * __restrict__ _Buf,int _Mode,size_t _Size);
  __attribute__ ((__pure__))
  __attribute__((__format__ (ms_printf, 1, 2))) __attribute__ ((__nonnull__ (1)))
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _scprintf(const char * __restrict__ _Format,...);
  __attribute__((__format__ (ms_scanf, 3, 4))) __attribute__ ((__nonnull__ (3)))
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snscanf(const char * __restrict__ _Src,size_t _MaxCount,const char * __restrict__ _Format,...) ;
  __attribute__ ((__pure__))
  __attribute__((__format__ (ms_printf, 1, 0))) __attribute__ ((__nonnull__ (1)))
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vscprintf(const char * __restrict__ _Format,va_list _ArgList);
  FILE *__attribute__((__cdecl__)) tmpfile(void) ;
  char *__attribute__((__cdecl__)) tmpnam(char *_Buffer);
  int __attribute__((__cdecl__)) ungetc(int _Ch,FILE *_File);
  __attribute__((__format__ (ms_printf, 3, 4))) __attribute__ ((__nonnull__ (3)))
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snprintf(char * __restrict__ _Dest,size_t _Count,const char * __restrict__ _Format,...) ;
  __attribute__((__format__ (ms_printf, 3, 0))) __attribute__ ((__nonnull__ (3)))
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vsnprintf(char * __restrict__ _Dest,size_t _Count,const char * __restrict__ _Format,va_list _Args) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _set_printf_count_output(int _Value);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _get_printf_count_output(void);
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _lock_file(FILE *_File);
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _unlock_file(FILE *_File);
  char *__attribute__((__cdecl__)) tempnam(const char *_Directory,const char *_FilePrefix) ;
  int __attribute__((__cdecl__)) fcloseall(void) ;
  FILE *__attribute__((__cdecl__)) fdopen(int _FileHandle,const char *_Format) ;
  int __attribute__((__cdecl__)) fgetchar(void) ;
  int __attribute__((__cdecl__)) fileno(FILE *_File) ;
  int __attribute__((__cdecl__)) flushall(void) ;
  int __attribute__((__cdecl__)) fputchar(int _Ch) ;
  int __attribute__((__cdecl__)) getw(FILE *_File) ;
  int __attribute__((__cdecl__)) putw(int _Ch,FILE *_File) ;
  int __attribute__((__cdecl__)) rmtmp(void) ;
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _spawnv(int _Mode,const char *_Filename,const char *const *_ArgList);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _spawnve(int _Mode,const char *_Filename,const char *const *_ArgList,const char *const *_Env);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _spawnvp(int _Mode,const char *_Filename,const char *const *_ArgList);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _spawnvpe(int _Mode,const char *_Filename,const char *const *_ArgList,const char *const *_Env);
       
       
       
       
#pragma pack(pop)
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) clearerr_s(FILE *_File);
  size_t __attribute__((__cdecl__)) fread_s(void *_DstBuf,size_t _DstSize,size_t _ElementSize,size_t _Count,FILE *_File);
  int __attribute__((__cdecl__)) fprintf_s(FILE *_File,const char *_Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fscanf_s_l(FILE *_File,const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) fscanf_s(FILE *_File, const char *_Format, ...);
  int __attribute__((__cdecl__)) printf_s(const char *_Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _scanf_l(const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _scanf_s_l(const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) scanf_s(const char *_Format, ...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snprintf_c(char *_DstBuf,size_t _MaxCount,const char *_Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vsnprintf_c(char *_DstBuf,size_t _MaxCount,const char *_Format,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fscanf_l(FILE *_File,const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _sscanf_l(const char *_Src,const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _sscanf_s_l(const char *_Src,const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) sscanf_s(const char *_Src,const char *_Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snscanf_s(const char *_Src,size_t _MaxCount,const char *_Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snscanf_l(const char *_Src,size_t _MaxCount,const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snscanf_s_l(const char *_Src,size_t _MaxCount,const char *_Format,_locale_t _Locale,...);
  int __attribute__((__cdecl__)) vfprintf_s(FILE *_File,const char *_Format,va_list _ArgList);
  int __attribute__((__cdecl__)) vprintf_s(const char *_Format,va_list _ArgList);
  int __attribute__((__cdecl__)) vsnprintf_s(char *_DstBuf,size_t _DstSize,size_t _MaxCount,const char *_Format,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vsnprintf_s(char *_DstBuf,size_t _DstSize,size_t _MaxCount,const char *_Format,va_list _ArgList);
  __attribute__((dllimport)) int __attribute__((__cdecl__)) vsprintf_s(char *_DstBuf,size_t _Size,const char *_Format,va_list _ArgList);
  __attribute__((dllimport)) int __attribute__((__cdecl__)) sprintf_s(char *_DstBuf,size_t _DstSize,const char *_Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snprintf_s(char *_DstBuf,size_t _DstSize,size_t _MaxCount,const char *_Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fprintf_p(FILE *_File,const char *_Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _printf_p(const char *_Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _sprintf_p(char *_Dst,size_t _MaxCount,const char *_Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vfprintf_p(FILE *_File,const char *_Format,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vprintf_p(const char *_Format,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vsprintf_p(char *_Dst,size_t _MaxCount,const char *_Format,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _scprintf_p(const char *_Format,...);
  __attribute__((dllimport)) int __attribute__((__cdecl__)) _vscprintf_p(const char *_Format,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _printf_l(const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _printf_p_l(const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vprintf_l(const char *_Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vprintf_p_l(const char *_Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fprintf_l(FILE *_File,const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fprintf_p_l(FILE *_File,const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vfprintf_l(FILE *_File,const char *_Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vfprintf_p_l(FILE *_File,const char *_Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _sprintf_l(char *_DstBuf,const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _sprintf_p_l(char *_DstBuf,size_t _MaxCount,const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vsprintf_l(char *_DstBuf,const char *_Format,_locale_t,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vsprintf_p_l(char *_DstBuf,size_t _MaxCount,const char *_Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _scprintf_l(const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _scprintf_p_l(const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vscprintf_l(const char *_Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vscprintf_p_l(const char *_Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _printf_s_l(const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vprintf_s_l(const char *_Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fprintf_s_l(FILE *_File,const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vfprintf_s_l(FILE *_File,const char *_Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _sprintf_s_l(char *_DstBuf,size_t _DstSize,const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vsprintf_s_l(char *_DstBuf,size_t _DstSize,const char *_Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snprintf_s_l(char *_DstBuf,size_t _DstSize,size_t _MaxCount,const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vsnprintf_s_l(char *_DstBuf,size_t _DstSize,size_t _MaxCount,const char *_Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snprintf_l(char *_DstBuf,size_t _MaxCount,const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snprintf_c_l(char *_DstBuf,size_t _MaxCount,const char *_Format,_locale_t _Locale,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vsnprintf_l(char *_DstBuf,size_t _MaxCount,const char *_Format,_locale_t _Locale,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vsnprintf_c_l(char *_DstBuf,size_t _MaxCount,const char *,_locale_t _Locale,va_list _ArgList);
 
 
 
 
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) fopen_s(FILE **_File,const char *_Filename,const char *_Mode);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) freopen_s(FILE** _File, const char *_Filename, const char *_Mode, FILE *_Stream);
  __attribute__ ((__dllimport__)) char* __attribute__((__cdecl__)) gets_s(char*,rsize_t);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) tmpfile_s(FILE **_File);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) tmpnam_s(char*,rsize_t);
 
typedef enum hash_map_err {
    HM_SUCCESS = 0,
    HM_KEY_NOT_FOUND,
    HM_MALLOC_MAP_FAILURE,
    HM_MALLOC_PAIR_FAILURE
} hash_map_err;
int uint_comp(size_t a, size_t b);
size_t uint_hash(size_t a, size_t hash_size);
size_t cstr_hash(char const * key, size_t bin_size);
int cstr_comp(char const * a, char const * b);
size_t address_hash(void const * val, size_t bin_size);
int address_comp(void const * a, void const * b);
void hm_print_error_message(hash_map_err status);
typedef struct size_t_pASTNode_pair {
    size_t key;
    pASTNode value;
} size_t_pASTNode_pair;
typedef struct size_t_pASTNode_map size_t_pASTNode_map;
typedef struct size_t_pASTNode_Type {
    _Bool (*in)(size_t_pASTNode_map * map, size_t key);
    hash_map_err (*get)(size_t_pASTNode_map * map, size_t key, pASTNode * value);
    hash_map_err (*set)(size_t_pASTNode_map * map, size_t key, pASTNode value);
    void (*dest)(size_t_pASTNode_map * map);
    hash_map_err (*pop)(size_t_pASTNode_map * map, size_t key, pASTNode * value);
    hash_map_err (*remove)(size_t_pASTNode_map * map, size_t key);
    void (*for_each)(size_t_pASTNode_map * map, int (*handle_item)(void * data, size_t key, pASTNode value), void * data);
} size_t_pASTNode_Type;
struct size_t_pASTNode_map {
    size_t_pASTNode_Type const * _class;
    size_t_pASTNode_pair ** bins;
    size_t capacity;
    size_t fill;
};
static hash_map_err size_t_pASTNode_insert(size_t_pASTNode_map * map, size_t_pASTNode_pair * pair);
static size_t_pASTNode_pair * size_t_pASTNode_get_pair(size_t_pASTNode_map * map, size_t key, size_t * bin_num);
static hash_map_err size_t_pASTNode_pop(size_t_pASTNode_map * map, size_t key, pASTNode * value);
static hash_map_err size_t_pASTNode_remove(size_t_pASTNode_map * map, size_t key);
static hash_map_err size_t_pASTNode_get(size_t_pASTNode_map * map, size_t key, pASTNode * value);
static _Bool size_t_pASTNode_in(size_t_pASTNode_map * map, size_t key);
static hash_map_err size_t_pASTNode_set(size_t_pASTNode_map * map, size_t key, pASTNode value);
static void size_t_pASTNode_dest(size_t_pASTNode_map * map);
static void size_t_pASTNode_for_each(size_t_pASTNode_map * map, int (*handle_item)(void * data, size_t key, pASTNode value), void * data);
static hash_map_err size_t_pASTNode_resize(size_t_pASTNode_map * map, size_t new_capacity);
static hash_map_err size_t_pASTNode_init(size_t_pASTNode_map * map, size_t init_capacity);
static const size_t_pASTNode_Type size_t_pASTNode_class = {
    .in = &(size_t_pASTNode_in),
    .dest = &(size_t_pASTNode_dest),
    .get = &(size_t_pASTNode_get),
    .set = &(size_t_pASTNode_set),
    .pop = &(size_t_pASTNode_pop),
    .remove = &(size_t_pASTNode_remove),
    .for_each = &(size_t_pASTNode_for_each),
};
static hash_map_err size_t_pASTNode_insert(size_t_pASTNode_map * map, size_t_pASTNode_pair * pair) {
    hash_map_err status = HM_SUCCESS;
    if (map->fill >= map->capacity * 2 / 5) {
        if ((status = size_t_pASTNode_resize(map, next_prime(2*map->capacity)))) {
            return status;
        }
    }
    size_t bin = uint_hash(pair->key, map->capacity);
    size_t_pASTNode_pair * p;
    while ((p = map->bins[bin], p) && (p != ((void *)1))) {
        if (++bin == map->capacity) {
            bin = 0;
        }
    }
    map->bins[bin] = pair;
    map->fill++;
    return status;
}
static size_t_pASTNode_pair * size_t_pASTNode_get_pair(size_t_pASTNode_map * map, size_t key, size_t * bin_num) {
    size_t bin_start = uint_hash(key, map->capacity);
    size_t_pASTNode_pair * pair = map->bins[bin_start];
    if (!(pair && ((pair == ((void *)1)) || uint_comp(pair->key, key)))) {
        if (bin_num) {
            *bin_num = bin_start;
        }
        return pair;
    }
    size_t bin = bin_start + 1;
    if (bin == map->capacity) {
        bin = 0;
    }
    while ((bin != bin_start) && (pair = map->bins[bin], pair && ((pair == ((void *)1)) || uint_comp(pair->key, key)))) {
        if (++bin == map->capacity) {
            bin = 0;
        }
    }
    if (bin == bin_start) {
        return ((void *)0);
    }
    if (bin_num) {
        *bin_num = bin;
    }
    return pair;
}
static hash_map_err size_t_pASTNode_pop(size_t_pASTNode_map * map, size_t key, pASTNode * value) {
    size_t bin_num;
    size_t_pASTNode_pair * pair = size_t_pASTNode_get_pair(map, key, &bin_num);
    if (!pair) {
        return HM_KEY_NOT_FOUND;
    }
    if (value) {
        memcpy(value, &(pair->value), sizeof(pASTNode));
    }
    free(map->bins[bin_num]);
    map->bins[bin_num] = ((void *)1);
    map->fill--;
    return HM_SUCCESS;
}
static hash_map_err size_t_pASTNode_remove(size_t_pASTNode_map * map, size_t key) {
    return size_t_pASTNode_pop(map, key, ((void *)0));
}
static hash_map_err size_t_pASTNode_get(size_t_pASTNode_map * map, size_t key, pASTNode * value) {
    size_t_pASTNode_pair * pair = size_t_pASTNode_get_pair(map, key, ((void *)0));
    if (!pair) {
        return HM_KEY_NOT_FOUND;
    }
    if (value) {
        memcpy(value, &(pair->value), sizeof(pASTNode));
    }
    return HM_SUCCESS;
}
static _Bool size_t_pASTNode_in(size_t_pASTNode_map * map, size_t key) {
    return size_t_pASTNode_get_pair(map, key, ((void *)0)) != ((void *)0);
}
static hash_map_err size_t_pASTNode_set(size_t_pASTNode_map * map, size_t key, pASTNode value) {
    size_t_pASTNode_pair * pair = size_t_pASTNode_get_pair(map, key, ((void *)0));
    if (pair) {
        memmove(&(pair->value), &value, sizeof(pASTNode));
        return HM_SUCCESS;
    } else {
    }
    pair = malloc(sizeof(size_t_pASTNode_pair));
    if (!pair) {
        return HM_MALLOC_PAIR_FAILURE;
    }
    memcpy(&(pair->key), &key, sizeof(size_t));
    memcpy(&(pair->value), &value, sizeof(pASTNode));
    return size_t_pASTNode_insert(map, pair);
}
static void size_t_pASTNode_dest(size_t_pASTNode_map * map) {
    if (map->bins) {
        for (size_t i = 0; i < map->capacity; i++) {
            size_t_pASTNode_pair * p = map->bins[i];
            if (p && (p != ((void *)1))) {
                free(p);
            }
            map->bins[i] = ((void *)0);
        }
        free(map->bins);
        map->bins = ((void *)0);
    }
    map->fill = 0;
    map->capacity = 0;
}
static void size_t_pASTNode_for_each(size_t_pASTNode_map * map, int (*handle_item)(void * data, size_t key, pASTNode value), void * data) {
    int status = 0;
    size_t i = 0;
    for (i = 0; i < map->capacity; i++) {
        size_t_pASTNode_pair * p = map->bins[i];
        if (p && (p != ((void *)1))) {
            if ((status = handle_item(data, p->key, p->value))) {
                break;
            }
        }
    }
}
static hash_map_err size_t_pASTNode_init(size_t_pASTNode_map * map, size_t init_capacity) {
    if (!init_capacity) {
        init_capacity = 17;
    }
    map->bins = ((void *)0);
    map->capacity = 0;
    map->fill = 0;
    map->_class = &(size_t_pASTNode_class);
    return size_t_pASTNode_resize(map, init_capacity);
}
static hash_map_err size_t_pASTNode_resize(size_t_pASTNode_map * map, size_t new_capacity) {
    if (!new_capacity) {
        size_t_pASTNode_dest(map);
        return HM_SUCCESS;
    }
    size_t_pASTNode_pair ** old_bins = map->bins;
    size_t old_capacity = map->capacity;
    map->bins = calloc(new_capacity, sizeof(*(map->bins)));
    map->fill = 0;
    map->capacity = new_capacity;
    if (!map->bins) {
        return HM_MALLOC_MAP_FAILURE;
    }
    hash_map_err status = HM_SUCCESS;
    for (size_t i = 0; i < old_capacity; i++) {
        if (old_bins[i]) {
            if ((status = size_t_pASTNode_insert(map, old_bins[i]))) {
                return status;
            }
        }
    }
    free(old_bins);
    return HM_SUCCESS;
}
typedef size_t_pASTNode_map SAMap, * pSAMap;
int pToken_comp(pToken a, pToken b);
size_t pToken_hash(pToken a, size_t hash_size);
typedef struct pToken_pSAMap_pair {
    pToken key;
    pSAMap value;
} pToken_pSAMap_pair;
typedef struct pToken_pSAMap_map pToken_pSAMap_map;
typedef struct pToken_pSAMap_Type {
    _Bool (*in)(pToken_pSAMap_map * map, pToken key);
    hash_map_err (*get)(pToken_pSAMap_map * map, pToken key, pSAMap * value);
    hash_map_err (*set)(pToken_pSAMap_map * map, pToken key, pSAMap value);
    void (*dest)(pToken_pSAMap_map * map);
    hash_map_err (*pop)(pToken_pSAMap_map * map, pToken key, pSAMap * value);
    hash_map_err (*remove)(pToken_pSAMap_map * map, pToken key);
    void (*for_each)(pToken_pSAMap_map * map, int (*handle_item)(void * data, pToken key, pSAMap value), void * data);
} pToken_pSAMap_Type;
struct pToken_pSAMap_map {
    pToken_pSAMap_Type const * _class;
    pToken_pSAMap_pair ** bins;
    size_t capacity;
    size_t fill;
};
static hash_map_err pToken_pSAMap_insert(pToken_pSAMap_map * map, pToken_pSAMap_pair * pair);
static pToken_pSAMap_pair * pToken_pSAMap_get_pair(pToken_pSAMap_map * map, pToken key, size_t * bin_num);
static hash_map_err pToken_pSAMap_pop(pToken_pSAMap_map * map, pToken key, pSAMap * value);
static hash_map_err pToken_pSAMap_remove(pToken_pSAMap_map * map, pToken key);
static hash_map_err pToken_pSAMap_get(pToken_pSAMap_map * map, pToken key, pSAMap * value);
static _Bool pToken_pSAMap_in(pToken_pSAMap_map * map, pToken key);
static hash_map_err pToken_pSAMap_set(pToken_pSAMap_map * map, pToken key, pSAMap value);
static void pToken_pSAMap_dest(pToken_pSAMap_map * map);
static void pToken_pSAMap_for_each(pToken_pSAMap_map * map, int (*handle_item)(void * data, pToken key, pSAMap value), void * data);
static hash_map_err pToken_pSAMap_resize(pToken_pSAMap_map * map, size_t new_capacity);
static hash_map_err pToken_pSAMap_init(pToken_pSAMap_map * map, size_t init_capacity);
static const pToken_pSAMap_Type pToken_pSAMap_class = {
    .in = &(pToken_pSAMap_in),
    .dest = &(pToken_pSAMap_dest),
    .get = &(pToken_pSAMap_get),
    .set = &(pToken_pSAMap_set),
    .pop = &(pToken_pSAMap_pop),
    .remove = &(pToken_pSAMap_remove),
    .for_each = &(pToken_pSAMap_for_each),
};
static hash_map_err pToken_pSAMap_insert(pToken_pSAMap_map * map, pToken_pSAMap_pair * pair) {
    hash_map_err status = HM_SUCCESS;
    if (map->fill >= map->capacity * 2 / 5) {
        if ((status = pToken_pSAMap_resize(map, next_prime(2*map->capacity)))) {
            return status;
        }
    }
    size_t bin = pToken_hash(pair->key, map->capacity);
    pToken_pSAMap_pair * p;
    while ((p = map->bins[bin], p) && (p != ((void *)1))) {
        if (++bin == map->capacity) {
            bin = 0;
        }
    }
    map->bins[bin] = pair;
    map->fill++;
    return status;
}
static pToken_pSAMap_pair * pToken_pSAMap_get_pair(pToken_pSAMap_map * map, pToken key, size_t * bin_num) {
    size_t bin_start = pToken_hash(key, map->capacity);
    pToken_pSAMap_pair * pair = map->bins[bin_start];
    if (!(pair && ((pair == ((void *)1)) || pToken_comp(pair->key, key)))) {
        if (bin_num) {
            *bin_num = bin_start;
        }
        return pair;
    }
    size_t bin = bin_start + 1;
    if (bin == map->capacity) {
        bin = 0;
    }
    while ((bin != bin_start) && (pair = map->bins[bin], pair && ((pair == ((void *)1)) || pToken_comp(pair->key, key)))) {
        if (++bin == map->capacity) {
            bin = 0;
        }
    }
    if (bin == bin_start) {
        return ((void *)0);
    }
    if (bin_num) {
        *bin_num = bin;
    }
    return pair;
}
static hash_map_err pToken_pSAMap_pop(pToken_pSAMap_map * map, pToken key, pSAMap * value) {
    size_t bin_num;
    pToken_pSAMap_pair * pair = pToken_pSAMap_get_pair(map, key, &bin_num);
    if (!pair) {
        return HM_KEY_NOT_FOUND;
    }
    if (value) {
        memcpy(value, &(pair->value), sizeof(pSAMap));
    }
    free(map->bins[bin_num]);
    map->bins[bin_num] = ((void *)1);
    map->fill--;
    return HM_SUCCESS;
}
static hash_map_err pToken_pSAMap_remove(pToken_pSAMap_map * map, pToken key) {
    return pToken_pSAMap_pop(map, key, ((void *)0));
}
static hash_map_err pToken_pSAMap_get(pToken_pSAMap_map * map, pToken key, pSAMap * value) {
    pToken_pSAMap_pair * pair = pToken_pSAMap_get_pair(map, key, ((void *)0));
    if (!pair) {
        return HM_KEY_NOT_FOUND;
    }
    if (value) {
        memcpy(value, &(pair->value), sizeof(pSAMap));
    }
    return HM_SUCCESS;
}
static _Bool pToken_pSAMap_in(pToken_pSAMap_map * map, pToken key) {
    return pToken_pSAMap_get_pair(map, key, ((void *)0)) != ((void *)0);
}
static hash_map_err pToken_pSAMap_set(pToken_pSAMap_map * map, pToken key, pSAMap value) {
    pToken_pSAMap_pair * pair = pToken_pSAMap_get_pair(map, key, ((void *)0));
    if (pair) {
        memmove(&(pair->value), &value, sizeof(pSAMap));
        return HM_SUCCESS;
    } else {
    }
    pair = malloc(sizeof(pToken_pSAMap_pair));
    if (!pair) {
        return HM_MALLOC_PAIR_FAILURE;
    }
    memcpy(&(pair->key), &key, sizeof(pToken));
    memcpy(&(pair->value), &value, sizeof(pSAMap));
    return pToken_pSAMap_insert(map, pair);
}
static void pToken_pSAMap_dest(pToken_pSAMap_map * map) {
    if (map->bins) {
        for (size_t i = 0; i < map->capacity; i++) {
            pToken_pSAMap_pair * p = map->bins[i];
            if (p && (p != ((void *)1))) {
                free(p);
            }
            map->bins[i] = ((void *)0);
        }
        free(map->bins);
        map->bins = ((void *)0);
    }
    map->fill = 0;
    map->capacity = 0;
}
static void pToken_pSAMap_for_each(pToken_pSAMap_map * map, int (*handle_item)(void * data, pToken key, pSAMap value), void * data) {
    int status = 0;
    size_t i = 0;
    for (i = 0; i < map->capacity; i++) {
        pToken_pSAMap_pair * p = map->bins[i];
        if (p && (p != ((void *)1))) {
            if ((status = handle_item(data, p->key, p->value))) {
                break;
            }
        }
    }
}
static hash_map_err pToken_pSAMap_init(pToken_pSAMap_map * map, size_t init_capacity) {
    if (!init_capacity) {
        init_capacity = 17;
    }
    map->bins = ((void *)0);
    map->capacity = 0;
    map->fill = 0;
    map->_class = &(pToken_pSAMap_class);
    return pToken_pSAMap_resize(map, init_capacity);
}
static hash_map_err pToken_pSAMap_resize(pToken_pSAMap_map * map, size_t new_capacity) {
    if (!new_capacity) {
        pToken_pSAMap_dest(map);
        return HM_SUCCESS;
    }
    pToken_pSAMap_pair ** old_bins = map->bins;
    size_t old_capacity = map->capacity;
    map->bins = calloc(new_capacity, sizeof(*(map->bins)));
    map->fill = 0;
    map->capacity = new_capacity;
    if (!map->bins) {
        return HM_MALLOC_MAP_FAILURE;
    }
    hash_map_err status = HM_SUCCESS;
    for (size_t i = 0; i < old_capacity; i++) {
        if (old_bins[i]) {
            if ((status = pToken_pSAMap_insert(map, old_bins[i]))) {
                return status;
            }
        }
    }
    free(old_bins);
    return HM_SUCCESS;
}
typedef pToken_pSAMap_map PackratCache;
hash_map_err PackratCache_init(PackratCache * cache);
hash_map_err PackratCache_get(PackratCache * cache, Token * tok, ASTNode ** node);
hash_map_err PackratCache_set(PackratCache * cache, Token * tok, ASTNode * node);
void PackratCache_clear_token(PackratCache * cache, Token * tok);
int PackratCache_clear_handle(void * data, Token * tok, SAMap * inner_map);
void PackratCache_dest(PackratCache * cache);
struct Rule {
    struct RuleType * _class;
    rule_id_type id;
    PackratCache cache_;
};
extern struct RuleType {
    Type const * _class;
    Rule * (*new)(int);
    err_type (*init)(Rule * self, int id);
    void (*dest)(Rule * self);
    void (*del)(Rule * self);
    void (*cache_check_)(Rule * self, Parser * parser, size_t token_key, ASTNode * result);
    ASTNode * (*check_cache_)(Rule * self, Parser * parser, size_t token_key);
    void (*clear_cache)(Rule * self, Token * tok);
    ASTNode * (*check_rule_)(Rule * self, Parser * parser, size_t token_key, _Bool disable_cache_check);
    ASTNode * (*check)(Rule * self, Parser * parser, _Bool disable_cache_check);
} Rule_class;
Rule * Rule_new(int id);
err_type Rule_init(Rule * self, int id);
void Rule_dest(Rule * self);
void Rule_del(Rule * self);
void Rule_cache_check_(Rule * self, Parser * parser, size_t token_key, ASTNode * result);
ASTNode * Rule_check_cache_(Rule * self, Parser * parser, size_t token_key);
void Rule_clear_cache(Rule * self, Token * tok);
ASTNode * Rule_check_rule_(Rule * self, Parser * parser, size_t token_key, _Bool disable_cache_check);
ASTNode * Rule_check(Rule * self, Parser * parser, _Bool disable_cache_check);
typedef struct ChainRule ChainRule;
struct ChainRule {
    struct ChainRuleType * _class;
    Rule Rule;
    Rule ** deps;
    size_t deps_size;
};
extern struct ChainRuleType {
    Type const * _class;
    struct RuleType Rule_class;
    ChainRule * (*new)(rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
    err_type (*init)(ChainRule * self, rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
    void (*dest)(ChainRule * self);
    void (*del)(ChainRule * self);
} ChainRule_class;
ChainRule * ChainRule_new(rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
err_type ChainRule_init(ChainRule * self, rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
void ChainRule_dest(ChainRule * self);
void ChainRule_del(ChainRule * self);
void ChainRule_as_Rule_del(Rule * chain_rule);
void ChainRule_clear_cache(Rule * chain_rule, Token * tok);
typedef struct SequenceRule SequenceRule;
struct SequenceRule {
    struct SequenceRuleType * _class;
    ChainRule ChainRule;
};
extern struct SequenceRuleType {
    Type const * _class;
    struct ChainRuleType ChainRule_class;
    SequenceRule * (*new)(rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
    err_type (*init)(SequenceRule * self, rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
    void (*dest)(SequenceRule * self);
    void (*del)(SequenceRule * self);
} SequenceRule_class;
SequenceRule * SequenceRule_new(rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
err_type SequenceRule_init(SequenceRule * self, rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
void SequenceRule_dest(SequenceRule * self);
void SequenceRule_del(SequenceRule * self);
void SequenceRule_as_ChainRule_del(ChainRule * sequence_chain);
void SequenceRule_as_Rule_del(Rule * sequence_rule);
ASTNode * SequenceRule_check_rule_(Rule * sequence_rule, Parser * parser, size_t token_key, _Bool disable_cache_check);
typedef struct ChoiceRule ChoiceRule;
struct ChoiceRule {
    struct ChoiceRuleType * _class;
    ChainRule ChainRule;
};
extern struct ChoiceRuleType {
    Type const * _class;
    struct ChainRuleType ChainRule_class;
    ChoiceRule * (*new)(rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
    err_type (*init)(ChoiceRule * self, rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
    void (*dest)(ChoiceRule * self);
    void (*del)(ChoiceRule * self);
} ChoiceRule_class;
ChoiceRule * ChoiceRule_new(rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
err_type ChoiceRule_init(ChoiceRule * self, rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
void ChoiceRule_dest(ChoiceRule * self);
void ChoiceRule_del(ChoiceRule * self);
void ChoiceRule_as_ChainRule_del(ChainRule * choice_chain);
void ChoiceRule_as_Rule_del(Rule * choice_rule);
ASTNode * ChoiceRule_check_rule_(Rule * choice_rule, Parser * parser, size_t token_key, _Bool disable_cache_check);
typedef struct LiteralRule LiteralRule;
struct LiteralRule {
    struct LiteralRuleType * _class;
    Rule Rule;
    char const * regex_s;
    regex_t regex;
    regmatch_t match[1];
    _Bool compiled;
};
extern struct LiteralRuleType {
    Type const * _class;
    struct RuleType Rule_class;
    LiteralRule * (*new)(rule_id_type id, char const * regex_s);
    err_type (*init)(LiteralRule * self, rule_id_type id, char const * regex_s);
    err_type (*compile)(LiteralRule * self);
    void (*dest)(LiteralRule * self);
    void (*del)(LiteralRule * self);
} LiteralRule_class;
LiteralRule * LiteralRule_new(rule_id_type id, char const * regex_s);
err_type LiteralRule_init(LiteralRule * self, rule_id_type id, char const * regex_s);
void LiteralRule_dest(LiteralRule * self);
void LiteralRule_del(LiteralRule * self);
err_type LiteralRule_compile_regex(LiteralRule * self);
void LiteralRule_as_Rule_del(Rule * literal_rule);
void LiteralRule_as_Rule_dest(Rule * literal_rule);
ASTNode * LiteralRule_check_rule_(Rule * literal_rule, Parser * parser, size_t token_key, _Bool disable_cache_check);
typedef struct NamedProduction NamedProduction;
struct NamedProduction {
    struct NamedProductionType * _class;
    Rule Rule;
};
extern struct NamedProductionType {
    Type const * _class;
    struct RuleType Rule_class;
    NamedProduction * (*new)(rule_id_type id);
    err_type (*init)(NamedProduction * self, rule_id_type id);
    void (*dest)(NamedProduction * self);
    void (*del)(NamedProduction * self);
} NamedProduction_class;
NamedProduction * NamedProduction_new(rule_id_type id);
err_type NamedProduction_init(NamedProduction * self, rule_id_type id);
void NamedProduction_dest(NamedProduction * self);
void NamedProduction_del(NamedProduction * self);
void NamedProduction_as_Rule_del(Rule * named_production_rule);
typedef struct DerivedRule DerivedRule;
struct DerivedRule {
    struct DerivedRuleType * _class;
    Rule Rule;
    Rule * rule;
};
extern struct DerivedRuleType {
    Type const * _class;
    struct RuleType Rule_class;
    DerivedRule * (*new)(rule_id_type id, Rule * rule);
    err_type (*init)(DerivedRule * self, rule_id_type id, Rule * rule);
    void (*dest)(DerivedRule * self);
    void (*del)(DerivedRule * self);
} DerivedRule_class;
DerivedRule * DerivedRule_new(rule_id_type id, Rule * rule);
err_type DerivedRule_init(DerivedRule * self, rule_id_type id, Rule * rule);
void DerivedRule_dest(DerivedRule * self);
void DerivedRule_del(DerivedRule * self);
void DerivedRule_as_Rule_del(Rule * derived_rule);
void DerivedRule_clear_cache(Rule * derived_rule, Token * tok);
typedef struct ListRule ListRule;
struct ListRule {
    struct ListRuleType * _class;
    DerivedRule DerivedRule;
    Rule * delim;
};
extern struct ListRuleType {
    Type const * _class;
    struct DerivedRuleType DerivedRule_class;
    ListRule * (*new)(rule_id_type id, Rule * rule, Rule * delim);
    err_type (*init)(ListRule * self, rule_id_type id, Rule * rule, Rule * delim);
    void (*dest)(ListRule * self);
    void (*del)(ListRule * self);
} ListRule_class;
ListRule * ListRule_new(rule_id_type id, Rule * rule, Rule * delim);
err_type ListRule_init(ListRule * self, rule_id_type id, Rule * rule, Rule * delim);
void ListRule_dest(ListRule * self);
void ListRule_del(ListRule * self);
void ListRule_as_DerivedRule_del(DerivedRule * list_rule);
void ListRule_as_Rule_del(Rule * list_rule);
ASTNode * ListRule_check_rule_(Rule * list_rule, Parser * parser, size_t token_key, _Bool disable_cache_check);
void ListRule_clear_cache(Rule * list_rule, Token * tok);
typedef struct RepeatRule RepeatRule;
struct RepeatRule {
    struct RepeatRuleType * _class;
    DerivedRule DerivedRule;
    size_t min_rep;
    size_t max_rep;
};
extern struct RepeatRuleType {
    Type const * _class;
    struct DerivedRuleType DerivedRule_class;
    RepeatRule * (*new)(rule_id_type id, Rule * rule, size_t min_rep, size_t max_rep);
    err_type (*init)(RepeatRule * self, rule_id_type id, Rule * rule, size_t min_rep, size_t max_rep);
    void (*dest)(RepeatRule * self);
    void (*del)(RepeatRule * self);
} RepeatRule_class;
RepeatRule * RepeatRule_new(rule_id_type id, Rule * rule, size_t min_rep, size_t max_rep);
err_type RepeatRule_init(RepeatRule * selfp, rule_id_type id, Rule * rule, size_t min_rep, size_t max_rep);
void RepeatRule_dest(RepeatRule * self);
void RepeatRule_del(RepeatRule * self);
void RepeatRule_as_DerivedRule_del(DerivedRule * repeat_rule);
void RepeatRule_as_Rule_del(Rule * repeat_rule);
ASTNode * RepeatRule_check_rule_(Rule * repeat_rule, Parser * parser, size_t token_key, _Bool disable_cache_check);
typedef struct OptionalRule OptionalRule;
struct OptionalRule {
    struct OptionalRuleType * _class;
    DerivedRule DerivedRule;
};
extern struct OptionalRuleType {
    Type const * _class;
    struct DerivedRuleType DerivedRule_class;
    OptionalRule * (*new)(rule_id_type id, Rule * rule);
    err_type (*init)(OptionalRule * self, rule_id_type id, Rule * rule);
    void (*dest)(OptionalRule * self);
    void (*del)(OptionalRule * self);
} OptionalRule_class;
OptionalRule * OptionalRule_new(rule_id_type id, Rule * rule);
err_type OptionalRule_init(OptionalRule * self, rule_id_type id, Rule * rule);
void OptionalRule_dest(OptionalRule * self);
void OptionalRule_del(OptionalRule * self);
void OptionalRule_as_DerivedRule_del(DerivedRule * optional_rule);
void OptionalRule_as_Rule_del(Rule * optional_rule);
ASTNode * OptionalRule_check_rule_(Rule * optional_rule, Parser * parser, size_t token_key, _Bool disable_cache_check);
typedef struct NegativeLookahead NegativeLookahead;
struct NegativeLookahead {
    struct NegativeLookaheadType * _class;
    DerivedRule DerivedRule;
};
extern struct NegativeLookaheadType {
    Type const * _class;
    struct DerivedRuleType DerivedRule_class;
    NegativeLookahead * (*new)(rule_id_type id, Rule * rule);
    err_type (*init)(NegativeLookahead * self, rule_id_type id, Rule * rule);
    void (*dest)(NegativeLookahead * self);
    void (*del)(NegativeLookahead * self);
} NegativeLookahead_class;
NegativeLookahead * NegativeLookahead_new(rule_id_type id, Rule * rule);
err_type NegativeLookahead_init(NegativeLookahead * self, rule_id_type id, Rule * rule);
void NegativeLookahead_dest(NegativeLookahead * self);
void NegativeLookahead_del(NegativeLookahead * self);
void NegativeLookahead_as_DerivedRule_del(DerivedRule * negative_lookahead);
void NegativeLookahead_as_Rule_del(Rule * negative_lookahead);
ASTNode * NegativeLookahead_check_rule_(Rule * negative_lookahead, Parser * parser, size_t token_key, _Bool disable_cache_check);
typedef struct PositiveLookahead PositiveLookahead;
struct PositiveLookahead {
    struct PositiveLookaheadType * _class;
    DerivedRule DerivedRule;
};
extern struct PositiveLookaheadType {
    Type const * _class;
    struct DerivedRuleType DerivedRule_class;
    PositiveLookahead * (*new)(rule_id_type id, Rule * rule);
    err_type (*init)(PositiveLookahead * self, rule_id_type id, Rule * rule);
    void (*dest)(PositiveLookahead * self);
    void (*del)(PositiveLookahead * self);
} PositiveLookahead_class;
PositiveLookahead * PositiveLookahead_new(rule_id_type id, Rule * rule);
err_type PositiveLookahead_init(PositiveLookahead * self, rule_id_type id, Rule * rule);
void PositiveLookahead_dest(PositiveLookahead * self);
void PositiveLookahead_del(PositiveLookahead * self);
void PositiveLookahead_as_DerivedRule_del(DerivedRule * positive_lookahead);
void PositiveLookahead_as_Rule_del(Rule * positive_lookahead);
ASTNode * PositiveLookahead_check_rule_(Rule * positive_lookahead, Parser * parser, size_t token_key, _Bool disable_cache_check);
typedef struct AnonymousProduction AnonymousProduction;
struct AnonymousProduction {
    struct AnonymousProductionType * _class;
    DerivedRule DerivedRule;
};
extern struct AnonymousProductionType {
    Type const * _class;
    struct DerivedRuleType DerivedRule_class;
    AnonymousProduction * (*new)(rule_id_type id, Rule * rule);
    err_type (*init)(AnonymousProduction * self, rule_id_type id, Rule * rule);
    void (*dest)(AnonymousProduction * self);
    void (*del)(AnonymousProduction * self);
} AnonymousProduction_class;
AnonymousProduction * AnonymousProduction_new(rule_id_type id, Rule * rule);
err_type AnonymousProduction_init(AnonymousProduction * self, rule_id_type id, Rule * rule);
void AnonymousProduction_dest(AnonymousProduction * self);
void AnonymousProduction_del(AnonymousProduction * self);
void AnonymousProduction_as_DerivedRule_del(DerivedRule * anonymous_production);
void AnonymousProduction_as_Rule_del(Rule * anonymous_production);
ASTNode * build_action_default(Parser * parser, ASTNode * node);
typedef ASTNode * (*build_action_ftype)(Parser * parser, ASTNode *node);
typedef struct Production Production;
struct Production {
    struct ProductionType * _class;
    AnonymousProduction AnonymousProduction;
    build_action_ftype build_action;
};
extern struct ProductionType {
    Type const * _class;
    struct AnonymousProductionType AnonymousProduction_class;
    Production * (*new)(rule_id_type id, Rule * rule, build_action_ftype build_action);
    err_type (*init)(Production * self, rule_id_type id, Rule * rule, build_action_ftype build_action);
    void (*dest)(Production * self);
    void (*del)(Production * self);
} Production_class;
Production * Production_new(rule_id_type id, Rule * rule, build_action_ftype build_action);
err_type Production_init(Production * self, rule_id_type id, Rule * rule, build_action_ftype build_action);
void Production_dest(Production * self);
void Production_del(Production * self);
void Production_as_AnonymousProduction_del(AnonymousProduction * production);
void Production_as_DerivedRule_del(DerivedRule * production);
void Production_as_Rule_del(Rule * production);
ASTNode * Production_check_rule_(Rule * production, Parser * parser, size_t token_key, _Bool disable_cache_check);
typedef enum seek_origin {
    P_SEEK_SET,
    P_SEEK_CUR,
    P_SEEK_END,
} seek_origin;
struct Parser {
    struct ParserType const * _class;
    Rule * token_rule;
    Rule * root_rule;
    Token ** tokens;
    Token ** tokens_gc;
    ASTNode ** node_list;
    char const * name;
    char * log_buffer;
    size_t log_buffer_length;
    char const * log_file;
    size_t loc_;
    size_t name_length;
    size_t tokens_length;
    size_t tokens_capacity;
    size_t tokens_gc_length;
    size_t tokens_gc_capacity;
    size_t node_list_length;
    size_t node_list_capacity;
    _Bool disable_cache_check;
    ASTNode * ast;
};
extern struct ParserType {
    Type const * _class;
    Parser * (*new)(char const * name, size_t name_length, char const * string, size_t string_length, Rule * token_rule,
                    Rule * root_rule, unsigned int line_offset,
                    unsigned int col_offset, _Bool lazy_parse,
                    char const * log_file);
    err_type (*init)(Parser * parser, char const * name, size_t name_length,
                         char const * string, size_t string_length, Rule * token_rule,
                         Rule * root_rule, unsigned int line_offset,
                         unsigned int col_offset, _Bool lazy_parse,
                         char const * log_file);
    void (*dest)(Parser * parser);
    void (*del)(Parser * parser);
    size_t (*tell)(Parser * parser);
    void (*seek)(Parser * parser, long long loc, seek_origin origin);
    void (*log)(Parser * parser, size_t loc, Rule * rule, ASTNode * result);
    void (*log_check_fail_)(Parser * parser, size_t loc, Rule * rule);
    void (*get_line_col_end)(Parser * parser, Token * tok, unsigned int * line_out, unsigned int * col_out);
    Token * (*gen_final_token_)(Parser * parser, ASTNode * node);
    err_type (*skip_token)(Parser * parser, ASTNode * node);
    err_type (*add_token)(Parser * parser, ASTNode * node);
    err_type (*add_node)(Parser * parser, ASTNode * node);
    _Bool (*gen_next_token_)(Parser * parser);
    err_type (*get)(Parser * parser, size_t key, Token ** tok);
    Token ** (*get_tokens)(Parser * parser, ASTNode * node, size_t * ntokens);
    void (*parse)(Parser * parser);
    err_type (*traverse)(Parser * parser, void (*traverse_action)(void * ctxt, ASTNode * node), void * ctxt);
    void (*print_ast)(Parser * parser, char * buffer, size_t buffer_size);
} Parser_class;
extern Type const Parser_TYPE;
Parser * Parser_new(char const * name, size_t name_length, char const * string, size_t string_length, Rule * token_rule,
                    Rule * root_rule, unsigned int line_offset, unsigned intcol_offset,
                    _Bool lazy_parse, char const * log_file);
err_type Parser_init(Parser * parser, char const * name, size_t name_length,
                         char const * string, size_t string_length, Rule * token_rule,
                         Rule * root_rule, unsigned int line_offset,
                         unsigned int col_offset, _Bool lazy_parse,
                         char const * log_file);
void Parser_dest(Parser * parser);
void Parser_del(Parser * parser);
size_t Parser_tell(Parser * parser);
void Parser_seek(Parser * parser, long long loc, seek_origin origin);
void Parser_log(Parser * parser, size_t loc, Rule * rule, ASTNode * result);
void Parser_log_check_fail_(Parser * parser, size_t loc, Rule * rule);
void Parser_get_line_col_end(Parser * parser, Token * tok, unsigned int * line_out, unsigned int * col_out);
Token * Parser_gen_final_token_(Parser * parser, ASTNode * node);
err_type Parser_skip_token(Parser * parser, ASTNode * node);
err_type Parser_add_token(Parser * parser, ASTNode * node);
err_type Parser_add_node(Parser * parser, ASTNode * node);
_Bool Parser_gen_next_token_(Parser * parser);
err_type Parser_get(Parser * parser, size_t key, Token ** tok);
Token ** Parser_get_tokens(Parser * parser, ASTNode * node, size_t * ntokens);
void Parser_parse(Parser * parser);
err_type Parser_traverse(Parser * parser, void (*traverse_action)(void * ctxt, ASTNode * node), void * ctxt);
void Parser_print_ast(Parser * parser, char * buffer, size_t buffer_size);
ASTNode * skip_token(Parser * parser, ASTNode * node);
ASTNode * token_action(Parser * parser, ASTNode * node);
typedef enum jsonrule {
 PUNCTUATOR,
 COMMA,
 LBRACE,
 RBRACE,
 LBRACKET,
 RBRACKET,
 COLON,
 KEYWORD,
 TRUE_KW,
 FALSE_KW,
 NULL_KW,
 JSON,
 REP_1_0_12,
 TOKEN,
 CHOICE_5_15,
 WHITESPACE,
 WHITESPACE_RE,
 STRING,
 STRING_RE,
 INT_CONSTANT,
 INT_CONSTANT_RE,
 DECIMAL_FLOAT_CONSTANT,
 DECIMAL_FLOAT_CONSTANT_RE,
 NUMBER,
 CHOICE_2_25,
 VALUE,
 CHOICE_5_26,
 OBJECT,
 SEQ_3_29,
 REP_0_1_30,
 MEMBERS,
 LIST_2_32,
 MEMBER,
 SEQ_3_34,
 ARRAY,
 SEQ_3_35,
 REP_0_1_36,
 ELEMENTS,
 LIST_2_38,
 JSON_NRULES
} jsonrule;
extern Production json_token;
extern Production json_json;
void json_dest(void);
ASTNode * handle_json(Parser * parser, ASTNode * node);
LiteralRule json_punctuator;
LiteralRule comma;
LiteralRule lbrace;
LiteralRule rbrace;
LiteralRule lbracket;
LiteralRule rbracket;
LiteralRule colon;
LiteralRule json_keyword;
LiteralRule true_kw;
LiteralRule false_kw;
LiteralRule null_kw;
Production json_json;
RepeatRule json_rep_1_0_12;
Production json_token;
ChoiceRule json_choice_5_15;
Production json_whitespace;
LiteralRule json_whitespace_re;
Production json_string;
LiteralRule json_string_re;
Production json_int_constant;
LiteralRule json_int_constant_re;
Production json_decimal_float_constant;
LiteralRule json_decimal_float_constant_re;
Production json_number;
ChoiceRule json_choice_2_25;
Production json_value;
ChoiceRule json_choice_5_26;
Production json_object;
SequenceRule json_seq_3_29;
RepeatRule json_rep_0_1_30;
Production json_members;
ListRule json_list_2_32;
Production json_member;
SequenceRule json_seq_3_34;
Production json_array;
SequenceRule json_seq_3_35;
RepeatRule json_rep_0_1_36;
Production json_elements;
ListRule json_list_2_38;
Production json_value = { ._class = &Production_class, .AnonymousProduction = { ._class = &(Production_class.AnonymousProduction_class), .DerivedRule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class), .Rule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class), .id = VALUE, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .rule = &json_choice_5_26.ChainRule.Rule } }, .build_action = &build_action_default };
LiteralRule rbrace = { ._class = &LiteralRule_class, .Rule = { ._class = &(LiteralRule_class.Rule_class), .id = RBRACE, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .regex_s = "^(})", .compiled = 0 };
Production json_member = { ._class = &Production_class, .AnonymousProduction = { ._class = &(Production_class.AnonymousProduction_class), .DerivedRule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class), .Rule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class), .id = MEMBER, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .rule = &json_seq_3_34.ChainRule.Rule } }, .build_action = &build_action_default };
Production json_int_constant = { ._class = &Production_class, .AnonymousProduction = { ._class = &(Production_class.AnonymousProduction_class), .DerivedRule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class), .Rule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class), .id = INT_CONSTANT, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .rule = &json_int_constant_re.Rule } }, .build_action = &build_action_default };
LiteralRule lbracket = { ._class = &LiteralRule_class, .Rule = { ._class = &(LiteralRule_class.Rule_class), .id = LBRACKET, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .regex_s = "^(\\[)", .compiled = 0 };
RepeatRule json_rep_0_1_30 = { ._class = &RepeatRule_class, .DerivedRule = { ._class = &(RepeatRule_class.DerivedRule_class), .Rule = { ._class = &(RepeatRule_class.DerivedRule_class.Rule_class), .id = REP_0_1_30, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .rule = &json_members.AnonymousProduction.DerivedRule.Rule }, .min_rep = 0, .max_rep = 1 };
Production json_decimal_float_constant = { ._class = &Production_class, .AnonymousProduction = { ._class = &(Production_class.AnonymousProduction_class), .DerivedRule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class), .Rule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class), .id = DECIMAL_FLOAT_CONSTANT, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .rule = &json_decimal_float_constant_re.Rule } }, .build_action = &build_action_default };
LiteralRule true_kw = { ._class = &LiteralRule_class, .Rule = { ._class = &(LiteralRule_class.Rule_class), .id = TRUE_KW, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .regex_s = "^(true)", .compiled = 0 };
ChoiceRule json_choice_2_25 = { ._class = &ChoiceRule_class, .ChainRule = { ._class = &(ChoiceRule_class.ChainRule_class), .Rule = { ._class = &(ChoiceRule_class.ChainRule_class.Rule_class), .id = CHOICE_2_25, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .deps = (Rule *[2]) {&json_decimal_float_constant.AnonymousProduction.DerivedRule.Rule, &json_int_constant.AnonymousProduction.DerivedRule.Rule}, .deps_size = 2 } };
LiteralRule json_decimal_float_constant_re = { ._class = &LiteralRule_class, .Rule = { ._class = &(LiteralRule_class.Rule_class), .id = DECIMAL_FLOAT_CONSTANT_RE, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .regex_s = "^([-+]?(((([0-9]*\\.[0-9]+)|([0-9]+\\.))([eE][-+]?[0-9]+)?)|([0-9]+[eE][-+]?[0-9]+)))", .compiled = 0 };
RepeatRule json_rep_0_1_36 = { ._class = &RepeatRule_class, .DerivedRule = { ._class = &(RepeatRule_class.DerivedRule_class), .Rule = { ._class = &(RepeatRule_class.DerivedRule_class.Rule_class), .id = REP_0_1_36, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .rule = &json_elements.AnonymousProduction.DerivedRule.Rule }, .min_rep = 0, .max_rep = 1 };
LiteralRule json_punctuator = { ._class = &LiteralRule_class, .Rule = { ._class = &(LiteralRule_class.Rule_class), .id = PUNCTUATOR, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .regex_s = "^((,)|(\\{)|(})|(\\[)|(])|(:))", .compiled = 0 };
LiteralRule json_int_constant_re = { ._class = &LiteralRule_class, .Rule = { ._class = &(LiteralRule_class.Rule_class), .id = INT_CONSTANT_RE, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .regex_s = "^([-+]?(([1-9][0-9]*)|(0(x|X)[A-Fa-f0-9]+)|(0[0-7]*)))", .compiled = 0 };
SequenceRule json_seq_3_29 = { ._class = &SequenceRule_class, .ChainRule = { ._class = &(SequenceRule_class.ChainRule_class), .Rule = { ._class = &(SequenceRule_class.ChainRule_class.Rule_class), .id = SEQ_3_29, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .deps = (Rule *[3]) {&lbrace.Rule, &json_rep_0_1_30.DerivedRule.Rule, &rbrace.Rule}, .deps_size = 3 } };
Production json_object = { ._class = &Production_class, .AnonymousProduction = { ._class = &(Production_class.AnonymousProduction_class), .DerivedRule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class), .Rule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class), .id = OBJECT, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .rule = &json_seq_3_29.ChainRule.Rule } }, .build_action = &build_action_default };
LiteralRule json_string_re = { ._class = &LiteralRule_class, .Rule = { ._class = &(LiteralRule_class.Rule_class), .id = STRING_RE, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .regex_s = "^(\"((\\\\\")|[^\"])*\")", .compiled = 0 };
Production json_string = { ._class = &Production_class, .AnonymousProduction = { ._class = &(Production_class.AnonymousProduction_class), .DerivedRule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class), .Rule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class), .id = STRING, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .rule = &json_string_re.Rule } }, .build_action = &build_action_default };
LiteralRule null_kw = { ._class = &LiteralRule_class, .Rule = { ._class = &(LiteralRule_class.Rule_class), .id = NULL_KW, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .regex_s = "^(null)", .compiled = 0 };
LiteralRule colon = { ._class = &LiteralRule_class, .Rule = { ._class = &(LiteralRule_class.Rule_class), .id = COLON, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .regex_s = "^(:)", .compiled = 0 };
LiteralRule json_keyword = { ._class = &LiteralRule_class, .Rule = { ._class = &(LiteralRule_class.Rule_class), .id = KEYWORD, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .regex_s = "^((true)|(false)|(null))", .compiled = 0 };
Production json_members = { ._class = &Production_class, .AnonymousProduction = { ._class = &(Production_class.AnonymousProduction_class), .DerivedRule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class), .Rule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class), .id = MEMBERS, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .rule = &json_list_2_32.DerivedRule.Rule } }, .build_action = &build_action_default };
LiteralRule rbracket = { ._class = &LiteralRule_class, .Rule = { ._class = &(LiteralRule_class.Rule_class), .id = RBRACKET, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .regex_s = "^(])", .compiled = 0 };
Production json_whitespace = { ._class = &Production_class, .AnonymousProduction = { ._class = &(Production_class.AnonymousProduction_class), .DerivedRule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class), .Rule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class), .id = WHITESPACE, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .rule = &json_whitespace_re.Rule } }, .build_action = &skip_token };
LiteralRule json_whitespace_re = { ._class = &LiteralRule_class, .Rule = { ._class = &(LiteralRule_class.Rule_class), .id = WHITESPACE_RE, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .regex_s = "^([[:space:]]+)", .compiled = 0 };
Production json_token = { ._class = &Production_class, .AnonymousProduction = { ._class = &(Production_class.AnonymousProduction_class), .DerivedRule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class), .Rule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class), .id = TOKEN, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .rule = &json_choice_5_15.ChainRule.Rule } }, .build_action = &token_action };
ListRule json_list_2_32 = { ._class = &ListRule_class, .DerivedRule = { ._class = &(ListRule_class.DerivedRule_class), .Rule = { ._class = &(ListRule_class.DerivedRule_class.Rule_class), .id = LIST_2_32, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .rule = &json_member.AnonymousProduction.DerivedRule.Rule }, .delim = &comma.Rule };
ChoiceRule json_choice_5_15 = { ._class = &ChoiceRule_class, .ChainRule = { ._class = &(ChoiceRule_class.ChainRule_class), .Rule = { ._class = &(ChoiceRule_class.ChainRule_class.Rule_class), .id = CHOICE_5_15, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .deps = (Rule *[5]) {&json_whitespace.AnonymousProduction.DerivedRule.Rule, &json_string.AnonymousProduction.DerivedRule.Rule, &json_keyword.Rule, &json_number.AnonymousProduction.DerivedRule.Rule, &json_punctuator.Rule}, .deps_size = 5 } };
LiteralRule lbrace = { ._class = &LiteralRule_class, .Rule = { ._class = &(LiteralRule_class.Rule_class), .id = LBRACE, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .regex_s = "^(\\{)", .compiled = 0 };
LiteralRule comma = { ._class = &LiteralRule_class, .Rule = { ._class = &(LiteralRule_class.Rule_class), .id = COMMA, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .regex_s = "^(,)", .compiled = 0 };
RepeatRule json_rep_1_0_12 = { ._class = &RepeatRule_class, .DerivedRule = { ._class = &(RepeatRule_class.DerivedRule_class), .Rule = { ._class = &(RepeatRule_class.DerivedRule_class.Rule_class), .id = REP_1_0_12, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .rule = &json_value.AnonymousProduction.DerivedRule.Rule }, .min_rep = 1, .max_rep = 0 };
Production json_json = { ._class = &Production_class, .AnonymousProduction = { ._class = &(Production_class.AnonymousProduction_class), .DerivedRule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class), .Rule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class), .id = JSON, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .rule = &json_rep_1_0_12.DerivedRule.Rule } }, .build_action = &handle_json };
ChoiceRule json_choice_5_26 = { ._class = &ChoiceRule_class, .ChainRule = { ._class = &(ChoiceRule_class.ChainRule_class), .Rule = { ._class = &(ChoiceRule_class.ChainRule_class.Rule_class), .id = CHOICE_5_26, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .deps = (Rule *[5]) {&json_object.AnonymousProduction.DerivedRule.Rule, &json_array.AnonymousProduction.DerivedRule.Rule, &json_keyword.Rule, &json_string.AnonymousProduction.DerivedRule.Rule, &json_number.AnonymousProduction.DerivedRule.Rule}, .deps_size = 5 } };
SequenceRule json_seq_3_35 = { ._class = &SequenceRule_class, .ChainRule = { ._class = &(SequenceRule_class.ChainRule_class), .Rule = { ._class = &(SequenceRule_class.ChainRule_class.Rule_class), .id = SEQ_3_35, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .deps = (Rule *[3]) {&lbracket.Rule, &json_rep_0_1_36.DerivedRule.Rule, &rbracket.Rule}, .deps_size = 3 } };
Production json_elements = { ._class = &Production_class, .AnonymousProduction = { ._class = &(Production_class.AnonymousProduction_class), .DerivedRule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class), .Rule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class), .id = ELEMENTS, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .rule = &json_list_2_38.DerivedRule.Rule } }, .build_action = &build_action_default };
SequenceRule json_seq_3_34 = { ._class = &SequenceRule_class, .ChainRule = { ._class = &(SequenceRule_class.ChainRule_class), .Rule = { ._class = &(SequenceRule_class.ChainRule_class.Rule_class), .id = SEQ_3_34, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .deps = (Rule *[3]) {&json_string.AnonymousProduction.DerivedRule.Rule, &colon.Rule, &json_value.AnonymousProduction.DerivedRule.Rule}, .deps_size = 3 } };
Production json_array = { ._class = &Production_class, .AnonymousProduction = { ._class = &(Production_class.AnonymousProduction_class), .DerivedRule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class), .Rule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class), .id = ARRAY, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .rule = &json_seq_3_35.ChainRule.Rule } }, .build_action = &build_action_default };
LiteralRule false_kw = { ._class = &LiteralRule_class, .Rule = { ._class = &(LiteralRule_class.Rule_class), .id = FALSE_KW, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .regex_s = "^(false)", .compiled = 0 };
ListRule json_list_2_38 = { ._class = &ListRule_class, .DerivedRule = { ._class = &(ListRule_class.DerivedRule_class), .Rule = { ._class = &(ListRule_class.DerivedRule_class.Rule_class), .id = LIST_2_38, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .rule = &json_value.AnonymousProduction.DerivedRule.Rule }, .delim = &comma.Rule };
Production json_number = { ._class = &Production_class, .AnonymousProduction = { ._class = &(Production_class.AnonymousProduction_class), .DerivedRule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class), .Rule = { ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class), .id = NUMBER, .cache_ = { .bins = ((void *)0), .capacity = 0, .fill = 0, ._class = &pToken_pSAMap_class, } }, .rule = &json_choice_2_25.ChainRule.Rule } }, .build_action = &build_action_default };
Rule * jsonrules[JSON_NRULES + 1] = {
 &json_value.AnonymousProduction.DerivedRule.Rule,
 &rbrace.Rule,
 &json_member.AnonymousProduction.DerivedRule.Rule,
 &json_int_constant.AnonymousProduction.DerivedRule.Rule,
 &lbracket.Rule,
 &json_rep_0_1_30.DerivedRule.Rule,
 &json_decimal_float_constant.AnonymousProduction.DerivedRule.Rule,
 &true_kw.Rule,
 &json_choice_2_25.ChainRule.Rule,
 &json_decimal_float_constant_re.Rule,
 &json_rep_0_1_36.DerivedRule.Rule,
 &json_punctuator.Rule,
 &json_int_constant_re.Rule,
 &json_seq_3_29.ChainRule.Rule,
 &json_object.AnonymousProduction.DerivedRule.Rule,
 &json_string_re.Rule,
 &json_string.AnonymousProduction.DerivedRule.Rule,
 &null_kw.Rule,
 &colon.Rule,
 &json_keyword.Rule,
 &json_members.AnonymousProduction.DerivedRule.Rule,
 &rbracket.Rule,
 &json_whitespace.AnonymousProduction.DerivedRule.Rule,
 &json_whitespace_re.Rule,
 &json_token.AnonymousProduction.DerivedRule.Rule,
 &json_list_2_32.DerivedRule.Rule,
 &json_choice_5_15.ChainRule.Rule,
 &lbrace.Rule,
 &comma.Rule,
 &json_rep_1_0_12.DerivedRule.Rule,
 &json_json.AnonymousProduction.DerivedRule.Rule,
 &json_choice_5_26.ChainRule.Rule,
 &json_seq_3_35.ChainRule.Rule,
 &json_elements.AnonymousProduction.DerivedRule.Rule,
 &json_seq_3_34.ChainRule.Rule,
 &json_array.AnonymousProduction.DerivedRule.Rule,
 &false_kw.Rule,
 &json_list_2_38.DerivedRule.Rule,
 &json_number.AnonymousProduction.DerivedRule.Rule,
 ((void *)0)
};
void json_dest(void) {
 int i = 0;
 while (jsonrules[i]) {
  jsonrules[i]->_class->dest(jsonrules[i]);
  i++;
 }
}
