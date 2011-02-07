/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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
#include "QGoWizardDB.h"

#include "QGoCreateDataBasePage.h"
#include "QGoOpenCreateProjectPage.h"
#include "QGoOpenCreateImgSessionPage.h"
#include "QGoCreateImgSessionPage.h"
#include "CreateDataBaseHelper.h"
#include "QueryDataBaseHelper.h"
#include "SelectQueryDatabaseHelper.h"
#include "GoDBRecordSet.h"
#include "GoDBRecordSetHelper.h"
#include "itkMegaCaptureImport.h"
#include "GoFigureFileInfoMultiIndexContainerHelper.h"

#include <iostream>
#include <QVariant>
#include <QFormLayout>
#include <QRect>
#include <QFileDialog>
#include <QPushButton>
#include <string>
#include <QLabel>
#include <Qt>
#include <QMessageBox>
#include <QFont>
#include <QPushButton>
#include <QCheckBox>
#include <QCloseEvent>

//-------------------------------------------------------------------------
QGoWizardDB::QGoWizardDB(QWidget *iParent):
  QWizard(iParent)
{
  this->m_ImgSessionName = "";
  this->m_ImgSessionID = 0;
  this->m_IsAnOpenRecentFile = false;
  QFont tfont;
  tfont.setBold(true);
  this->setFont(tfont);

  QFont font2;
  font2.setBold(false);

  nextButton = new QPushButton( tr("Next") );
  nextButton->setFont(font2);
  this->setButton (QWizard::NextButton, nextButton);
  QPushButton *backButton = new QPushButton( tr("Back") );
  backButton->setFont(font2);
  this->setButton (QWizard::BackButton, backButton);
  this->setOptions(QWizard::NoCancelButton);
  QPushButton *finishButton = new QPushButton( tr("Finish") );
  finishButton->setFont(font2);
  this->setButton (QWizard::FinishButton, finishButton);
  this->m_ConnectServerPage = new QGoConnectServerPage;
  setPage(ConnectServerPageID, this->m_ConnectServerPage);
  //setPage(CreateDataBasePageID, new QGoCreateDataBasePage);
  setPage(OpenOrCreateProjectPageID, new QGoOpenCreateProjectPage);
  setPage(OpenOrCreateImgSessionPageID, new QGoOpenCreateImgSessionPage);
  setPage(CreateImgSessionPageID, new QGoCreateImgSessionPage);
  setWindowTitle( tr("Use DataBase") );
  QObject::connect( this->m_ConnectServerPage, SIGNAL( NoGofigureDatabase() ),
                    this, SLOT( hide() ) );
  QObject::connect( this->m_ConnectServerPage, SIGNAL( NoGofigureDatabase() ),
                    this, SIGNAL( NoGofigureDatabase() ) );
  QObject::connect( this->m_ConnectServerPage, SIGNAL( GofigureDatabaseExists() ),
                    this, SIGNAL ( GofigureDatabaseExists() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector< std::vector< std::string > > QGoWizardDB::GetFilenamesFromDB()
{
  std::vector< std::vector< std::string > > oFilenames;
  //first, open a connection to the database:
  std::string Server = field("ServerName").toString().toStdString();
  std::string User = field("User").toString().toStdString();
  std::string Password = field("Password").toString().toStdString();
  std::string DBName = field("DBName").toString().toStdString();

  std::pair< bool, vtkMySQLDatabase * > ConnectionDatabase = ConnectToDatabase(
    Server, User, Password, DBName);

  if ( !ConnectionDatabase.first )
    {
    std::cout << "No connection open for QGoOpenOrCreateImgSession" << std::endl;
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }

  vtkMySQLDatabase *DatabaseConnector = ConnectionDatabase.second;

  //Get the number of channels with their id as a map
  //ListChannelsIDNumber[channelID]=ChannelNumber:
  std::vector<std::string> ChannelAttributes (2);
  ChannelAttributes[0] = "channelID";
  ChannelAttributes[1] = "ChannelNumber";

  std::map< std::string, std::string > ListChannelsIDNumber =
    MapTwoColumnsFromTable( DatabaseConnector, ChannelAttributes,"channel",
    "ImagingSessionID", field("ImgSessionID").toString().toStdString() );

 // std::map< std::string, std::string > ListChannelsIDNumber =
  //  MapTwoColumnsFromTable( DatabaseConnector, "channelID", "ChannelNumber",
   //                         "channel", "ImagingSessionID", field("ImgSessionID").toString().toStdString() );

  std::map< std::string, std::string >::iterator it = ListChannelsIDNumber.begin();
  oFilenames.resize( ListChannelsIDNumber.size() );
  int i = 0;
  while ( it != ListChannelsIDNumber.end() )
    {
    std::string ChannelID = it->first;
    //get the filenames of all the images corresponding to ChannelID:
    oFilenames[i] = ListSpecificValuesForOneColumn(
      DatabaseConnector, "image", "Filename", "channelID", ChannelID);
    it++;
    i++;
    }

  //close the database connection:
  DatabaseConnector->Close();
  DatabaseConnector->Delete();
  return oFilenames;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QGoWizardDB::GetNameDB()
{
  return field("NameDB").toString();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoWizardDB::GetImagingSessionID()
{
  if ( field("ImgSessionID").toInt() != 0 )
    {
    return field("ImgSessionID").toInt();
    }
  if ( this->m_ImgSessionID != 0 )
    {
    return this->m_ImgSessionID;
    }
  return 0;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QGoWizardDB::GetImagingSessionName()
{
  if ( this->m_ImgSessionName.empty() )
    {
    return field("ImgSessionName").toString();
    }
  else
    {
    return this->m_ImgSessionName.c_str();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QGoWizardDB::GetServer()
{
  return field("ServerName").toString();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QGoWizardDB::GetLogin()
{
  return field("User").toString();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QGoWizardDB::GetPassword()
{
  return field("Password").toString();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoWizardDB::closeEvent(QCloseEvent *iEvent)
{
  int          CurrentPageID = this->currentId();
  QWizardPage *CurrentPage = this->currentPage();

  if ( !field("ImgSessionName").toString().toStdString().empty() )
    {
    this->m_ImgSessionName =
      field("ImgSessionName").toString().toStdString();
    this->m_ImgSessionID = 0;
    }
  if ( !field("DBName").toString().toStdString().empty() )
    {
    this->SetFirstFileName();
    }
  this->m_ImgSessionName.clear();
  this->m_IsAnOpenRecentFile = false;
  //this->restart();
  switch ( CurrentPageID )
    {
    case 0:
      {
      //QGoConnectServerPage* ServerPage =
      // dynamic_cast<QGoConnectServerPage*>(CurrentPage);
      //ServerPage->m_ConnectionServer.second->Close();
      //ServerPage->m_ConnectionServer.second->Delete();
      //delete ServerPage;
      break;
      }
    case 2:
      {
      QGoOpenCreateProjectPage *ProjectPage = dynamic_cast< QGoOpenCreateProjectPage * >( CurrentPage );
      if ( ProjectPage->m_DatabaseConnector )
        {
        ProjectPage->m_DatabaseConnector->Close();
        ProjectPage->m_DatabaseConnector = 0;
        }
      break;
      }
    case 3:
      {
      QGoOpenCreateImgSessionPage *ImgSessionPage = dynamic_cast< QGoOpenCreateImgSessionPage * >( CurrentPage );
      if ( ImgSessionPage->m_DatabaseConnector )
        {
        ImgSessionPage->m_DatabaseConnector->Close();
        ImgSessionPage->m_DatabaseConnector = 0;
        }
      break;
      }
    case 4:
      {
      QGoCreateImgSessionPage *CreateImgSessionPage = dynamic_cast< QGoCreateImgSessionPage * >( CurrentPage );
      if ( CreateImgSessionPage->m_DatabaseConnector )
        {
        CreateImgSessionPage->m_DatabaseConnector->Close();
        CreateImgSessionPage->m_DatabaseConnector = 0;
        }
      break;
      }
    default:
      {
      break;
      }
    }

/*  QGoOpenCreateImgSessionPage* F = dynamic_cast<QGoOpenCreateImgSessionPage*>(CurrentPage);
  if (F !=0)
    {
    F->m_DatabaseConnector->Close();
    F->m_DatabaseConnector->Delete();
    }
  delete F;*/
  iEvent->accept();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoFigureFileInfoHelperMultiIndexContainer
QGoWizardDB::GetMultiIndexFileContainer()
{
  QGoCreateImgSessionPage *img_page =
    dynamic_cast< QGoCreateImgSessionPage * >( this->page(CreateImgSessionPageID) );

  if ( img_page )
    {
    return img_page->GetMultiIndexFileContainer();
    }
  else
    {
    return GoFigureFileInfoHelperMultiIndexContainer();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string
QGoWizardDB::GetMegaCaptureHeaderFilename()
{
  QGoCreateImgSessionPage *img_page =
    dynamic_cast< QGoCreateImgSessionPage * >( this->page(CreateImgSessionPageID) );

  std::string oFilename;

  if ( img_page )
    {
    oFilename = img_page->GetMegaCaptureHeaderFilename();
    }

  return oFilename;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoWizardDB::setImgSessionName(std::string iImgSessionName)
{
  this->m_ImgSessionName = iImgSessionName;
  this->m_IsAnOpenRecentFile = true;
  this->m_ConnectServerPage->SetImgSessionName(iImgSessionName);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoWizardDB::SetFirstFileName()
{
  /** \todo Lydie: redundant, create an OpenDBConnection....*/
  std::string Server = field("ServerName").toString().toStdString();
  std::string User = field("User").toString().toStdString();
  std::string Password = field("Password").toString().toStdString();
  std::string DBName = field("DBName").toString().toStdString();

  std::pair< bool, vtkMySQLDatabase * > ConnectionDatabase = ConnectToDatabase(
    Server, User, Password, DBName);

  if ( !ConnectionDatabase.first )
    {
    std::cout << "No connection open for QGoOpenOrCreateImgSession" << std::endl;
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }

  vtkMySQLDatabase *DatabaseConnector = ConnectionDatabase.second;
  this->m_ImgSessionID = FindOneID(DatabaseConnector, "imagingsession",
                                   "ImagingSessionID", "Name", this->m_ImgSessionName);
  this->m_FirstFileName = ReturnOnlyOneValue( DatabaseConnector,
                                              "image", "Filename", "ImagingSessionID",
                                              ConvertToString< int >(this->m_ImgSessionID) );

  DatabaseConnector->Close();
  DatabaseConnector->Delete();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoWizardDB::GetFirstFileName()
{
  if ( !this->m_IsAnOpenRecentFile )
    {
    this->m_ImgSessionName =
      field("ImgSessionName").toString().toStdString();
    }
  this->SetFirstFileName();
  this->m_ImgSessionName.clear();
  this->m_ConnectServerPage->SetIsAnOpenRecentFile(false);
  return this->m_FirstFileName;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoWizardDB::GetIsAnOpenRecentFile()
{
  return this->m_IsAnOpenRecentFile;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoWizardDB::SetIsAnOpenRecentFile(bool iIsAnOpenRecentFile)
{
  this->m_IsAnOpenRecentFile = iIsAnOpenRecentFile;
}
