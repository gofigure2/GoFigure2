/*=========================================================================
  Author: $Author: arnaudgelas $  // Author of last commit
  Version: $Rev: 1150 $  // Revision of last commit
  Date: $Date: 2010-03-30 15:00:47 -0400 (Tue, 30 Mar 2010) $  // Date of last commit
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
#include "QGoCreateImgSessionPage.h"
#include "QueryDataBaseHelper.h"
#include "SelectQueryDatabaseHelper.h"
#include "GoDBImgSessionRow.h"
#include "GoDBImageRow.h"
#include "GoDBCoordinateRow.h"
#include "GoDBChannelRow.h"
#include "GoDBColorRow.h"
#include "GoDBRecordSetHelper.h"
#include "itkMegaCaptureImport.h"
#include "itkLsm3DSerieImport.h"
#include "ConvertToStringHelper.h"

#include <QGridLayout>
#include <QFileDialog>
#include <QDateTime>
#include <QMessageBox>
#include <iostream>


QGoCreateImgSessionPage::QGoCreateImgSessionPage( QWidget *iParent )
: QWizardPage( iParent )
{
  QFont tfont;
  tfont.setBold(false);
  this->setFont(tfont);

  textNewImgSessionName = new QLabel(tr("Name of the New Imaging Session*: "));
  lineNewImgSessionName = new QLineEdit;
  lineNewImgSessionName->setMaxLength(255);
  textDescription = new QLabel(tr("Description:"));
  lineDescription  = new QTextEditChild(this,1000);
  textChoiceMicroscope = new QLabel(tr("Choose the Microscope used*:"));
  ChoiceMicroscope = new QComboBox;
  BrowseButton = new QPushButton("&Browse", this);
  QLabel* textBrowseButton = new QLabel(tr("Select only 1 file\nfrom the Image Set*:"));
  lineFilename = new QTextEdit;

  QGridLayout* gridlayout = new QGridLayout;
  gridlayout->addWidget(textNewImgSessionName,0,0);
  gridlayout->addWidget(lineNewImgSessionName,0,1);
  gridlayout->addWidget(textDescription,1,0);
  gridlayout->addWidget(lineDescription,1,1);
  gridlayout->addWidget(textChoiceMicroscope,2,0);
  gridlayout->addWidget(ChoiceMicroscope,2,1);

  QGridLayout* BrowseButtonLayout = new QGridLayout;
  BrowseButtonLayout->addWidget( BrowseButton,0,2 );
  BrowseButtonLayout->addWidget(textBrowseButton,0,0);
  BrowseButtonLayout->addWidget(lineFilename,0,1);
  BrowseButtonLayout->setColumnStretch ( 0, 2);
  BrowseButtonLayout->setColumnStretch ( 1, 4);
  BrowseButtonLayout->setColumnStretch ( 2, 1);


  QVBoxLayout* vlayout = new QVBoxLayout;

  vlayout->addLayout(gridlayout);
  vlayout->addLayout(BrowseButtonLayout);
  setLayout( vlayout );

  FirstImage = new QFileInfo;
  QObject::connect( this->BrowseButton,SIGNAL( clicked() ),
  this,SLOT( SelectImages() ));
 }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoCreateImgSessionPage::initializePage()
{
  lineFilename->clear();
  OpenDBConnection();
  setSubTitle(
    tr("Import a new dataset for the project '%1'\n (*fields are required) and click on 'Finish' to load them:")
    .arg(field("ProjectName").toString() ) );

  ChoiceMicroscope->clear();
  ChoiceMicroscope->addItems(GetListMicroscopes());
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoCreateImgSessionPage::SelectImages()
{
   QString filename = QFileDialog::getOpenFileName(
    this,tr( "Import Image" ),"",tr( "Images (*.png *.bmp *.jpg *.jpeg *tif *.tiff *.mha *.mhd *.img *.lsm)" ));
   lineFilename->setText(filename);
   FirstImage->setFile(filename);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QStringList QGoCreateImgSessionPage::GetListMicroscopes()
{
  QStringList ListMicroscopes;
  std::vector<std::string> vectListMicroscopes =
      ListAllValuesForOneColumn( m_DatabaseConnector,"Name","microscope");

  if (!vectListMicroscopes.empty())
    {
    for(unsigned int i = 0; i < vectListMicroscopes.size(); ++i )
      {
      ListMicroscopes.append( vectListMicroscopes[i].c_str( ) );
      }
    }

  return ListMicroscopes;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoCreateImgSessionPage::CreateImgSession(vtkMySQLDatabase* DatabaseConnector)
{
  std::string m_ProjectName = field("ProjectName").toString().toStdString();
  int TimeInterval = m_HeaderFileInfo.m_TimeInterval;
  float RealPixelHeight =m_HeaderFileInfo.m_VoxelSizeX;
  float RealPixelWidth =m_HeaderFileInfo.m_VoxelSizeY;
  float RealPixelDepth =m_HeaderFileInfo.m_VoxelSizeZ;
  unsigned int XImageSize = m_HeaderFileInfo.m_DimensionX;
  unsigned int YImageSize = m_HeaderFileInfo.m_DimensionY;
  float XTileOverlap = 0; //todo get it from the header file
  float YTileOverlap = 0; //todo get it from the header file
  float ZTileOverlap = 0; //todo get it from the header file

  std::string CreationDateTime = m_HeaderFileInfo.m_CreationDate;

  GoDBImgSessionRow myNewImgSession;

  myNewImgSession.SetField("Name",lineNewImgSessionName->displayText().toStdString());
  myNewImgSession.SetField("Description",this->lineDescription->toPlainText().toStdString());
  myNewImgSession.SetField("ImagesTimeInterval",TimeInterval);
  myNewImgSession.SetField("RealPixelDepth",RealPixelDepth);
  myNewImgSession.SetField("RealPixelHeight",RealPixelHeight);
  myNewImgSession.SetField("RealPixelWidth", RealPixelWidth);
  myNewImgSession.SetField("ProjectName", m_ProjectName);
  myNewImgSession.SetField("MicroscopeName", ChoiceMicroscope->currentText().toStdString());
  myNewImgSession.SetField("CreationDate", CreationDateTime) ;
  myNewImgSession.SetField("XImageSize", XImageSize) ;
  myNewImgSession.SetField("YImageSize", YImageSize) ;
  myNewImgSession.SetField("XTileOverlap", XTileOverlap) ;
  myNewImgSession.SetField("YTileOverlap", YTileOverlap) ;
  myNewImgSession.SetField("ZTileOverlap", ZTileOverlap) ;

  int ExistingImgSession = myNewImgSession.DoesThisImagingSessionExist(DatabaseConnector);
  //if this is not a duplicate, the ExistingImgSession is = -1, so it is OK
  //to record it in the DB:
  if (ExistingImgSession == -1)
    {
    return AddOnlyOneNewObjectInTable< GoDBImgSessionRow >(DatabaseConnector,
      "imagingsession", myNewImgSession, "ImagingSessionID" );
    }
  //if the img session already exists, the function will return -1:
  return -1;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoCreateImgSessionPage::validatePage()
{
  if (lineNewImgSessionName->displayText()== "")
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please make sure that all the required fields (*) are not empty.") );
    msgBox.exec();
    return false;
    };
  if (lineNewImgSessionName->displayText()!= "")
    {
    std::vector<std::string> ProjectNameTaken = ListSpecificValuesForOneColumn(
      m_DatabaseConnector,"imagingsession", "Name","Name",
      lineNewImgSessionName->displayText().toStdString());
    if (ProjectNameTaken.size() != 0)
      {
      QMessageBox msgBox;
      msgBox.setText(
        tr("This name is already taken for an existing Imaging Session.\nPlease select another one.") );
      msgBox.exec();
      return false;
      }
  if (ChoiceMicroscope->currentText() == "")
      {
      QMessageBox msgBox;
      msgBox.setText( tr( "Please choose a microscope for your imaging session." ) );
      msgBox.exec();
      return false;
      }
    }
  if (!itk::MegaCaptureImport::IsNewMegaCapture(lineFilename->toPlainText().toStdString()))
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr( "The images you selected are of an old megacapture format, not supported by this database." ) );
    msgBox.exec();
    }

  //if all the info filled by the user are ok, try to save the info into the database:
  SaveInfoInDatabase();

  if (field("ImgSessionID").toInt() == -1)
    {
    QMessageBox msgBox;
    msgBox.setText( tr( "The images imported already belongs to an existing Imaging Session." ) );
    msgBox.exec();
    return false;
    }

  CloseDatabaseConnection(m_DatabaseConnector);
  setField("ImgSessionName",lineNewImgSessionName->text());
  return true;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoCreateImgSessionPage::cleanupPage()
{
  CloseDatabaseConnection(m_DatabaseConnector);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoCreateImgSessionPage::ImportImages(vtkMySQLDatabase* DatabaseConnector)
{
  if( DatabaseConnector != 0 )
    {
    try
      {
      //Create a Record Set with all the images to be saved in the DB:
      typedef GoDBRecordSet< GoDBImageRow > myRecordSetType;
      myRecordSetType* RecordSet = new myRecordSetType;
      RecordSet->SetConnector(DatabaseConnector);
      RecordSet->SetTableName( "image" );

      GoFigureFileInfoHelperMultiIndexContainer
        filelist = GetMultiIndexFileContainer();

      MultiIndexContainerIteratorType f_it  = filelist.begin();
      MultiIndexContainerIteratorType f_end = filelist.end();

      while( f_it != f_end )
        {
        GoDBImageRow Image = CreateImage(DatabaseConnector,f_it,
          field("ImgSessionID").toInt());
        RecordSet->AddObject( Image );
        ++f_it;
        }

      //Save all the images in the recordset in the Database:
      if( !RecordSet->SaveInDB() )
        {
        std::cout<<"The images have not been saved in the DB"<<std::endl;
        std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
        std::cout << std::endl;
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
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoCreateImgSessionPage::ImportInfoFromMegacapture(QString iNewfilename)
{
  try
    {
    m_importFileInfoList = itk::MegaCaptureImport::New();
    m_importFileInfoList->SetFileName( iNewfilename.toAscii().data() );

//     QProgressBar pBar;
    //importFileInfoList->SetProgressBar( &pBar );
    m_importFileInfoList->Update();

    //Get the headerfile once the headerfilename is gotten
    //from megacapture import:
    std::string HeaderFilename = m_importFileInfoList->GetHeaderFilename();
    m_HeaderFileInfo.SetFileName(HeaderFilename);
    m_HeaderFileInfo.Read();
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
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoCreateImgSessionPage::CreateChannels( vtkMySQLDatabase* DatabaseConnector,
  int ImagingSessionID)
{
  //creation of the record set to be filled with all the new channels to save in DB:
  typedef GoDBRecordSet< GoDBChannelRow > myRecordSetType;
          myRecordSetType* RecordSet = new myRecordSetType;
          RecordSet->SetConnector(DatabaseConnector);
          RecordSet->SetTableName( "channel" );

  for( unsigned int i = 0; i < m_HeaderFileInfo.m_NumberOfChannels;i++)
    {
    //for each channel, the color first has to be created from the headerfile
    //into the DB:
    GoDBColorRow myNewColor;    
    std::string StringChannelNumber = ConvertToString<int>(i);
    std::string ChannelName = "Channel " + StringChannelNumber;
    std::string ColorName = "ColorFor";
    ColorName += ChannelName;
    /** \todo take the channel names from the header file*/
    int Red   = m_HeaderFileInfo.m_ChannelColor[i][0];
    int Green = m_HeaderFileInfo.m_ChannelColor[i][1];
    int Blue  = m_HeaderFileInfo.m_ChannelColor[i][2];
    int Alpha = 255;

    myNewColor.SetField("Name",ColorName);
    myNewColor.SetField("Red",Red);
    myNewColor.SetField("Green",Green);
    myNewColor.SetField("Blue",Blue );
    myNewColor.SetField("Alpha",Alpha);
    
    int ColorID = myNewColor.SaveInDB(DatabaseConnector);

    //creation of the corresponding channel:
    GoDBChannelRow myNewChannel;

    myNewChannel.SetField("ColorID",ColorID);
    myNewChannel.SetField("Name",ChannelName);
    myNewChannel.SetField("ImagingSessionID",ImagingSessionID);
    myNewChannel.SetField("ChannelNumber",i);
    myNewChannel.SetField("NumberOfBits",m_HeaderFileInfo.m_ChannelDepth);
    RecordSet->AddObject(myNewChannel);
    }
  RecordSet->SaveInDB();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoCreateImgSessionPage::CreateImageCoordMin(vtkMySQLDatabase* DatabaseConnector,
  MultiIndexContainerIteratorType It)
{
  GoDBCoordinateRow myNewImageCoordMin;
  myNewImageCoordMin.SetField("PCoord",It->m_PCoord);
  myNewImageCoordMin.SetField("RCoord",It->m_RCoord);
  myNewImageCoordMin.SetField("CCoord",It->m_CCoord);
  myNewImageCoordMin.SetField("XTileCoord",It->m_XTileCoord);
  myNewImageCoordMin.SetField("YTileCoord",It->m_YTileCoord);
  myNewImageCoordMin.SetField("ZTileCoord",It->m_ZTileCoord);
  myNewImageCoordMin.SetField("XCoord",It->m_XCoord);
  myNewImageCoordMin.SetField("YCoord",It->m_YCoord);
  myNewImageCoordMin.SetField("ZCoord",It->m_ZCoord);
  myNewImageCoordMin.SetField("TCoord",It->m_TCoord);

  std::map<std::string,std::string>::iterator IterNewCoord = myNewImageCoordMin.MapBegin();
  std::map<std::string,std::string>::iterator IterNewCoordEnd = myNewImageCoordMin.MapEnd();
  std::map<std::string,std::string>::iterator IterMinCoord = m_ImgSessionCoordMin.MapBegin();
  std::map<std::string,std::string>::iterator IterMaxCoord = m_ImgSessionCoordMax.MapBegin();

  while (IterNewCoord != IterNewCoordEnd)
    {
    IterMinCoord->second = ConvertToString<int>(std::min(atoi(IterNewCoord->second.c_str()),
      atoi(IterMinCoord->second.c_str())));
    IterMaxCoord->second = ConvertToString<int>(std::max(atoi(IterNewCoord->second.c_str()),
      atoi(IterMaxCoord->second.c_str())));
    IterMinCoord ++;
    IterNewCoord ++;
    IterMaxCoord ++;
    }

  return myNewImageCoordMin.SaveInDB(DatabaseConnector);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoCreateImgSessionPage::FindChannelIDForImage(vtkMySQLDatabase* DatabaseConnector,
  int ImagingSessionID,int ChannelNumber)
{
  return FindOneID(DatabaseConnector,"channel","channelID",
    "ImagingSessionID",ConvertToString<int>(ImagingSessionID),"ChannelNumber",
    ConvertToString<int>(ChannelNumber));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoDBImageRow QGoCreateImgSessionPage::CreateImage(vtkMySQLDatabase* DatabaseConnector,
  MultiIndexContainerIteratorType It,int ImagingSessionID)
{
  GoDBImageRow myNewImage;
  myNewImage.SetField("ImagingSessionID", ImagingSessionID);
  myNewImage.SetField("CoordIDMin",CreateImageCoordMin(DatabaseConnector,It));
  myNewImage.SetField("Filename",It->m_Filename);

  //find the channelID for the image which corresponds to the channel number
  //found in the filename (m_Channel):
  int channel = FindChannelIDForImage(m_DatabaseConnector,
    ImagingSessionID, It->m_Channel);
  if (channel == -1)
    {
    std::cout<<"The channel doesn't exist in the DB"<<std::endl;
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return myNewImage;
    }

  myNewImage.SetField("ChannelID",channel);
  return myNewImage;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoCreateImgSessionPage::CreateImgSessionCoord(
  vtkMySQLDatabase* DatabaseConnector,int ImagingSessionID)
{
  int XImageSize = m_HeaderFileInfo.m_DimensionX;
  int YImageSize = m_HeaderFileInfo.m_DimensionY;
  m_ImgSessionCoordMax.SetField("XCoord",XImageSize);
  m_ImgSessionCoordMax.SetField("YCoord",YImageSize);

  //add a new coordinate to enter the info for ImgSession CoordMax
  int CoordIDMax = m_ImgSessionCoordMax.SaveInDB(DatabaseConnector);

  //update the CoordMaxID in Imgsession with the new created coordinate
  UpdateValueInDB(DatabaseConnector,"imagingsession",
    "CoordIDMax", ConvertToString<int>(CoordIDMax),"ImagingSessionID",
    ConvertToString<int>(ImagingSessionID));

  //add a new coordinate to enter the info for ImgSession CoordMin
  int CoordIDMin = m_ImgSessionCoordMin.SaveInDB(DatabaseConnector);

  //update the CoordMaxID in Imgsession with the new created coordinate
  UpdateValueInDB(DatabaseConnector,"imagingsession",
    "CoordIDMin", ConvertToString<int>(CoordIDMin),"ImagingSessionID",
    ConvertToString<int>(ImagingSessionID));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoCreateImgSessionPage::OpenDBConnection()
{
  std::string Server = field("ServerName").toString().toStdString();
  std::string User = field("User").toString().toStdString();
  std::string Password = field("Password").toString().toStdString();
  std::string DBName = field("DBName").toString().toStdString();

  m_DatabaseConnector = OpenDatabaseConnection(Server,User,Password,DBName);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoCreateImgSessionPage::SaveInfoInDatabase()
{
  /*First, get the info from the filenames and the headerfile: */
  ImportInfoFromMegacapture(lineFilename->toPlainText());

  /*Second, save in the DB the related ImagingSession as it is not possible to
  save in DB the images without knowing the corresponding ImagingSessionID:*/
  int ImgSessionID = CreateImgSession(m_DatabaseConnector);
  setField("ImgSessionID",ImgSessionID);
  if (ImgSessionID != -1)
    {
    QString ImgSessionName = lineNewImgSessionName->displayText();
    //setField("ImgSessionName",ImgSessionName);

    /*Save in the DB the channels corresponding to the ImagingSession and
          to the channelnumber of the images:*/
    CreateChannels(m_DatabaseConnector,field("ImgSessionID").toInt());

    /*Record all the images from the image set into the DB, and fill the
    ImgSessionCoordMin and Max with the values related to the images:*/
    ImportImages(m_DatabaseConnector);//,lineFilename->toPlainText());

    //Update CoordMin and CoordMax for ImagingSession:
    CreateImgSessionCoord(m_DatabaseConnector,field("ImgSessionID").toInt());
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoFigureFileInfoHelperMultiIndexContainer
QGoCreateImgSessionPage::
GetMultiIndexFileContainer()
{
  if( m_importFileInfoList.IsNotNull() )
    {
    return m_importFileInfoList->GetOutput();
    }
  else
    {
    return GoFigureFileInfoHelperMultiIndexContainer();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string
QGoCreateImgSessionPage::
GetMegaCaptureHeaderFilename()
{
  return m_importFileInfoList->GetHeaderFilename();
}
//-------------------------------------------------------------------------
