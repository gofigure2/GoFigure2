/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#include "QGoConnectServerPage.h"
#include "CreateDataBaseHelper.h"
#include "QGoWizardDB.h"

#include "QueryDataBaseHelper.h"
#include <QVariant>
#include <QFormLayout>
#include <QMessageBox>
#include <iostream>
#include <list>

QGoConnectServerPage::QGoConnectServerPage(QWidget *iParent):
  QWizardPage(iParent)
{
  this->m_ImgSessionName.clear();
  this->m_IsAnOpenRecentFile = false;
  QFont tfont;
  tfont.setBold(false);
  this->setFont(tfont);
  setSubTitle( tr("Connect to a MySQL DataBase Server:") );

  QFormLayout *formLayout = new QFormLayout;
  lineServerName = new QLineEdit( tr("localhost") );
  lineUserName = new QLineEdit( tr("gofigure") );
  linePassword = new QLineEdit( tr("gofigure") );
  linePassword->setEchoMode(QLineEdit::Password);
  linePassword->displayText();
  lineDBName = new QLineEdit;

  formLayout->addRow(tr("&ServerName:"), lineServerName);
  formLayout->addRow(tr("&User:"),       lineUserName);
  formLayout->addRow(tr("&Password:"),   linePassword);
  setLayout(formLayout);

  setLayout(formLayout);

  registerField("ServerName", lineServerName);
  registerField("User",       lineUserName);
  registerField("Password",   linePassword);
  registerField("DBName",     lineDBName);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoConnectServerPage::~QGoConnectServerPage()
{
  delete lineDBName;
  // make sure vtkMySQLDatabase* has been deleted
  CloseServerConnection();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoConnectServerPage::validatePage()
{
  if ( field("ServerName").toString() == ""
       || field("User").toString() == ""
       || field("Password").toString() == "" )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please fill all the fields.") );
    msgBox.exec();
    return false;
    }
  this->OpenConnectionToServer();
  if ( !m_ConnectionServer.first )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Unable to connect to the server: please make sure you entered the right fields.") );
    msgBox.exec();
    return false;
    }
  std::list< std::string > ListGoDB = ListGofigureDatabases();
  if ( ListGoDB.empty() )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr(
        "You have not yet set up your Gofigure Database\nPlease go to the menu 'Settings' and select 'Set Up Database'.") );
    msgBox.exec();
    emit NoGofigureDatabase();
    return false;
    }

  if ( ListGoDB.size() > 1 )
    {
    std::cout << "There is more than one Gofigure DataBase" << std::endl;
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return false;
    }

  std::list< std::string >::iterator i = ListGoDB.begin();
  std::string                        DBName = *i;
  this->wizard()->setField( "DBName", DBName.c_str() );
  std::cout << "the db name to open is: " << field("DBName").toString().toStdString().c_str() << std::endl;
  return true;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoConnectServerPage::nextId() const
{
  if ( !this->m_IsAnOpenRecentFile )
    {
    return QGoWizardDB::OpenOrCreateProjectPageID;
    }
  else
    {
    return -1;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< std::string > QGoConnectServerPage::ListGofigureDatabases() const
{
  //Get the list of all the existing databases:
  this->OpenConnectionToServer();
  std::vector< std::string > vectListDB = ListDatabases(m_ConnectionServer.second);
  CloseServerConnection();

  /*For each existing database, check if they are of Gofigure Type, if so, put them
  in the ListGoDB*/
  std::list< std::string > ListGoDB;
  for ( unsigned int i = 0; i < vectListDB.size(); ++i )
    {
    //First, create the connection to the database named vectListDB[i]and check
    // it is open:
    std::pair< bool, vtkMySQLDatabase * > DatabaseConnection = ConnectToDatabase(
      field("ServerName").toString().toStdString(),
      field("User").toString().toStdString(),
      field("Password").toString().toStdString(), vectListDB[i]);
    if ( !DatabaseConnection.first )
      {
      std::cout << "Cannot check if " << vectListDB[i].c_str()
                << " is of\
                  Gofigure Type"
                <<
      std::endl;
      std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
      std::cout << std::endl;
      }
    //test if it is of GofigureType:
    if ( IsDatabaseOfGoFigureType(DatabaseConnection.second) )
      {
      ListGoDB.push_back(vectListDB[i]);
      }
    DatabaseConnection.second->Close();
    DatabaseConnection.second->Delete();
    }
  return ListGoDB;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoConnectServerPage::CloseServerConnection() const
{
  if ( m_ConnectionServer.second )
    {
    m_ConnectionServer.second->Close();
    m_ConnectionServer.second->Delete();
    m_ConnectionServer.second = 0;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoConnectServerPage::OpenConnectionToServer() const
{
  if ( this->m_ConnectionServer.second == 0 )
    {
    m_ConnectionServer = ConnectToServer(
      field("ServerName").toString().toStdString(),
      field("User").toString().toStdString(),
      field("Password").toString().toStdString() );
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoConnectServerPage::SetImgSessionName(std::string iImgSessionName)
{
  this->m_ImgSessionName = iImgSessionName;
  this->m_IsAnOpenRecentFile = true;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoConnectServerPage::SetIsAnOpenRecentFile(bool iIsAnOpenRecentFile)
{
  this->m_IsAnOpenRecentFile = iIsAnOpenRecentFile;
}
