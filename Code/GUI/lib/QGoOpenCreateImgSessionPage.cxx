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
#include "QGoOpenCreateImgSessionPage.h"
#include "QueryDataBaseHelper.h"
#include "SelectQueryDatabaseHelper.h"
#include "QGoWizardDB.h"

#include <QGridLayout>

QGoOpenCreateImgSessionPage::QGoOpenCreateImgSessionPage(QWidget *iParent):
  QWizardPage(iParent),
  m_DatabaseConnector(0)
{
  QFont tfont;

  tfont.setBold(false);
  this->setFont(tfont);

  CreateImgSessionRadioButton =
    new QRadioButton( tr("Create a new Imaging Session    ") );
  OpenImgSessionRadioButton   =
    new QRadioButton( tr("Open an existing Imaging Session") );

  textChoiceImgSession = new QLabel( tr("Imaging Session to open:") );
  ChoiceImgSession  = new QComboBox;
  textDescription = new QLabel( tr("Description:") );
  lineDescription  = new QTextEdit;
  lineImgSessionID = new QLineEdit;
  lineImgSessionName = new QLineEdit;

  QVBoxLayout *vlayout = new QVBoxLayout;
  QVBoxLayout *RadioButtonLayout = new QVBoxLayout;
  RadioButtonLayout->addWidget(CreateImgSessionRadioButton);
  RadioButtonLayout->addWidget(OpenImgSessionRadioButton);
  vlayout->addLayout(RadioButtonLayout);
  vlayout->setAlignment(RadioButtonLayout, Qt::AlignHCenter);
  QGridLayout *gridlayout = new QGridLayout;
  gridlayout->addWidget(textChoiceImgSession, 0, 0);
  gridlayout->addWidget(ChoiceImgSession, 0, 1);
  gridlayout->addWidget(textDescription, 3, 0);
  gridlayout->addWidget(lineDescription, 3, 1);
  vlayout->addLayout(gridlayout);
  setLayout(vlayout);

  registerField("ImgSessionName", lineImgSessionName);
  registerField("ImgSessionID", lineImgSessionID);

  QObject::connect( this->ChoiceImgSession, SIGNAL( currentIndexChanged(QString) ),
                    this, SLOT( DisplayInfoImgSession(QString) ) );

  QObject::connect( this->OpenImgSessionRadioButton, SIGNAL( clicked() ),
                    this, SLOT( ChangeToOpenImgSessionDisplay() ) );

  QObject::connect( this->CreateImgSessionRadioButton, SIGNAL( clicked() ),
                    this, SLOT( ChangeToCreateImgSessionDisplay() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoOpenCreateImgSessionPage::~QGoOpenCreateImgSessionPage()
{
  delete lineImgSessionID;
  delete lineImgSessionName;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoOpenCreateImgSessionPage::initializePage()
{
  if ( !m_ListImgSession.isEmpty() )
    {
    m_ListImgSession.clear();
    }
  ChoiceImgSession->clear();

  OpenDBConnection();

  if ( !GetListImgSession() )
    {
    std::cout << "Pb, there is no existing Img session" << std::endl;
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }
  else
    {
    OpenImgSessionRadioButton->setChecked(true);
    ChangeToOpenImgSessionDisplay();
    ChoiceImgSession->addItems(m_ListImgSession);
    DisplayInfoImgSession( ChoiceImgSession->currentText() );
    this->setFinalPage(false);
    }

  lineDescription->setReadOnly(true);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoOpenCreateImgSessionPage::GetListImgSession()
{
  m_ListImgSession.clear();
  ChoiceImgSession->clear();
  m_MapImgSessionIDName.clear();

  m_MapImgSessionIDName = MapTwoColumnsFromTable( m_DatabaseConnector,
                                                  "Name", "ImagingSessionID", "imagingsession", "ProjectName",
                                                  field("ProjectName").toString().toStdString() );

  std::map< std::string, std::string >::iterator it = m_MapImgSessionIDName.begin();
  if ( !m_MapImgSessionIDName.empty() )
    {
    while ( it != m_MapImgSessionIDName.end() )
      {
      m_ListImgSession.append( it->first.c_str() );
      it++;
      }
    return true;
    }
  return false;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoOpenCreateImgSessionPage::nextId() const
{
  if ( m_DatabaseConnector == 0 && !LeavingPage )
    {
    OpenDBConnection();
    }

  if ( CreateImgSessionRadioButton->isChecked() )
    {
    return QGoWizardDB::CreateImgSessionPageID;
    }
  return -1;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoOpenCreateImgSessionPage::validatePage()
{
  if ( CloseDatabaseConnection(m_DatabaseConnector) )
    {
    m_DatabaseConnector = 0;
    }
  LeavingPage = true;

  //get the imagingsessionID from the selected imagingsession in the combobox
  //in case the user wants to open an imagingsession:
  if ( OpenImgSessionRadioButton->isChecked() )
    {
    std::string ImgID =
      m_MapImgSessionIDName[ChoiceImgSession->currentText().toStdString()];
    setField( "ImgSessionID", ImgID.c_str() );
    setField( "ImgSessionName", ChoiceImgSession->currentText() );
    }
  return true;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoOpenCreateImgSessionPage::cleanupPage()
{
  if ( CloseDatabaseConnection(m_DatabaseConnector) )
    {
    m_DatabaseConnector = 0;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoOpenCreateImgSessionPage::OpenDBConnection() const
{
  std::string Server = field("ServerName").toString().toStdString();
  std::string User = field("User").toString().toStdString();
  std::string Password = field("Password").toString().toStdString();
  std::string DBName = field("DBName").toString().toStdString();

  m_DatabaseConnector = OpenDatabaseConnection(Server, User, Password, DBName);

  LeavingPage = false;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoOpenCreateImgSessionPage::DisplayInfoImgSession(
  QString ImgSessionName)
{
  // First, get the corresponding ImagingSessionID from the map:
  std::string ImagingSessionID = "0";

  if ( ImgSessionName != "" )
    {
    std::map< std::string, std::string >::iterator it = m_MapImgSessionIDName.begin();
    while ( it != m_MapImgSessionIDName.end() && ImagingSessionID == "0" )
      {
      if ( it->first == ImgSessionName.toStdString() )
        {
        ImagingSessionID = it->second;
        }
      it++;
      }

    std::vector< std::string > ListDescription = ListSpecificValuesForOneColumn(
      m_DatabaseConnector, "imagingsession", "Description",
      "ImagingSessionID", ImagingSessionID);
    if ( ListDescription.size() != 1 )
      {
      std::cout << "Pb, the imagingsession " << ImgSessionName.toStdString().c_str()
                << "has more than 1 description" << std::endl;
      std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
      std::cout << std::endl;
      }

    lineDescription->setText( ListDescription[0].c_str() );
    }
  this->setFinalPage(false);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoOpenCreateImgSessionPage::ChangeToCreateImgSessionDisplay()
{
  setSubTitle(
    tr("Click on 'Next' if you want to import a new dataset\n or choose 'Open an imaging session':") );
  textChoiceImgSession->setVisible(false);
  ChoiceImgSession->setVisible(false);
  lineDescription->setVisible(false);
  textDescription->setVisible(false);
  this->setFinalPage(false);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoOpenCreateImgSessionPage::ChangeToOpenImgSessionDisplay()
{
  setSubTitle(
    tr(
      "Select the imaging session you want to open and click on 'Finish' to load the corresponding images or choose 'Create a new imaging session':") );
  textChoiceImgSession->setVisible(true);
  ChoiceImgSession->setVisible(true);
  lineDescription->setVisible(true);
  textDescription->setVisible(true);
  this->setFinalPage(false);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
