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

#ifndef __GoDBImport_h
#define __GoDBImport_h

#include "vtkMySQLDatabase.h"
#include "ContourMeshStructure.h"
#include "ContourMeshContainer.h"
#include "GoDBMeshRow.h"

#include <vector>
#include <map>

#include "QGoIOConfigure.h"

/**
\class GoDBImport
\brief This class get the data of traces from a textfile and save them into
the GoFigure Database
\ingroup DB
*/
class QGOIO_EXPORT GoDBImport
{
public:

  GoDBImport(const std::string & iServerName, const std::string & iLogin,
             const std::string & iPassword, int iImagingSessionID,
             const std::string & iFilename, int iCurrentTimePoint);

  virtual ~GoDBImport();

  /** \brief get the data needed from the import file to
  save the contours listed in it, including the color,
  the mesh they belong to, the tracks the previous meshes
  belong to, etc...and fill the vectors of new IDs and the
  needed info for the visu to add these new contours*/
  void ImportContours();

  /**
  \brief get the data needed from the import file to
  save the meshes listed in it, including the color,their
  intensities,the tracks they belong to, the lineages the
  previous tracks belong to, etc...and fill the vectors of
  new IDs and the needed info for the visu to add these new
  meshes
  */
  void ImportMeshes();

  /**
  \brief get the data needed from the import file to
  save the tracks listed in it, including the color,their
  meshes and intensities,the lineages they belong to etc...
  and fill the vectors of new IDs and the needed info for
  the visu to add these new tracks and meshes.
  */
  void ImportTracks();

  /** \brief return a vector of the IDs for the meshes read from
  the import file and saved in the database*/
  std::vector< int > GetVectorNewMeshIDs()
  {
    return this->m_NewMeshIDs;
  }

  /** \brief return a vector of the IDs for the contours read from
  the import file and saved in the database*/
  std::vector< int > GetVectorNewContourIDs()
  {
    return this->m_NewContourIDs;
  }

  /** \brief return a vector of the IDs for the tracks read from
  the the import file and saved in the database*/
  std::vector< int > GetVectorNewTracksIDs()
  {
    return this->m_NewTracksIDs;
  }

  /** \brief return a vector of the info needed to add in the
  visu the contours read from the import file and
  saved in the database*/
  ContourMeshContainer * GetNewContourInfo()
  {
    return this->m_NewContourInfoForVisu;
  }

private:
  vtkMySQLDatabase *    m_DatabaseConnector;
  std::string           m_ServerName;
  std::string           m_Password;
  std::string           m_Login;
  int                   m_ImagingSessionID;
  int                   m_CurrentTimePoint;
  std::ifstream         m_InFile;
  std::vector< int >    m_NewMeshIDs;
  std::vector< int >    m_NewContourIDs;
  std::vector< int >    m_NewTracksIDs;
  std::vector< int >    m_NewLineageIDs;
  ContourMeshContainer *m_NewContourInfoForVisu;
  ContourMeshContainer *m_NewMeshInfoForVisu;

  typedef std::map< int, int > IntMapType;

  /** \brief Return the name of the field contained in the line*/
  std::string FindFieldName(const std::string & iLine);

  /** \brief Return the value contained in the line and "NoValueOnTheLine"
  if the line doesn't contain any*/
  std::string GetValueForTheLine(const std::string & iLine);

  /** \brief Return true if the line containes "Number Of"*/
  bool IsLineForNumberOfEntities(const std::string & iLine);

  /** \brief Get the values from the Infile, save the non traces entities,
  fill the matching map for old and new IDs and return the current line content*/
  std::string SaveNoTracesEntities(IntMapType & ioMapColorIDs,
                                   IntMapType & ioMapCellTypeIDs,
                                   IntMapType & ioMapSubCellTypeIDs,
                                   IntMapType & ioMapCoordIDs);

  void OpenDBConnection();

  void CloseDBConnection();

  /** \brief Get the info for the traces from the import file and from the matching
   IDs maps previously filled, then save them in the database if their bounding box
   doesn't match any existing ones and save the intensities for the meshes if
   SaveIntensities is set to true*/
  void SaveTracesEntities(const IntMapType  & iMapColorIDs,
                          const IntMapType  & iMapCoordIDs,
                          const std::string & iLineContent,
                          const IntMapType  & iMapCellTypeIDs,
                          const IntMapType  & iMapSubCellTypeIDs,
                          bool SaveIntensities = false);

  /** \brief fill the info needed for the new imported contours to add
  them in the visu*/
  //void FillContourInfoForVisu(
  //std::vector<int> iListContourIDs);

  //void FillMeshInfoForVisu(std::vector<int> iListMeshIDs);

  void SaveIntensityForMesh(std::string & ioLineContent,
                            const IntMapType & iMapMeshIDs,
                            const IntMapType & iMapColorIDs);

  /** \brief get the values from the import file,save the
  corresponding number of entities in the database, return
  the last line content from the import file and update the
  matchingIDs with the new ones matching the old ones written in
  the import file*/
  template< typename T >
  std::string SaveImportedEntitiesInDatabase(int iNumberOfEntities,
                                             IntMapType & ioMapMatchingIDs)
  {
    std::string LineContent;

    for ( int i = 0; i < iNumberOfEntities; i++ )
      {
      T EntityToSave;
      LineContent = this->GetValuesFromInfile< T >(EntityToSave);
      int OldID = atoi( EntityToSave.GetMapValue( EntityToSave.GetTableIDName() ).c_str() );
      if (OldID > 0) // in case their is an error in the file to import
        {
        EntityToSave.SetField(EntityToSave.GetTableIDName(), "0");
        int NewID = EntityToSave.SaveInDB(this->m_DatabaseConnector);
        ioMapMatchingIDs[OldID] = NewID;
        }
      }
    return LineContent;
  }

  /** \brief Get the values from the import File to fill
  the corresponding GoDBRow*/
  template< typename T >
  std::string GetValuesFromInfile(T & ioEntityToFill)
  {
    std::string LineContent;

    getline(this->m_InFile, LineContent);
    std::string FieldName = this->FindFieldName(LineContent);
    std::string ValueForField = this->GetValueForTheLine(LineContent);
    while ( ValueForField != "NoValueOnTheLine" )
      {
      ioEntityToFill.SetField(FieldName, ValueForField);
      getline(this->m_InFile, LineContent);
      ValueForField = this->GetValueForTheLine(LineContent);
      FieldName = this->FindFieldName(LineContent);
      }
    //skip the line </NameOfEntity>:
    getline(this->m_InFile, LineContent);
    return LineContent;
  }

  /** \brief replace in the entity to be saved the fieldname
  with the new IDs created that matches the old one in the
  iMapIDs*/
  template< typename T >
  void ReplaceTheFieldWithNewIDs(const IntMapType & iMapIDs,
                                 std::string iFieldName, T & ioEntity)
  {
    std::string temp = ioEntity.GetMapValue(iFieldName);
    int value = atoi( temp.c_str() );

    typename IntMapType::const_iterator iter = iMapIDs.find( value );

    //in case the value of the field name is 0 which corresponds to
    //an not yet associated value, it won't be found in the map, just return
    if ( iter != iMapIDs.end() )
      {
      int NewID = iter->second;
      ioEntity.SetField(iFieldName, NewID);
      }
  }

  /** \brief replace old IDs found in the import file with
  new created IDs in the trace to be saved for common fields
  for the 4 traces: colorID, coordIDMin, CoordIDMax and
  CollectionID*/
  template< typename T >
  void ReplaceCommonFieldsForTraces(T & ioEntityToSave,
                                    const IntMapType & iMapColorIDs,
                                    const IntMapType & iMapCoordIDs,
                                    const IntMapType & iMapCollectionIDs)
  {
    ioEntityToSave.SetField(
      "ImagingSessionID", this->m_ImagingSessionID);
    this->ReplaceTheFieldWithNewIDs< T >(
      iMapColorIDs, "ColorID", ioEntityToSave);
    this->ReplaceTheFieldWithNewIDs< T >(
      iMapCoordIDs, "CoordIDMax", ioEntityToSave);
    this->ReplaceTheFieldWithNewIDs< T >(
      iMapCoordIDs, "CoordIDMin", ioEntityToSave);
    if ( ioEntityToSave.GetCollectionIDName() != "NoneID" )
      {
      this->ReplaceTheFieldWithNewIDs< T >(
        iMapCollectionIDs, ioEntityToSave.GetCollectionIDName(),
        ioEntityToSave);
      }
  }

  /** brief save all the entities in the database for a given
  trace*/
  template< typename T >
  void SaveTraces(const IntMapType &  iMapColorIDs,
                  const IntMapType &  iMapCoordIDs,
                  const IntMapType &  iMapCollectionIDs,
                  std::string & ioLineContent,
                  std::vector< int > & ioNewTracesIDs,
                  IntMapType & ioMapTraceIDs,
                  const IntMapType & iMapIDsSpecificOne,
                  const IntMapType & iMapIDsSpecificTwo
                  )
  {
    (void) iMapIDsSpecificOne;
    (void) iMapIDsSpecificTwo;

    T   TraceToSave;
    int NumberOfTraces = atoi( this->GetValueForTheLine(ioLineContent).c_str() );

    getline(this->m_InFile, ioLineContent);

    for ( int i = 0; i < NumberOfTraces; i++ )
      {
      ioLineContent = this->GetValuesFromInfile< T >(
        TraceToSave);

      this->ReplaceCommonFieldsForTraces(
        TraceToSave, iMapColorIDs, iMapCoordIDs, iMapCollectionIDs);
      int OldTraceID = atoi( TraceToSave.GetMapValue( TraceToSave.GetTableIDName() ).c_str() );

      // in order the query works, the TraceID to be saved has to be set to 0 otherwise
      // if the TraceID already exists,the query will return the error
      // "Duplicate entry TraceID for key primary":
      TraceToSave.SetField(TraceToSave.GetTableIDName(), "0");
      int NewTraceID = TraceToSave.DoesThisBoundingBoxExist(this->m_DatabaseConnector);

      if ( NewTraceID == -1 )
        {
        NewTraceID = TraceToSave.SaveInDB(this->m_DatabaseConnector);
        // this->m_NewTraceIDs.push_back(NewTraceID);
        }
      else
        {
        std::cout << "The trace" << OldTraceID << " has the same bounding box as ";
        std::cout << "the existing trace " << NewTraceID;
        std::cout << "so the imported contours belonging to the mesh " << OldTraceID;
        std::cout << " will belong to the existing mesh " << NewTraceID << std::endl;
        }
      ioNewTracesIDs.push_back(NewTraceID);
      ioMapTraceIDs[OldTraceID] = NewTraceID;
      }
    }

  void SaveTrackFamilyEntities(const IntMapType & iMapTrackIDs,
                               IntMapType & ioMapTrackFamilyIDs,
                               std::string & ioLineContent);

};
#endif
