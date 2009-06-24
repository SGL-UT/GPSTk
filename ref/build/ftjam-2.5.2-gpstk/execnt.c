/*
 * Copyright 1993, 1995 Christopher Seiwald.
 *
 * This file is part of Jam - see jam.c for Copyright information.
 */

# include "jam.h"
# include "lists.h"
# include "execcmd.h"
# include <errno.h>

# ifdef USE_EXECNT

# define WIN32_LEAN_AND_MEAN
# include <windows.h>		/* do the ugly deed */
# include <process.h>

# if !defined( __BORLANDC__ ) && !defined( OS_OS2 )
# define wait my_wait
static int my_wait( int *status );
# endif

/*
 * execnt.c - execute a shell command on Windows NT and Windows 95/98
 *
 * If $(JAMSHELL) is defined, uses that to formulate execvp()/spawnvp().
 * The default is:
 *
 *	/bin/sh -c %		[ on UNIX/AmigaOS ]
 *	cmd.exe /c %		[ on Windows NT ]
 *
 * Each word must be an individual element in a jam variable value.
 *
 * In $(JAMSHELL), % expands to the command string and ! expands to 
 * the slot number (starting at 1) for multiprocess (-j) invocations.
 * If $(JAMSHELL) doesn't include a %, it is tacked on as the last
 * argument.
 *
 * Don't just set JAMSHELL to /bin/sh or cmd.exe - it won't work!
 *
 * External routines:
 *	execcmd() - launch an async command execution
 * 	execwait() - wait and drive at most one execution completion
 *
 * Internal routines:
 *	onintr() - bump intr to note command interruption
 *
 * 04/08/94 (seiwald) - Coherent/386 support added.
 * 05/04/94 (seiwald) - async multiprocess interface
 * 01/22/95 (seiwald) - $(JAMSHELL) support
 * 06/02/97 (gsar)    - full async multiprocess support for Win32
 */

static int intr = 0;
static int cmdsrunning = 0;
static void (*istat)( int );

static int  is_nt_351        = 0;
static int  is_win95         = 1;
static int  is_win95_defined = 0;


static struct
{
	int	pid; /* on win32, a real process handle */
	void	(*func)( void *closure, int status );
	void 	*closure;
	char	*tempfile;

} cmdtab[ MAXJOBS ] = {{0}};


static void
set_is_win95( void )
{
  OSVERSIONINFO  os_info;

  os_info.dwOSVersionInfoSize = sizeof(os_info);
  os_info.dwPlatformId        = VER_PLATFORM_WIN32_WINDOWS;
  GetVersionEx( &os_info );
  
  is_win95         = (os_info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS);
  is_win95_defined = 1;
  
  /* now, test wether we're running Windows 3.51                */
  /* this is later used to limit the system call command length */
  if (os_info.dwPlatformId ==  VER_PLATFORM_WIN32_NT)
    is_nt_351 = os_info.dwMajorVersion == 3;
}


static char**
string_to_args( const char*  string, int*  pcount )
{
  int    total    = strlen( string );
  int    in_quote = 0, num_args;
  char*  line;
  char*  p;
  char** arg;
  char** args;

  *pcount = 0;  

  /* do not copy trailing newlines, if any */  
  {
    int  i;
    
    for ( i = total-1; i > 0; i-- )
    {
      if ( string[i] != '\n' && string[i] != '\r' )
        break;
      total --;
    }
  }
  
  /* first of all, copy the input string */
  line    = (char*)malloc( total+2 );
  if (!line)
    return 0;
    
  memcpy( line+1, string, total );
  line[0]       = 0;
  line[total+1] = 0;
  
  in_quote = 0;
  for ( p = line+1; p[0]; p++ )
  {
    switch (p[0])
    {
      case '"':
        in_quote = !in_quote;
        break;
        
      case ' ':
      case '\t':
        if (!in_quote)
          p[0]    = 0;
        
      default:
        ;
    }
  }
  
  /* now count the arguments.. */
  for ( p = line; p < line+total+1; p++ )
    if ( !p[0] && p[1] )
      num_args++;
      
  /* allocate the args array */
  args = (char**)malloc( num_args*sizeof(char*)+2 );
  if (!args)
  {
    free( line );
    return 0;
  }
  
  arg = args+1;
  for ( p = line; p < line+total+1; p++ )
    if ( !p[0] && p[1] )
    {
      arg[0] = p+1;
      arg++;
    }
  arg[0]  = 0;
  *pcount = num_args;
  args[0] = line;
  return args+1;
}

static void
free_args( char** args )
{
  free( args[-1] );
  free( args-1 );
}


/* process a "del" or "erase" command under Windows 95/98 */
static int
process_del( char*  command )
{
  char** arg;
  char*  p = command, *q;
  int    wildcard = 0, result = 0;

  /* first of all, skip the command itself */
  if ( p[0] == 'd' )
    p += 3; /* assumes "del..;" */
  else if ( p[0] == 'e' )
    p += 5; /* assumes "erase.." */
  else
    return 1; /* invalid command */

  /* process all targets independently */
  for (;;)
  {
    /* skip leading spaces */
    while ( *p && isspace(*p) )
      p++;
      
    /* exit if we encounter an end of string */
    if (!*p)
      return 0;
      
    /* ignore toggles/flags */
    if (*p == '/')
    {
      p++;
      while ( *p && isalnum(*p) )
        p++;
    }
    else
    {
      int  in_quote = 0;
      int  wildcard = 0;
      int  go_on    = 1;
      
      q = p;
      while (go_on)
      {
        switch (*p)
        {
          case '"':
            in_quote = !in_quote;
            break;
          
          case '?':
          case '*':
            if (!in_quote)
              wildcard = 1;
            break;
            
          case '\0':
            if (in_quote)
              return 1;
            /* fall-through */
              
          case ' ':
          case '\t':
            if (!in_quote)
            {
              int    len = p - q;
              int    result;
              char*  line;
              
              /* q..p-1 contains the delete argument */
              if ( len <= 0 )
                return 1;
  
              line = (char*)malloc( len+4+1 );
              if (!line)
                return 1;
                
              strncpy( line, "del ", 4 );
              strncpy( line+4, q, len );
              line[len+4] = '\0';
              
              if ( wildcard )
                result = system( line );
              else
                result = !DeleteFile( line+4 );
  
              free( line );
              if (result)
                return 1;
                
              go_on = 0;
            }
            
          default:
            ;
        }
        p++;
      } /* while (go_on) */
    }
  }
}


/*
 * onintr() - bump intr to note command interruption
 */

void
onintr( int disp )
{
	intr++;
	printf( "...interrupted\n" );
}

/*
 * execcmd() - launch an async command execution
 */

void
execcmd( 
	char *string,
	void (*func)( void *closure, int status ),
	void *closure,
	LIST *shell )
{
	int pid;
	int slot;
	int max_line;
	char *argv[ MAXARGC + 1 ];	/* +1 for NULL */
	char *p;

        if ( !is_win95_defined )
          set_is_win95();
          
	/* Find a slot in the running commands table for this one. */
        if ( is_win95 )
        {
          /* only synchronous spans are supported on Windows 95/98 */
          slot = 0;
        }
        else
        {
	  for( slot = 0; slot < MAXJOBS; slot++ )
	      if( !cmdtab[ slot ].pid )
		  break;
	}
	if( slot == MAXJOBS )
	{
	    printf( "no slots for child!\n" );
	    exit( EXITBAD );
	}
  
	if( !cmdtab[ slot ].tempfile )
	{
	      char *tempdir;
  
	      if( !( tempdir = getenv( "TEMP" ) ) &&
		  !( tempdir = getenv( "TMP" ) ) )
		      tempdir = "\\temp";
  
	      cmdtab[ slot ].tempfile = malloc( strlen( tempdir ) + 14 );
  
	      sprintf( cmdtab[ slot ].tempfile, "%s\\jamtmp%02d.bat", 
				  tempdir, slot );
        }

	/* Trim leading, ending white space */

	while( isspace( *string ) )
		++string;

	p = strchr( string, '\n' );

	while( p && isspace( *p ) )
		++p;

        /* on Windows NT 3.51, the maximul line length is 996 bytes !! */
        /* while it's much bigger NT 4 and 2k                          */
        max_line = is_nt_351 ? 996 : MAXLINE;

	/* If multi line, or too long, or JAMSHELL is set, write to bat file. */
	/* Otherwise, exec directly. */
	/* Frankly, if it is a single long line I don't think the */
	/* command interpreter will do any better -- it will fail. */

	if( p && *p || strlen( string ) > max_line || shell )
	{
	    FILE *f;

	    /* Write command to bat file. */

	    f = fopen( cmdtab[ slot ].tempfile, "w" );
	    fputs( string, f );
	    fclose( f );

	    string = cmdtab[ slot ].tempfile;
	}

	/* Forumulate argv */
	/* If shell was defined, be prepared for % and ! subs. */
	/* Otherwise, use stock /bin/sh (on unix) or cmd.exe (on NT). */

	if( shell )
	{
	    int i;
	    char jobno[4];
	    int gotpercent = 0;

	    sprintf( jobno, "%d", slot + 1 );

	    for( i = 0; shell && i < MAXARGC; i++, shell = list_next( shell ) )
	    {
		switch( shell->string[0] )
		{
		case '%':	argv[i] = string; gotpercent++; break;
		case '!':	argv[i] = jobno; break;
		default:	argv[i] = shell->string;
		}
		if( DEBUG_EXECCMD )
		    printf( "argv[%d] = '%s'\n", i, argv[i] );
	    }

	    if( !gotpercent )
		argv[i++] = string;

	    argv[i] = 0;
	}
	else
	{
        /* don't worry, this is ignored on Win95/98, see later.. */
	    argv[0] = "cmd.exe";
	    argv[1] = "/Q/C";		/* anything more is non-portable */
	    argv[2] = string;
	    argv[3] = 0;
	}

	/* Catch interrupts whenever commands are running. */

	if( !cmdsrunning++ )
	    istat = signal( SIGINT, onintr );

	/* Start the command */

        /* on Win95, we only do a synchronous call */
        if ( is_win95 )
        {
          static const char* hard_coded[] =
          {
            "del", "erase", "copy", "mkdir", "rmdir", "cls", "dir",
            "ren", "rename", "move", 0
          };
          
          const char**  keyword;
          int           len, spawn = 1;
          int           result;
          
          for ( keyword = hard_coded; keyword[0]; keyword++ )
          {
            len = strlen( keyword[0] );
            if ( strnicmp( string, keyword[0], len ) == 0 &&
                 !isalnum(string[len]) )
            {
              /* this is one of the hard coded symbols, use 'system' to run */
              /* them.. except for "del"/"erase"                            */
              if ( keyword - hard_coded < 2 )
                result = process_del( string );
              else
                result = system( string );

              spawn  = 0;
              break;
            }
          }
          
          if (spawn)
          {
            char**  args;
            int     num_args;
            
            /* convert the string into an array of arguments */
            /* we need to take care of double quotes !!      */
            args = string_to_args( string, &num_args );
            if ( args )
            {
#if 0
              char** arg;
              fprintf( stderr, "%s: ", args[0] );
              arg = args+1;
              while ( arg[0] )
              {
                fprintf( stderr, " {%s}", arg[0] );
                arg++;
              }
              fprintf( stderr, "\n" );
#endif              
              result = spawnvp( P_WAIT, args[0], args );
              free_args( args );
            }
            else
              result = 1;
          }
          func( closure, result ? EXEC_CMD_FAIL : EXEC_CMD_OK );
          return;
        }

    /* the rest is for Windows NT only */
	if( ( pid = spawnvp( P_NOWAIT, argv[0], argv ) ) == -1 )
	{
	    perror( "spawn" );
	    exit( EXITBAD );
	}
	/* Save the operation for execwait() to find. */

	cmdtab[ slot ].pid = pid;
	cmdtab[ slot ].func = func;
	cmdtab[ slot ].closure = closure;

	/* Wait until we're under the limit of concurrent commands. */
	/* Don't trust globs.jobs alone.                            */

	while( cmdsrunning >= MAXJOBS || cmdsrunning >= globs.jobs )
	    if( !execwait() )
		break;
}

/*
 * execwait() - wait and drive at most one execution completion
 */

int
execwait()
{
	int i;
	int status, w;
	int rstat;
        
	/* Handle naive make1() which doesn't know if cmds are running. */

	if( !cmdsrunning )
	    return 0;

        if ( is_win95 )
          return 0;
          
	/* Pick up process pid and status */
    
	while( ( w = wait( &status ) ) == -1 && errno == EINTR )
		;

	if( w == -1 )
	{
	    printf( "child process(es) lost!\n" );
	    perror("wait");
	    exit( EXITBAD );
	}

	/* Find the process in the cmdtab. */

	for( i = 0; i < MAXJOBS; i++ )
	    if( w == cmdtab[ i ].pid )
		break;

	if( i == MAXJOBS )
	{
	    printf( "waif child found!\n" );
	    exit( EXITBAD );
	}

	/* Drive the completion */

	if( !--cmdsrunning )
	    signal( SIGINT, istat );

	if( intr )
	    rstat = EXEC_CMD_INTR;
	else if( w == -1 || status != 0 )
	    rstat = EXEC_CMD_FAIL;
	else
	    rstat = EXEC_CMD_OK;

	cmdtab[ i ].pid = 0;

	(*cmdtab[ i ].func)( cmdtab[ i ].closure, rstat );

	return 1;
}

# if !defined( __BORLANDC__ )

static int
my_wait( int *status )
{
	int i, num_active = 0;
	DWORD exitcode, waitcode;
	static HANDLE *active_handles = 0;

	if (!active_handles)
	    active_handles = (HANDLE *)malloc(globs.jobs * sizeof(HANDLE) );

	/* first see if any non-waited-for processes are dead,
	 * and return if so.
	 */
	for ( i = 0; i < globs.jobs; i++ ) {
	    if ( cmdtab[i].pid ) {
		if ( GetExitCodeProcess((HANDLE)cmdtab[i].pid, &exitcode) ) {
		    if ( exitcode == STILL_ACTIVE )
			active_handles[num_active++] = (HANDLE)cmdtab[i].pid;
		    else {
			CloseHandle((HANDLE)cmdtab[i].pid);
			*status = (int)((exitcode & 0xff) << 8);
			return cmdtab[i].pid;
		    }
		}
		else
		    goto FAILED;
	    }
	}

	/* if a child exists, wait for it to die */
	if ( !num_active ) {
	    errno = ECHILD;
	    return -1;
	}
	waitcode = WaitForMultipleObjects( num_active,
					   active_handles,
					   FALSE,
					   INFINITE );
	if ( waitcode != WAIT_FAILED ) {
	    if ( waitcode >= WAIT_ABANDONED_0
		&& waitcode < WAIT_ABANDONED_0 + num_active )
		i = waitcode - WAIT_ABANDONED_0;
	    else
		i = waitcode - WAIT_OBJECT_0;
	    if ( GetExitCodeProcess(active_handles[i], &exitcode) ) {
		CloseHandle(active_handles[i]);
		*status = (int)((exitcode & 0xff) << 8);
		return (int)active_handles[i];
	    }
	}

FAILED:
	errno = GetLastError();
	return -1;
    
}

# endif /* !__BORLANDC__ */

# endif /* USE_EXECNT */
