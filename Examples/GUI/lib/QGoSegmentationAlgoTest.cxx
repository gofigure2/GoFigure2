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
  std::vector< double > voi3D(6);
  voi3D[0] = 10;
  voi3D[1] = 20;
  voi3D[2] = 10;
  voi3D[3] = 20;
  voi3D[4] = 10;
  voi3D[5] = 20;

  double voi3D[6] = {10, 20, 10, 20, 10, 20};
  vtkSmartPointer<vtkImageData> roi3D = vtkSmartPointer<vtkImageData>::New();
  roi3D->ShallowCopy( algo.ExtractROI( voi3D, reader->GetOutput() ));

  assert( roi3D->GetDataDimension() == 3);

  // convert vtk to itk
  ImageTypePointer
    itkImage3D = algo.ConvertVTK2ITK< PixelType, dimension >(roi3D);

  // convert itk to vtk
  vtkSmartPointer<vtkImageData> vtkImage3D = vtkSmartPointer<vtkImageData>::New();
  vtkImage3D->ShallowCopy(algo.ConvertITK2VTK< PixelType, dimension >(itkImage3D));
  vtkImage3D->Update();

  std::cout << "Dimension 3D: " << vtkImage3D->GetDataDimension() <<std::endl;
  std::cout << "Data Dimensions: " << vtkImage3D->GetDimensions()[0]
                          << " x " << vtkImage3D->GetDimensions()[1]
                          << " x " << vtkImage3D->GetDimensions()[2]
                          <<std::endl;
  assert( vtkImage3D->GetDataDimension() == 3);

  // Reconstruct polydata
  vtkSmartPointer<vtkPolyData> poly3D = vtkSmartPointer<vtkPolyData>::New();
  poly3D->ShallowCopy(algo.ExtractPolyData( vtkImage3D, 100 ));
  ShowPolyData(poly3D);

  //------------------------------------------------------------------
  // 3d - too large ROI
  //------------------------------------------------------------------
  // extract roi
  std::vector< double > voi3D2Large(6);
  voi3D2Large[0] = 0;
  voi3D2Large[1] = 30;
  voi3D2Large[2] = 0;
  voi3D2Large[3] = 50;
  voi3D2Large[4] = 0;
  voi3D2Large[5] = 50;

  vtkSmartPointer<vtkImageData> roi3D2Large = vtkSmartPointer<vtkImageData>::New();
  roi3D2Large->ShallowCopy(algo.ExtractROI( voi3D2Large, reader->GetOutput() ));

  assert( roi3D2Large->GetDataDimension() == 3);

  // convert vtk to itk
  ImageTypePointer
    itkImage3D2Large = algo.ConvertVTK2ITK< PixelType, dimension >(roi3D2Large);

  // convert itk to vtk
  vtkSmartPointer<vtkImageData> vtkImage3D2Large = vtkSmartPointer<vtkImageData>::New();
  vtkImage3D2Large->ShallowCopy(algo.ConvertITK2VTK< PixelType, dimension >(itkImage3D2Large));
  vtkImage3D2Large->Update();

  std::cout << "Dimension 3D Too Large: " << vtkImage3D2Large->GetDataDimension() <<std::endl;
  std::cout << "Data Dimensions: " << vtkImage3D2Large->GetDimensions()[0]
                          << " x " << vtkImage3D2Large->GetDimensions()[1]
                          << " x " << vtkImage3D2Large->GetDimensions()[2]
                          <<std::endl;

  assert( vtkImage3D2Large->GetDataDimension() == 3);

  // Reconstruct polydata
  vtkSmartPointer<vtkPolyData> poly3D2Large = vtkSmartPointer<vtkPolyData>::New();
  poly3D2Large->ShallowCopy(algo.ExtractPolyData( vtkImage3D2Large, 100 ));
  ShowPolyData(poly3D2Large);

  // Decimate polydata
  vtkSmartPointer<vtkPolyData> decimate3D = vtkSmartPointer<vtkPolyData>::New();
  decimate3D->ShallowCopy(algo.DecimatePolyData(poly3D2Large, 200));
  ShowPolyData(decimate3D);

  //------------------------------------------------------------------
  // 2d
  //------------------------------------------------------------------
  // extract roi
  std::vector< double > voi2D(6);
  voi2D[0] = 15;
  voi2D[1] = 15;
  voi2D[2] = 0;
  voi2D[3] = 30;
  voi2D[4] = 0;
  voi2D[5] = 30;

  double voi2D[6] = {15, 15, 0, 30, 0, 30};
  vtkSmartPointer<vtkImageData> roi2D = vtkSmartPointer<vtkImageData>::New();
  roi2D->ShallowCopy(algo.ExtractROI( voi2D, reader->GetOutput() ));

  assert( roi2D->GetDataDimension() == 2);

  // convert vtk to itk
  ImageTypePointer
    itkImage2D = algo.ConvertVTK2ITK< PixelType, dimension >(roi2D);

  // convert itk to vtk
  vtkSmartPointer<vtkImageData> vtkImage2D = vtkSmartPointer<vtkImageData>::New();
  vtkImage2D->ShallowCopy(algo.ConvertITK2VTK< PixelType, dimension >(itkImage2D));
  vtkImage2D->Update();

  std::cout << "Dimension 2D: " << vtkImage2D->GetDataDimension() <<std::endl;
  std::cout << "Data Dimensions: " << vtkImage2D->GetDimensions()[0]
                          << " x " << vtkImage2D->GetDimensions()[1]
                          << " x " << vtkImage2D->GetDimensions()[2]
                          <<std::endl;

  assert( vtkImage2D->GetDataDimension() == 2);

  // Reconstruct polydata
  vtkSmartPointer<vtkPolyData> poly2D = vtkSmartPointer<vtkPolyData>::New();
  poly2D->ShallowCopy(algo.ExtractPolyData( vtkImage2D, 100 ));
  ShowPolyData(poly2D);

  vtkSmartPointer<vtkPolyData> decimate2D = vtkSmartPointer<vtkPolyData>::New();
  decimate2D->ShallowCopy(algo.DecimatePolyData(poly2D, 10));

  ShowPolyData(decimate2D);

  return EXIT_SUCCESS;
}
