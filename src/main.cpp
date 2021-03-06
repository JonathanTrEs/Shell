#include <iostream>
#include <string>

#include <cli/callbacks.hpp>
#include <cli/cli.hpp>
#include <cli/parsers.hpp>

#include <errno.h>      // errno
#include <fcntl.h>      // open()
#include <stdlib.h>     // exit(), setenv(), ...
#include <string.h>     // strerror()
#include <sys/types.h>  // waitpid()
#include <sys/wait.h>   // waitpid(), open()
#include <sys/stat.h>   // open()
#include <unistd.h>     // exec(), fork(), close(), dup2(), pipe(), ...

const char INTRO_TEXT[] = "                       Jonathan Trujillo Estévez\n";
const char PROMPT_TEXT[] = "$ ";

bool exitCommandCallback(const std::string& command, cli::parser::shellparser::CommandArguments const& arguments)
{
    return true;
}

int fds[2]={-1,-1};
int tuberia = -1;
int stdPrev = -1;

bool defaultCommandCallback(const std::string& command, cli::parser::shellparser::CommandArguments const& arguments)
{
  	using namespace cli::auxiliary;
	using namespace cli::parser::shellparser;
//    	std::cout << command << ": ";
//    	std::cout << arguments << std::endl;

	if (arguments.terminator == CommandArguments::PIPED)
	{
		   std::cout <<" pipe 1 " << arguments << std::endl;
		   int fds [2]={-1,-1};
		   stdPrev=-1;
		   int aux = pipe(fds);
		   if ( aux == 0)
		   {	      		     
		      tuberia=1;
		   } else 
		     std::cerr << program_invocation_short_name << "pipe: " << strerror(errno) << std::endl;
	 }
//Tenemos la tuberia en el caso de que se encuentre un pipe
	pid_t childPid = fork();
	if (childPid == 0) {            // Proceso hijo

	//Redirecciones
	typedef std::vector<StdioRedirection>::const_iterator RedirectionIter;
        for (RedirectionIter iter = arguments.redirections.begin(); iter < arguments.redirections.end(); ++iter)
        {
            int mode = S_IRUSR | S_IWUSR |      // u+rw
                       S_IRGRP | S_IWGRP |      // g+rw
                       S_IROTH | S_IWOTH;       // o+rw

            if (iter->type == StdioRedirection::TRUNCATED_OUTPUT)
	    {
                int fd = open(iter->argument.c_str(), O_CREAT | O_TRUNC | O_WRONLY, mode);
                dup2(fd, 1);    // El fd 1 se cierra antes de duplicar
                close(fd);
            } 
            else if (iter->type == StdioRedirection::TRUNCATED_INPUT)
	    {
                int fd = open(iter->argument.c_str(), O_RDONLY | O_SYNC  , mode);
                dup2(fd, 0); //mira lo que tiene fd y lo copia en 0
                close(fd);
	    } 
	    else if (iter->type == StdioRedirection::APPENDED_OUTPUT)
	    {
                int fd = open(iter->argument.c_str(), O_RDWR | O_CREAT | O_APPEND  , mode);
                dup2(fd, 1);
                close(fd);                
	    }
	}

	if (tuberia==1) 
	{
	  std::cout <<" pipe  2" << arguments << std::endl;
	  stdPrev = 1;
	  dup2(fds[0],0);
	  close(fds[0]);
	}
        	
	if (stdPrev) 
	{
		 stdPrev = 0;
		 dup2(fds[1],1);
		 close(fds[1]);
		 tuberia = 0;
	}
		    	
	boost::shared_array<char*> argv = stdVectorStringToArgV(arguments.arguments);
        execvp(argv[0], argv.get());
	
        std::cerr << program_invocation_short_name << ": execvp: " << strerror(errno) << std::endl;
        exit(-1);
    }
    else if (childPid > 0 )
    {       // Proceso padre

        // esperar a que el proceso hijo termine
        if (arguments.terminator == CommandArguments::NORMAL)
	{
            waitpid(childPid, NULL, 0);
        }
    }
    else 
    {
        std::cerr << program_invocation_short_name << ": fork: " << strerror(errno) << std::endl;
    }
    return false;
}

// Main function
int main(int argc, char** argv)
{
    // Create the interpreter object with the ShellParser parser
    cli::CommandLineInterpreter<cli::parser::ShellParser> interpreter;

    // Set the intro and prompt texts
    interpreter.setIntroText(INTRO_TEXT);
    interpreter.setPromptText(PROMPT_TEXT);

    // Set the callback function that will be invoked when the user inputs
    // the 'exit' command
    interpreter.setCallback<cli::callback::DoCommandCallback>(
        &exitCommandCallback, "exit");

    // Set the callback function that will be invoked when the user inputs
    // any other command
    interpreter.setCallback<cli::callback::DoCommandCallback>(
        &defaultCommandCallback);

    // Run the interpreter
    interpreter.loop();

    return 0;
}
