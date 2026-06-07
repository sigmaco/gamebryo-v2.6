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

#ifndef NIBOXBVMACROS_H
#define NIBOXBVMACROS_H

#define BB_EPSILON FLT_EPSILON

//---------------------------------------------------------------------------
// compare [-r,r] to NdD+dt*NdW
#define BT_TEST0(NdD,R,dt,N,W) \
{ \
    if ((NdD) > (R)) \
    { \
        if ((NdD) + ((dt) * (N).Dot(W)) > (R)) \
            return false; \
    } \
    else if ((NdD) < -(R)) \
    { \
        if ((NdD) + ((dt) * (N).Dot(W)) < -(R)) \
            return false; \
    } \
}
//---------------------------------------------------------------------------
// compare [-r,r] to [min{p,p+d0,p+d1},max{p,p+d0,p+d1}]+t*w where t >= 0
#define BT_TEST1(p,t,w,d0,d1,r) \
{ \
    if ((p) > (r)) \
    { \
        float min; \
        if ((d0) > 0.0f) \
        { \
            if ((d1) >= 0.0f) \
            { \
                if ((p) + ((t) * (w)) > (r)) \
                    return false; \
            } \
            else \
            { \
                min = (p) + (d1); \
                if ((min > (r)) && (min + ((t) * (w)) > (r))) \
                    return false; \
            } \
        } \
        else if ((d1) <= (d0)) \
        { \
            min = (p) + (d1); \
            if ((min > (r)) && (min + ((t) * (w)) > (r))) \
                return false; \
        } \
        else \
        { \
            min = (p) + (d0); \
            if ((min > (r)) && (min + ((t) * (w)) > (r))) \
                return false; \
        } \
    } \
    else if ((p) < -(r)) \
    { \
        float max; \
        if ((d0) <= 0.0f) \
        { \
            if ((d1) <= 0.0f) \
            { \
                if ((p) + ((t) * (w)) < -(r)) \
                    return false; \
            } \
            else \
            { \
                max = (p) + (d1); \
                if ((max < -(r)) && (max + ((t) * (w)) < -(r))) \
                    return false; \
            } \
        } \
        else if ((d1) >= (d0)) \
        { \
            max = (p) + (d1); \
            if ((max < -(r)) && (max + ((t) * (w)) < -(r))) \
                return false; \
        } \
        else \
        { \
            max = (p) + (d0); \
            if ((max < -(r)) && (max + ((t) * (w)) < -(r))) \
                return false; \
        } \
    } \
}
//---------------------------------------------------------------------------
// compare [-r,r] to [min{p,p+d},max{p,p+d}]+t*w where t >= 0
#define BT_TEST2(p,t,w,d,r) \
{ \
    if ((p) > (r)) \
    { \
        if ((d) >= 0.0f) \
        { \
            if ((p) + ((t) * (w)) > (r)) \
                return false; \
        } \
        else \
        { \
            float min = (p) + (d); \
            if ((min > (r)) && (min + ((t) * (w)) > (r))) \
                return false; \
        } \
    } \
    else if ((p) < -(r)) \
    { \
        if ((d) <= 0.0f) \
        { \
            if ((p) + ((t) * (w)) < -(r)) \
                return false; \
        } \
        else \
        { \
            float max = (p) + (d); \
            if ((max < -(r)) && (max + ((t) * (w)) < -(r))) \
                return false; \
        } \
    } \
}
//---------------------------------------------------------------------------
// compare [-r,r] to NdD+t*NdW
#define BT_FIND0(NdD,R,t,N,W,tmax,type,side,axis) \
{ \
    float tmp, NdW; \
    if ((NdD) > (R)) \
    { \
        NdW = (N).Dot(W); \
        if ((NdD) + ((t) * NdW) > (R)) \
            return false; /* (axis) is separating */ \
        tmp = ((R) - (NdD)) / NdW; \
        if (tmp > (tmax)) \
        { \
            (tmax) = tmp; \
            (type) = (axis); \
            (side) = 1; \
        } \
    } \
    else if ((NdD) < -(R)) \
    { \
        NdW = (N).Dot(W); \
        if ((NdD) + ((t) * NdW) < -(R)) \
            return false; /* (axis) is separating */ \
        tmp = -((R) + (NdD)) / NdW; \
        if (tmp > (tmax)) \
        { \
            (tmax) = tmp; \
            (type) = (axis); \
            (side) = -1; \
        } \
    } \
}
//---------------------------------------------------------------------------
// compare [-r,r] to [min{p,p+d0,p+d1},max{p,p+d0,p+d1}]+t*w where t >= 0
#define BT_FIND1(p,t,w,d0,d1,r,tmax,type,extr,side,axis) \
{ \
    float tmp; \
    if ((p) > (r)) \
    { \
        float min; \
        if ((d0) > 0.0f) \
        { \
            if ((d1) >= 0.0f) \
            { \
                if ((p) + ((t) * (w)) > (r)) \
                    return false; /* (axis) is separating */ \
                tmp = ((r) - (p)) / (w); \
                if (tmp > (tmax)) \
                { \
                    (tmax) = tmp; \
                    (type) = (axis); \
                    (extr) = 0; \
                    (side) = 1; \
                } \
            } \
            else \
            { \
                min = (p) + (d1); \
                if (min > (r)) \
                { \
                    if (min + ((t) * (w)) > (r)) \
                        return false; /* (axis) is separating */ \
                    tmp = ((r) - min) / (w); \
                    if (tmp > (tmax)) \
                    { \
                        (tmax) = tmp; \
                        (type) = (axis); \
                        (extr) = 2; \
                        (side) = 1; \
                    } \
                } \
            } \
        } \
        else if ((d1) <= (d0)) \
        { \
            min = (p) + (d1); \
            if (min > (r)) \
            { \
                if (min + ((t) * (w)) > (r)) \
                    return false; /* (axis) is separating */ \
                tmp = ((r) - min) / (w); \
                if (tmp > (tmax)) \
                { \
                    (tmax) = tmp; \
                    (type) = (axis); \
                    (extr) = 2; \
                    (side) = 1; \
                } \
            } \
        } \
        else \
        { \
            min = (p) + (d0); \
            if (min > (r)) \
            { \
                if (min + ((t) * (w)) > (r)) \
                    return false; /* (axis) is separating */ \
                tmp = ((r) - min) / (w); \
                if (tmp > (tmax)) \
                { \
                    (tmax) = tmp; \
                    (type) = (axis); \
                    (extr) = 1; \
                    (side) = 1; \
                } \
            } \
        } \
    } \
    else if ((p) < -(r)) \
    { \
        float max; \
        if ((d0) <= 0.0f) \
        { \
            if ((d1) <= 0.0f) \
            { \
                if ((p) + ((t) * (w)) < -(r)) \
                    return false; /* (axis) is separating */ \
                tmp = -((r) + (p)) / (w); \
                if (tmp > (tmax)) \
                { \
                    (tmax) = tmp; \
                    (type) = (axis); \
                    (extr) = 0; \
                    (side) = -1; \
                } \
            } \
            else \
            { \
                max = (p) + (d1); \
                if (max < -(r)) \
                { \
                    if (max + ((t) * (w)) < -(r)) \
                        return false; /* (axis) is separating */ \
                    tmp = -((r) + max) / (w); \
                    if (tmp > (tmax)) \
                    { \
                        (tmax) = tmp; \
                        (type) = (axis); \
                        (extr) = 2; \
                        (side) = -1; \
                    } \
                } \
            } \
        } \
        else if ((d1) >= (d0)) \
        { \
            max = (p) + (d1); \
            if (max < -(r)) \
            { \
                if (max + ((t) * (w)) < -(r)) \
                    return false; /* (axis) is separating */ \
                tmp = -((r) + max) / (w); \
                if (tmp > (tmax)) \
                { \
                    (tmax) = tmp; \
                    (type) = (axis); \
                    (extr) = 2; \
                    (side) = -1; \
                } \
            } \
        } \
        else \
        { \
            max = (p) + (d0); \
            if (max < -(r)) \
            { \
                if (max + ((t) * (w)) < -(r)) \
                    return false; /* (axis) is separating */ \
                tmp = -((r) + max) / (w); \
                if (tmp > (tmax)) \
                { \
                    (tmax) = tmp; \
                    (type) = (axis); \
                    (extr) = 1; \
                    (side) = -1; \
                } \
            } \
        } \
    } \
}
//---------------------------------------------------------------------------
// compare [-r,r] to [min{p,p+d},max{p,p+d}]+t*w where t >= 0
#define BT_FIND2(p,t,w,d,r,tmax,type,extr,side,axis) \
{ \
    float tmp; \
    if ((p) > (r)) \
    { \
        if ((d) >= 0.0f) \
        { \
            if ((p) + ((t) * (w)) > (r)) \
                return false; /* (axis) is separating */ \
            tmp = ((r) - (p)) / (w); \
            if (tmp > (tmax)) \
            { \
                (tmax) = tmp; \
                (type) = (axis); \
                (extr) = 0; \
                (side) = 1; \
            } \
        } \
        else \
        { \
            float min = (p) + (d); \
            if (min > (r)) \
            { \
                if (min + ((t) * (w)) > (r)) \
                    return false; /* (axis) is separating */ \
                tmp = ((r) - min) / (w); \
                if (tmp > (tmax)) \
                { \
                    (tmax) = tmp; \
                    (type) = (axis); \
                    (extr) = 1; \
                    (side) = 1; \
                } \
            } \
        } \
    } \
    else if ((p) < -(r)) \
    { \
        if ((d) <= 0.0f) \
        { \
            if ((p) + ((t) * (w)) < -(r)) \
                return false; /* (axis) is separating */ \
            tmp = -((r) + (p)) / (w); \
            if (tmp > (tmax)) \
            { \
                (tmax) = tmp; \
                (type) = (axis); \
                (extr) = 0; \
                (side) = -1; \
            } \
        } \
        else \
        { \
            float max = (p) + (d); \
            if (max < -(r)) \
            { \
                if (max + ((t) * (w)) < -(r)) \
                    return false; /* (axis) is separating */ \
                tmp = -((r) + max) / (w); \
                if (tmp > (tmax)) \
                { \
                    (tmax) = tmp; \
                    (type) = (axis); \
                    (extr) = 1; \
                    (side) = -1; \
                } \
            } \
        } \
    } \
}
//---------------------------------------------------------------------------
#define BT_GET_COEFF_PLUS_MINUS(coeff,side,cmat,ext) \
{ \
    if ((cmat) > 0.0f) \
        (coeff) = (side) * (ext); \
    else if ((cmat) < 0.0f) \
        (coeff) = -(side) * (ext); \
    else \
        (coeff) = 0.0f; \
}
//---------------------------------------------------------------------------
#define BT_GET_COEFF_MINUS_PLUS(coeff,side,cmat,ext) \
{ \
    if ((cmat) > 0.0f) \
        (coeff) = -(side) * (ext); \
    else if ((cmat) < 0.0f) \
        (coeff) = (side) * (ext); \
    else \
        (coeff) = 0.0f; \
}
//---------------------------------------------------------------------------
#define BT_GET_POINT(P,box,T,V,x) \
{ \
    const NiPoint3& center = (box).GetCenter(); \
    const NiPoint3* axis = (box).GetAxes(); \
    for (int k = 0; k < 3; k++) \
    { \
        (P)[k] = center[k] + (T) * (V)[k] + (x)[0] * axis[0][k] + \
            (x)[1] * axis[1][k] + (x)[2] * axis[2][k]; \
    } \
}
//---------------------------------------------------------------------------
#define BB_FIND0(p,q,r,t,tmax,type,side,axis) \
{ \
    float tmp; \
    if ((p) > (r)) \
    { \
        if ((q) > (r)) \
            return false; /* (axis) is separating */ \
        tmp = (t) * ((r) - (p)) / ((q) - (p)); \
        if (tmp > (tmax) + BB_EPSILON) \
        { \
            (tmax) = tmp; \
            (type) = (axis); \
            (side) = 1; \
        } \
    } \
    else if ((p) < -(r)) \
    { \
        if ((q) < -(r)) \
            return false; /* (axis) is separating */ \
        tmp = -(t) * ((r) + (p)) / ((q) - (p)); \
        if (tmp > (tmax) + BB_EPSILON) \
        { \
            (tmax) = tmp; \
            (type) = (axis); \
            (side) = -1; \
        } \
    } \
}
//---------------------------------------------------------------------------
#define BB_FIND1(p,q0,q1,q2,q3,r,t,tmax,type,side,axis) \
{ \
    float q, tmp; \
    if ((p) > (r)) \
    { \
        q = ((q0) * (q1)) - ((q2) * (q3)); \
        if (q > (r)) \
            return false; /* (axis) is separating */ \
        tmp = (t) * ((r) - (p)) / (q - (p)); \
        if (tmp > (tmax) + BB_EPSILON) \
        { \
            (tmax) = tmp; \
            (type) = (axis); \
            (side) = 1; \
        } \
    } \
    else if ((p) < -(r)) \
    { \
        q = ((q0) * (q1)) - ((q2) * (q3)); \
        if (q < -(r)) \
            return false; /* (axis) is separating */ \
        tmp = -(t) * ((r) + (p)) / (q - (p)); \
        if (tmp > (tmax) + BB_EPSILON) \
        { \
            (tmax) = tmp; \
            (type) = (axis); \
            (side) = -1; \
        } \
    } \
}
//---------------------------------------------------------------------------
#define BB_GET_COEFF_PLUS_MINUS(coeff,side,cmat,ext) \
{ \
    if ((cmat) > 0.0f) \
        (coeff) = (side) * (ext); \
    else if ((cmat) < 0.0f) \
        (coeff) = -(side) * (ext); \
    else \
        (coeff) = 0.0f; \
}
//---------------------------------------------------------------------------
#define BB_GET_COEFF_MINUS_PLUS(coeff,side,cmat,ext) \
{ \
    if ((cmat) > 0.0f) \
        (coeff) = -(side) * (ext); \
    else if ((cmat) < 0.0f) \
        (coeff) = (side) * (ext); \
    else \
        (coeff) = 0.0f; \
}
//---------------------------------------------------------------------------
#define BB_GET_POINT(kIntrPoint,box,fIntrTime,V,coeff) \
{ \
    for (int k = 0; k < 3; k++) \
    { \
        (kIntrPoint)[k] = (box).GetCenter()[k] + ((fIntrTime) * (V)[k]) + \
            (coeff)[0] * (box).GetAxis(0)[k] + \
            (coeff)[1] * (box).GetAxis(1)[k] + \
            (coeff)[2] * (box).GetAxis(2)[k]; \
    } \
}
//---------------------------------------------------------------------------

#endif
