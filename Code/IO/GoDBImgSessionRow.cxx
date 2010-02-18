/*=========================================================================
  Author: $Author: lydiesouhait $  // Author of last commit
  Version: $Rev: 374 $  // Revision of last commit
  Date: $Date: 2009-07-08 10:24:11 -0400 (Wed, 08 Jul 2009) $  // Date of last commit
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
#include "GoDBImgSessionRow.h"
#include "SelectQueryDatabaseHelper.h"


GoDBImgSessionRow::GoDBImgSessionRow()
{
  this->InitializeMap();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBImgSessionRow::InitializeMap()
{
  this->m_MapRow["ImagingSessionID"] = ConvertToString<int>(0);
  this->m_MapRow["CoordIDMax"] = ConvertToString<int>(0);
  this->m_MapRow["CoordIDMin"] = ConvertToString<int>(0);
  this->m_MapRow["Name"] = "";
  this->m_MapRow["Description"] = "";
  this->m_MapRow["ImagesTimeInterval"] = ConvertToString<float>(0);
  this->m_MapRow["RealPixelDepth"] = ConvertToString<float>(0);
  this->m_MapRow["RealPixelHeight"] = ConvertToString<float>(0);
  this->m_MapRow["RealPixelWidth"] = ConvertToString<float>(0);
  this->m_MapRow["ProjectName"] = "";
  this->m_MapRow["MicroscopeName"] = "";
  this->m_MapRow["CreationDate"] = "";
  this->m_MapRow["XImageSize"] = ConvertToString<int>(0);
  this->m_MapRow["YImageSize"] = ConvertToString<int>(0);
  this->m_MapRow["XTileOverlap"] = ConvertToString<float>(0);
  this->m_MapRow["YTileOverlap"] = ConvertToString<float>(0);
  this->m_MapRow["ZTileOverlap"] = ConvertToString<float>(0);
}    
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int GoDBImgSessionRow::DoesThisImagingSessionExist(
  vtkMySQLDatabase* DatabaseConnector)
{
  //std::string MicroscopeName = this->GetMapValue("MicroscopeName");
  //std::string CreationDate = this->GetMapValue("CreationDate");

  return FindOneID(DatabaseConnector,"imagingsession", "ImagingSessionID",
    "MicroscopeName",this->GetMapValue("MicroscopeName"),
    "CreationDate",this->GetMapValue("CreationDate"));
}


