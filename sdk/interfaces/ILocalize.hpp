#pragma once

#include "IAppSystem.hpp"
#include "..\misc\vfunc.hpp"

class ILocalizeTextQuery
{
public:
	virtual int ComputeTextWidth(const wchar_t* pString) = 0;
};

class ILocalizationChangeCallback
{
public:
	virtual void OnLocalizationChanged() = 0;
};

class ILocalize : public IAppSystem
{
public:
	virtual bool					AddFile(const char* fileName, const char* pPathID = nullptr, bool bIncludeFallbackSearchPaths = false) = 0;
	virtual void					RemoveAll() = 0;
	virtual wchar_t*				Find(const char* tokenName) = 0;
	virtual const wchar_t*			FindSafe(const char* tokenName) = 0;
	virtual int						ConvertANSIToUnicode(const char* ansi, wchar_t* unicode, int unicodeBufferSizeInBytes) = 0;
	virtual int						ConvertUnicodeToANSI(const wchar_t* unicode, char* ansi, int ansiBufferSize) = 0;
	virtual unsigned	FindIndex(const char* tokenName) = 0;
	virtual void					ConstructString(wchar_t* unicodeOuput, int unicodeBufferSizeInBytes, const wchar_t* formatString, int numFormatParameters, ...) = 0;
	virtual const char*				GetNameByIndex(unsigned index) = 0;
	virtual wchar_t*				GetValueByIndex(unsigned index) = 0;
};

struct IBaseFileSystem
{

	int Read(void* pOutput, int size, void* file)
	{
		using Fn = int(__thiscall*)(void*, void*, int, void*);
		return call_virtual<Fn>(this, 0)(this, pOutput, size, file);
	}

	// if pathID is NULL, all paths will be searched for the file
	void* Open(const char* pFileName, const char* pOptions, const char* pathID = nullptr)
	{
		using Fn = void* (__thiscall*)(void*, const char*, const char*, const char*);
		return call_virtual<Fn>(this, 2)(this, pFileName, pOptions, pathID);
	}

	void Close(void* file)
	{
		using Fn = void(__thiscall*)(void*, void*);
		return call_virtual<Fn>(this, 3)(this, file);
	}

	unsigned int Size(void* file)
	{
		using Fn = unsigned int(__thiscall*)(void*, void*);
		return call_virtual<Fn>(this, 7)(this, file);
	}
};