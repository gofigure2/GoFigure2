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
#ifndef __GoDBTWContainerForTrack_h
#define __GoDBTWContainerForTrack_h

#include "GoDBTWContainerForTrackLineage.h"
#include "QGoIOConfigure.h"
#include "GoFigureTrackAttributes.h"
/**
\brief
*/
class QGOIO_EXPORT GoDBTWContainerForTrack:public GoDBTWContainerForTrackLineage
{

public:
  GoDBTWContainerForTrack(int iImgSessionID);
  ~GoDBTWContainerForTrack();

  virtual TWContainerType GetContainerForOneSpecificTrace(
    vtkMySQLDatabase *iDatabaseConnector, int iTraceID );

  /**
  \brief set m_TrackAttributes to iMeshAttributes, needs to be called
  before displaying the volume, area values
  \param[in] iTrackAttributes values for the track computed from visu
  */
  void SetTrackAttributes(GoFigureTrackAttributes *iTrackAttributes);

protected:

  GoFigureTrackAttributes* m_TrackAttributes;
  /**
  \brief add the specific info for a track to the columns description
  */
  void SetSpecificInfoForTrackTable();

  void FillRowContainerForTrackComputedValues(int iTrackID);
};
#endif
