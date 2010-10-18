/*=========================================================================
 Modifications were made by the GoFigure Dev. Team.
 while at Megason Lab, Systems biology, Harvard Medical school, 2009-10

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

/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSurfaceReconstructionFilter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME vtkNormalEstimationFilter - estimate normals from unorganized points
// .SECTION Description
// vtkNormalEstimationFilter takes a list of points assumed to lie on
// the surface of a solid 3D object. This procedure is based on the PhD
// work of Hugues Hoppe: http://www.research.microsoft.com/~hoppe

#ifndef __vtkNormalEstimationFilter_h
#define __vtkNormalEstimationFilter_h

#include "vtkPolyDataAlgorithm.h"
#include "PoissonReconstructionConfigure.h"

struct SurfacePoint;

class PoissonReconstruction_EXPORT vtkNormalEstimationFilter:public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkNormalEstimationFilter, vtkPolyDataAlgorithm);
  void PrintSelf(ostream & os, vtkIndent indent);

  // Description:
  // Construct with NeighborhoodSize=20.
  static vtkNormalEstimationFilter * New();

  // Description:
  vtkGetMacro(RadiusRatio, double);
  vtkSetMacro(RadiusRatio, double);
protected:
  vtkNormalEstimationFilter();
  ~vtkNormalEstimationFilter();

  SurfacePoint *m_SurfacePoints;

  virtual int RequestInformation(vtkInformation *,
                                 vtkInformationVector **,
                                 vtkInformationVector *);

  virtual int RequestData(vtkInformation *,
                          vtkInformationVector **,
                          vtkInformationVector *);

  double RadiusRatio;

  virtual int FillInputPortInformation(int, vtkInformation *);

  void BuildLocalConnectivity(vtkDataSet *input);

  void EstimatePlanes(vtkDataSet *input);

  void ComputeCostForMST(vtkDataSet *input);

  int ConsistencyPropagation();

private:
  vtkNormalEstimationFilter(const vtkNormalEstimationFilter &); // Not
                                                                // implemented.
  void operator=(const vtkNormalEstimationFilter &);            // Not
                                                                // implemented.
};

#endif
