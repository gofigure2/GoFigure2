/*========================================================================
 Copyright (c) INRIA - ASCLEPIOS Project (http://www-sop.inria.fr/asclepios).
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 * Neither the name of INRIA or ASCLEPIOS, nor the names of any contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 * Modified source versions must be plainly marked as such, and must not be
 misrepresented as being the original software.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 =========================================================================*/

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

#ifndef __vtkFillImageWithPolyData_h
#define __vtkFillImageWithPolyData_h

#include "vtkThreadedImageAlgorithm.h"
#include <vtkPolyData.h>
#include "MegaVTK2Configure.h"

/**
 * \class vtkFillImageWithPolyData
 * \brief TO BE FILLED
 * \ingroup MegaVTK
 */
class VTK_RENDERINGADDON2_EXPORT vtkFillImageWithPolyData:
  public vtkThreadedImageAlgorithm
{
public:
    /**
     * \brief Convenient method to access the constructor
     */
  static vtkFillImageWithPolyData * New();

  vtkTypeRevisionMacro (vtkFillImageWithPolyData, vtkThreadedImageAlgorithm);
  void PrintSelf(ostream & os, vtkIndent indent);

  /**
   * \brief Set the Polydata
   */
  vtkSetObjectMacro (PolyData, vtkPolyData);

  /**
   * \brief Get the Polydata
   */
  vtkGetObjectMacro (PolyData, vtkPolyData);

  /**
   * \brief Set the value of the pixel inside
   */

  vtkSetMacro (InsidePixelValue, double);

  /**
   * \brief Get the value of the pixel inside
   */
  vtkGetMacro (InsidePixelValue, double);

  /**
   * \brief Set the direction of extraction.
   * 0: X, 1: Y, 2: Z
   */
  vtkSetMacro (ExtractionDirection, int);

  /**
   * \brief Set the direction of extraction.
   * 0: X, 1: Y, 2: Z
   */
  vtkGetMacro (ExtractionDirection, int);

  /**
   * \brief Compute the angle between the 2 input points.
   * \param[in] dp1 Position of the first point.
   * \param[in] dp2 Position of the second point.
   * \return The angle between the 2 input points.
   */
  double Angle2D(const double dp1[2], const double dp2[2]);

protected:
  /**
   * \fn vtkFillImageWithPolyData::vtkFillImageWithPolyData()
   * \brief Constructor
   */
  vtkFillImageWithPolyData();
  /**
   * \brief Destructor
   */
  ~vtkFillImageWithPolyData();

  virtual int RequestInformation (vtkInformation *vtkNotUsed(request),
                                  vtkInformationVector **inputVector,
                                  vtkInformationVector *outputVector);

  virtual void ThreadedRequestData(vtkInformation *vtkNotUsed(request),
                                   vtkInformationVector **vtkNotUsed(inputVector),
                                   vtkInformationVector *vtkNotUsed(outputVector),
                                   vtkImageData ***inData,
                                   vtkImageData **outData,
                                   int extent[6], int threadId);
private:
  /**
   * \overload vtkFillImageWithPolyData::vtkFillImageWithPolyData(const vtkFillImageWithPolyData &)
   * \brief Constructor
   */
  vtkFillImageWithPolyData (const vtkFillImageWithPolyData &);
  void operator=(const vtkFillImageWithPolyData &);

  vtkPolyData *PolyData;
  double       InsidePixelValue;
  int          ExtractionDirection;
  double       BBox[6];
};

#endif
