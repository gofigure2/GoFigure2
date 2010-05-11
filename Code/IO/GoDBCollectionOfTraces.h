/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Revision$  // Revision of last commit
  Date: $Date$  // Date of last commit
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
#ifndef __GoDBCollectionOfTraces_h
#define __GoDBCollectionOfTraces_h

#include <QString>
#include <QStringList>
#include <QObject>
#include "MegaVTK2Configure.h"
#include "GoDBRecordSetHelper.h"
#include "vtkMySQLDatabase.h"
#include "GoDBRow.h"
#include "GoDBCoordinateRow.h"
#include "GoDBTraceInfoForTableWidget.h"
#include "GoDBTableWidgetContainer.h"
#include <QStringList>
#include <map>

class QGOIO_EXPORT GoDBCollectionOfTraces
{

public:

  explicit GoDBCollectionOfTraces();
  explicit GoDBCollectionOfTraces(
    std::string CollectionName,std::string Traces);
  virtual  ~GoDBCollectionOfTraces();

  typedef GoDBTableWidgetContainer::DBTableWidgetContainerType
    DBTableWidgetContainerType;

  /** \brief fill the global values for the collection of traces,
  the databaseconnector is only useful if
  the trace is a mesh*/
  void SetCollectionInfo(std::string iCollectionName,
    std::string iTracesName);
  
  void SetImgSessionID (unsigned int iImgSessionID);
  
  /** \brief Delete the corresponding trace in the database*/
  void DeleteTraceInDB(int TraceToDelete,vtkMySQLDatabase* DatabaseConnector);

  /** \brief Delete in the Database all the traces listed in the list of int */
  void DeleteTracesInDB(std::list<int> TracesToDelete,
    vtkMySQLDatabase* DatabaseConnector);

  /** \brief Calcul the bounding box of the corresponding collection and update
  it in the database*/
  void RecalculateDBBoundingBox(
    vtkMySQLDatabase* iDatabaseConnector,int iCollectionID);

  /** \brief Update the collectionID of the selected traces in the DB trace table,
  update the bounding box of the collection, update the bounding boxes of the 
  previous collections the traces belonged to and return the list of the 
  previous collection with an updated bounding box*/
  std::list<int> UpdateDBDataForAddedTracesToExistingCollection(
    std::list<int> ListSelectedTraces,int iNewCollectionID,
    vtkMySQLDatabase* DatabaseConnector);

  /** \brief Update the collectionID of the selected traces in the DB traces table
  with the new collectionID: */
  void UpdateCollectionIDOfSelectedTraces(
    std::list<int> iListSelectedTraces,int iCollectionID,
    vtkMySQLDatabase* iDatabaseConnector);

  /** \brief Return a map with all the ColumnNames for the table widget to be
  completed by the value for each column:*/
  std::list<std::string> GetListColumnsNamesForTableWidget();
  /** \brief return the name of the trace of the current collection which
  is also a collection of*/
  std::string GetCollectionOf();

  GoDBTableWidgetContainer* GetLinkToRowContainer()
    {
    return m_LinkToRowContainer;
    }

  /** \brief get the results of the queries and put them in the row container corresponding
  to all the data needed to fill the table widget for the traces and return the corresponding
  row container*/
  DBTableWidgetContainerType GetRowContainer(vtkMySQLDatabase* DatabaseConnector);
  
  /** \brief get the results of the queries and put them in the row container corresponding
  to all the data needed to fill the table widget for the new created trace and return the 
  link to the corresponding row container which has only 1 row*/
  GoDBTableWidgetContainer*  GetLinkToNewCreatedTraceContainer(
    vtkMySQLDatabase* iDatabaseConnector,int iTraceID = 0);

  /** \brief get the results of the queries and put them in the row container corresponding
  to all the data needed to fill the table widget for the updated trace and return the 
  link to the corresponding row container which has only 1 row*/
  GoDBTableWidgetContainer* GetLinkToUpdatedTraceContainer(
    vtkMySQLDatabase* iDatabaseConnector, int iUpdatedTraceID);

  /** \brief get the channels info from the database and set the channels info of the
  GoDBTableWidgetContainer for mesh*/
  void SetChannelsInfo(vtkMySQLDatabase* DatabaseConnector,
    GoDBTableWidgetContainer* iLinkToRowContainer = 0);
 /* int CreateNewCollectionFromSelection(
  std::list<int> iListSelectedTraces, vtkMySQLDatabase* DatabaseConnector,
    GoDBTraceRow iNewCollection);*/

  QStringList ListCollectionID( vtkMySQLDatabase* DatabaseConnector);

  /** \brief create the first collection in the database and return the corresponding
  ID*/
  int CreateCollectionWithNoTraces(vtkMySQLDatabase* DatabaseConnector, 
    GoDBTraceRow iNewCollection, int iTimePoint);

  template<typename T>
  int CreateNewCollectionFromSelection(
  std::list<int> iListSelectedTraces, vtkMySQLDatabase* DatabaseConnector,
    T iNewCollection)
  {
  /** \todo merge createNewCollectionFromSelection and CreateCollectionWithNotraces*/
   //the following fields are common to all the collections, the one
   //that are different are specified in QGoPrintDatabase:
   iNewCollection.SetField("ImagingSessionID",this->m_ImgSessionID);
   //the CollectionID is set to 0 as it is a new Collection that will be created, not
   //contours to be added to an existing collection:
   iNewCollection.SetField("CoordIDMax",this->GetCoordMaxID(
     DatabaseConnector,0,iListSelectedTraces));
   iNewCollection.SetField("CoordIDMin",this->GetCoordMinID(
     DatabaseConnector,0,iListSelectedTraces));

   int NewCollectionID = this->CreateNewCollection(DatabaseConnector,iNewCollection);

    UpdateCollectionIDOfSelectedTraces(iListSelectedTraces,NewCollectionID, 
      DatabaseConnector);

   return NewCollectionID;
   }

protected:

  std::string  m_CollectionName;
  std::string  m_CollectionIDName;
  std::string  m_TracesName;
  std::string  m_TracesIDName;
  unsigned int m_ImgSessionID;

  std::vector<GoDBTraceInfoForTableWidget>  m_ColumnsInfos;
  DBTableWidgetContainerType                m_RowContainer;
  GoDBTableWidgetContainer*                 m_LinkToRowContainer;

  /** \brief Create a new collection Row in the collection table and
  return the collectionID from the created row: */
  int CreateNewCollection();

   /** \brief create a new collection in the database and return the corresponding
  ID*/
  int CreateNewCollection(vtkMySQLDatabase* DatabaseConnector, GoDBTraceRow myNewObject);
  
  /** \brief set the timepoint for the coordmax and coordmin of a new created mesh 
  without any contours*/
  void SetTheTimePointForMesh(int iTimePoint,GoDBTraceRow & ioNewMesh,
    vtkMySQLDatabase* iDatabaseConnector);

  /** \brief get the max of all the coordinates in the ListSelectedTraces, compare it
  to the max of the existing collection and update the max coordinate of the bounding
  box of the collection if necessary*/
  int GetCoordMaxID(vtkMySQLDatabase* DatabaseConnector,
    int CollectionID,std::list<int> ListSelectedTraces);

  /** \brief get the max of all the coordinates of the traces belonging to the 
  collection and record them in the database*/
  int GetCoordMaxID(vtkMySQLDatabase* DatabaseConnector,int iCollectionID);

  /** \brief get the min of all the coordinates in the ListSelectedTraces, compare it
  to the min of the existing collection and update the min coordinate of the bounding
  box of the collection if necessary*/
  int GetCoordMinID(vtkMySQLDatabase* DatabaseConnector,
    int CollectionID,std::list<int> ListSelectedTraces);

  /** \brief get the min of all the coordinates of the traces belonging to the 
  collection and record them in the database*/
  int GetCoordMinID(vtkMySQLDatabase* DatabaseConnector,int iCollectionID);
  
  int GetCoordIDMaxForBoundingBoxWithNoTraces(
    vtkMySQLDatabase* iDatabaseConnector);
  int GetCoordIDMinForBoundingBoxWithNoTraces(
    vtkMySQLDatabase* iDatabaseConnector);
  /** \brief return the coordinate min of all the coordinates of the
  selected traces*/
  GoDBCoordinateRow GetSelectingTracesCoordMin(
    vtkMySQLDatabase* DatabaseConnector, std::vector<std::string> ListSelectedTracesID);

  /** \brief Update in the database the coordid max and min of the collection*/
  void UpdateCollectionBoundingBoxInDB(int iCoordIDMin, int iCoordIDMax, 
    int iCollectionID, vtkMySQLDatabase* iDatabaseConnector);

  /** \brief return the coordinate max of all the coordinates of the
  selected traces*/
  GoDBCoordinateRow GetSelectingTracesCoordMax(
    vtkMySQLDatabase* DatabaseConnector, std::vector<std::string> ListSelectedTracesID);

  /** \brief return the coordinate min for the existing Collection*/
  GoDBCoordinateRow GetExistingCoordMin(
    vtkMySQLDatabase* DatabaseConnector, int CollectionCoordIDMin,
    int CollectionID );

   /** \brief return the coordinate max for the existing Collection*/
  GoDBCoordinateRow GetExistingCoordMax(
    vtkMySQLDatabase* DatabaseConnector, int CollectionCoordIDMax,
    int CollectionID );

  /** \brief change the collection ID of the trace*/
  void UpdateCollectionIDOfSelectedTrace(int iSelectedTraceID,int inewCollectionID,
    vtkMySQLDatabase* DatabaseConnector);
  
  /** \brief return a vector containing a vector of the Channels Name and a vector of
  the ChannelIDs corresponding to the ImagingSession*/
  std::vector<std::vector<std::string> >  GetChannelsInfo(vtkMySQLDatabase* DatabaseConnector);
  
  void FillRowContainerForIntensityValues(vtkMySQLDatabase* DatabaseConnector,
    std::vector<std::string> iVectMeshIDs,GoDBTableWidgetContainer* iLinkToRowContainer);
};
#endif
