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

#include "itkMegaCaptureReader.h"

#include "vtkImageData.h"
#include "vtkImageAppend.h"

#include "vtkJPEGReader.h"
#include "vtkBMPReader.h"
#include "vtkPNGReader.h"
#include "vtkTIFFReader.h"
#include "vtkMetaImageReader.h"

#include "VolumeBuilderHelper.h"

namespace itk
{

/**
 *
 */
MegaCaptureReader::
MegaCaptureReader() : m_OutputImage( 0 ), m_FileType( GoFigure::PNG ),
  m_MinTimePoint( 0 ), m_MaxTimePoint( 0 ), m_UpdateTimePoint( 0 ),
  m_MinZSlice( 0 ), m_MaxZSlice( 0 ), m_UpdateZSlice( 0 ),
  m_MinChannel( 0 ), m_MaxChannel( 0 ), m_UpdateChannel( 0 ),
  m_TimeBased( true )
{
  m_HeaderReader = new MegaCaptureHeaderReader( "" );
}

/**
 *
 */
MegaCaptureReader::
~MegaCaptureReader()
{
  delete m_HeaderReader;
}

/**
 *
 * \param iT
 */
void
MegaCaptureReader::
SetTimePoint( const unsigned int& iT )
{
  if( iT < m_MinTimePoint )
    {
    m_UpdateTimePoint = m_MinTimePoint;
    }
  else
    {
    if( iT > m_MaxTimePoint )
      {
      m_UpdateTimePoint = m_MaxTimePoint;
      }
    else
      {
      m_UpdateTimePoint = iT;
      }
    }
}

/**
 *
 * \param iZs
 */
void
MegaCaptureReader::
SetZSlice( const unsigned int& iZs )
{
  if( iZs < m_MinZSlice )
    {
    m_UpdateZSlice = m_MinZSlice;
    }
  else
    {
    if( iZs > m_MaxZSlice )
      {
      m_UpdateZSlice = m_MaxZSlice;
      }
    else
      {
      m_UpdateZSlice = iZs;
      }
    }
}

/**
 *
 * \param iCh
 */
void
MegaCaptureReader::
SetChannel( const unsigned int& iCh )
{
  if( iCh < m_MinChannel )
    {
    m_UpdateChannel = m_MinChannel;
    }
  else
    {
    if( iCh > m_MaxChannel )
      {
      m_UpdateChannel = m_MaxTimePoint;
      }
    else
      {
      m_UpdateChannel = iCh;
      }
    }
}

/**
 *
 * \param iHeader
 */
void
MegaCaptureReader::
SetMegaCaptureHeader( const std::string& iHeader )
{
  m_HeaderReader->SetFileName( iHeader );
  m_HeaderReader->Read();
}

/**
 *
 * \param iUserFileList
 */
void
MegaCaptureReader::
SetInput( const GoFigureFileInfoHelperMultiIndexContainer& iUserFileList )
{
  if( iUserFileList.size() == 0 )
    {
    std::cerr <<"iUserFileList.empty()" <<std::endl;
    return;
    }

  m_FileList = iUserFileList;

  ComputeBounds();
}


/**
 *
 */
void
MegaCaptureReader::
ComputeBounds()
{
  // get min time point
  GoFigureFileInfoHelperMultiIndexContainer::index< m_TCoord >::type::iterator
    tm_it = m_FileList.get< m_TCoord >().begin();

  m_MinTimePoint = (*tm_it).m_TCoord;

  // get max time point
  GoFigureFileInfoHelperMultiIndexContainer::index< m_TCoord >::type::reverse_iterator
    r_tm_it = m_FileList.get< m_TCoord >().rbegin();

  m_MaxTimePoint = (*r_tm_it).m_TCoord;

  // get min Z slices
  GoFigureFileInfoHelperMultiIndexContainer::index< m_ZCoord >::type::iterator
    zs_it = m_FileList.get< m_ZCoord >().begin();

  m_MinZSlice = (*zs_it).m_ZCoord;

  // get max Z slices
  GoFigureFileInfoHelperMultiIndexContainer::index< m_ZCoord >::type::reverse_iterator
    r_zs_it = m_FileList.get< m_ZCoord >().rbegin();

  m_MaxZSlice = (*r_zs_it).m_ZCoord;

  // get min channel
  GoFigureFileInfoHelperMultiIndexContainer::index< m_Channel >::type::iterator
    ch_it = m_FileList.get< m_Channel >().begin();

  m_MinChannel = (*ch_it).m_Channel;

  // (note that the real one could be different)
  GoFigureFileInfoHelperMultiIndexContainer::index< m_Channel >::type::reverse_iterator
    r_ch_it = m_FileList.get< m_Channel >().rbegin();

  m_MaxChannel = (*r_ch_it).m_Channel;
}

/**
 *
 */
void
MegaCaptureReader::
Update()
{
  std::list< std::string > filelist;

  if( m_TimeBased )
    {
    filelist = GetAllFileNamesForGivenTCoordAndChannel( m_FileList,
      m_UpdateTimePoint, m_UpdateChannel );
    }
  else
    {
    filelist = GetAllFileNamesForGivenZCoordPointAndChannel( m_FileList,
      m_UpdateZSlice, m_UpdateChannel );
    }

  // prepare the final output
  vtkImageAppend* volumeBuilder = vtkImageAppend::New();
  volumeBuilder->SetAppendAxis( 2 ); //append along Z

  std::list< std::string >::iterator f_it    = filelist.begin();
  std::list< std::string >::iterator f_end = filelist.end();

  int counter = 0;

  while( f_it != f_end )
    {
    switch( m_FileType )
      {
      case GoFigure::JPEG:
        {
        AddToVolumeBuilder< vtkJPEGReader >( counter, (*f_it), volumeBuilder );
        break;
        }
      case GoFigure::BMP:
        {
        AddToVolumeBuilder< vtkBMPReader >( counter, (*f_it), volumeBuilder );
        break;
        }
      case GoFigure::PNG:
        {
        AddToVolumeBuilder< vtkPNGReader >( counter, (*f_it), volumeBuilder );
        break;
        }
      case GoFigure::TIFF:
        {
        AddToVolumeBuilder< vtkTIFFReader >( counter, (*f_it), volumeBuilder );
        break;
        }
      case GoFigure::MHA:
        {
        AddToVolumeBuilder< vtkMetaImageReader >( counter, (*f_it),
          volumeBuilder );
        break;
        }
      case GoFigure::LSM:
        {
        itkGenericExceptionMacro( << "stacks of 2D LSM are not supported at this time." );
        break;
        }
      default:
        {
        itkGenericExceptionMacro( << "unsupported type: " << m_FileType << "." );
        break;
        }
      }

      ++f_it;
      ++counter;
    }

    volumeBuilder->Update();

    vtkImageData* temp_output = volumeBuilder->GetOutput();

    double zspacing = 1.;

    /// \note We are using m_VoxelSizeZ no matter if m_TimeBased is true or false.
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

    temp_output->SetSpacing( m_HeaderReader->m_VoxelSizeX,
        m_HeaderReader->m_VoxelSizeY,
        zspacing );

    if( !m_OutputImage )
      {
      m_OutputImage = vtkImageData::New();
      }

    m_OutputImage->ShallowCopy( temp_output );
    volumeBuilder->Delete();
}

/**
 *
 * \return
 */
vtkImageData*
MegaCaptureReader::
GetOutput()
{
  return m_OutputImage;
}

} //end of namespace
