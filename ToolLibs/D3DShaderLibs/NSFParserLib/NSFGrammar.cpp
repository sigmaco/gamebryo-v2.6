/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

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

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse NSFParserparse
#define yylex   NSFParserlex
#define yyerror NSFParsererror
#define yylval  NSFParserlval
#define yychar  NSFParserchar
#define yydebug NSFParserdebug
#define yynerrs NSFParsernerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     EOLN = 258,
     PATH = 259,
     L_ACCOLADE = 260,
     R_ACCOLADE = 261,
     L_PARENTHESE = 262,
     R_PARENTHESE = 263,
     L_BRACKET = 264,
     R_BRACKET = 265,
     L_ANGLEBRACKET = 266,
     R_ANGLEBRACKET = 267,
     OR = 268,
     ASSIGN = 269,
     COMMA = 270,
     NSF_AT_SYMBOL = 271,
     NSF_COLON = 272,
     NSF_SEMICOLON = 273,
     UNDERSCORE = 274,
     ASTERIK = 275,
     FORWARDSLASH = 276,
     PLUS = 277,
     MINUS = 278,
     N_HEX = 279,
     N_FLOAT = 280,
     N_INT = 281,
     N_STRING = 282,
     N_QUOTE = 283,
     N_BOOL = 284,
     N_VERSION = 285,
     NSFSHADER = 286,
     ARTIST = 287,
     HIDDEN = 288,
     SAVE = 289,
     ATTRIBUTES = 290,
     GLOBALATTRIBUTES = 291,
     ATTRIB = 292,
     ATTRIB_BOOL = 293,
     ATTRIB_STRING = 294,
     ATTRIB_UINT = 295,
     ATTRIB_FLOAT = 296,
     ATTRIB_POINT2 = 297,
     ATTRIB_POINT3 = 298,
     ATTRIB_POINT4 = 299,
     ATTRIB_MATRIX3 = 300,
     ATTRIB_TRANSFORM = 301,
     ATTRIB_COLOR = 302,
     ATTRIB_TEXTURE = 303,
     PACKINGDEF = 304,
     PD_STREAM = 305,
     PD_FIXEDFUNCTION = 306,
     SEMANTICADAPTERTABLE = 307,
     PDP_POSITION = 308,
     PDP_POSITION0 = 309,
     PDP_POSITION1 = 310,
     PDP_POSITION2 = 311,
     PDP_POSITION3 = 312,
     PDP_POSITION4 = 313,
     PDP_POSITION5 = 314,
     PDP_POSITION6 = 315,
     PDP_POSITION7 = 316,
     PDP_BLENDWEIGHTS = 317,
     PDP_BLENDINDICES = 318,
     PDP_NORMAL = 319,
     PDP_POINTSIZE = 320,
     PDP_COLOR = 321,
     PDP_COLOR2 = 322,
     PDP_TEXCOORD0 = 323,
     PDP_TEXCOORD1 = 324,
     PDP_TEXCOORD2 = 325,
     PDP_TEXCOORD3 = 326,
     PDP_TEXCOORD4 = 327,
     PDP_TEXCOORD5 = 328,
     PDP_TEXCOORD6 = 329,
     PDP_TEXCOORD7 = 330,
     PDP_NORMAL2 = 331,
     PDP_TANGENT = 332,
     PDP_BINORMAL = 333,
     PDP_EXTRADATA = 334,
     PDT_FLOAT1 = 335,
     PDT_FLOAT2 = 336,
     PDT_FLOAT3 = 337,
     PDT_FLOAT4 = 338,
     PDT_UBYTECOLOR = 339,
     PDT_SHORT1 = 340,
     PDT_SHORT2 = 341,
     PDT_SHORT3 = 342,
     PDT_SHORT4 = 343,
     PDT_UBYTE4 = 344,
     PDT_NORMSHORT1 = 345,
     PDT_NORMSHORT2 = 346,
     PDT_NORMSHORT3 = 347,
     PDT_NORMSHORT4 = 348,
     PDT_NORMPACKED3 = 349,
     PDT_PBYTE1 = 350,
     PDT_PBYTE2 = 351,
     PDT_PBYTE3 = 352,
     PDT_PBYTE4 = 353,
     PDT_FLOAT2H = 354,
     PDT_NORMUBYTE4 = 355,
     PDT_NORMUSHORT2 = 356,
     PDT_NORMUSHORT4 = 357,
     PDT_UDEC3 = 358,
     PDT_NORMDEC3 = 359,
     PDT_FLOAT16_2 = 360,
     PDT_FLOAT16_4 = 361,
     PDTESS_DEFAULT = 362,
     PDTESS_PARTIALU = 363,
     PDTESS_PARTIALV = 364,
     PDTESS_CROSSUV = 365,
     PDTESS_UV = 366,
     PDTESS_LOOKUP = 367,
     PDTESS_LOOKUPPRESAMPLED = 368,
     PDU_POSITION = 369,
     PDU_BLENDWEIGHT = 370,
     PDU_BLENDINDICES = 371,
     PDU_NORMAL = 372,
     PDU_PSIZE = 373,
     PDU_TEXCOORD = 374,
     PDU_TANGENT = 375,
     PDU_BINORMAL = 376,
     PDU_TESSFACTOR = 377,
     PDU_POSITIONT = 378,
     PDU_COLOR = 379,
     PDU_FOG = 380,
     PDU_DEPTH = 381,
     PDU_SAMPLE = 382,
     RENDERSTATES = 383,
     CMDEFINED = 384,
     CMATTRIBUTE = 385,
     CMCONSTANT = 386,
     CMGLOBAL = 387,
     CMOPERATOR = 388,
     VSCONSTANTMAP = 389,
     VSPROGRAM = 390,
     GSCONSTANTMAP = 391,
     GSPROGRAM = 392,
     PSCONSTANTMAP = 393,
     PSPROGRAM = 394,
     PROGRAM = 395,
     ENTRYPOINT = 396,
     SHADERTARGET = 397,
     SOFTWAREVP = 398,
     SKINBONEMATRIX3 = 399,
     REQUIREMENTS = 400,
     VSVERSION = 401,
     GSVERSION = 402,
     PSVERSION = 403,
     USERVERSION = 404,
     PLATFORM = 405,
     BONESPERPARTITION = 406,
     BINORMALTANGENTMETHOD = 407,
     BINORMALTANGENTUVSOURCE = 408,
     NBTMETHOD_NONE = 409,
     NBTMETHOD_NI = 410,
     NBTMETHOD_MAX = 411,
     NBTMETHOD_ATI = 412,
     USERDEFINEDDATA = 413,
     IMPLEMENTATION = 414,
     OUTPUTSTREAM = 415,
     STREAMOUTPUT = 416,
     STREAMOUTTARGETS = 417,
     STREAMOUTAPPEND = 418,
     MAXVERTEXCOUNT = 419,
     OUTPUTPRIMTYPE = 420,
     _POINT = 421,
     _LINE = 422,
     _TRIANGLE = 423,
     VERTEXFORMAT = 424,
     FMT_FLOAT = 425,
     FMT_INT = 426,
     FMT_UINT = 427,
     CLASSNAME = 428,
     PASS = 429,
     STAGE = 430,
     TSS_TEXTURE = 431,
     TSS_COLOROP = 432,
     TSS_COLORARG0 = 433,
     TSS_COLORARG1 = 434,
     TSS_COLORARG2 = 435,
     TSS_ALPHAOP = 436,
     TSS_ALPHAARG0 = 437,
     TSS_ALPHAARG1 = 438,
     TSS_ALPHAARG2 = 439,
     TSS_RESULTARG = 440,
     TSS_CONSTANT_DEPRECATED = 441,
     TSS_BUMPENVMAT00 = 442,
     TSS_BUMPENVMAT01 = 443,
     TSS_BUMPENVMAT10 = 444,
     TSS_BUMPENVMAT11 = 445,
     TSS_BUMPENVLSCALE = 446,
     TSS_BUMPENVLOFFSET = 447,
     TSS_TEXCOORDINDEX = 448,
     TSS_TEXTURETRANSFORMFLAGS = 449,
     TSS_TEXTRANSMATRIX = 450,
     TTFF_DISABLE = 451,
     TTFF_COUNT1 = 452,
     TTFF_COUNT2 = 453,
     TTFF_COUNT3 = 454,
     TTFF_COUNT4 = 455,
     TTFF_PROJECTED = 456,
     PROJECTED = 457,
     USEMAPINDEX = 458,
     INVERSE = 459,
     TRANSPOSE = 460,
     TTSRC_GLOBAL = 461,
     TTSRC_CONSTANT = 462,
     TT_WORLD_PARALLEL = 463,
     TT_WORLD_PERSPECTIVE = 464,
     TT_WORLD_SPHERE_MAP = 465,
     TT_CAMERA_SPHERE_MAP = 466,
     TT_SPECULAR_CUBE_MAP = 467,
     TT_DIFFUSE_CUBE_MAP = 468,
     TCI_PASSTHRU = 469,
     TCI_CAMERASPACENORMAL = 470,
     TCI_CAMERASPACEPOSITION = 471,
     TCI_CAMERASPACEREFLECT = 472,
     TCI_SPHEREMAP = 473,
     TOP_DISABLE = 474,
     TOP_SELECTARG1 = 475,
     TOP_SELECTARG2 = 476,
     TOP_MODULATE = 477,
     TOP_MODULATE2X = 478,
     TOP_MODULATE4X = 479,
     TOP_ADD = 480,
     TOP_ADDSIGNED = 481,
     TOP_ADDSIGNED2X = 482,
     TOP_SUBTRACT = 483,
     TOP_ADDSMOOTH = 484,
     TOP_BLENDDIFFUSEALPHA = 485,
     TOP_BLENDTEXTUREALPHA = 486,
     TOP_BLENDFACTORALPHA = 487,
     TOP_BLENDTEXTUREALPHAPM = 488,
     TOP_BLENDCURRENTALPHA = 489,
     TOP_PREMODULATE = 490,
     TOP_MODULATEALPHA_ADDCOLOR = 491,
     TOP_MODULATECOLOR_ADDALPHA = 492,
     TOP_MODULATEINVALPHA_ADDCOLOR = 493,
     TOP_MODULATEINVCOLOR_ADDALPHA = 494,
     TOP_BUMPENVMAP = 495,
     TOP_BUMPENVMAPLUMINANCE = 496,
     TOP_DOTPRODUCT3 = 497,
     TOP_MULTIPLYADD = 498,
     TOP_LERP = 499,
     TA_CURRENT = 500,
     TA_DIFFUSE = 501,
     TA_SELECTMASK = 502,
     TA_SPECULAR = 503,
     TA_TEMP = 504,
     TA_TEXTURE = 505,
     TA_TFACTOR = 506,
     TA_ALPHAREPLICATE = 507,
     TA_COMPLEMENT = 508,
     NTM_BASE = 509,
     NTM_DARK = 510,
     NTM_DETAIL = 511,
     NTM_GLOSS = 512,
     NTM_GLOW = 513,
     NTM_BUMP = 514,
     NTM_NORMAL = 515,
     NTM_PARALLAX = 516,
     NTM_DECAL = 517,
     SAMPLER = 518,
     TSAMP_ADDRESSU = 519,
     TSAMP_ADDRESSV = 520,
     TSAMP_ADDRESSW = 521,
     TSAMP_BORDERCOLOR = 522,
     TSAMP_MAGFILTER = 523,
     TSAMP_MINFILTER = 524,
     TSAMP_MIPFILTER = 525,
     TSAMP_MIPMAPLODBIAS = 526,
     TSAMP_MAXMIPLEVEL = 527,
     TSAMP_MAXANISOTROPY = 528,
     TSAMP_SRGBTEXTURE = 529,
     TSAMP_ELEMENTINDEX = 530,
     TSAMP_DMAPOFFSET = 531,
     TSAMP_ALPHAKILL_DEPRECATED = 532,
     TSAMP_COLORKEYOP_DEPRECATED = 533,
     TSAMP_COLORSIGN_DEPRECATED = 534,
     TSAMP_COLORKEYCOLOR_DEPRECATED = 535,
     TADDR_WRAP = 536,
     TADDR_MIRROR = 537,
     TADDR_CLAMP = 538,
     TADDR_BORDER = 539,
     TADDR_MIRRORONCE = 540,
     TADDR_CLAMPTOEDGE_DEPRECATED = 541,
     TEXF_NONE = 542,
     TEXF_POINT = 543,
     TEXF_LINEAR = 544,
     TEXF_ANISOTROPIC = 545,
     TEXF_PYRAMIDALQUAD = 546,
     TEXF_GAUSSIANQUAD = 547,
     TEXF_FLATCUBIC_DEPRECATED = 548,
     TEXF_GAUSSIANCUBIC_DEPRECATED = 549,
     TEXF_QUINCUNX_DEPRECATED = 550,
     TEXF_MAX_DEPRECATED = 551,
     TAK_DISABLE_DEPRECATED = 552,
     TAK_ENABLE_DEPRECATED = 553,
     TCKOP_DISABLE_DEPRECATED = 554,
     TCKOP_ALPHA_DEPRECATED = 555,
     TCKOP_RGBA_DEPRECATED = 556,
     TCKOP_KILL_DEPRECATED = 557,
     TOKEN_TEXTURE = 558,
     TEXTURE_SOURCE = 559,
     OBJECTS = 560,
     EFFECT_GENERALLIGHT = 561,
     EFFECT_POINTLIGHT = 562,
     EFFECT_DIRECTIONALLIGHT = 563,
     EFFECT_SPOTLIGHT = 564,
     EFFECT_SHADOWPOINTLIGHT = 565,
     EFFECT_SHADOWDIRECTIONALLIGHT = 566,
     EFFECT_SHADOWSPOTLIGHT = 567,
     EFFECT = 568,
     EFFECT_ENVIRONMENTMAP = 569,
     EFFECT_PROJECTEDSHADOWMAP = 570,
     EFFECT_PROJECTEDLIGHTMAP = 571,
     EFFECT_FOGMAP = 572,
     USEMAPVALUE = 573,
     CMOBJECT = 574
   };
#endif
/* Tokens.  */
#define EOLN 258
#define PATH 259
#define L_ACCOLADE 260
#define R_ACCOLADE 261
#define L_PARENTHESE 262
#define R_PARENTHESE 263
#define L_BRACKET 264
#define R_BRACKET 265
#define L_ANGLEBRACKET 266
#define R_ANGLEBRACKET 267
#define OR 268
#define ASSIGN 269
#define COMMA 270
#define NSF_AT_SYMBOL 271
#define NSF_COLON 272
#define NSF_SEMICOLON 273
#define UNDERSCORE 274
#define ASTERIK 275
#define FORWARDSLASH 276
#define PLUS 277
#define MINUS 278
#define N_HEX 279
#define N_FLOAT 280
#define N_INT 281
#define N_STRING 282
#define N_QUOTE 283
#define N_BOOL 284
#define N_VERSION 285
#define NSFSHADER 286
#define ARTIST 287
#define HIDDEN 288
#define SAVE 289
#define ATTRIBUTES 290
#define GLOBALATTRIBUTES 291
#define ATTRIB 292
#define ATTRIB_BOOL 293
#define ATTRIB_STRING 294
#define ATTRIB_UINT 295
#define ATTRIB_FLOAT 296
#define ATTRIB_POINT2 297
#define ATTRIB_POINT3 298
#define ATTRIB_POINT4 299
#define ATTRIB_MATRIX3 300
#define ATTRIB_TRANSFORM 301
#define ATTRIB_COLOR 302
#define ATTRIB_TEXTURE 303
#define PACKINGDEF 304
#define PD_STREAM 305
#define PD_FIXEDFUNCTION 306
#define SEMANTICADAPTERTABLE 307
#define PDP_POSITION 308
#define PDP_POSITION0 309
#define PDP_POSITION1 310
#define PDP_POSITION2 311
#define PDP_POSITION3 312
#define PDP_POSITION4 313
#define PDP_POSITION5 314
#define PDP_POSITION6 315
#define PDP_POSITION7 316
#define PDP_BLENDWEIGHTS 317
#define PDP_BLENDINDICES 318
#define PDP_NORMAL 319
#define PDP_POINTSIZE 320
#define PDP_COLOR 321
#define PDP_COLOR2 322
#define PDP_TEXCOORD0 323
#define PDP_TEXCOORD1 324
#define PDP_TEXCOORD2 325
#define PDP_TEXCOORD3 326
#define PDP_TEXCOORD4 327
#define PDP_TEXCOORD5 328
#define PDP_TEXCOORD6 329
#define PDP_TEXCOORD7 330
#define PDP_NORMAL2 331
#define PDP_TANGENT 332
#define PDP_BINORMAL 333
#define PDP_EXTRADATA 334
#define PDT_FLOAT1 335
#define PDT_FLOAT2 336
#define PDT_FLOAT3 337
#define PDT_FLOAT4 338
#define PDT_UBYTECOLOR 339
#define PDT_SHORT1 340
#define PDT_SHORT2 341
#define PDT_SHORT3 342
#define PDT_SHORT4 343
#define PDT_UBYTE4 344
#define PDT_NORMSHORT1 345
#define PDT_NORMSHORT2 346
#define PDT_NORMSHORT3 347
#define PDT_NORMSHORT4 348
#define PDT_NORMPACKED3 349
#define PDT_PBYTE1 350
#define PDT_PBYTE2 351
#define PDT_PBYTE3 352
#define PDT_PBYTE4 353
#define PDT_FLOAT2H 354
#define PDT_NORMUBYTE4 355
#define PDT_NORMUSHORT2 356
#define PDT_NORMUSHORT4 357
#define PDT_UDEC3 358
#define PDT_NORMDEC3 359
#define PDT_FLOAT16_2 360
#define PDT_FLOAT16_4 361
#define PDTESS_DEFAULT 362
#define PDTESS_PARTIALU 363
#define PDTESS_PARTIALV 364
#define PDTESS_CROSSUV 365
#define PDTESS_UV 366
#define PDTESS_LOOKUP 367
#define PDTESS_LOOKUPPRESAMPLED 368
#define PDU_POSITION 369
#define PDU_BLENDWEIGHT 370
#define PDU_BLENDINDICES 371
#define PDU_NORMAL 372
#define PDU_PSIZE 373
#define PDU_TEXCOORD 374
#define PDU_TANGENT 375
#define PDU_BINORMAL 376
#define PDU_TESSFACTOR 377
#define PDU_POSITIONT 378
#define PDU_COLOR 379
#define PDU_FOG 380
#define PDU_DEPTH 381
#define PDU_SAMPLE 382
#define RENDERSTATES 383
#define CMDEFINED 384
#define CMATTRIBUTE 385
#define CMCONSTANT 386
#define CMGLOBAL 387
#define CMOPERATOR 388
#define VSCONSTANTMAP 389
#define VSPROGRAM 390
#define GSCONSTANTMAP 391
#define GSPROGRAM 392
#define PSCONSTANTMAP 393
#define PSPROGRAM 394
#define PROGRAM 395
#define ENTRYPOINT 396
#define SHADERTARGET 397
#define SOFTWAREVP 398
#define SKINBONEMATRIX3 399
#define REQUIREMENTS 400
#define VSVERSION 401
#define GSVERSION 402
#define PSVERSION 403
#define USERVERSION 404
#define PLATFORM 405
#define BONESPERPARTITION 406
#define BINORMALTANGENTMETHOD 407
#define BINORMALTANGENTUVSOURCE 408
#define NBTMETHOD_NONE 409
#define NBTMETHOD_NI 410
#define NBTMETHOD_MAX 411
#define NBTMETHOD_ATI 412
#define USERDEFINEDDATA 413
#define IMPLEMENTATION 414
#define OUTPUTSTREAM 415
#define STREAMOUTPUT 416
#define STREAMOUTTARGETS 417
#define STREAMOUTAPPEND 418
#define MAXVERTEXCOUNT 419
#define OUTPUTPRIMTYPE 420
#define _POINT 421
#define _LINE 422
#define _TRIANGLE 423
#define VERTEXFORMAT 424
#define FMT_FLOAT 425
#define FMT_INT 426
#define FMT_UINT 427
#define CLASSNAME 428
#define PASS 429
#define STAGE 430
#define TSS_TEXTURE 431
#define TSS_COLOROP 432
#define TSS_COLORARG0 433
#define TSS_COLORARG1 434
#define TSS_COLORARG2 435
#define TSS_ALPHAOP 436
#define TSS_ALPHAARG0 437
#define TSS_ALPHAARG1 438
#define TSS_ALPHAARG2 439
#define TSS_RESULTARG 440
#define TSS_CONSTANT_DEPRECATED 441
#define TSS_BUMPENVMAT00 442
#define TSS_BUMPENVMAT01 443
#define TSS_BUMPENVMAT10 444
#define TSS_BUMPENVMAT11 445
#define TSS_BUMPENVLSCALE 446
#define TSS_BUMPENVLOFFSET 447
#define TSS_TEXCOORDINDEX 448
#define TSS_TEXTURETRANSFORMFLAGS 449
#define TSS_TEXTRANSMATRIX 450
#define TTFF_DISABLE 451
#define TTFF_COUNT1 452
#define TTFF_COUNT2 453
#define TTFF_COUNT3 454
#define TTFF_COUNT4 455
#define TTFF_PROJECTED 456
#define PROJECTED 457
#define USEMAPINDEX 458
#define INVERSE 459
#define TRANSPOSE 460
#define TTSRC_GLOBAL 461
#define TTSRC_CONSTANT 462
#define TT_WORLD_PARALLEL 463
#define TT_WORLD_PERSPECTIVE 464
#define TT_WORLD_SPHERE_MAP 465
#define TT_CAMERA_SPHERE_MAP 466
#define TT_SPECULAR_CUBE_MAP 467
#define TT_DIFFUSE_CUBE_MAP 468
#define TCI_PASSTHRU 469
#define TCI_CAMERASPACENORMAL 470
#define TCI_CAMERASPACEPOSITION 471
#define TCI_CAMERASPACEREFLECT 472
#define TCI_SPHEREMAP 473
#define TOP_DISABLE 474
#define TOP_SELECTARG1 475
#define TOP_SELECTARG2 476
#define TOP_MODULATE 477
#define TOP_MODULATE2X 478
#define TOP_MODULATE4X 479
#define TOP_ADD 480
#define TOP_ADDSIGNED 481
#define TOP_ADDSIGNED2X 482
#define TOP_SUBTRACT 483
#define TOP_ADDSMOOTH 484
#define TOP_BLENDDIFFUSEALPHA 485
#define TOP_BLENDTEXTUREALPHA 486
#define TOP_BLENDFACTORALPHA 487
#define TOP_BLENDTEXTUREALPHAPM 488
#define TOP_BLENDCURRENTALPHA 489
#define TOP_PREMODULATE 490
#define TOP_MODULATEALPHA_ADDCOLOR 491
#define TOP_MODULATECOLOR_ADDALPHA 492
#define TOP_MODULATEINVALPHA_ADDCOLOR 493
#define TOP_MODULATEINVCOLOR_ADDALPHA 494
#define TOP_BUMPENVMAP 495
#define TOP_BUMPENVMAPLUMINANCE 496
#define TOP_DOTPRODUCT3 497
#define TOP_MULTIPLYADD 498
#define TOP_LERP 499
#define TA_CURRENT 500
#define TA_DIFFUSE 501
#define TA_SELECTMASK 502
#define TA_SPECULAR 503
#define TA_TEMP 504
#define TA_TEXTURE 505
#define TA_TFACTOR 506
#define TA_ALPHAREPLICATE 507
#define TA_COMPLEMENT 508
#define NTM_BASE 509
#define NTM_DARK 510
#define NTM_DETAIL 511
#define NTM_GLOSS 512
#define NTM_GLOW 513
#define NTM_BUMP 514
#define NTM_NORMAL 515
#define NTM_PARALLAX 516
#define NTM_DECAL 517
#define SAMPLER 518
#define TSAMP_ADDRESSU 519
#define TSAMP_ADDRESSV 520
#define TSAMP_ADDRESSW 521
#define TSAMP_BORDERCOLOR 522
#define TSAMP_MAGFILTER 523
#define TSAMP_MINFILTER 524
#define TSAMP_MIPFILTER 525
#define TSAMP_MIPMAPLODBIAS 526
#define TSAMP_MAXMIPLEVEL 527
#define TSAMP_MAXANISOTROPY 528
#define TSAMP_SRGBTEXTURE 529
#define TSAMP_ELEMENTINDEX 530
#define TSAMP_DMAPOFFSET 531
#define TSAMP_ALPHAKILL_DEPRECATED 532
#define TSAMP_COLORKEYOP_DEPRECATED 533
#define TSAMP_COLORSIGN_DEPRECATED 534
#define TSAMP_COLORKEYCOLOR_DEPRECATED 535
#define TADDR_WRAP 536
#define TADDR_MIRROR 537
#define TADDR_CLAMP 538
#define TADDR_BORDER 539
#define TADDR_MIRRORONCE 540
#define TADDR_CLAMPTOEDGE_DEPRECATED 541
#define TEXF_NONE 542
#define TEXF_POINT 543
#define TEXF_LINEAR 544
#define TEXF_ANISOTROPIC 545
#define TEXF_PYRAMIDALQUAD 546
#define TEXF_GAUSSIANQUAD 547
#define TEXF_FLATCUBIC_DEPRECATED 548
#define TEXF_GAUSSIANCUBIC_DEPRECATED 549
#define TEXF_QUINCUNX_DEPRECATED 550
#define TEXF_MAX_DEPRECATED 551
#define TAK_DISABLE_DEPRECATED 552
#define TAK_ENABLE_DEPRECATED 553
#define TCKOP_DISABLE_DEPRECATED 554
#define TCKOP_ALPHA_DEPRECATED 555
#define TCKOP_RGBA_DEPRECATED 556
#define TCKOP_KILL_DEPRECATED 557
#define TOKEN_TEXTURE 558
#define TEXTURE_SOURCE 559
#define OBJECTS 560
#define EFFECT_GENERALLIGHT 561
#define EFFECT_POINTLIGHT 562
#define EFFECT_DIRECTIONALLIGHT 563
#define EFFECT_SPOTLIGHT 564
#define EFFECT_SHADOWPOINTLIGHT 565
#define EFFECT_SHADOWDIRECTIONALLIGHT 566
#define EFFECT_SHADOWSPOTLIGHT 567
#define EFFECT 568
#define EFFECT_ENVIRONMENTMAP 569
#define EFFECT_PROJECTEDSHADOWMAP 570
#define EFFECT_PROJECTEDLIGHTMAP 571
#define EFFECT_FOGMAP 572
#define USEMAPVALUE 573
#define CMOBJECT 574




/* Copy the first part of user declarations.  */



// Turn off warning in automatically generated grammar:
//  warning C4065: switch statement contains 'default' but no 'case' labels
#if defined(WIN32) || defined(_XENON)
#pragma warning( disable : 4065 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4244 )
#pragma warning( disable : 4245 )
#pragma warning( disable : 4267 )
#pragma warning( disable : 4706 )
#pragma warning( disable : 4702 )
#endif

#if defined(_XENON)
    #include <xtl.h>
    #include <malloc.h>
    #include <NiD3DDefines.h>
#elif defined (WIN32)
    #include "windows.h"
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <math.h>
    #include <malloc.h>
    #include <NiD3DDefines.h>
#elif defined (_PS3)
    #include <ctype.h>    
#endif  //#if defined(_XENON)

    #include "NSFTextFile.h"
    #include "NSFParsedShader.h"

    #include <NiTArray.h>
    #include <NiShaderFactory.h>

    #include <NiTextureStage.h>
    #include <NiSemanticAdapterTable.h>
    
    #include <NSBStateGroup.h>
    #include <NSBRenderStates.h>
    #include <NSBStageAndSamplerStates.h>

    #include <NSBShader.h>
    #include <NSBAttributeDesc.h>
    #include <NSBAttributeTable.h>
    #include <NSBObjectTable.h>
    #include <NSBPackingDef.h>
    #include <NSBImplementation.h>
    #include <NSBStateGroup.h>
    #include <NSBConstantMap.h>
    #include <NSBPass.h>
    #include <NSBTextureStage.h>
    #include <NSBTexture.h>
    #include <NiOutputStreamDescriptor.h>
    #include <NiStreamOutSettings.h>
    
    unsigned int F2DW(float fValue)
    {
        union FloatIntRep
        {
            float f;
            unsigned int ui;
        } kValue;
        
        kValue.f = fValue;
        
        return kValue.ui;
    }
    
    #include <NSBUserDefinedDataSet.h>
    
    
    void NSFParsererror2(const char *s);
    void NSFParsererror(const char *s);
    int  yylex    (void);

    NSFTextFile* g_pkFile = 0;
    
    extern int NSFParserGetLineNumber();

//    #define _ENABLE_DEBUG_STRING_OUT_
    char g_szDSO[1024];
    bool g_bFirstDSOFileAccess = true;
    int g_iDSOIndent = 0;
    FILE* g_pfDSOFile = 0;
    void DebugStringOut(const char* pszOut, bool bIndent = true);

    #define YYDEBUG                 1
    #define NSFParsererror_VERBOSE  1
    #define YYERROR_VERBOSE         1
    #define YYMALLOC NiExternalMalloc
    #define YYFREE NiExternalFree

    #define ERR_INVALID_ENUM    "Invalid enumerant"
    #define ERR_INVALID_COMMAND "Invalid command in block"

    // Gamebryo
        
    #define FLOAT_ARRAY_SIZE        64
    #define MAX_QUOTE_LENGTH    6 * 1024

    NiTPrimitiveArray<float>* g_afValues;
    
    unsigned int ResetFloatValueArray(void);
    unsigned int AddFloatToValueArray(float fValue);
    
    void ResetFloatRangeArrays(void);
    unsigned int AddFloatToLowArray(float fValue);
    unsigned int AddFloatToHighArray(float fValue);
    
    // Ranges
    bool g_bRanged;
    unsigned int g_uiLow, g_uiHigh;
    unsigned int g_uiLowFloatValues;
    float g_afLowValues[FLOAT_ARRAY_SIZE];
    unsigned int g_uiHighFloatValues;
    float g_afHighValues[FLOAT_ARRAY_SIZE];
    
    typedef enum
    {
        CONSTMAP_VERTEX = 0,
        CONSTMAP_PIXEL,
        CONSTMAP_GEOMETRY
    } NiConstantMapMode;
    
    NiConstantMapMode g_eConstMapMode;
    unsigned int g_uiCurrentPlatforms;
    bool g_bConstantMapPlatformBlock = false;
    
    void AddObjectToObjectTable(NiShaderAttributeDesc::ObjectType eType,
        unsigned int uiIndex, const char* pcName, const char* pcDebugString);
    unsigned int DecodeAttribTypeString(char* pszAttribType);
    unsigned int DecodePlatformString(char* pszPlatform);
    bool AddAttributeToConstantMap(char* pszName, 
        unsigned int uiRegisterStart, unsigned int uiRegisterCount,
        unsigned int uiExtraNumber, bool bIsGlobal);
    bool SetupOperatorEntry(char* pszName, int iRegStart, int iRegCount, 
        char* pszEntry1, int iOperation, char* pszEntry2, bool bInverse, 
        bool bTranspose);
    NiShaderAttributeDesc::AttributeType DetermineOperatorResult(
        int iOperation, NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);
    NiShaderAttributeDesc::AttributeType DetermineResultMultiply(
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);
    NiShaderAttributeDesc::AttributeType DetermineResultDivide(
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);
    NiShaderAttributeDesc::AttributeType DetermineResultAdd(
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);
    NiShaderAttributeDesc::AttributeType DetermineResultSubtract(
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);

    void SetShaderProgramFile(NSBPass* pkPass, const char* pcFile,
        unsigned int uiPlatforms, NiGPUProgram::ProgramType eType);
    void SetShaderProgramEntryPoint(NSBPass* pkPass,
        const char* pcEntryPoint,unsigned int uiPlatforms,
        NiGPUProgram::ProgramType eType);
    void SetShaderProgramShaderTarget(NSBPass* pkPass,
        const char* pcShaderTarget, unsigned int uiPlatforms,
        NiGPUProgram::ProgramType eType);

    NiTPointerList<NSFParsedShader*> g_kParsedShaderList;
    NSFParsedShader* g_pkCurrShader = 0;

    // Binary Shader
    NSBShader* g_pkCurrNSBShader = 0;

    // Attribute Table
    bool g_bGlobalAttributes = false;
    NSBAttributeTable* g_pkCurrAttribTable = 0;

    // Object Table
    NSBObjectTable* g_pkCurrObjectTable = 0;

    // Packing Definition
    unsigned int g_uiCurrPDStream = 0;    
    bool g_bCurrPDFixedFunction = false;
    NSBPackingDef* g_pkCurrPackingDef = 0;

    // Requirements
    NSBRequirements* g_pkCurrRequirements = 0;

    // Implementation
    unsigned int g_uiCurrImplementation = 0;
    NSBImplementation* g_pkCurrImplementation = 0;

    // OutputStream
    NiOutputStreamDescriptor* g_pkCurrentOutputStreamDescriptor = 0;
    NiOutputStreamDescriptor::DataType g_eDataType = 
        NiOutputStreamDescriptor::DATATYPE_MAX;

    // RenderState Group
    NSBStateGroup* g_pkCurrRSGroup = 0;

    // ConstantMap
    unsigned int g_uiCurrImplemVSConstantMap = 0;
    unsigned int g_uiCurrImplemGSConstantMap = 0;
    unsigned int g_uiCurrImplemPSConstantMap = 0;
    unsigned int g_uiCurrPassVSConstantMap = 0;
    unsigned int g_uiCurrPassGSConstantMap = 0;
    unsigned int g_uiCurrPassPSConstantMap = 0;
    NSBConstantMap* g_pkCurrConstantMap = 0;

    // Pass
    unsigned int g_uiCurrPassIndex = 0;
    NSBPass* g_pkCurrPass = 0;

    // TextureStage
    NSBTextureStage* g_pkCurrTextureStage = 0;
    unsigned int g_uiCurrTextureSlot = 0;
    NSBTexture* g_pkCurrTexture = 0;
    
    bool g_bCurrStateValid = false;
    unsigned int g_uiCurrStateState = 0;
    unsigned int g_uiCurrStateValue = 0;
    bool g_bUseMapValue = false;

    NSBUserDefinedDataSet* g_pkCurrUDDataSet = 0;
    NSBUserDefinedDataBlock* g_pkCurrUDDataBlock = 0;
    


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)

typedef union YYSTYPE {
    float fval;
    int   ival;
    char* sval;
    unsigned int uival;
    unsigned long  dword;
    unsigned short word;
    unsigned char  byte;
    bool bval;
    unsigned int vers;
} YYSTYPE;
/* Line 196 of yacc.c.  */

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 219 of yacc.c.  */


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T) && (defined (__STDC__) || defined (__cplusplus))
# include <stddef.h> /* INFRINGES ON USER NAME SPACE */
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if defined (__STDC__) || defined (__cplusplus)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     define YYINCLUDED_STDLIB_H
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2005 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM ((YYSIZE_T) -1)
#  endif
#  ifdef __cplusplus
extern "C" {
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if (! defined (malloc) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if (! defined (free) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifdef __cplusplus
}
#  endif
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1255

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  322
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  271
/* YYNRULES -- Number of rules. */
#define YYNRULES  670
/* YYNRULES -- Number of states. */
#define YYNSTATES  1153

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   574

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned short int yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     320,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   321,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     6,     8,     9,    10,    19,    22,    23,
      25,    28,    30,    32,    34,    36,    38,    40,    42,    44,
      45,    47,    49,    52,    53,    55,    56,    58,    60,    62,
      64,    68,    69,    90,   107,   108,   129,   130,   147,   148,
     161,   162,   167,   168,   173,   178,   180,   184,   186,   218,
     219,   225,   226,   232,   233,   237,   240,   242,   244,   246,
     248,   250,   252,   254,   256,   258,   260,   262,   264,   266,
     271,   272,   279,   280,   288,   289,   297,   298,   306,   307,
     315,   316,   323,   324,   331,   332,   339,   345,   350,   351,
     357,   360,   362,   364,   366,   368,   370,   372,   374,   376,
     378,   380,   382,   384,   388,   392,   396,   400,   404,   408,
     412,   416,   420,   424,   428,   429,   436,   439,   440,   442,
     444,   446,   448,   450,   452,   454,   456,   458,   460,   462,
     464,   466,   468,   470,   472,   474,   476,   478,   480,   482,
     484,   486,   488,   490,   492,   494,   497,   499,   501,   503,
     505,   507,   509,   511,   513,   515,   517,   519,   521,   523,
     525,   527,   529,   531,   533,   535,   537,   539,   541,   543,
     545,   547,   549,   551,   553,   555,   557,   559,   561,   563,
     565,   567,   569,   571,   573,   575,   577,   579,   581,   583,
     585,   587,   589,   591,   594,   596,   599,   602,   605,   613,
     618,   625,   629,   630,   636,   639,   641,   652,   653,   659,
     662,   664,   666,   668,   671,   673,   675,   677,   679,   681,
     684,   688,   692,   696,   700,   704,   708,   711,   713,   715,
     717,   719,   721,   723,   725,   727,   728,   729,   738,   744,
     750,   755,   760,   765,   770,   771,   778,   779,   786,   787,
     793,   803,   805,   807,   809,   811,   814,   815,   817,   818,
     820,   821,   827,   828,   834,   835,   841,   845,   848,   851,
     854,   858,   861,   863,   867,   871,   875,   877,   880,   882,
     886,   890,   894,   895,   896,   906,   908,   910,   913,   916,
     919,   923,   926,   928,   932,   936,   940,   942,   945,   947,
     951,   955,   956,   957,   967,   969,   971,   974,   977,   980,
     984,   987,   989,   993,   997,  1001,  1003,  1006,  1008,  1012,
    1016,  1017,  1018,  1028,  1030,  1032,  1033,  1039,  1042,  1044,
    1046,  1048,  1050,  1052,  1054,  1056,  1058,  1060,  1062,  1065,
    1069,  1073,  1077,  1081,  1085,  1093,  1099,  1103,  1105,  1109,
    1113,  1117,  1121,  1123,  1125,  1127,  1129,  1132,  1134,  1136,
    1138,  1139,  1147,  1148,  1150,  1153,  1155,  1157,  1159,  1161,
    1163,  1165,  1168,  1170,  1172,  1174,  1176,  1178,  1180,  1182,
    1184,  1186,  1188,  1190,  1192,  1194,  1196,  1198,  1200,  1202,
    1204,  1207,  1211,  1215,  1219,  1223,  1225,  1227,  1229,  1231,
    1233,  1235,  1237,  1239,  1242,  1246,  1250,  1254,  1258,  1262,
    1266,  1270,  1274,  1278,  1282,  1286,  1290,  1294,  1298,  1302,
    1306,  1310,  1314,  1318,  1322,  1326,  1330,  1335,  1340,  1345,
    1349,  1353,  1355,  1357,  1359,  1361,  1362,  1364,  1366,  1368,
    1370,  1372,  1374,  1376,  1378,  1380,  1382,  1384,  1386,  1388,
    1390,  1392,  1394,  1396,  1398,  1400,  1402,  1404,  1406,  1408,
    1410,  1412,  1414,  1416,  1419,  1422,  1425,  1428,  1431,  1434,
    1437,  1440,  1441,  1443,  1446,  1449,  1451,  1453,  1454,  1456,
    1458,  1460,  1462,  1464,  1467,  1471,  1475,  1476,  1481,  1482,
    1484,  1486,  1488,  1490,  1492,  1494,  1497,  1498,  1506,  1507,
    1514,  1515,  1517,  1520,  1522,  1524,  1526,  1528,  1531,  1533,
    1535,  1537,  1539,  1541,  1543,  1545,  1547,  1549,  1551,  1553,
    1555,  1557,  1559,  1561,  1563,  1565,  1568,  1572,  1576,  1580,
    1584,  1588,  1592,  1596,  1600,  1604,  1608,  1612,  1616,  1620,
    1624,  1628,  1632,  1636,  1640,  1644,  1648,  1652,  1656,  1660,
    1664,  1668,  1672,  1676,  1678,  1680,  1682,  1684,  1686,  1688,
    1690,  1693,  1695,  1697,  1699,  1701,  1703,  1705,  1707,  1709,
    1711,  1713,  1715,  1718,  1720,  1722,  1725,  1727,  1729,  1731,
    1733,  1736,  1737,  1745,  1746,  1753,  1754,  1756,  1759,  1761,
    1763,  1766,  1770,  1774,  1778,  1782,  1785,  1787,  1789,  1791,
    1793,  1795,  1797,  1799,  1801,  1803,  1805,  1807,  1808,  1815,
    1816,  1818,  1821,  1823,  1825,  1827,  1829,  1831,  1833,  1835,
    1837,  1839,  1841,  1843,  1844,  1845,  1854,  1855,  1861,  1864,
    1866,  1868,  1870,  1874,  1879,  1882,  1884,  1886,  1887,  1894,
    1897,  1899,  1901,  1903,  1905,  1909,  1910,  1916,  1919,  1921,
    1928,  1930,  1932,  1934,  1936,  1938,  1940,  1944,  1946,  1948,
    1950,  1952,  1954,  1956,  1960,  1964,  1968,  1972,  1974,  1977,
    1978
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
     323,     0,    -1,   323,   324,    -1,   324,    -1,    -1,    -1,
      31,    27,     5,   325,   332,   326,   327,     6,    -1,     1,
     320,    -1,    -1,   328,    -1,   328,   329,    -1,   329,    -1,
     345,    -1,   371,    -1,   347,    -1,   386,    -1,   560,    -1,
     571,    -1,   591,    -1,    -1,    27,    -1,    28,    -1,   331,
      28,    -1,    -1,   331,    -1,    -1,    28,    -1,     4,    -1,
      27,    -1,    28,    -1,    11,    27,    12,    -1,    -1,     9,
       7,    25,    15,    25,    15,    25,    15,    25,     8,     7,
      25,    15,    25,    15,    25,    15,    25,     8,    10,    -1,
       9,     7,    25,    15,    25,    15,    25,     8,     7,    25,
      15,    25,    15,    25,     8,    10,    -1,    -1,     9,     7,
      25,    15,    25,    15,    25,    15,    25,     8,     7,    25,
      15,    25,    15,    25,    15,    25,     8,    10,    -1,    -1,
       9,     7,    25,    15,    25,    15,    25,     8,     7,    25,
      15,    25,    15,    25,     8,    10,    -1,    -1,     9,     7,
      25,    15,    25,     8,     7,    25,    15,    25,     8,    10,
      -1,    -1,     9,    25,    25,    10,    -1,    -1,     9,    26,
      26,    10,    -1,     9,    25,    25,    10,    -1,   343,    -1,
     343,    15,   342,    -1,    25,    -1,    25,    15,    25,    15,
      25,    15,    25,    15,    25,    15,    25,    15,    25,    15,
      25,    15,    25,    15,    25,    15,    25,    15,    25,    15,
      25,    15,    25,    15,    25,    15,    25,    -1,    -1,    35,
       5,   346,   350,     6,    -1,    -1,    36,     5,   348,   350,
       6,    -1,    -1,     9,    26,    10,    -1,   350,   351,    -1,
     351,    -1,   353,    -1,   354,    -1,   356,    -1,   358,    -1,
     360,    -1,   362,    -1,   364,    -1,   366,    -1,   368,    -1,
     370,    -1,    32,    -1,    33,    -1,    38,    27,   352,    29,
      -1,    -1,    40,   355,    27,   352,    26,   341,    -1,    -1,
      41,   357,    27,   349,   352,   342,   340,    -1,    -1,    42,
     359,    27,   349,   352,   342,   339,    -1,    -1,    43,   361,
      27,   349,   352,   342,   338,    -1,    -1,    44,   363,    27,
     349,   352,   342,   337,    -1,    -1,    45,   365,    27,   349,
     352,   342,    -1,    -1,    46,   367,    27,   349,   352,   342,
      -1,    -1,    47,   369,    27,   352,   342,   336,    -1,    48,
      27,   352,    26,   333,    -1,    48,    27,   352,   333,    -1,
      -1,   305,     5,   372,   373,     6,    -1,   373,   374,    -1,
     374,    -1,   375,    -1,   376,    -1,   377,    -1,   378,    -1,
     379,    -1,   380,    -1,   381,    -1,   382,    -1,   383,    -1,
     384,    -1,   385,    -1,   306,    26,    27,    -1,   307,    26,
      27,    -1,   308,    26,    27,    -1,   309,    26,    27,    -1,
     310,    26,    27,    -1,   311,    26,    27,    -1,   312,    26,
      27,    -1,   314,    26,    27,    -1,   315,    26,    27,    -1,
     316,    26,    27,    -1,   317,    26,    27,    -1,    -1,    49,
      27,     5,   387,   393,     6,    -1,    49,    27,    -1,    -1,
      80,    -1,    81,    -1,    82,    -1,    83,    -1,    84,    -1,
      85,    -1,    86,    -1,    87,    -1,    88,    -1,    89,    -1,
      90,    -1,    91,    -1,    92,    -1,    93,    -1,    94,    -1,
      95,    -1,    96,    -1,    97,    -1,    98,    -1,    99,    -1,
     100,    -1,   101,    -1,   102,    -1,   103,    -1,   104,    -1,
     105,    -1,   106,    -1,     1,   320,    -1,    53,    -1,    54,
      -1,    55,    -1,    56,    -1,    57,    -1,    58,    -1,    59,
      -1,    60,    -1,    61,    -1,    62,    -1,    63,    -1,    64,
      -1,    65,    -1,    66,    -1,    67,    -1,    68,    -1,    69,
      -1,    70,    -1,    71,    -1,    72,    -1,    73,    -1,    74,
      -1,    75,    -1,    76,    -1,    77,    -1,    78,    -1,   107,
      -1,   108,    -1,   109,    -1,   110,    -1,   111,    -1,   112,
      -1,   113,    -1,   114,    -1,   115,    -1,   116,    -1,   117,
      -1,   118,    -1,   119,    -1,   120,    -1,   121,    -1,   122,
      -1,   123,    -1,   124,    -1,   125,    -1,   126,    -1,   127,
      -1,   393,   394,    -1,   394,    -1,     1,   320,    -1,    50,
      26,    -1,    51,    29,    -1,    79,    26,    26,   389,   391,
     392,    26,    -1,    79,    26,    26,   389,    -1,   390,    26,
     389,   391,   392,    26,    -1,   390,    26,   389,    -1,    -1,
      52,     5,   396,   397,     6,    -1,   397,   398,    -1,   398,
      -1,    26,   334,    17,    26,   334,    17,    26,    16,    26,
      18,    -1,    -1,   128,     5,   400,   401,     6,    -1,   401,
     402,    -1,   402,    -1,   403,    -1,   404,    -1,   404,    34,
      -1,   407,    -1,   409,    -1,   408,    -1,   405,    -1,   406,
      -1,     1,   320,    -1,    27,    14,   335,    -1,    27,    14,
      27,    -1,    27,    14,    29,    -1,    27,    14,    25,    -1,
      27,    14,    24,    -1,    27,    14,    26,    -1,   410,   411,
      -1,   411,    -1,   415,    -1,   416,    -1,   417,    -1,   419,
      -1,   421,    -1,   423,    -1,   412,    -1,    -1,    -1,     5,
     413,   150,    14,   470,   414,   410,     6,    -1,   129,   144,
      26,    26,    26,    -1,   129,   144,   334,    26,    26,    -1,
     129,   334,    26,    26,    -1,   129,   334,   334,    26,    -1,
     319,    27,    27,    26,    -1,   319,    27,    27,    27,    -1,
      -1,   130,   418,   334,    26,    26,    26,    -1,    -1,   131,
     420,   334,    26,    26,   342,    -1,    -1,   132,   422,   334,
      26,    26,    -1,   133,    27,    26,    26,    27,   424,    27,
     426,   425,    -1,    20,    -1,    21,    -1,    22,    -1,    23,
      -1,     1,   320,    -1,    -1,   205,    -1,    -1,   204,    -1,
      -1,   134,     5,   428,   410,     6,    -1,    -1,   136,     5,
     430,   410,     6,    -1,    -1,   138,     5,   432,   410,     6,
      -1,   173,    14,   334,    -1,   135,     4,    -1,   135,    27,
      -1,   135,    28,    -1,   434,    27,    27,    -1,   434,    27,
      -1,   434,    -1,   140,    14,     4,    -1,   140,    14,    27,
      -1,   140,    14,    28,    -1,   438,    -1,   438,   437,    -1,
     436,    -1,   141,    14,    27,    -1,   142,    14,    27,    -1,
     143,    14,    29,    -1,    -1,    -1,   135,     5,   440,   150,
      14,   470,   441,   437,     6,    -1,   439,    -1,   435,    -1,
     137,     4,    -1,   137,    27,    -1,   137,    28,    -1,   443,
      27,    27,    -1,   443,    27,    -1,   443,    -1,   140,    14,
       4,    -1,   140,    14,    27,    -1,   140,    14,    28,    -1,
     447,    -1,   447,   446,    -1,   445,    -1,   141,    14,    27,
      -1,   142,    14,    27,    -1,    -1,    -1,   137,     5,   449,
     150,    14,   470,   450,   446,     6,    -1,   448,    -1,   444,
      -1,   139,     4,    -1,   139,    27,    -1,   139,    28,    -1,
     452,    27,    27,    -1,   452,    27,    -1,   452,    -1,   140,
      14,     4,    -1,   140,    14,    27,    -1,   140,    14,    28,
      -1,   456,    -1,   456,   455,    -1,   454,    -1,   141,    14,
      27,    -1,   142,    14,    27,    -1,    -1,    -1,   139,     5,
     458,   150,    14,   470,   459,   455,     6,    -1,   457,    -1,
     453,    -1,    -1,   145,     5,   462,   463,     6,    -1,   463,
     464,    -1,   464,    -1,   465,    -1,   466,    -1,   467,    -1,
     468,    -1,   469,    -1,   472,    -1,   474,    -1,   473,    -1,
     471,    -1,     1,   320,    -1,   146,    14,    30,    -1,   147,
      14,    30,    -1,   148,    14,    30,    -1,   149,    14,    30,
      -1,   150,    14,   470,    -1,    27,    13,    27,    13,    27,
      13,    27,    -1,    27,    13,    27,    13,    27,    -1,    27,
      13,    27,    -1,    27,    -1,    27,    14,    29,    -1,   151,
      14,    26,    -1,   153,    14,    26,    -1,   152,    14,   475,
      -1,   154,    -1,   155,    -1,   157,    -1,   156,    -1,     1,
     320,    -1,   477,    -1,   517,    -1,   546,    -1,    -1,   175,
      26,   330,     5,   478,   479,     6,    -1,    -1,   480,    -1,
     480,   481,    -1,   481,    -1,   485,    -1,   513,    -1,   482,
      -1,   483,    -1,   484,    -1,   484,    34,    -1,   488,    -1,
     489,    -1,   490,    -1,   491,    -1,   492,    -1,   493,    -1,
     494,    -1,   495,    -1,   496,    -1,   497,    -1,   498,    -1,
     499,    -1,   500,    -1,   501,    -1,   502,    -1,   503,    -1,
     504,    -1,   505,    -1,     1,   320,    -1,   176,    14,   486,
      -1,   176,    14,   487,    -1,   176,    14,   335,    -1,   176,
      14,    27,    -1,   254,    -1,   255,    -1,   256,    -1,   257,
      -1,   258,    -1,   259,    -1,   260,    -1,   261,    -1,   262,
      26,    -1,   177,    14,   508,    -1,   178,    14,   509,    -1,
     179,    14,   509,    -1,   180,    14,   509,    -1,   181,    14,
     508,    -1,   182,    14,   509,    -1,   183,    14,   509,    -1,
     184,    14,   509,    -1,   185,    14,   509,    -1,   186,    14,
      24,    -1,   187,    14,    25,    -1,   187,    14,   335,    -1,
     188,    14,    25,    -1,   188,    14,   335,    -1,   189,    14,
      25,    -1,   189,    14,   335,    -1,   190,    14,    25,    -1,
     190,    14,   335,    -1,   191,    14,    25,    -1,   191,    14,
     335,    -1,   192,    14,    25,    -1,   192,    14,   335,    -1,
     193,    14,   512,    26,    -1,   193,    14,   512,   203,    -1,
     194,    14,   506,   507,    -1,   194,    14,   201,    -1,   194,
      14,   196,    -1,   197,    -1,   198,    -1,   199,    -1,   200,
      -1,    -1,   201,    -1,   219,    -1,   220,    -1,   221,    -1,
     222,    -1,   223,    -1,   224,    -1,   225,    -1,   226,    -1,
     227,    -1,   228,    -1,   229,    -1,   230,    -1,   231,    -1,
     232,    -1,   233,    -1,   234,    -1,   235,    -1,   236,    -1,
     237,    -1,   238,    -1,   239,    -1,   240,    -1,   241,    -1,
     242,    -1,   243,    -1,   244,    -1,     1,   320,    -1,   245,
     510,    -1,   246,   510,    -1,   247,   510,    -1,   248,   510,
      -1,   249,   510,    -1,   250,   510,    -1,   251,   510,    -1,
      -1,   511,    -1,   252,   253,    -1,   253,   252,    -1,   252,
      -1,   253,    -1,    -1,   214,    -1,   215,    -1,   216,    -1,
     217,    -1,   218,    -1,     1,   320,    -1,   195,    14,   514,
      -1,   206,   516,    27,    -1,    -1,   207,   515,   516,   344,
      -1,    -1,   208,    -1,   209,    -1,   210,    -1,   211,    -1,
     212,    -1,   213,    -1,     1,   320,    -1,    -1,   263,    26,
     330,     5,   518,   520,     6,    -1,    -1,   263,   330,     5,
     519,   520,     6,    -1,    -1,   521,    -1,   521,   522,    -1,
     522,    -1,   523,    -1,   524,    -1,   485,    -1,   524,    34,
      -1,   525,    -1,   526,    -1,   527,    -1,   528,    -1,   529,
      -1,   530,    -1,   531,    -1,   532,    -1,   533,    -1,   534,
      -1,   535,    -1,   536,    -1,   537,    -1,   538,    -1,   539,
      -1,   540,    -1,   541,    -1,     1,   320,    -1,   264,    14,
     542,    -1,   265,    14,   542,    -1,   266,    14,   542,    -1,
     267,    14,    24,    -1,   267,    14,   335,    -1,   268,    14,
     543,    -1,   269,    14,   543,    -1,   270,    14,   543,    -1,
     271,    14,    26,    -1,   271,    14,   335,    -1,   272,    14,
      26,    -1,   272,    14,   335,    -1,   273,    14,    26,    -1,
     273,    14,   335,    -1,   274,    14,    26,    -1,   274,    14,
     335,    -1,   275,    14,    26,    -1,   275,    14,   335,    -1,
     276,    14,    26,    -1,   276,    14,   335,    -1,   277,    14,
     544,    -1,   277,    14,   335,    -1,   278,    14,   545,    -1,
     279,    14,    27,    -1,   279,    14,   321,    -1,   280,    14,
      24,    -1,   280,    14,   335,    -1,   281,    -1,   282,    -1,
     283,    -1,   284,    -1,   285,    -1,   286,    -1,   318,    -1,
       1,   320,    -1,   287,    -1,   288,    -1,   289,    -1,   290,
      -1,   291,    -1,   292,    -1,   295,    -1,   293,    -1,   294,
      -1,   296,    -1,   318,    -1,     1,   320,    -1,   297,    -1,
     298,    -1,     1,   320,    -1,   299,    -1,   300,    -1,   301,
      -1,   302,    -1,     1,   320,    -1,    -1,   303,    26,   330,
       5,   547,   549,     6,    -1,    -1,   303,   330,     5,   548,
     549,     6,    -1,    -1,   550,    -1,   550,   551,    -1,   551,
      -1,   552,    -1,     1,   320,    -1,   304,    14,   486,    -1,
     304,    14,   487,    -1,   304,    14,   335,    -1,   304,    14,
      27,    -1,   553,   554,    -1,   554,    -1,   399,    -1,   442,
      -1,   427,    -1,   451,    -1,   429,    -1,   460,    -1,   431,
      -1,   476,    -1,   591,    -1,   563,    -1,    -1,   174,   330,
       5,   556,   553,     6,    -1,    -1,   558,    -1,   558,   559,
      -1,   559,    -1,   399,    -1,   388,    -1,   555,    -1,   461,
      -1,   427,    -1,   429,    -1,   431,    -1,   433,    -1,   591,
      -1,   395,    -1,    -1,    -1,   159,    27,     5,   561,   332,
     562,   557,     6,    -1,    -1,   161,     5,   564,   565,     6,
      -1,   565,   566,    -1,   566,    -1,   568,    -1,   567,    -1,
     163,    14,    29,    -1,   162,     5,   569,     6,    -1,   569,
     570,    -1,   570,    -1,    27,    -1,    -1,   160,    27,     5,
     572,   573,     6,    -1,   573,   574,    -1,   574,    -1,   575,
      -1,   584,    -1,   576,    -1,   164,    14,    26,    -1,    -1,
     169,     5,   577,   578,     6,    -1,   578,   579,    -1,   579,
      -1,   580,    26,   334,    17,    26,    18,    -1,   581,    -1,
     582,    -1,   583,    -1,   170,    -1,   171,    -1,   172,    -1,
     165,    14,   585,    -1,   586,    -1,   587,    -1,   588,    -1,
     166,    -1,   167,    -1,   168,    -1,   334,    14,    26,    -1,
     334,    14,    29,    -1,   334,    14,   342,    -1,   334,    14,
     334,    -1,   589,    -1,   589,   590,    -1,    -1,   158,    27,
       5,   592,   590,     6,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   540,   540,   541,   548,   591,   545,   612,   619,   621,
     625,   626,   630,   631,   632,   633,   634,   635,   636,   643,
     644,   648,   652,   665,   666,   670,   671,   672,   676,   677,
     681,   689,   692,   711,   732,   735,   758,   761,   782,   785,
     800,   803,   813,   816,   822,   836,   837,   841,   848,   880,
     878,   909,   907,   938,   942,   950,   951,   956,   957,   958,
     959,   960,   961,   962,   963,   964,   965,   971,   972,   977,
    1008,  1007,  1072,  1071,  1162,  1161,  1253,  1252,  1346,  1345,
    1441,  1440,  1532,  1531,  1620,  1619,  1774,  1798,  1833,  1831,
    1859,  1860,  1865,  1866,  1867,  1868,  1869,  1870,  1871,  1872,
    1873,  1874,  1875,  1880,  1894,  1908,  1922,  1936,  1950,  1964,
    1978,  1992,  2006,  2020,  2039,  2036,  2071,  2108,  2109,  2110,
    2111,  2112,  2113,  2114,  2115,  2116,  2117,  2118,  2119,  2120,
    2121,  2122,  2123,  2124,  2125,  2126,  2127,  2128,  2129,  2130,
    2131,  2132,  2133,  2134,  2135,  2136,  2144,  2146,  2148,  2150,
    2152,  2154,  2156,  2158,  2160,  2162,  2164,  2166,  2168,  2170,
    2172,  2174,  2176,  2178,  2180,  2182,  2184,  2186,  2188,  2190,
    2192,  2194,  2199,  2201,  2203,  2205,  2207,  2209,  2211,  2216,
    2218,  2220,  2222,  2224,  2226,  2228,  2230,  2232,  2234,  2236,
    2238,  2240,  2242,  2247,  2248,  2249,  2257,  2263,  2269,  2322,
    2357,  2402,  2443,  2441,  2461,  2462,  2469,  2502,  2500,  2535,
    2536,  2540,  2550,  2563,  2567,  2568,  2569,  2570,  2571,  2572,
    2580,  2613,  2669,  2711,  2753,  2791,  2835,  2836,  2840,  2841,
    2842,  2843,  2844,  2845,  2846,  2851,  2863,  2850,  2876,  2916,
    2956,  2996,  3039,  3117,  3200,  3199,  3226,  3225,  3287,  3286,
    3312,  3344,  3346,  3348,  3350,  3352,  3360,  3361,  3365,  3366,
    3374,  3372,  3413,  3411,  3452,  3450,  3490,  3505,  3512,  3519,
    3529,  3546,  3558,  3567,  3568,  3569,  3573,  3574,  3579,  3586,
    3593,  3600,  3612,  3618,  3610,  3631,  3632,  3639,  3646,  3653,
    3663,  3680,  3691,  3700,  3701,  3702,  3706,  3707,  3712,  3719,
    3726,  3738,  3744,  3736,  3757,  3758,  3765,  3772,  3779,  3789,
    3806,  3817,  3826,  3827,  3828,  3832,  3833,  3838,  3845,  3852,
    3864,  3870,  3862,  3883,  3884,  3893,  3891,  3930,  3931,  3935,
    3936,  3937,  3938,  3939,  3940,  3941,  3942,  3943,  3944,  3952,
    3963,  3984,  4004,  4015,  4026,  4070,  4105,  4132,  4153,  4191,
    4202,  4215,  4230,  4231,  4232,  4233,  4244,  4254,  4255,  4256,
    4265,  4263,  4296,  4298,  4302,  4303,  4307,  4308,  4309,  4313,
    4331,  4352,  4356,  4357,  4358,  4359,  4360,  4361,  4362,  4363,
    4364,  4365,  4366,  4367,  4368,  4369,  4370,  4371,  4372,  4373,
    4374,  4383,  4388,  4393,  4482,  4515,  4516,  4517,  4518,  4519,
    4520,  4521,  4522,  4527,  4534,  4547,  4560,  4573,  4586,  4599,
    4612,  4625,  4638,  4651,  4668,  4678,  4688,  4698,  4708,  4718,
    4728,  4738,  4748,  4758,  4768,  4778,  4788,  4799,  4816,  4834,
    4848,  4865,  4866,  4867,  4868,  4873,  4874,  4879,  4883,  4887,
    4891,  4895,  4899,  4903,  4907,  4911,  4915,  4919,  4923,  4927,
    4931,  4935,  4939,  4943,  4947,  4951,  4955,  4959,  4963,  4967,
    4971,  4975,  4979,  4983,  4991,  4994,  4997,  5000,  5003,  5006,
    5009,  5015,  5016,  5020,  5026,  5031,  5033,  5039,  5042,  5046,
    5050,  5054,  5058,  5062,  5070,  5077,  5094,  5093,  5137,  5138,
    5140,  5142,  5144,  5146,  5148,  5150,  5163,  5161,  5190,  5188,
    5215,  5217,  5221,  5222,  5226,  5245,  5264,  5272,  5276,  5277,
    5278,  5279,  5280,  5281,  5282,  5283,  5284,  5285,  5286,  5287,
    5288,  5289,  5290,  5291,  5292,  5293,  5301,  5330,  5359,  5399,
    5410,  5421,  5450,  5479,  5508,  5519,  5530,  5541,  5552,  5563,
    5574,  5585,  5596,  5607,  5618,  5629,  5640,  5650,  5665,  5678,
    5690,  5703,  5713,  5728,  5730,  5732,  5734,  5736,  5738,  5740,
    5745,  5753,  5755,  5757,  5759,  5761,  5763,  5765,  5767,  5769,
    5771,  5773,  5778,  5786,  5787,  5788,  5796,  5797,  5798,  5799,
    5800,  5813,  5811,  5839,  5837,  5864,  5866,  5870,  5871,  5875,
    5876,  5884,  5889,  5894,  5983,  6018,  6019,  6023,  6024,  6025,
    6026,  6027,  6028,  6029,  6030,  6031,  6032,  6038,  6036,  6069,
    6071,  6075,  6076,  6080,  6081,  6082,  6083,  6084,  6085,  6086,
    6087,  6088,  6089,  6095,  6114,  6093,  6144,  6142,  6151,  6152,
    6155,  6156,  6159,  6170,  6176,  6177,  6180,  6198,  6195,  6218,
    6219,  6222,  6223,  6224,  6227,  6236,  6235,  6246,  6247,  6252,
    6276,  6277,  6278,  6281,  6287,  6293,  6300,  6303,  6304,  6305,
    6308,  6315,  6322,  6334,  6365,  6396,  6488,  6523,  6524,  6530,
    6528
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "EOLN", "PATH", "L_ACCOLADE",
  "R_ACCOLADE", "L_PARENTHESE", "R_PARENTHESE", "L_BRACKET", "R_BRACKET",
  "L_ANGLEBRACKET", "R_ANGLEBRACKET", "OR", "ASSIGN", "COMMA",
  "NSF_AT_SYMBOL", "NSF_COLON", "NSF_SEMICOLON", "UNDERSCORE", "ASTERIK",
  "FORWARDSLASH", "PLUS", "MINUS", "N_HEX", "N_FLOAT", "N_INT", "N_STRING",
  "N_QUOTE", "N_BOOL", "N_VERSION", "NSFSHADER", "ARTIST", "HIDDEN",
  "SAVE", "ATTRIBUTES", "GLOBALATTRIBUTES", "ATTRIB", "ATTRIB_BOOL",
  "ATTRIB_STRING", "ATTRIB_UINT", "ATTRIB_FLOAT", "ATTRIB_POINT2",
  "ATTRIB_POINT3", "ATTRIB_POINT4", "ATTRIB_MATRIX3", "ATTRIB_TRANSFORM",
  "ATTRIB_COLOR", "ATTRIB_TEXTURE", "PACKINGDEF", "PD_STREAM",
  "PD_FIXEDFUNCTION", "SEMANTICADAPTERTABLE", "PDP_POSITION",
  "PDP_POSITION0", "PDP_POSITION1", "PDP_POSITION2", "PDP_POSITION3",
  "PDP_POSITION4", "PDP_POSITION5", "PDP_POSITION6", "PDP_POSITION7",
  "PDP_BLENDWEIGHTS", "PDP_BLENDINDICES", "PDP_NORMAL", "PDP_POINTSIZE",
  "PDP_COLOR", "PDP_COLOR2", "PDP_TEXCOORD0", "PDP_TEXCOORD1",
  "PDP_TEXCOORD2", "PDP_TEXCOORD3", "PDP_TEXCOORD4", "PDP_TEXCOORD5",
  "PDP_TEXCOORD6", "PDP_TEXCOORD7", "PDP_NORMAL2", "PDP_TANGENT",
  "PDP_BINORMAL", "PDP_EXTRADATA", "PDT_FLOAT1", "PDT_FLOAT2",
  "PDT_FLOAT3", "PDT_FLOAT4", "PDT_UBYTECOLOR", "PDT_SHORT1", "PDT_SHORT2",
  "PDT_SHORT3", "PDT_SHORT4", "PDT_UBYTE4", "PDT_NORMSHORT1",
  "PDT_NORMSHORT2", "PDT_NORMSHORT3", "PDT_NORMSHORT4", "PDT_NORMPACKED3",
  "PDT_PBYTE1", "PDT_PBYTE2", "PDT_PBYTE3", "PDT_PBYTE4", "PDT_FLOAT2H",
  "PDT_NORMUBYTE4", "PDT_NORMUSHORT2", "PDT_NORMUSHORT4", "PDT_UDEC3",
  "PDT_NORMDEC3", "PDT_FLOAT16_2", "PDT_FLOAT16_4", "PDTESS_DEFAULT",
  "PDTESS_PARTIALU", "PDTESS_PARTIALV", "PDTESS_CROSSUV", "PDTESS_UV",
  "PDTESS_LOOKUP", "PDTESS_LOOKUPPRESAMPLED", "PDU_POSITION",
  "PDU_BLENDWEIGHT", "PDU_BLENDINDICES", "PDU_NORMAL", "PDU_PSIZE",
  "PDU_TEXCOORD", "PDU_TANGENT", "PDU_BINORMAL", "PDU_TESSFACTOR",
  "PDU_POSITIONT", "PDU_COLOR", "PDU_FOG", "PDU_DEPTH", "PDU_SAMPLE",
  "RENDERSTATES", "CMDEFINED", "CMATTRIBUTE", "CMCONSTANT", "CMGLOBAL",
  "CMOPERATOR", "VSCONSTANTMAP", "VSPROGRAM", "GSCONSTANTMAP", "GSPROGRAM",
  "PSCONSTANTMAP", "PSPROGRAM", "PROGRAM", "ENTRYPOINT", "SHADERTARGET",
  "SOFTWAREVP", "SKINBONEMATRIX3", "REQUIREMENTS", "VSVERSION",
  "GSVERSION", "PSVERSION", "USERVERSION", "PLATFORM", "BONESPERPARTITION",
  "BINORMALTANGENTMETHOD", "BINORMALTANGENTUVSOURCE", "NBTMETHOD_NONE",
  "NBTMETHOD_NI", "NBTMETHOD_MAX", "NBTMETHOD_ATI", "USERDEFINEDDATA",
  "IMPLEMENTATION", "OUTPUTSTREAM", "STREAMOUTPUT", "STREAMOUTTARGETS",
  "STREAMOUTAPPEND", "MAXVERTEXCOUNT", "OUTPUTPRIMTYPE", "_POINT", "_LINE",
  "_TRIANGLE", "VERTEXFORMAT", "FMT_FLOAT", "FMT_INT", "FMT_UINT",
  "CLASSNAME", "PASS", "STAGE", "TSS_TEXTURE", "TSS_COLOROP",
  "TSS_COLORARG0", "TSS_COLORARG1", "TSS_COLORARG2", "TSS_ALPHAOP",
  "TSS_ALPHAARG0", "TSS_ALPHAARG1", "TSS_ALPHAARG2", "TSS_RESULTARG",
  "TSS_CONSTANT_DEPRECATED", "TSS_BUMPENVMAT00", "TSS_BUMPENVMAT01",
  "TSS_BUMPENVMAT10", "TSS_BUMPENVMAT11", "TSS_BUMPENVLSCALE",
  "TSS_BUMPENVLOFFSET", "TSS_TEXCOORDINDEX", "TSS_TEXTURETRANSFORMFLAGS",
  "TSS_TEXTRANSMATRIX", "TTFF_DISABLE", "TTFF_COUNT1", "TTFF_COUNT2",
  "TTFF_COUNT3", "TTFF_COUNT4", "TTFF_PROJECTED", "PROJECTED",
  "USEMAPINDEX", "INVERSE", "TRANSPOSE", "TTSRC_GLOBAL", "TTSRC_CONSTANT",
  "TT_WORLD_PARALLEL", "TT_WORLD_PERSPECTIVE", "TT_WORLD_SPHERE_MAP",
  "TT_CAMERA_SPHERE_MAP", "TT_SPECULAR_CUBE_MAP", "TT_DIFFUSE_CUBE_MAP",
  "TCI_PASSTHRU", "TCI_CAMERASPACENORMAL", "TCI_CAMERASPACEPOSITION",
  "TCI_CAMERASPACEREFLECT", "TCI_SPHEREMAP", "TOP_DISABLE",
  "TOP_SELECTARG1", "TOP_SELECTARG2", "TOP_MODULATE", "TOP_MODULATE2X",
  "TOP_MODULATE4X", "TOP_ADD", "TOP_ADDSIGNED", "TOP_ADDSIGNED2X",
  "TOP_SUBTRACT", "TOP_ADDSMOOTH", "TOP_BLENDDIFFUSEALPHA",
  "TOP_BLENDTEXTUREALPHA", "TOP_BLENDFACTORALPHA",
  "TOP_BLENDTEXTUREALPHAPM", "TOP_BLENDCURRENTALPHA", "TOP_PREMODULATE",
  "TOP_MODULATEALPHA_ADDCOLOR", "TOP_MODULATECOLOR_ADDALPHA",
  "TOP_MODULATEINVALPHA_ADDCOLOR", "TOP_MODULATEINVCOLOR_ADDALPHA",
  "TOP_BUMPENVMAP", "TOP_BUMPENVMAPLUMINANCE", "TOP_DOTPRODUCT3",
  "TOP_MULTIPLYADD", "TOP_LERP", "TA_CURRENT", "TA_DIFFUSE",
  "TA_SELECTMASK", "TA_SPECULAR", "TA_TEMP", "TA_TEXTURE", "TA_TFACTOR",
  "TA_ALPHAREPLICATE", "TA_COMPLEMENT", "NTM_BASE", "NTM_DARK",
  "NTM_DETAIL", "NTM_GLOSS", "NTM_GLOW", "NTM_BUMP", "NTM_NORMAL",
  "NTM_PARALLAX", "NTM_DECAL", "SAMPLER", "TSAMP_ADDRESSU",
  "TSAMP_ADDRESSV", "TSAMP_ADDRESSW", "TSAMP_BORDERCOLOR",
  "TSAMP_MAGFILTER", "TSAMP_MINFILTER", "TSAMP_MIPFILTER",
  "TSAMP_MIPMAPLODBIAS", "TSAMP_MAXMIPLEVEL", "TSAMP_MAXANISOTROPY",
  "TSAMP_SRGBTEXTURE", "TSAMP_ELEMENTINDEX", "TSAMP_DMAPOFFSET",
  "TSAMP_ALPHAKILL_DEPRECATED", "TSAMP_COLORKEYOP_DEPRECATED",
  "TSAMP_COLORSIGN_DEPRECATED", "TSAMP_COLORKEYCOLOR_DEPRECATED",
  "TADDR_WRAP", "TADDR_MIRROR", "TADDR_CLAMP", "TADDR_BORDER",
  "TADDR_MIRRORONCE", "TADDR_CLAMPTOEDGE_DEPRECATED", "TEXF_NONE",
  "TEXF_POINT", "TEXF_LINEAR", "TEXF_ANISOTROPIC", "TEXF_PYRAMIDALQUAD",
  "TEXF_GAUSSIANQUAD", "TEXF_FLATCUBIC_DEPRECATED",
  "TEXF_GAUSSIANCUBIC_DEPRECATED", "TEXF_QUINCUNX_DEPRECATED",
  "TEXF_MAX_DEPRECATED", "TAK_DISABLE_DEPRECATED", "TAK_ENABLE_DEPRECATED",
  "TCKOP_DISABLE_DEPRECATED", "TCKOP_ALPHA_DEPRECATED",
  "TCKOP_RGBA_DEPRECATED", "TCKOP_KILL_DEPRECATED", "TOKEN_TEXTURE",
  "TEXTURE_SOURCE", "OBJECTS", "EFFECT_GENERALLIGHT", "EFFECT_POINTLIGHT",
  "EFFECT_DIRECTIONALLIGHT", "EFFECT_SPOTLIGHT", "EFFECT_SHADOWPOINTLIGHT",
  "EFFECT_SHADOWDIRECTIONALLIGHT", "EFFECT_SHADOWSPOTLIGHT", "EFFECT",
  "EFFECT_ENVIRONMENTMAP", "EFFECT_PROJECTEDSHADOWMAP",
  "EFFECT_PROJECTEDLIGHTMAP", "EFFECT_FOGMAP", "USEMAPVALUE", "CMOBJECT",
  "'\\n'", "'0'", "$accept", "shader_file", "shader", "@1", "@2",
  "nsfshader_components_optional", "nsfshader_components_list",
  "nsfshader_component", "optional_string", "optional_multi_string",
  "optional_description", "optional_filename", "string_or_quote",
  "attribute_name", "range_color_optional", "range_point4_optional",
  "range_point3_optional", "range_point2_optional", "range_float_optional",
  "range_int_optional", "float_values_arbitrary_list", "float_values_1",
  "float_values_16", "attribute_list_with_brackets", "@3",
  "global_attribute_list_with_brackets", "@4", "optional_multiplicity",
  "attribute_list", "attribute_value", "artist_conditional",
  "attribute_bool", "attribute_uint", "@5", "attribute_float", "@6",
  "attribute_point2", "@7", "attribute_point3", "@8", "attribute_point4",
  "@9", "attribute_matrix3", "@10", "attribute_transform", "@11",
  "attribute_color", "@12", "attribute_texture",
  "object_list_with_brackets", "@13", "object_list", "object_value",
  "object_effect_general_light", "object_effect_point_light",
  "object_effect_directional_light", "object_effect_spot_light",
  "object_effect_shadow_point_light",
  "object_effect_shadow_directional_light",
  "object_effect_shadow_spot_light", "object_effect_environment_map",
  "object_effect_projected_shadow_map",
  "object_effect_projected_light_map", "object_effect_fog_map",
  "packing_definition_definition", "@14", "packing_definition_declaration",
  "packing_definition_type", "packing_definition_parameter",
  "packing_definition_tesselator", "packing_definition_usage",
  "packing_definition_entries", "packing_definition_entry",
  "semantic_adapter_table_declaration", "@15", "semantic_adapter_list",
  "semantic_adapter_entry", "renderstate_list_with_brackets", "@16",
  "renderstate_list", "renderstate_entry_save_optional",
  "renderstate_entry_save", "renderstate_entry", "renderstate_attribute",
  "renderstate_string", "renderstate_bool", "renderstate_float",
  "renderstate_hex", "constantmap_list", "constantmap_entry",
  "constantmap_platform_block", "@17", "@18", "constantmap_entry_defined",
  "constantmap_entry_object", "constantmap_entry_attribute", "@19",
  "constantmap_entry_constant", "@20", "constantmap_entry_global", "@21",
  "constantmap_entry_operator", "operator_type",
  "operator_optional_transpose", "operator_optional_inverse",
  "vs_constantmap_with_brackets", "@22", "gs_constantmap_with_brackets",
  "@23", "ps_constantmap_with_brackets", "@24", "userdefined_classname",
  "vertexshader_program_name", "vertexshader_program",
  "vertexshader_program_name_only",
  "vertexshader_program_bracket_contents_list",
  "vertexshader_program_bracket_content", "vertexshader_program_bracket",
  "@25", "@26", "vertexshader_program_entry",
  "geometryshader_program_name", "geometryshader_program",
  "geometryshader_program_name_only",
  "geometryshader_program_bracket_contents_list",
  "geometryshader_program_bracket_content",
  "geometryshader_program_bracket", "@27", "@28",
  "geometryshader_program_entry", "pixelshader_program_name",
  "pixelshader_program", "pixelshader_program_name_only",
  "pixelshader_program_bracket_contents_list",
  "pixelshader_program_bracket_content", "pixelshader_program_bracket",
  "@29", "@30", "pixelshader_program_entry",
  "requirement_list_with_brackets", "@31", "requirement_list",
  "requirement_entry", "requirement_vsversion", "requirement_gsversion",
  "requirement_psversion", "requirement_userdefined",
  "requirement_platform", "requirement_platform_list",
  "requirement_remaining", "requirement_bonesperpartition",
  "requirement_usesbinormaltangentuvsource",
  "requirement_usesbinormaltangent", "binormaltanget_method",
  "stage_or_sampler_or_texture", "stage", "@32",
  "stage_entry_list_optional", "stage_entry_list",
  "stage_entry_or_texture", "stage_entry_save_optional",
  "stage_entry_save", "stage_entry", "stage_texture",
  "stage_texture_map_ndl", "stage_texture_map_ndl_decal", "stage_color_op",
  "stage_color_arg0", "stage_color_arg1", "stage_color_arg2",
  "stage_alpha_op", "stage_alpha_arg0", "stage_alpha_arg1",
  "stage_alpha_arg2", "stage_result_arg", "stage_constant_deprecated",
  "stage_bumpenvmat00", "stage_bumpenvmat01", "stage_bumpenvmat10",
  "stage_bumpenvmat11", "stage_bumpenvlscale", "stage_bumpenvloffset",
  "stage_texcoordindex", "stage_textransflags",
  "stage_texturetransformflags_count",
  "stage_texturetransformflags_optional_projection",
  "stage_texture_operation", "stage_texture_argument",
  "stage_texture_argument_modifiers_optional",
  "stage_texture_argument_modifier", "stage_texcoordindex_flags",
  "stage_textransmatrix", "stage_textransmatrix_assignment", "@33",
  "stage_textransmatrix_option", "sampler", "@34", "@35",
  "sampler_entry_list_optional", "sampler_entry_list",
  "sampler_entry_save_optional", "sampler_entry_save", "sampler_entry",
  "sampler_addressu", "sampler_addressv", "sampler_addressw",
  "sampler_bordercolor", "sampler_magfilter", "sampler_minfilter",
  "sampler_mipfilter", "sampler_mipmaplodbias", "sampler_maxmiplevel",
  "sampler_maxanisotropy", "sampler_srgbtexture", "sampler_elementindex",
  "sampler_dmapoffset", "sampler_alphakill_deprecated",
  "sampler_colorkeyop_deprecated", "sampler_colorsign_deprecated",
  "sampler_colorkeycolor_deprecated", "sampler_texture_address",
  "sampler_texture_filter", "sampler_texture_alphakill",
  "sampler_texture_colorkeyop", "texture", "@36", "@37",
  "texture_entry_list_optional", "texture_entry_list", "texture_entry",
  "texture_source", "pass_component_list", "pass_component", "pass", "@38",
  "implementation_component_list_optional",
  "implementation_component_list", "implementation_component",
  "implementation", "@39", "@40", "streamoutput", "@41",
  "streamoutput_component_list", "streamoutput_component",
  "streamoutappend", "streamouttargets", "streamoutputtarget_list",
  "streamoutputtarget", "outputstream", "@42",
  "outputstream_component_list", "outputstream_component",
  "maxvertexcount", "vertexformat", "@43", "vertexformat_entry_list",
  "vertexformat_entry", "vertexformat_entry_datatype",
  "vertexformat_entry_datatype_float", "vertexformat_entry_datatype_int",
  "vertexformat_entry_datatype_uint", "outputprimtype", "outputprimtype2",
  "outputprimtype2_POINT", "outputprimtype2_LINE",
  "outputprimtype2_TRIANGLE", "userdefineddata", "userdefineddata_list",
  "userdefineddata_block", "@44", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   472,   473,   474,
     475,   476,   477,   478,   479,   480,   481,   482,   483,   484,
     485,   486,   487,   488,   489,   490,   491,   492,   493,   494,
     495,   496,   497,   498,   499,   500,   501,   502,   503,   504,
     505,   506,   507,   508,   509,   510,   511,   512,   513,   514,
     515,   516,   517,   518,   519,   520,   521,   522,   523,   524,
     525,   526,   527,   528,   529,   530,   531,   532,   533,   534,
     535,   536,   537,   538,   539,   540,   541,   542,   543,   544,
     545,   546,   547,   548,   549,   550,   551,   552,   553,   554,
     555,   556,   557,   558,   559,   560,   561,   562,   563,   564,
     565,   566,   567,   568,   569,   570,   571,   572,   573,   574,
      10,    48
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short int yyr1[] =
{
       0,   322,   323,   323,   325,   326,   324,   324,   327,   327,
     328,   328,   329,   329,   329,   329,   329,   329,   329,   330,
     330,   331,   331,   332,   332,   333,   333,   333,   334,   334,
     335,   336,   336,   336,   337,   337,   338,   338,   339,   339,
     340,   340,   341,   341,   341,   342,   342,   343,   344,   346,
     345,   348,   347,   349,   349,   350,   350,   351,   351,   351,
     351,   351,   351,   351,   351,   351,   351,   352,   352,   353,
     355,   354,   357,   356,   359,   358,   361,   360,   363,   362,
     365,   364,   367,   366,   369,   368,   370,   370,   372,   371,
     373,   373,   374,   374,   374,   374,   374,   374,   374,   374,
     374,   374,   374,   375,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   387,   386,   388,   389,   389,   389,
     389,   389,   389,   389,   389,   389,   389,   389,   389,   389,
     389,   389,   389,   389,   389,   389,   389,   389,   389,   389,
     389,   389,   389,   389,   389,   389,   390,   390,   390,   390,
     390,   390,   390,   390,   390,   390,   390,   390,   390,   390,
     390,   390,   390,   390,   390,   390,   390,   390,   390,   390,
     390,   390,   391,   391,   391,   391,   391,   391,   391,   392,
     392,   392,   392,   392,   392,   392,   392,   392,   392,   392,
     392,   392,   392,   393,   393,   393,   394,   394,   394,   394,
     394,   394,   396,   395,   397,   397,   398,   400,   399,   401,
     401,   402,   402,   403,   404,   404,   404,   404,   404,   404,
     405,   406,   407,   408,   409,   409,   410,   410,   411,   411,
     411,   411,   411,   411,   411,   413,   414,   412,   415,   415,
     415,   415,   416,   416,   418,   417,   420,   419,   422,   421,
     423,   424,   424,   424,   424,   424,   425,   425,   426,   426,
     428,   427,   430,   429,   432,   431,   433,   434,   434,   434,
     435,   435,   435,   436,   436,   436,   437,   437,   438,   438,
     438,   438,   440,   441,   439,   442,   442,   443,   443,   443,
     444,   444,   444,   445,   445,   445,   446,   446,   447,   447,
     447,   449,   450,   448,   451,   451,   452,   452,   452,   453,
     453,   453,   454,   454,   454,   455,   455,   456,   456,   456,
     458,   459,   457,   460,   460,   462,   461,   463,   463,   464,
     464,   464,   464,   464,   464,   464,   464,   464,   464,   465,
     466,   467,   468,   469,   470,   470,   470,   470,   471,   472,
     473,   474,   475,   475,   475,   475,   475,   476,   476,   476,
     478,   477,   479,   479,   480,   480,   481,   481,   481,   482,
     482,   483,   484,   484,   484,   484,   484,   484,   484,   484,
     484,   484,   484,   484,   484,   484,   484,   484,   484,   484,
     484,   485,   485,   485,   485,   486,   486,   486,   486,   486,
     486,   486,   486,   487,   488,   489,   490,   491,   492,   493,
     494,   495,   496,   497,   498,   498,   499,   499,   500,   500,
     501,   501,   502,   502,   503,   503,   504,   504,   505,   505,
     505,   506,   506,   506,   506,   507,   507,   508,   508,   508,
     508,   508,   508,   508,   508,   508,   508,   508,   508,   508,
     508,   508,   508,   508,   508,   508,   508,   508,   508,   508,
     508,   508,   508,   508,   509,   509,   509,   509,   509,   509,
     509,   510,   510,   511,   511,   511,   511,   512,   512,   512,
     512,   512,   512,   512,   513,   514,   515,   514,   516,   516,
     516,   516,   516,   516,   516,   516,   518,   517,   519,   517,
     520,   520,   521,   521,   522,   522,   522,   523,   524,   524,
     524,   524,   524,   524,   524,   524,   524,   524,   524,   524,
     524,   524,   524,   524,   524,   524,   525,   526,   527,   528,
     528,   529,   530,   531,   532,   532,   533,   533,   534,   534,
     535,   535,   536,   536,   537,   537,   538,   538,   539,   540,
     540,   541,   541,   542,   542,   542,   542,   542,   542,   542,
     542,   543,   543,   543,   543,   543,   543,   543,   543,   543,
     543,   543,   543,   544,   544,   544,   545,   545,   545,   545,
     545,   547,   546,   548,   546,   549,   549,   550,   550,   551,
     551,   552,   552,   552,   552,   553,   553,   554,   554,   554,
     554,   554,   554,   554,   554,   554,   554,   556,   555,   557,
     557,   558,   558,   559,   559,   559,   559,   559,   559,   559,
     559,   559,   559,   561,   562,   560,   564,   563,   565,   565,
     566,   566,   567,   568,   569,   569,   570,   572,   571,   573,
     573,   574,   574,   574,   575,   577,   576,   578,   578,   579,
     580,   580,   580,   581,   582,   583,   584,   585,   585,   585,
     586,   587,   588,   589,   589,   589,   589,   590,   590,   592,
     591
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     1,     0,     0,     8,     2,     0,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       1,     1,     2,     0,     1,     0,     1,     1,     1,     1,
       3,     0,    20,    16,     0,    20,     0,    16,     0,    12,
       0,     4,     0,     4,     4,     1,     3,     1,    31,     0,
       5,     0,     5,     0,     3,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     4,
       0,     6,     0,     7,     0,     7,     0,     7,     0,     7,
       0,     6,     0,     6,     0,     6,     5,     4,     0,     5,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     0,     6,     2,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     2,     2,     2,     7,     4,
       6,     3,     0,     5,     2,     1,    10,     0,     5,     2,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     2,
       3,     3,     3,     3,     3,     3,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     0,     8,     5,     5,
       4,     4,     4,     4,     0,     6,     0,     6,     0,     5,
       9,     1,     1,     1,     1,     2,     0,     1,     0,     1,
       0,     5,     0,     5,     0,     5,     3,     2,     2,     2,
       3,     2,     1,     3,     3,     3,     1,     2,     1,     3,
       3,     3,     0,     0,     9,     1,     1,     2,     2,     2,
       3,     2,     1,     3,     3,     3,     1,     2,     1,     3,
       3,     0,     0,     9,     1,     1,     2,     2,     2,     3,
       2,     1,     3,     3,     3,     1,     2,     1,     3,     3,
       0,     0,     9,     1,     1,     0,     5,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     3,
       3,     3,     3,     3,     7,     5,     3,     1,     3,     3,
       3,     3,     1,     1,     1,     1,     2,     1,     1,     1,
       0,     7,     0,     1,     2,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     3,     3,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     4,     4,     4,     3,
       3,     1,     1,     1,     1,     0,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     2,     2,     2,     2,     2,
       2,     0,     1,     2,     2,     1,     1,     0,     1,     1,
       1,     1,     1,     2,     3,     3,     0,     4,     0,     1,
       1,     1,     1,     1,     1,     2,     0,     7,     0,     6,
       0,     1,     2,     1,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     1,     2,     1,     1,     1,     1,
       2,     0,     7,     0,     6,     0,     1,     2,     1,     1,
       2,     3,     3,     3,     3,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     6,     0,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     0,     8,     0,     5,     2,     1,
       1,     1,     3,     4,     2,     1,     1,     0,     6,     2,
       1,     1,     1,     1,     3,     0,     5,     2,     1,     6,
       1,     1,     1,     1,     1,     1,     3,     1,     1,     1,
       1,     1,     1,     3,     3,     3,     3,     1,     2,     0,
       6
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short int yydefact[] =
{
       0,     0,     0,     0,     3,     7,     0,     1,     2,     4,
      23,    21,    24,     5,    22,     8,     0,     0,     0,     0,
       0,     0,     0,     0,     9,    11,    12,    14,    13,    15,
      16,    17,    18,    49,    51,     0,     0,     0,     0,    88,
       6,    10,     0,     0,   114,   669,   623,   637,     0,     0,
      70,    72,    74,    76,    78,    80,    82,    84,     0,     0,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,     0,     0,     0,    23,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      50,    55,    52,     0,     0,     0,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,     0,     0,     0,   194,    28,    29,     0,   667,
       0,   624,     0,     0,     0,     0,   640,   641,   643,   642,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    89,    90,    67,    68,     0,     0,    53,    53,    53,
      53,    53,    53,     0,    25,   195,   196,   197,     0,     0,
     115,   193,     0,   668,   670,   609,     0,     0,   645,   638,
     639,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,    69,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    27,    25,    26,    87,     0,     0,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   201,    47,   663,   664,   666,
     665,    45,     0,     0,     0,     0,     0,     0,     0,     0,
      19,   614,   622,   613,   617,   618,   619,   620,   616,   615,
       0,   610,   612,   621,   644,   660,   661,   662,   656,   657,
     658,   659,     0,    42,     0,     0,     0,     0,     0,     0,
       0,    31,    86,   199,   145,   172,   173,   174,   175,   176,
     177,   178,     0,     0,   116,   202,   207,   260,   262,   264,
     325,     0,    20,     0,   625,   611,   653,   654,   655,     0,
     648,     0,   650,   651,   652,     0,    71,    54,    40,    38,
      36,    34,    81,    83,     0,    85,     0,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
     192,     0,    46,     0,     0,     0,     0,     0,     0,   266,
     607,   646,   647,     0,     0,     0,     0,    73,     0,    75,
       0,    77,     0,    79,     0,     0,   200,     0,     0,   205,
       0,     0,     0,   210,   211,   212,   217,   218,   214,   216,
     215,   235,     0,   244,   246,   248,     0,     0,     0,   227,
     234,   228,   229,   230,   231,   232,   233,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     328,   329,   330,   331,   332,   333,   337,   334,   336,   335,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   198,
       0,   203,   204,   219,     0,   208,   209,   213,     0,     0,
       0,     0,     0,     0,     0,     0,   261,   226,   263,   265,
     338,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     326,   327,     0,     0,     0,     0,     0,    19,    19,   597,
     599,   601,   603,   272,   286,   285,   598,   292,   305,   304,
     600,   311,   324,   323,   602,   604,   357,   358,   359,     0,
     596,   606,   605,     0,    44,    43,     0,     0,     0,     0,
       0,     0,     0,   224,   223,   225,   221,   222,   220,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   348,
     339,   340,   341,   342,   347,   343,   349,     0,   352,   353,
     355,   354,   351,   350,   267,   282,   268,   269,   287,   301,
     288,   289,   306,   320,   307,   308,   626,    19,    19,     0,
      19,     0,   271,   291,   310,   608,   595,     0,    41,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   240,   241,
       0,     0,     0,     0,   242,   243,     0,   356,     0,     0,
       0,     0,     0,     0,   498,     0,   583,   270,   290,   309,
     649,     0,     0,     0,     0,     0,    30,   236,   238,   239,
       0,     0,   249,     0,   346,     0,     0,     0,     0,     0,
       0,   629,   631,   630,   360,   496,     0,   581,     0,     0,
       0,     0,     0,     0,     0,   245,   247,     0,   251,   252,
     253,   254,     0,     0,     0,     0,     0,     0,     0,   627,
     628,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   506,     0,     0,   503,   504,   505,   508,   509,
     510,   511,   512,   513,   514,   515,   516,   517,   518,   519,
     520,   521,   522,   523,   524,     0,     0,     0,     0,     0,
     588,   589,     0,     0,     0,     0,     0,     0,     0,   255,
     258,   345,   283,   302,   321,   636,     0,   635,   632,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   365,   368,   369,   370,   366,   372,   373,   374,   375,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,   389,   367,     0,   525,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   499,   502,   507,     0,   590,
       0,   584,   587,     0,     0,     0,     0,     0,     0,   237,
     259,   256,     0,     0,     0,     0,   633,   634,   390,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   361,   364,
     371,   497,   394,   395,   396,   397,   398,   399,   400,   401,
     402,     0,   393,   391,   392,     0,   553,   554,   555,   556,
     557,   558,   559,   526,   527,   528,   529,   530,     0,   561,
     562,   563,   564,   565,   566,   568,   569,   567,   570,   571,
     531,   532,   533,   534,   535,   536,   537,   538,   539,   540,
     541,   542,   543,   544,   545,     0,   573,   574,   547,   546,
       0,   576,   577,   578,   579,   548,   549,   550,   551,   552,
     582,   594,   593,   591,   592,     0,     0,     0,     0,     0,
       0,   257,   250,   344,     0,     0,     0,     0,   278,     0,
     276,     0,     0,     0,   298,     0,   296,     0,     0,     0,
     317,     0,   315,     0,   437,   438,   439,   440,   441,   442,
     443,   444,   445,   446,   447,   448,   449,   450,   451,   452,
     453,   454,   455,   456,   457,   458,   459,   460,   461,   462,
     404,   471,   471,   471,   471,   471,   471,   471,   405,   406,
     407,   408,   409,   410,   411,   412,   413,   414,   415,   416,
     417,   418,   419,   420,   421,   422,   423,   424,   425,     0,
     478,   479,   480,   481,   482,     0,   430,   431,   432,   433,
     434,   429,   435,     0,   486,   484,   403,   560,   572,   575,
     580,     0,     0,     0,     0,     0,   206,     0,     0,     0,
       0,   284,   277,     0,     0,     0,   303,   297,     0,     0,
       0,   322,   316,   463,   475,   476,   464,   472,   465,   466,
     467,   468,   469,   470,   483,   426,   427,   436,   428,     0,
     489,   490,   491,   492,   493,   494,     0,     0,     0,     0,
       0,     0,     0,   273,   274,   275,   279,   280,   281,   293,
     294,   295,   299,   300,   312,   313,   314,   318,   319,   473,
     474,   495,   485,     0,    39,     0,     0,     0,     0,     0,
     487,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    33,     0,     0,    37,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    32,     0,    35,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    48
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     3,     4,    10,    15,    23,    24,    25,   323,    12,
      13,   225,   148,   528,   345,   383,   381,   379,   377,   336,
     260,   261,  1100,    26,    42,    27,    43,   215,    59,    60,
     175,    61,    62,   101,    63,   102,    64,   103,    65,   104,
      66,   105,    67,   106,    68,   107,    69,   108,    70,    28,
      48,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,    29,    72,   271,   255,   143,   312,
     361,   144,   145,   272,   363,   388,   389,   273,   364,   392,
     393,   394,   395,   396,   397,   398,   399,   400,   408,   409,
     410,   458,   644,   411,   412,   413,   461,   414,   462,   415,
     463,   416,   652,   922,   811,   274,   365,   275,   366,   276,
     367,   277,   493,   494,   928,   929,   930,   495,   598,   813,
     496,   497,   498,   934,   935,   936,   499,   599,   814,   500,
     501,   502,   940,   941,   942,   503,   600,   815,   504,   278,
     368,   429,   430,   431,   432,   433,   434,   435,   545,   436,
     437,   438,   439,   552,   505,   506,   661,   749,   750,   751,
     752,   753,   754,   682,   853,   854,   756,   757,   758,   759,
     760,   761,   762,   763,   764,   765,   766,   767,   768,   769,
     770,   771,   772,   773,  1012,  1058,   970,   978,  1046,  1047,
    1005,   774,  1015,  1067,  1066,   507,   662,   636,   683,   684,
     685,   686,   687,   688,   689,   690,   691,   692,   693,   694,
     695,   696,   697,   698,   699,   700,   701,   702,   703,   704,
     863,   880,   899,   905,   508,   705,   638,   708,   709,   710,
     711,   509,   510,   279,   440,   280,   281,   282,    30,    74,
     195,   511,   601,   630,   631,   632,   633,   726,   727,    31,
      75,   155,   156,   157,   158,   292,   329,   330,   331,   332,
     333,   334,   159,   288,   289,   290,   291,   149,   150,    32,
      73
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -634
static const short int yypact[] =
{
      35,  -286,    25,    62,  -634,  -634,    65,  -634,  -634,  -634,
      72,  -634,    74,  -634,  -634,    18,   113,   123,   136,   161,
     177,   195,   174,   200,    18,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,   269,   306,   334,   416,  -634,
    -634,  -634,   657,   657,  -634,  -634,  -634,  -634,   377,   198,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,   201,   487,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,   538,   681,   141,    72,   -68,   442,   492,   500,   526,
     528,   534,   551,   583,   594,   664,   670,    -2,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
     231,   462,   465,   706,   736,   766,   767,   768,   769,   231,
    -634,  -634,  -634,   477,   772,   770,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,   774,   775,   711,  -634,  -634,  -634,   788,   141,
     797,  -634,   790,   791,   801,    43,  -634,  -634,  -634,  -634,
     780,   781,   782,   783,   784,   785,   786,   787,   789,   792,
     793,  -634,  -634,  -634,  -634,   794,   231,   806,   806,   806,
     806,   806,   806,   231,   223,  -634,  -634,  -634,   795,   568,
    -634,  -634,   521,  -634,  -634,   417,   796,   118,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,   798,   799,   231,   231,   231,   231,   231,
     231,   802,  -634,    82,  -634,  -634,   568,   497,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,   276,  -634,  -634,  -634,  -634,
    -634,   803,   804,   821,   823,   824,   825,   827,   828,   820,
     808,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
     830,   417,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,   302,   829,   831,   802,   802,   802,   802,   802,
     802,   833,  -634,   276,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,   478,   802,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,   141,  -634,   832,  -634,  -634,  -634,  -634,  -634,    31,
    -634,   813,  -634,  -634,  -634,   316,  -634,  -634,   834,   835,
     836,   837,  -634,  -634,   840,  -634,   478,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,   814,  -634,   822,    56,    24,    24,    24,    86,  -634,
    -634,  -634,  -634,   141,   826,   838,   841,  -634,   842,  -634,
     843,  -634,   845,  -634,   844,   839,  -634,   141,    93,  -634,
     533,   846,    79,  -634,  -634,   847,  -634,  -634,  -634,  -634,
    -634,  -634,    28,  -634,  -634,  -634,   848,   849,     2,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,     7,    14,   535,
     853,   854,   856,   857,   858,   859,   860,   863,   864,   464,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
     -93,   862,   851,   852,   855,   861,   865,   866,   867,  -634,
     868,  -634,  -634,  -634,   532,  -634,  -634,  -634,   704,   468,
     514,   141,   141,   141,   869,   870,  -634,  -634,  -634,  -634,
    -634,   871,   872,   873,   874,   875,   879,   881,    63,   882,
    -634,  -634,    67,   170,   219,   878,   883,   318,   352,  -634,
    -634,  -634,  -634,   884,  -634,  -634,  -634,   885,  -634,  -634,
    -634,   886,  -634,  -634,  -634,  -634,  -634,  -634,  -634,    55,
    -634,  -634,  -634,   888,  -634,  -634,   877,   895,   900,   901,
     876,   891,   892,  -634,  -634,  -634,  -634,  -634,  -634,   880,
     894,   896,   897,   898,   899,   902,   903,   904,   364,  -634,
    -634,  -634,  -634,  -634,   850,  -634,  -634,   536,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,   808,   808,   887,
     808,   893,   905,   906,   907,  -634,  -634,   908,  -634,   910,
     911,   912,   916,   141,   909,   879,   913,   914,  -634,  -634,
     915,   917,   918,   919,  -634,  -634,   920,  -634,   707,   708,
     709,   236,   922,   933,  -634,   937,  -634,  -634,  -634,  -634,
    -634,   940,   930,   934,   925,   935,  -634,  -634,  -634,  -634,
     927,   802,  -634,   516,   938,   941,   942,   943,   949,   944,
      -1,  -634,  -634,  -634,  -634,  -634,    97,  -634,    16,   889,
     936,   939,   152,   945,    24,  -634,  -634,   564,  -634,  -634,
    -634,  -634,   932,   946,   879,   879,   879,   947,   931,  -634,
    -634,   225,    97,   569,   948,   951,   952,   953,   954,   955,
     956,   958,   961,   962,   963,   964,   965,   966,   967,   968,
     969,   970,  -634,   957,   158,  -634,  -634,   959,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,    16,   573,   971,   980,    20,
    -634,  -634,   972,   979,   973,   982,   974,   975,    19,  -634,
     684,   977,  -634,  -634,  -634,  -634,    84,  -634,  -634,   579,
     978,   981,   984,   986,   987,   988,   989,   990,   991,   992,
     993,   994,   995,   996,   997,   998,   999,  1000,  1001,  1010,
     321,  -634,  -634,  -634,   960,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  1011,  -634,     0,    17,    17,
      17,   335,     1,     1,     1,    58,   147,   155,   160,   188,
     239,    39,    29,     4,   336,  -634,  -634,  -634,  1012,  -634,
      21,  -634,  -634,  1004,  1013,  1002,  1003,  1014,  1005,  -634,
    -634,   713,  1006,   430,   447,   466,  -634,  -634,  -634,   220,
     461,   461,   461,   220,   461,   461,   461,   461,  1008,   101,
     184,   204,   241,   242,   329,   179,   367,   507,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  1009,  -634,  -634,  -634,   676,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,   701,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,   703,  -634,  -634,  -634,  -634,
     705,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  1015,  1016,  1018,  1019,  1017,
    1020,  -634,  -634,  -634,  1022,  1023,  1025,  1028,  -634,  1024,
     430,  1029,  1030,  1031,  -634,  1040,   447,  1033,  1034,  1035,
    -634,  1044,   466,   731,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,   463,   463,   463,   463,   463,   463,   463,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,   732,
    -634,  -634,  -634,  -634,  -634,    -3,  -634,  -634,  -634,  -634,
    -634,  -634,   890,    32,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  1021,  1038,  1047,  1032,  1036,  -634,    87,  1037,  1039,
    1026,  -634,  -634,   328,  1041,  1042,  -634,  -634,   353,  1043,
    1045,  -634,  -634,  -634,   805,   807,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,   740,
    -634,  -634,  -634,  -634,  -634,  -634,  1046,   140,  1052,  1049,
    1050,  1048,  1056,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  1051,  -634,  1062,  1063,  1054,  1055,  1066,
    -634,  1057,  1058,  1059,  1069,  1060,  1078,  1072,  1079,  1065,
    1073,  1082,  1068,  -634,  1080,  1071,  -634,  1083,  1074,  1085,
    1076,  1086,  1077,  1089,  1093,  1090,  1094,  -634,  1081,  -634,
    1092,  1084,  1095,  1087,  1096,  1088,  1099,  1091,  1100,  1097,
    1102,  1098,  1103,  1101,  1104,  1105,  1106,  1107,  1109,  1108,
    1110,  1111,  -634
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -634,  -634,  1053,  -634,  -634,  -634,  -634,  1113,  -383,  -634,
    1061,   921,  -191,  -310,  -634,  -634,  -634,  -634,  -634,  -634,
    -221,  -634,  -634,  -634,  -634,  -634,  -634,   546,  1112,   -20,
     -95,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  1064,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,   923,  -634,   762,
     800,  -634,   976,  -634,  -634,  -634,   720,  -393,  -634,  -634,
     735,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -357,  -402,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -382,  -634,  -380,  -634,  -367,
    -634,  -634,  -634,  -634,  -634,   199,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,   192,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,   189,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,   710,  -634,  -634,  -634,  -634,  -634,  -547,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,   384,
    -634,  -634,  -634,  -633,   338,   340,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,   319,  -339,  -254,  -634,
    -634,  -634,  -634,  -634,    76,  -634,  -634,  -634,   479,  -634,
     469,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,
     -50,     8,  -634,  -634,  -634,  -634,  -634,   440,  -634,   438,
    -634,  -634,   639,  -634,  -634,  -634,  -634,   924,  -634,  -634,
    -634,  -634,  -634,  -634,   520,  -634,  -634,  -634,   426,  -634,
    -634,  -634,  1007,  -634,  -634,  -634,  -634,   926,  -634,  -634,
    -634,  -634,  -634,  -634,  -634,  -634,  -634,  -634,  1027,  -192,
    -634
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -587
static const short int yytable[] =
{
     301,   259,   868,   283,   171,   659,   467,   401,   466,   417,
     418,   522,   401,   468,   184,   467,   467,   706,   855,   401,
     469,   706,  -585,  1055,   401,   809,  -586,   842,   755,   401,
     900,   906,   522,  1059,     5,   264,     1,   371,   617,   111,
     895,   265,   482,   266,   483,   267,   484,   489,   911,   199,
     522,   111,     6,    16,    17,   146,   147,   390,   490,  -488,
     491,   575,     7,     1,   547,    19,     2,    18,   485,   522,
       9,   554,   555,   492,   338,   339,   340,   341,   342,   343,
     390,   213,   486,   391,   883,   455,   222,   419,   221,   283,
     816,  1073,   362,     2,   556,   557,   152,   153,   663,   451,
      11,   154,    14,  -500,   569,   571,   391,   722,   723,   724,
     224,   725,   522,   420,  1074,  1075,   489,   755,    33,   387,
     295,   296,   297,   298,   299,   300,   987,   490,    34,   491,
     369,   402,   403,   404,   405,   406,   402,   403,   404,   405,
     406,  1059,   492,   402,   403,   404,   405,   406,   402,   403,
     404,   405,   406,   402,   403,   404,   405,   406,   522,   663,
     715,   628,   629,    35,  -501,  -488,   522,   716,   146,   147,
     487,   522,   459,   885,   558,   559,    19,    20,    21,    39,
     999,   887,   441,   264,   602,   603,   889,   605,    36,   265,
     482,   266,   483,   267,   484,   522,   450,   560,   561,   522,
    1056,   326,   327,   328,    37,  -477,    40,   152,   153,   989,
     488,   460,   154,    19,   891,   522,   485,   548,   549,   550,
     551,   943,    38,   562,   563,   100,   729,   222,   109,   991,
     486,  -362,   421,   422,   423,   424,   425,   426,   427,   428,
    1060,  1061,  1062,  1063,  1064,  1065,   564,   565,   512,   223,
     522,   224,   522,   522,   843,   844,   845,   846,   847,   848,
     849,   850,   851,   173,   174,   893,   993,   995,   531,   533,
     534,   535,   536,   664,    44,   843,   844,   845,   846,   847,
     848,   849,   850,   851,   285,   286,   287,   718,   869,   870,
     871,   872,   873,   874,   875,   876,   877,   878,   856,   857,
     858,   859,   860,   861,    76,    77,    78,    79,    80,    81,
      82,    45,    83,    84,    85,    86,   467,   512,   487,   879,
     707,   407,   729,    22,   707,   907,   407,  -363,   901,   902,
     903,   904,  1079,   407,   664,   862,   896,   897,   407,    46,
     522,   374,   375,   407,   568,   322,   522,   522,  1060,  1061,
    1062,  1063,  1064,  1065,   997,  1080,  1081,  1084,   488,   866,
     908,   665,   666,   667,   668,   669,   670,   671,   672,   673,
     674,   675,   676,   677,   678,   679,   680,   681,   570,   322,
    1085,  1086,  -477,   305,   306,   307,   308,   309,   310,   311,
     594,   595,   615,  1000,  1001,  1002,  1003,  1004,   628,   629,
     646,   664,   730,   731,   732,   733,   734,   735,   736,   737,
     738,   739,   740,   741,   742,   743,   744,   745,   746,   747,
     748,    47,   665,   666,   667,   668,   669,   670,   671,   672,
     673,   674,   675,   676,   677,   678,   679,   680,   681,   944,
     945,   946,   947,   948,   949,   950,   951,   952,   953,   954,
     955,   956,   957,   958,   959,   960,   961,   962,   963,   964,
     965,   966,   967,   968,   969,   419,   262,   852,   160,   263,
     480,   867,   326,   327,   328,   884,   886,   888,   890,   892,
     894,   898,   979,   980,   909,   982,   983,   984,   985,   176,
     912,   420,   177,   110,   530,   146,   147,   664,   730,   731,
     732,   733,   734,   735,   736,   737,   738,   739,   740,   741,
     742,   743,   744,   745,   746,   747,   748,   647,   161,   988,
     990,   992,   994,   996,   998,    49,   162,    50,    51,    52,
      53,    54,    55,    56,    57,    58,   648,   649,   650,   651,
     532,   146,   147,   522,   112,   264,   256,   257,   146,   147,
     258,   265,   163,   266,   164,   267,   523,   524,   525,   526,
     165,   527,   268,  1006,  1007,  1008,  1009,  1010,  1011,   227,
     924,   925,   926,   927,  -117,    19,    49,   166,    50,    51,
      52,    53,    54,    55,    56,    57,    58,   931,   932,   933,
     269,   270,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   937,   938,   939,   167,
     421,   422,   423,   424,   425,   426,   427,   428,  -117,  -117,
     168,  -117,  -117,  -117,  -117,  -117,  -117,  -117,  -117,  -117,
    -117,  -117,  -117,  -117,  -117,  -117,  -117,  -117,  -117,  -117,
    -117,  -117,  -117,  -117,  -117,  -117,  -117,  -117,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,   244,   245,   246,   247,   248,   249,
     250,   251,   252,   253,   254,  -117,  -117,  -117,  -117,  -117,
    -117,  -117,   113,    76,    77,    78,    79,    80,    81,    82,
     169,    83,    84,    85,    86,    49,   170,    50,    51,    52,
      53,    54,    55,    56,    57,    58,   971,   972,   973,   974,
     975,   976,   977,  1013,  1014,  1044,  1045,   190,  1048,  1049,
    1050,  1051,  1052,  1053,   216,   217,   218,   219,   220,   864,
     865,   114,   115,   178,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   114,   115,   179,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   881,   882,   180,   181,   182,   183,   185,   186,   187,
     188,   189,   192,   194,   196,   197,   198,   201,   202,   203,
     204,   205,   206,   207,   208,   214,   209,   304,   313,   210,
     211,   226,   284,   212,   293,   294,   315,   256,   316,   317,
     318,   314,   319,   320,   321,   322,   324,   370,   335,   373,
     386,   337,   344,   376,   378,   380,   382,   384,   387,   445,
     446,   442,   447,   453,   529,   470,   597,   625,   626,   627,
     454,   514,   515,   596,   443,   449,   444,   471,   472,   448,
     473,   474,   475,   476,   477,   464,   465,   478,   479,   513,
     516,   457,   520,   566,   719,   521,   517,   578,   810,   776,
     518,   519,   604,   799,   585,   537,   712,   538,   606,   818,
     539,   582,   540,   541,   542,   543,   544,   546,   553,   567,
     579,   572,   573,   574,   577,   580,   581,   583,   921,   584,
     586,   616,   587,   588,   589,   590,   610,   634,   591,   592,
     593,   614,   607,   608,   609,   611,   612,   613,   635,   618,
     619,   620,   637,   621,   622,   640,   623,   624,   639,   641,
     642,   653,   643,   645,   657,   654,   655,   656,   658,   720,
     728,   713,   777,   795,   714,   778,   779,   780,   781,   782,
     783,   717,   784,   721,   725,   785,   786,   787,   788,   789,
     790,   791,   792,   793,   794,   800,   801,   804,   805,   806,
     812,   808,   819,   797,   840,   820,  1017,   803,   821,   807,
     822,   823,   824,   825,   826,   827,   828,   829,   830,   831,
     832,   833,   834,   835,   836,   837,   838,   841,   910,   915,
     916,  1018,   919,  1019,  1025,  1020,  1023,   917,   918,  1068,
    1031,   920,   986,   923,  1024,  1016,  1027,  1028,  1026,  1029,
    1021,  1022,  1030,  1033,  1034,  1035,  1036,  1038,  1039,  1040,
    1041,  1043,  1054,  1069,  1070,  1078,     8,  1071,  1089,  1090,
    1091,  1072,  1094,  1097,  1076,   346,  1077,  1108,  1082,  1083,
    1087,  1098,  1088,  1092,  1095,  1096,  1099,  1101,  1102,  1103,
    1104,  1105,  1106,  1107,  1109,  1110,  1111,  1112,  1115,  1113,
    1114,  1057,  1116,  1117,  1124,  1118,  1119,  1126,  1120,  1121,
    1122,  1123,  1125,  1127,  1129,  1128,  1130,  1131,   452,  1132,
    1133,  1135,  1134,  1136,  1137,  1139,  1138,  1141,  1143,  1145,
     191,  1147,  1140,  1142,  1149,  1151,  1144,   456,  1037,  1032,
    1146,  1042,  1148,  1150,   839,   151,  1152,    41,   913,   481,
     914,   775,   981,  1093,   302,   798,   385,   802,   576,   303,
     660,   172,   817,   796,     0,    71,     0,     0,     0,     0,
       0,     0,   200,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   193,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   325,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   372
};

static const short int yycheck[] =
{
     221,   192,     1,   195,     6,     6,   408,     5,     6,   366,
     367,    11,     5,     6,   109,   417,   418,     1,     1,     5,
       6,     1,     6,    26,     5,     6,     6,    27,   661,     5,
       1,    27,    11,     1,   320,   128,     1,     6,   585,    59,
       1,   134,   135,   136,   137,   138,   139,   440,    27,     6,
      11,    71,    27,    35,    36,    27,    28,     1,   440,    27,
     440,     6,     0,     1,     1,   158,    31,    49,   161,    11,
       5,     4,     5,   440,   295,   296,   297,   298,   299,   300,
       1,   176,   175,    27,    26,     6,     4,     1,   183,   281,
       6,     4,   313,    31,    27,    28,   164,   165,     1,     6,
      28,   169,    28,     6,   487,   488,    27,   654,   655,   656,
      28,    27,    11,    27,    27,    28,   509,   750,     5,    26,
     215,   216,   217,   218,   219,   220,    25,   509,     5,   509,
     321,   129,   130,   131,   132,   133,   129,   130,   131,   132,
     133,     1,   509,   129,   130,   131,   132,   133,   129,   130,
     131,   132,   133,   129,   130,   131,   132,   133,    11,     1,
       8,   162,   163,    27,     6,    25,    11,    15,    27,    28,
     263,    11,   144,    26,     4,     5,   158,   159,   160,     5,
       1,    26,   373,   128,   567,   568,    26,   570,    27,   134,
     135,   136,   137,   138,   139,    11,   387,    27,    28,    11,
     203,   170,   171,   172,    27,    26,     6,   164,   165,    25,
     303,   402,   169,   158,    26,    11,   161,   154,   155,   156,
     157,     1,    27,     4,     5,    27,     1,     4,    27,    25,
     175,     6,   146,   147,   148,   149,   150,   151,   152,   153,
     208,   209,   210,   211,   212,   213,    27,    28,   440,    26,
      11,    28,    11,    11,   254,   255,   256,   257,   258,   259,
     260,   261,   262,    32,    33,    26,    25,    25,   459,   460,
     461,   462,   463,   176,     5,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   166,   167,   168,   644,   287,   288,
     289,   290,   291,   292,   293,   294,   295,   296,   281,   282,
     283,   284,   285,   286,   306,   307,   308,   309,   310,   311,
     312,     5,   314,   315,   316,   317,   718,   509,   263,   318,
     304,   319,     1,   305,   304,   321,   319,     6,   299,   300,
     301,   302,     4,   319,   176,   318,   297,   298,   319,     5,
      11,    25,    26,   319,    26,    27,    11,    11,   208,   209,
     210,   211,   212,   213,    25,    27,    28,     4,   303,    24,
      24,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,   276,   277,   278,   279,   280,    26,    27,
      27,    28,   203,   107,   108,   109,   110,   111,   112,   113,
      26,    27,   583,   214,   215,   216,   217,   218,   162,   163,
     621,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,     5,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   219,
     220,   221,   222,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,   244,     1,    49,   777,    26,    52,
       6,   781,   170,   171,   172,   785,   786,   787,   788,   789,
     790,   791,   821,   822,   794,   824,   825,   826,   827,    27,
     800,    27,    27,     6,    26,    27,    28,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   192,   193,   194,   195,     1,    26,   829,
     830,   831,   832,   833,   834,    38,    26,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    20,    21,    22,    23,
      26,    27,    28,    11,     6,   128,    25,    26,    27,    28,
      29,   134,    26,   136,    26,   138,    24,    25,    26,    27,
      26,    29,   145,   196,   197,   198,   199,   200,   201,     1,
     140,   141,   142,   143,     6,   158,    38,    26,    40,    41,
      42,    43,    44,    45,    46,    47,    48,   140,   141,   142,
     173,   174,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   140,   141,   142,    26,
     146,   147,   148,   149,   150,   151,   152,   153,    50,    51,
      26,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,     1,   306,   307,   308,   309,   310,   311,   312,
      26,   314,   315,   316,   317,    38,    26,    40,    41,    42,
      43,    44,    45,    46,    47,    48,   245,   246,   247,   248,
     249,   250,   251,   206,   207,   252,   253,     6,   972,   973,
     974,   975,   976,   977,   178,   179,   180,   181,   182,   779,
     780,    50,    51,    27,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    50,    51,    27,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,   783,   784,    27,    27,    27,    27,   320,    26,    29,
      26,    26,    14,     6,    14,    14,     5,    27,    27,    27,
      27,    27,    27,    27,    27,     9,    27,   320,    15,    27,
      27,    26,    26,    29,    26,    26,     5,    25,     5,     5,
       5,    27,     5,     5,    14,    27,     6,     5,     9,    26,
      26,    10,     9,     9,     9,     9,     9,     7,    26,     7,
       7,    25,     7,   320,   150,   320,   320,   150,   150,   150,
      14,    10,    10,    13,    26,    26,    25,    14,    14,    25,
      14,    14,    14,    14,    14,    27,    27,    14,    14,    17,
      25,    34,    15,     5,   320,    17,    25,    10,   204,   320,
      25,    25,     5,   320,    14,    26,     7,    27,     5,   320,
      29,    25,    30,    30,    30,    30,    27,    26,    26,    26,
      15,    27,    27,    27,    26,    15,    15,    26,   205,    27,
      26,    12,    26,    26,    26,    26,    18,     5,    26,    26,
      26,    15,    27,    27,    27,    25,    25,    25,     5,    26,
      26,    26,     5,    26,    26,    15,    27,    27,     8,    15,
      25,    13,    17,    26,     5,    14,    14,    14,    14,    27,
      29,    25,    14,     6,    25,    14,    14,    14,    14,    14,
      14,    26,    14,    27,    27,    14,    14,    14,    14,    14,
      14,    14,    14,    14,    14,    14,     6,     8,    15,     7,
      13,    16,    14,    34,    34,    14,   320,    25,    14,    25,
      14,    14,    14,    14,    14,    14,    14,    14,    14,    14,
      14,    14,    14,    14,    14,    14,     6,     6,     6,    15,
       7,   320,     8,   320,     7,   320,     8,    25,    25,     8,
       6,    26,    24,    27,    15,    26,    14,    14,    18,    14,
      25,    25,    14,    14,    14,    14,     6,    14,    14,    14,
       6,   320,   320,    15,     7,    29,     3,    25,   253,   252,
     320,    25,    10,    15,    27,   303,    27,     8,    27,    27,
      27,    15,    27,    27,    25,    25,    25,    15,    15,    25,
      25,    15,    25,    25,    15,    25,     8,    15,    15,    10,
      25,   201,    10,    25,     8,    15,    25,     8,    15,    25,
      15,    25,    25,    10,    10,    15,    25,    15,   388,    25,
      15,    15,    25,    25,    15,    15,    25,    15,    15,    15,
     144,    15,    25,    25,    15,    15,    25,   392,   936,   930,
      25,   942,    25,    25,   750,    74,    25,    24,   800,   429,
     800,   662,   823,  1067,   223,   705,   346,   709,   509,   226,
     630,    87,   726,   684,    -1,    43,    -1,    -1,    -1,    -1,
      -1,    -1,   155,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   149,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   281,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   329
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short int yystos[] =
{
       0,     1,    31,   323,   324,   320,    27,     0,   324,     5,
     325,    28,   331,   332,    28,   326,    35,    36,    49,   158,
     159,   160,   305,   327,   328,   329,   345,   347,   371,   386,
     560,   571,   591,     5,     5,    27,    27,    27,    27,     5,
       6,   329,   346,   348,     5,     5,     5,     5,   372,    38,
      40,    41,    42,    43,    44,    45,    46,    47,    48,   350,
     351,   353,   354,   356,   358,   360,   362,   364,   366,   368,
     370,   350,   387,   592,   561,   572,   306,   307,   308,   309,
     310,   311,   312,   314,   315,   316,   317,   373,   374,   375,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
      27,   355,   357,   359,   361,   363,   365,   367,   369,    27,
       6,   351,     6,     1,    50,    51,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,   390,   393,   394,    27,    28,   334,   589,
     590,   332,   164,   165,   169,   573,   574,   575,   576,   584,
      26,    26,    26,    26,    26,    26,    26,    26,    26,    26,
      26,     6,   374,    32,    33,   352,    27,    27,    27,    27,
      27,    27,    27,    27,   352,   320,    26,    29,    26,    26,
       6,   394,    14,   590,     6,   562,    14,    14,     5,     6,
     574,    27,    27,    27,    27,    27,    27,    27,    27,    27,
      27,    27,    29,   352,     9,   349,   349,   349,   349,   349,
     349,   352,     4,    26,    28,   333,    26,     1,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   389,    25,    26,    29,   334,
     342,   343,    49,    52,   128,   134,   136,   138,   145,   173,
     174,   388,   395,   399,   427,   429,   431,   433,   461,   555,
     557,   558,   559,   591,    26,   166,   167,   168,   585,   586,
     587,   588,   577,    26,    26,   352,   352,   352,   352,   352,
     352,   342,   333,   389,   320,   107,   108,   109,   110,   111,
     112,   113,   391,    15,    27,     5,     5,     5,     5,     5,
       5,    14,    27,   330,     6,   559,   170,   171,   172,   578,
     579,   580,   581,   582,   583,     9,   341,    10,   342,   342,
     342,   342,   342,   342,     9,   336,   391,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   392,   342,   396,   400,   428,   430,   432,   462,   334,
       5,     6,   579,    26,    25,    26,     9,   340,     9,   339,
       9,   338,     9,   337,     7,   392,    26,    26,   397,   398,
       1,    27,   401,   402,   403,   404,   405,   406,   407,   408,
     409,     5,   129,   130,   131,   132,   133,   319,   410,   411,
     412,   415,   416,   417,   419,   421,   423,   410,   410,     1,
      27,   146,   147,   148,   149,   150,   151,   152,   153,   463,
     464,   465,   466,   467,   468,   469,   471,   472,   473,   474,
     556,   334,    25,    26,    25,     7,     7,     7,    25,    26,
     334,     6,   398,   320,    14,     6,   402,    34,   413,   144,
     334,   418,   420,   422,    27,    27,     6,   411,     6,     6,
     320,    14,    14,    14,    14,    14,    14,    14,    14,    14,
       6,   464,   135,   137,   139,   161,   175,   263,   303,   399,
     427,   429,   431,   434,   435,   439,   442,   443,   444,   448,
     451,   452,   453,   457,   460,   476,   477,   517,   546,   553,
     554,   563,   591,    17,    10,    10,    25,    25,    25,    25,
      15,    17,    11,    24,    25,    26,    27,    29,   335,   150,
      26,   334,    26,   334,   334,   334,   334,    26,    27,    29,
      30,    30,    30,    30,    27,   470,    26,     1,   154,   155,
     156,   157,   475,    26,     4,     5,    27,    28,     4,     5,
      27,    28,     4,     5,    27,    28,     5,    26,    26,   330,
      26,   330,    27,    27,    27,     6,   554,    26,    10,    15,
      15,    15,    25,    26,    27,    14,    26,    26,    26,    26,
      26,    26,    26,    26,    26,    27,    13,   320,   440,   449,
     458,   564,   330,   330,     5,   330,     5,    27,    27,    27,
      18,    25,    25,    25,    15,   334,    12,   470,    26,    26,
      26,    26,    26,    27,    27,   150,   150,   150,   162,   163,
     565,   566,   567,   568,     5,     5,   519,     5,   548,     8,
      15,    15,    25,    17,   414,    26,   342,     1,    20,    21,
      22,    23,   424,    13,    14,    14,    14,     5,    14,     6,
     566,   478,   518,     1,   176,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   277,   278,
     279,   280,   485,   520,   521,   522,   523,   524,   525,   526,
     527,   528,   529,   530,   531,   532,   533,   534,   535,   536,
     537,   538,   539,   540,   541,   547,     1,   304,   549,   550,
     551,   552,     7,    25,    25,     8,    15,    26,   410,   320,
      27,    27,   470,   470,   470,    27,   569,   570,    29,     1,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   479,
     480,   481,   482,   483,   484,   485,   488,   489,   490,   491,
     492,   493,   494,   495,   496,   497,   498,   499,   500,   501,
     502,   503,   504,   505,   513,   520,   320,    14,    14,    14,
      14,    14,    14,    14,    14,    14,    14,    14,    14,    14,
      14,    14,    14,    14,    14,     6,   522,    34,   549,   320,
      14,     6,   551,    25,     8,    15,     7,    25,    16,     6,
     204,   426,    13,   441,   450,   459,     6,   570,   320,    14,
      14,    14,    14,    14,    14,    14,    14,    14,    14,    14,
      14,    14,    14,    14,    14,    14,    14,    14,     6,   481,
      34,     6,    27,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   335,   486,   487,     1,   281,   282,   283,   284,
     285,   286,   318,   542,   542,   542,    24,   335,     1,   287,
     288,   289,   290,   291,   292,   293,   294,   295,   296,   318,
     543,   543,   543,    26,   335,    26,   335,    26,   335,    26,
     335,    26,   335,    26,   335,     1,   297,   298,   335,   544,
       1,   299,   300,   301,   302,   545,    27,   321,    24,   335,
       6,    27,   335,   486,   487,    15,     7,    25,    25,     8,
      26,   205,   425,    27,   140,   141,   142,   143,   436,   437,
     438,   140,   141,   142,   445,   446,   447,   140,   141,   142,
     454,   455,   456,     1,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     508,   245,   246,   247,   248,   249,   250,   251,   509,   509,
     509,   508,   509,   509,   509,   509,    24,    25,   335,    25,
     335,    25,   335,    25,   335,    25,   335,    25,   335,     1,
     214,   215,   216,   217,   218,   512,   196,   197,   198,   199,
     200,   201,   506,   206,   207,   514,    26,   320,   320,   320,
     320,    25,    25,     8,    15,     7,    18,    14,    14,    14,
      14,     6,   437,    14,    14,    14,     6,   446,    14,    14,
      14,     6,   455,   320,   252,   253,   510,   511,   510,   510,
     510,   510,   510,   510,   320,    26,   203,   201,   507,     1,
     208,   209,   210,   211,   212,   213,   516,   515,     8,    15,
       7,    25,    25,     4,    27,    28,    27,    27,    29,     4,
      27,    28,    27,    27,     4,    27,    28,    27,    27,   253,
     252,   320,    27,   516,    10,    25,    25,    15,    15,    25,
     344,    15,    15,    25,    25,    15,    25,    25,     8,    15,
      25,     8,    15,    10,    25,    15,    10,    25,    15,    25,
      15,    25,    15,    25,     8,    25,     8,    10,    15,    10,
      25,    15,    25,    15,    25,    15,    25,    15,    25,    15,
      25,    15,    25,    15,    25,    15,    25,    15,    25,    15,
      25,    15,    25
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr,					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname[yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      size_t yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

#endif /* YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()
    ;
#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:

    {
                g_bGlobalAttributes = false;
                g_pkCurrAttribTable = 0;
                g_pkCurrObjectTable = 0;
                g_uiCurrPDStream = 0;    
                g_bCurrPDFixedFunction = false;
                g_pkCurrPackingDef = 0;
                g_pkCurrRequirements = 0;
                g_uiCurrImplementation = 0;
                g_pkCurrImplementation = 0;
                g_pkCurrRSGroup = 0;
                g_uiCurrImplemVSConstantMap = 0;
                g_uiCurrImplemGSConstantMap = 0;
                g_uiCurrImplemPSConstantMap = 0;
                g_uiCurrPassVSConstantMap = 0;
                g_uiCurrPassGSConstantMap = 0;
                g_uiCurrPassPSConstantMap = 0;
                g_pkCurrConstantMap = 0;
                g_uiCurrPassIndex = 0;
                g_pkCurrPass = 0;
                g_pkCurrTextureStage = 0;
                g_uiCurrTextureSlot = 0;
                g_pkCurrTexture = 0;
                
                g_pkCurrShader = NiNew NSFParsedShader();
                if (g_pkCurrShader)
                {
                    g_kParsedShaderList.AddTail(g_pkCurrShader);
                    g_pkCurrNSBShader = g_pkCurrShader->GetShader();
                    g_pkCurrNSBShader->SetName((yyvsp[-1].sval));
                    g_uiCurrImplementation = 0;
                }
                else
                {
                    DebugStringOut("Failed to create NSFParsedShader!\n");
                }
                
                g_iDSOIndent = 0;
                NiSprintf(g_szDSO, 1024, "\nNSF Shader - %s\n",(yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            ;}
    break;

  case 5:

    {
                    NiSprintf(g_szDSO, 1024, "Description: %s\n", (yyvsp[0].sval));
                    DebugStringOut(g_szDSO);
                    
                    if (g_pkCurrNSBShader)
                        g_pkCurrNSBShader->SetDescription((yyvsp[0].sval));
                        
                    NiFree((yyvsp[0].sval));
                ;}
    break;

  case 6:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Completed NSF Shader - %s\n",(yyvsp[-6].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrShader = 0;
                g_pkCurrNSBShader = 0;
                
                NiFree((yyvsp[-6].sval));
            ;}
    break;

  case 7:

    {
                NSFParsererror("Syntax Error: shader");
                yyclearin;
            ;}
    break;

  case 19:

    {   (yyval.sval) = 0;    ;}
    break;

  case 20:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 21:

    {
                (yyval.sval) = (yyvsp[0].sval);
            ;}
    break;

  case 22:

    {
                // Assumes $1 is a null-terminated string
                NIASSERT((yyvsp[-1].sval));
                size_t stLen = strlen((yyvsp[-1].sval)) + 1;
                if (stLen < MAX_QUOTE_LENGTH)
                    stLen = MAX_QUOTE_LENGTH;
                (yyval.sval) = NiStrcat((yyvsp[-1].sval), stLen, (yyvsp[0].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 23:

    {   (yyval.sval) = 0;    ;}
    break;

  case 24:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 25:

    {   (yyval.sval) = 0;    ;}
    break;

  case 26:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 27:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 28:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 29:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 30:

    {   (yyval.sval) = (yyvsp[-1].sval);    ;}
    break;

  case 31:

    {
                g_bRanged = false;
            ;}
    break;

  case 32:

    {
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-17].fval));
                AddFloatToLowArray((yyvsp[-15].fval));
                AddFloatToLowArray((yyvsp[-13].fval));
                AddFloatToLowArray((yyvsp[-11].fval));
                AddFloatToHighArray((yyvsp[-8].fval));
                AddFloatToHighArray((yyvsp[-6].fval));
                AddFloatToHighArray((yyvsp[-4].fval));
                AddFloatToHighArray((yyvsp[-2].fval));
            ;}
    break;

  case 33:

    {
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-13].fval));
                AddFloatToLowArray((yyvsp[-11].fval));
                AddFloatToLowArray((yyvsp[-9].fval));
                AddFloatToHighArray((yyvsp[-6].fval));
                AddFloatToHighArray((yyvsp[-4].fval));
                AddFloatToHighArray((yyvsp[-2].fval));
            ;}
    break;

  case 34:

    {
                g_bRanged = false;
            ;}
    break;

  case 35:

    {
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-17].fval));
                AddFloatToLowArray((yyvsp[-15].fval));
                AddFloatToLowArray((yyvsp[-13].fval));
                AddFloatToLowArray((yyvsp[-11].fval));
                AddFloatToHighArray((yyvsp[-8].fval));
                AddFloatToHighArray((yyvsp[-6].fval));
                AddFloatToHighArray((yyvsp[-4].fval));
                AddFloatToHighArray((yyvsp[-2].fval));
            ;}
    break;

  case 36:

    {
                g_bRanged = false;
            ;}
    break;

  case 37:

    {
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-13].fval));
                AddFloatToLowArray((yyvsp[-11].fval));
                AddFloatToLowArray((yyvsp[-9].fval));
                AddFloatToHighArray((yyvsp[-6].fval));
                AddFloatToHighArray((yyvsp[-4].fval));
                AddFloatToHighArray((yyvsp[-2].fval));
            ;}
    break;

  case 38:

    {
                g_bRanged = false;
            ;}
    break;

  case 39:

    {
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-9].fval));
                AddFloatToLowArray((yyvsp[-7].fval));
                AddFloatToHighArray((yyvsp[-4].fval));
                AddFloatToHighArray((yyvsp[-2].fval));
            ;}
    break;

  case 40:

    {
                g_bRanged = false;
            ;}
    break;

  case 41:

    {
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-2].fval));
                AddFloatToHighArray((yyvsp[-1].fval));
            ;}
    break;

  case 42:

    {
                g_bRanged = false;
            ;}
    break;

  case 43:

    {
                g_bRanged    = true;
                AddFloatToLowArray((float)(yyvsp[-2].ival));
                AddFloatToHighArray((float)(yyvsp[-1].ival));
            ;}
    break;

  case 44:

    {
                // Allow floats for backwards compatibility
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-2].fval));
                AddFloatToHighArray((yyvsp[-1].fval));
            ;}
    break;

  case 47:

    {
                AddFloatToValueArray((yyvsp[0].fval));
            ;}
    break;

  case 48:

    {
                AddFloatToValueArray((yyvsp[-30].fval));
                AddFloatToValueArray((yyvsp[-28].fval));
                AddFloatToValueArray((yyvsp[-26].fval));
                AddFloatToValueArray((yyvsp[-24].fval));
                AddFloatToValueArray((yyvsp[-22].fval));
                AddFloatToValueArray((yyvsp[-20].fval));
                AddFloatToValueArray((yyvsp[-18].fval));
                AddFloatToValueArray((yyvsp[-16].fval));
                AddFloatToValueArray((yyvsp[-14].fval));
                AddFloatToValueArray((yyvsp[-12].fval));
                AddFloatToValueArray((yyvsp[-10].fval));
                AddFloatToValueArray((yyvsp[-8].fval));
                AddFloatToValueArray((yyvsp[-6].fval));
                AddFloatToValueArray((yyvsp[-4].fval));
                AddFloatToValueArray((yyvsp[-2].fval));
                AddFloatToValueArray((yyvsp[0].fval));
            ;}
    break;

  case 49:

    {
                NiSprintf(g_szDSO, 1024, "Attribute Table Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrNSBShader)
                {
                    g_bGlobalAttributes = false;
                    g_pkCurrAttribTable = 
                        g_pkCurrNSBShader->GetAttributeTable();
                }
                else
                {
                    g_pkCurrAttribTable = 0;
                }
            ;}
    break;

  case 50:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Attribute Table End\n");
                DebugStringOut(g_szDSO);
                g_pkCurrAttribTable = 0;
            ;}
    break;

  case 51:

    {
                NiSprintf(g_szDSO, 1024, "Global Attribute Table Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrNSBShader)
                {
                    g_bGlobalAttributes = true;
                    g_pkCurrAttribTable = 
                        g_pkCurrNSBShader->GetGlobalAttributeTable();
                }
                else
                {
                    g_pkCurrAttribTable = 0;
                }
            ;}
    break;

  case 52:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Global Attribute Table End\n");
                DebugStringOut(g_szDSO);
                g_pkCurrAttribTable = 0;
            ;}
    break;

  case 53:

    {
        (yyval.ival) = 0;
    ;}
    break;

  case 54:

    {
        (yyval.ival) = (yyvsp[-1].ival);
    ;}
    break;

  case 67:

    {   (yyval.bval) = true;      ;}
    break;

  case 68:

    {   (yyval.bval) = false;     ;}
    break;

  case 69:

    {
                if (g_pkCurrAttribTable)
                {
                    if (!g_pkCurrAttribTable->AddAttribDesc_Bool(
                        (yyvsp[-2].sval), 0, (yyvsp[-1].bval) ? false : true, 
                        (yyvsp[0].bval)))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    AddAttribDesc_Bool at line %d\n"
                            "    Desc name = %s\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].sval));
                    }
                }                
                NiSprintf(g_szDSO, 1024, "    Boolean: %16s - %6s - %s\n",
                    (yyvsp[-2].sval), (yyvsp[-1].bval) ? "ARTIST" : "HIDDEN", 
                    (yyvsp[0].bval) ? "TRUE" : "FALSE");
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 70:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 71:

    {
                if (g_pkCurrAttribTable)
                {
                    if (g_bRanged && !g_bGlobalAttributes)
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_UnsignedInt(
                            (yyvsp[-3].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            (unsigned int)(yyvsp[-1].ival),
                            (unsigned int)g_afLowValues[0], 
                            (unsigned int)g_afHighValues[0]))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_UnsignedInt at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-3].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_UnsignedInt(
                            (yyvsp[-3].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            (unsigned int)(yyvsp[-1].ival)))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_UnsignedInt at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-3].sval));
                        }
                    }
                }                

                NiSprintf(g_szDSO, 1024, "       uint: %16s - %6s - %d\n",
                    (yyvsp[-3].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                    (int)(yyvsp[-1].ival));
                DebugStringOut(g_szDSO);
                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_szDSO, 1024, "             Range: "
                        "[%4d..%4d]\n", 
                        (unsigned int)g_afLowValues[0], 
                        (unsigned int)g_afHighValues[0]);
                }

                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 72:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 73:

    {
                unsigned int uiExpectedLength = (((yyvsp[-3].ival) != 0) ? (yyvsp[-3].ival) : 1) * 1;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    // test for array
                    if ((yyvsp[-3].ival))
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT,
                            (yyvsp[-3].ival),
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Float at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Float(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            *(g_afValues->GetBase()),
                            g_afLowValues[0], g_afHighValues[0]))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Float at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }
                }

                NiSprintf(g_szDSO, 1024, 
                    "      Float: %16s - %6s - %8.5f\n",
                    (yyvsp[-4].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                    *(g_afValues->GetBase()));
                DebugStringOut(g_szDSO);

                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_szDSO, 1024, "             Range: "
                        "[%8.5f..%8.5f]\n",
                        g_afLowValues[0], g_afHighValues[0]);
                    DebugStringOut(g_szDSO);
                }

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 74:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 75:

    {
                unsigned int uiExpectedLength = (((yyvsp[-3].ival) != 0) ? (yyvsp[-3].ival) : 1) * 2;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    // test for array
                    if ((yyvsp[-3].ival))
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_POINT2,
                            (yyvsp[-3].ival),
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point2 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Point2(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            g_afValues->GetBase(),
                            g_afLowValues, g_afHighValues))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point2 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }

                }

                NiSprintf(g_szDSO, 1024, "     Point2: %16s - %6s - "
                    "%8.5f,%8.5f\n",
                    (yyvsp[-4].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                    g_afValues->GetAt(0), g_afValues->GetAt(1));
                DebugStringOut(g_szDSO);
                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_szDSO, 1024, "             Range: "
                        "[(%8.5f,%8.5f)..(%8.5f,%8.5f)]\n",
                        g_afLowValues[0], g_afLowValues[1],
                        g_afHighValues[0], g_afHighValues[1]);
                    DebugStringOut(g_szDSO);
                }

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 76:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 77:

    {
                unsigned int uiExpectedLength = (((yyvsp[-3].ival) != 0) ? (yyvsp[-3].ival) : 1) * 3;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    if ((yyvsp[-3].ival))
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_POINT3,
                            (yyvsp[-3].ival),
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point3 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Point3(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            g_afValues->GetBase(),
                            g_afLowValues, g_afHighValues))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point3 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }
                }
            
                NiSprintf(g_szDSO, 1024, "     Point3: %16s - %6s - "
                    "%8.5f,%8.5f,%8.5f\n",
                    (yyvsp[-4].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                    g_afValues->GetAt(0),
                    g_afValues->GetAt(1),
                    g_afValues->GetAt(2));
                DebugStringOut(g_szDSO);
                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_szDSO, 1024, "             Range: "
                        "[(%8.5f,%8.5f,%8.5f)..(%8.5f,%8.5f,%8.5f)]"
                        "\n",
                        g_afLowValues[0], g_afLowValues[1], 
                        g_afLowValues[2], g_afHighValues[0],
                        g_afHighValues[1], g_afHighValues[2]);
                    DebugStringOut(g_szDSO);
                }

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 78:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 79:

    {
                unsigned int uiExpectedLength = (((yyvsp[-3].ival) != 0) ? (yyvsp[-3].ival) : 1) * 4;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    // test for array
                    if ((yyvsp[-3].ival))
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_POINT4,
                            (yyvsp[-3].ival),
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point4 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Point4(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            g_afValues->GetBase(),
                            g_afLowValues, g_afHighValues))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point4 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }

                }

                NiSprintf(g_szDSO, 1024, "     Point4: %16s - %6s - "
                    "%8.5f,%8.5f,%8.5f,%8.5f\n",
                    (yyvsp[-4].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                    g_afValues->GetAt(0), g_afValues->GetAt(1), 
                    g_afValues->GetAt(2), g_afValues->GetAt(3));
                DebugStringOut(g_szDSO);
                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_szDSO, 1024, "             Range: "
                        "[(%8.5f,%8.5f,%8.5f,%8.5f).."
                        "(%8.5f,%8.5f,%8.5f,%8.5f)]\n",
                        g_afLowValues[0], g_afLowValues[1], 
                        g_afLowValues[2], g_afLowValues[3],
                        g_afHighValues[0], g_afHighValues[1], 
                        g_afHighValues[2], g_afHighValues[3]);
                    DebugStringOut(g_szDSO);
                }

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 80:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 81:

    {
                unsigned int uiExpectedLength = (((yyvsp[-2].ival) != 0) ? (yyvsp[-2].ival) : 1) * 9;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    if ((yyvsp[-2].ival))
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            (yyvsp[-3].sval), 0, (yyvsp[-1].bval) ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3,
                            (yyvsp[-2].ival),
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Matrix3 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-3].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Matrix3(
                                (yyvsp[-3].sval), 0, (yyvsp[-1].bval) ? false : true, 
                                g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Matrix3 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-3].sval));
                        }
                    }

                }

                NiSprintf(g_szDSO, 1024, 
                    "    Matrix3: %16s - %6s - %8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f\n",
                    (yyvsp[-3].sval), (yyvsp[-1].bval) ? "ARTIST" : "HIDDEN", 
                    g_afValues->GetAt(0),
                    g_afValues->GetAt(1),
                    g_afValues->GetAt(2),
                    " ", " ",
                    g_afValues->GetAt(3),
                    g_afValues->GetAt(4),
                    g_afValues->GetAt(5),
                    " ", " ",
                    g_afValues->GetAt(6),
                    g_afValues->GetAt(7),
                    g_afValues->GetAt(8));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 82:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 83:

    {
                unsigned int uiExpectedLength = (((yyvsp[-2].ival) != 0) ? (yyvsp[-2].ival) : 1) * 16;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    if ((yyvsp[-2].ival))
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            (yyvsp[-3].sval), 0, (yyvsp[-1].bval) ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4,
                            (yyvsp[-2].ival),
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Matrix4 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-3].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Matrix4(
                                (yyvsp[-3].sval), 0, (yyvsp[-1].bval) ? false : true, 
                                g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Matrix4 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-3].sval));
                        }
                    }
                }                

                NiSprintf(g_szDSO, 1024, 
                    "  Transform: %16s - %6s - %8.5f,%8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f,%8.5f\n",
                    (yyvsp[-3].sval), (yyvsp[-1].bval) ? "ARTIST" : "HIDDEN", 
                            g_afValues->GetAt( 0), g_afValues->GetAt( 1), 
                            g_afValues->GetAt( 2), g_afValues->GetAt( 3),
                    " ", " ", g_afValues->GetAt( 4), g_afValues->GetAt( 5),
                            g_afValues->GetAt( 6), g_afValues->GetAt( 7),
                    " ", " ", g_afValues->GetAt( 8), g_afValues->GetAt( 9),
                            g_afValues->GetAt(10), g_afValues->GetAt(11),
                    " ", " ", g_afValues->GetAt(12), g_afValues->GetAt(13),
                            g_afValues->GetAt(14), g_afValues->GetAt(15));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 84:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 85:

    {
                // This one is a bit special, the expected length is 3 *or* 4
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < 3)
                {
                    // Pad out to 3 if too short (with 0.0f's)
                    for (; uiFoundLength < 3; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > 4)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected 3 or 4 float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiFoundLength);

                    g_afValues->SetSize(4);
                }

                if (g_afValues->GetSize() == 3)
                {
                    if (g_pkCurrAttribTable)
                    {
                        if (g_bRanged && !g_bGlobalAttributes)
                        {
                            if (!g_pkCurrAttribTable->AddAttribDesc_Color(
                                (yyvsp[-3].sval), 0, 
                                (yyvsp[-2].bval) ? false : true, 
                                g_afValues->GetBase(),
                                g_afLowValues, g_afHighValues))
                            {
                                NiShaderFactory::ReportError(
                                    NISHADERERR_UNKNOWN, 
                                    true, "* PARSE ERROR: %s\n"
                                    "    AddAttribDesc_Color at line %d\n"
                                    "    Desc name = %s\n",
                                    g_pkFile->GetFilename(), 
                                    NSFParserGetLineNumber(), (yyvsp[-3].sval));
                            }
                        }
                        else
                        {
                            if (!g_pkCurrAttribTable->AddAttribDesc_Color(
                                (yyvsp[-3].sval), 0, 
                                (yyvsp[-2].bval) ? false : true, 
                                g_afValues->GetBase(),
                                g_afLowValues, g_afHighValues))
                            {
                                NiShaderFactory::ReportError(
                                    NISHADERERR_UNKNOWN, 
                                    true, "* PARSE ERROR: %s\n"
                                    "    AddAttribDesc_Color at line %d\n"
                                    "    Desc name = %s\n",
                                    g_pkFile->GetFilename(), 
                                    NSFParserGetLineNumber(), (yyvsp[-3].sval));
                            }
                        }
                    }                

                    NiSprintf(g_szDSO, 1024, "      Color: %16s - %6s - "
                        "%8.5f,%8.5f,%8.5f\n",
                        (yyvsp[-3].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                        g_afValues->GetAt(0),
                        g_afValues->GetAt(1),
                        g_afValues->GetAt(2));
                    DebugStringOut(g_szDSO);
                    if (g_bRanged && !g_bGlobalAttributes)
                    {
                        NiSprintf(g_szDSO, 1024, "             Range: "
                            "[(%8.5f,%8.5f,%8.5f).."
                            "(%8.5f,%8.5f,%8.5f)]\n",
                            g_afLowValues[0], g_afLowValues[1], 
                            g_afLowValues[2],
                            g_afHighValues[0], g_afHighValues[1], 
                            g_afHighValues[2]);
                        DebugStringOut(g_szDSO);
                    }
                }
                else
                {
                    if (g_pkCurrAttribTable)
                    {
                        if (g_bRanged && !g_bGlobalAttributes)
                        {
                            if (!g_pkCurrAttribTable->AddAttribDesc_ColorA(
                                (yyvsp[-3].sval), 0, 
                                (yyvsp[-2].bval) ? false : true, 
                                g_afValues->GetBase(),
                                g_afLowValues, g_afHighValues))
                            {
                                NiShaderFactory::ReportError(
                                    NISHADERERR_UNKNOWN, 
                                    true, "* PARSE ERROR: %s\n"
                                    "    AddAttribDesc_ColorA at line %d\n"
                                    "    Desc name = %s\n",
                                    g_pkFile->GetFilename(), 
                                    NSFParserGetLineNumber(), (yyvsp[-3].sval));
                            }
                        }
                        else
                        {
                            if (!g_pkCurrAttribTable->AddAttribDesc_ColorA(
                                (yyvsp[-3].sval), 0, 
                                (yyvsp[-2].bval) ? false : true, 
                                g_afValues->GetBase(),
                                g_afLowValues, g_afHighValues))
                            {
                                NiShaderFactory::ReportError(
                                    NISHADERERR_UNKNOWN, 
                                    true, "* PARSE ERROR: %s\n"
                                    "    AddAttribDesc_ColorA at line %d\n"
                                    "    Desc name = %s\n",
                                    g_pkFile->GetFilename(), 
                                    NSFParserGetLineNumber(), (yyvsp[-3].sval));
                            }
                        }
                    }                

                    NiSprintf(g_szDSO, 1024, "     ColorA: %16s - %6s - "
                        "%8.5f,%8.5f,%8.5f,%8.5f\n",
                        (yyvsp[-3].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                        g_afValues->GetAt(0), g_afValues->GetAt(1), 
                        g_afValues->GetAt(2), g_afValues->GetAt(3));
                    DebugStringOut(g_szDSO);
                    if (g_bRanged && !g_bGlobalAttributes)
                    {
                        NiSprintf(g_szDSO, 1024, "             Range: "
                            "[(%8.5f,%8.5f,%8.5f,%8.5f).."
                            "(%8.5f,%8.5f,%8.5f,%8.5f)]\n",
                            g_afLowValues[0], g_afLowValues[1], 
                            g_afLowValues[2], g_afLowValues[3],
                            g_afHighValues[0], g_afHighValues[1], 
                            g_afHighValues[2], g_afHighValues[3]);
                        DebugStringOut(g_szDSO);
                    }
                }

                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 86:

    {
                if (g_pkCurrAttribTable)
                {
                    if (!g_pkCurrAttribTable->AddAttribDesc_Texture(
                        (yyvsp[-3].sval), 0, (yyvsp[-2].bval) ? false : true, 
                        (unsigned int)(yyvsp[-1].ival), (yyvsp[0].sval)))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    AddAttribDesc_Texture at line %d\n"
                            "    Desc name = %s\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].bval));
                    }
                }                
                NiSprintf(g_szDSO, 1024, "    Texture: %16s - %6s - Slot %d\n",
                    (yyvsp[-3].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                    (int)(yyvsp[-1].ival));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-3].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 87:

    {
                if (g_pkCurrAttribTable)
                {
                    if (!g_pkCurrAttribTable->AddAttribDesc_Texture(
                        (yyvsp[-2].sval), 0, (yyvsp[-1].bval) ? false : true, 
                        g_uiCurrTextureSlot, (yyvsp[0].sval)))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    AddAttribDesc_Texture at line %d\n"
                            "    Desc name = %s\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-1].bval));
                    }
                }                
                NiSprintf(g_szDSO, 1024, "    Texture: %16s - %6s - Slot %d\n",
                    (yyvsp[-2].sval), (yyvsp[-1].bval) ? "ARTIST" : "HIDDEN", 
                    g_uiCurrTextureSlot);
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[0].sval));
                    
                g_uiCurrTextureSlot++;
            ;}
    break;

  case 88:

    {
                NiSprintf(g_szDSO, 1024, "Object Table Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                
                if (g_pkCurrNSBShader)
                {
                    g_pkCurrObjectTable = g_pkCurrNSBShader->GetObjectTable();
                }
                else
                {
                    g_pkCurrObjectTable = 0;
                }
            ;}
    break;

  case 89:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Object Table End\n");
                DebugStringOut(g_szDSO);
                g_pkCurrObjectTable = 0;
            ;}
    break;

  case 103:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_GENERALLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_GeneralLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 104:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_POINTLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_PointLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 105:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_DirectionalLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 106:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_SpotLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 107:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_ShadowPointLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 108:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWDIRECTIONALLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_ShadowDirectionalLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 109:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_ShadowSpotLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 110:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_EnvironmentMap");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 111:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_PROJECTEDSHADOWMAP,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_ProjectedShadowMap");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 112:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_PROJECTEDLIGHTMAP,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_ProjectedLightMap");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 113:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_FOGMAP,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_FogMap");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 114:

    {
                if (g_pkCurrNSBShader)
                {
                    g_pkCurrPackingDef = 
                        g_pkCurrNSBShader->GetPackingDef((yyvsp[-1].sval), true);
                }
                else
                {
                    g_pkCurrPackingDef = 0;
                }

                g_bCurrPDFixedFunction = false;
                
                NiSprintf(g_szDSO, 1024, "PackingDefinition Start %s\n", 
                    (yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            ;}
    break;

  case 115:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "PackingDefinition End %s\n", (yyvsp[-4].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrPackingDef = 0;
                
                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 116:

    {
                NiSprintf(g_szDSO, 1024, "Using PackingDefinition %s\n",(yyvsp[0].sval));
                DebugStringOut(g_szDSO);
                if (g_pkCurrNSBShader)
                {
                    NSBPackingDef* pkPackingDef = 
                        g_pkCurrNSBShader->GetPackingDef((yyvsp[0].sval), false);
                    if (!pkPackingDef)
                    {
                        NiSprintf(g_szDSO, 1024, "    WARNING: PackingDefinition %s "
                            "NOT FOUND\n",(yyvsp[0].sval));
                        DebugStringOut(g_szDSO);
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition %s\n"
                            "    at line %d\n"
                            "    NOT FOUND!\n",
                            g_pkFile->GetFilename(), (yyvsp[0].sval), 
                            NSFParserGetLineNumber());
                    }
                    else
                    {
                        if (g_pkCurrImplementation)
                        {
                            g_pkCurrImplementation->SetPackingDef(
                                (yyvsp[0].sval));
                        }
                    }
                }

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 117:

    {   (yyval.ival) = 0x7fffffff;                            ;}
    break;

  case 118:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_FLOAT1;        ;}
    break;

  case 119:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_FLOAT2;        ;}
    break;

  case 120:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_FLOAT3;        ;}
    break;

  case 121:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_FLOAT4;        ;}
    break;

  case 122:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_UBYTECOLOR;    ;}
    break;

  case 123:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_SHORT1;        ;}
    break;

  case 124:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_SHORT2;        ;}
    break;

  case 125:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_SHORT3;        ;}
    break;

  case 126:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_SHORT4;        ;}
    break;

  case 127:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_UBYTE4;        ;}
    break;

  case 128:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_NORMSHORT1;    ;}
    break;

  case 129:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_NORMSHORT2;    ;}
    break;

  case 130:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_NORMSHORT3;    ;}
    break;

  case 131:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_NORMSHORT4;    ;}
    break;

  case 132:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_NORMPACKED3;   ;}
    break;

  case 133:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_PBYTE1;        ;}
    break;

  case 134:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_PBYTE2;        ;}
    break;

  case 135:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_PBYTE3;        ;}
    break;

  case 136:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_PBYTE4;        ;}
    break;

  case 137:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_FLOAT2H;       ;}
    break;

  case 138:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_NORMUBYTE4;    ;}
    break;

  case 139:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_NORMUSHORT2;   ;}
    break;

  case 140:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_NORMUSHORT4;   ;}
    break;

  case 141:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_UDEC3;         ;}
    break;

  case 142:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_NORMDEC3;      ;}
    break;

  case 143:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_FLOAT16_2;     ;}
    break;

  case 144:

    {   (yyval.ival) = NSBPackingDef::NID3DPDT_FLOAT16_4;     ;}
    break;

  case 145:

    {
                NSFParsererror("Syntax Error: packing_definition_type");
                yyclearin;
            ;}
    break;

  case 146:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION0;       ;}
    break;

  case 147:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION0;       ;}
    break;

  case 148:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION1;       ;}
    break;

  case 149:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION2;       ;}
    break;

  case 150:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION3;       ;}
    break;

  case 151:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION4;       ;}
    break;

  case 152:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION5;       ;}
    break;

  case 153:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION6;       ;}
    break;

  case 154:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION7;       ;}
    break;

  case 155:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT;    ;}
    break;

  case 156:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_BLENDINDICES;   ;}
    break;

  case 157:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_NORMAL;         ;}
    break;

  case 158:

    {   (yyval.ival) =    0;                                                  ;}
    break;

  case 159:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_COLOR;          ;}
    break;

  case 160:

    {   (yyval.ival) =    0;                                                  ;}
    break;

  case 161:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0;      ;}
    break;

  case 162:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD1;      ;}
    break;

  case 163:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD2;      ;}
    break;

  case 164:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD3;      ;}
    break;

  case 165:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD4;      ;}
    break;

  case 166:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD5;      ;}
    break;

  case 167:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD6;      ;}
    break;

  case 168:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD7;      ;}
    break;

  case 169:

    {   (yyval.ival) =    0;                                                  ;}
    break;

  case 170:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TANGENT;        ;}
    break;

  case 171:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_BINORMAL;       ;}
    break;

  case 172:

    {   (yyval.ival) = NiShaderDeclaration::SPTESS_DEFAULT;            ;}
    break;

  case 173:

    {   (yyval.ival) = NiShaderDeclaration::SPTESS_PARTIALU;           ;}
    break;

  case 174:

    {   (yyval.ival) = NiShaderDeclaration::SPTESS_PARTIALV;           ;}
    break;

  case 175:

    {   (yyval.ival) = NiShaderDeclaration::SPTESS_CROSSUV;            ;}
    break;

  case 176:

    {   (yyval.ival) = NiShaderDeclaration::SPTESS_UV;                 ;}
    break;

  case 177:

    {   (yyval.ival) = NiShaderDeclaration::SPTESS_LOOKUP;             ;}
    break;

  case 178:

    {   (yyval.ival) = NiShaderDeclaration::SPTESS_LOOKUPPRESAMPLED;   ;}
    break;

  case 179:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_POSITION;      ;}
    break;

  case 180:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_BLENDWEIGHT;   ;}
    break;

  case 181:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_BLENDINDICES;  ;}
    break;

  case 182:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_NORMAL;        ;}
    break;

  case 183:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_PSIZE;         ;}
    break;

  case 184:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_TEXCOORD;      ;}
    break;

  case 185:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_TANGENT;       ;}
    break;

  case 186:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_BINORMAL;      ;}
    break;

  case 187:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_TESSFACTOR;    ;}
    break;

  case 188:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_POSITIONT;     ;}
    break;

  case 189:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_COLOR;         ;}
    break;

  case 190:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_FOG;           ;}
    break;

  case 191:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_DEPTH;         ;}
    break;

  case 192:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_SAMPLE;        ;}
    break;

  case 195:

    {
                NSFParsererror("Syntax Error: packing_definition_entries");
                yyclearin;
            ;}
    break;

  case 196:

    {
                g_uiCurrPDStream    = (unsigned int)(yyvsp[0].ival);
                NiSprintf(g_szDSO, 1024, "Stream %d\n", g_uiCurrPDStream);
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 197:

    {
                g_bCurrPDFixedFunction = (yyvsp[0].bval);
                if (g_pkCurrPackingDef)
                    g_pkCurrPackingDef->SetFixedFunction((yyvsp[0].bval));
            ;}
    break;

  case 198:

    {
                unsigned int uiParam = 
                    NiShaderDeclaration::SHADERPARAM_EXTRA_DATA_MASK;
                uiParam |= (yyvsp[-5].ival);
                unsigned int uiRegister = (unsigned int)(yyvsp[-4].ival);

                NSBPackingDef::NiD3DPackingDefType eType = 
                    (NSBPackingDef::NiD3DPackingDefType)(yyvsp[-3].ival);
                NiShaderDeclaration::ShaderParameterTesselator eTess = 
                    (NiShaderDeclaration::ShaderParameterTesselator)
                    (yyvsp[-2].ival);
                NiShaderDeclaration::ShaderParameterUsage eUsage = 
                    (NiShaderDeclaration::ShaderParameterUsage)
                    (yyvsp[-1].ival);
                unsigned int uiUsageIndex = (unsigned int)(yyvsp[0].ival);

                // Add the entry to the current stream
                if (g_pkCurrPackingDef)
                {
                    if (!g_pkCurrPackingDef->AddPackingEntry(
                        g_uiCurrPDStream, uiRegister, uiParam, eType, 
                        eTess, eUsage, uiUsageIndex))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition failed AddPackingEntry\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                    }
                }

                const char* pszParam = NSBPackingDef::GetParameterName(
                    (NiShaderDeclaration::ShaderParameter)uiParam);
                const char* pszType = NSBPackingDef::GetTypeName(eType);

                NiSprintf(g_szDSO, 1024, "    %16s %2d - Reg %3d - %16s - "
                    "0x%08x, 0x%08x, 0x%08x\n", 
                    pszParam, (int)(yyvsp[-5].ival), uiRegister, pszType, eTess, 
                    eUsage, uiUsageIndex);
                DebugStringOut(g_szDSO);

                NiSprintf(g_szDSO, 1024, "    %16s %2d - Reg %3d - %16s\n", 
                    pszParam, (int)(yyvsp[-5].ival), uiRegister, pszType);
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 199:

    {
                unsigned int uiParam = 
                    NiShaderDeclaration::SHADERPARAM_EXTRA_DATA_MASK;
                uiParam |= (yyvsp[-2].ival);
                unsigned int uiRegister = (unsigned int)(yyvsp[-1].ival);
                NSBPackingDef::NiD3DPackingDefType eType = 
                    (NSBPackingDef::NiD3DPackingDefType)(yyvsp[0].ival);
                
                // Add the entry to the current stream
                if (g_pkCurrPackingDef)
                {
                    if (!g_pkCurrPackingDef->AddPackingEntry(g_uiCurrPDStream, 
                        uiRegister, uiParam, eType,

                        NiShaderDeclaration::SPTESS_DEFAULT, 
                        NiShaderDeclaration::SPUSAGE_COUNT, 0))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition failed AddPackingEntry\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                    }
                }
                
                const char* pszParam = NSBPackingDef::GetParameterName(
                    (NiShaderDeclaration::ShaderParameter)uiParam);
                const char* pszType = NSBPackingDef::GetTypeName(eType);

                NiSprintf(g_szDSO, 1024, "    %16s %2d - Reg %3d - %16s\n", 
                    pszParam, (int)(yyvsp[-2].ival), uiRegister, pszType);
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 200:

    {
                unsigned int uiParam = (yyvsp[-5].ival);
                unsigned int uiRegister = (unsigned int)(yyvsp[-4].ival);
                NSBPackingDef::NiD3DPackingDefType eType = 
                    (NSBPackingDef::NiD3DPackingDefType)(yyvsp[-3].ival);
                NiShaderDeclaration::ShaderParameterTesselator eTess = 
                    (NiShaderDeclaration::ShaderParameterTesselator)
                    (yyvsp[-2].ival);
                NiShaderDeclaration::ShaderParameterUsage eUsage = 
                    (NiShaderDeclaration::ShaderParameterUsage)
                    (yyvsp[-1].ival);
                unsigned int uiUsageIndex = (unsigned int)(yyvsp[0].ival);

                // Add the entry to the current stream
                if (g_pkCurrPackingDef)
                {
                    if (!g_pkCurrPackingDef->AddPackingEntry(
                        g_uiCurrPDStream, uiRegister, uiParam, eType, 
                        eTess, eUsage, uiUsageIndex))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition failed AddPackingEntry\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                    }
                }

                const char* pszParam = NSBPackingDef::GetParameterName(
                    (NiShaderDeclaration::ShaderParameter)uiParam);
                const char* pszType = NSBPackingDef::GetTypeName(eType);

                NiSprintf(g_szDSO, 1024, "    %16s    - Reg %3d - %16s - "
                    "0x%08x, 0x%08x, 0x%08x\n", 
                    pszParam, uiRegister, pszType, eTess, eUsage, 
                    uiUsageIndex);
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 201:

    {
                unsigned int uiParam = (yyvsp[-2].ival);
                unsigned int uiRegister = (unsigned int)(yyvsp[-1].ival);
                NSBPackingDef::NiD3DPackingDefType eType = 
                    (NSBPackingDef::NiD3DPackingDefType)(yyvsp[0].ival);

                // Add the entry to the current stream
                if (g_pkCurrPackingDef)
                {
                    if (!g_pkCurrPackingDef->AddPackingEntry(
                        g_uiCurrPDStream, uiRegister, uiParam, eType,
                        NiShaderDeclaration::SPTESS_DEFAULT, 
                        NiShaderDeclaration::SPUSAGE_COUNT, 0))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition failed AddPackingEntry\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                    }
                }

                const char* pszParam = NSBPackingDef::GetParameterName(
                    (NiShaderDeclaration::ShaderParameter)uiParam);
                const char* pszType = NSBPackingDef::GetTypeName(eType);

                NiSprintf(g_szDSO, 1024, "    %16s    - Reg %3d - %16s\n", 
                    pszParam, uiRegister, pszType);
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 202:

    {
                NiSprintf(g_szDSO, 1024, "SemanticAdapterTable List Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            ;}
    break;

  case 203:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "SemanticAdapterTable List End\n");
                DebugStringOut(g_szDSO);

                //g_pkCurrRSGroup = 0;
            ;}
    break;

  case 206:

    {
                NiSprintf(g_szDSO, 1024, "SAT entry [%d elements map from "
                    "%s,%d to %s,%d,%d]\n", (yyvsp[-9].ival), (yyvsp[-8].sval), (yyvsp[-6].ival), (yyvsp[-5].sval), (yyvsp[-3].ival), (yyvsp[-1].ival));
                DebugStringOut(g_szDSO);

                if (g_pkCurrImplementation)
                {
                    NiSemanticAdapterTable& kTable =
                        g_pkCurrImplementation->GetSemanticAdapterTable();
                    unsigned int uiEntryID = kTable.GetFreeEntry();
                    
                    kTable.SetComponentCount(uiEntryID, (yyvsp[-9].ival));
                    kTable.SetGenericSemantic(uiEntryID, (yyvsp[-8].sval), (yyvsp[-6].ival));
                    kTable.SetRendererSemantic(uiEntryID, (yyvsp[-5].sval), (yyvsp[-3].ival));
                    kTable.SetSharedRendererSemanticPosition(uiEntryID, (yyvsp[-1].ival));
                }

                NiFree((yyvsp[-8].sval));
                NiFree((yyvsp[-5].sval));
            ;}
    break;

  case 207:

    {
                NiSprintf(g_szDSO, 1024, "RenderState List Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            
                // Check the pass first...    
                if (g_pkCurrPass)
                {
                    g_pkCurrRSGroup = g_pkCurrPass->GetRenderStateGroup();
                }
                else
                if (g_pkCurrImplementation)
                {
                    g_pkCurrRSGroup = 
                        g_pkCurrImplementation->GetRenderStateGroup();
                }
                else
                {
                    g_pkCurrRSGroup = 0;
                }
            ;}
    break;

  case 208:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "RenderState List End\n");
                DebugStringOut(g_szDSO);

                g_pkCurrRSGroup = 0;
            ;}
    break;

  case 211:

    {
                if (g_bCurrStateValid)
                {
                    g_pkCurrRSGroup->SetState(g_uiCurrStateState, 
                        g_uiCurrStateValue, true);

                    DebugStringOut("    SAVE\n", false);
                }
            ;}
    break;

  case 212:

    {
                if (g_bCurrStateValid)
                {
                    g_pkCurrRSGroup->SetState(g_uiCurrStateState, 
                        g_uiCurrStateValue, false);

                    DebugStringOut("\n", false);
                }
            ;}
    break;

  case 219:

    {
                NSFParsererror("Syntax Error: renderstate_entry");
                yyclearin;
            ;}
    break;

  case 220:

    {
                NSBRenderStates::NiD3DRenderState eRS = 
                    NSBRenderStates::LookupRenderState((yyvsp[-2].sval));
                if (eRS == NSBRenderStates::NID3DRS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                }
                else if (eRS == (NSBRenderStates::NID3DRS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                }
                
                NiSprintf(g_szDSO, 1024, "    %32s = ATTRIBUTE - %s", 
                    (yyvsp[-2].sval), (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 221:

    {
                NSBRenderStates::NiD3DRenderState eRS = 
                    NSBRenderStates::LookupRenderState((yyvsp[-2].sval));
                if (eRS == NSBRenderStates::NID3DRS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NID3DRS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue; 
                    if (!NSBRenderStates::LookupRenderStateValue(eRS, 
                        (yyvsp[0].sval), uiValue))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    Invalid RenderStateValue (%s)\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), (yyvsp[0].sval), 
                            NSFParserGetLineNumber());
                        g_bCurrStateValid = false;
                    }
                    else
                    {
                        g_uiCurrStateState = (unsigned int)eRS;
                        g_uiCurrStateValue = uiValue;
                        g_bCurrStateValid = true;
                    }
                }

                NiSprintf(g_szDSO, 1024, "    %32s = %s", 
                    (yyvsp[-2].sval), (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 222:

    {
                NSBRenderStates::NiD3DRenderState eRS = 
                    NSBRenderStates::LookupRenderState((yyvsp[-2].sval));
                if (eRS == NSBRenderStates::NID3DRS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NID3DRS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue = (yyvsp[0].bval) ? 1 : 0;

                    g_uiCurrStateState = (unsigned int)eRS;
                    g_uiCurrStateValue = uiValue;
                    g_bCurrStateValid = true;
                }

                NiSprintf(g_szDSO, 1024, "    %32s = %s", (yyvsp[-2].sval), 
                    (yyvsp[0].bval) ? "TRUE" : "FALSE");
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 223:

    {
                NSBRenderStates::NiD3DRenderState eRS = 
                    NSBRenderStates::LookupRenderState((yyvsp[-2].sval));
                if (eRS == NSBRenderStates::NID3DRS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NID3DRS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue = F2DW((yyvsp[0].fval));
                    
                    g_uiCurrStateState = (unsigned int)eRS;
                    g_uiCurrStateValue = uiValue;
                    g_bCurrStateValid = true;
                }

                NiSprintf(g_szDSO, 1024, "    %32s = %8.5f", 
                    (yyvsp[-2].sval), (yyvsp[0].fval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 224:

    {
                NSBRenderStates::NiD3DRenderState eRS = 
                    NSBRenderStates::LookupRenderState((yyvsp[-2].sval));
                if (eRS == NSBRenderStates::NID3DRS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NID3DRS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue = (yyvsp[0].dword);

                    g_uiCurrStateState = (unsigned int)eRS;
                    g_uiCurrStateValue = uiValue;
                    g_bCurrStateValid = true;
                }
                NiSprintf(g_szDSO, 1024, "    %32s = 0x%08x", 
                    (yyvsp[-2].sval), (yyvsp[0].dword));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 225:

    {
                NSBRenderStates::NiD3DRenderState eRS = 
                    NSBRenderStates::LookupRenderState((yyvsp[-2].sval));
                if (eRS == NSBRenderStates::NID3DRS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NID3DRS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue = (yyvsp[0].ival);

                    g_uiCurrStateState = (unsigned int)eRS;
                    g_uiCurrStateValue = uiValue;
                    g_bCurrStateValid = true;
                }
                NiSprintf(g_szDSO, 1024, "    %32s = 0x%08x", 
                    (yyvsp[-2].sval), (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 235:

    {
                if (g_bConstantMapPlatformBlock)
                {
                    NSFParsererror("Syntax Error: "
                        "Embedded ConstantMap Platform-block");
                }
                NiSprintf(g_szDSO, 1024,"ConstantMap Platform-block Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                g_bConstantMapPlatformBlock = true;
            ;}
    break;

  case 236:

    {
                    g_uiCurrentPlatforms = (yyvsp[0].ival);
                ;}
    break;

  case 237:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "ConstantMap Platform-block End\n");
                DebugStringOut(g_szDSO);
                g_bConstantMapPlatformBlock = false;
            ;}
    break;

  case 238:

    {
                // Name, RegStart, StartBoneIndex, BoneCount
                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                unsigned int uiExtra = (unsigned int)(yyvsp[-1].ival) |
                    ((unsigned int)(yyvsp[0].ival) << 16);
                unsigned int uiRegCount = 3 * (unsigned int)(yyvsp[0].ival);
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, "SkinBoneMatrix3", uiFlags, 
                        uiExtra, (unsigned int)(yyvsp[-2].ival), uiRegCount, 
                        NULL);
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry("SkinBoneMatrix3", uiFlags, 
                        uiExtra, (unsigned int)(yyvsp[-2].ival), uiRegCount, 
                        NULL);
                }

                if (g_pkCurrImplementation)
                {
                    g_pkCurrRequirements = 
                        g_pkCurrImplementation->GetRequirements();
                    if (g_pkCurrRequirements)
                    {
                        g_pkCurrRequirements->SetBoneMatrixRegisters(3);
                        g_pkCurrRequirements->SetBoneCalcMethod(
                            NiBinaryShader::BONECALC_SKIN);
                    }
                }
                    
                NiSprintf(g_szDSO, 1024,
                    "    Defined: SkinBoneMatrix3          "
                    "%3d %3d %3d\n", (int)(yyvsp[-2].ival), (int)(yyvsp[-1].ival), 
                    int((yyvsp[0].ival)));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 239:

    {
                // Name, RegStart, StartBoneIndex, BoneCount
                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                unsigned int uiExtra = (unsigned int)(yyvsp[-1].ival) |
                    ((unsigned int)(yyvsp[0].ival) << 16);
                unsigned int uiRegCount = 3 * (unsigned int)(yyvsp[0].ival);
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, "SkinBoneMatrix3", uiFlags, 
                        uiExtra, -1, uiRegCount, (yyvsp[-2].sval));
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry("SkinBoneMatrix3", uiFlags, 
                        uiExtra, -1, uiRegCount, (yyvsp[-2].sval));
                }

                if (g_pkCurrImplementation)
                {
                    g_pkCurrRequirements = 
                        g_pkCurrImplementation->GetRequirements();
                    if (g_pkCurrRequirements)
                    {
                        g_pkCurrRequirements->SetBoneMatrixRegisters(3);
                        g_pkCurrRequirements->SetBoneCalcMethod(
                            NiBinaryShader::BONECALC_SKIN);
                    }
                }
                    
                NiSprintf(g_szDSO, 1024,
                    "    Defined: SkinBoneMatrix3          "
                    "%24s %3d %3d\n", (yyvsp[-2].sval), (int)(yyvsp[-1].ival), 
                    int((yyvsp[0].ival)));
                DebugStringOut(g_szDSO);
                
                NiFree((yyvsp[-2].sval))
            ;}
    break;

  case 240:

    {
                // Name, RegStart, Extra
                if (NiShaderConstantMap::LookUpPredefinedMappingType(
                    (yyvsp[-2].sval)) == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED)
                {
                    char acBuffer[1024];
                    NiSprintf(acBuffer, sizeof(acBuffer),
                        "PARSE ERROR: %s (line %d)\n"
                        "Predefined mapping %s is deprecated or invalid.",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        (yyvsp[-2].sval));
                    NSFParsererror(acBuffer);
                    yyclearin;
                }
                else
                {
                    unsigned int uiFlags = 
                        NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                    if (g_bConstantMapPlatformBlock)
                    {
                        g_pkCurrConstantMap->AddPlatformSpecificEntry(
                            g_uiCurrentPlatforms, (yyvsp[-2].sval), uiFlags, 
                            (unsigned int)(yyvsp[0].ival), 
                            (unsigned int)(yyvsp[-1].ival), 0, 0);
                    }
                    else
                    {
                        g_pkCurrConstantMap->AddEntry((yyvsp[-2].sval), uiFlags, 
                            (unsigned int)(yyvsp[0].ival), 
                            (unsigned int)(yyvsp[-1].ival), 0, 0);
                    }

                    NiSprintf(g_szDSO, 1024, "    Defined: %24s %3d %3d\n",
                        (yyvsp[-2].sval), (int)(yyvsp[-1].ival), (int)(yyvsp[0].ival));
                    DebugStringOut(g_szDSO);
                }
                
                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 241:

    {
                // Name, VariableName, Extra
                if (NiShaderConstantMap::LookUpPredefinedMappingType(
                    (yyvsp[-2].sval)) == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED)
                {
                    char acBuffer[1024];
                    NiSprintf(acBuffer, sizeof(acBuffer),
                        "PARSE ERROR: %s (line %d)\n"
                        "Predefined mapping %s is deprecated or invalid.",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        (yyvsp[-2].sval));
                    NSFParsererror(acBuffer);
                    yyclearin;
                }
                else
                {
                    unsigned int uiFlags = 
                        NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                    if (g_bConstantMapPlatformBlock)
                    {
                        g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, (yyvsp[-2].sval), uiFlags, 
                        (unsigned int)(yyvsp[0].ival), -1, 0, (yyvsp[-1].sval));
                    }
                    else
                    {
                        g_pkCurrConstantMap->AddEntry((yyvsp[-2].sval), uiFlags, 
                            (unsigned int)(yyvsp[0].ival), -1, 0, (yyvsp[-1].sval));
                    }

                    NiSprintf(g_szDSO, 1024,
                        "    Defined: %24s %3d %24s %3d\n",
                        (yyvsp[-2].sval), -1, (yyvsp[-1].sval), (int)(yyvsp[0].ival));
                    DebugStringOut(g_szDSO);
                }
                
                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[-1].sval));
            ;}
    break;

  case 242:

    {
                // CM_Object, NameOfObject, Parameter, RegStart
                
                NSBObjectTable::ObjectDesc* pkDesc = NULL;
                NSBObjectTable* pkTable = g_pkCurrNSBShader->GetObjectTable();
                if (pkTable)
                {
                    pkDesc = pkTable->GetObjectByName((yyvsp[-2].sval));
                }
                if (!pkDesc)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    GetObjectByName at line %d\n"
                        "    Local name = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        (yyvsp[-2].sval));
                    break;
                }

                // Verify mapping is supported by object type.
                unsigned int uiMapping;
                bool bSuccess = NiShaderConstantMap::LookUpObjectMapping((yyvsp[-1].sval),
                    uiMapping);
                if (bSuccess)
                {
                    bSuccess = NiShaderConstantMap::IsObjectMappingValidForType(
                        (NiShaderConstantMap::ObjectMappings) uiMapping,
                        pkDesc->GetType());
                }
                if (!bSuccess)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    IsObjectMappingValidForType at line %d\n"
                        "    Object name = %s\n"
                        "    Object mapping = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        (yyvsp[-2].sval),
                        (yyvsp[-1].sval));
                    break;
                }

                unsigned int uiFlags =
                    NiShaderConstantMapEntry::SCME_MAP_OBJECT |
                    NiShaderConstantMapEntry::GetObjectFlags(
                        pkDesc->GetType());
                
                size_t stBufSize = strlen((yyvsp[-1].sval)) + strlen((yyvsp[-2].sval)) + 3;
                char* pcKey = NiAlloc(char, stBufSize);
                NiSprintf(pcKey, stBufSize, "%s@@%s", (yyvsp[-1].sval), (yyvsp[-2].sval));
                
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pcKey, uiFlags,
                        pkDesc->GetIndex(), (unsigned int) (yyvsp[0].ival), 0, 0);
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry(pcKey, uiFlags,
                        pkDesc->GetIndex(), (unsigned int) (yyvsp[0].ival), 0, 0);
                }
                
                NiSprintf(g_szDSO, 1024, "    Object: %24s %16s %3d\n",
                    (yyvsp[-2].sval), (yyvsp[-1].sval), (yyvsp[0].ival));
                DebugStringOut(g_szDSO);
                
                NiFree(pcKey);
                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[-1].sval));
            ;}
    break;

  case 243:

    {
                // CM_Object, NameOfObject, Parameter, VariableName
                
                NSBObjectTable::ObjectDesc* pkDesc = NULL;
                NSBObjectTable* pkTable = g_pkCurrNSBShader->GetObjectTable();
                if (pkTable)
                {
                    pkDesc = pkTable->GetObjectByName((yyvsp[-2].sval));
                }
                if (!pkDesc)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    GetObjectByName at line %d\n"
                        "    Local name = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        (yyvsp[-2].sval));
                    break;
                }
                
                // Verify mapping is supported by object type.
                unsigned int uiMapping;
                bool bSuccess = NiShaderConstantMap::LookUpObjectMapping((yyvsp[-1].sval),
                    uiMapping);
                if (bSuccess)
                {
                    bSuccess = NiShaderConstantMap::IsObjectMappingValidForType(
                        (NiShaderConstantMap::ObjectMappings) uiMapping,
                        pkDesc->GetType());
                }
                if (!bSuccess)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    IsObjectMappingValidForType at line %d\n"
                        "    Local name = %s\n"
                        "    Object mapping = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        (yyvsp[-2].sval),
                        (yyvsp[-1].sval));
                    break;
                }
                
                unsigned int uiFlags =
                    NiShaderConstantMapEntry::SCME_MAP_OBJECT |
                    NiShaderConstantMapEntry::GetObjectFlags(
                        pkDesc->GetType());
                
                size_t stBufSize = strlen((yyvsp[-1].sval)) + strlen((yyvsp[-2].sval)) + 3;
                char* pcKey = NiAlloc(char, stBufSize);
                NiSprintf(pcKey, stBufSize, "%s@@%s", (yyvsp[-1].sval), (yyvsp[-2].sval));
                
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pcKey, uiFlags,
                        pkDesc->GetIndex(), -1, 0, (yyvsp[0].sval));
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry(pcKey, uiFlags,
                        pkDesc->GetIndex(), -1, 0, (yyvsp[0].sval));
                }
                
                NiSprintf(g_szDSO, 1024, "    Object: %24s %16s %16s\n",
                    (yyvsp[-2].sval), (yyvsp[-1].sval), (yyvsp[0].sval));
                DebugStringOut(g_szDSO);
                
                NiFree(pcKey);
                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[-1].sval));
                NiFree((yyvsp[0].sval));
                
            ;}
    break;

  case 244:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 245:

    {
                // Name, RegStart, RegCount
                if (!AddAttributeToConstantMap((yyvsp[-3].sval), (yyvsp[-2].ival), (yyvsp[-1].ival), (yyvsp[0].ival), false))
                {
                    // Report the error
                    NSFParsererror(
                        "Syntax Error: AddAttributeToConstantMap!");
                    yyclearin;
                }

                NiSprintf(g_szDSO, 1024, "     Attrib: %24s %3d %3d %3d\n",
                    (yyvsp[-3].sval), (int)(yyvsp[-2].ival), (int)(yyvsp[-1].ival), (int)(yyvsp[0].ival));
                DebugStringOut(g_szDSO);
                                
                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 246:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 247:

    {
                // Name, RegStart, RegCount, Values

                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_CONSTANT |
                    NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;

                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, (yyvsp[-3].sval), uiFlags, 
                        0, (unsigned int)(yyvsp[-2].ival), 
                        (unsigned int)(yyvsp[-1].ival), NULL,
                        g_afValues->GetSize() * sizeof(float),
                        sizeof(float), g_afValues->GetBase(), true);
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry((yyvsp[-3].sval), uiFlags, 
                        0, (unsigned int)(yyvsp[-2].ival), 
                        (unsigned int)(yyvsp[-1].ival), NULL,
                        g_afValues->GetSize() * sizeof(float),
                        sizeof(float), g_afValues->GetBase(), true);
                }
                
                NiSprintf(g_szDSO, 1024, "      Const: %24s %3d %3d - %3d values\n",
                    (yyvsp[-3].sval), (int)(yyvsp[-2].ival), 
                    (int)(yyvsp[-1].ival), g_afValues->GetSize());
                DebugStringOut(g_szDSO);
                
                NiSprintf(g_szDSO, 1024, "             %24s         - ", " ");
                DebugStringOut(g_szDSO);
                
                for (unsigned int ui = 0; ui < g_afValues->GetSize(); ui++)
                {
                    NiSprintf(g_szDSO, 1024, "%8.5f,", g_afValues->GetAt(ui));
                    if ((((ui + 1) % 4) == 0) ||
                        (ui == (g_afValues->GetSize() - 1)))
                    {
                        NiStrcat(g_szDSO, 1024, "\n");
                    }
                    DebugStringOut(g_szDSO, false);
                    if ((((ui + 1) % 4) == 0) &&
                        (ui < (g_afValues->GetSize() - 1)))
                    {
                        NiSprintf(g_szDSO, 1024, "             %24s         - ", " ");
                        DebugStringOut(g_szDSO);
                    }
                }
                
                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 248:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 249:

    {
                // Name, RegStart, RegCount
                if (!AddAttributeToConstantMap((yyvsp[-2].sval), (yyvsp[-1].ival), (yyvsp[0].ival), 0, true))
                {
                    // Report the error
                    NSFParsererror(
                        "Syntax Error: AddAttributeToConstantMap!");
                    yyclearin;
                }

                NiSprintf(g_szDSO, 1024, "     Global: %24s %3d %3d\n",
                    (yyvsp[-2].sval), (int)(yyvsp[-1].ival), 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_szDSO);
                                
                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 250:

    {
                // Name, type, RegStart, RegCount, Entry1 * Entry2
                if (!SetupOperatorEntry((yyvsp[-7].sval), (yyvsp[-6].ival), 
                    (yyvsp[-5].ival), (yyvsp[-4].sval), (int)(yyvsp[-3].ival), 
                    (yyvsp[-2].sval), (yyvsp[-1].bval), (yyvsp[0].bval)))
                {
                    NiFree((yyvsp[-7].sval));
                    NiFree((yyvsp[-4].sval));
                    NiFree((yyvsp[-2].sval));
                    // Report the error
                    NSFParsererror("Syntax Error: SetupOperatorEntry!");
                    yyclearin;
                }
                else
                {
                    NiFree((yyvsp[-7].sval));
                    NiFree((yyvsp[-4].sval));
                    NiFree((yyvsp[-2].sval));
                }
            ;}
    break;

  case 251:

    {   (yyval.ival) = NiShaderConstantMapEntry::SCME_OPERATOR_MULTIPLY;   ;}
    break;

  case 252:

    {   (yyval.ival) = NiShaderConstantMapEntry::SCME_OPERATOR_DIVIDE;     ;}
    break;

  case 253:

    {   (yyval.ival) = NiShaderConstantMapEntry::SCME_OPERATOR_ADD;        ;}
    break;

  case 254:

    {   (yyval.ival) = NiShaderConstantMapEntry::SCME_OPERATOR_SUBTRACT;   ;}
    break;

  case 255:

    {
                NSFParsererror("Syntax Error: operator_type");
                yyclearin;
            ;}
    break;

  case 256:

    {   (yyval.bval) = false;     ;}
    break;

  case 257:

    {   (yyval.bval) = true;      ;}
    break;

  case 258:

    {   (yyval.bval) = false;     ;}
    break;

  case 259:

    {   (yyval.bval) = true;      ;}
    break;

  case 260:

    {
                g_eConstMapMode = CONSTMAP_VERTEX;
                NiSprintf(g_szDSO, 1024, "VertexShader ConstantMap Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                
                // Check the pass first...    
                if (g_pkCurrPass)
                {
                    g_pkCurrConstantMap = g_pkCurrPass->GetVertexConstantMap(
                        g_uiCurrPassVSConstantMap++);
                }
                else
                if (g_pkCurrImplementation)
                {
                    g_pkCurrConstantMap = g_pkCurrImplementation->
                        GetVertexConstantMap(g_uiCurrImplemVSConstantMap++);
                }
                else
                {
                    g_pkCurrConstantMap = 0;
                }
            ;}
    break;

  case 261:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "VertexShader ConstantMap End\n");
                DebugStringOut(g_szDSO);

                g_pkCurrConstantMap = 0;
            ;}
    break;

  case 262:

    {
                g_eConstMapMode = CONSTMAP_GEOMETRY;
                NiSprintf(g_szDSO, 1024, "GeometryShader ConstantMap Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                
                // Check the pass first...    
                if (g_pkCurrPass)
                {
                    g_pkCurrConstantMap = g_pkCurrPass->GetGeometryConstantMap(
                        g_uiCurrPassGSConstantMap++);
                }
                else
                if (g_pkCurrImplementation)
                {
                    g_pkCurrConstantMap = g_pkCurrImplementation->
                        GetGeometryConstantMap(g_uiCurrImplemGSConstantMap++);
                }
                else
                {
                    g_pkCurrConstantMap = 0;
                }
            ;}
    break;

  case 263:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "GeometryShader ConstantMap End\n");
                DebugStringOut(g_szDSO);

                g_pkCurrConstantMap = 0;
            ;}
    break;

  case 264:

    {
                g_eConstMapMode = CONSTMAP_PIXEL;
                NiSprintf(g_szDSO, 1024, "PixelShader ConstantMap Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                // Check the pass first...    
                if (g_pkCurrPass)
                {
                    g_pkCurrConstantMap = g_pkCurrPass->GetPixelConstantMap(
                        g_uiCurrPassPSConstantMap++);
                }
                else
                if (g_pkCurrImplementation)
                {
                    g_pkCurrConstantMap = g_pkCurrImplementation->
                        GetPixelConstantMap(g_uiCurrImplemPSConstantMap++);
                }
                else
                {
                    g_pkCurrConstantMap = 0;
                }
            ;}
    break;

  case 265:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "PixelShader ConstantMap End\n");
                DebugStringOut(g_szDSO);

                g_pkCurrConstantMap = 0;
            ;}
    break;

  case 266:

    {
                if (g_pkCurrImplementation)
                    g_pkCurrImplementation->SetClassName((yyvsp[0].sval));

                NiSprintf(g_szDSO, 1024, "ClassName = %s\n", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 267:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_VERTEX);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 268:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_VERTEX);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 269:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_VERTEX);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 270:

    {
                // shader, entry point, shader target
                NiSprintf(g_szDSO, 1024, "VertexShader File EP %s, ST %s\n", 
                    (yyvsp[-1].sval), (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                SetShaderProgramEntryPoint(g_pkCurrPass, (yyvsp[-1].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_VERTEX);
                SetShaderProgramShaderTarget(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_VERTEX);
                    
                NiFree((yyvsp[-1].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 271:

    {
                // shader, entry point
                NiSprintf(g_szDSO, 1024, "VertexShader File EP %s\n", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                SetShaderProgramEntryPoint(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_VERTEX);
                    
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 272:

    {
                // shader
                NiSprintf(g_szDSO, 1024, "VertexShader File\n");
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 273:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 274:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 275:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 278:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    g_uiCurrentPlatforms,
                    NiGPUProgram::PROGRAM_VERTEX);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 279:

    {
                SetShaderProgramEntryPoint(g_pkCurrPass, (yyvsp[0].sval),
                    g_uiCurrentPlatforms,
                    NiGPUProgram::PROGRAM_VERTEX);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 280:

    {
                SetShaderProgramShaderTarget(g_pkCurrPass, (yyvsp[0].sval),
                    g_uiCurrentPlatforms,
                    NiGPUProgram::PROGRAM_VERTEX);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 281:

    {
                if (g_pkCurrPass)
                {
                    g_pkCurrPass->SetSoftwareVertexProcessing((yyvsp[0].bval));
                }
            ;}
    break;

  case 282:

    {
                NiSprintf(g_szDSO, 1024, "VertexShader Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            ;}
    break;

  case 283:

    {
                    g_uiCurrentPlatforms = (yyvsp[0].ival);
                ;}
    break;

  case 284:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "VertexShader End\n");
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 287:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 288:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 289:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 290:

    {
                // shader, entry point, shader target
                NiSprintf(g_szDSO, 1024, "GeometryShader File EP %s, ST %s\n", 
                    (yyvsp[-1].sval), (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                SetShaderProgramEntryPoint(g_pkCurrPass, (yyvsp[-1].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                SetShaderProgramShaderTarget(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                    
                NiFree((yyvsp[-1].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 291:

    {
                // shader, entry point
                NiSprintf(g_szDSO, 1024, "GeometryShader File EP %s\n", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                SetShaderProgramEntryPoint(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 292:

    {
                // shader
                NiSprintf(g_szDSO, 1024, "GeometryShader File\n");
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 293:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 294:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 295:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 298:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    g_uiCurrentPlatforms,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 299:

    {
                SetShaderProgramEntryPoint(g_pkCurrPass, (yyvsp[0].sval),
                    g_uiCurrentPlatforms,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 300:

    {
                SetShaderProgramShaderTarget(g_pkCurrPass, (yyvsp[0].sval),
                    g_uiCurrentPlatforms,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 301:

    {
                NiSprintf(g_szDSO, 1024, "GeometryShader Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            ;}
    break;

  case 302:

    {
                    g_uiCurrentPlatforms = (yyvsp[0].ival);
                ;}
    break;

  case 303:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "GeometryShader End\n");
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 306:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_PIXEL);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 307:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_PIXEL);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 308:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_PIXEL);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 309:

    {
                // shader, entry point, shader target
                NiSprintf(g_szDSO, 1024, "PixelShader File EP %s, ST %s\n", 
                    (yyvsp[-1].sval), (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                SetShaderProgramEntryPoint(g_pkCurrPass, (yyvsp[-1].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_PIXEL);
                SetShaderProgramShaderTarget(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_PIXEL);
                    
                NiFree((yyvsp[-1].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 310:

    {
                // shader, entry point
                NiSprintf(g_szDSO, 1024, "PixelShader File EP %s\n", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                SetShaderProgramEntryPoint(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_PIXEL);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 311:

    {
                // shader
                NiSprintf(g_szDSO, 1024, "PixelShader File\n");
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 312:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 313:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 314:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 317:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    g_uiCurrentPlatforms,
                    NiGPUProgram::PROGRAM_PIXEL);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 318:

    {
                SetShaderProgramEntryPoint(g_pkCurrPass, (yyvsp[0].sval),
                    g_uiCurrentPlatforms,
                    NiGPUProgram::PROGRAM_PIXEL);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 319:

    {
                SetShaderProgramShaderTarget(g_pkCurrPass, (yyvsp[0].sval),
                    g_uiCurrentPlatforms,
                    NiGPUProgram::PROGRAM_PIXEL);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 320:

    {
                NiSprintf(g_szDSO, 1024, "PixelShader Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
            ;}
    break;

  case 321:

    {
                    g_uiCurrentPlatforms = (yyvsp[0].ival);
                ;}
    break;

  case 322:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "PixelShader End\n");
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 325:

    {
                NiSprintf(g_szDSO, 1024, "Requirement List Start\n");
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrImplementation)
                {
                    g_pkCurrRequirements = 
                        g_pkCurrImplementation->GetRequirements();
                }
            ;}
    break;

  case 326:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Requirement List End\n");
                DebugStringOut(g_szDSO);

                if (g_pkCurrNSBShader && g_pkCurrRequirements)
                {
                    // Update the shader requirements
                    g_pkCurrNSBShader->AddVertexShaderVersion(
                        g_pkCurrRequirements->GetVSVersion());
                    g_pkCurrNSBShader->AddGeometryShaderVersion(
                        g_pkCurrRequirements->GetGSVersion());
                    g_pkCurrNSBShader->AddPixelShaderVersion(
                        g_pkCurrRequirements->GetPSVersion());
                    g_pkCurrNSBShader->AddUserVersion(
                        g_pkCurrRequirements->GetUserVersion());
                    g_pkCurrNSBShader->AddPlatform(
                        g_pkCurrRequirements->GetPlatformFlags());
                }
                g_pkCurrRequirements = 0;
            ;}
    break;

  case 338:

    {
                NSFParsererror("Syntax Error: requirement_entry");
                yyclearin;
            ;}
    break;

  case 339:

    {
                NiSprintf(g_szDSO, 1024, "    VSVersion   0x%08x\n", (yyvsp[0].vers));
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetVSVersion((yyvsp[0].vers));
            ;}
    break;

  case 340:

    {
                // The N_VERSION method uses the VS method to set the value.
                // We need to flip it back to the GS method here.
#ifndef _PS3
                unsigned int uiMaj = D3DSHADER_VERSION_MAJOR((yyvsp[0].vers));
                unsigned int uiMin = D3DSHADER_VERSION_MINOR((yyvsp[0].vers));
                unsigned int uiVers = D3DGS_VERSION(uiMaj, uiMin);
#else
                unsigned int uiVers = (yyvsp[0].vers);
#endif                
            
                NiSprintf(g_szDSO, 1024, "    GSVersion   0x%08x\n", uiVers);
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetGSVersion(uiVers);
            ;}
    break;

  case 341:

    {
                // The N_VERSION method uses the VS method to set the value.
                // We need to flip it back to the PS method here
#ifndef _PS3
                unsigned int uiMaj = D3DSHADER_VERSION_MAJOR((yyvsp[0].vers));
                unsigned int uiMin = D3DSHADER_VERSION_MINOR((yyvsp[0].vers));
                unsigned int uiVers = D3DPS_VERSION(uiMaj, uiMin);
#else
                unsigned int uiVers = (yyvsp[0].vers);
#endif                
                NiSprintf(g_szDSO, 1024, "    PSVersion   0x%08x\n", uiVers);
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetPSVersion(uiVers);
            ;}
    break;

  case 342:

    {
                NiSprintf(g_szDSO, 1024, "    UserDefined 0x%08x\n", (yyvsp[0].vers));
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetUserVersion((yyvsp[0].vers));
            ;}
    break;

  case 343:

    {
                NiSprintf(g_szDSO, 1024, "       Platform 0x%08x\n", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetPlatformFlags((yyvsp[0].ival));
            ;}
    break;

  case 344:

    {
                unsigned int uiPlatform = 0;
                unsigned int uiTemp;
                
                uiTemp = DecodePlatformString((yyvsp[-6].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[-6].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                uiTemp = DecodePlatformString((yyvsp[-4].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[-4].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                uiTemp = DecodePlatformString((yyvsp[-2].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[-2].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                uiTemp = DecodePlatformString((yyvsp[0].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[0].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;

                 NiFree((yyvsp[-6].sval));
                 NiFree((yyvsp[-4].sval));
                 NiFree((yyvsp[-2].sval));
                 NiFree((yyvsp[0].sval));
                (yyval.ival) = uiPlatform;
            ;}
    break;

  case 345:

    {
                unsigned int uiPlatform = 0;
                unsigned int uiTemp;
                
                uiTemp = DecodePlatformString((yyvsp[-4].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[-4].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                uiTemp = DecodePlatformString((yyvsp[-2].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[-2].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                uiTemp = DecodePlatformString((yyvsp[0].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[0].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;

                 NiFree((yyvsp[-4].sval));
                 NiFree((yyvsp[-2].sval));
                 NiFree((yyvsp[0].sval));
                (yyval.ival) = uiPlatform;
            ;}
    break;

  case 346:

    {
                unsigned int uiPlatform = 0;
                unsigned int uiTemp;
                
                uiTemp = DecodePlatformString((yyvsp[-2].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[-2].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                uiTemp = DecodePlatformString((yyvsp[0].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[0].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                
                 NiFree((yyvsp[-2].sval));
                 NiFree((yyvsp[0].sval));

                (yyval.ival) = uiPlatform;
            ;}
    break;

  case 347:

    {
                unsigned int uiPlatform = 0;
                unsigned int uiTemp;
                
                uiTemp = DecodePlatformString((yyvsp[0].sval));
                if (uiTemp == 0)
                {
                    NiSprintf(g_szDSO, 1024, "Invalid Platform %s\n", (yyvsp[0].sval));
                    DebugStringOut(g_szDSO);
                }
                else
                    uiPlatform |= uiTemp;
                
                 NiFree((yyvsp[0].sval));

                (yyval.ival) = uiPlatform;
            ;}
    break;

  case 348:

    {
                NiSprintf(g_szDSO, 1024, "%15s %s\n", (yyvsp[-2].sval),
                    (yyvsp[0].bval) ? "true" : "false");
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                {
                    // Determine the requirement field
                    if (NiStricmp((yyvsp[-2].sval), "USESNIRENDERSTATE") == 0)
                    {
                        g_pkCurrRequirements->SetUsesNiRenderState(
                            (yyvsp[0].bval));
                    }
                    else if (NiStricmp((yyvsp[-2].sval), "USESNILIGHTSTATE") == 0)
                    {
                        g_pkCurrRequirements->SetUsesNiLightState(
                            (yyvsp[0].bval));
                    }
                    else if (NiStricmp((yyvsp[-2].sval), "SOFTWAREVPREQUIRED") == 
                        0)
                    {
                        g_pkCurrRequirements->SetSoftwareVPRequired(
                            (yyvsp[0].bval));
                    }
                    else if (NiStricmp((yyvsp[-2].sval), "SOFTWAREVPFALLBACK") == 
                        0)
                    {
                        g_pkCurrRequirements->SetSoftwareVPAcceptable(
                            (yyvsp[0].bval));
                    }
                }
                
                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 349:

    {
                NiSprintf(g_szDSO, 1024, "Bones/Partition %d\n", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetBonesPerPartition((yyvsp[0].ival));
            ;}
    break;

  case 350:

    {
                NiSprintf(g_szDSO, 1024, "BinormalTangentUVSource %d\n", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                {
                    g_pkCurrRequirements->SetBinormalTangentUVSource((yyvsp[0].ival));
                }
            ;}
    break;

  case 351:

    {
                NiSprintf(g_szDSO, 1024, "BinormalTangent %d\n", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                if (g_pkCurrRequirements)
                {
                    g_pkCurrRequirements->SetBinormalTangentMethod(
                        (NiShaderRequirementDesc::NBTFlags)(yyvsp[0].ival));
                }
            ;}
    break;

  case 352:

    {   (yyval.ival) = NiShaderRequirementDesc::NBT_METHOD_NONE;   ;}
    break;

  case 353:

    {   (yyval.ival) = NiShaderRequirementDesc::NBT_METHOD_NDL;    ;}
    break;

  case 354:

    {   (yyval.ival) = NiShaderRequirementDesc::NBT_METHOD_ATI;    ;}
    break;

  case 355:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    Deprecated value for BinormalTangentMethod "
                    "Requirement\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(),
                    NSFParserGetLineNumber());
                (yyval.ival) = NiShaderRequirementDesc::NBT_METHOD_NDL;    
            ;}
    break;

  case 356:

    {
                NSFParsererror("Syntax Error: binormaltanget_method");
                yyclearin;
            ;}
    break;

  case 360:

    {
                NiSprintf(g_szDSO, 1024, "Stage Start %3d - %s\n", (yyvsp[-2].ival), 
                    (yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                
                if (g_pkCurrPass)
                {
                    g_pkCurrTextureStage = 
                        g_pkCurrPass->GetStage((int)(yyvsp[-2].ival));
                    if (g_pkCurrTextureStage)
                    {
                        g_pkCurrTextureStage->SetUseTextureTransformation(
                            false);
                    }
                }
            ;}
    break;

  case 361:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Stage End %3d - %s\n", (int)(yyvsp[-5].ival), 
                    (yyvsp[-4].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrTextureStage = 0;

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 369:

    {
                if (g_bCurrStateValid)
                {
                    if (g_pkCurrTextureStage)
                    {
                        NSBStateGroup* pkGroup = 
                            g_pkCurrTextureStage->GetTextureStageGroup();
                        if (pkGroup)
                        {
                            pkGroup->SetState(g_uiCurrStateState, 
                                g_uiCurrStateValue, true);
                        }
                    }
                
                    DebugStringOut("    SAVE\n", false);
                }
            ;}
    break;

  case 370:

    {
                if (g_bCurrStateValid)
                {
                    if (g_pkCurrTextureStage)
                    {
                        NSBStateGroup* pkGroup = 
                            g_pkCurrTextureStage->GetTextureStageGroup();
                        if (pkGroup)
                        {
                            pkGroup->SetState(g_uiCurrStateState, 
                                g_uiCurrStateValue, false);
                        }
                    }
                    
                    DebugStringOut("\n", false);
                }
            ;}
    break;

  case 390:

    {
                NSFParsererror("Syntax Error: stage_entry");
                yyclearin;
            ;}
    break;

  case 391:

    {
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetNDLMap((yyvsp[0].ival));
            ;}
    break;

  case 392:

    {
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetDecalMap((yyvsp[0].ival));
            ;}
    break;

  case 393:

    {
                bool bFoundAttribute = false;
                NSBObjectTable* pkObjectTable = 0;
                if (g_pkCurrNSBShader)
                {
                    g_bGlobalAttributes = false;
                    g_pkCurrAttribTable = 
                        g_pkCurrNSBShader->GetAttributeTable();
                    pkObjectTable = g_pkCurrNSBShader->GetObjectTable();
                }
                if (g_pkCurrAttribTable)
                {
                    NSBAttributeDesc* pkAttrib = 
                        g_pkCurrAttribTable->GetAttributeByName((yyvsp[0].sval));
                    if (pkAttrib)
                    {
                        bFoundAttribute = true;
                        
                        unsigned int uiValue;
                        const char* pszValue;
                        
                        if (pkAttrib->GetValue_Texture(uiValue, pszValue))
                        {
                            uiValue |= NiTextureStage::TSTF_MAP_SHADER;
                            if (g_pkCurrTextureStage)
                                g_pkCurrTextureStage->SetShaderMap(uiValue);
                        }
                        else
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, true,
                                "* PARSE ERROR: %s\n"
                                "    GetValue_Texture at line %d\n"
                                "    Attribute name = %s\n",
                                g_pkFile->GetFilename(),
                                NSFParserGetLineNumber(),
                                (yyvsp[0].sval));
                        }
                    }
                    g_pkCurrAttribTable = 0;
                }
                
                if (!bFoundAttribute && pkObjectTable)
                {
                    NSBObjectTable::ObjectDesc* pkDesc =
                        pkObjectTable->GetObjectByName((yyvsp[0].sval));
                    if (pkDesc)
                    {
                        NiShaderAttributeDesc::ObjectType eObjectType =
                            pkDesc->GetType();
                        if (eObjectType <
                            NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP ||
                            eObjectType >
                            NiShaderAttributeDesc::OT_EFFECT_FOGMAP)
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, true,
                                "* PARSE ERROR: %s\n"
                                "    InvalidObjectType at line %d\n"
                                "    Object name = %s\n",
                                g_pkFile->GetFilename(),
                                NSFParserGetLineNumber(),
                                (yyvsp[0].sval));
                        }
                        else
                        {
                            if (g_pkCurrTextureStage)
                            {
                                g_pkCurrTextureStage->SetObjTextureSettings(
                                    eObjectType, pkDesc->GetIndex());
                            }
                        }
                    }
                    else
                    {
                        NiShaderFactory::ReportError(
                            NISHADERERR_UNKNOWN, true,
                            "* PARSE ERROR: %s\n"
                            "    TextureNotFound at line %d\n"
                            "    Attribute/Object name = %s\n",
                            g_pkFile->GetFilename(),
                            NSFParserGetLineNumber(),
                            (yyvsp[0].sval));
                    }
                }
                
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 394:

    {           
        NSBObjectTable::ObjectDesc* pkDesc = NULL;
        NSBObjectTable* pkTable = g_pkCurrNSBShader->GetObjectTable();
        if (pkTable)
        {
            pkDesc = pkTable->GetObjectByName((yyvsp[0].sval));
        }
        if (!pkDesc)
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                false,
                "* PARSE ERROR: %s\n"
                "    GetObjectByName at line %d\n"
                "    Local name = %s\n",
                g_pkFile->GetFilename(),
                NSFParserGetLineNumber(),
                (yyvsp[0].sval));
            NiFree((yyvsp[0].sval));                             
            break;
        }
        if (g_pkCurrTextureStage)
        {
            g_pkCurrTextureStage->SetObjTextureSettings(
                pkDesc->GetType(), pkDesc->GetIndex());
        }
                
        NiFree((yyvsp[0].sval));             
    ;}
    break;

  case 395:

    {   (yyval.ival) = NiTextureStage::TSTF_NDL_BASE;      ;}
    break;

  case 396:

    {   (yyval.ival) = NiTextureStage::TSTF_NDL_DARK;      ;}
    break;

  case 397:

    {   (yyval.ival) = NiTextureStage::TSTF_NDL_DETAIL;    ;}
    break;

  case 398:

    {   (yyval.ival) = NiTextureStage::TSTF_NDL_GLOSS;     ;}
    break;

  case 399:

    {   (yyval.ival) = NiTextureStage::TSTF_NDL_GLOW;      ;}
    break;

  case 400:

    {   (yyval.ival) = NiTextureStage::TSTF_NDL_BUMP;      ;}
    break;

  case 401:

    {   (yyval.ival) = NiTextureStage::TSTF_NDL_NORMAL;      ;}
    break;

  case 402:

    {   (yyval.ival) = NiTextureStage::TSTF_NDL_PARALLAX;      ;}
    break;

  case 403:

    {
                (yyval.ival) = NiTextureStage::TSTF_MAP_DECAL | (yyvsp[0].ival);
            ;}
    break;

  case 404:

    {
                NiSprintf(g_szDSO, 1024, "ColorOp            = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_COLOROP;
                g_uiCurrStateValue = (yyvsp[0].ival);  
                g_bCurrStateValid = true;  
            ;}
    break;

  case 405:

    {
                NiSprintf(g_szDSO, 1024, "ColorArg0          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_COLORARG0;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 406:

    {
                NiSprintf(g_szDSO, 1024, "ColorArg1          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_COLORARG1;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 407:

    {
                NiSprintf(g_szDSO, 1024, "ColorArg2          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_COLORARG2;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 408:

    {
                NiSprintf(g_szDSO, 1024, "AlphaOp            = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_ALPHAOP;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 409:

    {
                NiSprintf(g_szDSO, 1024, "AlphaArg0          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_ALPHAARG0;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 410:

    {
                NiSprintf(g_szDSO, 1024, "AlphaArg1          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_ALPHAARG1;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 411:

    {
                NiSprintf(g_szDSO, 1024, "AlphaArg2          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_ALPHAARG2;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 412:

    {
                NiSprintf(g_szDSO, 1024, "ResultArg          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_RESULTARG;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 413:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_INVALID;
                g_uiCurrStateValue = (yyvsp[0].dword);
                    
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedTextureStageState (TSS_CONSTANT)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(),
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;  
            ;}
    break;

  case 414:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat00       = %8.5f", (yyvsp[0].fval));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_BUMPENVMAT00;
                g_uiCurrStateValue = F2DW((yyvsp[0].fval));
                g_bCurrStateValid = true;  
            ;}
    break;

  case 415:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat00       = %s", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 416:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat01       = %8.5f", (yyvsp[0].fval));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_BUMPENVMAT01;
                g_uiCurrStateValue = F2DW((yyvsp[0].fval));
                g_bCurrStateValid = true;  
            ;}
    break;

  case 417:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat01       = %s", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 418:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat10       = %8.5f", (yyvsp[0].fval));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_BUMPENVMAT10;
                g_uiCurrStateValue = F2DW((yyvsp[0].fval));
                g_bCurrStateValid = true;  
            ;}
    break;

  case 419:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat10       = %s", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 420:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat11       = %8.5f", (yyvsp[0].fval));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_BUMPENVMAT11;
                g_uiCurrStateValue = F2DW((yyvsp[0].fval));
                g_bCurrStateValid = true;  
            ;}
    break;

  case 421:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvMat11       = %s", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 422:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvLScale      = %8.5f", (yyvsp[0].fval));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_BUMPENVLSCALE;
                g_uiCurrStateValue = F2DW((yyvsp[0].fval));
                g_bCurrStateValid = true;  
            ;}
    break;

  case 423:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvLScale      = %s", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 424:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvLOffset     = %8.5f", (yyvsp[0].fval));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_BUMPENVLOFFSET;
                g_uiCurrStateValue = F2DW((yyvsp[0].fval));
                g_bCurrStateValid = true;  
            ;}
    break;

  case 425:

    {
                NiSprintf(g_szDSO, 1024, "BumpEnvLOffset     = %s", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 426:

    {
                int iValue = (yyvsp[-1].ival) | (yyvsp[0].ival);
                NiSprintf(g_szDSO, 1024, "TexCoordIndex      = 0x%08x", iValue);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_TEXCOORDINDEX;
                g_uiCurrStateValue = iValue;
                g_bCurrStateValid = true;  
            ;}
    break;

  case 427:

    {
                int iValue = (yyvsp[-1].ival);
                NiSprintf(g_szDSO, 1024, "TexCoordIndex      = 0x%08x", iValue);
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_TEXCOORDINDEX;
                g_uiCurrStateValue = iValue;
                g_bCurrStateValid = true;  

                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetUseIndexFromMap(true);
            ;}
    break;

  case 428:

    {
                NiSprintf(g_szDSO, 1024, "TextureTransformFlags= COUNT%d", 
                    (int)(yyvsp[-1].ival));
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_TEXTURETRANSFORMFLAGS;
                g_uiCurrStateValue = (unsigned int)(yyvsp[-1].ival) | 
                    (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;  
                
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
            ;}
    break;

  case 429:

    {
                NiSprintf(g_szDSO, 1024, "TextureTransformFlags= NID3DTTFF_PROJECTED");
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_TEXTURETRANSFORMFLAGS;
                g_uiCurrStateValue = (unsigned int)
                    NSBStageAndSamplerStates::NID3DTTFF_PROJECTED;
                g_bCurrStateValid = true;  
                
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
            ;}
    break;

  case 430:

    {
                NiSprintf(g_szDSO, 1024, "TextureTransformFlags= DISABLE");
                DebugStringOut(g_szDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DTSS_TEXTURETRANSFORMFLAGS;
                g_uiCurrStateValue = 
                    NSBStageAndSamplerStates::NID3DTTFF_DISABLE;
                g_bCurrStateValid = true;  
                
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
            ;}
    break;

  case 431:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTTFF_COUNT1; ;}
    break;

  case 432:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTTFF_COUNT2; ;}
    break;

  case 433:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTTFF_COUNT3; ;}
    break;

  case 434:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTTFF_COUNT4; ;}
    break;

  case 435:

    {   (yyval.ival) = 0; ;}
    break;

  case 436:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTTFF_PROJECTED;  ;}
    break;

  case 437:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_DISABLE;                    
            ;}
    break;

  case 438:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_SELECTARG1;                 
            ;}
    break;

  case 439:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_SELECTARG2;                 
            ;}
    break;

  case 440:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATE;                   
            ;}
    break;

  case 441:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATE2X;                 
            ;}
    break;

  case 442:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATE4X;                 
            ;}
    break;

  case 443:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_ADD;                        
            ;}
    break;

  case 444:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_ADDSIGNED;                  
            ;}
    break;

  case 445:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_ADDSIGNED2X;                
            ;}
    break;

  case 446:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_SUBTRACT;                   
            ;}
    break;

  case 447:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_ADDSMOOTH;                  
            ;}
    break;

  case 448:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_BLENDDIFFUSEALPHA;          
            ;}
    break;

  case 449:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_BLENDTEXTUREALPHA;          
            ;}
    break;

  case 450:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_BLENDFACTORALPHA;           
            ;}
    break;

  case 451:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_BLENDTEXTUREALPHAPM;        
            ;}
    break;

  case 452:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_BLENDCURRENTALPHA;          
            ;}
    break;

  case 453:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_PREMODULATE;                
            ;}
    break;

  case 454:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATEALPHA_ADDCOLOR;     
            ;}
    break;

  case 455:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATECOLOR_ADDALPHA;     
            ;}
    break;

  case 456:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATEINVALPHA_ADDCOLOR;
            ;}
    break;

  case 457:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_MODULATEINVCOLOR_ADDALPHA;  
            ;}
    break;

  case 458:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_BUMPENVMAP;                 
            ;}
    break;

  case 459:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_BUMPENVMAPLUMINANCE;        
            ;}
    break;

  case 460:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_DOTPRODUCT3;                
            ;}
    break;

  case 461:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_MULTIPLYADD;                
            ;}
    break;

  case 462:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTOP_LERP;                       
            ;}
    break;

  case 463:

    {
                NSFParsererror("Syntax Error: stage_texture_operation");
                yyclearin;
            ;}
    break;

  case 464:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_CURRENT | 
                (yyvsp[0].ival); ;}
    break;

  case 465:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_DIFFUSE | 
                (yyvsp[0].ival); ;}
    break;

  case 466:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_SELECTMASK | 
                (yyvsp[0].ival); ;}
    break;

  case 467:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_SPECULAR | 
                (yyvsp[0].ival); ;}
    break;

  case 468:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_TEMP | 
                (yyvsp[0].ival); ;}
    break;

  case 469:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_TEXTURE | 
                (yyvsp[0].ival); ;}
    break;

  case 470:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_TFACTOR | 
                (yyvsp[0].ival); ;}
    break;

  case 471:

    {   (yyval.ival) = 0;     ;}
    break;

  case 472:

    {   (yyval.ival) = (yyvsp[0].ival);    ;}
    break;

  case 473:

    {
                (yyval.ival) = 
                    NSBStageAndSamplerStates::NID3DTA_ALPHAREPLICATE | 
                    NSBStageAndSamplerStates::NID3DTA_COMPLEMENT;
            ;}
    break;

  case 474:

    {
                (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_COMPLEMENT |
                    NSBStageAndSamplerStates::NID3DTA_ALPHAREPLICATE;
            ;}
    break;

  case 475:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_ALPHAREPLICATE;    ;}
    break;

  case 476:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTA_COMPLEMENT;        ;}
    break;

  case 477:

    {   (yyval.ival) = 
                0;
            ;}
    break;

  case 478:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTSI_PASSTHRU;
            ;}
    break;

  case 479:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTSI_CAMERASPACENORMAL;
            ;}
    break;

  case 480:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTSI_CAMERASPACEPOSITION;
            ;}
    break;

  case 481:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTSI_CAMERASPACEREFLECTIONVECTOR;
            ;}
    break;

  case 482:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NID3DTSI_SPHEREMAP;
            ;}
    break;

  case 483:

    {
                NSFParsererror("Syntax Error: stage_texcoordindex_flags");
                yyclearin;
            ;}
    break;

  case 484:

    {
                // The set will be performed in the assignment section!
            ;}
    break;

  case 485:

    {
                unsigned int uiFlags = NiTextureStage::TSTTF_GLOBAL;
                
                uiFlags |= (int)(yyvsp[-1].ival);

                if (g_pkCurrTextureStage)
                {
                    const char* pszGlobalName = (yyvsp[0].sval);
                    g_pkCurrTextureStage->SetTextureTransformFlags(uiFlags);
                    g_pkCurrTextureStage->SetGlobalName(pszGlobalName);
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
                }
                
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 486:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 487:

    {
                unsigned int uiFlags = NiTextureStage::TSTTF_CONSTANT;
        
                uiFlags |= (int)(yyvsp[-1].ival);

#ifndef _PS3
                if (g_pkCurrTextureStage)
                {
                    D3DMATRIX kTrans;
                    
                    kTrans._11 = g_afValues->GetAt( 0);
                    kTrans._12 = g_afValues->GetAt( 1);
                    kTrans._13 = g_afValues->GetAt( 2);
                    kTrans._14 = g_afValues->GetAt( 3);
                    kTrans._21 = g_afValues->GetAt( 4);
                    kTrans._22 = g_afValues->GetAt( 5);
                    kTrans._23 = g_afValues->GetAt( 6);
                    kTrans._24 = g_afValues->GetAt( 7);
                    kTrans._31 = g_afValues->GetAt( 8);
                    kTrans._32 = g_afValues->GetAt( 9);
                    kTrans._33 = g_afValues->GetAt(10);
                    kTrans._34 = g_afValues->GetAt(11);
                    kTrans._41 = g_afValues->GetAt(12);
                    kTrans._42 = g_afValues->GetAt(13);
                    kTrans._43 = g_afValues->GetAt(14);
                    kTrans._44 = g_afValues->GetAt(15);
                    
                    g_pkCurrTextureStage->SetTextureTransformFlags(uiFlags);
                    g_pkCurrTextureStage->SetTextureTransformation(kTrans);
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
                }
#endif
                
            ;}
    break;

  case 488:

    {   (yyval.ival) = NiTextureStage::TSTTF_NI_NO_CALC;           ;}
    break;

  case 489:

    {   (yyval.ival) = NiTextureStage::TSTTF_NI_WORLD_PARALLEL;    ;}
    break;

  case 490:

    {   (yyval.ival) = NiTextureStage::TSTTF_NI_WORLD_PERSPECTIVE; ;}
    break;

  case 491:

    {   (yyval.ival) = NiTextureStage::TSTTF_NI_WORLD_SPHERE_MAP;  ;}
    break;

  case 492:

    {   (yyval.ival) = NiTextureStage::TSTTF_NI_CAMERA_SPHERE_MAP; ;}
    break;

  case 493:

    {   (yyval.ival) = NiTextureStage::TSTTF_NI_SPECULAR_CUBE_MAP; ;}
    break;

  case 494:

    {   (yyval.ival) = NiTextureStage::TSTTF_NI_DIFFUSE_CUBE_MAP;  ;}
    break;

  case 495:

    {
                NSFParsererror("Syntax Error: stage_textransmatrix_option");
                yyclearin;
            ;}
    break;

  case 496:

    {
                NiSprintf(g_szDSO, 1024, "Sampler Start %3d - %s\n", 
                    (yyvsp[-2].ival), (yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrPass)
                {
                    g_pkCurrTextureStage = 
                        g_pkCurrPass->GetStage((int)(yyvsp[-2].ival));
                    g_pkCurrTextureStage->SetName((yyvsp[-1].sval));
                }
            ;}
    break;

  case 497:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Sampler End %3d - %s\n", 
                    (int)(yyvsp[-5].ival), (yyvsp[-4].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrTextureStage = 0;

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 498:

    {
                NiSprintf(g_szDSO, 1024, "Sampler Start - %s\n", (yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrPass)
                {
                    g_pkCurrTextureStage = 
                        g_pkCurrPass->GetStage(g_pkCurrPass->GetStageCount());
                    g_pkCurrTextureStage->SetName((yyvsp[-1].sval));
                }
            ;}
    break;

  case 499:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Sampler End %s\n", (yyvsp[-4].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrTextureStage = 0;

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 504:

    {
                if (g_bCurrStateValid)
                {
                    if (g_pkCurrTextureStage)
                    {
                        NSBStateGroup* pkGroup = 
                            g_pkCurrTextureStage->GetSamplerStageGroup();
                        if (pkGroup)
                        {
                            pkGroup->SetState(g_uiCurrStateState,
                                g_uiCurrStateValue, true, g_bUseMapValue);
                            g_bUseMapValue = false;
                        }
                    }
                    
                    DebugStringOut("    SAVE\n", false);
                }
            ;}
    break;

  case 505:

    {
                if (g_bCurrStateValid)
                {
                    if (g_pkCurrTextureStage)
                    {
                        NSBStateGroup* pkGroup = 
                            g_pkCurrTextureStage->GetSamplerStageGroup();
                        if (pkGroup)
                        {
                            pkGroup->SetState(g_uiCurrStateState, 
                                g_uiCurrStateValue, false, g_bUseMapValue);
                            g_bUseMapValue = false;
                        }
                    }
                    
                    DebugStringOut("\n", false);
                }
            ;}
    break;

  case 506:

    {
                // Do nothing. It's handles in the stage_texture block!
                DebugStringOut("Sampler Texture!\n");
            ;}
    break;

  case 525:

    {
                NSFParsererror("Syntax Error: sampler_entry");
                yyclearin;
            ;}
    break;

  case 526:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_ADDRESSU;
                g_uiCurrStateValue = (yyvsp[0].ival);
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NID3DTADDRESS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_ADDRESSU "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;

                    NiSprintf(g_szDSO, 1024, "AddressU             = 0x%08x", 
                        (int)(yyvsp[0].ival));
                    DebugStringOut(g_szDSO);
                }                
            ;}
    break;

  case 527:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_ADDRESSV;
                g_uiCurrStateValue = (yyvsp[0].ival);
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NID3DTADDRESS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_ADDRESSV "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;

                    NiSprintf(g_szDSO, 1024, "AddressV             = 0x%08x", 
                        (int)(yyvsp[0].ival));
                    DebugStringOut(g_szDSO);
                }
            ;}
    break;

  case 528:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_ADDRESSW;
                g_uiCurrStateValue = (yyvsp[0].ival);
                if (g_bUseMapValue)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false, "* PARSE ERROR: %s\n"
                        "    UseMapValue not allowed for TSAMP_ADDRESSW "
                        "SamplerState\n"
                        "    at line %d\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NID3DTADDRESS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_ADDRESSW "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;

                    NiSprintf(g_szDSO, 1024, "AddressW             = 0x%08x", 
                        (int)(yyvsp[0].ival));
                    DebugStringOut(g_szDSO);
                }                
            ;}
    break;

  case 529:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_BORDERCOLOR;
                g_uiCurrStateValue = (yyvsp[0].dword);
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "BorderColor         = 0x%08x", 
                    (int)(yyvsp[0].dword));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 530:

    {
                NiSprintf(g_szDSO, 1024, "BorderColor         = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 531:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_MAGFILTER;
                g_uiCurrStateValue = (yyvsp[0].ival);
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NID3DTEXF_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_MAGFILTER "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;

                    NiSprintf(g_szDSO, 1024, "MagFilter          = 0x%08x", 
                        (int)(yyvsp[0].ival));
                    DebugStringOut(g_szDSO);
                }
            ;}
    break;

  case 532:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_MINFILTER;
                g_uiCurrStateValue = (yyvsp[0].ival);
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NID3DTEXF_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_MINFILTER "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;
                    
                    NiSprintf(g_szDSO, 1024, "MinFilter          = 0x%08x", 
                        (int)(yyvsp[0].ival));
                    DebugStringOut(g_szDSO);
                }
            ;}
    break;

  case 533:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_MIPFILTER;
                g_uiCurrStateValue = (yyvsp[0].ival);
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NID3DTEXF_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_MIPFILTER "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;
                    
                    NiSprintf(g_szDSO, 1024, "MipFilter          = 0x%08x", 
                        (int)(yyvsp[0].ival));
                    DebugStringOut(g_szDSO);
                }
            ;}
    break;

  case 534:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_MIPMAPLODBIAS;
                g_uiCurrStateValue = (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "MipMapLODBias      = %d", 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 535:

    {
                NiSprintf(g_szDSO, 1024, "MipMapLODBias      = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 536:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_MAXMIPLEVEL;
                g_uiCurrStateValue = (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "MaxMipLevel        = %d", 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 537:

    {
                NiSprintf(g_szDSO, 1024, "MaxMipLevel        = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 538:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_MAXANISOTROPY;
                g_uiCurrStateValue = (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "MaxAnisotropy      = %d", 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 539:

    {
                NiSprintf(g_szDSO, 1024, "MaxAnisotropy      = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 540:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_SRGBTEXTURE;
                g_uiCurrStateValue = (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "SRGBTexture        = %d", 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 541:

    {
                NiSprintf(g_szDSO, 1024, "SRGBTexture        = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 542:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_ELEMENTINDEX;
                g_uiCurrStateValue = (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "ElementIndex       = %d", 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 543:

    {
                NiSprintf(g_szDSO, 1024, "ElementIndex       = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 544:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NID3DSAMP_DMAPOFFSET;
                g_uiCurrStateValue = (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;
                
                NiSprintf(g_szDSO, 1024, "DMapOffset         = %d", 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_szDSO);
            ;}
    break;

  case 545:

    {
                NiSprintf(g_szDSO, 1024, "DMapOffset         = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_szDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 546:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_ALPHAKILL)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;
            ;}
    break;

  case 547:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_ALPHAKILL)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 548:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORKEYOP)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;
            ;}
    break;

  case 549:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORSIGN)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 550:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORSIGN)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;
            ;}
    break;

  case 551:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORKEYCOLOR)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;
            ;}
    break;

  case 552:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORKEYCOLOR)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 553:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTADDRESS_WRAP;          ;}
    break;

  case 554:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTADDRESS_MIRROR;        ;}
    break;

  case 555:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTADDRESS_CLAMP;         ;}
    break;

  case 556:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTADDRESS_BORDER;        ;}
    break;

  case 557:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTADDRESS_MIRRORONCE;    ;}
    break;

  case 558:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTADDRESS_INVALID;       ;}
    break;

  case 559:

    {
                g_bUseMapValue = true;
                (yyval.ival) = NSBStageAndSamplerStates::NID3DTADDRESS_INVALID;
            ;}
    break;

  case 560:

    {
                NSFParsererror("Syntax Error: sampler_texture_address");
                yyclearin;
            ;}
    break;

  case 561:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_NONE;              ;}
    break;

  case 562:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_POINT;             ;}
    break;

  case 563:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_LINEAR;            ;}
    break;

  case 564:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_ANISOTROPIC;       ;}
    break;

  case 565:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_PYRAMIDALQUAD;     ;}
    break;

  case 566:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_GAUSSIANQUAD;      ;}
    break;

  case 567:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_INVALID;           ;}
    break;

  case 568:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_INVALID;           ;}
    break;

  case 569:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_INVALID;           ;}
    break;

  case 570:

    {   (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_INVALID;           ;}
    break;

  case 571:

    {
                g_bUseMapValue = true;
                (yyval.ival) = NSBStageAndSamplerStates::NID3DTEXF_INVALID;
            ;}
    break;

  case 572:

    {
                NSFParsererror("Syntax Error: sampler_texture_filter");
                yyclearin;
            ;}
    break;

  case 573:

    {   (yyval.ival) = 0x7fffffff;    ;}
    break;

  case 574:

    {   (yyval.ival) = 0x7fffffff;    ;}
    break;

  case 575:

    {
                NSFParsererror("Syntax Error: sampler_texture_alphakill");
                yyclearin;
            ;}
    break;

  case 576:

    {   (yyval.ival) = 0x7fffffff;    ;}
    break;

  case 577:

    {   (yyval.ival) = 0x7fffffff;    ;}
    break;

  case 578:

    {   (yyval.ival) = 0x7fffffff;    ;}
    break;

  case 579:

    {   (yyval.ival) = 0x7fffffff;    ;}
    break;

  case 580:

    {
                NSFParsererror("Syntax Error: sampler_texture_colorkeyop");
                yyclearin;
            ;}
    break;

  case 581:

    {
                NiSprintf(g_szDSO, 1024, "Texture Start %3d - %s\n", 
                    (yyvsp[-2].ival), (yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrPass)
                {
                    g_pkCurrTexture = g_pkCurrPass->GetTexture((int)(yyvsp[-2].ival));
                    g_pkCurrTexture->SetName((yyvsp[-1].sval));
                }
            ;}
    break;

  case 582:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Texture End %3d - %s\n", 
                    (int)(yyvsp[-5].ival), (yyvsp[-4].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrTexture = 0;

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 583:

    {
                NiSprintf(g_szDSO, 1024, "Texture Start - %s\n", (yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrPass)
                {
                    g_pkCurrTexture = 
                        g_pkCurrPass->GetTexture(g_pkCurrPass->GetTextureCount());
                    g_pkCurrTexture->SetName((yyvsp[-1].sval));
                }
            ;}
    break;

  case 584:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Texture End %s\n", (yyvsp[-4].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrTexture = 0;

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 590:

    {
                NSFParsererror("Syntax Error: texture_entry");
                yyclearin;
            ;}
    break;

  case 591:

    {
                if (g_pkCurrTexture)
                    g_pkCurrTexture->SetSource((yyvsp[0].ival));
            ;}
    break;

  case 592:

    {
                if (g_pkCurrTexture)
                    g_pkCurrTexture->SetSourceDecal((yyvsp[0].ival));
            ;}
    break;

  case 593:

    {
                bool bFoundAttribute = false;
                NSBObjectTable* pkObjectTable = 0;
                if (g_pkCurrNSBShader)
                {
                    g_bGlobalAttributes = false;
                    g_pkCurrAttribTable = 
                        g_pkCurrNSBShader->GetAttributeTable();
                    pkObjectTable = g_pkCurrNSBShader->GetObjectTable();
                }
                if (g_pkCurrAttribTable)
                {
                    NSBAttributeDesc* pkAttrib = 
                        g_pkCurrAttribTable->GetAttributeByName((yyvsp[0].sval));
                    if (pkAttrib)
                    {
                        bFoundAttribute = true;
                        
                        unsigned int uiValue;
                        const char* pszValue;
                        
                        if (pkAttrib->GetValue_Texture(uiValue, pszValue))
                        {
                            uiValue |= NiTextureStage::TSTF_MAP_SHADER;
                            if (g_pkCurrTexture)
                                g_pkCurrTexture->SetSourceShader(uiValue);
                        }
                        else
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, true,
                                "* PARSE ERROR: %s\n"
                                "    GetValue_Texture at line %d\n"
                                "    Attribute name = %s\n",
                                g_pkFile->GetFilename(),
                                NSFParserGetLineNumber(),
                                (yyvsp[0].sval));
                        }
                    }
                    g_pkCurrAttribTable = 0;
                }
                
                if (!bFoundAttribute && pkObjectTable)
                {
                    NSBObjectTable::ObjectDesc* pkDesc =
                        pkObjectTable->GetObjectByName((yyvsp[0].sval));
                    if (pkDesc)
                    {
                        NiShaderAttributeDesc::ObjectType eObjectType =
                            pkDesc->GetType();
                        if (eObjectType <
                            NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP ||
                            eObjectType >
                            NiShaderAttributeDesc::OT_EFFECT_FOGMAP)
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, true,
                                "* PARSE ERROR: %s\n"
                                "    InvalidObjectType at line %d\n"
                                "    Object name = %s\n",
                                g_pkFile->GetFilename(),
                                NSFParserGetLineNumber(),
                                (yyvsp[0].sval));
                        }
                        else
                        {
                            if (g_pkCurrTexture)
                            {
                                g_pkCurrTexture->SetSourceObject(
                                    eObjectType, pkDesc->GetIndex());
                            }
                        }
                    }
                    else
                    {
                        NiShaderFactory::ReportError(
                            NISHADERERR_UNKNOWN, true,
                            "* PARSE ERROR: %s\n"
                            "    TextureNotFound at line %d\n"
                            "    Attribute/Object name = %s\n",
                            g_pkFile->GetFilename(),
                            NSFParserGetLineNumber(),
                            (yyvsp[0].sval));
                    }
                }
                
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 594:

    {           
                NSBObjectTable::ObjectDesc* pkDesc = NULL;
                NSBObjectTable* pkTable = g_pkCurrNSBShader->GetObjectTable();
                if (pkTable)
                {
                    pkDesc = pkTable->GetObjectByName((yyvsp[0].sval));
                }
                if (!pkDesc)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    GetObjectByName at line %d\n"
                        "    Local name = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        (yyvsp[0].sval));
                    NiFree((yyvsp[0].sval));                             
                    break;
                }
                if (g_pkCurrTexture)
                {
                    g_pkCurrTexture->SetSourceObject(
                        pkDesc->GetType(), pkDesc->GetIndex());
                }
                        
                NiFree((yyvsp[0].sval));             
            ;}
    break;

  case 607:

    {
                NiSprintf(g_szDSO, 1024, "Pass Start %s\n", (yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                
                if (g_pkCurrImplementation)
                {
                    g_pkCurrPass = 
                        g_pkCurrImplementation->GetPass(g_uiCurrPassIndex);
                }
            ;}
    break;

  case 608:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Pass End %s\n", (yyvsp[-4].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrPass = 0;
                g_uiCurrPassVSConstantMap = 0;
                g_uiCurrPassGSConstantMap = 0;
                g_uiCurrPassPSConstantMap = 0;
                g_uiCurrPassIndex++;

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 623:

    {
                NiSprintf(g_szDSO, 1024, "Implementation Start %s\n", (yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrNSBShader)
                {
                    g_pkCurrImplementation = 
                        g_pkCurrNSBShader->GetImplementation((yyvsp[-1].sval), 
                            true, g_uiCurrImplementation);
                    if (g_pkCurrImplementation->GetIndex() == 
                        g_uiCurrImplementation)
                    {
                        g_uiCurrImplementation++;
                    }
                    g_uiCurrPassIndex = 0;
                }                    
            ;}
    break;

  case 624:

    {
                    NiSprintf(g_szDSO, 1024, "Description: %s\n", (yyvsp[0].sval));
                    DebugStringOut(g_szDSO);
                    
                    if (g_pkCurrImplementation)
                        g_pkCurrImplementation->SetDesc((yyvsp[0].sval));
                    NiFree((yyvsp[0].sval));
                ;}
    break;

  case 625:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "Implementation End %s\n", (yyvsp[-6].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrImplementation = 0;
                g_uiCurrImplemVSConstantMap = 0;
                g_uiCurrImplemGSConstantMap = 0;
                g_uiCurrImplemPSConstantMap = 0;
                
                NiFree((yyvsp[-6].sval));
            ;}
    break;

  case 626:

    {
            NIASSERT(g_pkCurrPass);
        ;}
    break;

  case 632:

    {
                NIASSERT(g_pkCurrPass);
                NiStreamOutSettings& kStreamOutSettings = 
                    g_pkCurrPass->GetStreamOutSettings();
                kStreamOutSettings.SetStreamOutAppend((yyvsp[0].bval));
            ;}
    break;

  case 636:

    {
                NIASSERT(g_pkCurrPass);
                NiStreamOutSettings& kStreamOutSettings = 
                    g_pkCurrPass->GetStreamOutSettings();
                kStreamOutSettings.AppendStreamOutTargets((yyvsp[0].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 637:

    {
                // Create a new OSD and set its name
                g_pkCurrentOutputStreamDescriptor = 
                NiNew NiOutputStreamDescriptor;
                g_pkCurrentOutputStreamDescriptor->SetName((yyvsp[-1].sval));
                
                NiFree((yyvsp[-1].sval));
            ;}
    break;

  case 638:

    {
                // Add the new OSD
                g_pkCurrNSBShader->AddOutputStreamDescriptor(
                    *g_pkCurrentOutputStreamDescriptor);
            
                NiDelete g_pkCurrentOutputStreamDescriptor;
                g_pkCurrentOutputStreamDescriptor = NULL;
            ;}
    break;

  case 644:

    {
                // note: 0 = same # of verts as in source mesh.
                g_pkCurrentOutputStreamDescriptor->SetMaxVertexCount((yyvsp[0].ival));
            ;}
    break;

  case 645:

    {
                g_eDataType = NiOutputStreamDescriptor::DATATYPE_MAX;
            ;}
    break;

  case 646:

    {
                g_eDataType = NiOutputStreamDescriptor::DATATYPE_MAX;
            ;}
    break;

  case 649:

    {
                NiSprintf(g_szDSO, 1024, "vertex format entry [%d elements "
                    "using %s,%d]\n", (yyvsp[-4].ival), (yyvsp[-3].sval), (yyvsp[-1].ival));
                DebugStringOut(g_szDSO);

                NiOutputStreamDescriptor::VertexFormatEntry kEntry;
                
                NIASSERT(
                    g_eDataType != NiOutputStreamDescriptor::DATATYPE_MAX);
                kEntry.m_eDataType = g_eDataType;
                kEntry.m_uiComponentCount = ((yyvsp[-4].ival));
                kEntry.m_kSemanticName = ((yyvsp[-3].sval));
                kEntry.m_uiSemanticIndex = ((yyvsp[-1].ival));
                g_pkCurrentOutputStreamDescriptor->AppendVertexFormat(kEntry);

                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 653:

    {
                g_eDataType = NiOutputStreamDescriptor::DATATYPE_FLOAT;
            ;}
    break;

  case 654:

    {
                g_eDataType = NiOutputStreamDescriptor::DATATYPE_INT;
            ;}
    break;

  case 655:

    {
                g_eDataType = NiOutputStreamDescriptor::DATATYPE_UINT;
            ;}
    break;

  case 660:

    {
                g_pkCurrentOutputStreamDescriptor->SetPrimType(
                    NiPrimitiveType::PRIMITIVE_POINTS);
            ;}
    break;

  case 661:

    {
                g_pkCurrentOutputStreamDescriptor->SetPrimType(
                    NiPrimitiveType::PRIMITIVE_LINES);
            ;}
    break;

  case 662:

    {
                g_pkCurrentOutputStreamDescriptor->SetPrimType(
                    NiPrimitiveType::PRIMITIVE_TRIANGLES);
            ;}
    break;

  case 663:

    {
                NiSprintf(g_szDSO, 1024, "Integer............%s\n", (yyvsp[-2].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                NiSprintf(g_szDSO, 1024, "%d\n", (yyvsp[0].ival));
                DebugStringOut(g_szDSO);
                g_iDSOIndent -= 4;
                
                if (g_pkCurrUDDataBlock)
                {
                    unsigned int uiValue = (unsigned int)(yyvsp[0].ival);
                    unsigned int uiFlags = 
                        NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE | 
                        NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
                    if (!g_pkCurrUDDataBlock->AddEntry((yyvsp[-2].sval), 
                        uiFlags, sizeof(unsigned int), sizeof(unsigned int), 
                        (void*)&uiValue, true))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Failed to add!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].sval));
                    }
                }                

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 664:

    {
                NiSprintf(g_szDSO, 1024, "Boolean............%s\n", (yyvsp[-2].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                NiSprintf(g_szDSO, 1024, "%s\n", (yyvsp[0].bval) ? "TRUE" : "FALSE");
                DebugStringOut(g_szDSO);
                g_iDSOIndent -= 4;

                if (g_pkCurrUDDataBlock)
                {
                    bool bValue = (yyvsp[0].bval) ? true : false;
                    unsigned int uiFlags = 
                        NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE | 
                        NiShaderAttributeDesc::ATTRIB_TYPE_BOOL;
                    if (!g_pkCurrUDDataBlock->AddEntry((yyvsp[-2].sval), 
                        uiFlags, sizeof(bool), sizeof(bool), (void*)&bValue, 
                        true))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Failed to add!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].sval));
                    }
                }                

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 665:

    {
                NiSprintf(g_szDSO, 1024, "Floats (%2d)........%s\n", 
                    g_afValues->GetSize(), (yyvsp[-2].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                for (unsigned int ui = 0; ui < g_afValues->GetSize(); ui++)
                {
                    if ((ui % 4) == 0)
                        DebugStringOut("");
                    NiSprintf(g_szDSO, 1024, "%-8.5f", g_afValues->GetAt(ui));
                    if ((((ui + 1) % 4) == 0) ||
                        (ui + 1 == g_afValues->GetSize()))
                    {
                        NiStrcat(g_szDSO, 1024, "\n");
                    }
                    DebugStringOut(g_szDSO, false);
                }
                g_iDSOIndent -= 4;
                
                if (g_pkCurrUDDataBlock)
                {
                    unsigned int uiFlags = 0;
                    
                    switch (g_afValues->GetSize())
                    {
                    case 1:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
                        break;
                    case 2:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
                        break;
                    case 3:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
                        break;
                    case 4:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
                        break;
                    case 8:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8;
                        break;
                    case 9:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
                        break;
                    case 12:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12;
                        break;
                    case 16:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
                        break;
                    default:
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            false, "* %s(%d): Unsupported or unexpected "
                            "attribute size\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                        break;
                    }
                    uiFlags |= 
                        NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE;
                    
                    if (uiFlags == 0)
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Invalid number of floats!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].sval));
                    }
                    else if (!g_pkCurrUDDataBlock->AddEntry((yyvsp[-2].sval), 
                        uiFlags, sizeof(float) * g_afValues->GetSize(), 
                        sizeof(float), (void*)g_afValues->GetBase(), true))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Failed to add!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].sval));
                    }
                }                

                // Reset the float arrays, so any entries that follow this
                // one will be handled correctly
                ResetFloatValueArray();
                ResetFloatRangeArrays();

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 666:

    {
                NiSprintf(g_szDSO, 1024, "String.............%s\n", (yyvsp[-2].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;
                NiSprintf(g_szDSO, 1024, "%s\n", (yyvsp[0].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent -= 4;
                
                if (g_pkCurrUDDataBlock)
                {
                    unsigned int uiFlags = 
                        NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE | 
                        NiShaderAttributeDesc::ATTRIB_TYPE_STRING;
                    if (!g_pkCurrUDDataBlock->AddEntry((yyvsp[-2].sval), 
                        uiFlags, sizeof(char) * strlen((yyvsp[0].sval)), 
                        sizeof(char) * strlen((yyvsp[0].sval)), 
                        (void*)(yyvsp[0].sval), true))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Failed to add!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].sval));
                    }
                }                

                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 669:

    {
                NiSprintf(g_szDSO, 1024, "UserDefinedDataBlock Start %s\n", 
                    (yyvsp[-1].sval));
                DebugStringOut(g_szDSO);
                g_iDSOIndent += 4;

                // Reset the float arrays, then each one will reset them
                // when they are done being processed
                ResetFloatValueArray();
                ResetFloatRangeArrays();

                if (g_pkCurrPass)
                {
                    g_pkCurrUDDataSet = 
                        g_pkCurrPass->GetUserDefinedDataSet();
                }
                else
                if (g_pkCurrImplementation)
                {
                    g_pkCurrUDDataSet = 
                        g_pkCurrImplementation->GetUserDefinedDataSet();
                }
                else
                if (g_pkCurrNSBShader)
                {
                    g_pkCurrUDDataSet = 
                        g_pkCurrNSBShader->GetUserDefinedDataSet();
                }

                if (!g_pkCurrUDDataSet)
                {
                    g_pkCurrUDDataSet = NiNew NSBUserDefinedDataSet();
                }

                if (!g_pkCurrUDDataSet)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* ALLOCATION ERROR: %s\n"
                        "    UserDefinedDataSet creation at line %d\n", 
                        g_pkFile->GetFilename(), 
                        NSFParserGetLineNumber());
                }
                else
                {
                    if (g_pkCurrPass)
                    {
                        g_pkCurrPass->SetUserDefinedDataSet(
                            g_pkCurrUDDataSet);
                    }
                    else
                    if (g_pkCurrImplementation)
                    {
                        g_pkCurrImplementation->SetUserDefinedDataSet(
                            g_pkCurrUDDataSet);
                    }
                    else
                    if (g_pkCurrNSBShader)
                    {
                        g_pkCurrNSBShader->SetUserDefinedDataSet(
                            g_pkCurrUDDataSet);
                    }
                }
                
                if (g_pkCurrUDDataSet)
                {
                    g_pkCurrUDDataBlock = 
                        g_pkCurrUDDataSet->GetBlock((yyvsp[-1].sval), false);
                    if (g_pkCurrUDDataBlock)
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedDataBlock at line %d\n"
                            "    Name = %s\n"
                            "    ALREADY EXISTS!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-1].sval));
                    }
                    else
                    {
                        g_pkCurrUDDataBlock = 
                            g_pkCurrUDDataSet->GetBlock((yyvsp[-1].sval), true);
                    }
                    NIASSERT(g_pkCurrUDDataBlock);
                }
            ;}
    break;

  case 670:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_szDSO, 1024, "UserDefinedDataBlock End %s\n", 
                    (yyvsp[-4].sval));
                DebugStringOut(g_szDSO);

                g_pkCurrUDDataSet = 0;
                g_pkCurrUDDataBlock = 0;

                NiFree((yyvsp[-4].sval));
            ;}
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */


  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  int yytype = YYTRANSLATE (yychar);
	  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
	  YYSIZE_T yysize = yysize0;
	  YYSIZE_T yysize1;
	  int yysize_overflow = 0;
	  char *yymsg = 0;
#	  define YYERROR_VERBOSE_ARGS_MAXIMUM 5
	  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int yyx;

#if 0
	  /* This is so xgettext sees the translatable formats that are
	     constructed on the fly.  */
	  YY_("syntax error, unexpected %s");
	  YY_("syntax error, unexpected %s, expecting %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
#endif
	  char *yyfmt;
	  char const *yyf;
	  static char const yyunexpected[] = "syntax error, unexpected %s";
	  static char const yyexpecting[] = ", expecting %s";
	  static char const yyor[] = " or %s";
	  char yyformat[sizeof yyunexpected
			+ sizeof yyexpecting - 1
			+ ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
			   * (sizeof yyor - 1))];
	  char const *yyprefix = yyexpecting;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 1;

	  yyarg[0] = yytname[yytype];
	  yyfmt = yystpcpy (yyformat, yyunexpected);

	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
		  {
		    yycount = 1;
		    yysize = yysize0;
		    yyformat[sizeof yyunexpected - 1] = '\0';
		    break;
		  }
		yyarg[yycount++] = yytname[yyx];
		yysize1 = yysize + yytnamerr (0, yytname[yyx]);
		yysize_overflow |= yysize1 < yysize;
		yysize = yysize1;
		yyfmt = yystpcpy (yyfmt, yyprefix);
		yyprefix = yyor;
	      }

	  yyf = YY_(yyformat);
	  yysize1 = yysize + yystrlen (yyf);
	  yysize_overflow |= yysize1 < yysize;
	  yysize = yysize1;

	  if (!yysize_overflow && yysize <= YYSTACK_ALLOC_MAXIMUM)
	    yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg)
	    {
	      /* Avoid sprintf, as that infringes on the user's name space.
		 Don't have undefined behavior even if the translation
		 produced a string with the wrong number of "%s"s.  */
	      char *yyp = yymsg;
	      int yyi = 0;
	      while ((*yyp = *yyf))
		{
		  if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		    {
		      yyp += yytnamerr (yyp, yyarg[yyi++]);
		      yyf += 2;
		    }
		  else
		    {
		      yyp++;
		      yyf++;
		    }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    {
	      yyerror (YY_("syntax error"));
	      goto yyexhaustedlab;
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (YY_("syntax error"));
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
        }
      else
	{
	  yydestruct ("Error: discarding", yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (0)
     goto yyerrorlab;

yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping", yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK;
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}





//---------------------------------------------------------------------------
int iErrors = 0;
extern void NSFParserResetLineNumber();
extern void NSFParserReset(); 
extern void NSFParserReleaseBuffer();
//---------------------------------------------------------------------------
int ParseShader(const char* pszFileName)
{ 
    NSFParserResetLineNumber();
    g_pkFile = NiNew NSFTextFile();

    NSFParserlval.fval = 0.0f;
    NSFParserlval.ival = 0;
    NSFParserlval.sval = 0;
    NSFParserlval.dword = 0;
    NSFParserlval.word = 0;
    NSFParserlval.byte = 0;
    NSFParserlval.bval = false;

    if (!pszFileName)
    {
        NiDelete g_pkFile;
        NiSprintf(g_szDSO, 1024, "err: need input file\n");
        DebugStringOut(g_szDSO);
        return -1;
    };

    NSFParsedShader* pkParsedShader;
    NiTListIterator    pos = g_kParsedShaderList.GetHeadPos();
    while (pos)
    {
        pkParsedShader = g_kParsedShaderList.GetNext(pos);
        NiDelete pkParsedShader;
    }
    g_kParsedShaderList.RemoveAll();
    g_pkCurrShader = 0;

    if (g_pkFile->Load(pszFileName) != 0)
    {
        NiDelete g_pkFile;
        NiSprintf(g_szDSO, 1024, "err: file not found!\n");
        DebugStringOut(g_szDSO);
        return -1;
    }

    // create array
    g_afValues = NiNew NiTPrimitiveArray<float>;

    iErrors = -1;
    yyparse ();
    if (iErrors != -1)
    {
        iErrors++;
        printf ( "*FAILURE! %d errors found.\n" , iErrors);
        NiSprintf(g_szDSO, 1024, "*FAILURE! %d errors found.\n" , iErrors);
        DebugStringOut(g_szDSO);

        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
            false, "* PARSE ERROR: %s\n"
            "    FAILED - %d errors found\n",
            g_pkFile->GetFilename(), iErrors);
    }
        
    NiDelete g_afValues;
    NiDelete g_pkFile;
    g_pkFile = 0;

    if (iErrors == -1)    
        return 0;

    return iErrors;
}
//---------------------------------------------------------------------------
void ResetParser()
{
    NSFParserReset();
} 
//---------------------------------------------------------------------------
void CleanupParser()
{
    NSFParserReleaseBuffer();
}
//---------------------------------------------------------------------------
void DebugStringOut(const char* pszOut, bool bIndent)
{
    NI_UNUSED_ARG(bIndent);
    NI_UNUSED_ARG(pszOut);
#if defined(_ENABLE_DEBUG_STRING_OUT_)
    if (g_bFirstDSOFileAccess)
    {
        g_pfDSOFile = fopen("NSFShaderParser.out", "wt");
        g_bFirstDSOFileAccess = false;
    }
    else
    {
        g_pfDSOFile = fopen("NSFShaderParser.out", "at");
    }

    if (bIndent)
    {
        for (int ii = 0; ii < g_iDSOIndent; ii++)
        {
            printf(" ");
            NiOutputDebugString(" ");
            if (g_pfDSOFile)
                fprintf(g_pfDSOFile, " ");
        }
    }
    printf(pszOut);
    NiOutputDebugString(pszOut);
    if (g_pfDSOFile)
    {
        fprintf(g_pfDSOFile, pszOut);
        fclose(g_pfDSOFile);
    }
#endif    //#if defined(_ENABLE_DEBUG_STRING_OUT_)
}
//---------------------------------------------------------------------------
unsigned int ResetFloatValueArray(void)
{
    g_afValues->RemoveAll();
    return g_afValues->GetSize();
}
//---------------------------------------------------------------------------
unsigned int AddFloatToValueArray(float fValue)
{
    g_afValues->Add(fValue);
    return g_afValues->GetSize();
}
//---------------------------------------------------------------------------
void ResetFloatRangeArrays(void)
{
    g_uiLowFloatValues    = 0;
    g_uiHighFloatValues    = 0;

    g_afLowValues[0] = 0.0f;
    g_afLowValues[1] = 0.0f;
    g_afLowValues[2] = 0.0f;
    g_afLowValues[3] = 0.0f;
    g_afHighValues[0] = 0.0f;
    g_afHighValues[1] = 0.0f;
    g_afHighValues[2] = 0.0f;
    g_afHighValues[3] = 0.0f;
}
//---------------------------------------------------------------------------
unsigned int AddFloatToLowArray(float fValue)
{
    if ((g_uiLowFloatValues + 1) == FLOAT_ARRAY_SIZE)
    {
        DebugStringOut("Low Float array overflow!");
        return 0;
    }
    g_afLowValues[g_uiLowFloatValues++] = fValue;
    return g_uiLowFloatValues;
}
//---------------------------------------------------------------------------
unsigned int AddFloatToHighArray(float fValue)
{
    if ((g_uiHighFloatValues + 1) == FLOAT_ARRAY_SIZE)
    {
        DebugStringOut("High Float array overfHigh!");
        return 0;
    }
    g_afHighValues[g_uiHighFloatValues++] = fValue;
    return g_uiHighFloatValues;
}
//---------------------------------------------------------------------------
void AddObjectToObjectTable(NiShaderAttributeDesc::ObjectType eType,
    unsigned int uiIndex, const char* pcName, const char* pcDebugString)
{
    if (g_pkCurrObjectTable)
    {
        if (!g_pkCurrObjectTable->AddObject(pcName, eType, uiIndex))
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                true, "* PARSE ERROR: %s\n"
                "    AddObject at line %d\n"
                "    Object name = %s\n",
                g_pkFile->GetFilename(),
                NSFParserGetLineNumber(), pcName);
        }
    }
    NiSprintf(g_szDSO, 1024, "    %24s: %d - %16s\n", pcDebugString, uiIndex,
        pcName);
    DebugStringOut(g_szDSO);
}
//---------------------------------------------------------------------------
unsigned int DecodeAttribTypeString(char* pszAttribType)
{
    if (!pszAttribType || pszAttribType[0] == '\0')
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
        
    // We need to look up the attribute in the attribute map, and then
    // return the type
    if (!g_pkCurrNSBShader)
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;

    // Local attributes override global!
    g_pkCurrAttribTable = g_pkCurrNSBShader->GetAttributeTable();
    if (!g_pkCurrAttribTable)
    {
        NIASSERT(!"Invalid attribute table during parse!");
        return 0;
    }
    
    NSBAttributeDesc* pkAttribDesc = 
        g_pkCurrAttribTable->GetAttributeByName(pszAttribType);
    if (!pkAttribDesc)
    {
        g_pkCurrAttribTable = g_pkCurrNSBShader->GetGlobalAttributeTable();
        if (!g_pkCurrAttribTable)
        {
            NIASSERT(!"Invalid attribute table during parse!");
            return 0;
        }
        pkAttribDesc = g_pkCurrAttribTable->GetAttributeByName(pszAttribType);
        if (!pkAttribDesc)
        {
            NIASSERT(!"Attribute not found!");
            return 0;
        }
    }
    
    g_pkCurrAttribTable = 0;
    
    return (unsigned int)(pkAttribDesc->GetType());
}
//---------------------------------------------------------------------------
unsigned int DecodePlatformString(char* pszPlatform)
{
    if (!pszPlatform || pszPlatform[0] == '\0')
        return 0;
        
    char acTemp[64];

    unsigned int ui = 0;
    for (; ui < strlen(pszPlatform); ui++)
        acTemp[ui] = toupper(pszPlatform[ui]);
    acTemp[ui] = 0;
    
    if (NiStricmp(acTemp, "DX9") == 0)
        return NiShader::NISHADER_DX9;
    if (NiStricmp(acTemp, "XENON") == 0)
        return NiShader::NISHADER_XENON;
    if (NiStricmp(acTemp, "PS3") == 0)
        return NiShader::NISHADER_PS3;
    if (NiStricmp(acTemp, "D3D10") == 0)
        return NiShader::NISHADER_D3D10;

    return 0;
}
//---------------------------------------------------------------------------
void SetShaderProgramFile(NSBPass* pkPass, const char* pcFile,
    unsigned int uiPlatforms, NiGPUProgram::ProgramType eType)
{
    if (!pkPass)
        return;

    if (uiPlatforms & NiShader::NISHADER_DX9)
    {
        pkPass->SetShaderProgramFile(pcFile,
            NiSystemDesc::RENDERER_DX9, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_D3D10)
    {
        pkPass->SetShaderProgramFile(pcFile,
            NiSystemDesc::RENDERER_D3D10, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_PS3)
    {
        pkPass->SetShaderProgramFile(pcFile,
            NiSystemDesc::RENDERER_PS3, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_XENON)
    {
        pkPass->SetShaderProgramFile(pcFile,
            NiSystemDesc::RENDERER_XENON, eType);
    }
}
//---------------------------------------------------------------------------
void SetShaderProgramEntryPoint(NSBPass* pkPass, const char* pcEntryPoint,
    unsigned int uiPlatforms, NiGPUProgram::ProgramType eType)
{
    if (!pkPass)
        return;

    if (uiPlatforms & NiShader::NISHADER_DX9)
    {
        pkPass->SetShaderProgramEntryPoint(pcEntryPoint,
            NiSystemDesc::RENDERER_DX9, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_D3D10)
    {
        pkPass->SetShaderProgramEntryPoint(pcEntryPoint,
            NiSystemDesc::RENDERER_D3D10, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_PS3)
    {
        pkPass->SetShaderProgramEntryPoint(pcEntryPoint,
            NiSystemDesc::RENDERER_PS3, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_XENON)
    {
        pkPass->SetShaderProgramEntryPoint(pcEntryPoint,
            NiSystemDesc::RENDERER_XENON, eType);
    }
}
//---------------------------------------------------------------------------
void SetShaderProgramShaderTarget(NSBPass* pkPass,
    const char* pcShaderTarget, unsigned int uiPlatforms,
    NiGPUProgram::ProgramType eType)
{
    if (!pkPass)
        return;

    if (uiPlatforms & NiShader::NISHADER_DX9)
    {
        pkPass->SetShaderProgramShaderTarget(pcShaderTarget,
            NiSystemDesc::RENDERER_DX9, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_D3D10)
    {
        pkPass->SetShaderProgramShaderTarget(pcShaderTarget,
            NiSystemDesc::RENDERER_D3D10, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_PS3)
    {
        pkPass->SetShaderProgramShaderTarget(pcShaderTarget,
            NiSystemDesc::RENDERER_PS3, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_XENON)
    {
        pkPass->SetShaderProgramShaderTarget(pcShaderTarget,
            NiSystemDesc::RENDERER_XENON, eType);
    }
}
//---------------------------------------------------------------------------
bool AddAttributeToConstantMap(char* pszName, 
    unsigned int uiRegisterStart, unsigned int uiRegisterCount, 
    unsigned int uiExtraNumber, bool bIsGlobal)
{
    if (!g_pkCurrConstantMap)
        return false;
    
    // Cheat to force a copy of the data
    unsigned int uiFlags = bIsGlobal
        ? NiShaderConstantMapEntry::SCME_MAP_GLOBAL
        : NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE;

    unsigned int uiSize;

    // Look up the global attribute
    if (!g_pkCurrNSBShader)
    {
        return false;
    }
    else
    {
        if (bIsGlobal)
        {
            g_pkCurrAttribTable = 
                g_pkCurrNSBShader->GetGlobalAttributeTable();
        }
        else
        {
            g_pkCurrAttribTable = 
                g_pkCurrNSBShader->GetAttributeTable();
        }

        NSBAttributeDesc* pkAttribDesc = 
            g_pkCurrAttribTable->GetAttributeByName(pszName);

        if (!pkAttribDesc)
        {
            return false;
        }
        else
        {
            if (NiShaderConstantMapEntry::IsBool(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_BOOL;
                uiSize = sizeof(bool);
                bool bValue;
                
                pkAttribDesc->GetValue_Bool(bValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&bValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&bValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsUnsignedInt(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
                uiSize = sizeof(unsigned int);
                unsigned int uiValue;
                
                pkAttribDesc->GetValue_UnsignedInt(uiValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&uiValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&uiValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsFloat(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
                uiSize = sizeof(float);
                float fValue;
                
                pkAttribDesc->GetValue_Float(fValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&fValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&fValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsPoint2(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
                uiSize = sizeof(NiPoint2);
                NiPoint2 kPt2Value;
                
                pkAttribDesc->GetValue_Point2(kPt2Value);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&kPt2Value, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&kPt2Value, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsPoint3(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
                uiSize = sizeof(NiPoint3);
                NiPoint3 kPt3Value;
                
                pkAttribDesc->GetValue_Point3(kPt3Value);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber,
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&kPt3Value, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&kPt3Value, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsPoint4(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
                uiSize = sizeof(float) * 4;
                float afValue[4];
                float* pfValue = &afValue[0];
                
                pkAttribDesc->GetValue_Point4(pfValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)afValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiSize, (void*)afValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsMatrix3(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
                uiSize = sizeof(NiMatrix3);
                NiMatrix3 kMat3Value;
                
                pkAttribDesc->GetValue_Matrix3(kMat3Value);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&kMat3Value, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiSize, (void*)&kMat3Value, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsMatrix4(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
                uiSize = sizeof(float) * 16;
                float afValue[16];
                float* pfValue = &afValue[0];
                
                pkAttribDesc->GetValue_Matrix4(pfValue, uiSize);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber,
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)afValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiSize, (void*)afValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsColor(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
                uiSize = sizeof(NiColorA);
                NiColorA kClrValue;
                
                pkAttribDesc->GetValue_ColorA(kClrValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber,
                        uiRegisterStart, uiRegisterCount, (char*)0,
                        uiSize, uiSize, (void*)&kClrValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiSize, (void*)&kClrValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsArray(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY;

                // get description of array data
                NiShaderAttributeDesc::AttributeType eType;
                unsigned int uiElementSize;
                unsigned int uiNumElements;
                pkAttribDesc->GetArrayParams(
                    eType,
                    uiElementSize,
                    uiNumElements);

                // get copy of data
                uiSize = uiElementSize*uiNumElements;
                float* pfValues = NiAlloc(float,uiSize/sizeof(float));
                pkAttribDesc->GetValue_Array(pfValues,uiSize);

                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pszName, uiFlags, uiExtraNumber,
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiElementSize, pfValues, true))
                    {
                        // PROBLEM
                        NiFree(pfValues);
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiElementSize, pfValues, true))
                    {
                        // PROBLEM!
                        NiFree(pfValues);
                        return false;
                    }
                }
                
                NiFree(pfValues);
                return true;
            }
            else
            {
                NIASSERT(!"Invalid Attribute Type");
                return false;
            }
        }
    }            
}
//---------------------------------------------------------------------------
bool SetupOperatorEntry(char* pszName, int iRegStart, int iRegCount, 
    char* pszEntry1, int iOperation, char* pszEntry2, bool bInverse, 
    bool bTranspose)
{
    if (!g_pkCurrConstantMap)
        return false;

    // Look up the 2 entries
    NSBConstantMap::NSBCM_Entry* pkEntry1;
    NSBConstantMap::NSBCM_Entry* pkEntry2;
    
    pkEntry1 = g_pkCurrConstantMap->GetEntryByKey(pszEntry1);
    pkEntry2 = g_pkCurrConstantMap->GetEntryByKey(pszEntry2);

    if (!pkEntry1 || !pkEntry2)    
    {
        NSFParsererror("CM_Operator operand not found\n");
        return false;
    }
    if (!(pkEntry1->IsDefined() || pkEntry1->IsGlobal() ||
          pkEntry1->IsAttribute() || pkEntry1->IsConstant()))
    {
        NSFParsererror("CM_Operator operand INVALID TYPE\n");
        return false;
    }
    if (!(pkEntry2->IsDefined() || pkEntry2->IsGlobal() ||
          pkEntry2->IsAttribute() || pkEntry2->IsConstant()))
    {
        NSFParsererror("CM_Operator operand INVALID TYPE\n");
        return false;
    }

    unsigned int uiEntry1;
    unsigned int uiEntry2;
    
    uiEntry1 = g_pkCurrConstantMap->GetEntryIndexByKey(pszEntry1);
    uiEntry2 = g_pkCurrConstantMap->GetEntryIndexByKey(pszEntry2);

    if ((uiEntry1 == 0xffffffff) || (uiEntry2 == 0xffffffff))    
    {
        NSFParsererror("CM_Operator operand INVALID INDEX\n");
        return false;
    }

    // Determine the results data type and set it in the flags
    NiShaderAttributeDesc::AttributeType eType1 = 
        pkEntry1->GetAttributeType();
    NiShaderAttributeDesc::AttributeType eType2 = 
        pkEntry2->GetAttributeType();

    if (pkEntry1->IsDefined())
    {
        // We have to look-up the type
        if (g_eConstMapMode == CONSTMAP_VERTEX)
        {
            eType1 = NiShaderConstantMap::LookUpPredefinedMappingType(
                pkEntry1->GetKey());
        }
        else
        if (g_eConstMapMode == CONSTMAP_GEOMETRY)
        {
            eType1 = NiShaderConstantMap::LookUpPredefinedMappingType(
                pkEntry1->GetKey());
        }
        else
        if (g_eConstMapMode == CONSTMAP_PIXEL)
        {
            eType1 = NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
        }
        else
        {
            NIASSERT(!"Invalid constant map mode!");
            return false;
        }
    }
    if (pkEntry2->IsDefined())
    {
        // We have to look-up the type
        if (g_eConstMapMode == CONSTMAP_VERTEX)
        {
            eType2 = NiShaderConstantMap::LookUpPredefinedMappingType(
                pkEntry2->GetKey());
        }
        else
        if (g_eConstMapMode == CONSTMAP_GEOMETRY)
        {
            eType2 = NiShaderConstantMap::LookUpPredefinedMappingType(
                pkEntry2->GetKey());
        }
        else
        if (g_eConstMapMode == CONSTMAP_PIXEL)
        {
            eType2 = NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
        }
        else
        {
            NIASSERT(!"Invalid constant map mode!");
            return false;
        }
    }
        
    if ((eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED) ||
        NiShaderConstantMapEntry::IsBool(eType1) ||
        NiShaderConstantMapEntry::IsString(eType1) ||
        NiShaderConstantMapEntry::IsTexture(eType1) ||
        (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED) ||
        NiShaderConstantMapEntry::IsBool(eType2) ||
        NiShaderConstantMapEntry::IsString(eType2) ||
        NiShaderConstantMapEntry::IsTexture(eType2))
    {
        NSFParsererror("Invalid Operator Type");
        return false;
    }

    NiShaderAttributeDesc::AttributeType eResultType = 
        DetermineOperatorResult(iOperation, eType1, eType2);
    if (eResultType == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED)
    {
        NSFParsererror("Invalid Operator - Result was invalid");
        return false;
    }
    
    // We have two valid entries, so let's setup the ConstantMapEntry
    // NOTE: If NSBConstantMap did not store it's list the proper way,
    // ie, via AddTail, this method would not work!
    unsigned int uiExtra = iOperation | uiEntry1 | 
        (uiEntry2 << NiShaderConstantMapEntry::SCME_OPERATOR_ENTRY2_SHIFT);
    if (bTranspose)
        uiExtra |= NiShaderConstantMapEntry::SCME_OPERATOR_RESULT_TRANSPOSE;
    if (bInverse)
        uiExtra |= NiShaderConstantMapEntry::SCME_OPERATOR_RESULT_INVERSE;

    unsigned int uiFlags = NiShaderConstantMapEntry::SCME_MAP_OPERATOR | 
        NiShaderConstantMapEntry::GetAttributeFlags(eResultType);

    if (g_bConstantMapPlatformBlock)
    {
        if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
            g_uiCurrentPlatforms, pszName, uiFlags, uiExtra, 
            iRegStart, iRegCount, (char*)0))
        {
            // PROBLEM
            return false;
        }
    }
    else
    {
        if (!g_pkCurrConstantMap->AddEntry(pszName, uiFlags, uiExtra, 
            iRegStart, iRegCount, (char*)0))
        {
            // PROBLEM
            return false;
        }
    }
        
    return true;
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineOperatorResult(int iOperation, 
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (iOperation)
    {
    case NiShaderConstantMapEntry::SCME_OPERATOR_MULTIPLY:
        return DetermineResultMultiply(eType1, eType2);
    case NiShaderConstantMapEntry::SCME_OPERATOR_DIVIDE:
        return DetermineResultDivide(eType1, eType2);
    case NiShaderConstantMapEntry::SCME_OPERATOR_ADD:
        return DetermineResultAdd(eType1, eType2);
    case NiShaderConstantMapEntry::SCME_OPERATOR_SUBTRACT:
        return DetermineResultSubtract(eType1, eType2);
    default:
        break;
    }
    return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineResultMultiply(
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
    default:
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    }
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineResultDivide(
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
    default:
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        break;
    }
    return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineResultAdd(
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
    default:
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    }
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineResultSubtract(
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
    default:
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    }
}
//---------------------------------------------------------------------------

