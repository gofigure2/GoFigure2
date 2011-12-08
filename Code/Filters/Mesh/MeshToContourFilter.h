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
#ifndef __MeshToContourFilter_h
#define __MeshToContourFilter_h

#include "map"

class vtkPolyData;

/**
 * \class MeshToContourFilter
 * \brief
 **/
class MeshToContourFilter
{
public:

  MeshToContourFilter();
  ~MeshToContourFilter();

  enum ORIENTATION {
    XY = 0,
    XZ = 1,
    YZ = 2
    };

  /**
    * \brief Set polydata to be splitted
    * \param[in] iInput polydata to be splitted
    */
  void SetInput(vtkPolyData* iInput);

  /**
    * \brief Set spacing of the original image to know how many slices to extract
    * \param[in] iX X spacing
    * \param[in] iY Y spacing
    * \param[in] iZ Z spacing
    */
  void SetSpacing(const double& iX, const double & iY, const double& iZ);

  /**
    * \brief Extract contours from polydata given an orientation
    * \param[in] iOrientation Desired Orientation (XY, XZ, YZ)
    * \return Map of polydatas indexed by slice position.
    * \note polydatas has to be deleted
    */
  std::map<double, vtkPolyData*> ExtractPolyData(ORIENTATION iOrientation);

private:
  vtkPolyData*                   m_Input;
  double                         m_Spacing[3];
};

#endif
