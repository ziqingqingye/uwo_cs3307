/**
Author: Ziqingqing
Description : the implementation cpp file of class command
Date: 09/28/2021
*/
#include "command.h"

/**
Function name: command
Description: constructor
Parameter descriptions: input, the input string
return description: none
*/
command::command(const std::string& input)
{
	 cmd = input;
}

/**
Function name: ~command
Description: destructor
Parameter descriptions: none
return description: none
*/
command::~command()
{
}

/**
Function name: execute
Description: execute the command and get output and return code
Parameter descriptions:none
return description: none
*/
void command:: execute()
{
	boost::process::ipstream pipe_stream;
	try
	{
		//start the process
		boost::process::child c(cmd, (boost::process::std_out & boost::process::std_err) > pipe_stream);
		std::string line;
		while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())  //get output
		{
			output.append(line);
			output.append("\n");
		}
	
		c.wait();	
		retcode = c.exit_code();	//get exit code
        }
        catch (boost::process::process_error& e)
        {
        	output = "process_error caught! Invalid command !\n";
        	retcode = -1;
        }

}

/**
Function name: cmd_set
Description: setter for cmd
Parameter descriptions: input, the input string
return description: none
*/     
void  command:: cmd_set(const std::string& input)
{
	cmd = input;
}


/**
Function name: cmd_get
Description: getter for cmd
Parameter descriptions: none
return description: a const string reference to cmd
*/             
const std::string&  command:: cmd_get()
{
	return cmd;
}

/**
Function name: output_get
Description: getter for output
Parameter descriptions: none
return description: a const string reference to output
*/       
const std::string&  command:: output_get()
{
	return output;
}

/**
Function name: retcode_get
Description: getter for retcode
Parameter descriptions: none
return description: retcode
*/   
int  command:: retcode_get()
{
	return retcode;
}

