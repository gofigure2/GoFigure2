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

//------------------------------------------------------------------------------
QGoWizardDB::QGoWizardDB( QWidget *parent )
: QWizard( parent )
{
  addPage( new Connect_ServerPage );
  addPage( new OpenOrCreate_Page);
  addPage( new Create_ExperimentPage);
  addPage( new Import_SerieGridPage);
  addPage( new Import_ManualSegmentationPage);
  setWindowTitle( tr("My First wizard") );
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
   //  pb when adding the variables Server,User,Password :
   //  when clicking next/back between the 2 pages, can connect
   //  sometimes.(once every 4 try...) :

   // Server = field("ServerName").toString().toAscii().data() );
   // std::cout<<Server<<std::endl;
   // User = field("User").toString().toAscii().data();
   // std::cout<<User<<std::endl;
   // Password = field("Password").toString().toAscii().data();
   // std::cout<<Password<<std::endl;

  if( !CanConnectToServer(
        field("ServerName").toString().toAscii().data(),
        field("User").toString().toAscii().data(),
        field("Password").toString().toAscii().data()))
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
  formLayout = new QFormLayout;
  ChoiceDB = new QComboBox;

  openDBRadioButton = new QRadioButton(tr("&Open an existing Database"));
  createDBRadioButton = new QRadioButton(tr("&Create a new Database"));
  createDBRadioButton->setChecked(true);
  lineNewDBName = new QLineEdit;

  formLayout->addWidget(createDBRadioButton);
  formLayout->addRow( tr("&Name of the new DB to create:"), lineNewDBName );
  formLayout->addWidget(openDBRadioButton);
  formLayout->addRow(tr("Name of the DB to open:"),ChoiceDB);

  setLayout(formLayout);

  QObject::connect( this->openDBRadioButton,SIGNAL( clicked() ),
  this,SLOT( PrintListDB() ));

  QObject::connect( this->createDBRadioButton,SIGNAL( clicked() ),
  this,SLOT( EnterNameDB() ));
  registerField( "DBIndextoOpen", ChoiceDB);
  registerField("DBNametoCreate",lineNewDBName);
 };
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void OpenOrCreate_Page::PrintListDB ()
{
  lineNewDBName->hide();
  lineNewDBName->setText("");

  if (m_ListDB.isEmpty())
  {
    std::vector<std::string> vectListDB =
      ListDataBases(
        field("ServerName").toString().toAscii().data(),
        field("User").toString().toAscii().data(),
        field("Password").toString().toAscii().data()
        );

    for(unsigned int i = 0; i < vectListDB.size(); ++i )
      {
      if( IsDatabaseOfGoFigureType(
            field("ServerName").toString().toAscii().data(),
            field("User").toString().toAscii().data(),
            field("Password").toString().toAscii().data(),
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
};
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void OpenOrCreate_Page::EnterNameDB ()
{
  m_ListDB.clear();
  ChoiceDB->clear();
  lineNewDBName->show();
  ChoiceDB->hide();
};
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
      if( !IsDatabaseOfGoFigureType(field("ServerName").toString().toAscii().data(),
            field("User").toString().toAscii().data(),
            field("Password").toString().toAscii().data(),
            NameDB.toAscii().data() ) )
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

  openExpRadioButton   = new QRadioButton( tr("&Open an existing Experiment") );
  createExpRadioButton = new QRadioButton( tr("&Create a new Experiment")     );
  openExpRadioButton->setChecked(false);
  createExpRadioButton->setChecked(false);

  formLayout->addWidget( createExpRadioButton );
  formLayout->addWidget( openExpRadioButton );
  formLayout->addRow( tr("Experiment to open:"), ChoiceExp );

//  ExperimentID = new QLineEdit;
  Name         = new QLineEdit;
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
  ExpID_fake = new QLineEdit;

 // formLayout->addRow( tr("&ExperimentID:"), ExperimentID );
  formLayout->addRow( tr("&Name:"),         Name );
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
  registerField("ExpID", ExpID_fake);



  setLayout( formLayout );

  QObject::connect( this->openExpRadioButton,SIGNAL( clicked() ),
  this,SLOT( PrintListExp() ));

  QObject::connect( this->createExpRadioButton,SIGNAL( clicked() ),
  this,SLOT( EnterInfoExp() ));

  QObject::connect( this->ChoiceExp,SIGNAL( currentIndexChanged(QString) ),
  this,SLOT( PrintValuesExpID(QString) ));


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
    CreateDataBaseMain(field("ServerName").toString().toAscii().data(),
      field("User").toString().toAscii().data(),
      field("Password").toString().toAscii().data(),
      field("DBNametoCreate").toString().toAscii().data());
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

  ChoiceExp->setVisible(false);
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


  ChoiceExp->setVisible(true);
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


    m_ListExpID.clear();
    ChoiceExp->clear();

    std::vector<std::string> vectListExpID =
      ListExpID(
        field("ServerName").toString().toAscii().data(),
        field("User").toString().toAscii().data(),
        field("Password").toString().toAscii().data(),
        field("NameDB").toString().toAscii().data());


   for(unsigned int i = 0; i < vectListExpID.size(); ++i )
      {
        m_ListExpID.append( vectListExpID[i].c_str( ) );
      }
    ChoiceExp->addItems( m_ListExpID );
    ChoiceExp->show();

    setField("OpenOrCreateExp","Open");



};
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------


void Create_ExperimentPage::PrintValuesExpID(QString ExpID)
{
  std::cout<<ExpID.toAscii().data()<<std::endl;

  std::vector<std::string> myvect =
    ListValuesforID(
        field("ServerName").toString().toAscii().data(),
        field("User").toString().toAscii().data(),
        field("Password").toString().toAscii().data(),
        field("NameDB").toString().toAscii().data(),"experiment",
        "experimentID",ExpID.toLatin1().data());

  std::cout<<"Nb of values in myvect: "<< myvect.size()<<std::endl;

  if ( !myvect.empty() )
    {
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

  setField("ExpID", ExpID.toInt());
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
  formlayout= new QFormLayout;
  RCoord    = new QLineEdit;
  CCoord    = new QLineEdit;
  TCoord    = new QLineEdit;
  YCoord    = new QLineEdit;
  XCoord    = new QLineEdit;
  ZCoord    = new QLineEdit;
  FileName  = new QLineEdit;
  OpenOrCreateSeriesGrid_fake = new QLineEdit;
  ImageID_fake = new QLineEdit;
  BrowseButton = new QPushButton("&Browse", this);

  ChoiceSeriesGrid = new QComboBox;
  openSeriesGridRadioButton = new QRadioButton(tr("&Open an existing SeriesGrid"));
  //createSeriesGridRadioButton = new QRadioButton(tr("&Import a new SeriesGrid"));

  //formlayout->setHorizontalSpacing(200);

  //formlayout->addWidget( createSeriesGridRadioButton );
  formlayout->addRow( tr("Import MultiFiles: "),BrowseButton );
  formlayout->addWidget( openSeriesGridRadioButton );
  formlayout->addRow( tr("SeriesGrid to open:"), ChoiceSeriesGrid );

  formlayout->addRow("FileName: ",FileName);
  formlayout->addRow("RCoord: ",RCoord);
  formlayout->addRow("CCoord: ",CCoord);
  formlayout->addRow("TCoord: ",TCoord);
  formlayout->addRow("YCoord: ",YCoord);
  formlayout->addRow("XCoord: ",XCoord);
  formlayout->addRow("ZCoord: ",ZCoord);

  setLayout(formlayout);

  registerField("RCoord",RCoord);
  registerField("CCoord",CCoord);
  registerField("TCoord",TCoord);
  registerField("YCoord",YCoord);
  registerField("XCoord",XCoord);
  registerField("ZCoord",ZCoord);
  registerField("FileName",FileName);
  registerField("OpenOrCreateSeriesGrid",OpenOrCreateSeriesGrid_fake);
  registerField("ImageID",ImageID_fake);

  QObject::connect( this->openSeriesGridRadioButton,SIGNAL( clicked() ),
  this,SLOT( PrintListSeriesGrid() ));

  //QObject::connect( this->createSeriesGridRadioButton,SIGNAL( clicked() ),
  //this,SLOT( EnterInfoSeriesGrid() ));

  QObject::connect( this->BrowseButton,SIGNAL( clicked() ),
  this,SLOT( EnterInfoSeriesGrid() ));

  QObject::connect( this->ChoiceSeriesGrid,SIGNAL( currentIndexChanged(int) ),
  this,SLOT( PrintValuesImageID(int) ));

}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void Import_SerieGridPage::initializePage()
{

  if (field("OpenOrCreateExp")=="Create")
    {

    GoDBExperimentRow myNewObject;

    myNewObject.name         = field("Name").toString().toAscii().data();
    myNewObject.description  = field("Description").toString().toAscii().data();
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
    myNewObject.filePattern  = field("FilePattern").toString().toAscii().data();


    std::cout<<"myNewObject.name of my exp is : "<<myNewObject.name<<std::endl;
    std::cout<<"myNewObject.tileWidth of my exp is : "<<myNewObject.tileWidth<<std::endl;

    RecordValues_inTable< GoDBExperimentRow >(
      field("ServerName").toString().toAscii().data(),
      field("User").toString().toAscii().data(),
      field("Password").toString().toAscii().data(),
      field("NameDB").toString().toAscii().data(),"experiment", myNewObject);

    std::vector<std::string> vectListExpID =
      ListExpID(
        field("ServerName").toString().toAscii().data(),
        field("User").toString().toAscii().data(),
        field("Password").toString().toAscii().data(),
        field("NameDB").toString().toAscii().data());

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


void Import_SerieGridPage::PrintListSeriesGrid()
{
  m_ListImageID.clear();
  ChoiceSeriesGrid->clear();

  ChoiceSeriesGrid->setVisible(true);
  RCoord->setEnabled(false);
  CCoord->setEnabled(false);
  TCoord->setEnabled(false);
  YCoord->setEnabled(false);
  XCoord->setEnabled(false);
  ZCoord->setEnabled(false);
  FileName->setEnabled(false);


  std::vector<std::string> vectListImageID = ListImageIDforExpID(
    field("ServerName").toString().toAscii().data(),
        field("User").toString().toAscii().data(),
        field("Password").toString().toAscii().data(),
        field("NameDB").toString().toAscii().data(),
        field("ExpID").toString().toAscii().data());

   for(unsigned int i = 0; i < vectListImageID.size(); ++i )
      {
        m_ListImageID.append( vectListImageID[i].c_str( ) );
      }

    ChoiceSeriesGrid->addItems( m_ListImageID );
    ChoiceSeriesGrid->show();

  setField("OpenOrCreateSeriesGrid","Open");

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
      setField("RCoord",myNewImage.RCoord);
      myNewImage.CCoord = (*It).CTile;
      setField("CCoord",myNewImage.CCoord);
      myNewImage.TCoord = (*It).TimePoint;
      setField("TCoord",myNewImage.TCoord);
      myNewImage.YCoord = (*It).YOffset;
      setField("YCoord",myNewImage.YCoord);
      myNewImage.XCoord = (*It).XOffset;
      setField("XCoord",myNewImage.XCoord);
      myNewImage.ZCoord = (*It).ZDepth;
      setField("ZCoord",myNewImage.ZCoord);
      myNewImage.filename = (*It).Filename.c_str();

      RecordValues_inTable<GoDBSeriesGridRow>(field("ServerName").toString().toAscii().data(),
      field("User").toString().toAscii().data(),
      field("Password").toString().toAscii().data(),
      field("NameDB").toString().toAscii().data(),"seriesgrid", myNewImage);

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



  ChoiceSeriesGrid->setVisible(false);
  RCoord->setEnabled(true);
  CCoord->setEnabled(true);
  TCoord->setEnabled(true);
  YCoord->setEnabled(true);
  XCoord->setEnabled(true);
  ZCoord->setEnabled(true);
  FileName->setEnabled(true);

  setField("OpenOrCreateSeriesGrid","Create");
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------

void Import_SerieGridPage::PrintValuesImageID(int ImageID)
{
  if( ImageID != -1 )
  {
  std::vector<std::string> myvect =
    ListValuesforID(
        field("ServerName").toString().toAscii().data(),
        field("User").toString().toAscii().data(),
        field("Password").toString().toAscii().data(),
        field("NameDB").toString().toAscii().data(),
        "seriesgrid", "ImageID",QString( "%1").arg( ImageID ).toLatin1().data());

  if( !myvect.empty() )
    {
    std::cout<<"Nb of values in myvect: "<< myvect.size()<<std::endl;

    //setField("ExpID", ExpID.toInt());
    setField("RCoord",myvect[2].c_str());
    setField("CCoord",myvect[3].c_str());
    setField("TCoord",myvect[4].c_str());
    setField("YCoord",myvect[5].c_str());
    setField("XCoord",myvect[6].c_str());
    setField("ZCoord",myvect[7].c_str());
    setField("FileName",myvect[8].c_str());

    setField("ImageID",ImageID);
    std::cout<<"ImageID is "<<field("ImageID").toString().toAscii().data()<<std::endl;
    }
  }
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
  if (field("OpenOrCreateSeriesGrid")=="Create")
    {
      /*  GoDBSeriesGridRow myNewObject;

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
          field("NameDB").toString().toAscii().data(),"seriesgrid", myNewObject);*/


       std::vector<std::string> vectListExpID =
        ListExpID(
          field("ServerName").toString().toAscii().data(),
          field("User").toString().toAscii().data(),
          field("Password").toString().toAscii().data(),
          field("NameDB").toString().toAscii().data());

      setField("ImageID",(unsigned int)vectListExpID.size());
      std::cout<<"create"<<std::endl;
      std::cout<<field("ExpID").toString().toAscii().data()<<std::endl;

    }
  else
  {

  }
   setTitle(tr("Experiment %1,'%2' from the database %3").arg(field("ExpID")
     .toString()).arg(field("Name").toString()).arg(field("NameDB").toString()));
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
