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
#include "NiAnimationPCH.h"

#include "NiPrimeNumber.h"
#include "NiDebug.h"

// Prime numbers up to 1024 (and slightly beyond).
const unsigned int sauiPrimeArray[] = 
    {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 
    71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 
    127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 
    179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 
    233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 
    283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 
    353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 
    419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 
    467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 
    547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 
    607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 
    661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 
    739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 
    811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 
    877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 
    947, 953, 967, 971, 977, 983, 991, 997, 1009, 1013, 
    1019, 1021, 1031};
const unsigned int sauiPrimeArrayCount = 
    sizeof(sauiPrimeArray) / sizeof(unsigned int);

//---------------------------------------------------------------------------
unsigned int NiPrimeNumber::GetPrimeArrayCount()
{
    return sauiPrimeArrayCount;
}
//---------------------------------------------------------------------------
unsigned int NiPrimeNumber::GetPrimeAt(unsigned int uiIndex)
{
    NIASSERT(uiIndex < sauiPrimeArrayCount);
    return sauiPrimeArray[uiIndex];
}
//---------------------------------------------------------------------------
unsigned int NiPrimeNumber::GetMaxPrime()
{
    NIASSERT(sauiPrimeArrayCount >= 1);
    return sauiPrimeArray[sauiPrimeArrayCount - 1];
}
//---------------------------------------------------------------------------
bool NiPrimeNumber::GetNearbyPrime(unsigned int uiNumber,
    unsigned int& uiPrime)
{
    NIASSERT(sauiPrimeArrayCount >= 2);

    // Compare against second last prime in array.
    if (uiNumber > sauiPrimeArray[sauiPrimeArrayCount - 2])
    {
        uiPrime = sauiPrimeArray[sauiPrimeArrayCount - 1];
        return (uiNumber <= uiPrime);
    }

    // Compare against first prime in array.
    if (uiNumber < sauiPrimeArray[0])
    {
        uiPrime = sauiPrimeArray[0];
        return true;
    }

    // Use binary search to find next largest prime.
    int iBottom = 0;
    int iTop = (int)sauiPrimeArrayCount - 1;
    int iMiddle = 0;

    while (iBottom <= iTop)
    {
        iMiddle = (iTop + iBottom) >> 1; // Average to get the middle.
        if (uiNumber == sauiPrimeArray[iMiddle])
        {
            // Number equals a prime.
            uiPrime = sauiPrimeArray[iMiddle];
            return true;
        }
        else if (uiNumber > sauiPrimeArray[iMiddle])
        {
            // Number is larger than "middle" prime.
            iBottom = iMiddle + 1;
        }
        else
        {
            // Number is less than "middle" prime.
            iTop = iMiddle - 1;
        }
    }

    // Number is less than or equal to "bottom" prime.
    uiPrime = sauiPrimeArray[iBottom];
    return true;
}
//---------------------------------------------------------------------------
