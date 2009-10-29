/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Rev: 542 $  // Revision of last commit
  Date: $Date: 2009-08-06 16:08:10 -0400 (Thu, 06 Aug 2009) $  // Date of last commit
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
#include "QGoOpenCreateProjectPage.h"
#include "QueryDataBaseHelper.h"
#include "SelectQueryDatabaseHelper.h"
#include "GoDBRecordSetHelper.h"
#include "GoDBProjectRow.h"
#include "QGoWizardDB.h"
#include "ConvertToStringHelper.h"
#include <QMessageBox>
#include <QRegExp>
#include <QDate>
#include <QTextEdit>
#include <QGridLayout>
#include <QComboBox>


QGoOpenCreateProjectPage::QGoOpenCreateProjectPage( QWidget *iParent )
: QWizardPage( iParent )
{
  QFont tfont;
  tfont.setBold(false);
  this->setFont(tfont);
  m_DatabaseVersion = "Version2";

  CreateProjectRadioButton =
    new QRadioButton(tr("Create a new Project    "));
  OpenProjectRadioButton   =
    new QRadioButton(tr("Open an existing Project"));

  textChoiceProject = new QLabel(tr("Project to open:"));
  ChoiceProject  = new QComboBox;
  textNewProjectName = new QLabel(tr("Name of the Project: "));
  lineNewProjectName = new QLineEdit;
  textDescription = new QLabel(tr("Description:"));
  lineDescription  = new QTextEdit;
  textChoiceAuthor = new QLabel(tr("Choose the name of the author: "));
  ChoiceAuthor = new QComboBox;
  textAuthor = new QLabel (tr("Author of the project:     "));
  lineAuthor = new QLineEdit;
  lineAuthor->setReadOnly(true);

  QVBoxLayout* vlayout = new QVBoxLayout;
  QVBoxLayout* RadioButtonLayout = new QVBoxLayout;
  RadioButtonLayout->addWidget(CreateProjectRadioButton);
  RadioButtonLayout->addWidget(OpenProjectRadioButton);
  vlayout->addLayout(RadioButtonLayout);
  vlayout->setAlignment(RadioButtonLayout,Qt::AlignHCenter);
  QGridLayout* gridlayout = new QGridLayout;
  gridlayout->addWidget(textChoiceProject,0,0);
  gridlayout->addWidget(ChoiceProject,0,1);
  gridlayout->addWidget(textNewProjectName,3,0);
  gridlayout->addWidget(lineNewProjectName,3,1);

  gridlayout->addWidget(textChoiceAuthor,4,0);
  gridlayout->addWidget(ChoiceAuthor,4,1);
  gridlayout->addWidget(textAuthor,5,0);
  gridlayout->addWidget(lineAuthor,5,1);
  gridlayout->addWidget(textDescription,6,0);
  gridlayout->addWidget(lineDescription,6,1);
  vlayout->addLayout(gridlayout);
  setLayout( vlayout );

  registerField( "ProjectName", lineNewProjectName );
  registerField("Author",ChoiceAuthor);

  QObject::connect( this->OpenProjectRadioButton,SIGNAL( clicked() ),
    this,SLOT(ChangeToOpenProjectDisplay()));

  QObject::connect( this->CreateProjectRadioButton,SIGNAL( clicked() ),
  this,SLOT( ChangeToCreateProjectDisplay() ));

  QObject::connect(this->lineDescription,SIGNAL(textChanged()),
  this,SLOT(GetDescription()));

  QObject::connect( this->ChoiceProject,SIGNAL( currentIndexChanged(QString) ),
  this,SLOT(DisplayInfoProject(QString)));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoOpenCreateProjectPage::initializePage()
{
 if (!m_ListProject.isEmpty())
   {
   m_ListProject.clear();
   }
 if (ChoiceProject->count() != 0)
   {
   ChoiceProject->clear();
   }

  OpenDatabaseConnection();

  setSubTitle(
      tr("You are currently using The Database %1 ").arg(
        field("DBName").toString() ) );
  OpenProjectRadioButton->setChecked(false);
  CreateProjectRadioButton->setChecked(true);

  if (!GetListProject())
    {
    ChangeToCreateProjectDisplay();
    OpenProjectRadioButton->setChecked(false);
    OpenProjectRadioButton->hide();
    CreateProjectRadioButton->setChecked(true);
    }
  else
    {
    ChangeToOpenProjectDisplay();
    OpenProjectRadioButton->setChecked(true);
    CreateProjectRadioButton->setChecked(false);
    OpenProjectRadioButton->setVisible(true);
    }

  field( "ProjectName").clear();
  field( "Author" ).clear();

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoOpenCreateProjectPage::GetListProject()const
{
  m_ListProject.clear();
  ChoiceProject->clear();

  std::vector<std::string> vectListProjName =
      ListAllValuesForOneColumn(m_DatabaseConnector,"projectname","project");

   if (!vectListProjName.empty())
    {
    for(unsigned int i = 0; i < vectListProjName.size(); ++i )
      {
      m_ListProject.append( vectListProjName[i].c_str( ) );
      }
    ChoiceProject->addItems(m_ListProject);
    return true;
    }
   else
    {
    return false;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoOpenCreateProjectPage::GetDescription()
{
  QString text;
  int maxChar = 1000;
  int leftChar = 0;
  int sizeText = lineDescription->toPlainText().size();
  leftChar = maxChar-sizeText;
  if (leftChar < 0)
    {
    text = lineDescription->toPlainText().left(maxChar);
    lineDescription->setText(text);
    lineDescription->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
    }
  return lineDescription->toPlainText().toStdString();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoOpenCreateProjectPage::CreateProject()
{
  QDate DateOfToday = QDate::currentDate();

  GoDBProjectRow myNewProject;
  myNewProject.SetField("ProjectName",field( "ProjectName").toString().toStdString());
  myNewProject.SetField("Description",GetDescription());
  myNewProject.SetField("AuthorID",AuthorIDForNewProject());
  myNewProject.SetField("CreationDate",DateOfToday.toString(Qt::ISODate).toStdString());
  myNewProject.SetField("DatabaseVersion",m_DatabaseVersion);

  AddOnlyOneNewObjectInTable< GoDBProjectRow >(m_DatabaseConnector,
    "project", myNewProject );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoOpenCreateProjectPage::AuthorIDForNewProject()
{
  QString Author = ChoiceAuthor->currentText();
  int spaces = Author.count(QRegExp(" "));
  std::stringstream AuthorName;
  AuthorName<<Author.toStdString();
  if (spaces < 2)
    {
    AuthorName<<" <none>";
    }
  return m_MapAuthorIDName[AuthorName.str()];
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QStringList QGoOpenCreateProjectPage::GetListAuthors()
{
  QStringList ListAuthors;
  if (m_DatabaseConnector == 0)
    {
    OpenDatabaseConnection();
    }

  std::vector<std::string> ListFirstNames = ListAllValuesForOneColumn(
      m_DatabaseConnector,"FirstName","author");
  std::vector<std::string> ListMiddleNames = ListAllValuesForOneColumn(
      m_DatabaseConnector,"MiddleName","author");
  std::vector<std::string> ListLastNames = ListAllValuesForOneColumn(
      m_DatabaseConnector,"LastName","author");
  std::vector<std::string> ListID = ListAllValuesForOneColumn(
      m_DatabaseConnector,"AuthorID","author");

  if (!ListFirstNames.empty())
    {
    for (int i=0; i<ListFirstNames.size(); i++)
      {
      std::stringstream AuthorTotalName;
      AuthorTotalName << ListLastNames[i];
      AuthorTotalName << " ";
      AuthorTotalName << ListFirstNames[i];
      AuthorTotalName << " ";
      AuthorTotalName << ListMiddleNames[i];

      m_MapAuthorIDName[AuthorTotalName.str()] = FindOneID(
        m_DatabaseConnector,"author","AuthorID",
        "FirstName",ListFirstNames[i],"MiddleName",ListMiddleNames[i],
        "LastName",ListLastNames[i]);
      }
    for( std::map<std::string, int>::iterator iter = m_MapAuthorIDName.begin();
      iter != m_MapAuthorIDName.end(); ++iter )
      {
      QString AuthorName = (*iter).first.c_str();
      if (AuthorName.contains(QRegExp("<none>")))
        {
        AuthorName.chop(7);
        }
      ListAuthors.append(AuthorName);
      }
    }
  else
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr( "Please enter first the name of your authors in the Database :" ) );
    msgBox.exec();
    }

  return ListAuthors;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoOpenCreateProjectPage::ChangeToCreateProjectDisplay()
{
  setSubTitle(
    tr("When you click on 'Next' the project will be created in the Gofigure Database\n or select 'Open an existing project':"));
  QStringList ListAuthors;

  textChoiceProject->setVisible(false);
  ChoiceProject->setVisible(false);
  textNewProjectName->setVisible(true);
  textNewProjectName->setText(tr("Name of the new Project"));
  lineNewProjectName->clear();
  lineNewProjectName->setVisible(true);
  lineDescription->setReadOnly(false);
  lineDescription->clear();
  textChoiceAuthor->setVisible(true);
  ChoiceAuthor->setVisible(true);
  textAuthor->setVisible(false);
  lineAuthor->setVisible(false);

  if (ListAuthors.isEmpty())
    {ListAuthors = GetListAuthors();
    ChoiceAuthor->addItems(ListAuthors);
    }
  ChoiceAuthor->setVisible(true);
  OpenOrCreateProject = "Create";
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoOpenCreateProjectPage::ChangeToOpenProjectDisplay()
{ 
  setSubTitle(
    tr("Select the project you want to open or choose 'Create a new project':"));

  textChoiceProject->setVisible(true);
  ChoiceProject->setVisible(true);
  textNewProjectName->setVisible(false);
  lineNewProjectName->setVisible(false);
  lineDescription->setReadOnly(true);
  textChoiceAuthor->setVisible(false);
  ChoiceAuthor->setVisible(false);
  textAuthor->setVisible(true);
  lineAuthor->setVisible(true);
  OpenOrCreateProject = "Open";

  if (!m_ListProject.isEmpty())
    {
    DisplayInfoProject(m_ListProject[0]);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoOpenCreateProjectPage::DisplayInfoProject(QString ProjectName)
{

 if (ProjectName != "")
   {
 /*Display the description of the existing project "ProjectName" */

  QString Description;
  
  if (m_DatabaseConnector == 0)
    {
    OpenDatabaseConnection();
    }
  std::vector<std::string> ResultQuery;
  ResultQuery = ListSpecificValuesForOneColumn(
    m_DatabaseConnector,"project", "Description",
    "ProjectName",ProjectName.toStdString());
  /*only one field in the ResultQuery as the project name is the primary key
    of the Project Table: */
  Description = ResultQuery[0].c_str();
  lineDescription->setText(Description);

  /*Display the AuthorName of the exisiting project "ProjectName"
  first, have to find the corresponding AuthorID in the Database :*/
  ResultQuery.clear();
  ResultQuery = ListSpecificValuesForOneColumn(
    m_DatabaseConnector,"project", "AuthorID",
    "ProjectName",ProjectName.toStdString());
  int AuthorID = atoi(ResultQuery[0].c_str());

  /*second, have to find the corresponding AuthorName in the map*/

  if (m_MapAuthorIDName.empty())
    {
    GetListAuthors();
    }

  QString AuthorName;
  for( std::map<std::string, int>::iterator iter = m_MapAuthorIDName.begin();
    iter != m_MapAuthorIDName.end() && AuthorName.isEmpty() ; iter++ )
    {
    if (iter->second == AuthorID)
      {
      AuthorName =(*iter).first.c_str();
      if (AuthorName.contains(QRegExp("<none>")))
        {
        AuthorName.chop(7);
        }
      }
    }
  lineAuthor->setText(AuthorName);
   }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoOpenCreateProjectPage::validatePage()
{
  if (OpenOrCreateProject == "Open")
    {
    setField("ProjectName",ChoiceProject->currentText());
    }
  else
    {
    if (lineNewProjectName->displayText() == "")
      {
      QMessageBox msgBox;
      msgBox.setText( tr( "Please enter a name for your new project." ) );
      msgBox.exec();
      return false;
      }
    if (m_ListProject.contains(field("ProjectName").toString(),Qt::CaseInsensitive))
      {
      QMessageBox msgBox;
      msgBox.setText( tr( "The name you entered for your project already exists." ) );
      msgBox.exec();
      return false;
      }
    if (ChoiceAuthor->currentText() == "")
      {
      QMessageBox msgBox;
      msgBox.setText( tr( "Please select an Author for your project." ) );
      msgBox.exec();
      return false;
      }
    CreateProject();
    }
  if (m_DatabaseConnector == 0 )
    {
    OpenDatabaseConnection();
    }
  ExistingImgSession = DoesProjectHaveExistingImgSession();

  this->CloseDatabaseConnection();
  LeavingPage = true;

  return true;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoOpenCreateProjectPage::cleanupPage()
{
  this->CloseDatabaseConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoOpenCreateProjectPage::nextId() const
{
  if (m_DatabaseConnector == 0 && !LeavingPage)
    {
    BackFromNextPage();
    }

  if (!ExistingImgSession)
    {
    return QGoWizardDB::CreateImgSessionPageID;
    }

  return QGoWizardDB::OpenOrCreateImgSessionPageID;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoOpenCreateProjectPage::OpenDatabaseConnection()const
{
  std::string m_Server = field("ServerName").toString().toStdString();
  std::string m_User = field("User").toString().toStdString();
  std::string m_Password = field("Password").toString().toStdString();
  std::string m_DBName = field("DBName").toString().toStdString();

  std::pair<bool,vtkMySQLDatabase*> ConnectionDatabase = ConnectToDatabase(
  m_Server,m_User,m_Password,m_DBName);

  if (!ConnectionDatabase.first)
    {
    std::cout<<"No connection open for QGoOpenCreateProject"<<std::endl;
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }

  m_DatabaseConnector = ConnectionDatabase.second;
  LeavingPage = false;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoOpenCreateProjectPage::CloseDatabaseConnection()
{
  if (m_DatabaseConnector != 0)
    {
    m_DatabaseConnector->Close();
    m_DatabaseConnector->Delete();
    m_DatabaseConnector = 0;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoOpenCreateProjectPage::DoesProjectHaveExistingImgSession()const
{
  std::vector<std::string> ListImgSessionID = ListSpecificValuesForOneColumn(
    m_DatabaseConnector,"imagingsession","imagingsessionID","projectName",
    field("ProjectName").toString().toStdString());

  return !ListImgSessionID.empty();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoOpenCreateProjectPage::BackFromNextPage()const
{
  this->OpenDatabaseConnection();
  this->GetListProject();
}