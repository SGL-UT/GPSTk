/*
 * Copyright 1993, 2000 Christopher Seiwald.
 *
 * This file is part of Jam - see jam.c for Copyright information.
 */

# include "jam.h"
# include "lists.h"
# include "parse.h"
# include "compile.h"
# include "rules.h"
# include "variable.h"
# include "regexp.h"
# include "hdrmacro.h"
# include "hash.h"
# include "newstr.h"

/*
 * hdrmacro.c - handle header files that define macros used in
 *              #include statements.
 *
 *  we look for lines like "#define MACRO  <....>" or '#define MACRO  "    "'
 *  in the target file. When found, we 
 *
 *  we then phony up a rule invocation like:
 *
 *	$(HDRRULE) <target> : <resolved included files> ;
 *
 * External routines:
 *    headers1() - scan a target for "#include MACRO" lines and try
 *                 to resolve them when needed
 *
 * Internal routines:
 *    headers1() - using regexp, scan a file and build include LIST
 *
 * 04/13/94 (seiwald) - added shorthand L0 for null list pointer
 * 09/10/00 (seiwald) - replaced call to compile_rule with evaluate_rule,
 *		so that headers() doesn't have to mock up a parse structure
 *		just to invoke a rule.
 */

static LIST *header_macros1( LIST *l, char *file, int rec, regexp *re[] );

/* this type is used to store a dictionary of file header macros */
typedef struct header_macro
{
  const char*  symbol;
  const char*  filename;  /* we could maybe use a LIST here ?? */
  
} HEADER_MACRO;
 
static struct hash*   header_macros_hash = 0;

/*
 * headers() - scan a target for include files and call HDRRULE
 */

# define MAXINC 10

void
macro_headers( TARGET *t )
{
	LIST	*hdrrule;
	regexp	*re;
	FILE	*f;
	char	buf[ 1024 ];
	int	i;

    if ( DEBUG_HEADER )
      printf( "macro header scan for %s\n", t->name );

    /* this regexp is used to detect lines of the form       */
    /* "#define  MACRO  <....>" or "#define  MACRO  "....."  */
    /* in the header macro files..                           */
    re = regcomp(
       "^[ 	]*#[ 	]*define[ 	]*([A-Za-z][A-Za-z0-9_]*)[ 	]*"
       "[<\"]([^\">]*)[\">].*$" );

	if( ( f = fopen( t->boundname, "r" ) ) == 0 )
          return;

	while( fgets( buf, sizeof( buf ), f ) )
	{
      HEADER_MACRO  var, *v = &var;
      
      if ( regexec( re, buf ) && re->startp[1] )
      {
        char  buf1[ MAXSYM ], buf2[ MAXSYM ];
        int   l1, l2;
        
        l1 = re->endp[1] - re->startp[1];
        l2 = re->endp[2] - re->startp[2];
        
        memcpy( buf1, re->startp[1], l1 );
        memcpy( buf2, re->startp[2], l2 );
        buf1[l1] = '\0';
        buf2[l2] = '\0';
        
        /* we detected a line that looks like "#define  MACRO  filename */
        if ( DEBUG_HEADER )
          printf( "macro '%s' used to define filename '%s' in '%s'\n",
                   buf1, buf2, t->boundname );

        /* add macro definition to hash table */
        if ( !header_macros_hash )
          header_macros_hash = hashinit( sizeof( HEADER_MACRO ), "hdrmacros" );

        v->symbol   = (const char*)buf1;
        v->filename = 0;
        if ( hashenter( header_macros_hash, (HASHDATA **)&v ) )
        {
          v->symbol   = newstr( buf1 );  /* never freed */
          v->filename = newstr( buf2 );  /* never freed */
        }
        /* XXXX: FOR NOW, WE IGNORE MULTIPLE MACRO DEFINITIONS !! */
        /*       WE MIGHT AS WELL USE A LIST TO STORE THEM..      */
      }
    }

	fclose( f );

    free( re );
}


const char*
macro_header_get( const char*  macro_name )
{
  HEADER_MACRO  var, *v = &var;

  v->symbol = (char*)macro_name;

  if( header_macros_hash && hashcheck( header_macros_hash, (HASHDATA **)&v ) )
  {
    if ( DEBUG_HEADER )
      printf( "### macro '%s' evaluated to '%s'\n", macro_name, v->filename );
    return (const char*) v->filename;
  }
  return 0;  
}

