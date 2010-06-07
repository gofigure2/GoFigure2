/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
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
#include "vtkPolyDataMySQLTextWriter.h"
#include "GoFigureMeshAttributes.h"

class QGOIO_EXPORT GoDBMeshRow : public GoDBTraceRow
{
public:
  GoDBMeshRow();
  /** \brief fill the mesh map with the values gotten from the visualization*/
  /*GoDBMeshRow(vtkMySQLDatabase* DatabaseConnector,
    GoDBCoordinateRow Min, GoDBCoordinateRow Max, unsigned int ImgSessionID,
    vtkPolyData* TraceVisu);*/

  GoDBMeshRow(vtkMySQLDatabase* DatabaseConnector,
    vtkPolyData* TraceVisu,GoDBCoordinateRow Min, GoDBCoordinateRow Max,
    unsigned int ImgSessionID,GoFigureMeshAttributes* iMeshAttributes = 0 );

  GoDBMeshRow( const GoDBMeshRow& iRow);
  
  ~GoDBMeshRow()
    {}
  //int DoesThisBoundingBoxMeshExist(vtkMySQLDatabase* DatabaseConnector);
  void SetCellType(vtkMySQLDatabase* DatabaseConnector,
    std::string CellTypeName);
  void SetSubCellType(vtkMySQLDatabase* DatabaseConnector,
    std::string SubCellTypeName);

  /** \brief save the mesh in the database and return the ID of the new
  created mesh*/
  int SaveInDB(vtkMySQLDatabase* DatabaseConnector);

  void SetCollectionID (int iCollectionID);

  void ReInitializeMapAfterCast();

  void SaveInDBTotalIntensityPerChannel(vtkMySQLDatabase* DatabaseConnector,
      std::map<std::string,int> iNameChannelWithValues);

  void SafeDownCast( GoDBTraceRow& iRow );

protected:
  virtual void InitializeMap();
  std::map<std::string,int> m_NameChannelWithValues;

};

#endif
