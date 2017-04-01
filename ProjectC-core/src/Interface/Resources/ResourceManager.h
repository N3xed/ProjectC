#pragma once
#include "../../UniString.h"
#include "../../Serialization/IByteReader.h"
#include <unordered_map>
#include <memory>
#include <mutex>

namespace ProjectC::Interface {
	class ResourceDelegate {
	public:
		virtual ~ResourceDelegate() {}

		virtual size_t GetSize() = 0;
		virtual const UniString& GetMimeType() const = 0;

		/// <summary>
		/// Loads a resource.
		/// Note: This method gets called as long as bytesRead is less than bytesCount and it returns true.
		/// To indicate an error return false.
		/// To indicate success, set bytesRead to byteCount.
		/// </summary>
		/// <param name="buffer">The buffer, which is of size byteCount (the returned size by ResourceDelegate::GetSize).</param>
		/// <param name="byteCount">The amount of bytes to be copied.</param>
		/// <param name="bytesCopied">The bytes copied. You must use this parameter to track the amount of bytes copied.</param>
		/// <returns><c>true</c> if no error, <c>false</c> if an error ocurred.</returns>
		virtual bool Load(void* buffer, size_t byteCount, size_t& bytesCopied) = 0;

		static ResourceDelegate* CreateFromFile(UniString mimeType, const UniString& path, bool preload = false, bool cache = false);
		static ResourceDelegate* CreateFromBuffer(UniString mimeType, void* data, size_t size);
		static ResourceDelegate* CreateFromBuffer(UniString mimeType, const void* data, size_t size, bool copy);
		static ResourceDelegate* CreateFromString(UniString mimeType, const UniString& data, bool copy);
		static ResourceDelegate* CreateFromString(UniString mimeType, UniString data);

		bool AsString(UniString& str);
	};

	class ResourceManager {
	private:
		static ResourceManager s_instance;

		std::unordered_map<UniString, std::unique_ptr<ResourceDelegate>> m_resourcDelegates;
		std::mutex m_mutex;

		ResourceManager();
	public:
		/// <summary>
		/// Adds the delegate.
		/// </summary>
		/// <param name="key">The key.</param>
		/// <param name="resDelegate">The resource delegate. Note: The ResourceManager takes ownership of the provided ResourceDelegate (except when this method return false).</param>
		/// <returns><c>true</c> when sucessfully added, <c>false</c> if not (ex.: the key isn't unique).</returns>
		bool AddDelegate(UniString key, ResourceDelegate* resDelegate);

		bool AddResourceFromFile(UniString key, UniString mimeType, const UniString& path, bool preload, bool cache = false);

		/// <summary>
		/// Adds a resource from a buffer. The ResourceManager takes ownership of this buffer and deletes it, when this delegate gets removed.
		/// </summary>
		/// <param name="key">The key.</param>
		/// <param name="mimeType">Type of MIME.</param>
		/// <param name="data">The data buffer.</param>
		/// <param name="size">The size in bytes of the buffer.</param>
		/// <returns><c>true</c> when sucessfully added, <c>false</c> if not (ex.: the key isn't unique).</returns>
		bool AddResourceFromBuffer(UniString key, UniString mimeType, void* data, size_t size);
		
		/// <summary>
		/// Adds a resource from a buffer, whether or not the buffer is copied decides |copy|.
		/// Note: If the buffer is not copied, the caller must make sure the buffer is always valid as long as this delegate exists.
		/// </summary>
		/// <param name="key">The key.</param>
		/// <param name="mimeType">Type of MIME.</param>
		/// <param name="data">The data buffer.</param>
		/// <param name="size">The size in bytes of the buffer.</param>
		/// <param name="copy">if set to <c>true</c> |data| gets copied into an internal buffer with size of |size|.</param>
		/// <returns>
		///   <c>true</c> when sucessfully added, <c>false</c> if not (ex.: the key isn't unique).
		/// </returns>
		bool AddResourceFromBuffer(UniString key, UniString mimeType, const void* data, size_t size, bool copy);

		bool AddResourceFromString(UniString key, UniString mimeType, UniString resource);
		bool AddResourceFromString(UniString key, UniString mimeType, const UniString& resource, bool copy);
		
		ResourceDelegate* GetDelegate(const UniString& key);

		static ResourceManager& GetInstance();

		static bool LoadResource(ResourceDelegate& del, void* buffer, size_t size);
		static void LoadResourceAsync(ResourceDelegate& del, void* buffer, size_t size, std::function<void(bool success)> callback);

	private:
		inline bool AddDelegateInternal(UniString key, std::unique_ptr<ResourceDelegate>&& del) {
			std::lock_guard<std::mutex> guard{ m_mutex };
			return m_resourcDelegates.emplace(std::move(key), std::move(del)).second;
		}
	};
}