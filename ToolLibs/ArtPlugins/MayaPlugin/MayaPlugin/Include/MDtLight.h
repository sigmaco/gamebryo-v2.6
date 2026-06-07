// Copyright (C) 1998-2007 Alias | Wavefront, a division of 
// Silicon Graphics Limited.
// The information in this file is provided for the exclusive use of the
// licensees of Alias | Wavefront.  Such users have the right to use, modify,
// and incorporate this code into other products for purposes authorized
// by the Alias | Wavefront license agreement, without fee.
// 
// ALIAS | WAVEFRONT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
// INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
// EVENT SHALL ALIAS | WAVEFRONT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
// CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
// DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.
// 
// 
// $Author: mtaylor $
// $Revision: /main/3 $
// $Date: 2000/04/19 15:32:32 $
//
//
// Original Author:    XCW
//
//

#ifndef   MDTLIGHT_H
#define   MDTLIGHT_H

int add_AmbientLight( MObject, MObject );
int add_DirectionLight( MObject, MObject );
int add_PointLight( MObject, MObject );
int add_SpotLight( MObject, MObject );
MObject is_LightGroup( MObject );
int addTransformLight( MObject, MObject );


bool DtLightGetShapeNode(int lightID, MObject& node);
bool DtLightGetTransformNode(int lightID, MObject& node);

#endif // MDTLIGHT_H

