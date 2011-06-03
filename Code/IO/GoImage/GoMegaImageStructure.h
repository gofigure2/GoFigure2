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

#ifndef GoMegaImageStructure_H
#define GoMegaImageStructure_H

// Required for dynamic libs on Windows (QGoIOExport)
#include "QGoIOConfigure.h"

// includes from external libs
// VTK
#include "vtkSmartPointer.h"
#include "vtkLookupTable.h"
#include "vtkImageData.h"
#include "vtkPiecewiseFunction.h"
#include "vtkImageAccumulate.h"

// convert VTK to ITK
#include "itkImage.h"
#include "itkVTKImageImport.h"
#include "vtkImageExport.h"
#include "vtkitkAdaptor.h"

/**
\defgroup Mega Mega
*/

/**
 * \struct GoMegaImageStructure
 * \brief  Convenience structure to store visible image
 * \ingroup Mega
 */
struct QGOIO_EXPORT GoMegaImageStructure
{
    unsigned int                                       Index;
    vtkSmartPointer<vtkLookupTable>                    LUT;
    vtkSmartPointer<vtkImageData>                      Image;
    std::vector< double >                              Color;
    bool                                               Visibility;
    std::string                                        Name;
    std::vector<std::map<unsigned int, unsigned int> > RGBA;
    vtkSmartPointer<vtkPiecewiseFunction>              OpacityTF;
    vtkSmartPointer<vtkImageAccumulate>                Histogram;

    /** Constructor */
    GoMegaImageStructure(unsigned int iIndex,
                       vtkSmartPointer<vtkLookupTable> iLUT,
                       vtkSmartPointer<vtkImageData> iImage,
                       std::vector< double > iColor,
                       bool iVisibility,
                       std::string iName):
      Index(iIndex), LUT(iLUT), Color(iColor), Visibility(iVisibility),
      Name(iName)
    {
    Image = iImage;

    // init RGBA vectors
    // vector might not be the best....
    RGBA.resize(4);
    for(int i; i<4; ++i)
      {
      RGBA[i][0] = 0;
      RGBA[i][255] = Color[i];
      }

    //temp
    OpacityTF = vtkSmartPointer<vtkPiecewiseFunction>::New();
    OpacityTF->AddPoint(0, 0);
    OpacityTF->AddPoint(255, Color[3]/255);

    //compute histogram
    double* range;
    range =  Image->GetScalarRange();
    Histogram = vtkSmartPointer<vtkImageAccumulate>::New();
    Histogram->SetInput( Image );
    Histogram->SetComponentExtent(
      0,
      static_cast<int>(range[1])-static_cast<int>(range[0])-1,0,0,0,0 );
    Histogram->SetComponentOrigin( range[0],0,0 );
    Histogram->SetComponentSpacing( 1,0,0 );
    Histogram->SetIgnoreZero( false );
    Histogram->Update();
    }

    // functions to modify the structure through the boost::multiindexcontainer
   void  setLUT(vtkSmartPointer<vtkLookupTable> iLUT)
   {
     LUT = iLUT;
   }

   void  setVisibility(bool iVisibility)
   {
     Visibility = iVisibility;
   }

   void  setName(std::string iName)
   {
     Name = iName;
   }

   void  setImage(vtkImageData* iImage)
   {
     Image = iImage;

     // compute new histogram
     double* range;
     range =  Image->GetScalarRange();

     Histogram->RemoveAllInputs();
     Histogram->SetInput( Image );
     Histogram->SetComponentExtent(
       0,
       static_cast<int>(range[1])-static_cast<int>(range[0])-1,0,0,0,0 );
     Histogram->SetComponentOrigin( range[0],0,0 );
     Histogram->SetComponentSpacing( 1,0,0 );
     Histogram->SetIgnoreZero( false );
     Histogram->Update();
   }

   void setPointsRGBA(std::vector< std::map< unsigned int, unsigned int> > iRGBA)
   {
     RGBA = iRGBA;
   }

   /*
     * \brief Convert a vtkImage to a itkImage. If we call after "ExtractROI",
     * the dimension should be 3 all the time.
     * (Even if we extract a2D region from a 3d image)
     * \tparam PixelType type of pixel (unsigned char, etc.)
     * \tparam VImageDimension dimension of the image (2 or 3)
     * \param[in] iInput Pointer to a vtkImageData
     * \return Pointer to an itkImage
     */
    template< class PixelType, const unsigned int VImageDimension >
    typename itk::Image< PixelType, VImageDimension >::Pointer
    Convert2ITK() const
    {
      //Export VTK image to ITK
      vtkSmartPointer<vtkImageExport> exporter =
          vtkSmartPointer<vtkImageExport>::New();
      exporter->SetInput(Image);
      exporter->Update();

      // ImageType
      typedef itk::Image< PixelType, VImageDimension > ImageType;
      // Import VTK Image to ITK
      typedef itk::VTKImageImport< ImageType >  ImageImportType;
      typedef typename ImageImportType::Pointer ImageImportPointer;
      ImageImportPointer importer = ImageImportType::New();

      ConnectPipelines< vtkImageExport, ImageImportPointer >(
        exporter,
        importer);

      typename ImageType::Pointer itkImage = importer->GetOutput();
      itkImage->DisconnectPipeline();

      return itkImage;
    }

    friend std::ostream& operator<<(std::ostream& os,const GoMegaImageStructure& e)
    {
      os<< "index: "<<e.Index<<std::endl;
      return os;
    }
};

#endif // GoMegaImageStructure_H
