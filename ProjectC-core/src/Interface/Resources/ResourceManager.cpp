#include "ResourceManager.h"
#include <fstream>
#include <boost/filesystem.hpp>
#include "../../App.h"


namespace ProjectC::Interface::Detail {
	class PreloadFileResourceDelegate : public ResourceDelegate {
		uint8_t* m_data;
		size_t m_dataSize;
		UniString m_mimeType;
	public:
		PreloadFileResourceDelegate(const UniString& filePath, UniString mimeType) : m_mimeType(mimeType) {
			if (!boost::filesystem::exists(filePath.ToWString()))
				throw std::exception(StringUtils::Concatenate("File '", filePath, "' not found.").c_str());
			m_dataSize = static_cast<size_t>(boost::filesystem::file_size(filePath.ToWString()));
			m_data = new uint8_t[m_dataSize];
			boost::filesystem::ifstream fs{ filePath.ToWString(), std::ios::in | std::ios::binary };
			if (fs.good()) {
				fs.read(reinterpret_cast<char*>(m_data), m_dataSize);
			}
		}
		~PreloadFileResourceDelegate() {
			delete[] m_data;
		}
		virtual size_t GetSize() override { return m_dataSize; }
		virtual const UniString& GetMimeType() const override { return m_mimeType; }
		virtual bool Load(void* buffer, size_t byteCount, size_t& bytesRead) override {
			if (!m_data)
				return false;

			std::memcpy(buffer, m_data, byteCount);
			bytesRead = byteCount;
			return true;
		}
	};
	class FileResourceDelegate : public ResourceDelegate {
		UniString m_mimeType;
		UniString m_path;
		uint8_t* m_data;
		size_t m_dataSize;
		bool m_cache;
	public:
		FileResourceDelegate(const UniString& filePath, UniString mimeType, bool cache) : m_mimeType(mimeType), m_path(filePath), m_cache(cache) { 
			if (!boost::filesystem::exists(m_path.ToWString()))
				throw std::exception(StringUtils::Concatenate("File '", filePath, "' not found.").c_str());
		}
		~FileResourceDelegate() {
			delete[] m_data;
		}
		virtual size_t GetSize() override {
			if (m_cache && m_dataSize != 0)
				return m_dataSize;
			return m_dataSize = static_cast<size_t>(boost::filesystem::file_size(m_path.ToWString()));
		}
		virtual const UniString& GetMimeType() const override { return m_mimeType; }
		virtual bool Load(void* buffer, size_t byteCount, size_t& bytesRead) override {
			try {
				if (!m_data || !m_cache) {
					delete[] m_data;
					m_data = new uint8_t[m_dataSize];

					boost::filesystem::ifstream fs{ m_path.ToWString(), std::ios::in | std::ios::binary };
					if (fs.good()) {
						fs.read(reinterpret_cast<char*>(m_data), m_dataSize);
					}
					else {
						LOG_WARN("Filestream not good.");
						return false;
					}
				}
				std::memcpy(buffer, m_data, byteCount);
				bytesRead = byteCount;
				return true;
			}
			catch (const std::exception& ex) {
				LOG_WARN("Exception thrown: ", ex.what());
			}
			return false;
		}
	};
	class BufferedResourceDelegate : public ResourceDelegate {
	protected:
		size_t m_dataSize;
		void* m_data;
		UniString m_mimeType;
	protected:
		BufferedResourceDelegate(UniString mimeType) : m_mimeType(mimeType) {}
	public:
		BufferedResourceDelegate(void* data, size_t dataSize, UniString mimeType) : m_mimeType(mimeType), m_dataSize(dataSize), m_data(data) {}
		virtual ~BufferedResourceDelegate() {}
		virtual size_t GetSize() override { return m_dataSize; }
		virtual const UniString& GetMimeType() const override { return m_mimeType; }
		virtual bool Load(void* buffer, size_t byteCount, size_t& bytesRead) override {
			if (!m_data)
				return false;
			std::memcpy(buffer, m_data, m_dataSize);
			bytesRead = m_dataSize;
			return false;
		}
	};
	class OwnedBufferedResourceDelegate : public BufferedResourceDelegate {
	public:
		OwnedBufferedResourceDelegate(void* data, size_t dataSize, UniString mimeType) : BufferedResourceDelegate(data, dataSize, mimeType) {}
		virtual ~OwnedBufferedResourceDelegate() {
			delete[] m_data;
		}
	};
	class CopiedBufferedResourceDelegate : public BufferedResourceDelegate {
	public:
		virtual ~CopiedBufferedResourceDelegate() {
			delete[] m_data;
		}
		CopiedBufferedResourceDelegate(const void* data, size_t dataSize, UniString mimeType) : BufferedResourceDelegate(mimeType) {
			m_data = new uint8_t[dataSize];
			std::memcpy(m_data, data, dataSize);
			m_dataSize = dataSize;
		}
	};

	class StringResourceDelegate : public ResourceDelegate {
	protected:
		UniString m_data;
		UniString m_mimeType;
	public:
		StringResourceDelegate(UniString data, UniString mimeType) : m_data(data), m_mimeType(mimeType) {}

		virtual size_t GetSize() override {
			return m_data.GetStruct()->length * sizeof(UniString::char_type);
		}
		virtual const UniString& GetMimeType() const override {
			return m_mimeType;
		}
		virtual bool Load(void* buffer, size_t byteCount, size_t& bytesRead) override {
			std::memcpy(buffer, m_data.GetStruct()->str, byteCount);
			bytesRead = byteCount;
			return true;
		}
	};
}

ProjectC::Interface::ResourceDelegate* ProjectC::Interface::ResourceDelegate::CreateFromFile(UniString mimeType, const UniString& path, bool preload /*= false*/, bool cache /*= false*/)
{
	std::unique_ptr<ResourceDelegate> result;
	try {
		switch (preload) {
		case true:
			result = std::make_unique<Detail::PreloadFileResourceDelegate>(path, mimeType);
		case false:
			result = std::make_unique<Detail::FileResourceDelegate>(path, mimeType, cache);
		}
		return result.release();
	}
	catch (const std::exception& ex) {
		LOG_WARN(ex.what());
		return nullptr;
	}
}

ProjectC::Interface::ResourceDelegate* ProjectC::Interface::ResourceDelegate::CreateFromBuffer(UniString mimeType, const void* data, size_t size, bool copy)
{
	switch (copy) {
	case true:
		return new Detail::CopiedBufferedResourceDelegate(data, size, mimeType);
	case false:
		return new Detail::BufferedResourceDelegate(const_cast<void*>(data), size, mimeType);
	}
	return nullptr;
}

ProjectC::Interface::ResourceDelegate* ProjectC::Interface::ResourceDelegate::CreateFromBuffer(UniString mimeType, void* data, size_t size)
{
	return new Detail::OwnedBufferedResourceDelegate(data, size, mimeType);
}

ProjectC::Interface::ResourceDelegate* ProjectC::Interface::ResourceDelegate::CreateFromString(UniString mimeType, const UniString& data, bool copy)
{
	return new Detail::StringResourceDelegate(UniString{ data.GetStruct()->str, data.GetStruct()->length, copy }, mimeType);
}

ProjectC::Interface::ResourceDelegate* ProjectC::Interface::ResourceDelegate::CreateFromString(UniString mimeType, UniString data)
{
	return new Detail::StringResourceDelegate(data, mimeType);
}

bool ProjectC::Interface::ResourceDelegate::AsString(UniString& str)
{
	size_t size = GetSize();
	std::unique_ptr<char[]> buf{ new char[size] };

	if (ResourceManager::LoadResource(*this, buf.get(), size)) {
		str.FromString(std::string{ buf.get(), size });
		return true;
	}
	return false;
}

ProjectC::Interface::ResourceManager ProjectC::Interface::ResourceManager::s_instance;

ProjectC::Interface::ResourceManager::ResourceManager()
{ }

bool ProjectC::Interface::ResourceManager::AddDelegate(UniString key, ResourceDelegate* resDelegate)
{
	std::lock_guard<std::mutex> guard{ m_mutex };
	return m_resourcDelegates.emplace(std::move(key), std::move(std::unique_ptr<ResourceDelegate>(resDelegate))).second;
}


bool ProjectC::Interface::ResourceManager::AddResourceFromFile(UniString key, UniString mimeType, const UniString& path, bool preload, bool cache)
{
	if (ResourceDelegate* del = ResourceDelegate::CreateFromFile(mimeType, path, preload, cache)) {
		return AddDelegateInternal(key, std::unique_ptr<ResourceDelegate>(del));
	}
	return false;
}

bool ProjectC::Interface::ResourceManager::AddResourceFromBuffer(UniString key, UniString mimeType, void* data, size_t size)
{
	return AddDelegateInternal(
		key,
		std::unique_ptr<ResourceDelegate>(ResourceDelegate::CreateFromBuffer(mimeType, data, size))
	);
}


bool ProjectC::Interface::ResourceManager::AddResourceFromBuffer(UniString key, UniString mimeType, const void* data, size_t size, bool copy)
{
	return AddDelegateInternal(
		key,
		std::unique_ptr<ResourceDelegate>(ResourceDelegate::CreateFromBuffer(mimeType, data, size, copy))
	);
}

bool ProjectC::Interface::ResourceManager::AddResourceFromString(UniString key, UniString mimeType, UniString resource)
{
	return AddDelegateInternal(
		key,
		std::unique_ptr<ResourceDelegate>(ResourceDelegate::CreateFromString(mimeType, resource))
	);
}

bool ProjectC::Interface::ResourceManager::AddResourceFromString(UniString key, UniString mimeType, const UniString& resource, bool copy)
{
	return AddDelegateInternal(
		key,
		std::unique_ptr<ResourceDelegate>(ResourceDelegate::CreateFromString(mimeType, resource, copy))
	);
}

ProjectC::Interface::ResourceDelegate* ProjectC::Interface::ResourceManager::GetDelegate(const UniString& key)
{
	std::lock_guard<std::mutex> guard{ m_mutex };
	auto it = m_resourcDelegates.find(key);
	if (it != m_resourcDelegates.end()) {
		return it->second.get();
	}
	return nullptr;
}

ProjectC::Interface::ResourceManager& ProjectC::Interface::ResourceManager::GetInstance()
{
	return s_instance;
}

bool ProjectC::Interface::ResourceManager::LoadResource(ResourceDelegate& del, void* buffer, size_t size)
{
	size_t bytesCopied = 0;
	while (bytesCopied < size && del.Load(buffer, size, bytesCopied)) {}
	if (bytesCopied < size || bytesCopied > size)
		return false;
	return true;
}

void ProjectC::Interface::ResourceManager::LoadResourceAsync(ResourceDelegate& del, void* buffer, size_t size, std::function<void(bool success)> callback)
{
	throw std::logic_error("Not implemented.");
}


