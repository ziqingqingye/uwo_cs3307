/**
Author: Ziqingqing
Description : the header file of class command that encapsulate and abstractly represent the commands
Date: 09/28/2021
*/
#ifndef COMMAND_H
#define COMMAND_H

#include <boost/process.hpp>
#include <string>
#include <iostream>
#include <vector>

class command{
    public:
        command(const std::string& input);       
        ~command();        
        void execute();          
        void cmd_set(const std::string& input);        
        const std::string& cmd_get();       
        const std::string& output_get();       
        int retcode_get();
        
    private:
       std::string cmd;   //the input cmd       
       std::string output;  //the output of the cmd      
       int retcode;  //the return code 
            
};


#endif
