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
#ifndef __MeshToContourFilter_txx
#define __MeshToContourFilter_txx

#include <map>

#include "vtkPolyData.h"
#include "vtkPlane.h"
#include "vtkCutter.h"

/**
 * \class MeshToContourFilter
 * \brief
 */

//-----------------------------------------------------------------------------
template< class TContainer >
MeshToContourFilter< TContainer >::
MeshToContourFilter() :
  m_Input(NULL)
{
  SetSpacing(0., 0., 0.);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template< class TContainer >
MeshToContourFilter< TContainer >::~MeshToContourFilter()
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template< class TContainer >
void
MeshToContourFilter< TContainer >::
SetInput(vtkPolyData* iInput)
{
  m_Input = iInput;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template< class TContainer >
void
MeshToContourFilter< TContainer >::
SetSpacing(const double& iX, const double & iY, const double& iZ)
{
  m_Spacing[0] = iX;
  m_Spacing[1] = iY;
  m_Spacing[2] = iZ;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template< class TContainer >
std::map<double, vtkPolyData *>
MeshToContourFilter< TContainer >::
ExtractPolyData(ORIENTATION iOrientation)
{
  std::map<double, vtkPolyData*> contours;

  // create plane to extract contours (based on orientation)
  double normal[3] = {0., 0., 0.};

  switch (iOrientation)
    {
    case XY:
      {
      normal[2] = 1;
      break;
      }
    case XZ:
      {
      normal[1] = 1;
      break;
      }
    case YZ:
      {
      normal[0] = 1;
      break;
      }
    default:
      {
      break;
      }
    }

  double origin[3] = {0., 0., 0.};
  origin[0] = m_Input->GetCenter()[0];
  origin[1] = m_Input->GetCenter()[1];
  origin[2] = m_Input->GetCenter()[2];

  double position = m_Input->GetBounds()[4 - 2*iOrientation];
  double maxPosition = m_Input->GetBounds()[5 - 2*iOrientation];
  double spacing = this->m_Spacing[2-iOrientation];

  while( position < maxPosition)
    {
    origin[2-iOrientation] = position;

    vtkPlane* plane = vtkPlane::New();
    plane->SetNormal(normal);
    plane->SetOrigin(origin);

    // cut
    vtkCutter* cutter = vtkCutter::New();
    cutter->SetInput(m_Input);
    cutter->SetCutFunction(plane);
    cutter->Update();

    vtkPolyData* polydata = vtkPolyData::New();
    polydata->DeepCopy( cutter->GetOutput() );

    plane->Delete();
    cutter->Delete();

    contours[position] = polydata;

    // increase position
    position += spacing;
    }

  return contours;
}
//-----------------------------------------------------------------------------
#endif
