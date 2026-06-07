/**********************************************************************
 
    FILE: IRadiosityMesh.h

    DESCRIPTION:  Public interface of a the Radiosity Mesh

    CREATED BY: Neil Hazzard, Discreet

    HISTORY: - created Sept 24, 2002

 *> Copyright (c) 1998-2002, All Rights Reserved.
 **********************************************************************/

#ifndef __RADIOSITYMESH__H
#define __RADIOSITYMESH__H

#include "Max.h"
#include "iFnPub.h"
#include "maxtypes.h"

#define IRADIOSITYMESHINTERFACE Interface_ID(0x3a4e3329, 0x290369ad)

class IRadiosityMesh: public FPStaticInterface
{
public:
    // Get the actual mesh used for the Radiosity solution.  This may not be 
    // topologically the same as the mesh in the stack, especially if 
    // Subdivision has been used in the solution
    virtual Mesh & GetRadiosityMesh(INode * node)=0;

    // Is the Solution valid and upto dat
    virtual BOOL IsRadiosityValid() =0;

    //Is the node in the solution - this needs to be checked before using
    //GetRadiosityMesh()
    virtual BOOL IsNodeInSolution(INode * node)=0;
};

#endif