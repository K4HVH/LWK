#pragma once

//Includes
#include <iostream>
#include <algorithm>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <winternl.h>
#include <intrin.h>


//Helpers
#define toStr(x) #x
#define FNV(x) IFH::hash::fnv1a<uint32_t>::hash(x)

// Search all Modules until Function found
#define IFH1(fn) IFH::GetProcAddr<decltype(&fn), 0, FNV(toStr(fn)), IFH::hash::strlen_c(toStr(fn))>() 

// Search for function in specific Module
#define IFH2(mod, fn) IFH::GetProcAddr<decltype(&fn), FNV(mod), FNV(toStr(fn)), IFH::hash::strlen_c(toStr(fn))>() 

//Overloading
#define IFH_GET_MACRO(_1,_2,NAME,...) NAME
#define IFH_EXPAND(x) x
#define IFH(...) IFH_EXPAND(IFH_GET_MACRO(__VA_ARGS__, IFH2, IFH1)(__VA_ARGS__))

namespace IFH {

#if defined(_WIN64) //If your IDE/Compiler doesn't define this just use something else to differentiate 64 bit from 32 bit
#define IFH_64
#endif

	//String Hashing
	namespace hash
	{
		template <typename S> struct fnv_internal;
		template <typename S> struct fnv1a;

		template <> struct fnv_internal<uint32_t>
		{
			constexpr static uint32_t default_offset_basis = 0x811C9DC5;
			constexpr static uint32_t prime = 0x01000193;
		};

		template <> struct fnv1a<uint32_t> : public fnv_internal<uint32_t>
		{
			constexpr static inline uint32_t hash(char const* const aString, const uint32_t val = default_offset_basis)
			{
				return (aString[0] == '\0') ? val : hash(&aString[1], (val ^ uint32_t(aString[0])) * prime);
			}

			constexpr static inline uint32_t hash(wchar_t const* const aString, const uint32_t val = default_offset_basis)
			{
				return (aString[0] == L'\0') ? val : hash(&aString[1], (val ^ uint32_t(aString[0])) * prime);
			}
		};

		constexpr size_t strlen_c(const char* str) {
			return *str ? 1 + strlen_c(str + 1) : 0;
		}
	}


	typedef struct _LDR_MODULE {

		LIST_ENTRY              InLoadOrderModuleList;
		LIST_ENTRY              InMemoryOrderModuleList;
		LIST_ENTRY              InInitializationOrderModuleList;
		PVOID                   BaseAddress;
		PVOID                   EntryPoint;
		ULONG                   SizeOfImage;
		UNICODE_STRING          FullDllName;
		UNICODE_STRING          BaseDllName;
		ULONG                   Flags;
		SHORT                   LoadCount;
		SHORT                   TlsIndex;
		LIST_ENTRY              HashTableEntry;
		ULONG                   TimeDateStamp;

	} LDR_MODULE, * PLDR_MODULE;

	template <uint32_t hash>
	constexpr void* GetFn(ULONG64 base) {

		auto DOSH = reinterpret_cast<PIMAGE_DOS_HEADER>(base);
		auto NTH = reinterpret_cast<PIMAGE_NT_HEADERS>((reinterpret_cast<PBYTE>(DOSH) + DOSH->e_lfanew));

		auto EATA = NTH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
		auto EAT = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(EATA + base);

		for (DWORD i = 0; i < EAT->NumberOfFunctions; i++) {
			PULONG FUNCNAME = 0;

			if (i >= EAT->NumberOfNames) {
				break; //Prevent crashes
			}

			FUNCNAME = reinterpret_cast<PULONG>(base + EAT->AddressOfNames + (sizeof(ULONG) * i));

			auto fName = reinterpret_cast<char*>(base + *FUNCNAME);

			if (!fName)
				continue;

			if (FNV(fName) == hash) {
				auto AONP = *reinterpret_cast<PWORD>(base + (EAT->AddressOfNameOrdinals + (i * sizeof(WORD))));
				auto AOF = reinterpret_cast<PULONG>(base + (EAT->AddressOfFunctions + (sizeof(ULONG) * AONP)));

				return reinterpret_cast<void*>(base + *AOF);
			}

		}

		return nullptr;

	}

	template <typename Fn = void*, unsigned int modHash, unsigned int hash, unsigned int len>
	constexpr Fn GetProcAddr() {

		static Fn fn = nullptr;
		if (fn)
			return fn;

		static PPEB peb = nullptr;

		if (!peb) {
#ifdef IFH_64
			PTEB tebPtr = reinterpret_cast<PTEB>(__readgsqword(0x30));
			peb = tebPtr->ProcessEnvironmentBlock;
#else
			peb = reinterpret_cast<PPEB>(__readfsdword(0x30));
#endif
		}

		if (!peb)
			return nullptr;

		auto ldr = peb->Ldr;
		auto p = static_cast<PLDR_MODULE>(ldr->Reserved2[1]);

		while (p->BaseAddress) {
			std::wstring fullname = p->FullDllName.Buffer;
			std::transform(fullname.begin(), fullname.end(), fullname.begin(), ::tolower);

			auto base = reinterpret_cast<ULONG64>(p->BaseAddress);
			std::wstring str = fullname.substr(fullname.length() - len + 1);

			if (modHash == 0 || modHash == FNV(str.c_str())) {
				if (fullname.substr(fullname.length() - 4) != L".exe") {
					auto FN = GetFn<hash>(base);
					if (FN) {
						fn = static_cast<Fn>(FN);
						return fn;
					}
				}
			}

			p = reinterpret_cast<PLDR_MODULE>(p->InLoadOrderModuleList.Flink);
		}
		return nullptr;
	}

}