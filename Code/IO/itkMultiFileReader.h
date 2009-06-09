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

#ifndef __itkMultiFileReader_h
#define __itkMultiFileReader_h

#include "itkLightProcessObject.h"

#include "vtkJPEGReader.h"
#include "vtkImageData.h"
#include "vtkImageAppend.h"

#include "GoFigureFileInfoHelper.h"
#include "MegaVTK2Configure.h"

enum FILETYPE
{
  BMP = 0,
  JPEG,
  PNG,
  TIFF,
  MHA,
  LSM
};

namespace itk
{
/**
*  \class MultiFileReader
*
*  \brief
*  Class that implement a reader for a stack of files
*
*  files can be of any dimensionality (we expect 2, 3 or 4D)
*  files can be of the following types: JPG, PNG, BMP, TIFF, MHA, LSM
*
*  \todo  Add Support for ProgressEvent
*
*
*/
class ITK_EXPORT MultiFileReader
{
public:


  /** Standard class typedefs.      */
  typedef MultiFileReader             Self;
  typedef LightProcessObject          Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** \brief set the time point you want to extract
      and load in memory.                              */
  void SetTimePoint( const int& UserTimePoint );

  /** \brief set the channel you want to extract
      and load in memory. -1 for all channels.         */
  void SetChannel( const int& UserChannel );
  void UpdateChannel();

  /** \brief set the input as a GoFigure format file list */
  void SetInput( FileListType* UserFileList );

  /** \brief  */
  void SetFileType( const FILETYPE UserFileType );

  /** \brief  */
  void SetDimensionality( int UserDimensionality );

  int GetNumberOfTimePoints( ) const { return m_NumberOfTimePoints; }

  /** \brief  */
  void SetMultiChannelImages( int value );
  void SetMultiChannelImagesON( )  { SetMultiChannelImages( 1 ); }
  void SetMultiChannelImagesOFF( ) { SetMultiChannelImages( 0 ); }

  /** \brief  */
  vtkImageData* GetOutput( ) const { return( m_OutputImage ); }

  // Fake It. The output is not of itkDataObject type
  // so pipeline mechanism would not work.
  void Update(void);

  MultiFileReader( );
  ~MultiFileReader();

  /** \brief Mandatory PrintSelf */
  void PrintSelf( std::ostream& os, Indent indent) const;

private:
  void ComputeUpdateFileList();

  MultiFileReader(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  vtkImageData*  m_OutputImage;
  FileListType*  m_FileList;
  FileListType   m_UpdateFileList;
  FILETYPE       m_FileType;        // suppose same file format for all files
  int            m_Dimensionality;  // suppose same dimensionality for all files
  int            m_DataScalarType;  // suppose same type for all files
  int            m_NumberOfChannels;// OfScalarComponents;
  int            m_NumberOfTimePoints;
  int            m_UpdateTimePoint;
  int            m_UpdateChannel;
  bool           m_AreImagesMultiChannel;
};

} // end of namespace itk

#endif
