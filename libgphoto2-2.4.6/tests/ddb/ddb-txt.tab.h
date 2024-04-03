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
     TOK_SEP = 258,
     TOK_COMMA = 259,
     TOK_WHITESPACE = 260,
     TOK_NEWLINE = 261,
     TOK_NUMBER = 262,
     TOK_STRING = 263,
     TOK_INTERFACE = 264,
     TOK_DRIVER = 265,
     TOK_DEVICE = 266,
     TOK_BEGIN = 267,
     TOK_END = 268,
     TOK_SERIAL = 269,
     TOK_SPEEDS = 270,
     TOK_USB = 271,
     TOK_PRODUCT = 272,
     TOK_VENDOR = 273,
     TOK_CLASS = 274,
     TOK_SUBCLASS = 275,
     TOK_PROTOCOL = 276,
     TOK_DISK = 277,
     TOK_PTPIP = 278,
     TOK_DEVICE_TYPE = 279,
     TOK_DRIVER_STATUS = 280,
     TOK_OPERATIONS = 281,
     TOK_FILE_OPERATIONS = 282,
     TOK_FOLDER_OPERATIONS = 283,
     TOK_DRIVER_OPTIONS = 284,
     TOK_OPTION = 285,
     VAL_FLAG = 286
   };
#endif
/* Tokens.  */
#define TOK_SEP 258
#define TOK_COMMA 259
#define TOK_WHITESPACE 260
#define TOK_NEWLINE 261
#define TOK_NUMBER 262
#define TOK_STRING 263
#define TOK_INTERFACE 264
#define TOK_DRIVER 265
#define TOK_DEVICE 266
#define TOK_BEGIN 267
#define TOK_END 268
#define TOK_SERIAL 269
#define TOK_SPEEDS 270
#define TOK_USB 271
#define TOK_PRODUCT 272
#define TOK_VENDOR 273
#define TOK_CLASS 274
#define TOK_SUBCLASS 275
#define TOK_PROTOCOL 276
#define TOK_DISK 277
#define TOK_PTPIP 278
#define TOK_DEVICE_TYPE 279
#define TOK_DRIVER_STATUS 280
#define TOK_OPERATIONS 281
#define TOK_FILE_OPERATIONS 282
#define TOK_FOLDER_OPERATIONS 283
#define TOK_DRIVER_OPTIONS 284
#define TOK_OPTION 285
#define VAL_FLAG 286




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE yylloc;
