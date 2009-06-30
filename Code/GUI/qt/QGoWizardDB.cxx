/*=========================================================================
  Author: $Author:$  // Author of last commit
  Version: $Rev:$  // Revision of last commit
  Date: $Date:$  // Date of last commit
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
#include "CreateDataBaseHelper.h"
#include "GoDBRecordSet.h"
#include "GoDBFigureRow.h"
#include "GoDBExperimentRow.h"
#include "GoDBSeriesGridRow.h"
#include "itkMegaCaptureImport.h"
#include "itkLsm3DSerieImport.h"
#include "GoFigureFileInfoHelper.h"

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


//------------------------------------------------------------------------------
QGoWizardDB::QGoWizardDB( QWidget *parent )
: QWizard( parent )
{
  QFont font;
  font.setBold(true);
  this->setFont(font);

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

  setPage(Connect_Server, new Connect_ServerPage );
  setPage(OpenOrCreateDB, new OpenOrCreate_Page);
  setPage(Create_Experiment, new Create_ExperimentPage);
  setPage(Import, new Import_SerieGridPage);
  setWindowTitle( tr("Use DataBase") );
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
FileListType QGoWizardDB::ListFilenames()
{
  FileListType ListFilenames;
  std::vector<std::string> vectListFilenames = ListValuesForRow(
  field("ServerName").toString().toStdString(),
        field("User").toString().toStdString(),
        field("Password").toString().toStdString(),
        field("NameDB").toString().toStdString(),"seriesgrid",
        "experimentID",field("ExpID").toString().toStdString());

  if (!vectListFilenames.empty())
    {
    for(unsigned int i = 0; i < vectListFilenames.size();)
      {
      GoFigureFileInfoHelper tempInfo;
      tempInfo.Filename = vectListFilenames[i+8];
      tempInfo.TimePoint = atoi(vectListFilenames[i+4].c_str());
      tempInfo.ZDepth = atoi(vectListFilenames[i+7].c_str());
      tempInfo.CTile = atoi(vectListFilenames[i+3].c_str());
      tempInfo.RTile = atoi(vectListFilenames[i+2].c_str());
      tempInfo.YOffset = atoi(vectListFilenames[i+5].c_str());
      tempInfo.XOffset = atoi(vectListFilenames[i+6].c_str());
      ListFilenames.push_back( tempInfo );
      i = i+9;
      }
    }

  std::sort( ListFilenames.begin(), ListFilenames.end() );
  return ListFilenames;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QString QGoWizardDB::NameDB()
{
  return field("NameDB").toString();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int QGoWizardDB::ExpID()
{
  return field("ExpID").toInt();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QString QGoWizardDB::ExpName()
{
  return field("Name").toString();
}

QString QGoWizardDB::Server()
{
  return field("ServerName").toString();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QString QGoWizardDB::login()
{
  return field("User").toString();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QString QGoWizardDB::Password()
{
  return field("Password").toString();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
Connect_ServerPage::Connect_ServerPage( QWidget *parent )
: QWizardPage( parent )
{
  QFont font;
  font.setBold(false);
  this->setFont(font);
  setSubTitle( tr("Step 1: Connect to a MySQL DataBase Server:"));
  //setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.png"));

  QFormLayout* formLayout = new QFormLayout;
  lineServerName = new QLineEdit( tr("localhost") );
  lineUserName = new QLineEdit( tr("gofigure") );
  linePassword = new QLineEdit( tr("gofigure") );
  linePassword->setEchoMode(QLineEdit::Password);
  linePassword->displayText();

  formLayout->addRow( tr("&ServerName:"), lineServerName );
  formLayout->addRow( tr("&User:"),       lineUserName );
  formLayout->addRow( tr("&Password:"),   linePassword );
  setLayout( formLayout );

  setLayout( formLayout );

  registerField( "ServerName", lineServerName );
  registerField( "User",       lineUserName );
  registerField( "Password",   linePassword );

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool Connect_ServerPage::validatePage()
{
  if( !CanConnectToServer(
        field("ServerName").toString().toStdString(),
        field("User").toString().toStdString(),
        field("Password").toString().toStdString() ) )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Unable to connect to the server: please make sure you entered the right fields.") );
    msgBox.exec();
    return false;
    }
  else
    {
    return true;
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
OpenOrCreate_Page::OpenOrCreate_Page(QWidget *parent)
:QWizardPage(parent)
{
  QFont font;
  font.setBold(false);
  this->setFont(font);

  gridLayout = new QGridLayout;
  ChoiceDB = new QComboBox;
  textChoiceDB = new QLabel(tr("Name of the DB to open :"));

  openDBCheckBox = new QCheckBox(tr("Open an existing DataBase"));
  createDBCheckBox = new QCheckBox(tr("Create a new DataBase"));
  openDBCheckBox->setChecked(false);
  createDBCheckBox->setChecked(false);
  textNewDBName = new QLabel(tr("Name of the new DB to create:"));
  //textNewDBName->hide();
  lineNewDBName = new QLineEdit;
  //lineNewDBName->hide();

  gridLayout->addWidget(createDBCheckBox,0,0,1,1);
  gridLayout->addWidget(textNewDBName,3,0,1,2);
  gridLayout->addWidget(lineNewDBName,3,1,1,1);
  gridLayout->addWidget(openDBCheckBox,5,0,1,1);
  gridLayout->addWidget(textChoiceDB,6,0,1,1);
  gridLayout->addWidget(ChoiceDB,6,1,1,1);

  //gridLayout->setAlignment(openDBCheckBox,Qt::AlignLeft);
  //gridLayout->setAlignment(createDBCheckBox,Qt::AlignLeft);
  gridLayout->setAlignment(textNewDBName,Qt::AlignLeft);
  gridLayout->setAlignment(textChoiceDB,Qt::AlignLeft);

  setLayout(gridLayout);

  QObject::connect( this->openDBCheckBox,SIGNAL( clicked() ),
  this,SLOT( PrintListDB() ));

  QObject::connect( this->createDBCheckBox,SIGNAL( clicked() ),
  this,SLOT( EnterNameDB() ));

  registerField( "DBIndextoOpen", ChoiceDB);
  registerField("DBNametoCreate",lineNewDBName);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void OpenOrCreate_Page::initializePage()
{
  this->setSubTitle(tr("Step 2: Chose what you want to do next:"));
  ChoiceDB->hide();
  textChoiceDB->hide();
  textNewDBName->hide();
  lineNewDBName->hide();
  openDBCheckBox->setChecked(false);
  createDBCheckBox->setChecked(false);
  field("DBIndextoOpen").clear();
  field("DBNametoCreate").clear();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void OpenOrCreate_Page::PrintListDB ()
{
  createDBCheckBox->setChecked(false);
  field("DBNametoCreate").clear();
  textChoiceDB->show();
  ChoiceDB->show();

  textNewDBName->hide();
  lineNewDBName->hide();
  lineNewDBName->setText("");

  if( m_ListDB.isEmpty() )
    {
    std::vector<std::string> vectListDB = ListDataBases(
      field("ServerName").toString().toStdString(),
      field("User").toString().toStdString(),
      field("Password").toString().toStdString() );

    for( unsigned int i = 0; i < vectListDB.size(); ++i )
      {
      if( IsDatabaseOfGoFigureType( field("ServerName").toString().toStdString(),
            field("User").toString().toStdString(),
            field("Password").toString().toStdString(),
            vectListDB[i].c_str( ) ) )
        {
        m_ListDB.append( vectListDB[i].c_str( ) );
        }
      }
    ChoiceDB->addItems( m_ListDB );
    }
  ChoiceDB->show();

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void OpenOrCreate_Page::EnterNameDB ()
{
  if( createDBCheckBox->isChecked() )
    {
    openDBCheckBox->setChecked(false);
    field("DBIndextoOpen").clear();
    m_ListDB.clear();
    ChoiceDB->clear();

    lineNewDBName->show();
    textNewDBName->show();
    ChoiceDB->hide();
    textChoiceDB->hide();
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool OpenOrCreate_Page::validatePage()
{
  QString NameDB;
  DBNametoOpen_fake = new QLineEdit;

  int i = field("DBIndextoOpen").toInt();
  if( !openDBCheckBox->isChecked() && !createDBCheckBox->isChecked() )
    {
    QMessageBox msgBox;
    msgBox.setText(tr("Please select one option").arg(NameDB));
    msgBox.exec();
    return false;
    }

  if( ( i > -1 ) && ( i < m_ListDB.size() ) )
    {
    NameDB = m_ListDB[i];
    }
  else
    {
    NameDB = "Null";
    }

  if( field("DBNametoCreate").toString().isEmpty() && ( NameDB=="Null" ) )
    {
    QMessageBox msgBox;
    msgBox.setText(tr("Please enter a name for your new DataBase").arg(NameDB));
    msgBox.exec();
    return false;
    }

  if( NameDB=="Null" )
      {
      return true;
      }

  if( !IsDatabaseOfGoFigureType(field("ServerName").toString().toStdString(),
            field("User").toString().toStdString(),
            field("Password").toString().toStdString(),
            NameDB.toStdString() ) )
    {
    QMessageBox msgBox;
    msgBox.setText(tr("The Database %1 is not a Gofigure Database").arg(NameDB));
    msgBox.exec();
    return false;
    }

  registerField( "DBNametoOpen", DBNametoOpen_fake );
  setField( "DBNametoOpen",NameDB );
  return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
Create_ExperimentPage::Create_ExperimentPage( QWidget *parent )
: QWizardPage( parent )
{
  QFont font;
  font.setBold(false);
  this->setFont(font);

  ChoiceExp  = new QComboBox;
  ExpID = new QLabel(tr("ExperimentID"));
  ID = new QLineEdit;
  textName = new QLabel(tr("Name of the Experiment: "));
  Name = new QLineEdit;

  openExpRadioButton   = new QRadioButton(tr("Open an existing Experiment"));
  createExpRadioButton = new QRadioButton(tr("Create a new Experiment    "));

  QVBoxLayout* vlayout = new QVBoxLayout;
  QVBoxLayout* RadioButtonLayout = new QVBoxLayout;
  RadioButtonLayout->addWidget(createExpRadioButton);
  RadioButtonLayout->addWidget(openExpRadioButton);
  vlayout->addLayout(RadioButtonLayout);
  vlayout->setAlignment(RadioButtonLayout,Qt::AlignHCenter);

  textChoiceExp = new QLabel(tr("Experiment to open:"));

  QGridLayout* gridlayout = new QGridLayout;
  gridlayout->addWidget(textChoiceExp,0,0);
  gridlayout->addWidget(ChoiceExp,0,1);
  gridlayout->addWidget(ExpID,1,0);
  gridlayout->addWidget(ID,1,1);
  gridlayout->addWidget(textName,3,0);
  gridlayout->addWidget(Name,3,1);

  Description  = new QLineEdit;
  QLabel* TextDescription = new QLabel(tr("Description"));
  TimeInterval = new QLineEdit;
  QLabel* TextTimeInterval = new QLabel(tr("TimeInterval"));
  TileHeight   = new QLineEdit;
  QLabel* TextTileHeight = new QLabel(tr("TileHeight"));
  TileWidth    = new QLineEdit;
  QLabel* TextTileWidth = new QLabel(tr("TileWidth"));
  PixelDepth   = new QLineEdit;
  QLabel* TextPixelDepth = new QLabel(tr("PixelDepth"));
  PixelHeight  = new QLineEdit;
  QLabel* TextPixelHeight = new QLabel(tr("PixelHeight"));
  PixelWidth   = new QLineEdit;
  QLabel* TextPixelWidth = new QLabel(tr("PixelWidth"));
  ColorDepth   = new QLineEdit;
  QLabel* TextColorDepth = new QLabel(tr("ColorDepth"));
  nTimePoints  = new QLineEdit;
  QLabel* TextnTimePoints = new QLabel(tr("nTimePoints"));
  nYTiles = new QLineEdit;
  QLabel* TextnYTiles = new QLabel(tr("nYTiles"));
  nXTiles = new QLineEdit;
  QLabel* TextnXTiles = new QLabel(tr("nXTiles"));
  nSlices = new QLineEdit;
  QLabel* TextnSlices = new QLabel(tr("nSlices"));
  nRows       = new QLineEdit;
  QLabel* TextnRows = new QLabel(tr("nRows"));
  nColumns    = new QLineEdit;
  QLabel* TextnColumns = new QLabel(tr("nColumns"));
  FilePattern = new QLineEdit;
  QLabel* TextFilePattern = new QLabel(tr("FilePattern"));
  OpenOrCreateExp_fake = new QLineEdit; /** \todo TO BE DELETED */

  gridlayout->addWidget(TextDescription,4,0);
  gridlayout->addWidget(Description,4,1);
  gridlayout->addWidget(TextTimeInterval,5,0);
  gridlayout->addWidget(TimeInterval,5,1);
  gridlayout->addWidget(TextTileHeight,6,0);
  gridlayout->addWidget(TileHeight,6,1);
  gridlayout->addWidget(TextTileWidth,7,0);
  gridlayout->addWidget(TileWidth,7,1);
  gridlayout->addWidget(TextPixelDepth,8,0);
  gridlayout->addWidget(PixelDepth,8,1);
  gridlayout->addWidget(TextPixelHeight,9,0);
  gridlayout->addWidget(PixelHeight,9,1);
  gridlayout->addWidget(TextPixelWidth,10,0);
  gridlayout->addWidget(PixelWidth,10,1);
  gridlayout->addWidget(TextColorDepth,11,0);
  gridlayout->addWidget(ColorDepth,11,1);
  gridlayout->addWidget(TextnTimePoints,12,0);
  gridlayout->addWidget(nTimePoints,12,1);
  gridlayout->addWidget(TextnYTiles,13,0);
  gridlayout->addWidget(nYTiles,13,1);
  gridlayout->addWidget(TextnXTiles,14,0);
  gridlayout->addWidget(nXTiles,14,1);
  gridlayout->addWidget(TextnSlices,15,0);
  gridlayout->addWidget(nSlices,15,1);
  gridlayout->addWidget(TextnRows,16,0);
  gridlayout->addWidget(nRows,16,1);
  gridlayout->addWidget(TextnColumns,17,0);
  gridlayout->addWidget(nColumns,17,1);
  gridlayout->addWidget(TextFilePattern,18,0);
  gridlayout->addWidget(FilePattern,18,1);

  vlayout->addLayout(gridlayout);

  registerField( "Name", Name );
  registerField( "Description", Description );
  registerField( "TimeInterval", TimeInterval );
  registerField( "TileHeight", TileHeight );
  registerField( "TileWidth", TileWidth );
  registerField( "PixelDepth", PixelDepth );
  registerField( "PixelHeight", PixelHeight );
  registerField( "PixelWidth", PixelWidth );
  registerField( "ColorDepth", ColorDepth );
  registerField( "nTimePoints", nTimePoints );
  registerField( "nYTiles", nYTiles );
  registerField( "nXTiles", nXTiles );
  registerField( "nSlices", nSlices );
  registerField( "nRows", nRows );
  registerField( "nColumns", nColumns );
  registerField( "FilePattern", FilePattern );
  registerField("OpenOrCreateExp",OpenOrCreateExp_fake);
  registerField("ExpID", ID);

  setLayout( vlayout );

  QObject::connect( this->openExpRadioButton,SIGNAL( clicked() ),
  this,SLOT( PrintListExp() ));

  QObject::connect( this->createExpRadioButton,SIGNAL( clicked() ),
  this,SLOT( EnterInfoExp() ));

  QObject::connect( this->ChoiceExp,SIGNAL( currentIndexChanged(QString) ),
  this,SLOT( PrintValuesExpName(QString) ));

  NameDB_fake = new QLineEdit; /** \todo TO BE DELETED */
  registerField("NameDB",NameDB_fake);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void Create_ExperimentPage::initializePage()
{
  ExpID->setVisible(false);
  ID->setVisible(false);
  ID->setEnabled(false);
  setFinalPage(false);

  if( !field("DBNametoCreate").toString().isEmpty() )
    {
    CreateDataBaseMain(field("ServerName").toString().toStdString(),
      field("User").toString().toStdString(),
      field("Password").toString().toStdString(),
      field("DBNametoCreate").toString().toStdString());

    setSubTitle(
      tr("You are currently using The Database %1 ").arg(
        field("DBNametoCreate").toString() ) );
    setField( "NameDB", field("DBNametoCreate") );
    openExpRadioButton->setChecked(false);
    createExpRadioButton->setChecked(true);
    EnterInfoExp();
    }
  else
    {
    setSubTitle(tr("You are currently using The Database %1 ").arg(field("DBNametoOpen").toString()));
    setField("NameDB",field("DBNametoOpen"));
    openExpRadioButton->setChecked(true);
    createExpRadioButton->setChecked(false);
    PrintListExp();
    if (!PrintListExp())
      {
      EnterInfoExp();
      openExpRadioButton->setChecked(false);
      createExpRadioButton->setChecked(true);
      }
    }

  field( "Name").clear();
  field( "Description").clear();
  field( "TimeInterval").clear();
  field( "TileHeight").clear();
  field( "TileWidth").clear();
  field( "PixelDepth").clear();
  field( "PixelHeight").clear();
  field( "PixelWidth").clear();
  field( "ColorDepth").clear();
  field( "nTimePoints").clear();
  field( "nYTiles").clear();
  field( "nXTiles").clear();
  field( "nSlices").clear();
  field( "nRows").clear();
  field( "nColumns").clear();
  field( "FilePattern").clear();
  field("OpenOrCreateExp").clear();
  field("ExpID").clear();

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void Create_ExperimentPage::EnterInfoExp()
{
  Name->clear();
  Description->clear();
  TimeInterval->clear();
  TileHeight->clear();
  TileWidth->clear();
  PixelDepth->clear();
  PixelHeight->clear();
  PixelWidth->clear();
  ColorDepth->clear();
  nTimePoints->clear();
  nYTiles->clear();
  nXTiles->clear();
  nSlices->clear();
  nRows->clear();
  nColumns->clear();
  FilePattern->clear();

  textChoiceExp->setVisible(false);
  ChoiceExp->setVisible(false);
  Name->setVisible(true);
  textName->setVisible(true);
  Name ->setEnabled(true);
  Description ->setEnabled(true);
  TimeInterval ->setEnabled(true);
  TileHeight ->setEnabled(true);
  TileWidth ->setEnabled(true);
  PixelDepth ->setEnabled(true);
  PixelHeight ->setEnabled(true);
  PixelWidth ->setEnabled(true);
  ColorDepth ->setEnabled(true);
  nTimePoints ->setEnabled(true);
  nYTiles ->setEnabled(true);
  nXTiles ->setEnabled(true);
  nSlices ->setEnabled(true);
  nRows ->setEnabled(true);
  nColumns ->setEnabled(true);
  FilePattern ->setEnabled(true);

  setField("OpenOrCreateExp","Create");
  setFinalPage(false);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool Create_ExperimentPage::PrintListExp()
{
  bool ok;

  textChoiceExp->setVisible(true);
  ChoiceExp->setVisible(true);
  Name ->setVisible(false);
  textName->setVisible(false);
  Description ->setEnabled(false);
  TimeInterval ->setEnabled(false);
  TileHeight ->setEnabled(false);
  TileWidth ->setEnabled(false);
  PixelDepth ->setEnabled(false);
  PixelHeight ->setEnabled(false);
  PixelWidth ->setEnabled(false);
  ColorDepth ->setEnabled(false);
  nTimePoints ->setEnabled(false);
  nYTiles ->setEnabled(false);
  nXTiles ->setEnabled(false);
  nSlices ->setEnabled(false);
  nRows ->setEnabled(false);
  nColumns ->setEnabled(false);
  FilePattern ->setEnabled(false);


  m_ListExpName.clear();
  m_ListExpID.clear();
  ChoiceExp->clear();

  std::vector<std::string> vectListExpName =
    ListExpName( field("ServerName").toString().toStdString(),
      field("User").toString().toStdString(),
      field("Password").toString().toStdString(),
      field("NameDB").toString().toStdString());

  if (!vectListExpName.empty())
    {
    for(unsigned int i = 0; i < vectListExpName.size(); ++i )
      {
      m_ListExpName.append( vectListExpName[i].c_str( ) );
      }
    ok = true;
    }
  else
    {
    ok = false;
    }

  ChoiceExp->addItems( m_ListExpName );
  ChoiceExp->show();

  setField("OpenOrCreateExp","Open");
  setFinalPage(false);

  return ok;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int Create_ExperimentPage::nextId() const
{
  if( field( "OpenOrCreateExp" ) == "Open" )
    {
    std::vector< std::string > ListFinalPage = ListValuesForOneColumn(
      field("ServerName").toString().toStdString(),
      field("User").toString().toStdString(),
      field("Password").toString().toStdString(),
      field("NameDB").toString().toStdString(), "seriesgrid",
      "filename", "experimentID", field("ExpID").toString().toStdString());

    if( !ListFinalPage.empty() )
      {
      std::cout<<"last page ID"<<std::endl;
      return -1;
      }
    else
      {
      return QGoWizardDB::Import;
      }
    }
  else
    {
    return QGoWizardDB::Import;
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void Create_ExperimentPage::PrintValuesExpName(QString ExpName)
{
  std::vector<std::string> myvect =
    ListValuesForRow(
        field("ServerName").toString().toStdString(),
        field("User").toString().toStdString(),
        field("Password").toString().toStdString(),
        field("NameDB").toString().toStdString(),"experiment",
        "name",ExpName.toStdString());

  if ( !myvect.empty() )
    {
    setField("ExpID",myvect[0].c_str());
    setField("Name",myvect[1].c_str());
    setField("Description",myvect[2].c_str());
    setField("TimeInterval",myvect[3].c_str());
    setField("TileHeight",myvect[4].c_str());
    setField("TileWidth",myvect[5].c_str());
    setField("PixelDepth",myvect[6].c_str());
    setField("PixelHeight",myvect[7].c_str());
    setField("PixelWidth",myvect[8].c_str());
    setField("ColorDepth",myvect[9].c_str());
    setField("nTimePoints",myvect[10].c_str());
    setField("nYTiles",myvect[11].c_str());
    setField("nXTiles",myvect[12].c_str());
    setField("nSlices",myvect[13].c_str());
    setField("nRows",myvect[14].c_str());
    setField("nColumns",myvect[15].c_str());
    setField("FilePattern",myvect[16].c_str());
    }

  std::vector<std::string> List;
  List.clear();
  List = ListValuesForOneColumn(
    field("ServerName").toString().toStdString(),
    field("User").toString().toStdString(),
    field("Password").toString().toStdString(),
    field("NameDB").toString().toStdString(), "seriesgrid",
    "filename", "experimentID", field("ExpID").toString().toStdString() );

  setFinalPage(false);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool Create_ExperimentPage::validatePage()
{
  if (createExpRadioButton->isChecked())
    {
    if( field("Name").isNull() )
      {
      QMessageBox msgBox;
      msgBox.setText(tr("Please enter a name for your experiment."));
      msgBox.exec();
      return false;
      }
    else
      {
      QStringList ListExistingNames;
      std::vector<std::string> vectListExpName =
        ListExpName(
          field("ServerName").toString().toStdString(),
          field("User").toString().toStdString(),
          field("Password").toString().toStdString(),
          field("NameDB").toString().toStdString() );

      if( !vectListExpName.empty() )
        {
        for( unsigned int i = 0; i < vectListExpName.size(); ++i )
          {
          ListExistingNames.append( vectListExpName[i].c_str( ) );
          }
        }
      if( ListExistingNames.contains( field("Name").toString(), Qt::CaseInsensitive ) )
        {
        QMessageBox msgBox;
        msgBox.setText( tr( "The name you entered for your experiment already exists." ) );
        msgBox.exec();
        return false;
        }
      else
        {
        return true;
        }
      }
    }
  else
    {
    if( field("Name").isNull() )
      {
      QMessageBox msgBox;
      msgBox.setText(
        tr("There is no existing Experiment in this DataBase, please create a new one."));
      msgBox.exec();
      return false;
      }
    else
      {
      return openExpRadioButton->isChecked();
      }
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
Import_SerieGridPage::Import_SerieGridPage( QWidget *parent )
: QWizardPage( parent )
{
  QFont font;
  font.setBold(false);
  this->setFont(font);
  this->adjustSize();
  newfilename.clear();

  gridlayout= new QGridLayout;
  BrowseButton = new QPushButton("&Browse", this);
  Explanation = new QLabel(tr("Click on the 'browse' button and select only 1 file from the Image Set you want to import:"));
  gridlayout->addWidget( BrowseButton,0,1 );
  gridlayout->addWidget(Explanation,0,0);
  gridlayout->setColumnStretch ( 0, 3);
  gridlayout->setColumnStretch ( 1, 1);

  setLayout(gridlayout);

  QObject::connect( this->BrowseButton,SIGNAL( clicked() ),
  this,SLOT( SelectSeriesGrid() ));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void Import_SerieGridPage::initializePage()
{
  newfilename.clear();
  completeChanged();
  BrowseButton->setVisible(true);

  if( field( "OpenOrCreateExp" ) == "Create" )
    {
    GoDBExperimentRow myNewObject;

    myNewObject.name         = field("Name").toString().toStdString();
    myNewObject.description  = field("Description").toString().toStdString();
    myNewObject.timeInterval = field("TimeInterval").toInt();
    myNewObject.tileHeight   = field("TileHeight").toInt();
    myNewObject.tileWidth    = field("TileWidth").toInt();
    myNewObject.pixelDepth   = field("PixelDepth").toDouble();
    myNewObject.pixelHeight  = field("PixelHeight").toDouble();
    myNewObject.pixelWidth   = field("PixelWidth").toDouble();
    myNewObject.colorDepth   = field("ColorDepth").toInt();
    myNewObject.nTimePoints  = field("nTimePoints").toInt();
    myNewObject.nYTiles      = field("nYTiles").toInt();
    myNewObject.nXTiles      = field("nXTiles").toInt();
    myNewObject.nSlices      = field("nSlices").toInt();
    myNewObject.nRows        = field("nRows").toInt();
    myNewObject.nColumns     = field("nColumns").toInt();
    myNewObject.filePattern  = field("FilePattern").toString().toStdString();

    RecordValues_inTable< GoDBExperimentRow >(
      field("ServerName").toString().toStdString(),
      field("User").toString().toStdString(),
      field("Password").toString().toStdString(),
      field("NameDB").toString().toStdString(), "experiment", myNewObject );

    std::vector<std::string> vectListExpID =
      ListExpID(
        field("ServerName").toString().toStdString(),
        field("User").toString().toStdString(),
        field("Password").toString().toStdString(),
        field("NameDB").toString().toStdString());

    setField( "ExpID", (unsigned int)vectListExpID.size() );
    }

  //setTitle( tr("Experiment: '%1'. DataBase: '%2'")
    //.arg(field("Name").toString()).arg(field("NameDB").toString()) );
  setTitle(tr("Experiment: '%1'").arg(field("Name").toString()));
  setSubTitle(tr("Database: '%1'").arg(field("NameDB").toString()));
  //setSubTitle(tr("Click on the 'browse' button and select only 1 file from the Image Set you want to import:"));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void Import_SerieGridPage::SelectSeriesGrid()
{
  newfilename = QFileDialog::getOpenFileName(
    this,tr( "Import Image" ),"",tr( "Images (*.png *.bmp *.jpg *.jpeg *.tiff *.mha *.mhd *.img *.lsm)" ));

  std::vector<std::string> myvect =
    ListValuesForRow(
        field("ServerName").toString().toStdString(),
        field("User").toString().toStdString(),
        field("Password").toString().toStdString(),
        field("NameDB").toString().toStdString(),"seriesgrid",
        "experimentID",field("ExpID").toString().toStdString());

  if( !myvect.empty() )
    {
    QMessageBox msgBox;
    msgBox.setText( tr("There is already an Image Set for this Experiment.") );
    msgBox.exec();
    return;
    }
  else
    {
    if( !newfilename.isEmpty( ) )
      {
      QString ext = QFileInfo( newfilename ).suffix();
      if( ext.compare( "lsm", Qt::CaseInsensitive ) == 0 )
        {
          std::cout<<"lsm files"<<std::endl;
          try
          {
            itk::Lsm3DSerieImport::Pointer  importFileInfoList = itk::Lsm3DSerieImport::New();
            importFileInfoList->SetFileName( newfilename.toAscii().data() );
            importFileInfoList->SetGroupId( 1 );
            importFileInfoList->Update();

            typedef GoDBRecordSet< GoDBSeriesGridRow > myRecordSetType;
            myRecordSetType* RecordSet = new myRecordSetType;
            RecordSet->SetServerName(field("ServerName").toString().toStdString());
            RecordSet->SetUser(field("User").toString().toStdString());
            RecordSet->SetPassword(field("Password").toString().toStdString());
            RecordSet->SetDataBaseName(field("NameDB").toString().toStdString());
            RecordSet->SetTableName( "seriesgrid" );

            typedef FileListType::iterator myFilesIteratorType;
            myFilesIteratorType It  = importFileInfoList->GetOutput()->begin();
            myFilesIteratorType end = importFileInfoList->GetOutput()->end();
            while( It != end )
              {
              GoDBSeriesGridRow row;
              row.experimentID = field("ExpID").toInt();
              row.RCoord = (*It).RTile;
              row.CCoord = (*It).CTile;
              row.TCoord = (*It).TimePoint;
              row.YCoord = (*It).YOffset;
              row.XCoord = (*It).XOffset;
              row.ZCoord = (*It).ZDepth;
              row.filename = (*It).Filename.c_str();

              RecordSet->AddObject( row );

              It++;
              }

            if( !RecordSet->SaveInDB() )
              {
              return;
              }
          delete RecordSet;
          }
          catch( const itk::ExceptionObject& e )
          {
          std::cerr << " caught an ITK exception: " << std::endl;
          e.Print( std::cerr);
          return;
          }
        catch( const std::exception& e )
          {
          std::cerr << " caught an std exception: " << std::endl;
          std::cerr << e.what() << std::endl;
          return;
          }
        catch( ... )
          {
          std::cerr << " caught an unknown exception!" << std::endl;
          return;
          }
        }
      else
        {
        try
          {
          itk::MegaCaptureImport::Pointer  importFileInfoList = itk::MegaCaptureImport::New();
          importFileInfoList->SetFileName( newfilename.toAscii().data() );
          importFileInfoList->Update();

          typedef GoDBRecordSet< GoDBSeriesGridRow > myRecordSetType;
          myRecordSetType* RecordSet = new myRecordSetType;
          RecordSet->SetServerName(field("ServerName").toString().toStdString());
          RecordSet->SetUser(field("User").toString().toStdString());
          RecordSet->SetPassword(field("Password").toString().toStdString());
          RecordSet->SetDataBaseName(field("NameDB").toString().toStdString());
          RecordSet->SetTableName( "seriesgrid" );

          typedef FileListType::iterator myFilesIteratorType;
          myFilesIteratorType It  = importFileInfoList->GetOutput()->begin();
          myFilesIteratorType end = importFileInfoList->GetOutput()->end();
          while( It != end )
            {
            GoDBSeriesGridRow row;
            row.experimentID = field("ExpID").toInt();
            row.RCoord = (*It).RTile;
            row.CCoord = (*It).CTile;
            row.TCoord = (*It).TimePoint;
            row.YCoord = (*It).YOffset;
            row.XCoord = (*It).XOffset;
            row.ZCoord = (*It).ZDepth;
            row.filename = (*It).Filename.c_str();

            RecordSet->AddObject( row );

            It++;
            }

          if( !RecordSet->SaveInDB() )
            {
            return;
            }
          delete RecordSet;
          }
         
        catch( const itk::ExceptionObject& e )
          {
          std::cerr << " caught an ITK exception: " << std::endl;
          e.Print( std::cerr);
          return;
          }
        catch( const std::exception& e )
          {
          std::cerr << " caught an std exception: " << std::endl;
          std::cerr << e.what() << std::endl;
          return;
          }
        catch( ... )
          {
          std::cerr << " caught an unknown exception!" << std::endl;
          return;
          }
        }      
      }
    }
    
  completeChanged();
  if (isComplete())
  {
    BrowseButton->hide();
    Explanation->setText(tr("When you click on finish, the Image Set of the Experiment %1 from the DataBase %2 will be opened.")
    .arg( field("Name").toString() ).arg( field("NameDB").toString() ) );
  }
  //setSubTitle(tr("When you click on finish, the Image Set of the Experiment %1 from the DataBase %2 will be opened.")
    //.arg( field("Name").toString() ).arg( field("NameDB").toString() ) );
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool Import_SerieGridPage::isComplete() const
{
  return ( !newfilename.isEmpty() );
}
