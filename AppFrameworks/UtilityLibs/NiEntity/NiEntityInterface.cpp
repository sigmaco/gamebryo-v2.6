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

// Precompiled Header
#include "NiEntityPCH.h"

#include "NiEntityInterface.h"

NiFixedString NiEntityInterface::IT_ENTITYINTERFACE;

//---------------------------------------------------------------------------
void NiEntityInterface::_SDMInit()
{
    IT_ENTITYINTERFACE = "Entity Interface";
}
//---------------------------------------------------------------------------
void NiEntityInterface::_SDMShutdown()
{
    IT_ENTITYINTERFACE = NULL;
}
//---------------------------------------------------------------------------
NiFixedString NiEntityInterface::GetInterfaceType()
{
    return IT_ENTITYINTERFACE;
}
//---------------------------------------------------------------------------
const NiFixedString& NiEntityInterface::GetTags() const
{
    return m_kTags;
}
//---------------------------------------------------------------------------
void NiEntityInterface::SetTags(const NiFixedString& kTags)
{
    m_kTags = kTags;
}
//---------------------------------------------------------------------------
void NiEntityInterface::AddTag(const char* pcTag)
{
    NIASSERT(pcTag && strlen(pcTag) > 0);

    NIASSERT(!ContainsTag(pcTag));

    // Determine the string lengths for the existing tags string and new tag.
    size_t stCurTagsLength = m_kTags.Exists() ? strlen(m_kTags) : 0;
    size_t stNewTagLength = strlen(pcTag);

    // Get delimiter character and build delimiter string.
    char cDelim = GetTagDelimiter();
    char acDelimString[2];
    acDelimString[0] = cDelim;
    acDelimString[1] = '\0';

    // Determine whether or not the tag strings already have delimiters.
    bool bCurTagsHasDelim =  m_kTags.Exists() ?
        (((const char*) m_kTags)[stCurTagsLength - 1] == cDelim) :
        true;
    bool bNewTagHasDelim = (pcTag[stNewTagLength - 1] == cDelim);

    // Check for a new tag consisting only of a delimiter.
    if (bNewTagHasDelim && stNewTagLength == 1)
    {
        return;
    }

    // Allocate temporary buffer for building new tags string.
    size_t stNewArraySize = stCurTagsLength + stNewTagLength + 3;
    char* pcNewTags = NiAlloc(char, stNewArraySize);

    // Build the new tags string.
    NiSprintf(pcNewTags, stNewArraySize, "%s%s%s%s",
        m_kTags.Exists() ? (const char*) m_kTags : "",
        bCurTagsHasDelim ? "" : acDelimString,
        pcTag,
        bNewTagHasDelim ? "" : acDelimString);

    // Set the new tags string and free the temporary buffer.
    m_kTags = pcNewTags;
    NiFree(pcNewTags);
}
//---------------------------------------------------------------------------
void NiEntityInterface::RemoveTag(const char* pcTag)
{
    NIASSERT(pcTag && strlen(pcTag) > 0);

    if (!m_kTags.Exists())
    {
        return;
    }

    // Build delimiter string.
    char cDelim = GetTagDelimiter();
    char acDelimString[2];
    acDelimString[0] = cDelim;
    acDelimString[1] = '\0';

    // Determine if tag already contains delimiter.
    size_t stTagLength = strlen(pcTag);
    if (pcTag[stTagLength - 1] == cDelim)
    {
        --stTagLength;
    }
    if (stTagLength == 0)
    {
        return;
    }

    // Allocate data arrays.
    size_t stArraySize = strlen(m_kTags) + 1;
    char* pcOldTags = NiAlloc(char, stArraySize);
    NiStrcpy(pcOldTags, stArraySize, m_kTags);
    char* pcNewTags = NiAlloc(char, stArraySize);
    pcNewTags[0] = '\0';

    // Build new tags string, skipping the tag to be removed.
    char* pcContext = NULL;
    char* pcToken = NiStrtok(pcOldTags, acDelimString, &pcContext);
    while (pcToken)
    {
        if (strncmp(pcToken, pcTag, stTagLength) != 0)
        {
            NiStrcat(pcNewTags, stArraySize, pcToken);
            NiStrcat(pcNewTags, stArraySize, acDelimString);
        }

        pcToken = NiStrtok(NULL, acDelimString, &pcContext);
    }

    // Set new tags string.
    m_kTags = pcNewTags;

    // Free data arrays.
    NiFree(pcOldTags);
    NiFree(pcNewTags);
}
//---------------------------------------------------------------------------
NiBool NiEntityInterface::ContainsTag(const char* pcTag)
{
    NIASSERT(pcTag && strlen(pcTag) > 0);

    if (!m_kTags.Exists())
    {
        return false;
    }

    // Build delimiter string.
    char cDelim = GetTagDelimiter();
    char acDelimString[2];
    acDelimString[0] = cDelim;
    acDelimString[1] = '\0';

    // Determine if tag already contains delimiter.
    size_t stTagLength = strlen(pcTag);
    if (pcTag[stTagLength - 1] == cDelim)
    {
        --stTagLength;
    }
    if (stTagLength == 0)
    {
        return false;
    }

    // Allocate data array.
    size_t stArraySize = strlen(m_kTags) + 1;
    char* pcTags = NiAlloc(char, stArraySize);
    NiStrcpy(pcTags, stArraySize, m_kTags);

    // Search for tag.
    bool bTagFound = false;
    char* pcContext = NULL;
    char* pcToken = NiStrtok(pcTags, acDelimString, &pcContext);
    while (pcToken)
    {
        if (strncmp(pcToken, pcTag, stTagLength) == 0)
        {
            bTagFound = true;
            break;
        }
        pcToken = NiStrtok(NULL, acDelimString, &pcContext);
    }

    // Free data array.
    NiFree(pcTags);

    return bTagFound;
}
//---------------------------------------------------------------------------
char NiEntityInterface::GetTagDelimiter()
{
    return ';';
}
//---------------------------------------------------------------------------
