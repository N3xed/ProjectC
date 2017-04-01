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
		bool found = false;
		for (auto& e : commands) {
			for (auto& name : e.names) {
				if (name == cmdName) {
					found = true;
					if (args.size() >= 2 && UniString{ args[1].String, args[1].Length, false } == "/info") {
						console.Write(e.description);
						console.Write("\n");
					}
					else
						e.callback(console, args);
				}
				if (found)
					break;
			}
			if (found)
				break;
		}
		return found;
	});
}

