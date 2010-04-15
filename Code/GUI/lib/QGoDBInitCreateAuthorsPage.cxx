/*=========================================================================
  Author: $Author: lydiesouhait $  // Author of last commit
  Version: $Rev: 1150 $  // Revision of last commit
  Date: $Date: 2010-03-30 15:00:47 -0400 (Tue, 30 Mar 2010) $  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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
#include "QGoDBInitCreateAuthorsPage.h"
#include "CreateDataBaseHelper.h"
#include "QGoDBInitializationWizard.h"
#include "vtkSQLQuery.h"
#include "QueryDataBaseHelper.h"
#include "SelectQueryDatabaseHelper.h"
#include <QFormLayout>
#include <QMessageBox>
#include <QVariant>
#include <QInputDialog>
#include <QDir>
#include <QPushButton>


QGoDBInitCreateAuthorsPage::QGoDBInitCreateAuthorsPage( QWidget *iParent)
  : QWizardPage( iParent )
{
  QFont tfont;
  tfont.setBold(false);
  this->setFont(tfont);
  m_DatabaseConnector = 0;
  setSubTitle( tr("Create the authors for the gofigure projects:"));

  QFormLayout* formLayout = new QFormLayout;

  lineLastName = new QLineEdit;
  lineMiddleName = new QLineEdit;
  lineFirstName  = new QLineEdit;
  CreateButton   = new QPushButton(tr("Create Author"));

  formLayout->addRow( tr("Enter the Author FirstName:"),   lineFirstName );
  formLayout->addRow( tr("Enter the Author MiddleName:"),   lineMiddleName );  
  formLayout->addRow( tr("Enter the Author LastName:"),  lineLastName );
  formLayout->addWidget(CreateButton);

  QObject::connect(this->CreateButton,SIGNAL(clicked()),
    this,SLOT(CreateAuthor()));
 
  setLayout( formLayout );
  registerField( "FirstName",  lineFirstName );
  registerField( "MiddleName", lineMiddleName );
  registerField( "LastName",   lineLastName );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoDBInitCreateAuthorsPage::validatePage()
{
  /*if(field("User").toString() == "" ||
       field("Password").toString() == "")
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please fill all the fields.") );
    msgBox.exec();
    return false;
    }
   if(!this->CreateUser())
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("There is a problem with the creation of your user.") );
    msgBox.exec();
    return false;
    }
   if(
    !CreateGoFigureDataBase(this->m_ServerName,field("User").toString().toStdString(),
    field("Password").toString().toStdString(),this->m_DBName))
     {
     QMessageBox msgBox;
     msgBox.setText(
        tr("There is a problem with the creation of your database.") );
     msgBox.exec();
     return false;
     }*/
    return true;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBInitCreateAuthorsPage::CreateAuthor()
{
  this->OpenDBConnection();
  std::string LastName = field("LastName").toString().toStdString();
  std::string FirstName = field("FirstName").toString().toStdString();
  std::string MiddleName = field("MiddleName").toString().toStdString();

  if(FindOneID(this->m_DatabaseConnector,"author", "AuthorID",
    "LastName",LastName,"FirstName",FirstName)!= -1 && MiddleName.empty())
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("There is already an Author with the same lastname and firstname, please enter a middlename") );
    msgBox.exec();
    return;
    }

  if(FindOneID(this->m_DatabaseConnector,"author", "AuthorID",
    "LastName",LastName,"FirstName",FirstName,"MiddleName",MiddleName) != -1 && !MiddleName.empty())
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("This author already exits") );
    msgBox.exec();
    return;
    }

  vtkSQLQuery* query = this->m_DatabaseConnector->GetQueryInstance();
 
  std::stringstream queryScript;
  queryScript << "INSERT INTO author (LastName,FirstName,MiddleName) VALUES ('";
  queryScript << LastName;
  queryScript << "','";
  queryScript << FirstName;
  if (MiddleName == "")
    {
    queryScript << "',default) ;";
    }
  else
    {
    queryScript << "', '";
    queryScript << MiddleName;
    queryScript << "') ;";
    }
  query->SetQuery(queryScript.str().c_str());
  if (!query->Execute())
    {
    std::cout<< "Insert Author query failed."<<std::endl;
    query->Delete();
    return;
    }

  query->Delete();
  QMessageBox msgBox;
    msgBox.setText(
      tr("Your author has been successfully created") );
    msgBox.exec();

  if (CloseDatabaseConnection(m_DatabaseConnector))
    {
    m_DatabaseConnector = 0;
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void QGoDBInitCreateAuthorsPage::OpenDBConnection()
{
  std::string Server = "localhost";
  std::string User = field("User").toString().toStdString();
  std::string Password = field("Password").toString().toStdString();
  std::string DBName = "gofiguredatabase";
  if (this->m_DatabaseConnector == 0)
    {
    m_DatabaseConnector = OpenDatabaseConnection(
      Server,User,Password,DBName);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
