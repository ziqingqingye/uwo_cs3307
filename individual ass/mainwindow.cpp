/**
Author: Ziqingqing
Description : the implemented cpp file of class MainWindow
Date: 09/28/2021
*/
#include <QMessageBox>
#include "mainwindow.h"

/**
Function name: MainWindow
Description: constructor
Parameter descriptions: parent, the parent QWidget
return description: none
*/    
MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent), 
  m_button("Execute", this), m_linetext(this), m_cmdlabel("Command:",this), m_outputlabel("Output",this),
  m_output(this),m_exitlabel("Exit status:  ",this),m_historylabel("History", this)
{
  setFixedSize(400,620);  //set the window size
  
  m_cmdlabel.setGeometry(QRect(QPoint(10, 20), QSize(70, 30)));  //place the "Command" label
   
  m_linetext.setGeometry(QRect(QPoint(90, 20), QSize(200, 30)));
  
  // Connect lineedit signal to appropriate slot
  connect(&m_linetext, &QLineEdit::returnPressed, this, &MainWindow::returnPressed);

 m_button.setGeometry(QRect(QPoint(300, 20), QSize(80, 30)));   //place "Execute" button
 
  // Connect button signal to appropriate slot
  connect(&m_button, &QPushButton::released, this, &MainWindow::returnPressed);
  
  m_outputlabel.setGeometry(QRect(QPoint(10, 50), QSize(70, 30)));
  
  m_output.setGeometry(QRect(QPoint(10, 80), QSize(370, 250)));
  m_output.setLineWrapMode(QTextEdit::LineWrapMode::NoWrap);  //don't wrap line
  m_output.setReadOnly(true);  //read only
  
  m_exitlabel.setGeometry(QRect(QPoint(10, 330), QSize(100, 30)));
  
  m_historylabel.setGeometry(QRect(QPoint(10, 360), QSize(100, 30)));
  
  
  m_cmdmodel.setParent(this);
 
  m_cmdview.setParent(this);
  m_cmdview.setGeometry(QRect(QPoint(10, 400), QSize(370, 200)));
  m_cmdview.setModel(&m_cmdmodel);
  m_cmdview.setEditTriggers(QAbstractItemView::NoEditTriggers);   //can't edit list view item
  
  // Connect click signal to appropriate slot
  connect(&m_cmdview, &QListView::clicked, this, &MainWindow::showClick);
}

/**
Function name: ~MainWindow
Description: destructor
Parameter descriptions: none
return description: none
*/    
MainWindow::~MainWindow()
{
 
} 

/**
Function name: returnPressed
Description: the signal process functon for Return pressed signal in the lineEdit and released signal for "Execute" button
Parameter descriptions: none
return description: none
*/    
void MainWindow::returnPressed()
{

	std::string input = m_linetext.text().trimmed().toStdString();   //get the input string and trim it
	if(input == "")    //check empty input
	{  
		QMessageBox::warning(this,"warning","can't execute empty command !" );	
		m_exitlabel.setText("Exit status:  ");
		m_output.setText("");
		return;
        }
       command in(input);  //init one command
       in.execute();  //execute it
       m_exitlabel.setText(QString::fromStdString("Exit status:  "+std::to_string(in.retcode_get())));
       m_cmd.push_back(in);  //save the command 
       
       //update history listview
       int row = m_cmdmodel.rowCount();
       m_cmdmodel.insertRow(row);
       m_cmdmodel.setData(m_cmdmodel.index(row),QString::fromStdString(std::to_string(row+1)+". "+input + " (" +std::to_string(in.retcode_get()) +")" ));
       
       //update output window
       m_output.setText(QString::fromStdString(in.output_get()));
}

/**
Function name: showClick
Description: the signal process functon for clicked signal for listview item
Parameter descriptions: index, the index of item clicked
return description: none
*/    
void MainWindow::showClick(QModelIndex index)
{
	int idx = index.row();	 //get row which is also the index into m_cmd
	
	//populate data into interface
	m_exitlabel.setText(QString::fromStdString("Exit status:  "+std::to_string(m_cmd[idx].retcode_get())));
	m_output.setText(QString::fromStdString(m_cmd[idx].output_get()));
	m_linetext.setText(QString::fromStdString(m_cmd[idx].cmd_get()));
}
