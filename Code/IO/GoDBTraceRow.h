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
#ifndef __GoDBTraceRow_h
#define __GoDBTraceRow_h

#include "vtkMySQLDatabase.h"
#include "GoDBCoordinateRow.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMySQLTextWriter.h"
#include "GoDBRecordSetHelper.h"
#include "GoDBRecordSet.h"

/**
\class GoDBTraceRow
\brief abstract class to be inherited by Contour,Mesh,Track and GoDBLineageRow
\ingroup DB
*/
class QGOIO_EXPORT GoDBTraceRow:public GoDBRow
{
public:
  GoDBTraceRow();

  /**
  \brief fill the trace map with the values gotten from the visualization
  \param[in] DatabaseConnector connection to the database
  \param[in] TraceVisu vtkPolyData the points will be extracted from to create 
  a string for "Points"
  \param[in] Min coordinate row for the minimum of the bounding box
  \param[in] Max coordinate row for the maximum of the bounding box
  \param[in] ImgSessionID ID of the current imagingsession
  */
  GoDBTraceRow(vtkMySQLDatabase *DatabaseConnector, vtkPolyData *TraceVisu,
               GoDBCoordinateRow Min, GoDBCoordinateRow Max, unsigned int ImgSessionID);

  /**
  \brief constructor
  \param[in] DatabaseConnector connection to the database
  \param[in] TraceVisu vtkPolyData the points will be extracted from
  \param[in] Min coordinate row for the minimum of the bounding box
  \param[in] Max coordinate row for the maximum of the bounding box
  \param[in] ImgSessionID ID of the current imagingsession
  */
  GoDBTraceRow(vtkMySQLDatabase *DatabaseConnector, std::string TraceVisu,
               GoDBCoordinateRow Min, GoDBCoordinateRow Max, unsigned int ImgSessionID);

  /**
  \brief 
  \param[in] ImgSessionID ID of the current imagingsession
  */
  GoDBTraceRow(unsigned int ImgSessionID);

  ~GoDBTraceRow()
  {}

  /**
  \brief check if a trace already has the same bounding box
  \param[in] DatabaseConnector connection to the database
  \return the TraceID of the Trace with the same bounding box
  already registered in the DB or -1 if not yet created 
  */
  int  DoesThisBoundingBoxExist(vtkMySQLDatabase *DatabaseConnector);


  /**
  \brief get the colorID corresponding to the rgba values and set the colorID
  field of the trace with it
  \param[in] Red
  \param[in] Green
  \param[in] Blue
  \param[in] Alpha
  \param[in] ColorName
  \param[in] DatabaseConnector connection to the database
  */
  void SetColor(unsigned int Red, unsigned int Green, unsigned int Blue,
                unsigned int Alpha, std::string ColorName, vtkMySQLDatabase *DatabaseConnector);

  /**
  \brief
  \return the collectionID name
  */
  std::string GetCollectionIDName();

  /**
  \brief
  \return the collection name
  */
  std::string GetCollectionName();

  /**
  \brief set the data from the visu for an existing Trace
  \param[in] DatabaseConnector connection to the database
  \param[in] TraceVisu vtkPolyData the points will be extracted from
  \param[in] iCoordMin coordinate row for the minimum of the bounding box
  \param[in] iCoordMax coordinate row for the maximum of the bounding box
  */
  void SetTheDataFromTheVisu(vtkMySQLDatabase *DatabaseConnector,
                             vtkPolyData *TraceVisu,
                             GoDBCoordinateRow iCoordMin,
                             GoDBCoordinateRow iCoordMax);

  /**
  \brief check in the database if the Coordinate Min and Max already exists,
  if yes fill the map["CoordIDMin"] and ["CoordIDmax"] with the existing CoordinateID
  if not, create the coordinates in the database and fill the map with the new created ID,
  if the bounding box already exists, a cout is generated
  \param[in] DatabaseConnector connection to the database
  \param[in] Min coordinate row for the minimum of the bounding box
  \param[in] Max coordinate row for the maximum of the bounding box
  */
  void SetTheBoundingBox(vtkMySQLDatabase *iDatabaseConnector,
                         GoDBCoordinateRow Min, GoDBCoordinateRow Max);

  /**
  \brief set the collectionID field to iCollectionID
  \param[in] iCollectionID collectionID to be set to
  */
  void SetCollectionID(unsigned int iCollectionID);

  /**
  \brief get the data from the database for a specific ID and fill the GoDBTraceRow
  with it
  \param[in] ID ID of the trace the data will be fetched for
  \param[in] iDatabaseConnector connection to the database
  */
  void SetValuesForSpecificID(int ID, vtkMySQLDatabase *iDatabaseConnector);

  /**
  \brief save the row in the database if the TraceID is set to "0", update the
  existing traceRow if the TraceID is <> 0
  \param[in] DatabaseConnector connection to the database
  \return the ID of the updated or saved trace
  */
  virtual int SaveInDB(vtkMySQLDatabase *DatabaseConnector) = 0;

protected:

  virtual void InitializeMap();

  /**\brief check in the database if the Coordinate Min adn Max already exists,
  if yes fill the map["CoordIDMin"] and ["CoordIDmax"] with the existing CoordinateID
  if not, create the coordinates in the database and fill the map with the new created ID,
  if the bounding box already exists, a cout is generated*/
  // void CreateBoundingBox(vtkMySQLDatabase* DatabaseConnector,
  // GoDBCoordinateRow Min,
  //                      GoDBCoordinateRow Max);

  std::string m_CollectionIDName;
  std::string m_CollectionName;

  /**
  \brief save the row in the database if the TraceID is set to "0", update the
  existing traceRow if the TraceID is <> 0
  \param[in] iDatabaseConnector connection to the database
  \param[in] iTrace trace to be saved
  \tparam T children of GoDBTraceRow
  \return the ID of the updated or saved trace
  */
  template< typename T >
  int SaveInDBTemplate(vtkMySQLDatabase *iDatabaseConnector, T iTrace)
  {
    int SavedTraceID;

    //in case the ID is different from 0, this means the values have been
    //updated for this mesh, so we update it in the database:
    if ( this->m_MapRow[this->m_TableIDName] != "0" )
      {
      SavedTraceID = UpdateOneNewObjectInTable< T >(iDatabaseConnector,
                                                    &iTrace);
      }
    else
      {
      SavedTraceID = AddOnlyOneNewObjectInTable< T >(iDatabaseConnector,
                                                     this->m_TableName, iTrace, this->m_TableIDName);
      this->SetField(this->m_TableIDName, SavedTraceID);
      }
    return SavedTraceID;
  }
};
#endif
