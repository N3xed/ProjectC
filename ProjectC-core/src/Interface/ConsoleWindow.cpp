#include "ConsoleWindow.h"

#ifdef OS_WIN 
#include "Detail/Platform/ConsoleWindowWin.h"
ProjectC::Interface::ConsoleWindow* ProjectC::Interface::ConsoleWindow::Create(const UniString& title, const UniString& delimiter /*= " "*/)
{
	return new Detail::ConsoleWindowWin(title, delimiter);
}
#else
#error No implementation for this OS.
ProjectC::Interface::ConsoleWindow* ProjectC::Interface::ConsoleWindow::Create(const UniString& title, const UniString& delimiter /*= " "*/)
{
	throw std::logic_error("No implementation");
}
#endif

void ProjectC::Interface::ConsoleWindow::AddCommand(std::vector<UniString> names, UniString info, CommandCallback callback)
{
	m_commands.emplace_back(names, info, callback);
}

void ProjectC::Interface::ConsoleWindow::AddCommand(UniString name, UniString info, CommandCallback callback)
{
	m_commands.emplace_back(std::vector<UniString>{ name }, info, callback);
}

ProjectC::Interface::ConsoleWindow::ConsoleWindow()
{
	OnCommandReceived().Add([](ConsoleWindow& console, const std::vector<StringUtils::SubStringInfo<UniString::char_type>>& args) {
		if (args.size() == 0)
			return false;
		std::vector<CommandInfo>& commands = console.GetCommands();
		UniString cmdName{ args[0].String, args[0].Length, false };
		
		for (auto& e : commands) {
			for (auto& name : e.names) {
				if (name == cmdName) {
					if (args.size() >= 2) {
						UniString arg1{ args[1].String, args[1].Length, false };
						if (arg1 == "/info") {
							console.Write(e.description);
							console.Write("\n");
							return true;
						}
						else if (arg1 == "/pin") {
							console.PinCommand(cmdName);
							console.Write("Pinned command: ");
							console.Write(cmdName);
							console.Write("\n");
							return true;
						}
						else if (arg1 == "/unpin") {
							console.UnpinCommand();
							console.Write("Unpinned command.\n");
							return true;
						}
					}
					if (!e.callback(console, args)) {
						console.Write("[Info] Command failed.\n");
					}
					return true;
				}
			}
		}
		return false;
	});
}

