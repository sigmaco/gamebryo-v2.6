// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef NICOMMAND_H
#define NICOMMAND_H

#include <NiMemObject.h>

class NiCommand : public NiMemObject
{
public:
    NiCommand(int iArgc, char** ppcArgv);
    NiCommand(const char* pcCmdline);
    ~NiCommand();
    int ExcessArguments();  // return values is index of first excess argument

    // set bounds for numerical arguments
    // if bounds required, they must be set for each argument
    NiCommand& Min(float fValue);
    NiCommand& Max(float fValue);
    NiCommand& Inf(float fValue);
    NiCommand& Sup(float fValue);

    // return value of the following methods is the option index within the
    // argument array

    // Use the boolean methods for options which take no argument, for
    // example in
    //           myprogram -debug -x 10 -y 20 filename
    // the option -debug has no argument.

    int Boolean(const char* pcName);  // returns existence of option
    int Boolean(const char* pcName, bool& bValue);
    int Integer(const char* pcName, int& iValue);
    int Float(const char* pcName, float& fValue);
    int String(const char* pcName, char* pcValue, unsigned int uiBufferLen);
    int Filename(char* pcFilename, unsigned int uiBufferLen);

    // last error reporting
    inline const char* GetLastError() { return m_pcLastError; }

protected:
    // constructor support
    void Initialize();

    // command line information
    int m_iArgc;       // number of arguments
    char** m_ppcArgv;    // argument list(array of strings)
    char* m_pcCmdline;  // argument list(single string)
    bool* m_pbUsed;  // keeps track of arguments already processed

    // parameters for bounds checking
    float m_fSmall;   // lower bound for numerical argument(min or inf)
    float m_fLarge;   // upper bound for numerical argument(max or sup)
    bool m_bMinSet;    // if true, compare:  small <= arg
    bool m_bMaxSet;    // if true, compare:  arg <= large
    bool m_bInfSet;    // if true, compare:  small < arg
    bool m_bSupSet;    // if true, compare:  arg < large

    // last error strings
    char* m_pcLastError;
    static char ms_acOptionNotFound[];
    static char ms_acArgumentRequired[];
    static char ms_acArgumentOutOfRange[];
    static char ms_acFilenameNotFound[];
};

#endif

