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
#include "QGoDBInitCreateMicroscopePage.h"
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


QGoDBInitCreateMicroscopePage::QGoDBInitCreateMicroscopePage( QWidget *iParent)
  : QWizardPage( iParent )
{
  QFont tfont;
  tfont.setBold(false);
  this->setFont(tfont);
  m_DatabaseConnector = 0;
  setSubTitle( tr("Create the microscopes for the gofigure projects:"));

  QFormLayout* formLayout = new QFormLayout;

  lineMicroscopeName = new QLineEdit;
  CreateButton   = new QPushButton(tr("Create Microscope"));

  formLayout->addRow( tr("Enter the Microscope Name:"),   lineMicroscopeName );
  formLayout->addWidget(CreateButton);

  QObject::connect(this->CreateButton,SIGNAL(clicked()),
    this,SLOT(CreateMicroscope()));

  setLayout( formLayout );
  //registerField( "MicroscopeName",  lineMicroscopeName );

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoDBInitCreateMicroscopePage::validatePage()
{
  this->OpenDBConnection();
  QMessageBox msgBox;
  if( ListAllValuesForOneColumn(this->m_DatabaseConnector,
      "Name", "microscope").empty())
    {
    msgBox.setText(
      tr("Please create at least one microscope.") );
    msgBox.exec();
    if (CloseDatabaseConnection(m_DatabaseConnector))
        {
        m_DatabaseConnector = 0;
        }
    return false;
    }
  if (CloseDatabaseConnection(m_DatabaseConnector))
    {
    m_DatabaseConnector = 0;
    }
  msgBox.setText( QObject::tr( "The MySql user and the GoFigure Database \n\
    have been successfully created !!\n\
    Now you can save the work you do with GoFigure into the Database !!" ) );
      msgBox.exec();
  return true;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBInitCreateMicroscopePage::CreateMicroscope()
{
  this->OpenDBConnection();
  QMessageBox msgBox;
  //std::string MicroscopeName = field("MicroscopeName").toString().toStdString();
  std::string MicroscopeName = this->lineMicroscopeName->text().toStdString();
  if(MicroscopeName.empty())
    {
    msgBox.setText(
      tr("Please enter a name for your microscope.") );
    msgBox.exec();
    return;
    }

  if(ListSpecificValuesForOneColumn(this->m_DatabaseConnector,"microscope",
      "Name","Name",MicroscopeName).size()>0)
      {
      msgBox.setText(
        tr("There is already a Microscope with the same name, please choose another one") );
      msgBox.exec();
      return;
      }

  vtkSQLQuery* query = this->m_DatabaseConnector->GetQueryInstance();
    std::stringstream queryScript;
    queryScript << "INSERT INTO microscope VALUES ('";
    queryScript << MicroscopeName;
    queryScript << "') ;";
  
    query->SetQuery(queryScript.str().c_str());
    if (!query->Execute())
      {
      std::cout<< "Insert Microscope query failed."<<std::endl;
      query->Delete();
      return;
      }
    query->Delete();
    msgBox.setText(
      tr("Your microscope has been successfully created") );
      msgBox.exec();
    emit NewMicroscopeCreated();

  if (CloseDatabaseConnection(m_DatabaseConnector))
    {
    m_DatabaseConnector = 0;
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void QGoDBInitCreateMicroscopePage::OpenDBConnection()
{
  if(this->m_User.empty() || this->m_Password.empty())
    {
    this->SetDatabaseVariables(field("User").toString().toStdString(),
      field("Password").toString().toStdString());
    }
  if (this->m_DatabaseConnector == 0)
    {
    m_DatabaseConnector = OpenDatabaseConnection(
      this->m_Server,this->m_User,this->m_Password,this->m_DBName);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBInitCreateMicroscopePage::SetDatabaseVariables(
  std::string iUser,std::string iPassword)
{
  this->m_User = iUser;
  this->m_Password = iPassword;
  this->m_Server = "localhost";
  this->m_DBName = "gofiguredatabase";
}