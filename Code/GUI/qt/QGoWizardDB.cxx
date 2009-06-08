#include "QGoWizardDB.h"
#include "CreateDataBaseHelper.h"
#include "GoDBRecordSet.h"
#include "GoDBFigureRow.h"
#include "GoDBExperimentRow.h"
#include "GoDBSeriesGridRow.h"
#include "itkMegaCaptureImport.h"
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

//------------------------------------------------------------------------------
QGoWizardDB::QGoWizardDB( QWidget *parent )
: QWizard( parent )
{
  QFont font;
  font.setBold(true);
  this->setFont(font);

  QFont font2;
  font2.setBold(false);

  QPushButton* nextButton = new QPushButton(tr("Next"));
  nextButton->setFont(font2);
  this->setButton ( QWizard::NextButton, nextButton );
  QPushButton* backButton = new QPushButton(tr("Back"));
  backButton->setFont(font2);
  this->setButton ( QWizard::BackButton, backButton );
  this->setOptions(QWizard::NoCancelButton);
  QPushButton* finishButton = new QPushButton(tr("Finish"));
  finishButton->setFont(font2);
  this->setButton ( QWizard::FinishButton, finishButton );

  addPage( new Connect_ServerPage );
  addPage( new OpenOrCreate_Page);
  addPage( new Create_ExperimentPage);
  addPage( new Import_SerieGridPage);
  addPage( new Finish_Page);
  setWindowTitle( tr("Use DataBase") );
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------

std::vector<std::string> QGoWizardDB::ListFilenames()
{
  std::vector<std::string> listFilenames = ListValuesfor1Column(
  field("ServerName").toString().toStdString(),
        field("User").toString().toStdString(),
        field("Password").toString().toStdString(),
        field("NameDB").toString().toStdString(),"seriesgrid",
        "filename","experimentID",field("ExpID").toString().toStdString());
  return listFilenames;
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
        field("Password").toString().toStdString()))
    {
      QMessageBox msgBox;
      msgBox.setText(tr
   ("Unable to connect to the server: please make sure you entered the right fields."));
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
void OpenOrCreate_Page::initializePage()
{
    this->setSubTitle(tr("Step 2: Chose what you want to do next:"));
    openDBRadioButton->show();
    createDBRadioButton->show();
  
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
  ChoiceDB->hide();
  textChoiceDB = new QLabel(tr("Name of the DB to open :"));
  textChoiceDB->hide();

  openDBRadioButton = new QRadioButton(tr("Open an existing DataBase"));
  openDBRadioButton->setChecked(false);
  createDBRadioButton = new QRadioButton(tr("Create a new DataBase"));
  createDBRadioButton->setChecked(false);
  textNewDBName = new QLabel(tr("Name of the new DB to create:"));
  textNewDBName->hide();
  lineNewDBName = new QLineEdit;
  lineNewDBName->hide();

  gridLayout->addWidget(createDBRadioButton,0,0,1,1);
  gridLayout->addWidget(textNewDBName,3,0,1,2);
  gridLayout->addWidget(lineNewDBName,3,1,1,1);
  gridLayout->addWidget(openDBRadioButton,5,0,1,1);
  gridLayout->addWidget(textChoiceDB,6,0,1,1);
  gridLayout->addWidget(ChoiceDB,6,1,1,1);

  gridLayout->setAlignment(openDBRadioButton,Qt::AlignLeft);
  gridLayout->setAlignment(createDBRadioButton,Qt::AlignLeft);
  gridLayout->setAlignment(textNewDBName,Qt::AlignLeft);
  gridLayout->setAlignment(textChoiceDB,Qt::AlignLeft);

  setLayout(gridLayout);

  QObject::connect( this->openDBRadioButton,SIGNAL( clicked() ),
  this,SLOT( PrintListDB() ));

  QObject::connect( this->createDBRadioButton,SIGNAL( clicked() ),
  this,SLOT( EnterNameDB() ));

  registerField( "DBIndextoOpen", ChoiceDB);
  registerField("DBNametoCreate",lineNewDBName);
 }
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void OpenOrCreate_Page::PrintListDB ()
{
  textChoiceDB->show();
  ChoiceDB->show();

  textNewDBName->hide();
  lineNewDBName->hide();
  lineNewDBName->setText("");



  if (m_ListDB.isEmpty())
  {
    std::vector<std::string> vectListDB =
      ListDataBases(
        field("ServerName").toString().toStdString(),
        field("User").toString().toStdString(),
        field("Password").toString().toStdString()
        );

    for(unsigned int i = 0; i < vectListDB.size(); ++i )
      {
      if( IsDatabaseOfGoFigureType(
            field("ServerName").toString().toStdString(),
            field("User").toString().toStdString(),
            field("Password").toString().toStdString(),
            vectListDB[i].c_str( )
            )
          )
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
  m_ListDB.clear();
  ChoiceDB->clear();

  lineNewDBName->show();
  textNewDBName->show();
  ChoiceDB->hide();
  textChoiceDB->hide();
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
bool OpenOrCreate_Page::validatePage()
{
  QString NameDB;
  DBNametoOpen_fake = new QLineEdit;
  bool Ok;
  int i = field("DBIndextoOpen").toInt();
  if( ( i > -1 ) && ( i < m_ListDB.size() ) )
    {
    NameDB = m_ListDB[i];
    }
  else
    {
    NameDB = "Null";
    }

  if (field("DBNametoCreate").toString().isEmpty()&& NameDB=="Null")
    {
    Ok = false;
    }
  else
    {
     if (NameDB=="Null")
     {
       Ok = true;
     }
     else
     {
      if( !IsDatabaseOfGoFigureType(field("ServerName").toString().toStdString(),
            field("User").toString().toStdString(),
            field("Password").toString().toStdString(),
            NameDB.toStdString() ) )
        {
        
        QMessageBox msgBox;
        msgBox.setText(tr("The Database %1 is not a Gofigure Database").arg(NameDB));
        msgBox.exec();
        //setTitle(tr("The Database %1 is not a Gofigure Database").arg(NameDB));
        Ok = false;
        }
      else
        {
        registerField( "DBNametoOpen", DBNametoOpen_fake );
        setField( "DBNametoOpen",NameDB );
        Ok = true;
        }
      }
    }
 return Ok;
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
  openExpRadioButton->setChecked(false);
  createExpRadioButton->setChecked(false);

  QVBoxLayout* vlayout = new QVBoxLayout;
  QVBoxLayout* RadioButtonLayout = new QVBoxLayout;
  RadioButtonLayout->addWidget(createExpRadioButton);
  //vlayout->setAlignment(createExpRadioButton,Qt::AlignHCenter);
  RadioButtonLayout->addWidget(openExpRadioButton);
  //vlayout->setAlignment(openExpRadioButton,Qt::AlignHCenter);
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
  OpenOrCreateExp_fake = new QLineEdit;

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

  ChoiceExp->setVisible(false);
  textChoiceExp->setVisible(false);
  ExpID->setVisible(false);
  ID->setVisible(false);
  ID->setEnabled(false);
  Name ->setEnabled(false);
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


}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void Create_ExperimentPage::initializePage()
{
  openExpRadioButton->setChecked(false);
  createExpRadioButton->setChecked(false);

  NameDB_fake = new QLineEdit;
  registerField("NameDB",NameDB_fake);
 

  if (!field("DBNametoCreate").toString().isEmpty())
    {
    CreateDataBaseMain(field("ServerName").toString().toStdString(),
      field("User").toString().toStdString(),
      field("Password").toString().toStdString(),
      field("DBNametoCreate").toString().toStdString());
    setSubTitle(tr("You are currently using The Database %1 ").arg(field("DBNametoCreate").toString()));
    setField("NameDB",field("DBNametoCreate"));
    }
  else
    {
    setSubTitle(tr("You are currently using The Database %1 ").arg(field("DBNametoOpen").toString()));
    setField("NameDB",field("DBNametoOpen"));
    }
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
 }
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void Create_ExperimentPage::PrintListExp()
{

  textChoiceExp->setVisible(true);
  ChoiceExp->setVisible(true);
  //ExpID->setVisible(true);
  //ID->setVisible(true);
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
      ListExpName(
        field("ServerName").toString().toStdString(),
        field("User").toString().toStdString(),
        field("Password").toString().toStdString(),
        field("NameDB").toString().toStdString());

   if (!vectListExpName.empty())
   {
   for(unsigned int i = 0; i < vectListExpName.size(); ++i )
      {
        m_ListExpName.append( vectListExpName[i].c_str( ) );
      }
   }

    ChoiceExp->addItems( m_ListExpName );
    ChoiceExp->show();

    setField("OpenOrCreateExp","Open");



}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------


void Create_ExperimentPage::PrintValuesExpName(QString ExpName)
{
  
  std::vector<std::string> myvect =
    ListValuesfor1Row(
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

  //setField("ExpID", ExpID.toInt());
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
bool Create_ExperimentPage::validatePage()
{
 if (createExpRadioButton->isChecked())
 {
     if (field("Name")=="")
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
          field("NameDB").toString().toStdString());

         if (!vectListExpName.empty())
         {
          for(unsigned int i = 0; i < vectListExpName.size(); ++i )
          {
           ListExistingNames.append( vectListExpName[i].c_str( ) );
          }
         }
         if (ListExistingNames.contains ( field("Name").toString(),Qt::CaseInsensitive ))
         {
             QMessageBox msgBox;
             msgBox.setText(tr("The name you entered for your experiment already exists."));
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
    if (field("Name")=="")
    {
     QMessageBox msgBox;
     msgBox.setText(tr
       ("There is no existing Experiment in this DataBase, please create a new one."));
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

  gridlayout= new QGridLayout;
  OpenOrCreateSeriesGrid_fake = new QLineEdit;
  BrowseButton = new QPushButton("&Browse", this);

  openSeriesGridRadioButton = new QRadioButton(tr("&Open the existing Image Set"));
  ImportSeriesGridRadioButton = new QRadioButton(tr("&Import a new Image Set"));

  gridlayout->addWidget( ImportSeriesGridRadioButton );
  gridlayout->addWidget( BrowseButton,0,1 );
  gridlayout->addWidget( openSeriesGridRadioButton );
  gridlayout->setColumnStretch ( 0, 3);
  gridlayout->setColumnStretch ( 1, 1);


  setLayout(gridlayout);

  registerField("OpenOrCreateSeriesGrid",OpenOrCreateSeriesGrid_fake);

  QObject::connect( this->ImportSeriesGridRadioButton,SIGNAL( clicked() ),
  this,SLOT( SelectImportSeriesGrid() ));

  QObject::connect( this->openSeriesGridRadioButton,SIGNAL( clicked() ),
  this,SLOT( SelectOpenSeriesGrid() ));

  QObject::connect( this->BrowseButton,SIGNAL( clicked() ),
  this,SLOT( EnterInfoSeriesGrid() ));


}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void Import_SerieGridPage::initializePage()
{
  BrowseButton->setVisible(false);
  if (field("OpenOrCreateExp")=="Create")
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
      field("NameDB").toString().toStdString(),"experiment", myNewObject);

    std::vector<std::string> vectListExpID =
      ListExpID(
        field("ServerName").toString().toStdString(),
        field("User").toString().toStdString(),
        field("Password").toString().toStdString(),
        field("NameDB").toString().toStdString());

    setField( "ExpID", (unsigned int)vectListExpID.size() );
    }

  setSubTitle(
    tr("Experiment: '%1'. DataBase: '%2'")
     .arg(field("Name").toString()).arg(field("NameDB").toString()));

  std::vector<std::string> myvect =
    ListValuesfor1Row(
        field("ServerName").toString().toStdString(),
        field("User").toString().toStdString(),
        field("Password").toString().toStdString(),
        field("NameDB").toString().toStdString(),"seriesgrid",
        "experimentID",field("ExpID").toString().toStdString());

  if (myvect.empty())
  {
    BrowseButton->setVisible(true);
    ImportSeriesGridRadioButton->setChecked(true);
    openSeriesGridRadioButton->setVisible(false);
  }
  else
  {
    BrowseButton->setVisible(false);
    openSeriesGridRadioButton->setVisible(true);
  }
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void Import_SerieGridPage::SelectImportSeriesGrid()
{
    BrowseButton->setVisible(true);
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void Import_SerieGridPage::SelectOpenSeriesGrid()
{
    BrowseButton->setVisible(false);
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void Import_SerieGridPage::EnterInfoSeriesGrid()
{
  QString filename = QFileDialog::getOpenFileName(
    this,
    tr( "Import Image" ),"",
    tr( "Images (*.png *.bmp *.jpg *.jpeg *.tiff *.mha *.mhd *.img *.lsm)" )
    );

  std::vector<std::string> myvect =
    ListValuesfor1Row(
        field("ServerName").toString().toStdString(),
        field("User").toString().toStdString(),
        field("Password").toString().toStdString(),
        field("NameDB").toString().toStdString(),"seriesgrid",
        "experimentID",field("ExpID").toString().toStdString());
  
  if (!myvect.empty())
  {
    QMessageBox msgBox;
     msgBox.setText(tr
       ("There is already an Image Set for this Experiment."));
     msgBox.exec();
     return;
  }
  else
  {
  if( !filename.isEmpty( ) )
  {
    try
    {
    itk::MegaCaptureImport::Pointer  importFileInfoList = itk::MegaCaptureImport::New();
    importFileInfoList->SetFileName( filename.toAscii().data() );
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
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------

Finish_Page::Finish_Page(QWidget *parent)
:QWizardPage(parent)
{
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------

void Finish_Page::initializePage()
{
  setTitle(tr("When you click on finish, the Image Set of the Experiment %1 from the DataBase %2 will be opened.")
  .arg(field("Name").toString()).arg(field("NameDB").toString()));
}

/*std::vector<std::string> Finish_Page::ListFilenames()
{
  std::vector<std::string> listFilenames = ListValuesfor1Column(
  field("ServerName").toString().toStdString(),
        field("User").toString().toStdString(),
        field("Password").toString().toStdString(),
        field("NameDB").toString().toStdString(),"seriesgrid",
        "filename","experimentID",field("ExpID").toString().toStdString());
  return listFilenames;
}*/

//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
