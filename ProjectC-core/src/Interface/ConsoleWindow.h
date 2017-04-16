#pragma once
#include "../InvocationList.h"
#include "../HandlerList.h"
#include "../Utils/String.h"
#include "../Logging.h"


namespace ProjectC::Interface {
	class ConsoleWindow {
	public:
		typedef std::vector<StringUtils::SubStringInfo<UniString::char_type>> SubStringVector;

		typedef HandlerList<bool(ConsoleWindow& console, const SubStringVector& cmd)> CommandReceivedHandlerList;
		typedef InvocationList<void(ConsoleWindow& console)> CloseListenerList;

		typedef std::function<bool(ConsoleWindow& console, const SubStringVector& args)> CommandCallback;

	public:
		class CommandInfo {
		public:
			const std::vector<UniString> names;
			const UniString description;
			const CommandCallback callback;
			CommandInfo(std::vector<UniString> _names, UniString _description, CommandCallback _callback) : names(_names), description(_description), callback(_callback)
			{}
		};

	protected:
		CommandReceivedHandlerList m_commandReceivedListeners;
		CloseListenerList m_closeListeners;
	
		std::vector<CommandInfo> m_commands;
		UniString m_pinnedCommand;
	public:
		ConsoleWindow();
		virtual ~ConsoleWindow() {}

		virtual CommandReceivedHandlerList& OnCommandReceived() { return m_commandReceivedListeners; }
		virtual CloseListenerList& OnClose() { return m_closeListeners; }

		virtual bool IsShown() const = 0;
		virtual void Show(bool show = true) = 0;
		virtual void Hide() { Show(false); }
		virtual void Close() = 0;

		virtual void Write(const UniString& str, uint32_t fontCol = 0xffffff) = 0;

		/// <summary>
		/// Adds the command.
		/// Note: The first element of the args is always the command name itself.
		/// This method is not thread safe.
		/// </summary>
		/// <param name="names">The names.</param>
		/// <param name="info">The info, which is available via: [command] /info</param>
		/// <param name="callback">The callback, whitch gets executed on the ConsoleWindow thread.</param>
		void AddCommand(std::vector<UniString> names, UniString info, CommandCallback callback);
		/// <summary>
		/// Adds the command.
		/// Note: The first element of the args is always the command name itself.
		/// This method is not thread safe.
		/// </summary>
		/// <param name="names">The names.</param>
		/// <param name="info">The info, which is available via: [command] /info</param>
		/// <param name="callback">The callback, whitch gets executed on the ConsoleWindow thread.</param>
		void AddCommand(UniString names, UniString info, CommandCallback callback);

		/// <summary>
		/// Gets the commands.
		/// Note: This method is not thread safe.
		/// </summary>
		/// <returns>All added commands.</returns>
		std::vector<CommandInfo>& GetCommands() { return m_commands; }

		void PinCommand(const UniString& cmdName) {
			m_pinnedCommand = cmdName;
		}
		void UnpinCommand() {
			m_pinnedCommand.clear();
		}

		static ConsoleWindow* Create(const UniString& title, const UniString& delimiter = " ");
	};
}