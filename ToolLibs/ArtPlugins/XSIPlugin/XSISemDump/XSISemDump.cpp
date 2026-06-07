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

#include "CrosswalkGamebryoHelpers.h"

#include "Scene.h"
#include "Model.h"
#include "EnvelopeList.h"
#include "Envelope.h"

#include <stdio.h>
#include <tchar.h>
#include <stdarg.h>
#include <map>
#include <set>

//---------------------------------------------------------------------------
// Indentation.
//---------------------------------------------------------------------------
class Indent
{
public:
    Indent(int nestingLevel, const char* prefix = " {", const char* suffix = "}")
        : m_Nesting(nestingLevel)
        , m_Prefix(prefix)
        , m_Suffix(suffix)
    {
    }

    Indent(const Indent& other, const char* prefix = " {", const char* suffix = "}")
        : m_Nesting(other.m_Nesting+1)
        , m_Prefix(prefix)
        , m_Suffix(suffix)
    {
    }

    void Print(const char * format, ...) const
    {
        if (m_Prefix)
        {
            printf("\n%s", indent(m_Nesting));
        }

        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);

        if (m_Prefix)
        {
            printf("%s", m_Prefix);
            m_Prefix = 0;
        }
    }

    ~Indent()
    {
        if (!m_Prefix && m_Suffix)
        {
            printf("%s", m_Suffix);
        }
    }

private:
    static const char* indent(int nestingLevel)
    {
        static char buffer[256];
        int length = nestingLevel * 2 < sizeof(buffer) - 2
                   ? nestingLevel * 2 : sizeof(buffer) - 2;
        memset(buffer, ' ', length);
        buffer[length] = 0;
        return buffer;
    }

    int                     m_Nesting;
    mutable const char*     m_Prefix;
    const char*             m_Suffix;
};

//---------------------------------------------------------------------------
// Model dumping.
//---------------------------------------------------------------------------
static void dumpModel(CSLModel* in_pModel, const Indent& indent)
{
    if (! in_pModel)
        return;

    indent.Print("\"%s\" [%s]", in_pModel->GetName(), epg::GetModelTypeName(*in_pModel));
}

//---------------------------------------------------------------------------
static void dumpModelChildren(
    CSLModel* in_pModel,
    const Indent& indent,
    std::set<CSLModel*>* in_pOnlyThese = 0)
{
    if (! in_pModel)
        return;

    CSLModel** children = in_pModel->GetChildrenList();
    for (int i = 0; i < in_pModel->GetChildrenCount(); ++i)
    {
        CSLModel* pChild = children[i];
        Indent childIndent(indent);
        if (!in_pOnlyThese || in_pOnlyThese->count(pChild))
            dumpModel(pChild, childIndent);
        dumpModelChildren(pChild, childIndent, in_pOnlyThese);
    }
}

//---------------------------------------------------------------------------
// Skin dumping.
//---------------------------------------------------------------------------
static void RemoveChildrenBones(CSLModel* in_pBone, std::set<CSLModel*>& io_Bones)
{
    if (!in_pBone)
        return;

    CSLModel** children = in_pBone->GetChildrenList();
    for (int i = 0; i < in_pBone->GetChildrenCount(); ++i)
    {
        CSLModel* pChild = children[i];
        if (io_Bones.count(pChild))
            io_Bones.erase(pChild);

        RemoveChildrenBones(pChild, io_Bones);
    }
}

//---------------------------------------------------------------------------
static void dumpEnvelopeList(CSLEnvelopeList* in_pEnvList, const Indent& indent)
{
    if (! in_pEnvList)
        return;

    indent.Print("\"%s\" [%s]", in_pEnvList->GetName(), epg::GetTemplateTypeName(*in_pEnvList));

    CSLEnvelope** children = in_pEnvList->Envelopes();
    std::map<CSLModel*, std::set<CSLModel*> > skinBones;
    for (int i = 0; i < in_pEnvList->GetEnvelopeCount(); ++i)
    {
        skinBones[children[i]->GetEnvelope()].insert(children[i]->GetDeformer());
    }

    typedef std::map<CSLModel*,std::set<CSLModel*> >::iterator iter;
    for (iter pos = skinBones.begin(); pos != skinBones.end(); ++pos)
    {
        CSLModel* pSkin = pos->first;
        std::set<CSLModel*>& bones = pos->second;

        Indent skinIndent(indent, " deformed by {");
        dumpModel(pSkin, skinIndent);

        std::set<CSLModel*> rootBones(bones);
        typedef std::set<CSLModel*>::iterator iter;
        for (iter pos = rootBones.begin(); pos != rootBones.end(); ++pos)
        {
            CSLModel* pBone = *pos;
            RemoveChildrenBones(pBone, rootBones);
        }

        for (iter pos = rootBones.begin(); pos != rootBones.end(); ++pos)
        {
            Indent boneIndent(skinIndent);
            dumpModel(*pos, boneIndent);
            dumpModelChildren(*pos, boneIndent, &bones);
        }
    }
}

//---------------------------------------------------------------------------
// The program.
//---------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
    if (argc < 2)
    {
        fprintf( stderr, "Usage: CW2GB <dotXSI filename>\n");
        return 1;
    }
    _TCHAR * fileName = argv[1];

    CSLScene scene;
    SI_Error error = scene.Open(fileName);
    if (SI_SUCCESS != error)
    {
        fprintf(stderr, "Cannot open \"%s\".\n", fileName);
        return 1;
    }
    error = scene.Read();
    if (SI_SUCCESS != error)
    {
        fprintf(stderr, "Cannot read \"%s\". (Corrupted file?)\n", fileName);
        return 1;
    }

    {
        Indent sceneIndent(0);
        dumpModel(scene.Root(), sceneIndent);
        dumpModelChildren(scene.Root(), sceneIndent);
    }

    {
        Indent envListIndent(0);
        dumpEnvelopeList(scene.EnvelopeList(), envListIndent);
    }

    return 0;
}

//---------------------------------------------------------------------------
