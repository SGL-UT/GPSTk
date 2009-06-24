/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     _LANGLE_t = 258,
     _LANGLE_EQUALS_t = 259,
     _EQUALS_t = 260,
     _RANGLE_t = 261,
     _RANGLE_EQUALS_t = 262,
     _BAR_t = 263,
     _BARBAR_t = 264,
     _SEMIC_t = 265,
     _COLON_t = 266,
     _BANG_t = 267,
     _BANG_EQUALS_t = 268,
     _QUESTION_EQUALS_t = 269,
     _LPAREN_t = 270,
     _RPAREN_t = 271,
     _LBRACKET_t = 272,
     _RBRACKET_t = 273,
     _LBRACE_t = 274,
     _RBRACE_t = 275,
     _AMPER_t = 276,
     _AMPERAMPER_t = 277,
     _PLUS_EQUALS_t = 278,
     ACTIONS_t = 279,
     BIND_t = 280,
     BREAK_t = 281,
     CASE_t = 282,
     CONTINUE_t = 283,
     DEFAULT_t = 284,
     ELSE_t = 285,
     EXISTING_t = 286,
     FOR_t = 287,
     IF_t = 288,
     IGNORE_t = 289,
     IN_t = 290,
     INCLUDE_t = 291,
     LOCAL_t = 292,
     MAXLINE_t = 293,
     ON_t = 294,
     PIECEMEAL_t = 295,
     QUIETLY_t = 296,
     RETURN_t = 297,
     RULE_t = 298,
     SWITCH_t = 299,
     TOGETHER_t = 300,
     UPDATED_t = 301,
     WHILE_t = 302,
     ARG = 303,
     STRING = 304
   };
#endif
/* Tokens.  */
#define _LANGLE_t 258
#define _LANGLE_EQUALS_t 259
#define _EQUALS_t 260
#define _RANGLE_t 261
#define _RANGLE_EQUALS_t 262
#define _BAR_t 263
#define _BARBAR_t 264
#define _SEMIC_t 265
#define _COLON_t 266
#define _BANG_t 267
#define _BANG_EQUALS_t 268
#define _QUESTION_EQUALS_t 269
#define _LPAREN_t 270
#define _RPAREN_t 271
#define _LBRACKET_t 272
#define _RBRACKET_t 273
#define _LBRACE_t 274
#define _RBRACE_t 275
#define _AMPER_t 276
#define _AMPERAMPER_t 277
#define _PLUS_EQUALS_t 278
#define ACTIONS_t 279
#define BIND_t 280
#define BREAK_t 281
#define CASE_t 282
#define CONTINUE_t 283
#define DEFAULT_t 284
#define ELSE_t 285
#define EXISTING_t 286
#define FOR_t 287
#define IF_t 288
#define IGNORE_t 289
#define IN_t 290
#define INCLUDE_t 291
#define LOCAL_t 292
#define MAXLINE_t 293
#define ON_t 294
#define PIECEMEAL_t 295
#define QUIETLY_t 296
#define RETURN_t 297
#define RULE_t 298
#define SWITCH_t 299
#define TOGETHER_t 300
#define UPDATED_t 301
#define WHILE_t 302
#define ARG 303
#define STRING 304




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

