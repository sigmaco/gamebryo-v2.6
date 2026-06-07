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

#ifndef NISKINPARTITION_H
#define NISKINPARTITION_H

#include "NiMainLibType.h"
#include "NiObject.h"
#include "NiSmartPointer.h"
#include "NiStream.h"
#include "NiTSet.h"

class NiSkinData;

/**
    This class is deprecated.

    It only exists to support loading old NIF files.
*/

class NIMAIN_ENTRY NiSkinPartition : public NiObject
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***

    class NIMAIN_ENTRY Partition : public NiMemObject
    {
    public:
        Partition();
        virtual ~Partition();

        unsigned short* m_pusBones;
        float* m_pfWeights;
        unsigned short* m_pusVertexMap;
        unsigned char* m_pucBonePalette;
        unsigned short* m_pusTriList;
        unsigned short* m_pusStripLengths;

        unsigned short m_usVertices;
        unsigned short m_usTriangles;
        unsigned short m_usBones;
        unsigned short m_usStrips;
        unsigned short m_usBonesPerVertex;

        void LoadBinary(NiStream& kStream);
        void SaveBinary(NiStream& kStream);
        bool IsEqual(const Partition& kPartition);
        void PostPrecache();
        unsigned int GetStripLengthSum() const;
    };

    NiSkinPartition();
    ~NiSkinPartition();

    inline unsigned int GetPartitionCount() const;
    Partition* GetPartitions();
    inline const Partition* GetPartitions() const;

    // *** end Emergent internal use only ***

protected:
    unsigned int m_uiPartitions;
    Partition* m_pkPartitions;
};

NiSmartPointer(NiSkinPartition);

#include "NiSkinPartition.inl"

#endif // NISKINPARTITION_H
