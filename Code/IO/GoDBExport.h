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

#ifndef __GoDBExport_h
#define __GoDBExport_h

#include "vtkMySQLDatabase.h"
#include <vector>

#include "QGoIOConfigure.h"

class QGOIO_EXPORT GoDBExport
  {
public:

  GoDBExport(std::string iServerName, std::string iLogin,
             std::string iPassword, int iImagingSessionID,
             std::string iFilename);
  virtual ~GoDBExport();

  /** \brief get all the imagingsession info,the info
  for the contours, the meshes they belong to and the
  tracks the previous meshes belong to from the database
  and put them in a text file*/
  void ExportContours();

  /** \brief get all the imagingsession info,the info
  for the meshes with points(that can be visualized),
  the tracks they belong to and the lineages the
  previous tracks belong to from the database
  and put them in a text file*/
  void ExportMeshes();

private:
  //QGoExport( const QGoExport& );
  //QGoExport operator = ( const QGoExport& );
  vtkMySQLDatabase* m_DatabaseConnector;
  std::string       m_ServerName;
  std::string       m_Password;
  std::string       m_Login;
  int               m_ImagingSessionID;
  std::fstream      m_outfile;
  std::string       m_NameDocXml;

  std::vector<std::string> m_VectorContourIDs;
  std::vector<std::string> m_VectorMeshIDs;
  std::vector<std::string> m_VectorTrackIDs;
  std::vector<std::string> m_VectorLineageIDs;
  std::vector<std::string> m_VectorChannelIDs;

  /** \brief return a vector of pair containing the name of the info as .first
  and the info as .second. for Imagingsession such as Name, creation date and
  microscope name*/
  std::vector<std::pair<std::string, std::string> > GetImagingSessionInfoFromDB();
  /** \brief return a pair containing as .first the iNameInfo and as.second the
  corresponding info found in the Database for the table imagingsession*/
  std::pair<std::string, std::string> GetOneInfoFromDBForImgSession(
    std::string iNameInfo);

  /** \brief Write the generale info about the textfile*/
  void WriteGeneraleInfo();

  /** \brief get the info from the database for all the entities from a table or
  with a limitation defined with field and value and write them in the output file
  after having written first the number of entities to be described*/
  template<typename T>
  void WriteTableInfoFromDB(std::string field, std::string value)
  {
    T                        TableRow;
    std::vector<std::string> ListTableIDs = ListSpecificValuesForOneColumn(
      this->m_DatabaseConnector, TableRow.GetTableName(),
      TableRow.GetTableIDName(), field, value);
    std::vector<std::string>::iterator iter = ListTableIDs.begin();
    while (iter != ListTableIDs.end())
      {
      std::vector<std::pair<std::string, std::string> > EntityInfo =
        this->GetOneEntityInfoFromDB(*iter, TableRow);
      this->WriteOnTheOutputFile(TableRow.GetTableName(), EntityInfo);
      iter++;
      }
  }
  /** \brief get the info from the database for all the entities from a table
  which IDs are in iListIDs and write them in the output file*/
  template<typename T>
  void WriteTableInfoFromDB(std::vector<std::string> iListIDs)
  {
    T TableRow;
    if (iListIDs.empty())
      {
      this->WriteNumberOfEntities(TableRow.GetTableName(), 0);
      return;
      }
    this->WriteNumberOfEntities(TableRow.GetTableName(), iListIDs.size());
    std::vector<std::string>::iterator iter = iListIDs.begin();
    while (iter != iListIDs.end())
      {
      std::vector<std::pair<std::string, std::string> > EntityInfo =
        this->GetOneEntityInfoFromDB(*iter, TableRow);
      this->WriteOnTheOutputFile(TableRow.GetTableName(), EntityInfo);
      iter++;
      }
  }
  /** \brief get the info with their names for an entity from the database
  and put them in a vector of pair of string (name of the info + value of the info)*/
  template<typename T>
  std::vector<std::pair<std::string, std::string> >
  GetOneEntityInfoFromDB(std::string iEntityID, T iTableRow)
  {
    std::vector<std::pair<std::string, std::string> > oEntityInfo;
    iTableRow.SetValuesForSpecificID(atoi(iEntityID.c_str()), this->m_DatabaseConnector);
    std::vector<std::string>           FieldNames = iTableRow.GetVectorColumnNames();
    std::vector<std::string>::iterator iter = FieldNames.begin();
    while (iter != FieldNames.end())
      {
      std::pair<std::string, std::string> FieldInfo;
      FieldInfo.first = *iter;
      FieldInfo.second = iTableRow.GetMapValue(*iter);
      oEntityInfo.push_back(FieldInfo);
      iter++;
      }
    return oEntityInfo;
  }

  /** \brief fill the different vectors needed for the queries
  depending if the vectors of IDs are empty or not: get the
  tables names, the key for the table and the tracesIDs*/
  void GetVectorsTableNamesTracesIDsAndFields(
    std::vector<std::string>& ioVectorTableNames,
    std::vector<std::vector<std::string> >& ioVectorTracesIDs,
    std::vector<std::string>& ioVectorFields,
    bool IncludeChannelIDs = false);

  /** \brief Get the celltype and subcelltype for the needed meshes from
  the database and write them on the output file*/
  void WriteCellTypeAndSubCellTypeInfoFromDatabase();

  /** \brief get the contours info which IDs are in the m_VectorContourIDs
  from the database and write them on the output file*/
  void WriteContoursInfoFromDatabase();

  /** \brief get the tracks info which IDs are in the m_VectorTrackIDs
  from the database and write them on the output file*/
  void WriteTracksInfoFromDatabase();

  /** \brief get the meshes info which IDs are in the m_VectorMeshIDs
  from the database and write them on the output file*/
  void WriteMeshesInfoFromDatabase();

  /** \brief get the lineages info which IDs are in the m_VectorLineageIDs
  from the database and write them on the output file*/
  void WriteLineagesInfoFromDatabase();

  /** \brief get the channels info which IDs are in the m_VectorChannelIDs
  from the database and write them on the output file*/
  void WriteChannelsInfoFromDatabase();

  /** \brief get the info for the intensities corresponding to the m_VectorMeshIDs
  and the m_VectorChannelIDs and write them on the output file*/
  void WriteIntensityInfoFromDatabase();

  /** \brief get the IDs of the contour belonging to the current imagingsession
  and fill the m_VectorContourIDs with them*/
  void UpdateVectorContourIDsForExportContours();

  /** \brief when exporting contours, if the contours belong to
  meshes, the info regarding these meshes are needed also, so fill
  m_VectorMeshIDs with these meshes IDs*/
  void UpdateVectorMeshIDsForExportContours();

  /** \brief when exporting meshes, we don't export the potential contours
  associated to the meshes, so we clear m_VectorContourIDs*/
  void UpdateVectorContourIDsForExportMeshes();

  /** \brief when exporting meshes, we export only the meshes with a 3D surface
  so we fill the m_VectorMeshIDs with the meshes with a non empty "Points"
  column from the database*/
  void UpdateVectorMeshIDsForExportMeshes();

  /** \brief when exporting meshes, the total intensity per channel has to be
  calculated, and the info for the channels need to be stored*/
  void UpdateVectorChannelIDsForExportMeshes();

  /** \brief check if for the meshes IDs found in the m_VectorMeshIDs,
  the corresponding meshes belongs to tracks, if so these tracks IDs
  are put in the m_VectorTrackIDs*/
  void UpdateVectorTrackIDsToExportInfo();

  /** \brief check if for the tracks IDs found in the m_VectorTrackIDs,
  the corresponding tracks belongs to lineages, if so these lineages IDs
  are put in the m_VectorLineageIDs*/
  void UpdateVectorLineageIDsToExportInfo();

  /** \brief fill the different vectors of traces IDs corresponding to
  the contours to export*/
  void UpdateAllVectorTracesIDsToExportContours();

  /** \brief fill the different vectors of traces IDs corresponding to
  the meshes to export*/
  void UpdateAllVectorTracesIDsToExportMeshes();

  /** \brief get the colors info from the database for the corresponding traces
  to export and write them in the output file*/
  void WriteTheColorsInfoFromDatabase();

  /** \brief get the coordinates without doublon corresponding to the coordidmax
  and min of the traces to export from the database and write them in the
  output file*/
  void WriteCoordinatesInfoFromDatabase();

  /** \brief return <iName> */
  std::string GetNameWithBrackets(std::string iName);

  /** \brief return </iName> */
  std::string GetNameWithSlashBrackets(std::string iName);

  /**\ brief write on the output file the info contained in the vector with
  the name of the entity they describe*/
  void WriteOnTheOutputFile(std::string iNameOfEntity,
                            std::vector<std::pair<std::string, std::string> > iInfoToWrite);

  /** \brief write on the output file the number of entities that are exported*/
  void WriteNumberOfEntities(std::string iNameOfEntity, int iNumber);

  /** \ brief add 2 spaces to the output file for xml tabulation*/
  void AddTabulation();

  void OpenDBConnection();
  void CloseDBConnection();

  };
#endif
