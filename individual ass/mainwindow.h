/**
Author: Ziqingqing
Description : the header file of class MainWindow
Date: 09/28/2021
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>
#include <QListView>
#include <QStringListModel>
#include <iostream>
#include <vector>
#include "command.h"

namespace Ui {
  class MainWindow;
}
 
class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
private slots:
  void returnPressed();     //for Return pressed in LineEdit
  void showClick(QModelIndex index);  //for click in history window
private:
  std::vector<command> m_cmd;   //to save the command
  QPushButton m_button;     //the execute button
  QLineEdit  m_linetext;        //the input Edit 
  QLabel  m_cmdlabel;        //the "Command" label
  QLabel  m_outputlabel;    //the "Output" label
  QTextEdit m_output;        //the read only TextEdit for output
  QLabel  m_exitlabel;        //the "Exit status:  " label
  QLabel  m_historylabel;    //the "History" label
  QListView m_cmdview;    //the listview for history command
  QStringListModel m_cmdmodel;  // the data model for the listview above
};
#endif // MAINWINDOW_H
