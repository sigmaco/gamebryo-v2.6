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

#include "NiMain.h"
#include <NiMeshLib.h>
#include <NiAnimation.h>

#include <iostream>

namespace
{
    void dumpStrings(NiViewerStringsArray& kStrings, int indentation);
    int dumpNode(NiNode& kNode, int indentation);
    int dumpObject(NiObject& kObject, int indentation);
    int dumpStream(NiStream& kStream);
    int dump(int argc, char* argv[]);

    //---------------------------------------------------------------------------
    void indent(int indentation)
    {
        static char blanks[] = "                                                 "
            "                                                       ";

        const int index = indentation * 2 >= sizeof(blanks)
                        ? sizeof(blanks) - 1
                        : indentation * 2;

        blanks[index] = 0;
        std::cout << blanks;
        blanks[index] = ' ';
    }

    //---------------------------------------------------------------------------
    void dumpStrings(NiViewerStringsArray& kStrings, int indentation)
    {
        for (unsigned int i = 0; i < kStrings.GetSize(); ++i)
        {
            char* pText = kStrings[i];
            if (pText)
            {
                indent(indentation);
                std::cout << pText << std::endl;
            }
        }
    }

    //---------------------------------------------------------------------------
    int dumpNode(NiNode& kNode, int indentation)
    {
        int result = 0;

        for (unsigned int i = 0; i < kNode.GetChildCount() && 0 == result; ++i)
        {
            NiAVObject* pkObject = kNode.GetAt(i);
            if (pkObject)
            {
                result = dumpObject(*pkObject, indentation+1);
            }
        }

        return result;
    }

    //---------------------------------------------------------------------------
    int dumpObject(NiObject& kObject, int indentation)
    {
        int result = 0;

        indent(indentation-1);
        std::cout << "{" << std::endl;

        NiViewerStringsArray kStrings;
        kObject.GetViewerStrings(&kStrings);
        dumpStrings(kStrings, indentation);

        if (NiNode* pkNode = NiDynamicCast(NiNode, &kObject))
        {
            result = dumpNode(*pkNode, indentation);
        }

        indent(indentation-1);
        std::cout << "}," << std::endl;

        return result;
    }

    //---------------------------------------------------------------------------
    int dumpStream(NiStream& kStream)
    {
        int result = 0;

        for (unsigned int i = 0; i < kStream.GetObjectCount() && 0 == result; ++i)
        {
            NiObject* pkObject = kStream.GetObjectAt(i);
            if (pkObject)
                result = dumpObject(*pkObject, 1);
        }

        return result;
    }

    //---------------------------------------------------------------------------
    int dump(int argc, char* argv[])
    {
        int result = 0;

        for (int i = 1; i < argc && 0 == result; ++i)
        {
            char* pFilename = argv[i];

            NiStream kStream;
            if (!kStream.Load(pFilename))
            {
                std::cerr << "ERROR: Cannot load stream \"" << pFilename << "\"." << std::endl;
                result = 1;
            }
            else
            {
                result = dumpStream(kStream);
            }
        }

        return result;
    }
}

int main(int argc, char* argv[])
{
    NiInit();
    NiSystemDesc::GetSystemDesc().SetToolMode(true);
    NiSystemDesc::GetSystemDesc().SetToolModeRendererID(
        NiSystemDesc::RENDERER_GENERIC);

    int result = dump(argc, argv);

    NiShutdown();

	return result;
}

