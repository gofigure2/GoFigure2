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

#ifndef __QGoDBMeshManager_h
#define __QGoDBMeshManager_h

#include "QGoTableWidget.h"
#include "GoDBCollectionOfTraces.h"
#include "GoDBTWContainerForMesh.h"
#include "QGoDBTraceManager.h"
#include "GoDBMeshRow.h"
#include "ContourMeshContainer.h"

class QGOGUILIB_EXPORT QGoDBMeshManager:public QGoDBTraceManager
{
  Q_OBJECT
public:
  QGoDBMeshManager(int iImgSessionID,
                   QWidget *iparent);
  ~QGoDBMeshManager();
  
  /**
  \brief set the m_MeshContainerInfoForVisu to the iContainerForVisu
  \param[in] iContainerForVisu common container for the visu and database
  */
  void SetMeshesInfoContainerForVisu(ContourMeshContainer *iContainerForVisu);

  /**
  \brief get all the data from the database to load all the meshes for the imagingsession
  into the table widget and the container for the visu
  \param[in] iDatabaseConnector connection to the database
  \param[in] iTimePoint current timepoint
  */
  void DisplayInfoAndLoadVisuContainerForAllMeshes(vtkMySQLDatabase *iDatabaseConnector,
	unsigned int iTimePoint);

  virtual void DisplayInfoForLastCreatedTrace(vtkMySQLDatabase *iDatabaseConnector);

  void DisplayInfoForLastCreatedMesh(vtkMySQLDatabase *iDatabaseConnector,
                                     GoFigureMeshAttributes *iMeshAttributes);

  virtual void DisplayInfoForExistingTrace(vtkMySQLDatabase *iDatabaseConnector,
                                           int iTraceID);

  void DisplayInfoForExistingTraceForMesh(vtkMySQLDatabase *iDatabaseConnector,
                                          int iTraceID, GoFigureMeshAttributes *iMeshAttributes);

  /**
  \brief display in the TW the volume and area extracted from iMeshAttributes
  * called when loading all meshes from database when opening an imagingsession
  \param[in] iMeshAttributes computed values for a mesh
  \param[in] iMeshID meshID of the mesh we want to display the volume and area
  */
  void DisplayOnlyVolumeAreaForExistingMesh(GoFigureMeshAttributes *iMeshAttributes,
                                            unsigned iMeshID);

  unsigned int CreateNewMeshWithNoContourNoPoints(
    vtkMySQLDatabase *iDatabaseConnector, NameWithColorData iColor,
    unsigned int iTimePoint, std::string iCellType, std::string iSubCellType,
    unsigned int iTrackID = 0);

  unsigned int SaveNewMeshFromVisu(unsigned int iXCoordMin,
                                   unsigned int iYCoordMin,
                                   unsigned int iZCoordMin,
                                   unsigned int iTCoord,
                                   unsigned int iXCoordMax,
                                   unsigned int iYCoordMax,
                                   unsigned int iZCoordMax,
                                   vtkPolyData *iTraceNodes,
                                   vtkMySQLDatabase *iDatabaseConnector,
                                   NameWithColorData iColor,
                                   unsigned int iTrackID,
                                   GoFigureMeshAttributes *iMeshAttributes,
                                   std::string iCellType,
                                   std::string iSubCellType);

  void SaveGeneratedMeshFromVisu(unsigned int iXCoordMin, unsigned int iYCoordMin,
                                 unsigned int iZCoordMin,
                                 unsigned int iTCoord, unsigned int iXCoordMax, unsigned int iYCoordMax,
                                 unsigned int iZCoordMax, vtkPolyData *iTraceNodes,
                                 vtkMySQLDatabase *iDatabaseConnector, GoFigureMeshAttributes *iMeshAttributes);

  /**
  \brief virtual pure method in QGoDBTraceManager
  */
  std::list< unsigned int > UpdateTheTracesColor(vtkMySQLDatabase *iDatabaseConnector,
                                                 NameWithColorData iNewColor);

  /**
  \brief update the bounding boxes for the meshes with no points
  */
  void UpdateBoundingBoxes(vtkMySQLDatabase *iDatabaseConnector,
                           std::list< unsigned int > iListTracesIDs);

  //virtual pure method in QGoDBTraceManager
  virtual void UpdateTWAndContainerForImportedTraces(std::vector< int > iVectorImportedTraces,
                                                     vtkMySQLDatabase *iDatabaseConnector);
  //virtual pure method in QGoDBTraceManager
  virtual void DeleteTraces(vtkMySQLDatabase *iDatabaseConnector);

   //virtual pure method in QGoDBTraceManager
  virtual std::list< unsigned int > GetListHighlightedIDs();

  /**
  \brief get the coordinate info for meshes needed for the visu
  for imported tracks
  \param[in] iMeshesIDs list of meshes IDs the info are needed
  \return a map with IDs as keys and info as value
  */
  std::map<unsigned int,double*> GetMeshesInfoForImportedMesh(
	std::list<unsigned int> iMeshesIDs);

  /**
  \brief check in the database if there is an existing mesh belonging
  to iTrackID with iTimePoint, if yes, reassign the trackID to 0 and 
  return the ID of the mesh with the new trackID set to 0.
  \param[in] iDatabaseConnector connection to the database
  \param[in] iTrackID ID of the track 
  \param[in] iTimePoint timepoint to be checked
  \return meshID of the mesh on the same timepoint which trackID is
  reassigned to 0
  */
  unsigned int ReassignTrackIDForPreviousMeshWithSameTimePoint( 
    vtkMySQLDatabase *iDatabaseConnector,unsigned int iTrackID,
    unsigned int iTimePoint);

  /**
  \brief if the track has already a mesh assigned for the current timepoint,
  the track of the previous mesh will be reassigned to 0 and a message will
  be displayed in the statusbar
  \param[in] iTrackID ID of the track to be checked
  \param[in] iTimePoint timepoint at which the existing meshes need to be checked
  \param[in] iDatabaseConnector connection to the database
  \return a message to be print in the status bar of the mainwindow, if no meshes
  reassigned, the message will be ""
  */
  QString CheckExistingMeshesForTheTrack(
   unsigned int iTrackID,int iTimePoint,vtkMySQLDatabase* iDatabaseConnector);

protected:
  GoDBTWContainerForMesh *m_TWContainer;
  ContourMeshContainer   *m_MeshContainerInfoForVisu;

  //virtual pure method in QGoDBTraceManager
  virtual void SetCollectionsTraceNames();

  void PrintValuesForMeshWithNoPoints(unsigned int iTraceID);

  virtual void AddActionsContextMenu(QMenu *iMenu);

  //virtual pure method in QGoDBTraceManager
  virtual void DisplayInfoForAllTraces(vtkMySQLDatabase *iDatabaseConnector);

  void SetMeshBoundingBoxAndPoints(unsigned int iXCoordMin,
                                   unsigned int iYCoordMin,
                                   unsigned int iZCoordMin,
                                   unsigned int iTCoord,
                                   unsigned int iXCoordMax,
                                   unsigned int iYCoordMax,
                                   unsigned int iZCoordMax,
                                   vtkPolyData *iTraceNodes,
                                   vtkMySQLDatabase *iDatabaseConnector,
                                   GoDBMeshRow & iMesh,
                                   GoFigureMeshAttributes *iMeshAttributes);
  //virtual pure method in QGoDBTraceManager
  virtual void GetTracesInfoFromDBAndModifyContainerForVisu(
    vtkMySQLDatabase* iDatabaseConnector,std::vector<int> iVectIDs = std::vector< int >());

  /**
  \brief get the meshesIDs belonging to iTimePoint and set their visibility
  on after setting all meshesIDs visibility off
  \param[in] iTimePoint current timepoint
  */
  //void UpdateTracesVisibilityForGivenTimePoint(unsigned int iTimePoint);

protected slots:
  //virtual pure method in QGoDBTraceManager
  virtual void UpdateHighlightedElementsInVisuContainer(int iTraceID);

  //virtual pure method in QGoDBTraceManager
  virtual void UpdateVisibleElementsInVisuContainer(int iTraceID);
};
#endif
