/*=========================================================================
  Author: $Author: nicolasrannou $  // Author of last commit
  Version: $Rev: 2037 $  // Revision of last commit
  Date: $Date: 2010-08-23 16:33:20 -0400 (Mon, 23 Aug 2010) $  // Date of last commit
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
#include "QGoFilterShape.h"

#include "QGoGUILibConfigure.h"

#include "QGoContourSemiAutoShapeWidget.h"

// Shapes to be generated
#include "vtkPolyData.h"
#include "vtkPointData.h"
 #include "vtkPlane.h"
 #include "vtkCutter.h"
//Sphere
#include "vtkSphereSource.h"
//Cube
#include "vtkCubeSource.h"
#include "vtkTriangleFilter.h"
//cylinder
#include "vtkCylinderSource.h"
//#include "vtkTriangleFilter.h"

//--------------------------------------------------------------------------
QGoFilterShape::
QGoFilterShape(int iDimension)
{
  m_Dimension = iDimension;

  QString name = "Shape ";
  name.append(QString::number(m_Dimension, 10));
  name.append("D");

  setName(name);
  QGoContourSemiAutoShapeWidget* widget =
      new QGoContourSemiAutoShapeWidget(NULL, m_Dimension);
  setWidget(widget);

  m_Shape = 0;
  m_Sampling = 0;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoFilterShape::
~QGoFilterShape()
{

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyData*
QGoFilterShape::
Apply()
{
  // Radius has to be > 0
  if(getRadius() <= 0)
    {
    std::cerr << "Radius should be > 0 " << std::endl;
    return NULL;
    }

  double* center2 = new double[3];

  // update the pointed value of the seeds
  emit UpdateSeeds();

  // LOOP  FOR EACH SEED
  for (int i = 0; i < getPoints()->GetNumberOfPoints(); i++)
    {
    getPoints()->GetPoint(i, center2);

    // useful to translate the polydata afterwards
    setCenter(center2);

    // need a switch depending on shape
    vtkPolyData* testing;

    switch(m_Shape)
    {
      case 0:
        testing = GenerateSphere(getCenter());
        break;
      case 1:
        testing = GenerateCube(getCenter());
        break;
      case 2:
        testing = GenerateCylinder(getCenter());
        break;
      default:
        break;
    }

    if(m_Dimension == 2)
      {
      // Radius has to be > 0
      if(m_Sampling <= 0)
        {
        std::cerr << "Sampling should be > 0 " << std::endl;
        delete[] center2;
        return NULL;
        }

      // Extract each slice according top the sampling
      vtkPlane* implicitFunction = vtkPlane::New();
      implicitFunction->SetNormal(0, 0, 1);

      vtkCutter* cutter = vtkCutter::New();
      cutter->SetInput(testing);
      cutter->SetCutFunction(implicitFunction);

      for(int i=0; i<m_Sampling; ++i)
        {
      // radius
      ///TODO should use the spacing to make sure the contour in on a slice
        implicitFunction
      ->SetOrigin((center2[0]-getRadius()+(i+1)*2*getRadius()/(m_Sampling+1)),
                  (center2[1]-getRadius()+(i+1)*2*getRadius()/(m_Sampling+1)),
                  (center2[2]-getRadius()+(i+1)*2*getRadius()/(m_Sampling+1)));
        cutter->Update();
        emit ContourCreated(cutter->GetOutput());
        }
      }
    else // if dimension == 3, create a mesh
      {
      emit MeshCreated(testing);
      }
    }
  emit SegmentationFinished();

  delete[] center2;

  return NULL;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterShape::
ConnectSignals(int iFilterNumber)
{
  QGoFilterSemiAutoBase::ConnectSignals(iFilterNumber);

  // connect specific
  QObject::connect(getWidget(), SIGNAL(Shape(int)),
      this, SLOT(setShape(int)));
  QObject::connect(getWidget(), SIGNAL(Sampling(int)),
      this, SLOT(setSampling(int)));
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterShape::
setShape(int iShape)
{
  m_Shape = iShape;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterShape::
setSampling(int iSampling)
{
  m_Sampling = iSampling;
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
vtkPolyData*
QGoFilterShape::
GenerateSphere(double* iCenter)
{
  // create sphere geometry
  vtkSphereSource* sphere = vtkSphereSource::New();
  sphere->SetRadius(getRadius());
  sphere->SetThetaResolution(18);
  sphere->SetPhiResolution(18);
  sphere->SetCenter(iCenter);
  sphere->Update();
  sphere->GetOutput()->GetPointData()->SetNormals(NULL);

  return sphere->GetOutput();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyData*
QGoFilterShape::
GenerateCube(double* iCenter)
{
  // create cube geometry
  vtkCubeSource* cube = vtkCubeSource::New();
  cube->SetXLength(getRadius());
  cube->SetYLength(getRadius());
  cube->SetZLength(getRadius());
  cube->SetCenter(iCenter);
  cube->Update();
  cube->GetOutput()->GetPointData()->SetNormals(NULL);

  vtkTriangleFilter* triangle = vtkTriangleFilter::New();
  triangle->SetInput(cube->GetOutput());
  triangle->Update();

  cube->Delete();
  return triangle->GetOutput();
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
vtkPolyData*
QGoFilterShape::
GenerateCylinder(double* iCenter)
{
  // create cube geometry
  vtkCylinderSource* cylinder = vtkCylinderSource::New();
  cylinder->SetHeight(2*getRadius());
  cylinder->SetRadius(getRadius());
  cylinder->SetCenter(iCenter);
  cylinder->SetResolution(10);
  cylinder->Update();
  cylinder->GetOutput()->GetPointData()->SetNormals(NULL);

  vtkTriangleFilter* triangle = vtkTriangleFilter::New();
  triangle->SetInput(cylinder->GetOutput());

  return triangle->GetOutput();
}
//--------------------------------------------------------------------------
