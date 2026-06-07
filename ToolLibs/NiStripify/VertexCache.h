// nVidia PROPRIETARY INFORMATION
//
// This source code is from the nVidia developer site,
//     http://developer.nvidia.com
// The project utilized is nvTriStrip. Slight modifications were made to
// utilize the source in Gamebryo.
//
//      Copyright (c) 2004 nVidia Corporation
//      All Rights Reserved.
//
#ifndef VERTEX_CACHE_H

#define VERTEX_CACHE_H

#include "NiStripifyLibType.h"

class NISTRIPIFY_ENTRY VertexCache 
{
    
public:
    
    VertexCache(int size)
    {
        numEntries = size;
        
        entries = new int[numEntries];
        
        for(unsigned int i = 0; i < numEntries; i++)
            entries[i] = -1;
    }
        
    VertexCache() { VertexCache(16); }
    ~VertexCache() { delete[] entries; entries = 0; }
    
    inline bool InCache(int entry)
    {
        bool returnVal = false;
        for(unsigned int i = 0; i < numEntries; i++)
        {
            if(entries[i] == entry)
            {
                returnVal = true;
                break;
            }
        }
        
        return returnVal;
    }
    
    inline int AddEntry(int entry)
    {
        int removed;
        
        removed = entries[numEntries - 1];
        
        //push everything right one
        for(int i = numEntries - 2; i >= 0; i--)
        {
            entries[i + 1] = entries[i];
        }
        
        entries[0] = entry;
        
        return removed;
    }

    inline void Clear()
    {
        memset(entries, -1, sizeof(int) * numEntries);
    }
    
    inline void Copy(VertexCache* inVcache) 
    {
        for(unsigned int i = 0; i < numEntries; i++)
        {
            inVcache->Set(i, entries[i]);
        }
    }

    inline int At(int index) { return entries[index]; }
    inline void Set(int index, int value) { entries[index] = value; }

private:

  int *entries;
  unsigned int numEntries;

};

#endif
