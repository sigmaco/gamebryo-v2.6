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

#ifndef NO_CSTUDIO
//---------------------------------------------------------------------------
inline NiMAXGeomConverter::NiBoneList::NtBoneEle* 
    NiMAXGeomConverter::NiBoneList::GetBoneEle(NtBoneData* pBoneData)
{
    NtBoneEle* pBoneEle = m_ppBones[pBoneData->uiBoneIndex];
    NIASSERT(pBoneEle);
    INode* pListNode = pBoneEle->pNode;
    while (pListNode != pBoneData->pBone)
    {
        pBoneEle = pBoneEle->pNext;
        NIASSERT(pBoneEle);
        pListNode = pBoneEle->pNode;
    }
    return pBoneEle;
}
//---------------------------------------------------------------------------
inline unsigned int
NiMAXGeomConverter::NiBoneList::GetNumBones()
{
    return m_uiNumBones;
}
//---------------------------------------------------------------------------
#endif
