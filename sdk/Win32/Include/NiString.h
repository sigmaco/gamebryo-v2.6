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

#ifndef NISTRING_H
#define NISTRING_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include <NiFixedString.h>

class NiPoint2;
class NiPoint3;
class NiPoint4;
class NiColor;
class NiColorA;

// A class that implements a smart character string.
class NIMAIN_ENTRY NiString : public NiMemObject
{
public:
    // Construct an NiString using the input buffer size.
    NiString(size_t stBuffLength = 256);

    NiString(int iBuffLength);

    // Construct an NiString from a single char
    NiString(char ch);

    // Construct an NiString from an input string. The input string is copied
    // into an internal buffer
    NiString(const char* pcStr);

    // Copy constructor. The internal string buffer's refcount is 
    // incremented, it is NOT copied
    NiString(const NiString& kString);

    // Construct an NiString from an NiFixedString. The input string is copied
    // into an internal buffer
    NiString(const NiFixedString& kStr);

    // Destroys an NiString. The refcount of the internal string buffer is
    // decremented. If the refcount is now zero, the string buffer is deleted.
    ~NiString();

    enum
    {
        INVALID_INDEX = (unsigned int) -1
    };

    // Gets the length of the string
    unsigned int Length() const;
    
    // Tests whether a NiString object contains no characters.
    bool IsEmpty() const;

    // Forces a string to have 0 length. 
    void Empty(); 
    
    // Returns the character at a given position. 
    char GetAt(unsigned int ui) const;
    
    // Sets a character at a given position
    void SetAt(unsigned int ui, char c);
    
    // This method compares pNiString lexicographically to the current
    // string and returns a value indicating their relationship.
    int Compare(const char* pNiString) const;

    // This method lexicographically compares lowercase versions of the 
    // current string and pNiString and returns a value indicating their 
    // relationship.
    int CompareNoCase(const char* pNiString) const;

    // Does this string exactly equal the input string?
    bool Equals(const char* pcStr) const ;

    // Does this string exactly equal the input string ignoring case?
    bool EqualsNoCase(const char* pcStr) const;

    // Get a separate copy of a substring from uiBegin up to uiEnd
    NiString GetSubstring(unsigned int uiBegin, unsigned int uiEnd) const;

    // Get a separate copy of a substring from uiBegin up to the first 
    // character found in pcDelimiters
    NiString GetSubstring(unsigned int uiBegin, 
        const char* pcDelimeters) const;
    
    // Find a character within this string and return its index
    unsigned int Find(char c, unsigned int uiStart = 0) const;

    // Find a substring within this string and return its index
    unsigned int Find(const char* pcStr, unsigned int uiStart = 0) const;

    // Is the input character string a substring of this string
    bool Contains(const char* pcStr) const;
    bool ContainsNoCase(const char* pcStr) const;

    // Find a character within this string, starting from the end and working
    // backwards, and return its index
    unsigned int FindReverse(char c) const;
    // Find a character within this string, starting from the uiSearchEnd 
    // and working backwards, and return its index
    unsigned int FindReverse(char c, unsigned int uiStartIdx) const;

    // Find a character within this string from the set of characters in pcStr
    // and return its index 
    unsigned int FindOneOf(const char* pcStr, unsigned int uiStart = 0) const;

    // Extracts the middle part of a string 
    NiString Mid(unsigned int uiBegin, 
        unsigned int uiCount = INVALID_INDEX) const;

    // Extracts the left part of a string 
    NiString Left( unsigned int uiCount ) const;

    // Extracts the right part of a string
    NiString Right( unsigned int uiCount ) const;

    // Replace all exact matches of pcWhatToReplace in this string with
    // pcReplaceWith
    void Replace(const char* pcWhatToReplace , const char* pcReplaceWith);

    // Insert the input string at the specified index, shifting all subsequent
    // characters to the right. If the index is >= the original string length,
    // the insertion string is concatenated. The address of the insertion 
    // string cannot be within the address space of the internal string buffer.
    void Insert(const char* pcInsertString, unsigned int uiIndex);

    // Remove the substring specified
    void RemoveRange(unsigned int uiStartIdx, unsigned int uiNumChars);

    /// Format the String buffer according to the standard printf string
    void Format(const char* pcFormatString ...);

    // Convert the string to uppercase internally
    void ToUpper();
    // Convert the string to lowercase internally
    void ToLower();
    // Reverse the string internally
    void Reverse();

    // Trim this character from the beginning of the string
    void TrimLeft(char c = ' ');
    // Trim this character from the end of the string
    void TrimRight(char c = ' ');

    // Add the char* to the end of this string, 
    // growing the buffer as necessary.
    void Concatenate(const char* pcStr);

    // Add the char to the end of this string, 
    // growing the buffer as necessary.
    void Concatenate(char ch);

    // Convert the string into a boolean. True if successful, false if not.
    bool ToBool(bool& b) const;
    bool To(bool& b) const;
    // Convert the string into a float. True if successful, false if not.
    bool ToFloat(float& f) const;
    bool To(float& f) const;
    // Convert the string into an NiInt32. True if successful, false if not.
    bool ToInt(NiInt32& i) const;
    bool To(NiInt32& i) const;
    bool To(NiInt16& i) const;
    bool To(NiInt8& i) const;
    // Convert the string into an NiUInt32. True if successful, false if not.
    bool ToUInt(NiUInt32& ui) const;
    bool To(NiUInt32& ui) const;
    bool To(NiUInt16& ui) const;
    bool To(NiUInt8& ui) const;
    // Convert the string into an NiPoint2. True if successful, false if not.
    bool ToPoint2(NiPoint2& kPoint) const;
    bool To(NiPoint2& kPoint) const;
    // Convert the string into an NiPoint3. True if successful, false if not.
    bool ToPoint3(NiPoint3& kPoint) const;
    bool To(NiPoint3& kPoint) const;
    // Convert the string into an NiPoint4. True if successful, false if not.
    bool ToPoint4(NiPoint4& kPoint) const;
    bool To(NiPoint4& kPoint) const;
    // Convert the string into an NiColor. True if successful, false if not.
    bool ToRGB(NiColor& kColor) const;
    bool To(NiColor& kColor) const;
    // Convert the string into an NiColorA. True if successful, false if not.
    bool ToRGBA(NiColorA& kColor) const;
    bool To(NiColorA& kColor) const;

    // Returns the NiString representation of this object.
    static NiString FromBool(bool b);
    static NiString From(bool b);
    // Returns the NiString representation of this object.
    static NiString FromFloat(float f);
    static NiString From(float f);
    // Returns the NiString representation of this object.
    static NiString FromInt(NiInt32 i);
    static NiString From(NiInt32 i);
    static NiString From(NiInt16 i);
    static NiString From(NiInt8 i);
    // Returns the NiString representation of this object.
    static NiString FromUInt(NiUInt32 ui);
    static NiString From(NiUInt32 ui);
    static NiString From(NiUInt16 ui);
    static NiString From(NiUInt8 ui);
    // Returns the NiString representation of this object.
    static NiString FromPoint2(NiPoint2& kPt);
    static NiString From(NiPoint2& kPt);
    // Returns the NiString representation of this object.
    static NiString FromPoint3(NiPoint3& kPt);
    static NiString From(NiPoint3& kPt);
    // Returns the NiString representation of this object.
    static NiString FromPoint4(NiPoint4& kPt);
    static NiString From(NiPoint4& kPt);
    // Returns the NiString representation of this object.
    static NiString FromRGB(NiColor& kColor);
    static NiString From(NiColor& kColor);
    // Returns the NiString representation of this object.
    static NiString FromRGBA(NiColorA& kColor);
    static NiString From(NiColorA& kColor);

    // return pointer to const string
    operator const char*() const;

    // Ref-counted copy from another NiString
    NiString& operator=(NiString& stringSrc);

    // copy string content from char*
    NiString& operator=(const char* psz);

    NiString& operator=(const NiFixedString& kString);
    // set string content to single character
    NiString& operator=(char ch);

    // concatenate from another NiString
    NiString& operator+=(NiString& string);

    // Concatenate a single character
    NiString& operator+=(char ch);

    // Concatenate a const char*
    NiString& operator+=(const char* pcString);

    NiString& operator+=(const NiFixedString& kString);

    // Create a new string that is the composite of String 1 and String 2
    friend NiString NIMAIN_ENTRY operator+(const NiString& string1,
        const NiString& string2);

    // Create a new string that is the composite of String 1 and a character
    friend NiString NIMAIN_ENTRY operator+(const NiString& string, 
        char ch);
    
    // Create a new string that is the composite of a character and String 2
    friend NiString NIMAIN_ENTRY operator+(char ch, 
        const NiString& string);
    
    // Create a new string that is the composite of String 1 and a char*
    friend NiString NIMAIN_ENTRY operator+(const NiString& string, 
        const char* lpsz);

    friend NiString NIMAIN_ENTRY operator+(const NiString& string, 
        const NiFixedString& kString2);

    // Create a new string that is the composite of a char* and String 2
    friend NiString NIMAIN_ENTRY operator+(const char* lpsz, 
        const NiString& string);

    friend NiString NIMAIN_ENTRY operator+(const NiFixedString& 
        kString, const NiString& string);

    // create a character array that is not managed by this NiString
    char* MakeExternalCopy();

protected:
    typedef char* StringHandle;

    StringHandle m_kHandle;

    static StringHandle Allocate(size_t stStrLength);
    static StringHandle AllocateAndCopy(const char* pcString);
    static StringHandle AllocateAndCopyHandle(StringHandle kHandle);
    static void Deallocate(StringHandle& kHandle);

    static void IncRefCount(StringHandle& kHandle, bool bValidate = true);
    static void DecRefCount(StringHandle& kHandle, bool bValidate = true);
    static void Swap(StringHandle& kHandle, const char* pcNewValue,
        bool bValidate = true);
    static char* GetString(const StringHandle& kHandle, 
        bool bValidate = true);
    static size_t GetLength(const StringHandle& kHandle, 
        bool bValidate = true);
    static size_t GetRefCount(const StringHandle& kHandle, 
        bool bValidate = true);
    static size_t GetAllocationSize(const StringHandle& kHandle, 
        bool bValidate = true);
    static size_t GetBufferSize(const StringHandle& kHandle, 
        bool bValidate = true);
    static void SetLength(const StringHandle& kHandle, size_t stLength);
    static char* GetRealBufferStart(const StringHandle& kHandle);
    static bool ValidateString(const StringHandle& kHandle);
    static unsigned int GetBestBufferSize(unsigned int uiReqSize);

    bool Resize(unsigned int uiDelta);
    void SetBuffer(StringHandle& kHandle);
    
    void CalcLength();

    // Called before any operation that changes the string buffer.
    // If this string is the only one holding on the the buffer, a copy
    // is not made. Otherwise, a copy is made.
    void CopyOnWrite(bool bForceCopy = false);
    void CopyOnWriteAndResize(unsigned int uiSizeDelta, 
        bool bForceCopy = false);

    char* ConsumeWhitespace(const char* pcStr, unsigned int uiLength) const;

    /// Internal implementation of vformat
    StringHandle vformat(const char *fmt, va_list argPtr);
};

// Do these two strings exactly match?
bool  NIMAIN_ENTRY operator==(const NiString& s1, const NiString& s2);
// Do these two strings exactly match?
bool  NIMAIN_ENTRY operator==(const NiString& s1, const char* s2);
// Do these two strings exactly match?
bool  NIMAIN_ENTRY operator==(const NiString& s1, const NiFixedString& s2);
// Do these two strings exactly match?
bool  NIMAIN_ENTRY operator==(const char* s1, const NiString& s2);
// Do these two strings exactly match?
bool  NIMAIN_ENTRY operator==(const NiFixedString& s1, const NiString& s2);
// Do these two strings not match?
bool  NIMAIN_ENTRY operator!=(const NiString& s1, const NiString& s2);
// Do these two strings not match?
bool  NIMAIN_ENTRY operator!=(const NiString& s1, const char* s2);
// Do these two strings not match?
bool  NIMAIN_ENTRY operator!=(const NiString& s1, 
    const NiFixedString& s2);
bool  NIMAIN_ENTRY operator!=(const char* s1, const NiString& s2);
bool  NIMAIN_ENTRY operator!=(const NiFixedString& s1, 
    const NiString& s2);
// Is s1 less than s2?
bool  NIMAIN_ENTRY operator<(const NiString& s1, const NiString& s2);
// Is s1 less than s2?
bool  NIMAIN_ENTRY operator<(const NiString& s1, const char* s2);
// Is s1 less than s2?
bool  NIMAIN_ENTRY operator<(const NiString& s1, 
    const NiFixedString& s2);
bool  NIMAIN_ENTRY operator<(const char* s1, const NiString& s2);
bool  NIMAIN_ENTRY operator<(const NiFixedString& s1, 
    const NiString& s2);
// Is s1 greater than s2?
bool  NIMAIN_ENTRY operator>(const NiString& s1, const NiString& s2);
// Is s1 greater than s2?
bool  NIMAIN_ENTRY operator>(const NiString& s1, const char* s2);
// Is s1 greater than s2?
bool  NIMAIN_ENTRY operator>(const NiString& s1, 
    const NiFixedString& s2);
bool  NIMAIN_ENTRY operator>(const char* s1, const NiString& s2);
bool  NIMAIN_ENTRY operator>(const NiFixedString& s1, 
    const NiString& s2);
// Is s1 less than or equal to s2?
bool  NIMAIN_ENTRY operator<=(const NiString& s1, const NiString& s2);
// Is s1 less than or equal to s2?
bool  NIMAIN_ENTRY operator<=(const NiString& s1, const char* s2);
// Is s1 less than or equal to s2?
bool  NIMAIN_ENTRY operator<=(const NiString& s1, 
    const NiFixedString& s2);
bool  NIMAIN_ENTRY operator<=(const char* s1, const NiString& s2);
bool  NIMAIN_ENTRY operator<=(const NiFixedString& s1, 
    const NiString& s2);
// Is s1 greater than or equal to s2?
bool  NIMAIN_ENTRY operator>=(const NiString& s1, const NiString& s2);
// Is s1 greater than or equal to s2?
bool  NIMAIN_ENTRY operator>=(const NiString& s1, const char* s2);
// Is s1 greater than or equal to s2?
bool  NIMAIN_ENTRY operator>=(const NiString& s1, 
    const NiFixedString& s2);
bool  NIMAIN_ENTRY operator>=(const char* s1, const NiString& s2);
bool  NIMAIN_ENTRY operator>=(const NiFixedString& s1, 
    const NiString& s2);

bool NIMAIN_ENTRY IsTrue(NiString strStr);
#endif  // #ifndef NISTRING_H
