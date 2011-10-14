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
#ifndef __QGoSegmentationAlgo_h
#define __QGoSegmentationAlgo_h

#include "QGoAlgorithmWidget.h"
#include "QGoGUILibConfigure.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "QGoGUILibConfigure.h"

// convert VTK to ITK
#include "itkImage.h"
#include "itkVTKImageImport.h"
#include "vtkImageExport.h"
#include "vtkitkAdaptor.h"

// convert itk to vtk
#include "itkImageToVTKImageFilter.h"

// extract 2d from 2d or 3d from 3d
#include "itkRegionOfInterestImageFilter.h"

// extract 2d from 3d
#include "itkExtractImageFilter.h"

// change info
#include "itkChangeInformationImageFilter.h"

class GoImageProcessor;

/**
\class QGoSegmentationAlgo
\brief abstract class to be the interface between the algorithms for meshes
and contours and GoFigure
*/
class QGOGUILIB_EXPORT QGoSegmentationAlgo:public QObject
{
  Q_OBJECT
public:
  explicit QGoSegmentationAlgo(QWidget *iParent = 0);
  virtual ~QGoSegmentationAlgo();

  /**
  \brief return the algowidget
  */
  QGoAlgorithmWidget* GetAlgoWidget();

  /**
  \brief return the vtkpolydata created by the algorithm
  */
  /*virtual std::vector<vtkPolyData*> ApplyAlgo(
    GoImageProcessor* iImages,
    std::string iChannel,
    bool iIsInvertedOn = false) = 0;*/

  /*
   * \note Nicolas-shouldnt be public-move to protected
   */
  /*
   * \brief Extract region of interest, given a bounding box and a list of vtk images
   * \param[in] iBounds bounding box (xmin, xmax, ymin, ymax, zmin, zmax)
   * \param[in] iImages vector of vtkimagedata
   * \return list of roi
   */
  std::vector<vtkImageData*> VTKExtractROI(
    const std::vector<double>& iBounds,
    const std::vector< vtkSmartPointer< vtkImageData > >& iImages);
  /*
   * \brief Extract region of interest, given a bounding box and a vtk image
   * \param[in] iBounds bounding box (xmin, xmax, ymin, ymax, zmin, zmax)
   * \param[in] iImage vtkimagedata
   * \return roi
   */
  vtkImageData* VTKExtractROI(
    const std::vector<double>& iBounds,
    const vtkSmartPointer< vtkImageData > & iImage);

  /*
   * \brief Convert a vtkImage to a itkImage. If we call after "ExtractROI",
   * the dimension should be 3 all the time.
   * (Even if we extract a2D region from a 3d image)
   * \tparam PixelType type of pixel (unsigned char, etc.)
   * \tparam VImageDimension dimension of the image (2 or 3)
   * \param[in] iInput Pointer to a vtkImageData
   * \return Pointer to an itkImage
   */
  template< class PixelType, unsigned int VImageDimension >
  typename itk::Image< PixelType, VImageDimension >::Pointer
  ConvertVTK2ITK(vtkImageData *iInput)
  {
    // make sure there is an input
    assert ( iInput );

    //Export VTK image to ITK
    vtkSmartPointer<vtkImageExport> exporter =
        vtkSmartPointer<vtkImageExport>::New();
    exporter->SetInput(iInput);
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

  /*
   * \brief Convert a itkImage to a vtkImage. If we call after "ExtractROI",
   * the dimension should be 3 all the time.
   * (Even if we extract a2D region from a 3d image)
   * \tparam PixelType type of pixel (unsigned char, etc.)
   * \tparam VImageDimension dimension of the image (2 or 3)
   * \param[in] iInput Pointer to an itkImage
   * \return Pointer to an vtkImageData
  */
  template< class PixelType, unsigned int VImageDimension >
  vtkImageData*
  ConvertITK2VTK(typename itk::Image< PixelType, VImageDimension >::Pointer iInput)
  {
    typedef itk::Image< PixelType, VImageDimension >        InternalImageType;
    typedef itk::ImageToVTKImageFilter< InternalImageType > ConverterType;
    typedef typename ConverterType::Pointer                 ConverterPointer;

    ConverterPointer converter = ConverterType::New();
    converter->SetInput(iInput);

    try
      {
      converter->Update();
      }
    catch (itk::ExceptionObject & err)
      {
      std::cerr << "converter Exception:" << err << std::endl;
      }

    vtkImageData* output_image = vtkImageData::New();
    output_image->DeepCopy( converter->GetOutput() );

    return output_image;
  }

  //-------------------------------------------------------------------------
  template< class PixelType, unsigned int VImageDimension >
  typename itk::Image< PixelType, VImageDimension >::Pointer
  ITKExtractROI(
    const std::vector< double > & iBounds,
    typename itk::Image< PixelType, VImageDimension >::Pointer iInput )
  {
    typedef itk::Image< PixelType, VImageDimension >  ImageType;
    typedef typename ImageType::PointType             ImagePointType;
    typedef typename ImageType::IndexType             ImageIndexType;
    typedef typename ImageType::IndexValueType        ImageIndexValueType;
    typedef typename ImageType::SizeType              ImageSizeType;
    typedef typename ImageType::SizeValueType         ImageSizeValueType;
    typedef typename ImageType::RegionType            ImageRegionType;
    typedef typename ImageType::SpacingType           ImageSpacingType;

    assert( iBounds.size() == 2 * VImageDimension );

    if ( iInput.IsNull() )
      {
      std::cerr << "iInput is Null" << std::endl;
      }

    ImagePointType t_min, t_max;

    unsigned int k = 0;
    for( unsigned int dim = 0; dim < VImageDimension; dim++ )
      {
      t_min[dim] = iBounds[k++];
      t_max[dim] = iBounds[k++];
      }

    ImageIndexType startOfROI, endOfROI;
    iInput->TransformPhysicalPointToIndex( t_min, startOfROI );
    iInput->TransformPhysicalPointToIndex( t_max, endOfROI );

    ImageSizeType  sizeOfLargeImage =
        iInput->GetLargestPossibleRegion().GetSize();

    ImageSizeType  size;
    size.Fill( 0 );

    for( unsigned int dim = 0; dim < VImageDimension; dim++ )
      {
      if( startOfROI[dim] < 0 )
        {
        startOfROI[dim] = 0;
        }
      if( startOfROI[dim] > sizeOfLargeImage[dim] )
        {
        startOfROI[dim] = sizeOfLargeImage[dim] - 1;
        }

      if( endOfROI[dim] < 0 )
        {
        endOfROI[dim] = 0;
        }
      if( endOfROI[dim] > sizeOfLargeImage[dim] )
        {
        endOfROI[dim] = sizeOfLargeImage[dim] - 1;
        }

      size[dim] = endOfROI[dim] - startOfROI[dim];

      if( size[dim] < 0 )
        {
        size[dim] = 0;
        }
      }

    ImageRegionType region;
    region.SetSize(size);
    region.SetIndex(startOfROI);

    typedef itk::RegionOfInterestImageFilter< ImageType, ImageType >  ROIFilterType;
    typedef typename ROIFilterType::Pointer                           ROIFilterPointer;

    ROIFilterPointer roi = ROIFilterType::New();
    roi->SetInput(iInput);
    roi->SetRegionOfInterest(region);
    try
      {
      roi->Update();
      }
    catch (itk::ExceptionObject & err)
      {
      std::cerr << "roi Exception:" << err << std::endl;
      }
    return roi->GetOutput();
  }

  template< class PixelType>
  typename itk::Image< PixelType, 2>::Pointer
  ITKExtractSlice(
    const std::vector< double > & iBounds,
    typename itk::Image< PixelType, 3 >::Pointer iInput )
  {
    typedef itk::Image< PixelType, 3 > InputImageType;
    typedef itk::Image< PixelType, 2 > OutputImageType;

    typedef typename InputImageType::PointType             ImagePointType;
    typedef typename InputImageType::IndexType             ImageIndexType;
    typedef typename InputImageType::IndexValueType        ImageIndexValueType;
    typedef typename InputImageType::SizeType              ImageSizeType;
    typedef typename InputImageType::SizeValueType         ImageSizeValueType;
    typedef typename InputImageType::RegionType            ImageRegionType;
    typedef typename InputImageType::SpacingType           ImageSpacingType;

    typedef typename OutputImageType::PointType            oImagePointType;
    typedef typename OutputImageType::OffsetType           oImageOffsetType;

    ImagePointType t_min, t_max;
    oImagePointType new_origin;

    // create ROI
    unsigned int k = 0;
    for( unsigned int dim = 0; dim < 3; dim++ )
      {
      t_min[dim] = iBounds[k++];
      t_max[dim] = iBounds[k++];
      }

    ImageIndexType startOfROI, endOfROI;
    iInput->TransformPhysicalPointToIndex( t_min, startOfROI );
    iInput->TransformPhysicalPointToIndex( t_max, endOfROI );

    ImageSizeType  sizeOfLargeImage =
        iInput->GetLargestPossibleRegion().GetSize();

    ImageSizeType  size;
    size.Fill( 0 );
#ifdef ITKv4
    oImageOffsetType  new_offset;
#else
    long int* new_offset;
#endif


    int l = 0;

    for( unsigned int dim = 0; dim < 3; dim++ )
      {
      if( startOfROI[dim] < 0 )
        {
        startOfROI[dim] = 0;
        }
      if( startOfROI[dim] > sizeOfLargeImage[dim] )
        {
        startOfROI[dim] = sizeOfLargeImage[dim] - 1;
        }

      if( endOfROI[dim] < 0 )
        {
        endOfROI[dim] = 0;
        }
      if( endOfROI[dim] > sizeOfLargeImage[dim] )
        {
        endOfROI[dim] = sizeOfLargeImage[dim] - 1;
        }

      size[dim] = endOfROI[dim] - startOfROI[dim];

      if( size[dim] < 0 )
        {
        size[dim] = 0;
        }

      // new origin
      if( size[dim] > 0 )
        {
        new_origin[l] = t_min[dim];
        double sizeTest = startOfROI[dim];
        new_offset[l] = -sizeTest;
        ++l;
        }
      }

    ImageRegionType region;
    region.SetSize(size);
    region.SetIndex(startOfROI);

    typedef itk::ExtractImageFilter< InputImageType, OutputImageType > FilterType;
    typedef typename FilterType::Pointer FilterTypePointer;
    FilterTypePointer filter = FilterType::New();
    filter->SetExtractionRegion( region );
    filter->SetInput( iInput );
#ifdef ITKv4
    filter->SetDirectionCollapseToIdentity();
#endif

    try
      {
      filter->Update();
      }
    catch (itk::ExceptionObject & err)
      {
      std::cerr << "extract slice Exception:" << err << std::endl;
      }

    // change information
    typedef typename itk::ChangeInformationImageFilter<OutputImageType > CenterFilterType;
    typedef typename CenterFilterType::Pointer CenterFilterTypePointer;

    CenterFilterTypePointer center = CenterFilterType::New();
    center->SetInput(filter->GetOutput());
    center->ChangeOriginOn();
    center->SetOutputOrigin(new_origin);
    center->ChangeRegionOn();
    center->SetOutputOffset(new_offset);

    try
      {
      center->Update();
      }
    catch (itk::ExceptionObject & err)
      {
      std::cerr << "change information slice Exception:" << err << std::endl;
      }
    return center->GetOutput();
  }

  /*
   * \brief Generate list of polydata given a list of vtkimages and a threshold
   * \param[in] iInputImage list of images
   * \param[in] iThreshold threshold
   * \return list of polydatas
   */
  std::vector<vtkPolyData*>  ExtractPolyData(
    std::vector<vtkImageData*>& iInputImage,
    const double & iThreshold);
  /*
   * \brief Generate a polydata given a vtkimage and a threshold
   * \param[in] iInputImage vtk image
   * \param[in] iThreshold threshold
   * \return polydata
   */
  vtkSmartPointer<vtkPolyData>  ExtractPolyData(
    vtkImageData *iInputImage,
    const double & iThreshold);

  /*
   * \brief Decimate a polydata
   * \param[in] iPolyData polyData to be decimated
   * \param[in] iNumberOfPoints target number of points
   * \return Decimated polyData
   */
  vtkSmartPointer<vtkPolyData> DecimatePolyData(
    vtkSmartPointer<vtkPolyData>& iPolyData,
    const unsigned int& iNumberOfPoints);

  /*
   * \brief Enable decimation during polydata extraction.
   * \param[in] iDecimate yes (true), no (false)
   */
  void SetDecimate(bool& iDecimate);
  /*
   * \brief Is decimation enabled?
   * \return true (yes), false (no)
   */
  bool GetDecimate();

  /*
   * \brief Set target number of points for polydata extraction
   * \param[in] iNumberOfPoints number of points
   */
  void SetNumberOfPoints( const unsigned int& iNumberOfPoints);
  /*
   * \brief Get target number of points for polydata extraction
   * \return number of points
   */
  unsigned int GetNumberOfPoints() const;

private:

  /*
   * \brief Reconstruct a contour from a vtkImageData and a threshold
   * \param[in] iInputImage vtkImageData
   * \param[in] iThreshold threshold
   * \return Pointer to a vtkPolyData
   */
  vtkSmartPointer<vtkPolyData> ExtractContour(
      vtkSmartPointer<vtkImageData> iInputImage,
      const double & iThreshold);

  /*
   * \brief Reorganize points within a contour
   * Required if we want to reedit this contour after.
   * 1-reorganize
   * \param[in] iInputImage vtkImageData
   * \return Pointer to a vtkPolyData
   */
  vtkSmartPointer<vtkPolyData> ReorganizeContour(
      vtkSmartPointer<vtkPolyData> iInputImage);

  /*
   * \brief Reconstruct a mesh from a vtkImageData and a threshold
   * \param[in] iInputImage vtkImageData
   * \param[in] iThreshold threshold
   * \return Pointer to a vtkPolyData
   */
  vtkSmartPointer<vtkPolyData> ExtractMesh(
      vtkSmartPointer<vtkImageData> iInputImage,
      const double & iThreshold);

  /*
   * \brief Decimate a contour (line)
   * \param[in] iPolyData polyData to be decimated
   * \param[in] iNumberOfPoints target number of points
   * \return Decimated polyData
   */
  vtkSmartPointer<vtkPolyData> DecimateContour(
      vtkSmartPointer<vtkPolyData> iPolyData, unsigned int iNumberOfPoints);

  /*
   * \brief Decimate a mesh
   * \param[in] iPolyData polyData to be decimated
   * \param[in] iNumberOfPoints target number of points
   * \return Decimated polyData
   */
  vtkSmartPointer<vtkPolyData> DecimateMesh(
      vtkSmartPointer<vtkPolyData> iPolyData, unsigned int iNumberOfPoints);

  bool m_Decimate;
  unsigned int m_NumberOfPoints;

protected:
  QGoAlgorithmWidget*             m_AlgoWidget;

  /**
  \brief construct the algowidget with the different parameters
  */
  virtual void SetAlgoWidget(QWidget* iParent = 0) = 0;

  /**
  \brief delete the different parameters
  */
  virtual void DeleteParameters() = 0;

  /*
   * \todo Arnaud has something for itkimage to vtkpolydata in 3d
   */
  //add a method std::vector<PolyData*> ConvertITKImagesToPolyData(std::vector<itk::Image> iImages)
  //add a method std::vector<TraceAttribut> GetAttribut(std::vector<vtkPolyData*> iNewTraces)
  //add a method itkImage ConvertVTKToITK(vtkImageIData iImage)
};

#endif
