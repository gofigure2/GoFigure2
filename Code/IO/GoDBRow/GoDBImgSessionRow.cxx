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
  this->m_MapRow["ImagingSessionID"] = std::string( "0" );//ConvertToString< int >(0);
  this->m_MapRow["CoordIDMax"] = std::string( "0" );//ConvertToString< int >(0);
  this->m_MapRow["CoordIDMin"] = std::string( "0" );//ConvertToString< int >(0);
  this->m_MapRow["Name"] = "";
  this->m_MapRow["Description"] = "";
  this->m_MapRow["ImagesTimeInterval"] = std::string( "0" );//ConvertToString< float >(0);
  this->m_MapRow["RealPixelDepth"] = std::string( "0" );//ConvertToString< float >(0);
  this->m_MapRow["RealPixelHeight"] = std::string( "0" );//ConvertToString< float >(0);
  this->m_MapRow["RealPixelWidth"] = std::string( "0" );//ConvertToString< float >(0);
  this->m_MapRow["ProjectName"] = "";
  this->m_MapRow["MicroscopeName"] = "";
  this->m_MapRow["CreationDate"] = "";
  this->m_MapRow["XImageSize"] = std::string( "0" );//ConvertToString< int >(0);
  this->m_MapRow["YImageSize"] = std::string( "0" );//ConvertToString< int >(0);
  this->m_MapRow["XTileOverlap"] = std::string( "0" );//ConvertToString< float >(0);
  this->m_MapRow["YTileOverlap"] = std::string( "0" );//ConvertToString< float >(0);
  this->m_MapRow["ZTileOverlap"] = std::string( "0" );//ConvertToString< float >(0);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int GoDBImgSessionRow::DoesThisImagingSessionExist(
  vtkMySQLDatabase *DatabaseConnector)
{
  //std::string MicroscopeName = this->GetMapValue("MicroscopeName");
  //std::string CreationDate = this->GetMapValue("CreationDate");
  std::vector< FieldWithValue > Conditions;
  this->AddConditions("MicroscopeName", Conditions);
  this->AddConditions("CreationDate", Conditions);

  return FindOneID(DatabaseConnector, "imagingsession", "ImagingSessionID",
                   Conditions);
}
