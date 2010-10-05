/*=========================================================================
  Author: $Author: lydiesouhait$  // Author of last commit
  Version: $Rev: 1972 $  // Revision of last commit
  Date: $Date: 2010-08-16 12:23:05 -0400 (Mon, 16 Aug 2010) $  // Date of last commit
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
#ifndef __GoDBTWContainerForMesh_h
#define __GoDBTWContainerForMesh_h

#include "GoDBTWContainerForContourMesh.h"
#include "QGoIOConfigure.h"
#include "GoFigureMeshAttributes.h"
/** 
\brief 
*/
class QGOIO_EXPORT GoDBTWContainerForMesh: public GoDBTWContainerForContourMesh
  {

public:
  GoDBTWContainerForMesh(int iImgSessionID);
  ~GoDBTWContainerForMesh();

  virtual TWContainerType GetContainerLoadedWithAllFromDB(
  vtkMySQLDatabase* iDatabaseConnector);

  virtual TWContainerType GetContainerForOneSpecificTrace(
  vtkMySQLDatabase* iDatabaseConnector, int iTraceID);

  //virtual TWContainerType GetContainerForLastCreatedTrace(
    //vtkMySQLDatabase* iDatabaseConnector);
  void SetMeshAttributes(GoFigureMeshAttributes* iMeshAttributes);

protected:
  std::vector<std::vector<std::string> >   m_ChannelsInfo;
  GoFigureMeshAttributes* m_MeshAttributes;

  void SetSpecificColumnsInfoForMesh();

  //std::vector<std::vector<std::string> > GetChannelsInfo();

  void SetChannelsInfo(vtkMySQLDatabase* iDatabaseConnector);
  //intensities + surface area + volume
  //if the volume and surface area needs to be filled, the method
  //setmeshattributes had to be called before
  void FillRowContainerForMeshValues(
    vtkMySQLDatabase* iDatabaseConnector, std::vector<std::string> iVectMeshIDs);

  void FillRowContainerForMeshValues(
   vtkMySQLDatabase* iDatabaseConnector, int iMeshID);

  //void FillRowContainerForIntensityValues(
    //vtkMySQLDatabase* iDatabaseConnector,int iMeshID);

  void GetValuesForSurfaceVolume(
    std::vector<std::vector<std::string> > &ioValuesToFill,
    std::vector<std::string>               &ioSelectFields);

  void GetValuesForIntensities(
    vtkMySQLDatabase* iDatabaseConnector, 
    std::vector<std::string> iVectMeshIDs,
    std::vector<std::vector<std::string> > &ioValuesToFill,
    std::vector<std::string>               &ioSelectFields);

  void ClearRowContainerValues();

  //void SetChannelsInfo(vtkMySQLDatabase* iDatabaseConnector);

  };
#endif
