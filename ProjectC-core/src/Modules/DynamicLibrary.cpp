#include "DynamicLibrary.h"



ProjectC::Modules::DynamicLibrary::DynamicLibrary(const std::string& filePath)
{
	const wchar_t* wideString = UnicodeString::fromUTF8(filePath).getTerminatedBuffer();
	wchar_t fname[_MAX_FNAME];
	wchar_t extension[_MAX_EXT];
	_wsplitpath(wideString, NULL, NULL, fname, extension);
	m_name = StringUtils::ToUTF8(UnicodeString(fname) + UnicodeString(extension));

	m_libraryHandle = LoadLibraryW(wideString);
	if (m_libraryHandle == NULL)
		throw std::exception(("Could not load library: " + filePath).c_str(), GetLastError());
}

ProjectC::Modules::DynamicLibrary::DynamicLibrary(UnicodeString& filePath)
{
	const wchar_t* wideString = filePath.getTerminatedBuffer();
	wchar_t fname[_MAX_FNAME];
	wchar_t extension[_MAX_EXT];
	_wsplitpath(wideString, NULL, NULL, fname, extension);
	m_name = StringUtils::ToUTF8(UnicodeString(fname) + UnicodeString(extension));

	m_libraryHandle = LoadLibraryW(wideString);
	if (m_libraryHandle == NULL)
		throw std::exception(StringUtils::ToUTF8("Could not load library: " + filePath).c_str(), GetLastError());
}

ProjectC::Modules::DynamicLibrary::DynamicLibrary(DynamicLibrary&& obj)
{
	m_libraryHandle = obj.m_libraryHandle;
	obj.m_libraryHandle = NULL;
}

void* ProjectC::Modules::DynamicLibrary::GetFunction(std::string name) const noexcept
{
	assert(m_libraryHandle != NULL);
	return GetProcAddress(m_libraryHandle, name.c_str());
}

ProjectC::Modules::DynamicLibrary::~DynamicLibrary()
{
	FreeLibrary(m_libraryHandle);
}

const std::string& ProjectC::Modules::DynamicLibrary::GetName() const noexcept
{
	return m_name;
}
