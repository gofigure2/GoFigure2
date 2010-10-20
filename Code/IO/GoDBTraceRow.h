/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

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

class QGOIO_EXPORT GoDBTraceRow:public GoDBRow
{
public:
  GoDBTraceRow();

  /**\brief fill the trace map with the values gotten from the visualization*/
  GoDBTraceRow(vtkMySQLDatabase *DatabaseConnector, vtkPolyData *TraceVisu,
               GoDBCoordinateRow Min, GoDBCoordinateRow Max, unsigned int ImgSessionID);

  GoDBTraceRow(vtkMySQLDatabase *DatabaseConnector, std::string TraceVisu,
               GoDBCoordinateRow Min, GoDBCoordinateRow Max, unsigned int ImgSessionID);
  GoDBTraceRow(unsigned int ImgSessionID);

  ~GoDBTraceRow()
  {}

/**\brief return the TraceID of the Trace with the same bounding box
  already registered in the DB or -1 if not yet created*/
  int  DoesThisBoundingBoxExist(vtkMySQLDatabase *DatabaseConnector);

  void SetColor(unsigned int Red, unsigned int Green, unsigned int Blue,
                unsigned int Alpha, std::string ColorName, vtkMySQLDatabase *DatabaseConnector);

  std::string GetCollectionIDName();

  std::string GetCollectionName();

  //set the data from the visu for an existing Trace
  void SetTheDataFromTheVisu(vtkMySQLDatabase *DatabaseConnector,
                             vtkPolyData *TraceVisu,
                             GoDBCoordinateRow iCoordMin,
                             GoDBCoordinateRow iCoordMax);

  /**\brief check in the database if the Coordinate Min adn Max already exists,
  if yes fill the map["CoordIDMin"] and ["CoordIDmax"] with the existing CoordinateID
  if not, create the coordinates in the database and fill the map with the new created ID,
  if the bounding box already exists, a cout is generated*/
  void SetTheBoundingBox(vtkMySQLDatabase *iDatabaseConnector,
                         GoDBCoordinateRow Min, GoDBCoordinateRow Max);

  void SetCollectionID(unsigned int iCollectionID);

  void SetValuesForSpecificID(int ID, vtkMySQLDatabase *iDatabaseConnector);

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
