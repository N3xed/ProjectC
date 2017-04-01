#include "DynamicLibrary.h"


ProjectC::Modules::DynamicLibrary::DynamicLibrary(const UniString& filePath)
{
	std::wstring wstr = filePath.ToWString();

	const wchar_t* str = wstr.c_str();
	wchar_t fname[_MAX_FNAME];
	wchar_t extension[_MAX_EXT];
	_wsplitpath(str, NULL, NULL, fname, extension);
	m_name = std::string();

	m_libraryHandle = LoadLibraryW(str);
	if (m_libraryHandle == NULL)
		throw std::exception(("Could not load library: " + filePath.ToString()).c_str(), GetLastError());
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
