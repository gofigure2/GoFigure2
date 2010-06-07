/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
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
#ifndef __QGoSeedsSegmentation_h
#define __QGoSeedsSegmentation_h

#include "QGoGUILibConfigure.h"

#include "vtkImageData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkViewImage2D.h"

#include "vtkSmartPointer.h"

/**
\class QGoSeedsSegmentation
\brief Contains all segmentations from seed points
\example GUI/lib/qgotabimageview2d.cxx
*/
class QGOGUILIB_EXPORT QGoSeedsSegmentation
{
public:
  /**
   * \brief Constructor
   */
  QGoSeedsSegmentation();

  /**
   * \brief Destructor
   */
  ~QGoSeedsSegmentation();

  /**
   * \brief Set the input Volume to be used for levelset segmentation
   */
  void setInputVolume( vtkImageData* iInputVolume);
  //vtkImageData* inputVolume( );

  /**
   * \brief Set the Origin points for all segmentations
   */
  void setSeedsPosition( double iSeedsPosition[3]);

  /**
   * \brief Set the radius for all segmentations
   */
  void setRadius( double iRadius );

  /**
   * \brief Set the number of iterations for levelset segmentation
   */
  void setNumberOfIterations( int iNumberOfIterations );

  /**
   * \brief Set the curvature weight for levelset segmentation
   */
  void setCurvatureWeight( int iCurvatureWeight );

  /**
   * \brief Easy way to access informations about input image
   */
  void setOriginImageInformation( vtkViewImage2D* iOriginImageInformation );

  /**
   * \brief Get the result of the segmentation in a polydata
   */
  vtkSmartPointer<vtkPolyData> output();

  /**
   * \brief 2D levelset segmentation in the choosen direction
   */
  vtkSmartPointer<vtkPolyData> LevelSetSegmentation2D(int iDirection);

  /**
   * \brief 3D levelset segmentation
   */
  vtkSmartPointer<vtkPolyData> LevelSetSegmentation3D();

  /**
   * \brief Sphere contour segmentation
   */
  std::vector< vtkSmartPointer<vtkPolyData> > SphereContoursSegmentation();

  /**
   * \brief Useful fot the sphere contour segmentation
   */
  vtkSmartPointer<vtkPolyData> GenerateCircleFromGivenSphereAndGivenZ(
    double iC[3], const double& iRadius, double iZ, const int& iN );

  /**
  * \brief Sphere volume segmentation
  */
  vtkSmartPointer<vtkPolyData>  SphereVolumeSegmentation();

private:

  /**
  * \brief Volume to be used for levelset segmentation
  */
  vtkSmartPointer<vtkImageData> m_OriginImage;

  /**
   * \brief Origin points for all segmentations
   */
  double    m_SeedsPosition[3];

  /**
   * \brief Output polydata for each segmentation
   */
  vtkSmartPointer<vtkPolyData>  m_OutputPolyData;

  /**
   *
   */
  double m_Radius;

  /**
   *
   */
  int m_NumberOfIterations;

  /**
   *
   */
  int m_CurvatureWeight;

  /**
   *
   */

  vtkSmartPointer<vtkViewImage2D> m_OriginImageInformations;
};
#endif

