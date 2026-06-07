// Copyright (C) 1998-2000 Alias | Wavefront, a division of Silicon Graphics
// Limited.
//
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
// $Revision: /main/3 $
// $Date: 2000/04/19 15:30:34 $
//
// Description:
//              Metacycle is not implemented in Maya.
//              All the functions in this file either return NULL 
//              or commented out at this point.
//

// Precompiled Headers
#include "MayaPluginPCH.h"


// HERE ARE THE BACKDOORS!
//
#define ACN_NODE_PREVIEW_LOOPS  0
#define ACN_NODE_PREVIEW_FRAMES 1
#define ACN_STARTFRAME_ONE      0
#define ACN_STARTFRAME_END      1
#define ACN_STARTFRAME_SPACED   2
#define ACN_STARTFRAME_ALL      3

#define ACN_ENDFRAME_START          0
#define ACN_ENDFRAME_ONE            1

#define ACN_ROLLING_ENDPOINT     0
#define ACN_FIXED_ENDPOINT       1

#define ACN_ROLLING_STARTPOINT     0
#define ACN_FIXED_STARTPOINT       1

#define ACN_LINEAR               0
#define ACN_EASE                 1

#ifdef WIN32
#include <string.h>
#else
#include <strings.h>
#endif

/*
 * This is a magic number to terminate a list of transitions.
 */
#define LIST_TERM -999999


//
// Return a count of the number of characters in the scene
//
int DtCnetCharacterCount( int *count )
{
    *count = 0;

    return 0;
}

// 
// Returns the range of frames defined in the character network.
// 1 on success, 0 on failure
//
//  Not implemented for Maya

int DtCnetCharacterGetFrameRange(int cnetId, int *start, int *end)
{
    NI_UNUSED_ARG(cnetId);
    *start = 0;
    *end = 0;

    return(0);
}  


//
// Return the number and list of parent shapes that belong
// to the character.
// 1 on success, 0 on failure.
//
//  Not implemented for Maya

int DtCnetCharacterGetShapes(int cnetId, int *count, int **shapelist)
{
    NI_UNUSED_ARG(cnetId);
    *count = 0;
    *shapelist = NULL;

    return(0);
}
