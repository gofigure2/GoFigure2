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

#include <QApplication>

#include "vtkSmartPointer.h"

#include "vtkMetaImageReader.h"
#include "itkImage.h"

// helper for debugging
#include "VisualizePolydataHelper.h"
#include "vtkPolyDataWriter.h"
#include "vtkContourWidget.h"
#include "vtkOrientedGlyphContourRepresentation.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

//converter to be tested
#include "QGoMeshLevelSetAlgo.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  if ( argc != 3 )
    {
    std::cout << "Usage : QGoSynchronizedView2DTest(.exe) " << std::endl;
    std::cout << "1-file.mhd" << std::endl;
    std::cout << "2-test (boolean)" << std::endl;
    return EXIT_FAILURE;
    }

  vtkSmartPointer< vtkMetaImageReader > reader =
      vtkSmartPointer< vtkMetaImageReader >::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  std::cout << "Data Range: " << reader->GetOutput()->GetScalarRange()[0]
                    << " to " << reader->GetOutput()->GetScalarRange()[1]
                    << std::endl;
  std::cout << "Data Dimensions: " << reader->GetOutput()->GetDimensions()[0]
                          << " x " << reader->GetOutput()->GetDimensions()[1]
                          << " x " << reader->GetOutput()->GetDimensions()[2]
                          <<std::endl;

  QGoMeshLevelSetAlgo algo(NULL, NULL);

  // itk definitions
  const int dimension = 3;
  typedef float                                PixelType;
  typedef itk::Image< PixelType, dimension >   ImageType;
  typedef ImageType::Pointer                   ImageTypePointer;


  //------------------------------------------------------------------
  // 3d
  //------------------------------------------------------------------
  // extract roi
  double voi3D[6] = {10, 20, 10, 20, 10, 20};
  vtkImageData* roi3D = algo.ExtractROI( voi3D, reader->GetOutput() );

  assert( roi3D->GetDataDimension() == 3);

  // convert vtk to itk
  ImageTypePointer
    itkImage3D = algo.ConvertVTK2ITK< PixelType, dimension >(roi3D);

  // convert itk to vtk
  vtkImageData *vtkImage3D = algo.ConvertITK2VTK< PixelType, dimension >(itkImage3D);
  vtkImage3D->Update();

  std::cout << "Dimension 3D: " << vtkImage3D->GetDataDimension() <<std::endl;
  std::cout << "Data Dimensions: " << vtkImage3D->GetDimensions()[0]
                          << " x " << vtkImage3D->GetDimensions()[1]
                          << " x " << vtkImage3D->GetDimensions()[2]
                          <<std::endl;
  assert( vtkImage3D->GetDataDimension() == 3);

  // Reconstruct polydata
  vtkPolyData* poly3D = algo.ExtractPolyData( vtkImage3D, 100 );

  //ShowPolyData(poly3D);

  vtkImage3D->Delete();
  poly3D->Delete();
  roi3D->Delete();

  //------------------------------------------------------------------
  // 3d - too large ROI
  //------------------------------------------------------------------
  // extract roi
  double voi3D2Large[6] = {0, 30, 0, 50, 0, 50};
  vtkImageData* roi3D2Large = algo.ExtractROI( voi3D2Large, reader->GetOutput() );

  assert( roi3D2Large->GetDataDimension() == 3);

  // convert vtk to itk
  ImageTypePointer
    itkImage3D2Large = algo.ConvertVTK2ITK< PixelType, dimension >(roi3D2Large);

  // convert itk to vtk
  vtkImageData *vtkImage3D2Large = algo.ConvertITK2VTK< PixelType, dimension >(itkImage3D2Large);
  vtkImage3D2Large->Update();

  std::cout << "Dimension 3D Too Large: " << vtkImage3D2Large->GetDataDimension() <<std::endl;
  std::cout << "Data Dimensions: " << vtkImage3D2Large->GetDimensions()[0]
                          << " x " << vtkImage3D2Large->GetDimensions()[1]
                          << " x " << vtkImage3D2Large->GetDimensions()[2]
                          <<std::endl;

  assert( vtkImage3D2Large->GetDataDimension() == 3);

  // Reconstruct polydata
  vtkPolyData* poly3D2Large = algo.ExtractPolyData( vtkImage3D2Large, 100 );
  ShowPolyData(poly3D2Large);

  // Decimate polydata
  vtkPolyData* decimate3D = algo.DecimatePolyData(poly3D2Large, 100);
  ShowPolyData(decimate3D);

  decimate3D->Delete();
  vtkImage3D2Large->Delete();
  poly3D2Large->Delete();
  roi3D2Large->Delete();

  //------------------------------------------------------------------
  // 2d
  //------------------------------------------------------------------
  // extract roi
  double voi2D[6] = {15, 15, 0, 30, 0, 30};
  vtkImageData* roi2D = algo.ExtractROI( voi2D, reader->GetOutput() );

  assert( roi2D->GetDataDimension() == 2);

  // convert vtk to itk
  ImageTypePointer
    itkImage2D = algo.ConvertVTK2ITK< PixelType, dimension >(roi2D);

  // convert itk to vtk
  vtkImageData *vtkImage2D = algo.ConvertITK2VTK< PixelType, dimension >(itkImage2D);
  vtkImage2D->Update();

  std::cout << "Dimension 2D: " << vtkImage2D->GetDataDimension() <<std::endl;
  std::cout << "Data Dimensions: " << vtkImage2D->GetDimensions()[0]
                          << " x " << vtkImage2D->GetDimensions()[1]
                          << " x " << vtkImage2D->GetDimensions()[2]
                          <<std::endl;

  assert( vtkImage2D->GetDataDimension() == 2);

  // Reconstruct polydata
  vtkPolyData* poly2D = algo.ExtractPolyData( vtkImage2D, 100 );

  vtkPolyData* decimate2D = algo.DecimatePolyData(poly2D, 10);

  //ShowPolyData(poly2D);

  vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
  writer->SetFileName("contour.vtk");
  writer->SetInput(poly2D);
  writer->Write();
  writer->Delete();

  vtkSmartPointer< vtkPolyDataMapper > mapper =
    vtkSmartPointer< vtkPolyDataMapper >::New();
  mapper->SetInput(decimate2D);

  vtkSmartPointer< vtkActor > actor =
    vtkSmartPointer< vtkActor >::New();
  actor->SetMapper(mapper);

  vtkSmartPointer< vtkRenderer > renderer =
    vtkSmartPointer< vtkRenderer >::New();
  renderer->AddActor(actor);

  vtkSmartPointer< vtkRenderWindow > renderWindow =
    vtkSmartPointer< vtkRenderWindow >::New();
  renderWindow->AddRenderer(renderer);

  vtkSmartPointer< vtkRenderWindowInteractor > renderWindowInteractor =
    vtkSmartPointer< vtkRenderWindowInteractor >::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindowInteractor->Initialize();
  renderWindow->Render();

  vtkSmartPointer<vtkOrientedGlyphContourRepresentation> contourRep =
         vtkSmartPointer<vtkOrientedGlyphContourRepresentation>::New();
       vtkSmartPointer<vtkContourWidget> contourWidget =
         vtkSmartPointer<vtkContourWidget>::New();
       contourWidget->SetInteractor(renderWindowInteractor);
       contourWidget->SetRepresentation(contourRep);
       contourWidget->On();
  contourWidget->Initialize(decimate2D);
  contourWidget->Render();

  renderWindowInteractor->Start();

  decimate2D->Delete();
  vtkImage2D->Delete();
  poly2D->Delete();
  roi2D->Delete();

  return EXIT_SUCCESS;
}
