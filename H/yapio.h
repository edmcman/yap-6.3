/*************************************************************************
*									 *
*	 YAP Prolog 	%W% %G%
*									 *
*	Yap Prolog was developed at NCCUP - Universidade do Porto	 *
*									 *
* Copyright L.Damas, V.S.Costa and Universidade do Porto 1985-1997	 *
*									 *
**************************************************************************
*									 *
* File:		io.h							 *
* Last rev:	19/2/88							 *
* mods:									 *
* comments:	Input/Output information				 *
*									 *
*************************************************************************/


#ifdef SIMICS
#undef HAVE_LIBREADLINE
#endif

#include <stdio.h>

#ifndef YAP_STDIO

#define YP_printf	printf
#define YP_putchar	putchar
#define YP_getc		getc
#define YP_fgetc	fgetc
#define YP_getchar	getchar
#define YP_fgets	fgets
#define YP_clearerr     clearerr
#define YP_feof		feof
#define YP_ferror	ferror
#if _MSC_VER || defined(__MINGW32__)
#define YP_fileno	_fileno
#else
#define YP_fileno	fileno
#endif
#define YP_fopen	fopen
#define YP_fclose	fclose
#define YP_ftell	ftell
#define YP_fseek	fseek
#define YP_setbuf	setbuf
#define YP_fputs	fputs
#define YP_ungetc	ungetc
#define YP_fdopen	fdopen
#define init_yp_stdio()

#if _MSC_VER || defined(__MINGW32__)
#define open _open
#define close _close
#define popen _popen
#define pclose _pclose
#define read _read
#define write _write
#define isatty _isatty
#define putenv(S) _putenv(S)
#define chdir(P) _chdir(P)
#define getcwd(B,S) _getcwd(B,S)
#endif

#define YP_FILE		FILE
extern YP_FILE *Yap_stdin;
extern YP_FILE *Yap_stdout;
extern YP_FILE *Yap_stderr;

int     STD_PROTO(YP_putc,(int, int));

#else

#ifdef putc
#undef putc
#undef getc
#undef putchar
#undef getchar
#undef stdin
#undef stdout
#undef stderr
#endif

#define printf	ERR_printf
#define fprintf ERR_fprintf
#define putchar	ERR_putchar
#define putc	ERR_putc
#define getc	ERR_getc
#define fgetc	ERR_fgetc
#define getchar	ERR_getchar
#define fgets	ERR_fgets
#define clearerr ERR_clearerr
#define feof	ERR_feof
#define ferror	ERR_ferror
#define fileno	ERR_fileno
#define fopen	ERR_fopen
#define fclose	ERR_fclose
#define fflush	ERR_fflush



/* flags for files in IOSTREAM struct */
#define _YP_IO_WRITE	1
#define _YP_IO_READ	2

#define _YP_IO_ERR	0x04
#define _YP_IO_EOF	0x08

#define _YP_IO_FILE	0x10
#define _YP_IO_SOCK	0x20


typedef struct IOSTREAM {
  int   check;
  int	fd;			/* file descriptor 	*/
  int 	flags;
  int   cnt;
  int	buflen;
  char  buf[2];
  char  *ptr;
  char  *base;
  int   (*close)(int fd);	/* close file		*/
  int	(*read)(int fd, char *b, int n); /* read bytes	*/
  int	(*write)(int fd, char *b, int n);/* write bytes */
} YP_FILE;

#define YP_stdin    &yp_iob[0]
#define YP_stdout   &yp_iob[1]
#define YP_stderr   &yp_iob[2]



#define YP_getc(f)	(--(f)->cnt < 0 ? YP_fillbuf(f) :  *((unsigned char *) ((f)->ptr++)))
#define YP_fgetc(f)	YP_fgetc(f)
#define YP_putc(c,f)	(--(f)->cnt < 0 ? YP_flushbuf(c,f) : (unsigned char) (*(f)->ptr++ = (char) c))
#define YP_putchar(cc)	YP_putc(cc,YP_stdout)
#define YP_getchar()	YP_getc(YP_stdin)

int YP_fillbuf(YP_FILE *f);
int YP_flushbuf(int c, YP_FILE *f);

int YP_printf(char *, ...);
int YP_fprintf(YP_FILE *, char *, ...);
char* YP_fgets(char *, int, YP_FILE *);
char* YP_gets(char *);
YP_FILE *YP_fopen(char *, char *);
int YP_fclose(YP_FILE *);
int YP_fileno(YP_FILE *);
int YP_fflush(YP_FILE *);
int YP_feof(YP_FILE *);
int YP_ftell(YP_FILE *);
int YP_fseek(YP_FILE *, int, int);
int YP_clearerr(YP_FILE *);
void init_yp_stdio(void);
int YP_fputs(char *s, YP_FILE *f);
int YP_puts(char *s);
int YP_setbuf(YP_FILE *f, char *buf);


#define YP_MAX_FILES 40

extern YP_FILE yp_iob[YP_MAX_FILES];

#endif /* YAP_STDIO */

#if defined(FILENAME_MAX) && !defined(__hpux)
#define YAP_FILENAME_MAX FILENAME_MAX
#else
#define YAP_FILENAME_MAX 1024 /* This is ok for Linux, should be ok for everyone */
#endif

extern char Yap_FileNameBuf[YAP_FILENAME_MAX], Yap_FileNameBuf2[YAP_FILENAME_MAX];

typedef YP_FILE *YP_File;

enum TokenKinds {
  Name_tok,
  Number_tok,
  Var_tok,
  String_tok,
  Ponctuation_tok,
  Error_tok,
  eot_tok
};

typedef	 struct	TOKEN {
  unsigned char Tok;
  Term TokInfo;
  int	TokPos;
  struct TOKEN *TokNext;
} TokEntry;

#define	Ord(X) ((int) X)

#define	NextToken GNextToken()

typedef	struct VARSTRUCT {
  Term VarAdr;
  CELL hv;
  struct VARSTRUCT *VarLeft, *VarRight;
  char VarRep[1];
} VarEntry;

/* Character types for tokenizer and write.c */

#define UC      1       /* Upper case */
#define UL      2       /* Underline */
#define LC      3       /* Lower case */
#define NU      4       /* digit */
#define	QT	5	/* single quote */
#define	DC	6	/* double quote */
#define SY      7       /* Symbol character */
#define SL      8       /* Solo character */
#define BK      9       /* Brackets & friends */
#define BS      10      /* Blank */
#define EF	11	/* End of File marker */
#define CC	12	/* comment char %	*/

#define EOFCHAR EOF

#if USE_SOCKET
/****************** defines for sockets *********************************/

typedef enum{        /* in YAP, sockets may be in one of 4 possible status */
  new_socket,
    server_socket,
    client_socket,
    server_session_socket,
    closed_socket
} socket_info;

typedef enum{       /* we accept two domains for the moment, IPV6 may follow */
      af_inet,      /* IPV4 */
      af_unix       /* or AF_FILE */
} socket_domain;

Term  STD_PROTO(Yap_InitSocketStream,(int, socket_info, socket_domain));
int   STD_PROTO(Yap_CheckSocketStream,(Term, char *));
socket_domain   STD_PROTO(Yap_GetSocketDomain,(int));
socket_info   STD_PROTO(Yap_GetSocketStatus,(int));
void  STD_PROTO(Yap_UpdateSocketStream,(int, socket_info, socket_domain));

/* routines in ypsocks.c */
Int STD_PROTO(Yap_CloseSocket,(int, socket_info, socket_domain));

#endif /* USE_SOCKET */

/* info on aliases */
typedef struct AliasDescS {
    Atom name;
    int alias_stream;
} * AliasDesc;

/****************** character definition table **************************/
#define NUMBER_OF_CHARS 256
extern char *Yap_chtype;

/*************** variables concerned with parsing   *********************/
extern TokEntry	*Yap_tokptr, *Yap_toktide;
extern VarEntry	*Yap_VarTable, *Yap_AnonVarTable;
extern int Yap_eot_before_eof;

/* parser stack, used to be AuxSp, now is ASP */
#define ParserAuxSp (TR)

/* routines in parser.c */
VarEntry STD_PROTO(*Yap_LookupVar,(char *));
Term STD_PROTO(Yap_VarNames,(VarEntry *,Term));

/* routines ins scanner.c */
TokEntry STD_PROTO(*Yap_tokenizer,(int));
Term     STD_PROTO(Yap_scan_num,(int (*)(int)));
char	 STD_PROTO(*Yap_AllocScannerMemory,(unsigned int));

/* routines in iopreds.c */
Int   STD_PROTO(Yap_FirstLineInParse,(void));
int   STD_PROTO(Yap_CheckIOStream,(Term, char *));
int   STD_PROTO(Yap_GetStreamFd,(int));
void  STD_PROTO(Yap_CloseStreams,(int));
void  STD_PROTO(Yap_CloseStream,(int));
int   STD_PROTO(Yap_PlGetchar,(void));
int   STD_PROTO(Yap_PlFGetchar,(void));
int   STD_PROTO(Yap_GetCharForSIGINT,(void));
int   STD_PROTO(Yap_StreamToFileNo,(Term));
Term  STD_PROTO(Yap_OpenStream,(FILE *,char *,Term,int));

extern int
  Yap_c_input_stream,
  Yap_c_output_stream,
  Yap_c_error_stream;

#define YAP_INPUT_STREAM	0x01
#define YAP_OUTPUT_STREAM	0x02
#define YAP_APPEND_STREAM	0x04
#define YAP_PIPE_STREAM 	0x08
#define YAP_TTY_STREAM	 	0x10
#define YAP_POPEN_STREAM	0x20
#define YAP_BINARY_STREAM	0x40
#define YAP_SEEKABLE_STREAM	0x80


#define	Quote_illegal_f		1
#define	Ignore_ops_f		2
#define	Handle_vars_f		4
#define	Use_portray_f		8

/* write.c */
void	STD_PROTO(Yap_plwrite,(Term,int (*)(int, int),int));

/* grow.c */
int  STD_PROTO(Yap_growstack_in_parser,  (tr_fr_ptr *, TokEntry **, VarEntry **));



#if HAVE_ERRNO_H
#include <errno.h>
#else
extern int errno;
#endif

#if DEBUG
#if COROUTINING
extern int  Yap_Portray_delays;
#endif
#endif

#define HashFunction(CHP,OUT) { (OUT)=0; while(*(CHP) != '\0') (OUT) += *(CHP)++; (OUT) %= MaxHash; }

#define FAIL_ON_PARSER_ERROR      0
#define QUIET_ON_PARSER_ERROR     1
#define CONTINUE_ON_PARSER_ERROR  2
#define EXCEPTION_ON_PARSER_ERROR 3

extern jmp_buf Yap_IOBotch;

#ifdef DEBUG
extern YP_FILE *Yap_logfile;
#endif

#if USE_SOCKET
extern int Yap_sockets_io;
#endif

