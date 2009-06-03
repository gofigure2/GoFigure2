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

//------------------------------------------------------------------------------
QGoWizardDB::QGoWizardDB( QWidget *parent )
: QWizard( parent )
{
  addPage( new Connect_ServerPage );
  addPage( new OpenOrCreate_Page);
  addPage( new Create_ExperimentPage);
  addPage( new Import_SerieGridPage);
  addPage( new Import_ManualSegmentationPage);
  setWindowTitle( tr("Use DataBase") );
};
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
Connect_ServerPage::Connect_ServerPage( QWidget *parent )
: QWizardPage( parent )
{
  setTitle( tr("Step 1: Connect to a MySQL DataBase Server."));
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

};
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void OpenOrCreate_Page::initializePage()
{
  if( !CanConnectToServer(
        field("ServerName").toString().toStdString(),
        field("User").toString().toStdString(),
        field("Password").toString().toStdString()))
    {
    this->setSubTitle(tr("Unable to connect to server: please go back and make sure you have the right Server Name"));
    openDBRadioButton->hide();
    createDBRadioButton->hide();
    }
  else
    {
    this->setTitle(tr("Step 2: Chose what you want to do next:"));
    openDBRadioButton->show();
    createDBRadioButton->show();
    }
};
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
OpenOrCreate_Page::OpenOrCreate_Page(QWidget *parent)
:QWizardPage(parent)
{
  gridLayout = new QGridLayout;
  ChoiceDB = new QComboBox;
  ChoiceDB->hide();
  textChoiceDB = new QLabel(tr("Name of the DB to open"));
  textChoiceDB->hide();

  openDBRadioButton = new QRadioButton(tr("Open an exisiting DataBase"));
  openDBRadioButton->setChecked(false);
  createDBRadioButton = new QRadioButton(tr("Create a new DataBase        "));
  createDBRadioButton->setChecked(false);
  textNewDBName = new QLabel(tr("Name of the new DB to create:"));
  textNewDBName->hide();
  lineNewDBName = new QLineEdit;
  lineNewDBName->hide();

  gridLayout->addWidget(createDBRadioButton,0,0,1,2);
  gridLayout->addWidget(textNewDBName,3,0);
  gridLayout->addWidget(lineNewDBName,3,1);
  gridLayout->addWidget(openDBRadioButton,5,0,1,2);
  gridLayout->addWidget(textChoiceDB,6,0);
  gridLayout->addWidget(ChoiceDB,6,1);

  gridLayout->setAlignment(openDBRadioButton,Qt::AlignHCenter);
  gridLayout->setAlignment(createDBRadioButton,Qt::AlignHCenter);

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
    std::cout<<"this is the name of the DB to open: "<< NameDB.toAscii().data() << std::endl;
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
        setTitle(tr("The Database %1 is not a Gofigure Database").arg(NameDB));
        Ok = false;
        }
      else
        {
        registerField( "DBNametoOpen", DBNametoOpen_fake );
        setField( "DBNametoOpen",NameDB );
        std::cout << "DBNametoOpen is " << field( "DBNametoOpen" ).toString().toAscii().data() << std::endl;
        Ok = true;
        }
      }
    }
 return Ok;
};
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
Create_ExperimentPage::Create_ExperimentPage( QWidget *parent )
: QWizardPage( parent )
{
  formLayout = new QFormLayout;
  ChoiceExp  = new QComboBox;
  ExpID = new QLabel(tr("ExperimentID"));
  ID = new QLineEdit;
  textName = new QLabel(tr("Name"));
  Name = new QLineEdit;

  openExpRadioButton   = new QRadioButton(tr("Open an existing Experiment"));
  createExpRadioButton = new QRadioButton(tr("Create a new Experiment"));
  openExpRadioButton->setChecked(false);
  createExpRadioButton->setChecked(false);

  QVBoxLayout* vlayout = new QVBoxLayout;
  vlayout->addWidget(createExpRadioButton);
  vlayout->addWidget(openExpRadioButton);
  textChoiceExp = new QLabel(tr("Experiment to open:"));
  QGridLayout* gridlayout = new QGridLayout;
  gridlayout->addWidget(textChoiceExp,0,0);
  gridlayout->addWidget(ChoiceExp,0,1);
  gridlayout->addWidget(ExpID,1,0);
  gridlayout->addWidget(ID,1,1);
  gridlayout->addWidget(textName,3,0);
  gridlayout->addWidget(Name,3,1);
  vlayout->addLayout(gridlayout);

  Description  = new QLineEdit;
  TimeInterval = new QLineEdit;
  TileHeight   = new QLineEdit;
  TileWidth    = new QLineEdit;
  PixelDepth   = new QLineEdit;
  PixelHeight  = new QLineEdit;
  PixelWidth   = new QLineEdit;
  ColorDepth   = new QLineEdit;
  nTimePoints  = new QLineEdit;
  nYTiles = new QLineEdit;
  nXTiles = new QLineEdit;
  nSlices = new QLineEdit;
  nRows       = new QLineEdit;
  nColumns    = new QLineEdit;
  FilePattern = new QLineEdit;
  OpenOrCreateExp_fake = new QLineEdit;

  formLayout->addRow( tr("&Description:"),  Description );
  formLayout->addRow( tr("&TimeInterval:"), TimeInterval );
  formLayout->addRow( tr("&TileHeight:"),   TileHeight );
  formLayout->addRow( tr("&TileWidth:"),    TileWidth );
  formLayout->addRow( tr("&PixelDepth:"),   PixelDepth );
  formLayout->addRow( tr("&PixelHeight:"),  PixelHeight );
  formLayout->addRow( tr("&PixelWidth:"),   PixelWidth );
  formLayout->addRow( tr("&ColorDepth:"),   ColorDepth );
  formLayout->addRow( tr("&nTimePoints:"),  nTimePoints );
  formLayout->addRow( tr("&nYTiles:"),      nYTiles );
  formLayout->addRow( tr("&nXTiles:"),      nXTiles );
  formLayout->addRow( tr("&nSlices:"),      nSlices );
  formLayout->addRow( tr("&nRows:"),        nRows );
  formLayout->addRow( tr("&nColumns:"),     nColumns );
  formLayout->addRow( tr("&FilePattern:"),  FilePattern );

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

  vlayout->addLayout(formLayout);

  setLayout( vlayout );

  QObject::connect( this->openExpRadioButton,SIGNAL( clicked() ),
  this,SLOT( PrintListExp() ));

  QObject::connect( this->createExpRadioButton,SIGNAL( clicked() ),
  this,SLOT( EnterInfoExp() ));

  QObject::connect( this->ChoiceExp,SIGNAL( currentIndexChanged(QString) ),
  this,SLOT( PrintValuesExpName(QString) ));


};
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
    setTitle(tr("You are currently using The Database %1 ").arg(field("DBNametoCreate").toString()));
    setField("NameDB",field("DBNametoCreate"));
    }
  else
    {
    setTitle(tr("You are currently using The Database %1 ").arg(field("DBNametoOpen").toString()));
    setField("NameDB",field("DBNametoOpen"));
    }
};
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
 };
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void Create_ExperimentPage::PrintListExp()
{

  textChoiceExp->setVisible(true);
  ChoiceExp->setVisible(true);
  ExpID->setVisible(true);
  ID->setVisible(true);
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

    std::vector<std::string> vectListExpID =
      ListExpID(
        field("ServerName").toString().toStdString(),
        field("User").toString().toStdString(),
        field("Password").toString().toStdString(),
        field("NameDB").toString().toStdString());

   if (!vectListExpID.empty())
   {
   for(unsigned int i = 0; i < vectListExpID.size(); ++i )
      {
        m_ListExpID.append( vectListExpID[i].c_str( ) );
      }
   }
    ChoiceExp->addItems( m_ListExpName );
    ChoiceExp->show();

    setField("OpenOrCreateExp","Open");



};
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------


void Create_ExperimentPage::PrintValuesExpName(QString ExpName)
{
  std::cout<<ExpName.toAscii().data()<<std::endl;

  std::vector<std::string> myvect =
    ListValuesfor1Row(
        field("ServerName").toString().toStdString(),
        field("User").toString().toStdString(),
        field("Password").toString().toStdString(),
        field("NameDB").toString().toStdString(),"experiment",
        "name",ExpName.toStdString());

  std::cout<<"Nb of values in myvect: "<< myvect.size()<<std::endl;

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
 return openExpRadioButton->isChecked() != createExpRadioButton->isChecked();
}

//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
Import_SerieGridPage::Import_SerieGridPage( QWidget *parent )
: QWizardPage( parent )
{
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


    std::cout<<"myNewObject.name of my exp is : "<<myNewObject.name<<std::endl;
    std::cout<<"myNewObject.tileWidth of my exp is : "<<myNewObject.tileWidth<<std::endl;

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
    std::cout << "create" << std::endl;
    std::cout << field("ExpID").toString().toAscii().data() << std::endl;
    }
  else
    {
    std::cout<<"open"<<std::endl;
    std::cout<<field("ExpID").toString().toAscii().data()<<std::endl;
    }
  setTitle(
    tr("Experiment %1,'%2' from the database %3").arg(field("ExpID")
     .toString()).arg(field("Name").toString()).arg(field("NameDB").toString()));
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

  if( !filename.isEmpty( ) )
  {
  try
    {
    itk::MegaCaptureImport::Pointer  importFileInfoList = itk::MegaCaptureImport::New();
    importFileInfoList->SetFileName( filename.toAscii().data() );
    importFileInfoList->Update();

    typedef FileListType::iterator myFilesIteratorType;
    myFilesIteratorType It  = importFileInfoList->GetOutput()->begin();
    myFilesIteratorType end = importFileInfoList->GetOutput()->end();

    while( It != end )
      {
      GoDBSeriesGridRow myNewImage;

      myNewImage.experimentID = field("ExpID").toInt();
      myNewImage.RCoord = (*It).RTile;
      myNewImage.CCoord = (*It).CTile;
      myNewImage.TCoord = (*It).TimePoint;
      myNewImage.YCoord = (*It).YOffset;
      myNewImage.XCoord = (*It).XOffset;
      myNewImage.ZCoord = (*It).ZDepth;
      myNewImage.filename = (*It).Filename.c_str();

      RecordValues_inTable<GoDBSeriesGridRow>(field("ServerName").toString().toStdString(),
      field("User").toString().toStdString(),
      field("Password").toString().toStdString(),
      field("NameDB").toString().toStdString(),"seriesgrid", myNewImage);

      It++;
      }
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

  setField("OpenOrCreateSeriesGrid","Create");
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------

Import_ManualSegmentationPage::Import_ManualSegmentationPage(QWidget *parent)
:QWizardPage(parent)
{
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------

void Import_ManualSegmentationPage::initializePage()
{
  /*if (field("OpenOrCreateSeriesGrid")=="Create")
    {
        GoDBSeriesGridRow myNewObject;

        myNewObject.experimentID = field("ExpID").toInt();
        myNewObject.RCoord  = field("RCoord").toInt();
        myNewObject.CCoord = field("CCoord").toInt();
        myNewObject.TCoord   = field("TCoord").toInt();
        myNewObject.YCoord    = field("YCoord").toInt();
        myNewObject.XCoord   = field("XCoord").toInt();
        myNewObject.ZCoord  = field("ZCoord").toInt();
        myNewObject.filename   = field("filename").toString().toAscii().data();



        std::cout<<"myNewObject.experimentID of my exp is : "<<myNewObject.experimentID<<std::endl;
        std::cout<<"myNewObject.filename of my exp is : "<<myNewObject.filename<<std::endl;

          RecordValues_inTable< GoDBSeriesGridRow >(
          field("ServerName").toString().toAscii().data(),
          field("User").toString().toAscii().data(),
          field("Password").toString().toAscii().data(),
          field("NameDB").toString().toAscii().data(),"seriesgrid", myNewObject);


       std::vector<std::string> vectListExpID =
        ListExpID(
          field("ServerName").toString().toStdString(),
          field("User").toString().toStdString(),
          field("Password").toString().toStdString(),
          field("NameDB").toString().toStdString());

      setField("ImageID",(unsigned int)vectListExpID.size());
      std::cout<<"create"<<std::endl;
      std::cout<<field("ExpID").toString().toAscii().data()<<std::endl;

    }
  else
  {

  }*/
   setTitle(tr("Experiment %1,'%2' from the database %3").arg(field("ExpID")
     .toString()).arg(field("Name").toString()).arg(field("NameDB").toString()));
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
