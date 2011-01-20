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

#ifndef __QGoDBTrackManager_h
#define __QGoDBTrackManager_h

#include "QGoTableWidget.h"
#include "GoDBCollectionOfTraces.h"
#include "GoDBTWContainerForTrack.h"
#include "QGoDBTraceManager.h"
#include "QGoGUILibConfigure.h"
#include "TrackContainer.h"


class QGOGUILIB_EXPORT QGoDBTrackManager:public QGoDBTraceManager
{
  Q_OBJECT
public:
  QGoDBTrackManager(int iImgSessionID, QWidget *iparent);
  ~QGoDBTrackManager();

  virtual void DisplayInfoForLastCreatedTrace(vtkMySQLDatabase *iDatabaseConnector);

  virtual void DisplayInfoForExistingTrace(vtkMySQLDatabase *iDatabaseConnector,
                                           int iTraceID);

  /**
  \brief set the m_TrackContainerInfoForVisu to the iContainerForVisu
  \param[in] iContainerForVisu common container for the visu and database
  */
  void SetTracksInfoContainerForVisu(TrackContainer *iContainerForVisu);

  /**
  \brief get all the data from the database to load all the tracks for the imagingsession
  into the table widget and the container for the visu
  \param[in] iDatabaseConnector connection to the database
  */
  void DisplayInfoAndLoadVisuContainerForAllTracks(
    vtkMySQLDatabase *iDatabaseConnector);

  /**
  \brief display in the TW the values extracted from iTrackAttributes
  * called when loading all tracks from database when opening an imagingsession
  \param[in] iTrackAttributes computed values for a track
  \param[in] iTrackID trackID of the track we want to display the values
  */
  void DisplayOnlyCalculatedValuesForExistingTrack(
    GoFigureTrackAttributes *iTrackAttributes, unsigned iTrackID);

  /**
  \brief create a new track with no mesh and no points in the database, add it in the
  TW and in the visu container
  \param[in] iDatabaseConnector connection to the database
  \return the ID of the new track just created
  */
  unsigned int CreateNewTrackWithNoMesh(
    vtkMySQLDatabase *iDatabaseConnector);

  //virtual pure method in QGoDBTraceManager
  std::list< unsigned int > UpdateTheTracesColor(vtkMySQLDatabase *iDatabaseConnector);

  //virtual pure method in QGoDBTraceManager
  virtual void UpdateTWAndContainerForImportedTraces(std::vector< int > iVectorImportedTraces,
                                                     vtkMySQLDatabase *iDatabaseConnector);
  
  /**
  \brief delete the traces of the list from the database, the TW and the
  container for visu
  \param[in] iDatabaseConnector connection to the database
  \param[in] iListTraces list of the tracesIDs to be deleted

  */
  void DeleteListTraces(vtkMySQLDatabase *iDatabaseConnector,
    std::list<unsigned int> iListTraces);

  //virtual pure method in QGoDBTraceManager
  virtual void DeleteCheckedTraces( vtkMySQLDatabase *iDatabaseConnector);

   //virtual pure method in QGoDBTraceManager
  virtual std::list< unsigned int > GetListHighlightedIDs();

  /**
  \brief get the data from the TrackContainer corresponding to the user selected TrackID
  and put them in the current element of the track container
  */
  void UpdateCurrentElementTrackContainer();

  /**
  \brief update the points of the imported track in current_element
  with the info from the meshes
  */
  void UpdatePointsOfCurrentElementForImportedTrack(
	  std::map<unsigned int,double*> iMeshesInfo);

  //method in QGoDBTraceManager
  virtual void UpdateBoundingBoxes(
  vtkMySQLDatabase *iDatabaseConnector,std::list< unsigned int > iListTracesIDs);

signals:
  void NeedMeshesInfoForImportedTrack(unsigned int iTrackID);
  void TrackToSplit(unsigned int iTrackID, std::list<unsigned int> iListMeshIDs);
  void TrackIDToBeModifiedWithWidget(std::list<unsigned int> iListTracksID);

protected:
  GoDBTWContainerForTrack *m_TWContainer;
  TrackContainer          *m_TrackContainerInfoForVisu;

  //virtual pure method in QGoDBTraceManager
  virtual void SetCollectionsTraceNames();

  //virtual pure method in QGoDBTraceManager
  virtual void DisplayInfoForAllTraces(vtkMySQLDatabase *iDatabaseConnector);

  //virtual pure method in QGoDBTraceManager
  virtual void GetTracesInfoFromDBAndModifyContainerForVisu(
    vtkMySQLDatabase* iDatabaseConnector,
    std::list<unsigned int> iListTraceIDs = std::list< unsigned int >());

  /**
  \brief get the center of bounding boxes from the database for the meshes
  belonging to the iTrackID, update the polydata in the container for visu and save it in
  the database
  \param[in] iDatabaseConnector connection to the database
  \param[in] iTrackID ID for the track the polydata needs to be recalculated
  */
  void UpdateTrackPolydataForVisu(vtkMySQLDatabase *iDatabaseConnector,unsigned int iTrackID);

  //virtual in QGoDBTraceManager
  void AddActionsContextMenu(QMenu *iMenu);

protected slots:
  //virtual pure method in QGoDBTraceManager
  virtual void UpdateHighlightedElementsInVisuContainer(int iTraceID);

  //virtual pure method in QGoDBTraceManager
  virtual void UpdateVisibleElementsInVisuContainer(int iTraceID);

  /**
  \brief create or update the track contained in the current element of the
  track container into the database, the table widget and insert the current
  element into the track container
  \param[in] iDatabaseConnector connection to the database
  */
  void SaveTrackCurrentElement(vtkMySQLDatabase* iDatabaseConnector);

 //virtual pure method in QGoDBTraceManager
  virtual void SetColorCoding(bool IsChecked);

  void SplitTrackWithWidget();

  void TrackIDToEmit();
  //void SelectionClick(int iID);
};
#endif
