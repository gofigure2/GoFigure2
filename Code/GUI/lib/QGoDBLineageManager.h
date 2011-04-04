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

#ifndef __QGoDBLineageManager_h
#define __QGoDBLineageManager_h

#include "QGoTableWidget.h"
#include "GoDBCollectionOfTraces.h"
#include "GoDBTWContainerForLineage.h"
#include "QGoDBTraceManager.h"
#include "QGoGUILibConfigure.h"
#include "LineageContainer.h"
#include "TrackContainer.h"

/**
\class QGoDBLineageManager
\brief This class manages the database queries, the table widget and
the data from the database in the Container for visu for the lineages
\ingroup DB, GUI
*/
class QGOGUILIB_EXPORT QGoDBLineageManager:public QGoDBTraceManager
{
  Q_OBJECT
public:
  QGoDBLineageManager(int iImgSessionID, QWidget *iparent);
  ~QGoDBLineageManager();

  virtual void DisplayInfoForLastCreatedTrace(vtkMySQLDatabase *iDatabaseConnector);

  virtual void DisplayInfoForExistingTrace(vtkMySQLDatabase *iDatabaseConnector,
                                           int iTraceID);

  /**
  \brief set the m_LineageContainerInfoForVisu and the m_TrackContainerInfoForVisu 
  to the iContainerForVisu and iTrackContainerInfoForvisu
  \param[in] iContainerForVisu common container for the visu and database
  \param[in] iTrackContainerInfoForvisu common container for tracks for the visu and database
  */
  void SetLineagesInfoContainersForVisu( LineageContainer *iContainerForVisu, 
    TrackContainer *iTrackContainerInfoForvisu);

  /**
  \brief get all the data from the database to load all the lineages for the imagingsession
  into the table widget and the container for the visu
  \param[in] iDatabaseConnector connection to the database
  */
  void DisplayInfoAndLoadVisuContainerForAllLineages(
    vtkMySQLDatabase *iDatabaseConnector);

  /**
  \brief create a new lineage with IDRoot in the database, add it in the
  TW and in the visu container
  \param[in] iDatabaseConnector connection to the database
  \param[in] iTrackRoot ID of the track that is the root of the new lineage to create
  \return the ID of the new lineage just created
  */
  unsigned int CreateNewLineageWithTrackRoot(
      vtkMySQLDatabase *iDatabaseConnector, unsigned int iTrackRoot);

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

  /*
   * \brief Update the scalars for all the divisions of all the lineages
   */
  void UpdateDivisionsScalars();
  /*
   * \brief Update the scalars for all the divisions of the given lineage
   * \param[in] iLineageID ID of the lineage to be processed
   */
  void UpdateDivisionsScalars( unsigned int iLineageID);
  /*
   * \brief Update the color for all the divisions of all the lineages
   */
  void UpdateDivisionsColors();
  /*
   * \brief Update the color for all the divisions of the given lineage
   * \param[in] iLineageID ID of the lineage to be processed
   */
  void UpdateDivisionsColors( unsigned int iLineageID);


public slots:

  void UpdateElementHighlighting(unsigned int);

protected:
  GoDBTWContainerForLineage      *m_TWContainer;
  LineageContainer               *m_LineageContainerInfoForVisu;
  TrackContainer                 *m_TrackContainerInfoForVisu;

  //virtual pure method in QGoDBTraceManager
  virtual void SetCollectionsTraceNames();

  //virtual pure method in QGoDBTraceManager
  virtual void DisplayInfoForAllTraces(vtkMySQLDatabase *iDatabaseConnector);

  //virtual pure method in QGoDBTraceManager
  virtual void GetTracesInfoFromDBAndModifyContainerForVisu(
    vtkMySQLDatabase* iDatabaseConnector,
    std::list<unsigned int> iListTraceIDs = std::list< unsigned int >());

  /**
  \brief update the trackID root for the lineage with iTrackIDRoot
  \param[in] iDatabaseConnector connection to the database
  \param[in] iLineageID ID of the lineage the TrackIDRoot needs to be updated
  \param[in] iTrackIDRoot ID of the track to be the root of the lineage
  */
  void UpdateTrackRootSelectedLineage(vtkMySQLDatabase* iDatabaseConnector, 
    unsigned int iLineageID, unsigned int iTrackIDRoot);

protected slots:
  //virtual pure method in QGoDBTraceManager
  virtual void UpdateHighlightedElementsInVisuContainer(int iTraceID);

  //virtual pure method in QGoDBTraceManager
  virtual void UpdateVisibleElementsInVisuContainer(int iTraceID);

 //virtual pure method in QGoDBTraceManager
  virtual void SetColorCoding(bool IsChecked);

};
#endif
