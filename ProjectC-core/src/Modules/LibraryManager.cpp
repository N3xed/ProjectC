#include "LibraryManager.h"

ProjectC::Modules::DynamicLibrary& ProjectC::Modules::LibraryManager::AddLibrary(DynamicLibrary&& library)
{
	m_libraries.push_front(std::move(library));
	return m_libraries.front();
}

ProjectC::Modules::DynamicLibrary& ProjectC::Modules::LibraryManager::AddLibrary(const std::string& path)
{
	m_libraries.push_front(DynamicLibrary(path));
	return m_libraries.front();
}

ProjectC::Modules::DynamicLibrary& ProjectC::Modules::LibraryManager::AddLibrary(UnicodeString& path)
{
	m_libraries.push_front(DynamicLibrary(path));
	return m_libraries.front();
}

bool ProjectC::Modules::LibraryManager::RemoveLibrary(const UnicodeString& name)
{
	bool success = false;
	std::string utf8String = StringUtils::ToUTF8(name);
	m_libraries.remove_if([&utf8String, &success](DynamicLibrary& e) {
		if (e.GetName() == utf8String) {
			success = true;
			return true;
		}
		return false;
	});
	return success;
}

bool ProjectC::Modules::LibraryManager::RemoveLibrary(const std::string& name)
{
	bool success = false;
	m_libraries.remove_if([&name, &success](DynamicLibrary& e) {
		if (e.GetName() == name) {
			success = true;
			return true;
		}
		return false;
	});
	return success;
}

bool ProjectC::Modules::LibraryManager::RemoveLibrary(const DynamicLibrary& library)
{
	bool success = false;
	m_libraries.remove_if([&library, &success](DynamicLibrary& e) {
		if (&e == &library) {
			success = true;
			return true;
		}
		return false;
	});
	return success;
}

const std::forward_list<ProjectC::Modules::DynamicLibrary>& ProjectC::Modules::LibraryManager::GetLibraries() const noexcept
{
	return m_libraries;
}