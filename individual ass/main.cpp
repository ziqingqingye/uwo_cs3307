/**
Author: Ziqingqing
Description : the main cpp file for the program
Date: 09/28/2021
*/
#include <QApplication>
#include "mainwindow.h"

/**
Function name: main
Description: the entry point of the program
Parameter descriptions: argc, number of arguments; argv, the arguments array pointer
return description: the exit code
*/    
int main(int argc, char *argv[]) 
{
  QApplication app(argc, argv);
  MainWindow mainWindow;
  mainWindow.show();
  return app.exec();
}
