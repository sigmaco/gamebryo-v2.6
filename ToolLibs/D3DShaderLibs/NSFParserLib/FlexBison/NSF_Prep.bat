@echo off

SETLOCAL

rem Batch file for NSFParserLib file generation.

rem Set paths for Bison and Flex
SET BISON_PATH=..\..\..\..\ThirdPartyCode\Bison\Bison2.1\bin
SET FLEX_PATH=..\..\..\..\ThirdPartyCode\Flex\Flex2.5.4a\bin

rem Run Bison to generate the parser
%BISON_PATH%\bison --defines -l -p NSFParser --verbose -o ..\NSFGrammar.cpp NSFGrammar.y

rem Clobber NSFLexSymbol.h
if exist ..\NSFLexSymbol.h del ..\NSFLexSymbol.h

rem Rename NSFGrammar.hpp to NSFLexSymbol.h
ren ..\NSFGrammar.hpp NSFLexSymbol.h

rem Run Flex to generate the lexer
%FLEX_PATH%\flex -s -PNSFParser -i -o..\NSFLexer.cpp NSFLexer.l

rem Clean-up
del ..\NSFGrammar.output

ENDLOCAL
