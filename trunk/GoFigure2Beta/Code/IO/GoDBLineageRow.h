/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Rev: 455 $  // Revision of last commit
  Date: $Date: 2009-07-28 14:31:26 -0400 (Tue, 28 Jul 2009) $  // Date of last commit
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
#ifndef __GoDBLineageRow_h
#define __GoDBLineageRow_h

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include "GoDBTraceRow.h"
#include "ConvertToStringHelper.h"
#include "vtkMySQLDatabase.h"

class GoDBLineageRow : public GoDBTraceRow
{
public:
  GoDBLineageRow();
  
  ~GoDBLineageRow()
    {}
   /**\brief fill the lineage map with the values gotten from the visualization*/
  GoDBLineageRow(vtkMySQLDatabase* DatabaseConnector,GoDBCoordinateRow Min, 
    GoDBCoordinateRow Max,unsigned int ImgSessionID,vtkPolyData* TraceVisu);

   /**\brief return the LineageID of the Lineage with the same bounding box
  already registered in the DB or -1 if not yet created*/
  int DoesThisBoundingBoxLineageExist(vtkMySQLDatabase* DatabaseConnector);

  /**\brief save the lineage in the database and return the ID of the new 
  created lineage*/
  int SaveInDB(vtkMySQLDatabase* DatabaseConnector);

protected:
  virtual void InitializeMap();
    
};

#endif
