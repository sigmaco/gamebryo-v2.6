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

#ifndef NIBSPLINEBASISDATA_H
#define NIBSPLINEBASISDATA_H

#include <NiObject.h>
#include <NiStream.h>
#include "NiAnimationLibType.h"
#include "NiBSplineBasis.h"

// NiBSplineBasisData contains an NiBSplineBasis for use in interpolation 
// of open, uniform B-Splines. This class is separate from NiBSplineData  
// because NiBSplineBasis caches its values internally so that subsequent  
// calls will save calculations. Since multiple NiBSplineInterpolators
// may have the same NiBSplineBasis, this value is encapsulated 
// in an NiObject container, allowing it to be cloned and streamed
// appropriately. 

class NIANIMATION_ENTRY NiBSplineBasisData : public NiObject
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareViewerStrings;
    NiDeclareClone(NiBSplineBasisData);

public:
    NiBSplineBasisData ();
    virtual ~NiBSplineBasisData ();

    void SetBasis(const NiBSplineBasis<float, 3>* pkBasisDegree3);
    NiBSplineBasis<float, 3>& GetDegree3Basis();

protected:
    NiBSplineBasis<float, 3> m_kBasisDegree3;
};

NiSmartPointer(NiBSplineBasisData);

//#include "NiBSplineBasisData.inl"

#endif
