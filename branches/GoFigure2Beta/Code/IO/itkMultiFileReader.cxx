/*=========================================================================
  Author: $Author: arnaudgelas $  // Author of last commit
  Version: $Rev: 536 $  // Revision of last commit
  Date: $Date: 2009-08-06 13:33:19 -0400 (Thu, 06 Aug 2009) $  // Date of last commit
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

#include "itkMultiFileReader.h"

#include "vtkImageAppendComponents.h"
#include "vtkImageExtractComponents.h"

#include "vtkLSMReader.h"
#include "vtkMetaImageReader.h"
#include "vtkBMPReader.h"
#include "vtkTIFFReader.h"
#include "vtkJPEGReader.h"
#include "vtkPNGReader.h"

namespace itk
{

//-----------------------------------------------------------------------------
MultiFileReader::MultiFileReader( ) : m_OutputImage( 0 ), m_FileList( 0 ),
  m_Dimensionality( 0 ), m_DataScalarType( -1 ), m_NumberOfChannels( -1 ),
  m_NumberOfTimePoints( -1 ), m_NumberOfSlices( -1 ), m_UpdateTimePoint( -1 ),
  m_UpdateZSlice( -1 ), m_UpdateChannel( -1 ), m_MultiChannelImages( false ),
  m_TimeBased( true )//, m_ProgressBar( 0 ), m_IsProgressBarSet( false )
{
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
MultiFileReader::~MultiFileReader()
{
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void MultiFileReader::SetTimePoint( const int& UserTimePoint )
{
  if( UserTimePoint <= m_NumberOfTimePoints )
    {
    this->m_UpdateTimePoint = UserTimePoint;
    }
  else
    {
    this->m_UpdateTimePoint = m_NumberOfTimePoints;
    }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MultiFileReader::SetZDepth( const int& iZ )
{
  if( iZ <= m_NumberOfSlices )
    {
    this->m_UpdateZSlice = iZ;
    }
  else
    {
    this->m_UpdateZSlice = m_NumberOfSlices;
    }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MultiFileReader::SetChannel( const int& UserChannel )
{
  m_UpdateChannel = UserChannel;

  UpdateChannel();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void MultiFileReader::UpdateChannel()
{
  if( this->m_UpdateChannel > m_NumberOfChannels )
    {
    this->m_UpdateChannel = m_NumberOfChannels - 1;
    }
  if( this->m_UpdateChannel < 0 )
    {
    this->m_UpdateChannel = 0;
    }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MultiFileReader::SetInput( FileListType* UserFileList )
{
  if( !UserFileList )
    {
    return;
    }

  if( UserFileList->empty() )
    {
    return;
    }

  this->m_FileList = UserFileList;

  FileListType::iterator endIt = m_FileList->end();
  FileListType::iterator It = m_FileList->begin();

  if( this->m_TimeBased )
    {
    unsigned int CurrentTimePoint = (*It).m_TCoord;
    this->m_NumberOfTimePoints = 1;
    while( It != endIt )
      {
      if( (*It).m_TCoord > CurrentTimePoint )
        {
        CurrentTimePoint = (*It).m_TCoord;
        ++m_NumberOfTimePoints;
        }
      ++It;
      }
    }
  else
    {
    unsigned int CurrentZSlice = (*It).m_ZCoord;
    this->m_NumberOfSlices = 1;
    while( It != endIt )
      {
      if( (*It).m_ZCoord > CurrentZSlice )
        {
        CurrentZSlice = (*It).m_ZCoord;
        ++m_NumberOfSlices;
        }
      ++It;
      }
    }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MultiFileReader::PrintSelf( std::ostream& os, Indent indent) const
{
  os << indent << "Images Dimensionality: "
     << m_Dimensionality << std::endl;
  os << indent << "Images Value Type: "
     << m_DataScalarType << std::endl;
  os << indent << "Number Of Channels: "
     << m_NumberOfChannels << std::endl;
  os << indent << "Number of Time Points: "
     << m_NumberOfTimePoints << std::endl;
  os << indent << "Update Time Point: "
     << m_UpdateTimePoint << std::endl;
  os << indent << "Update Channel: "
     << m_UpdateChannel << std::endl;
  os << indent << "Are Images multichannel? "
     << m_MultiChannelImages << std::endl;
}
//-----------------------------------------------------------------------------

void MultiFileReader::BuildVolumeFrom2DImages()
{
  // prepare the final output
  vtkImageAppend* volumeBuilder = vtkImageAppend::New();
  volumeBuilder->SetAppendAxis( 2 ); //append along Z

  // read the files and feed the volume builder
  // NOTE ALEX: sould convert to a vtkStringArray and use SetFileNames
  // note the S at  the end of the method name
  FileListType::iterator endIt = m_UpdateFileList.end();
  FileListType::iterator It    = m_UpdateFileList.begin();
  int counter = 0;
  while( It != endIt )
    {
    switch( this->m_FileType )
      {
      case JPEG:
        {
        AddToVolumeBuilder< vtkJPEGReader >( counter, (*It).m_Filename,
          volumeBuilder );
        break;
        }
      case BMP:
        {
        AddToVolumeBuilder< vtkBMPReader >( counter, (*It).m_Filename,
          volumeBuilder );
        break;
        }
      case PNG:
        {
        AddToVolumeBuilder< vtkPNGReader >( counter, (*It).m_Filename,
          volumeBuilder );
        break;
        }
      case TIFF:
        {
        AddToVolumeBuilder< vtkTIFFReader >( counter, (*It).m_Filename,
          volumeBuilder );
        break;
        }
      case MHA:
        {
        AddToVolumeBuilder< vtkMetaImageReader >( counter, (*It).m_Filename,
          volumeBuilder );
        break;
        }
      case LSM:
        {
        itkGenericExceptionMacro( << "stacks of 2D LSM are not supported at this time." );
        break;
        }
      default:
        {
        itkGenericExceptionMacro( << "unsupported type: " << this->m_FileType << "." );
        break;
        }
      }

//     if( m_IsProgressBarSet )
//       {
//       m_ProgressBar->setValue(
//           static_cast< float >( counter * 80 ) /
//           static_cast< float >( m_UpdateFileList.size() ) );
//       }
      ++It;
      ++counter;
    }

    volumeBuilder->Update();
    vtkImageData* temp_output = volumeBuilder->GetOutput();
    m_NumberOfChannels = temp_output->GetNumberOfScalarComponents();

    UpdateChannel();

    if( m_MultiChannelImages )
      {
      m_OutputImage = vtkImageData::New();
      m_OutputImage->ShallowCopy( temp_output );
      volumeBuilder->Delete();
      }
    else
      {
      vtkImageExtractComponents* extractComp = vtkImageExtractComponents::New();
      extractComp->SetComponents( m_UpdateChannel );
      extractComp->SetInput( temp_output );
      extractComp->Update( );

      volumeBuilder->Delete();

      m_OutputImage = vtkImageData::New();
      m_OutputImage->ShallowCopy( extractComp->GetOutput() );
      extractComp->Delete();
      }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// void MultiFileReader::SetProgressBar( QProgressBar* PB )
// {
//   if( PB )
//     {
//     m_ProgressBar = PB;
//     m_IsProgressBarSet = true;
//     }
// }
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
vtkImageData* MultiFileReader::GetOutput() const
{
  return m_OutputImage;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MultiFileReader::Update()
{
  this->GenerateData();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MultiFileReader::GenerateData( )
{
  if( m_OutputImage )
    {
    m_OutputImage->Delete();
    m_OutputImage = 0;
    }

//   if( m_IsProgressBarSet )
//     {
//     if( m_ProgressBar )
//       {
//       m_ProgressBar->show();
//       m_ProgressBar->setValue( 1 );
//       }
//     }

  ComputeUpdateFileList();
  if( m_UpdateFileList.empty() )
    {
    std::cout <<"Problem: m_UpdateFileList is empty :-/ (after ComputeUpdateFileList)" <<std::endl;
    return;
    }
//   if( m_IsProgressBarSet )
//     {
//     this->m_ProgressBar->setValue( 5 );
//     }

  if( this->m_Dimensionality == 2 )
    {
    BuildVolumeFrom2DImages();
    } // end of dimensionality == 2

  if( this->m_Dimensionality == 3 )
    {
    CreateVolumeFromOne3DImageFile();
    }
}
//-----------------------------------------------------------------------------
void MultiFileReader::CreateVolumeFromOne3DImageFile()
{
  FileListType::iterator It = m_UpdateFileList.begin();

  switch( this->m_FileType )
    {
    case JPEG: // fallthrough
    case BMP:  // falltrhough
    case PNG:  // fallthrough
      {
      itkGenericExceptionMacro( << "JPEG/BMP/PNG cannot be of dimensionality 3." );
      break;
      }
    case TIFF:
      {
      m_OutputImage = vtkImageData::New();
      Copy3DImage< vtkTIFFReader >( m_OutputImage, (*It).m_Filename.c_str() );
      break;
      }
    case MHA:
      {
      m_OutputImage = vtkImageData::New();
      Copy3DImage< vtkMetaImageReader >( m_OutputImage, (*It).m_Filename.c_str() );
      break;
      }
    case LSM:
      {
      /// \note this is still the old way to proceed...
      if( m_MultiChannelImages )
        {
        vtkImageData* myImage_ch1 = vtkImageData::New();
        vtkLSMReader* reader = vtkLSMReader::New();
        reader->SetFileName( (*It).m_Filename.c_str() );
        reader->Update();
        int NumberOfChannels = reader->GetNumberOfChannels();
        myImage_ch1->ShallowCopy( reader->GetOutput() );
        reader->Delete();

//         if( m_IsProgressBarSet )
//           {
//           m_ProgressBar->setValue( 20 );
//           }

        if( ( NumberOfChannels == 1 ) )
          {
          m_OutputImage = myImage_ch1;
          return;
          }

        vtkImageData* myImage_ch2 = vtkImageData::New();
        vtkLSMReader* reader2=vtkLSMReader::New();
        reader2->SetFileName( (*It).m_Filename.c_str() );
        reader2->SetUpdateChannel( 1 );
        reader2->Update();
        myImage_ch2->ShallowCopy( reader2->GetOutput() );
        reader2->Delete();

//         if( m_IsProgressBarSet )
//           {
//           m_ProgressBar->setValue( 40 );
//           }

        vtkImageAppendComponents* appendFilter = vtkImageAppendComponents::New();
        appendFilter->AddInput( myImage_ch1 );
        appendFilter->AddInput( myImage_ch2 );

//         if( m_IsProgressBarSet )
//           {
//           m_ProgressBar->setValue( 60 );
//           }

        vtkImageData* myImage_ch3 = vtkImageData::New();
        if( NumberOfChannels == 2 )
          {
          myImage_ch3->ShallowCopy( myImage_ch1 );
          }
        else
          {
          vtkLSMReader* reader3 = vtkLSMReader::New();
          reader3->SetFileName( (*It).m_Filename.c_str() );
          reader3->SetUpdateChannel( 2 );
          reader3->Update();
          myImage_ch3->ShallowCopy( reader3->GetOutput() );
          reader3->Delete();
          }

//         if( m_IsProgressBarSet )
//           {
//           m_ProgressBar->setValue( 80 );
//           }

        appendFilter->AddInput( myImage_ch3 );
        appendFilter->Update();

        vtkImageData* myImage3 = vtkImageData::New();
        myImage3->ShallowCopy( appendFilter->GetOutput() );

        appendFilter->Delete();
        myImage_ch3->Delete();
        myImage_ch2->Delete();
        myImage_ch1->Delete();

//         if( m_IsProgressBarSet )
//           {
//           m_ProgressBar->setValue( 100 );
//           }

        m_OutputImage = myImage3;
        }
      else
        {
        vtkLSMReader* reader = vtkLSMReader::New();
        reader->SetFileName( (*It).m_Filename.c_str() );
        reader->SetUpdateChannel( m_UpdateChannel );
        reader->Update();

        m_OutputImage = vtkImageData::New();
        m_OutputImage->ShallowCopy( reader->GetOutput() );
        reader->Delete();
        }
      break;
      }
    default:
      {
      itkGenericExceptionMacro( << "unsupported type: " << this->m_FileType << "." );
      break;
      }
    }

  m_NumberOfChannels = m_OutputImage->GetNumberOfScalarComponents();
  UpdateChannel();
}
//-----------------------------------------------------------------------------
void MultiFileReader::ComputeUpdateFileList()
{
  if( m_FileList->empty() )
    {
    itkGenericExceptionMacro( << "The File List is empty." );
    return;
    }

  if( this->m_Dimensionality < 4 )
    {
    // extract files of interest
    FileListType::iterator startIt;
    FileListType::iterator endIt = m_FileList->end();
    FileListType::iterator It = m_FileList->begin();

    // get the first file
    if( m_TimeBased )
      {
      while( It != endIt
            && static_cast< int >( (*It).m_TCoord ) < this->m_UpdateTimePoint )
        {
        It++;
        }
      }
    else
      {
      while( It != endIt
            && static_cast< int >( (*It).m_ZCoord ) < this->m_UpdateZSlice )
        {
        It++;
        }
      }
    startIt = It;

    // get the last file
    unsigned int counter = 0;
    if( this->m_Dimensionality == 3 )  // only one file
      {
      It++;
      }
    else // find all the 2D files needed for the 3D volume
      {
      if( m_TimeBased )
        {
        while( It != endIt
               && ( static_cast< int >( (*It).m_TCoord ) == this->m_UpdateTimePoint ) )
          {
          It++;
          counter++;
          }
        }
      else
        {
        while( It != endIt
               && static_cast< int >( (*It).m_ZCoord ) == this->m_UpdateZSlice )
          {
          It++;
          counter++;
          }
        }
      }
    endIt = It;

    // build up the file list;
    m_UpdateFileList.clear();
    It = startIt;
    while( It != endIt )
      {
      m_UpdateFileList.push_back( (*It) );
      It++;
      }
    }
  else
    {
    /// \todo to be defined (ex LSM 4D)
    }
}
//-----------------------------------------------------------------------------
}
