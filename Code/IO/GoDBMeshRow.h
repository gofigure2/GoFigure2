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
#ifndef __GoDBMeshRow_h
#define __GoDBMeshRow_h

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include "GoDBTraceRow.h"
#include "GoDBCoordinateRow.h"
#include "ConvertToStringHelper.h"
#include "vtkMySQLDatabase.h"
#include "vtkPolyData.h"
#include "GoFigureMeshAttributes.h"

/**
\class GoDBMeshRow
\brief this class manages the map with the keys matching the fields of the
Mesh gofiguredatabase table and values of the map matching a row of the Mesh table
\ingroup DB
*/
class QGOIO_EXPORT GoDBMeshRow:public GoDBTraceRow
{
public:
  GoDBMeshRow();
  
  /**
  \brief fill the mesh map with the values gotten from the visualization
  \param[in] DatabaseConnector connection to the database
  \param[in] TraceVisu vtkPolyData the points will be extracted from to create 
  a string for "Points"
  \param[in] Min coordinate row for the minimum of the bounding box
  \param[in] Max coordinate row for the maximum of the bounding box
  \param[in] ImgSessionID ID of the current imagingsession
  \param[in] iMeshAttributes container for intensity values
  */
  explicit GoDBMeshRow(vtkMySQLDatabase *DatabaseConnector,
                       vtkPolyData *TraceVisu, GoDBCoordinateRow Min, GoDBCoordinateRow Max,
                       unsigned int ImgSessionID, GoFigureMeshAttributes *iMeshAttributes = 0);

  GoDBMeshRow(const GoDBMeshRow & iRow);

  /**
  \brief 
  \param[in] ImagingSessionID ID of the current imagingsession
  */
  GoDBMeshRow(unsigned int ImagingSessionID);

  //constructor GoDBTraceRow
  GoDBMeshRow(unsigned int iExistingID,vtkMySQLDatabase *iDatabaseConnector);

  ~GoDBMeshRow();

  //int DoesThisBoundingBoxMeshExist(vtkMySQLDatabase* DatabaseConnector);

  /**
  \brief put the value of map["Celltype"] to CellTypeName
  \param[in] DatabaseConnector connection to the database
  \param[in] CellTypeName 
  */
  void SetCellType(vtkMySQLDatabase *DatabaseConnector,
                   std::string CellTypeName);

  /**
  \brief put the value of map["SubCelltype"] to SubCellTypeName
  \param[in] DatabaseConnector connection to the database
  \param[in] SubCellTypeName 
  */
  void SetSubCellType(vtkMySQLDatabase *DatabaseConnector,
                      std::string SubCellTypeName);

  //mother class method
  virtual int SaveInDB(vtkMySQLDatabase *DatabaseConnector);

  //void ReInitializeMapAfterCast();


  /**
  \brief create the intensities per channel based on the channel names and values
  contained in iNameChannelWithValues
  \param[in] DatabaseConnector connection to the database
  \param[in] iNameChannelWithValues map with keys as channel names and values as intensities
  */
  void SaveInDBTotalIntensityPerChannel(vtkMySQLDatabase *DatabaseConnector,
                                        std::map< std::string, int > iNameChannelWithValues);

  /**
  \brief convert a GoDBTraceRow in GoDBMeshRow
  \param[in] iRow 
  */
  void SafeDownCast(GoDBTraceRow & iRow);

  /**
  \overload
  */
  void SetTheDataFromTheVisu(vtkMySQLDatabase *DatabaseConnector,
                             vtkPolyData *TraceVisu, GoDBCoordinateRow iCoordMin,
                             GoDBCoordinateRow iCoordMax,
                             GoFigureMeshAttributes *iMeshAttributes);

  /**
  \brief static method.get the CellTypeID base on the name of the celltype
  \param[in] iDatabaseConnector connection to the database
  \param[in] iCellTypeName name of the celltype
  \return the ID of the celltype
  */
  static int GetCellTypeID(vtkMySQLDatabase *iDatabaseConnector,
                                 std::string iCellTypeName);

  /**
  \brief static method.get the SubCellTypeID base on the name of the subcelltype
  \param[in] iDatabaseConnector connection to the database
  \param[in] iSubCellTypeName name of the subcelltype
  \return the ID of the subcelltype
  */
  static int GetSubCellTypeID(vtkMySQLDatabase *iDatabaseConnector,
                                 std::string iSubCellTypeName);

protected:
  //mother class method
  virtual void InitializeMap();

  std::map< std::string, int > m_NameChannelWithValues;
};

#endif
