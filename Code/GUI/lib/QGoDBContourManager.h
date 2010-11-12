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

#ifndef __QGoDBContourManager_h
#define __QGoDBContourManager_h

#include "QGoTableWidget.h"
#include "GoDBCollectionOfTraces.h"
#include "GoDBTWContainerForContourMesh.h"
#include "QGoDBTraceManager.h"
#include "ContourMeshContainer.h"

class QGOGUILIB_EXPORT QGoDBContourManager:public QGoDBTraceManager
{
  Q_OBJECT
public:
  QGoDBContourManager(int iImgSessionID, QWidget *iparent);
  ~QGoDBContourManager();
  typedef std::pair< std::string, QColor > NameWithColorData;

  //void SetDatabaseConnection(vtkMySQLDatabase *iDatabaseConnector);

  void SetSelectedColor(NameWithColorData iSelectedColor);

  //void SetSelectedCollection(NameWithColorData iSelectedCollection);

  /**
  \brief set the m_ContourContainerInfoForVisu to the iContainerForVisu
  \param[in] iContainerForVisu common container for the visu and database
  */
  void SetContoursInfoContainerForVisu(ContourMeshContainer *iContainerForVisu);

  unsigned int SaveNewContourFromVisu(unsigned int iXCoordMin,
                                      unsigned int iYCoordMin,
                                      unsigned int iZCoordMin,
                                      unsigned int iTCoord,
                                      unsigned int iXCoordMax,
                                      unsigned int iYCoordMax,
                                      unsigned int iZCoordMax,
                                      vtkPolyData *iTraceNodes,
                                      vtkMySQLDatabase *iDatabaseConnector,
                                      NameWithColorData iColor,
                                      unsigned int iMeshID);

  /**
  \brief update the bounding box and the points for the checked contour and return
  the contourid
  */
  unsigned int  SaveReeditedContourFromVisu(unsigned int iXCoordMin,
                                            unsigned int iYCoordMin,
                                            unsigned int iZCoordMin,
                                            unsigned int iTCoord,
                                            unsigned int iXCoordMax,
                                            unsigned int iYCoordMax,
                                            unsigned int iZCoordMax,
                                            vtkPolyData *iContourNodes,
                                            vtkMySQLDatabase *iDatabaseConnector);

  /**
  \brief get all the data from the database to load all the contours for the imagingsession
  into the table widget and the container for the visu
  \param[in] iDatabaseConnector connection to the database
  \param[in] iTimePoint current timepoint
  */
  void DisplayInfoAndLoadVisuContainerForAllContours(vtkMySQLDatabase *iDatabaseConnector,
    unsigned int iTimePoint);

  /**
  \brief is called when the user choses the change color action in the
  context menu,emit signals to get the databaseconnection and the selectedColor
  and updates directly the checked traces with the new color in
  the database, the TW and the visu container as contour has no collectionof
  */
  void ChangeTraceColor();

  /**
  \brief virtual pure method in QGoDBTraceManager
  */
  virtual std::list< unsigned int > UpdateTheTracesColor(vtkMySQLDatabase *iDatabaseConnector,
                                                         NameWithColorData iNewColor);

  //virtual pure method in QGoDBTraceManager
  virtual void UpdateTWAndContainerForImportedTraces(std::vector< int > iVectorImportedTraces,
                                                     vtkMySQLDatabase *iDatabaseConnector);
  //virtual pure method in QGoDBTraceManager
  virtual void DeleteTraces(vtkMySQLDatabase *iDatabaseConnector);

  //virtual pure method in QGoDBTraceManager
  virtual std::list< unsigned int > GetListHighlightedIDs();

signals:
  void NeedToGetCurrentSelectedColor();

  //void NeedToGetCurrentSelectedCollection();
  /**
  \brief signal emitted when the user clicks on "Reedit contour" in the context menu
  */
  void TraceToReEdit(unsigned int);

protected:
  GoDBTWContainerForContourMesh *m_TWContainer;
  ContourMeshContainer          *m_ContourContainerInfoForVisu;
  NameWithColorData              m_SelectedColor;
  NameWithColorData              m_SelectedCollection;
  vtkMySQLDatabase *             m_DatabaseConnector;

  //virtual pure method in QGoDBTraceManager
  virtual void SetCollectionsTraceNames();

  virtual void AddActionsContextMenu(QMenu *iMenu);

  /**
  \brief add the action "generate a mesh from contours" in the context menu
  \param[in] iMenu context menu
  */
  void AddActionForCreateNewCollectionFromCheckedTraces(QMenu *iMenu);

  virtual void DisplayInfoForLastCreatedTrace(vtkMySQLDatabase *iDatabaseConnector);

  virtual void DisplayInfoForExistingTrace(vtkMySQLDatabase *iDatabaseConnector,
                                           int iTraceID);

  //virtual pure method in QGoDBTraceManager
  virtual void DisplayInfoForAllTraces(vtkMySQLDatabase *iDatabaseConnector);

  /**
  \brief get the contoursIDs belonging to iTimePoint and set their visibility
  on after setting all contoursIDs visibility off
  \param[in] iTimePoint current timepoint
  */
  //void UpdateTracesVisibilityForGivenTimePoint(unsigned int iTimePoint);
  
  //virtual pure method in QGoDBTraceManager
  virtual void GetTracesInfoFromDBAndModifyContainerForVisu(
    vtkMySQLDatabase* iDatabaseConnector,std::vector<int> iVectIDs = std::vector< int >());

protected slots:
  /**
  \brief emit TraceToReEdit if one and only one contour is checked in the TW
  */
  void ReEditTrace();

  //virtual pure method in QGoDBTraceManager
  virtual void UpdateHighlightedElementsInVisuContainer(int iTraceID);

  //virtual pure method in QGoDBTraceManager
  virtual void UpdateVisibleElementsInVisuContainer(int iTraceID);
};
#endif
