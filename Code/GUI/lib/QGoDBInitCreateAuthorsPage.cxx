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
#include "QGoDBInitCreateAuthorsPage.h"
#include "CreateDataBaseHelper.h"
#include "QGoDBInitializationWizard.h"
#include "vtkSQLQuery.h"
#include "QueryDataBaseHelper.h"
#include "SelectQueryDatabaseHelper.h"
#include "GoDBAuthorRow.h"
#include <QFormLayout>
#include <QMessageBox>
#include <QVariant>
#include <QInputDialog>
#include <QDir>
#include <QPushButton>

QGoDBInitCreateAuthorsPage::QGoDBInitCreateAuthorsPage(QWidget *iParent):
  QWizardPage(iParent)
{
  QFont tfont;

  tfont.setBold(false);
  this->setFont(tfont);
  m_DatabaseConnector = 0;
  setSubTitle( tr("Create the authors for the gofigure projects:") );

  QFormLayout *formLayout = new QFormLayout;

  lineLastName = new QLineEdit;
  lineMiddleName = new QLineEdit;
  lineFirstName  = new QLineEdit;
  CreateButton   = new QPushButton( tr("Create Author") );

  formLayout->addRow(tr("Enter the Author FirstName:"),   lineFirstName);
  formLayout->addRow(tr("Enter the Author MiddleName:"),   lineMiddleName);
  formLayout->addRow(tr("Enter the Author LastName:"),  lineLastName);
  formLayout->addWidget(CreateButton);

  QObject::connect( this->CreateButton, SIGNAL( clicked() ),
                    this, SLOT( CreateAuthor() ) );

  setLayout(formLayout);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoDBInitCreateAuthorsPage::validatePage()
{
  this->OpenDBConnection();

  if ( ListAllValuesForOneColumn(this->m_DatabaseConnector,
                                 "AuthorID", "author").empty() )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please create at least one author for your project.") );
    msgBox.exec();
    if ( CloseDatabaseConnection(m_DatabaseConnector) )
      {
      m_DatabaseConnector = 0;
      }
    return false;
    }
  if ( CloseDatabaseConnection(m_DatabaseConnector) )
    {
    m_DatabaseConnector = 0;
    }
  return true;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBInitCreateAuthorsPage::CreateAuthor()
{
  this->OpenDBConnection();
  QMessageBox msgBox;
  std::string LastNameValue = lineLastName->text().toStdString();
  std::string FirstNameValue = lineFirstName->text().toStdString();
  std::string MiddleNameValue =  lineMiddleName->text().toStdString();

  if (FirstNameValue.empty() || LastNameValue.empty() )
    {
    msgBox.setText(
      tr("Please enter at least the lastname and the firstname of your author") );
    msgBox.exec();
    return;
    }

  std::vector<FieldWithValue> Conditions;
  FieldWithValue FirstName ("FirstName",FirstNameValue);
  FieldWithValue LastName ("LastName",LastNameValue);
  Conditions.push_back(FirstName);
  Conditions.push_back(LastName);
  
  if( FindOneID(this->m_DatabaseConnector,"author", "AuthorID",Conditions) != -1 && MiddleNameValue.empty() )
    {
    msgBox.setText(
      tr("There is already an Author with the same lastname and firstname, please enter a middlename") );
    msgBox.exec();
    return;
    }

  GoDBAuthorRow NewAuthor;
  NewAuthor.SetField("FirstName", lineFirstName->text().toStdString() );
  NewAuthor.SetField("LastName", lineLastName->text().toStdString() );
  NewAuthor.SetField("MiddleName",lineMiddleName->text().toStdString() );

  if (NewAuthor.DoesThisAuthorAlreadyExists(this->m_DatabaseConnector) != -1)
    {
     msgBox.setText(
      tr("This author already exists") );
    msgBox.exec();
    return;
    }
  NewAuthor.SaveInDB(this->m_DatabaseConnector) ;
 
  msgBox.setText(
    tr("Your author has been successfully created") );
  msgBox.exec();
  emit NewAuthorCreated();

  if ( CloseDatabaseConnection(m_DatabaseConnector) )
    {
    m_DatabaseConnector = 0;
    }
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void QGoDBInitCreateAuthorsPage::OpenDBConnection()
{
  if ( this->m_User.empty() || this->m_Password.empty() )
    {
    this->SetDatabaseVariables( field("User").toString().toStdString(),
                                field("Password").toString().toStdString() );
    }
  if ( this->m_DatabaseConnector == 0 )
    {
    m_DatabaseConnector = OpenDatabaseConnection(
      this->m_Server, this->m_User, this->m_Password, this->m_DBName);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBInitCreateAuthorsPage::SetDatabaseVariables(
  std::string iUser, std::string iPassword)
{
  this->m_User = iUser;
  this->m_Password = iPassword;
  this->m_Server = "localhost";
  this->m_DBName = "gofiguredatabase";
}
