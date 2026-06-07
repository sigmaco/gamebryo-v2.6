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

#include "NiApplicationPCH.h"
#include <NiRTLib.h>
#include <NiSystem.h>
#include "NiCommand.h"

char NiCommand::ms_acOptionNotFound[] = "option not found";
char NiCommand::ms_acArgumentRequired[] = "option requires an argument";
char NiCommand::ms_acArgumentOutOfRange[] = "argument out of range";
char NiCommand::ms_acFilenameNotFound[] = "filename not found";

//---------------------------------------------------------------------------
NiCommand::NiCommand(int iArgc, char** ppcArgv)
{
    m_iArgc = iArgc;
    m_ppcArgv = ppcArgv;
    m_pcCmdline = 0;
    m_pbUsed = 0;

    Initialize();
}
//---------------------------------------------------------------------------
NiCommand::NiCommand(const char* pcCmdline)
{
    typedef struct Argument
    {
        char* pcItem;
        Argument* pkNext;
    }
    Argument;

    m_iArgc = 0;
    m_ppcArgv = 0;
    m_pbUsed = 0;
    m_pcCmdline = 0;

    if (pcCmdline == 0 || strlen(pcCmdline) == 0)
        return;

    // Build a list of pointers to the command line tokens.  A copy of the
    // input command line is used because changes are made to the command
    // line string by this class.
    size_t stLen = strlen(pcCmdline) + 1;
    m_pcCmdline = NiAlloc(char, stLen);
    NiStrcpy(m_pcCmdline, stLen, pcCmdline);

    // Remove any new line characters from the command line buffer.
    for (unsigned int ui = 0; ui < strlen(pcCmdline) + 1; ui++)
    {
        if (m_pcCmdline[ui] == '\r' || m_pcCmdline[ui] == '\n')
        {
            m_pcCmdline[ui] = ' ';
        }
    }

    char* pcToken = m_pcCmdline;
    Argument* pkList = 0;

    while (pcToken && *pcToken)
    {
        m_iArgc++;
        Argument* pkCurrent = NiAlloc(Argument,1);
        pkCurrent->pkNext = pkList;
        pkList = pkCurrent;

        if (pcToken[0] == '\"')
        {
            // token is quoted, strip first quote
            pkList->pcItem = ++pcToken;

            // find matching quote
            while (*pcToken != 0 && *pcToken != '\"')
                pcToken++;
        }
        else
        {
            // use the token as is
            pkList->pcItem = pcToken;
            while (*pcToken != 0 && *pcToken != ' ' && *pcToken != '\t')
                pcToken++;
        }

        // null-terminate current token
        if (*pcToken != 0)
            *pcToken++ = 0;

        // search for next token
        while (*pcToken != 0 && (*pcToken == ' ' || *pcToken == '\t'))
            pcToken++;
    }

    m_iArgc++;
    m_ppcArgv = NiAlloc(char*, m_iArgc);
    m_ppcArgv[0] = m_pcCmdline;
    int i = m_iArgc - 1;
    while (pkList)
    {
        m_ppcArgv[i--] = pkList->pcItem;
        
        Argument* pkSave = pkList->pkNext;
        NiFree(pkList);
        pkList = pkSave;
    }

    Initialize();
}
//---------------------------------------------------------------------------
NiCommand::~NiCommand()
{
    NiFree(m_pbUsed);

    if (m_pcCmdline)
    {
        NiFree(m_ppcArgv);
        NiFree(m_pcCmdline);
    }
}
//---------------------------------------------------------------------------
void NiCommand::Initialize()
{
    m_pbUsed = NiAlloc(bool, m_iArgc);
    memset(m_pbUsed, false, m_iArgc*sizeof(bool));

    m_fSmall = 0.0f;
    m_fLarge = 0.0f;
    m_bMinSet = false;
    m_bMaxSet = false;
    m_bInfSet = false;
    m_bSupSet = false;
}
//---------------------------------------------------------------------------
int NiCommand::ExcessArguments()
{
    if (!this)
        return 0;

    // checks to see if any command line arguments were not processed
    for (int k = 1; k < m_iArgc; k++)
    {
        if (!m_pbUsed[k])
            return k;
    }

    return 0;
}
//---------------------------------------------------------------------------
NiCommand& NiCommand::Min(float fValue)
{
    m_fSmall = fValue;
    m_bMinSet = true;
    return *this;
}
//---------------------------------------------------------------------------
NiCommand& NiCommand::Max(float fValue)
{
    m_fLarge = fValue;
    m_bMaxSet = true;
    return *this;
}
//---------------------------------------------------------------------------
NiCommand& NiCommand::Inf(float fValue)
{
    m_fSmall = fValue;
    m_bInfSet = true;
    return *this;
}
//---------------------------------------------------------------------------
NiCommand& NiCommand::Sup(float fValue)
{
    m_fLarge = fValue;
    m_bSupSet = true;
    return *this;
}
//---------------------------------------------------------------------------
int NiCommand::Boolean(const char* pcName)
{
    if (!this)
        return 0;

    bool bValue = false;
    return Boolean(pcName, bValue);
}
//---------------------------------------------------------------------------
int NiCommand::Boolean(const char* pcName, bool& bValue)
{
    if (!this)
        return 0;

    int iMatchFound = 0;
    bValue = false;
    for (int k = 1; k < m_iArgc; k++)
    {
        char* pcPtr = m_ppcArgv[k];
        if (!m_pbUsed[k] && pcPtr[0] == '-' && strcmp(pcName, ++pcPtr) == 0)
        {
            m_pbUsed[k] = true;
            iMatchFound = k;
            bValue = true;
            break;
        }
    }

    if (iMatchFound == 0)
        m_pcLastError = ms_acOptionNotFound;

    return iMatchFound;
}
//---------------------------------------------------------------------------
int NiCommand::Integer(const char* pcName, int& iValue)
{
    if (!this)
        return 0;

    int iMatchFound = 0;
    for (int k = 1; k < m_iArgc; k++)
    {
        char* pcPtr = m_ppcArgv[k];
        if (!m_pbUsed[k] && pcPtr[0] == '-' && strcmp(pcName, ++pcPtr) == 0)
        {
            // get argument
            pcPtr = m_ppcArgv[k+1];
            if ( m_pbUsed[k+1] || (pcPtr[0] == '-' && 
                 !(pcPtr[1] >= '0' && pcPtr[1] <= '9')) )
            {
                m_pcLastError = ms_acArgumentRequired;
                return 0;
            }
#if defined(_MSC_VER) && _MSC_VER >= 1400
            sscanf_s(pcPtr, "%d", &iValue);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
            sscanf(pcPtr, "%d", &iValue);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
            if ((m_bMinSet && iValue < m_fSmall) ||
                (m_bMaxSet && iValue > m_fLarge) ||
                (m_bInfSet && iValue <= m_fSmall) ||
                (m_bSupSet && iValue >= m_fLarge))
            {
                m_pcLastError = ms_acArgumentOutOfRange;
                return 0;
            }
            m_pbUsed[k] = true;
            m_pbUsed[k+1] = true;
            iMatchFound = k;
            break;
        }
    }

    m_bMinSet = false;
    m_bMaxSet = false;
    m_bInfSet = false;
    m_bSupSet = false;

    if (iMatchFound == 0)
        m_pcLastError = ms_acOptionNotFound;

    return iMatchFound;
}
//---------------------------------------------------------------------------
int NiCommand::Float(const char* pcName, float& fValue)
{
    if (!this)
        return 0;

    int iMatchFound = 0;
    for (int k = 1; k < m_iArgc; k++)
    {
        char* pcPtr = m_ppcArgv[k];
        if (!m_pbUsed[k] && pcPtr[0] == '-' && strcmp(pcName, ++pcPtr) == 0)
        {
            // get argument
            pcPtr = m_ppcArgv[k + 1];
            if ( m_pbUsed[k+1] || (pcPtr[0] == '-' && 
                 !(pcPtr[1] >= '0' && pcPtr[1] <= '9')) )
            {
                m_pcLastError = ms_acArgumentRequired;
                return 0;
            }
#if defined(_MSC_VER) && _MSC_VER >= 1400
            sscanf_s(pcPtr, "%f", &fValue);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
            sscanf(pcPtr, "%f", &fValue);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
            if ((m_bMinSet && fValue < m_fSmall) ||
                (m_bMaxSet && fValue > m_fLarge) ||
                (m_bInfSet && fValue <= m_fSmall) ||
                (m_bSupSet && fValue >= m_fLarge))
            {
                m_pcLastError = ms_acArgumentOutOfRange;
                return 0;
            }
            m_pbUsed[k] = true;
            m_pbUsed[k+1] = true;
            iMatchFound = k;
            break;
        }
    }

    m_bMinSet = false;
    m_bMaxSet = false;
    m_bInfSet = false;
    m_bSupSet = false;

    if (iMatchFound == 0)
        m_pcLastError = ms_acOptionNotFound;

    return iMatchFound;
}
//---------------------------------------------------------------------------
int NiCommand::String(const char* pcName, char* pcValue, 
    unsigned int uiBufferLen)
{
    if (!this)
        return 0;

    int iMatchFound = 0;
    for (int k = 1; k < m_iArgc; k++)
    {
        char* pcPtr = m_ppcArgv[k];
        if (!m_pbUsed[k] && pcPtr[0] == '-' && strcmp(pcName, ++pcPtr) == 0)
        {
            // get argument
            pcPtr = m_ppcArgv[k+1];
            if (m_pbUsed[k+1] || pcPtr[0] == '-')
            {
                m_pcLastError = ms_acArgumentRequired;
                return 0;
            }
            NiStrcpy(pcValue, uiBufferLen, pcPtr);
            m_pbUsed[k] = true;
            m_pbUsed[k+1] = true;
            iMatchFound = k;
            break;
        }
    }

    if (iMatchFound == 0)
        m_pcLastError = ms_acOptionNotFound;

    return iMatchFound;
}
//---------------------------------------------------------------------------
int NiCommand::Filename(char* pcFilename, unsigned int uiBufferLen)
{
    if (!this)
        return 0;

    int iMatchFound = 0;
    for (int k = 1; k < m_iArgc; k++)
    {
        char* pcPtr = m_ppcArgv[k];
        if (!m_pbUsed[k] && pcPtr[0] != '-')
        {
            NiStrcpy(pcFilename, uiBufferLen, pcPtr);
            m_pbUsed[k] = true;
            iMatchFound = k;
            break;
        }
    }

    if (iMatchFound == 0)
        m_pcLastError = ms_acFilenameNotFound;

    return iMatchFound;
}
//---------------------------------------------------------------------------
