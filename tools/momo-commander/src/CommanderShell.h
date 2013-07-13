#ifndef __CommanderShell_h
#define __CommanderShell_h

#include <string>
#include <vector>

namespace CMDRES {
	enum CODE {
		kSuccess = 0,
		kError = -1,
		kQuit = 256
	};
}
typedef CMDRES::CODE (*command_handler)(int,char**);

class CommanderShell
{
public:
	CommanderShell();
	CMDRES::CODE Do( int argc, char** argv );

	void RegisterCommand( const char* name, command_handler handler );
	void SetPrompt( const char* prompt ) { m_prompt = prompt; }
private:
	std::string m_prompt;
	void Prompt();

	CMDRES::CODE ExecuteCommand( int argc, char** argv );

	struct CommandMapEntry {
		std::string name;
		command_handler handler;
	};
	typedef std::vector<CommandMapEntry> CommandMap;

	CommandMap m_commands;
};

#endif