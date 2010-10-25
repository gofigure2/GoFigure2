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
QGoFilterShape::QGoFilterShape(QObject *iParent, int iDimension):
  QGoFilterSemiAutoBase(iParent)
{
  m_Dimension = iDimension;

  QString name = "Shape ";
  if ( m_Dimension < 2 )
    {
    name.append( QString::number(m_Dimension + 2, 10) );
    name.append("D");
    }
  else
    {
    name = "2D Shapes in 1 mesh";
    }

  setName(name);
  QGoContourSemiAutoShapeWidget *widget =
    new QGoContourSemiAutoShapeWidget(NULL);
  setWidget(widget);

  m_Shape = 0;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoFilterShape::
~QGoFilterShape()
{}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyData *
QGoFilterShape::Apply()
{
  // Radius has to be > 0
  if ( getRadius() <= 0 )
    {
    std::cerr << "Radius should be > 0 " << std::endl;
    return NULL;
    }

  double *center2 = new double[3];

  // update the pointed value of the seeds
  emit UpdateSeeds();

  // LOOP  FOR EACH SEED
  for ( int i = 0; i < getPoints()->GetNumberOfPoints(); i++ )
    {
    getPoints()->GetPoint(i, center2);

    // useful to translate the polydata afterwards
    setCenter(center2);

    // need a switch depending on shape
    vtkPolyData *testing = NULL;

    switch ( m_Shape )
      {
      case 0:
        testing = GenerateSphere( getCenter() );
        break;
      case 1:
        testing = GenerateCube( getCenter() );
        break;
      //case 2:
      //  testing = GenerateCylinder(getCenter());
      //  break;
      default:
        break;
      }

    if ( m_Dimension == 0 )
      {
      // Extract each slice according top the sampling
      vtkPlane *implicitFunction = vtkPlane::New();
      implicitFunction->SetNormal(0, 0, 1);
      implicitFunction->SetOrigin(center2[0], center2[1], center2[2]);

      vtkCutter *cutter = vtkCutter::New();
      cutter->SetInput(testing);
      cutter->SetCutFunction(implicitFunction);
      cutter->Update();

      vtkPolyData *output = ReorganizeContour( cutter->GetOutput() );
      emit         ContourCreated(output);

      implicitFunction->Delete();
      cutter->Delete();
      testing->Delete();
      }
    else // if dimension == 1, create a mesh
      {
      if ( m_Dimension == 1 )
        {
        emit MeshCreated(testing);
        }
      else
        {
        //emit CreateEmptyMesh();
        // Extract each slice according top the sampling
        vtkPlane *implicitFunction = vtkPlane::New();
        implicitFunction->SetNormal(0, 0, 1);

        vtkCutter *cutter = vtkCutter::New();
        cutter->SetInput(testing);
        cutter->SetCutFunction(implicitFunction);

        double step = 2. * getRadius() / static_cast< double >( getSampling() + 1 );

        for ( int j = 0; j < getSampling(); ++j )
          {
          implicitFunction
          ->SetOrigin(
            ( center2[0] - getRadius() + static_cast< double >( j + 1 ) * step ),
            ( center2[1] - getRadius() + static_cast< double >( j + 1 ) * step ),
            ( center2[2] - getRadius() + static_cast< double >( j + 1 ) * step ) );
          cutter->Update();
          vtkPolyData *output = ReorganizeContour( cutter->GetOutput() );
          emit         AddContourForMeshToContours(output);
          }

        implicitFunction->Delete();
        cutter->Delete();
        testing->Delete();
        emit CreateCorrespondingMesh( getSampling() );
        }
      }
    }
  emit SegmentationFinished();

  delete[] center2;

  return NULL;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterShape::ConnectSignals(int iFilterNumber)
{
  QGoFilterSemiAutoBase::ConnectSignals(iFilterNumber);

  // connect specific
  QObject::connect( getWidget(), SIGNAL( Shape(int) ),
                    this, SLOT( setShape(int) ) );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoFilterShape::setShape(int iShape)
{
  m_Shape = iShape;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyData *
QGoFilterShape::GenerateSphere(double *iCenter)
{
  // create sphere geometry
  vtkSphereSource *sphere = vtkSphereSource::New();

  sphere->SetRadius( getRadius() );
  sphere->SetThetaResolution(18);
  sphere->SetPhiResolution(18);
  sphere->SetCenter(iCenter);
  sphere->Update();
  sphere->GetOutput()->GetPointData()->SetNormals(NULL);

  vtkPolyData *output = vtkPolyData::New();
  output->DeepCopy( sphere->GetOutput() );

  sphere->Delete();

  return output;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyData *
QGoFilterShape::GenerateCube(double *iCenter)
{
  // create cube geometry
  vtkCubeSource *cube = vtkCubeSource::New();

  cube->SetXLength( getRadius() );
  cube->SetYLength( getRadius() );
  cube->SetZLength( getRadius() );
  cube->SetCenter(iCenter);
  cube->Update();
  cube->GetOutput()->GetPointData()->SetNormals(NULL);

  vtkTriangleFilter *triangle = vtkTriangleFilter::New();
  triangle->SetInput( cube->GetOutput() );
  triangle->Update();

  vtkPolyData *output = vtkPolyData::New();
  output->DeepCopy( triangle->GetOutput() );

  triangle->Delete();
  cube->Delete();

  return output;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyData *
QGoFilterShape::GenerateCylinder(double *iCenter)
{
  // create cube geometry
  vtkCylinderSource *cylinder = vtkCylinderSource::New();

  cylinder->SetHeight( 2 * getRadius() );
  cylinder->SetRadius( getRadius() );
  cylinder->SetCenter(iCenter);
  cylinder->SetResolution(10);
  cylinder->Update();
  cylinder->GetOutput()->GetPointData()->SetNormals(NULL);

  vtkTriangleFilter *triangle = vtkTriangleFilter::New();
  triangle->SetInput( cylinder->GetOutput() );

  vtkPolyData *output = vtkPolyData::New();
  output->DeepCopy( triangle->GetOutput() );

  triangle->Delete();
  cylinder->Delete();

  return output;
}

//--------------------------------------------------------------------------
