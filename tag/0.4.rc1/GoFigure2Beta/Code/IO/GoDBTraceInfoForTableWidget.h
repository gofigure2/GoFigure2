/*=========================================================================
  Author: $Author: lydiesouhait$  // Author of last commit
  Version: $Rev: 577 $  // Revision of last commit
  Date: $Date: 2009-08-18 10:45:50 -0400 (Tue, 18 Aug 2009) $  // Date of last commit
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
#ifndef __GoDBTraceInfoForTableWidget_h
#define __GoDBTraceInfoForTableWidget_h

#include <string>

/**\brief structure to pass the column information between the Table Widget 
and the Database*/
struct GoDBTraceInfoForTableWidget
{
  std::string     InfoName;
  std::string     ColumnNameTableWidget;//Name of the column in the Table Widget
  std::string     ColumnNameDatabase;//Name of the field in the database
  std::string     TableNameDatabase; //name of the table in the database
  bool            SameFieldForDifferentValues;/*set to true if the same table.field is 
                    called several times and corresponds to different values: expl
                    coordinate.TCoord for both CoordIDMin and CoordIDMax*/
  std::string     TableKeyDatabase; //Name of the primary key of the table "TableNameDatabase"
  std::string     TableForeignKeyDatabase;//Name of the foreign key corresponding in the trace table
  std::string     AccessFromTraceTableThroughWhichTable;
   
  GoDBTraceInfoForTableWidget()
    {
    InfoName = "None";
    ColumnNameTableWidget = "None";
    ColumnNameDatabase = "None";
    TableNameDatabase = "None";
    TableKeyDatabase = "None";
    TableForeignKeyDatabase = "None";
    SameFieldForDifferentValues = false;
    AccessFromTraceTableThroughWhichTable = "None";
    }
  void Clear()
    {
    InfoName = "None";
    ColumnNameTableWidget = "None";
    ColumnNameDatabase = "None";
    TableNameDatabase = "None";
    TableKeyDatabase = "None";
    TableForeignKeyDatabase = "None";
    SameFieldForDifferentValues = false;
    AccessFromTraceTableThroughWhichTable = "None";
    }
};
#endif