/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
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

#ifndef __itkMegaCaptureReader_h
#define __itkMegaCaptureReader_h

#include "GoFigureGlobalDefinition.h"
#include "itkLightProcessObject.h"
#include "GoFigureFileInfoMultiIndexContainerHelper.h"
#include "MegaCaptureHeaderReader.h"

#include "QGoIOConfigure.h"

class vtkImageData;

namespace itk
{
class QGOIO_EXPORT MegaCaptureReader:public LightProcessObject
{
public:
  /** Standard class typedefs.      */
  typedef MegaCaptureReader          Self;
  typedef LightProcessObject         Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  itkNewMacro(Self);

  itkTypeMacro(MegaCaptureReader, LightProcessObject);

  /** \brief set the input as a GoFigure format file list */
  void SetInput(const GoFigureFileInfoHelperMultiIndexContainer & UserFileList);

  void SetMegaCaptureHeader(const std::string & iHeader);

  /** \brief  */
  itkSetMacro(FileType, GoFigure::FileType);
  itkSetMacro(TimeBased, bool);

  void SetTimePoint(const unsigned int & iTm);

  itkGetConstMacro(UpdateTimePoint, unsigned int);

  itkGetConstMacro(MinTimePoint, unsigned int);
  itkGetConstMacro(MaxTimePoint, unsigned int);

  void SetZSlice(const unsigned int & iZs);

  itkGetConstMacro(UpdateZSlice, unsigned int);

  itkGetConstMacro(MinZSlice, unsigned int);
  itkGetConstMacro(MaxZSlice, unsigned int);

  itkGetConstMacro(MinChannel, unsigned int);
  itkGetConstMacro(MaxChannel, unsigned int);

  void Update();

  vtkImageData * GetOutput(const unsigned int & iChannel);

  std::map< unsigned int, vtkImageData * > GetOutputs();

protected:
  MegaCaptureReader();
  ~MegaCaptureReader();

  void ComputeBounds();

  std::map< unsigned int, vtkImageData * >     m_OutputImageMap;
  GoFigureFileInfoHelperMultiIndexContainer m_FileList;
  GoFigure::FileType m_FileType;
  MegaCaptureHeaderReader *m_HeaderReader;

  unsigned int m_MinTimePoint;
  unsigned int m_MaxTimePoint;
  unsigned int m_UpdateTimePoint;

  unsigned int m_MinZSlice;
  unsigned int m_MaxZSlice;
  unsigned int m_UpdateZSlice;

  unsigned int m_MinChannel;
  unsigned int m_MaxChannel;

  bool m_TimeBased;
  bool m_Modified;
private:
  MegaCaptureReader(const Self &);
  void operator=(const Self &);
};
}
#endif
