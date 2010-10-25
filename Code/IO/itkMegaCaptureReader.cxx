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

#include "itkMegaCaptureReader.h"
#include "itkNumericTraits.h"

#include "vtkImageData.h"
#include "vtkImageAppend.h"

#include "vtkJPEGReader.h"
#include "vtkBMPReader.h"
#include "vtkPNGReader.h"
#include "vtkTIFFReader.h"
#include "vtkMetaImageReader.h"

#include "vtkSmartPointer.h"

#include "VolumeBuilderHelper.h"

namespace itk
{
//--------------------------------------------------------------------------
MegaCaptureReader::
MegaCaptureReader() :
  m_FileType(GoFigure::PNG),
  m_TimeBased(true),
  m_Modified(true)
{
  m_HeaderReader = new MegaCaptureHeaderReader("");
  unsigned int max_uint = itk::NumericTraits< unsigned int >::max();
  m_MinTimePoint = max_uint;
  m_MaxTimePoint = max_uint;
  m_UpdateTimePoint = max_uint;
  m_MinZSlice = max_uint;
  m_MaxZSlice = max_uint;
  m_UpdateZSlice = max_uint;
  m_MinChannel = max_uint;
  m_MaxChannel = max_uint;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
MegaCaptureReader::
~MegaCaptureReader()
{
  delete m_HeaderReader;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
MegaCaptureReader::SetTimePoint(const unsigned int & iT)
{
  if ( iT < m_MinTimePoint )
    {
    m_Modified = ( m_UpdateTimePoint != m_MinTimePoint );
    m_UpdateTimePoint = m_MinTimePoint;
    }
  else
    {
    if ( iT > m_MaxTimePoint )
      {
      m_Modified = ( m_UpdateTimePoint != m_MaxTimePoint );
      m_UpdateTimePoint = m_MaxTimePoint;
      }
    else
      {
      m_Modified = ( m_UpdateTimePoint != iT );
      m_UpdateTimePoint = iT;
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
MegaCaptureReader::SetZSlice(const unsigned int & iZs)
{
  if ( iZs < m_MinZSlice )
    {
    m_Modified = ( m_UpdateZSlice != m_MinZSlice );
    m_UpdateZSlice = m_MinZSlice;
    }
  else
    {
    if ( iZs > m_MaxZSlice )
      {
      m_Modified = ( m_UpdateZSlice != m_MaxZSlice );
      m_UpdateZSlice = m_MaxZSlice;
      }
    else
      {
      m_Modified = ( m_UpdateZSlice != iZs );
      m_UpdateZSlice = iZs;
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
MegaCaptureReader::SetMegaCaptureHeader(const std::string & iHeader)
{
  m_HeaderReader->SetFileName(iHeader);
  m_HeaderReader->Read();
  m_Modified = true;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
MegaCaptureReader::SetInput(const GoFigureFileInfoHelperMultiIndexContainer & iUserFileList)
{
  if ( iUserFileList.size() == 0 )
    {
    std::cerr << "iUserFileList.empty()" << std::endl;
    return;
    }

  m_FileList = iUserFileList;
  m_Modified = true;

  ComputeBounds();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
MegaCaptureReader::ComputeBounds()
{
  // get min time point
  GoFigureFileInfoHelperMultiIndexContainer::index< m_TCoord >::type::iterator
    tm_it = m_FileList.get< m_TCoord >().begin();

  m_MinTimePoint = ( *tm_it ).m_TCoord;

  // get max time point
  GoFigureFileInfoHelperMultiIndexContainer::index< m_TCoord >::type::reverse_iterator
    r_tm_it = m_FileList.get< m_TCoord >().rbegin();

  m_MaxTimePoint = ( *r_tm_it ).m_TCoord;

  // get min Z slices
  GoFigureFileInfoHelperMultiIndexContainer::index< m_ZCoord >::type::iterator
    zs_it = m_FileList.get< m_ZCoord >().begin();

  m_MinZSlice = ( *zs_it ).m_ZCoord;

  // get max Z slices
  GoFigureFileInfoHelperMultiIndexContainer::index< m_ZCoord >::type::reverse_iterator
    r_zs_it = m_FileList.get< m_ZCoord >().rbegin();

  m_MaxZSlice = ( *r_zs_it ).m_ZCoord;

  // get min channel
  GoFigureFileInfoHelperMultiIndexContainer::index< m_Channel >::type::iterator
    ch_it = m_FileList.get< m_Channel >().begin();

  m_MinChannel = ( *ch_it ).m_Channel;

  // (note that the real one could be different)
  GoFigureFileInfoHelperMultiIndexContainer::index< m_Channel >::type::reverse_iterator
    r_ch_it = m_FileList.get< m_Channel >().rbegin();

  m_MaxChannel = ( *r_ch_it ).m_Channel;
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void
MegaCaptureReader::
AddToVTKVolumeBuilder( const int& iCounter,
                    const std::string& iFileName,
                    vtkImageAppend *iBuilder )
{
  switch ( this->m_FileType )
    {
    case GoFigure::JPEG:
      {
      AddToVolumeBuilder< vtkJPEGReader >( iCounter, iFileName, iBuilder);
      break;
      }
    case GoFigure::BMP:
      {
      AddToVolumeBuilder< vtkBMPReader >(iCounter, iFileName, iBuilder);
      break;
      }
    case GoFigure::PNG:
      {
      AddToVolumeBuilder< vtkPNGReader >(iCounter, iFileName, iBuilder);
      break;
      }
    case GoFigure::TIFF:
      {
      AddToVolumeBuilder< vtkTIFFReader >(iCounter, iFileName, iBuilder);
      break;
      }
    case GoFigure::MHA:
      {
      AddToVolumeBuilder< vtkMetaImageReader >(iCounter, iFileName,
                                               iBuilder);
      break;
      }
    case GoFigure::LSM:
      {
      itkGenericExceptionMacro(<< "stacks of 2D LSM are not supported at this time.");
      break;
      }
    default:
      {
      itkGenericExceptionMacro(<< "unsupported type: " << m_FileType << ".");
      break;
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
MegaCaptureReader::Update()
{
  if ( m_Modified )
    {
    std::map< unsigned int, std::list< std::string > > filelistperchannel;

    if ( m_TimeBased )
      {
      filelistperchannel =
        GetAllFileNamesForGivenTCoord(m_FileList, m_UpdateTimePoint,
                                      m_MinChannel, m_MaxChannel);
      }
    else
      {
      filelistperchannel =
        GetAllFileNamesForGivenZCoord(m_FileList, m_UpdateZSlice,
                                      m_MinChannel, m_MaxChannel);
      }

    // prepare the final output

    std::map< unsigned int, std::list< std::string > >::iterator
      fch_it = filelistperchannel.begin();
    std::map< unsigned int, std::list< std::string > >::iterator
      fch_end = filelistperchannel.end();

    while ( fch_it != fch_end )
      {
      int counter = 0;

      vtkSmartPointer< vtkImageAppend > volumeBuilder =
        vtkSmartPointer< vtkImageAppend >::New();
      volumeBuilder->SetAppendAxis(2);

      std::list< std::string >::iterator f_it = fch_it->second.begin();
      std::list< std::string >::iterator f_end = fch_it->second.end();

      while ( f_it != f_end )
        {
        AddToVTKVolumeBuilder( counter, *f_it, volumeBuilder );

        ++f_it;
        ++counter;
        }

      volumeBuilder->Update();
      vtkImageData *temp_output = volumeBuilder->GetOutput();

      double zspacing = 1.;

      /// \note We are using m_VoxelSizeZ no matter if m_TimeBased is true or
      // false.
      /// Since m_TimeInterval >> m_VoxelSizeX, it makes sense to do like this.
      zspacing = m_HeaderReader->m_VoxelSizeZ;

      //     if( m_TimeBased )
      //       {
      //       zspacing = m_HeaderReader.m_VoxelSizeZ;
      //       }
      //     else
      //       {
      //       zspacing = m_HeaderReader.m_TimeInterval;
      //       }

      temp_output->SetSpacing(m_HeaderReader->m_VoxelSizeX,
                              m_HeaderReader->m_VoxelSizeY,
                              zspacing);

      if ( m_OutputImageMap.find(fch_it->first) == m_OutputImageMap.end() )
        {
        m_OutputImageMap[fch_it->first] = vtkImageData::New();
        }
      m_OutputImageMap[fch_it->first]->ShallowCopy(temp_output);

      ++fch_it;
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkImageData *
MegaCaptureReader::GetOutput(const unsigned int & iChannel)
{
  std::map< unsigned int, vtkImageData * >::iterator
    it = m_OutputImageMap.find(iChannel);

  if ( it != m_OutputImageMap.end() )
    {
    return it->second;
    }
  else
    {
    return NULL;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkImageData *
MegaCaptureReader::
GetImage( const unsigned int & iCh,
          const unsigned int & iT )
{
  std::list< std::string > filenames =
    GetAllFileNamesForGivenTCoordAndChannel( this->m_FileList, iT, iCh );

  if( filenames.empty() )
    {
    return NULL;
    }
  else
    {
    vtkSmartPointer< vtkImageAppend > volumeBuilder =
      vtkSmartPointer< vtkImageAppend >::New();
    volumeBuilder->SetAppendAxis(2);

    std::list< std::string >::iterator f_it = filenames.begin();
    std::list< std::string >::iterator f_end = filenames.end();
    int counter = 0;

    while( f_it != f_end )
      {
      this->AddToVTKVolumeBuilder( counter, *f_it, volumeBuilder );
      ++f_it;
      }
    volumeBuilder->Update();
    vtkImageData *temp_output = volumeBuilder->GetOutput();
    temp_output->SetSpacing(m_HeaderReader->m_VoxelSizeX,
                            m_HeaderReader->m_VoxelSizeY,
                            m_HeaderReader->m_VoxelSizeZ);
    return temp_output;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::map< unsigned int, vtkImageData * >
MegaCaptureReader::GetOutputs()
{
  return m_OutputImageMap;
}
} //end of namespace
