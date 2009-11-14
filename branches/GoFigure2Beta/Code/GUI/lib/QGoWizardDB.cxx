/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Rev: 577 $  // Revision of last commit
  Date: $Date: 2009-08-18 10:45:50 -0400 (Tue, 18 Aug 2009) $  // Date of last commit
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
#include "QGoWizardDB.h"

#include "QGoConnectServerPage.h"
#include "QGoCreateDataBasePage.h"
#include "QGoOpenCreateProjectPage.h"
#include "QGoOpenCreateImgSessionPage.h"
#include "QGoCreateImgSessionPage.h"

#include "CreateDataBaseHelper.h"
#include "QueryDataBaseHelper.h"
#include "SelectQueryDatabaseHelper.h"
#include "GoDBRecordSet.h"
#include "GoDBRecordSetHelper.h"
#include "GoDBSeriesGridRow.h"
#include "itkMegaCaptureImport.h"
#include "itkLsm3DSerieImport.h"
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
QGoWizardDB::QGoWizardDB( QWidget *iParent )
: QWizard( iParent )
{
  QFont tfont;
  tfont.setBold(true);
  this->setFont(tfont);

  QFont font2;
  font2.setBold(false);

  nextButton = new QPushButton(tr("Next"));
  nextButton->setFont(font2);
  this->setButton ( QWizard::NextButton, nextButton );
  QPushButton* backButton = new QPushButton(tr("Back"));
  backButton->setFont(font2);
  this->setButton ( QWizard::BackButton, backButton );
  this->setOptions(QWizard::NoCancelButton);
  QPushButton* finishButton = new QPushButton(tr("Finish"));
  finishButton->setFont(font2);
  this->setButton ( QWizard::FinishButton, finishButton );

  setPage(ConnectServerPageID, new QGoConnectServerPage );
  //setPage(CreateDataBasePageID, new QGoCreateDataBasePage);
  setPage(OpenOrCreateProjectPageID, new QGoOpenCreateProjectPage);
  setPage(OpenOrCreateImgSessionPageID, new QGoOpenCreateImgSessionPage);
  setPage(CreateImgSessionPageID, new QGoCreateImgSessionPage);
  setWindowTitle( tr("Use DataBase") );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<std::vector<std::string> > QGoWizardDB::GetFilenamesFromDB()
{
  std::vector<std::vector<std::string> > oFilenames;
  //first, open a connection to the database:
  std::string Server = field("ServerName").toString().toStdString();
  std::string User = field("User").toString().toStdString();
  std::string Password = field("Password").toString().toStdString();
  std::string DBName = field("DBName").toString().toStdString();

  std::pair<bool,vtkMySQLDatabase*> ConnectionDatabase = ConnectToDatabase(
  Server,User,Password,DBName);

  if (!ConnectionDatabase.first)
    {
    std::cout<<"No connection open for QGoOpenOrCreateImgSession"<<std::endl;
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }

  vtkMySQLDatabase* DatabaseConnector = ConnectionDatabase.second;

  //Get the number of channels with their id as a map
  //ListChannelsIDNumber[channelID]=ChannelNumber:
  std::map<std::string,std::string> ListChannelsIDNumber =
    MapTwoColumnsFromTable( DatabaseConnector, "channelID", "ChannelNumber",
      "channel", "ImagingSessionID", field("ImgSessionID").toString().toStdString() );

  std::map< std::string, std::string >::iterator it = ListChannelsIDNumber.begin();
  oFilenames.resize(ListChannelsIDNumber.size());
  int i = 0;
  while (it != ListChannelsIDNumber.end())
    {
    std::string ChannelID = it->first;
    //get the filenames of all the images corresponding to ChannelID:
    oFilenames[i] = ListSpecificValuesForOneColumn(
      DatabaseConnector,"image", "Filename","channelID",ChannelID);
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
  return field("ImgSessionID").toInt();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QGoWizardDB::GetImagingSessionName()
{
  return field("ImgSessionName").toString();
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
void QGoWizardDB::closeEvent(QCloseEvent* iEvent)
{
  int CurrentPageID = this->currentId();
  QWizardPage* CurrentPage = this->currentPage();

  switch (CurrentPageID)
    {
    case 0:
      {
      //QGoConnectServerPage* ServerPage = dynamic_cast<QGoConnectServerPage*>(CurrentPage);
      //ServerPage->m_ConnectionServer.second->Close();
      //ServerPage->m_ConnectionServer.second->Delete();
      //delete ServerPage;
      break;
      }
    case 2:
      {
      QGoOpenCreateProjectPage* ProjectPage = dynamic_cast<QGoOpenCreateProjectPage*>(CurrentPage);
      ProjectPage->m_DatabaseConnector->Close();
      ProjectPage->m_DatabaseConnector->Delete();
      delete ProjectPage;
      break;
      }
    case 3:
      {
      QGoOpenCreateImgSessionPage* ImgSessionPage = dynamic_cast<QGoOpenCreateImgSessionPage*>(CurrentPage);
      ImgSessionPage->m_DatabaseConnector->Close();
      ImgSessionPage->m_DatabaseConnector->Delete();
      delete ImgSessionPage;
      break;
      }
    case 4:
      {
      QGoCreateImgSessionPage* CreateImgSessionPage = dynamic_cast<QGoCreateImgSessionPage*>(CurrentPage);
      CreateImgSessionPage->m_DatabaseConnector->Close();
      CreateImgSessionPage->m_DatabaseConnector->Delete();
      delete CreateImgSessionPage;
      break;
      }
    default:
      {
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

GoFigureFileInfoHelperMultiIndexContainer
QGoWizardDB::
GetMultiIndexFileContainer()
{
  QGoCreateImgSessionPage* img_page =
    dynamic_cast< QGoCreateImgSessionPage* >( this->page( CreateImgSessionPageID ) );

  if( img_page )
    {
    return img_page->GetMultiIndexFileContainer();
    }
  else
    {
    GoFigureFileInfoHelperMultiIndexContainer();
    }
}

std::string
QGoWizardDB::
GetMegaCaptureHeaderFilename()
{
  QGoCreateImgSessionPage* img_page =
    dynamic_cast< QGoCreateImgSessionPage* >( this->page( CreateImgSessionPageID ) );

  std::string oFilename;

  if( img_page )
    {
    oFilename = img_page->GetMegaCaptureHeaderFilename();
    }

  return oFilename;
}