#ifndef __Shell_h
#define __Shell_h

#include <string>
#include <vector>

namespace CMDRES {
	enum CODE {
		kSuccess = 0,
		kError = -1,
		kQuit = 256
	};
}
class Shell;
typedef std::vector<const char*> ArgList;
typedef CMDRES::CODE (*command_handler)( const Shell& shell, const ArgList& );

class Shell
{
public:
	Shell( bool addEchoCommand = true );
	CMDRES::CODE Do( const ArgList& args );

	void RegisterCommand( const std::string& name, command_handler handler );
	void SetPrompt( const char* prompt ) { m_prompt = prompt; }

	virtual CMDRES::CODE Startup( const ArgList& args ) { return CMDRES::kSuccess; };
private:
	std::string m_prompt;
	void Prompt();

	CMDRES::CODE ExecuteCommand( const ArgList& args );

	void PrintHelp();

	struct CommandMapEntry {
		std::string name;
		command_handler handler;
	};
	typedef std::vector<CommandMapEntry> CommandMap;

	CommandMap m_commands;
	command_handler m_catchAllHandler;
};

#endif