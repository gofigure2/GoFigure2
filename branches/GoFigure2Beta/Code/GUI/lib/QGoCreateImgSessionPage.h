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
#ifndef __QGoCreateImgSessionPage_h
#define __QGoCreateImgSessionPage_h

#include <QWizardPage>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>
#include <QRadioButton>
#include <QComboBox>
#include <QStringList>
#include <QPushButton>
#include <QFileInfo>
#include <QString>
#include <string>

#include "GoDBImageRow.h"
#include "GoDBCoordinateRow.h"
#include "GoFigureFileInfoMultiIndexContainerHelper.h"
#include "vtkMySQLDatabase.h"
#include "MegaCaptureHeaderReader.h"
#include "itkMegaCaptureImport.h"

class QGoCreateImgSessionPage : public QWizardPage
{
  Q_OBJECT

public:
  QGoCreateImgSessionPage(QWidget *parent = 0);
  void initializePage();
  bool validatePage();
  void cleanupPage();
  vtkMySQLDatabase* m_DatabaseConnector;

  GoFigureFileInfoHelperMultiIndexContainer GetMultiIndexFileContainer();

private:
  /**\brief get the list of the existing microscopes registered in the DB */
  QStringList GetListMicroscopes();

  /** \brief create a new imaging session into the DB with the
  information filled by the user and collected from the filenames, and
  return the ImagingSessionID just created or -1 if the img session already exists*/
  int  CreateImgSession(vtkMySQLDatabase* DatabaseConnector);

  /** \brief create the imaging session, all the channels into the DB,
  then the images selected by the user and at the end, update the CoordIDMax
  and Min of the imaging session into the DB*/
  void ImportImages(vtkMySQLDatabase* DatabaseConnector);//,QString newfilename);


  typedef GoFigureFileInfoHelperMultiIndexContainer::iterator
    MultiIndexContainerIteratorType;

  /** \brief create the coordinate CoordMin in the DB, check and update if its values
  are less than the other CoordMin created for the images belonging to the same imaging
  session and return the CoordID of the coordinate just created */
  int CreateImageCoordMin( vtkMySQLDatabase* DatabaseConnector,
    MultiIndexContainerIteratorType It );

  /** \brief return the ChannelID from the DB corresponding to the imaging session and
  to the channel number given by the image filename*/
  int FindChannelIDForImage( vtkMySQLDatabase* DatabaseConnector,
    int ImagingSessionID,
    int ChannelNumber );

  /** \brief return a GoDBImageRow filled with all the data corresponding */
  GoDBImageRow CreateImage( vtkMySQLDatabase* DatabaseConnector,
    MultiIndexContainerIteratorType It, int ImagingSessionID );

  /** \brief create the channels and their corresponding colors in the database,
  from the data gotten from the headerfile*/
  void CreateChannels(vtkMySQLDatabase* DatabaseConnector, int ImagingSessionID);

  /** \brief create into the DB the coordinates corresponding to the CoordID
  Min and Max for the Imaging Session and update the CoordIDMax and Min for
  the imaging session in the DB with the newly created coordinates*/
  void CreateImgSessionCoord(vtkMySQLDatabase* DatabaseConnector,int ImagingSessionID);

  void OpenDBConnection();
  //void CloseDatabaseConnection();

  void SaveInfoInDatabase();

  /** \brief fill m_importFileInfoList from the filenames of the images and
  m_HeaderFileInfo from the header file*/
  void ImportInfoFromMegacapture(QString newfilename);

  QLabel*      textNewImgSessionName;
  QLineEdit*   lineNewImgSessionName;
  QLabel*      textDescription;
  QTextEdit*   lineDescription;
  QLabel*      textChoiceMicroscope;
  QComboBox*   ChoiceMicroscope;
  QPushButton* BrowseButton;
  QTextEdit*   lineFilename;
  QString      newfilename;
  QFileInfo*   FirstImage;

  GoDBCoordinateRow                m_ImgSessionCoordMax;
  GoDBCoordinateRow                m_ImgSessionCoordMin;
  MegaCaptureHeaderReader          m_HeaderFileInfo;
  itk::MegaCaptureImport::Pointer  m_importFileInfoList;

protected slots:

  void SelectImages();

   /** \brief Prevent the user to enter more than 1000 characters in the
  QTextEdit and return the text entered */
  std::string GetDescription();

};
#endif