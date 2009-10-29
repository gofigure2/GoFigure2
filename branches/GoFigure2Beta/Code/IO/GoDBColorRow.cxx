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
#include "GoDBColorRow.h"
#include "SelectQueryDatabaseHelper.h"

GoDBColorRow::GoDBColorRow()
{
  this->InitializeMap();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBColorRow::InitializeMap()
{
  this->m_MapRow["ColorID"] = ConvertToString<int>(0);
  this->m_MapRow["Name"] = "";
  this->m_MapRow["Red"] = ConvertToString<int>(0);
  this->m_MapRow["Green"] = ConvertToString<int>(0);
  this->m_MapRow["Blue"] = ConvertToString<int>(0);
  this->m_MapRow["Alpha"] = ConvertToString<int>(0);
  this->m_MapRow["Description"] = "";

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int GoDBColorRow::DoesThisColorAlreadyExists(vtkMySQLDatabase* DatabaseConnector)
{
  std::string Red = this->GetMapValue("Red");
  std::string Blue = this->GetMapValue("Blue");
  std::string Green = this->GetMapValue("Green");
  std::string Alpha = this->GetMapValue("Alpha");

  return FindOneID(DatabaseConnector,"color", "ColorID","Red",Red,"Green",Green,
  "Blue",Blue,"Alpha",Alpha);
}