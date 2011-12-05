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
#ifndef __GoDBTWContainerForMesh_h
#define __GoDBTWContainerForMesh_h

#include "GoDBTWContainerForContourMesh.h"
#include "QGoIOConfigure.h"
#include "GoFigureMeshAttributes.h"

/** 
\class GoDBTWContainerForMesh 
\brief This class describes the specificities of the GoDBTWContainerForContourMesh for mesh
\ingroup DB
*/
class QGOIO_EXPORT GoDBTWContainerForMesh:public GoDBTWContainerForContourMesh
{
public:
  GoDBTWContainerForMesh(int iImgSessionID);
  ~GoDBTWContainerForMesh();

  //from mother class
  virtual TWContainerType GetContainerLoadedWithAllFromDB(
    vtkMySQLDatabase *iDatabaseConnector, std::list<unsigned int> iListTPs = std::list<unsigned int>());
  //from mother class
  virtual TWContainerType GetContainerForOneSpecificTrace(
    vtkMySQLDatabase *iDatabaseConnector, int iTraceID);

  /**
  \brief set m_MeshAttributes to iMeshAttributes, needs to be called
  before displaying the volume, area values
  \param[in] iMeshAttributes attributes for the mesh computed from visu
  */
  void SetMeshAttributes(GoFigureMeshAttributes *iMeshAttributes);

protected:
  std::vector< std::vector< std::string > > m_ChannelsInfo;
  GoFigureMeshAttributes *                  m_MeshAttributes;

  /**
  \brief add the specific info for a trace to the columns description
  */
  void SetSpecificInfoForMeshTable();

  /**
  \brief add as many columns with their description as there is Channels
  */
  void SetColumnsInfoBasedOnChannelsInfo();

  /**
  \brief get the info for the channels from the database and set the corresponding
  columns for the intensities values
  \param[in] iDatabaseConnector connection to the database
  */
  void SetChannelsInfo(vtkMySQLDatabase *iDatabaseConnector);

  /**
  \brief fill the row container with intensities values only if there is
  more than one mesh in the iVectmeshIDs and that m_MeshAttributes = 0,
  (expl: when all meshes are loaded from the database)
  if not, fill the container with values for volume and area also (expl:
  when a mesh is created from the visu)but the method SetMeshAttributes
  has to be called before.
  \param[in] iDatabaseConnector connection to the database
  \param[in] iVectMeshIDs vector of the meshIDs
  */
  void FillRowContainerForMeshValues(
    vtkMySQLDatabase *iDatabaseConnector, std::vector< std::string > iVectMeshIDs);

  /**
  \overload
  */
  void FillRowContainerForMeshValues(
    vtkMySQLDatabase *iDatabaseConnector, int iMeshID);

  /**
  \brief extract the volume and area values from the m_MeshAttributes,put
  them in ioValuesToFill and put the corresponding columns names in ioSelectFields
  \param[in,out] ioValuesToFill vector of the values where volume and area values
  will be pushed
  \param[in,out] ioSelectFields vector of the selected fields where the volume and
  area columns names in the TW will be pushed
  */
  void GetValuesForSurfaceVolume(
    std::vector< std::vector< std::string > > & ioValuesToFill,
    std::vector< std::string >               & ioSelectFields);

  /**
  \brief get the intensities values from the database,put them in ioValuesToFill,
  and put the corresponding columns names in ioSelectFields (from m_ChannelsInfo)
  \param[in] iDatabaseConnector connection to the database
  \param[in] iVectMeshIDs vector of the meshIDs for which the intensities are needed
  \param[in,out] ioValuesToFill vector of the values where the intensities values
  will be pushed
  \param[in,out] ioSelectFields vector of the selected fields where the intensities
  columns names in the TW will be pushed
  */
  void GetValuesForIntensities(
    vtkMySQLDatabase *iDatabaseConnector,
    std::vector< std::string > iVectMeshIDs,
    std::vector< std::vector< std::string > > & ioValuesToFill,
    std::vector< std::string >               & ioSelectFields);

  /**
  \brief get the intensities values from the database,and put them in ioValuesToFill for
  only one mesh
  \param[in] iMeshID meshID for the mesh the intensity values are needed
  \param[in,out] ioValuesToFill vector of the values where the intensities values
  will be pushed
  \param[in] iDatabaseConnector connection to the database
  */
  void GetIntensityValuesForOneMesh(std::string iMeshID,
                                    std::vector< std::vector< std::string > > & ioValuesToFill,
                                    vtkMySQLDatabase *iDatabaseConnector);

  /**
  \brief sort the values in iResultQuery to fill ioValuesToFill
  \param[in] iResultQuery vector with all the data from the database
  \param[in] iVectMeshIDs vector of all the meshIDs
  \param[in] ioValuesToFill vector of the values where the intensities values
  will be pushed
  */
  void GetValuesToFillForIntensityFromQueryResults(
    std::vector< std::string > iResultQuery, std::vector< std::string > iVectMeshIDs,
    std::vector< std::vector< std::string > > & ioValuesToFill);

  /**
  \brief set the columns and their description for the specific columns for mesh
  except the ones related to channels as a connection to the database is needed
  to know the number of channels
  */
  void SetSpecificColumnsInfoForMesh();
};
#endif
