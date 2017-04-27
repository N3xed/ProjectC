#include "DynamicLibrary.h"
#include "../Utils/String.h"
#include "../Exception.hpp"


ProjectC::Modules::DynamicLibrary::DynamicLibrary(const UniString& filePath)
{
	std::wstring wstr = filePath.ToWString();

	const wchar_t* str = wstr.c_str();
	wchar_t fname[_MAX_FNAME];
	wchar_t extension[_MAX_EXT];
	_wsplitpath(str, NULL, NULL, fname, extension);
	m_name = std::wstring{ fname } + std::wstring{ extension };

	m_libraryHandle = LoadLibraryW(str);
	if (m_libraryHandle == NULL)
		throw Exception(StringUtils::Concatenate<char>("Could not load library: ", filePath.ToString(), ": ", GetLastError()));
}


ProjectC::Modules::DynamicLibrary::DynamicLibrary(DynamicLibrary&& obj)
{
	m_libraryHandle = obj.m_libraryHandle;
	obj.m_libraryHandle = NULL;
}

void* ProjectC::Modules::DynamicLibrary::GetFunction(const UniString& name) const noexcept
{
	assert(m_libraryHandle != NULL);
	return GetProcAddress(m_libraryHandle, name.ToString().c_str());
}

ProjectC::Modules::DynamicLibrary::~DynamicLibrary()
{
	FreeLibrary(m_libraryHandle);
}
