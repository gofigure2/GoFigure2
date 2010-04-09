/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Rev: 1188 $  // Revision of last commit
  Date: $Date: 2010-04-06 15:24:27 -0400 (Tue, 06 Apr 2010) $  // Date of last commit
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

#ifndef __GoDBImport_h
#define __GoDBImport_h

#include "vtkMySQLDatabase.h"
#include <vector>
#include <map>


class GoDBImport
{
public:
  GoDBImport(std::string iServerName,std::string iLogin,
    std::string iPassword, int iImagingSessionID,
    std::string iFilename);
  virtual ~GoDBImport();
  void ImportContours();

private:
  vtkMySQLDatabase*        m_DatabaseConnector;
  std::string              m_ServerName;
  std::string              m_Password;
  std::string              m_Login;
  int                      m_ImagingSessionID;
  std::ifstream            m_InFile;

  /** \brief Return the name of the field contained in the line*/
  std::string FindFieldName(std::string iLine);

  /** \brief Return the value contained in the line and "NoValueOnTheLine"
  if the line doesn't contain any*/
  std::string GetValueForTheLine(std::string iLine);

  /** \brief Return true if the line containes "Number Of"*/
  bool IsLineForNumberOfEntities(std::string iLine);

  /** \brief Get the values from the Infile, save the non traces entities, 
  fill the matching map for old and new IDs and return the current line content*/
  std::string SaveNoTracesEntities(std::map<int,int> & ioMapColorIDs,
    std::map<int,int> & ioMapCellTypeIDs,std::map<int,int> & ioMapSubCellTypeIDs,
    std::map<int,int> & ioMapCoordIDs);
  void OpenDBConnection();
  void CloseDBConnection();
  
  /** \brief Get the info for the meshes to import for the contours, from
  the infile and from the matching IDs maps previously filled*/
  void SaveMeshes(std::map<int,int> iMapColorIDs,
    std::map<int,int> iMapCellTypeIDs,
    std::map<int,int> iMapSubCellTypeIDs,
    std::map<int,int> iMapCoordIDs,
    std::string & ioLineContent,
    std::map<int,int> & ioMapMeshIDs);
  
  void SaveContours(std::map<int,int> iMapColorIDs,
    std::map<int,int> iMapCoordIDs,
    std::string & ioLineContent,
    std::map<int,int> iMapMeshIDs);

  /** \brief get the values from the Infile,save the 
  corresponding number of entities in the database and return
  the last line content from the infile*/
  template< typename T >
  std::string SaveImportedEntitiesInDatabase(int iNumberOfEntities,
    std::map<int,int> & ioMapMatchingIDs)
    {
    std::string LineContent;
    for(int i = 0; i < iNumberOfEntities; i++)
      {
      T EntityToSave;
      LineContent = this->GetValuesFromInfile<T>(EntityToSave);
      int OldID = atoi(EntityToSave.GetMapValue(EntityToSave.GetTableIDName()).c_str());
      int NewID = EntityToSave.SaveInDB(this->m_DatabaseConnector);
      ioMapMatchingIDs[OldID]= NewID;  
      }
    return LineContent;
    }
  /** \brief Get the values from the inFile to fill the corresponding GoDBRow*/
  template< typename T >
  std::string GetValuesFromInfile(T & ioEntityToFill)
    {
    std::string LineContent;
    getline(this->m_InFile,LineContent);
    std::string FieldName = this->FindFieldName(LineContent);
    std::string ValueForField = this->GetValueForTheLine(LineContent);
    while (ValueForField != "NoValueOnTheLine")
      {
      ioEntityToFill.SetField(FieldName,ValueForField);
      getline(this->m_InFile,LineContent);
      ValueForField = this->GetValueForTheLine(LineContent);
      FieldName = this->FindFieldName(LineContent);
      }   
    //skip the line </NameOfEntity>:
    getline(this->m_InFile,LineContent);
    return LineContent;
    }
  /** \brief replace in the entity to be saved the fieldname
  with the new IDs created that matches the old one in the 
  iMapIDs*/
  template< typename T >
  void ReplaceTheFieldWithNewIDs(std::map<int,int> iMapIDs, 
    std::string iFieldName, T & ioEntity)
  {
    std::map<int,int>::iterator iter =
      iMapIDs.find(atoi(ioEntity.GetMapValue(iFieldName).c_str()));
    int NewID = iter->second;
    ioEntity.SetField(iFieldName,NewID);
  }

  template< typename T >
  void ReplaceCommonFieldsForContourAndMesh( T & ioEntityToSave,
    std::map<int,int> iMapColorIDs,std::map<int,int> iMapCoordIDs)
  {
   ioEntityToSave.SetField(
     "ImagingSessionID",this->m_ImagingSessionID);
   this->ReplaceTheFieldWithNewIDs<T>(
     iMapColorIDs,"ColorID",ioEntityToSave);
   this->ReplaceTheFieldWithNewIDs<T>(
     iMapCoordIDs,"CoordIDMax",ioEntityToSave);
   this->ReplaceTheFieldWithNewIDs<T>(
     iMapCoordIDs,"CoordIDMin",ioEntityToSave);
  }


};
#endif
