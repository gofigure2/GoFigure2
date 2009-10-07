/*=========================================================================
  Author: $Author: arnaudgelas $  // Author of last commit
  Version: $Rev: 195 $  // Revision of last commit
  Date: $Date: 2009-05-19 15:48:57 -0400 (Tue, 19 May 2009) $  // Date of last commit
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

#ifndef __itkLsm3DSerieImport_h
#define __itkLsm3DSerieImport_h

#include <vector>
#include <algorithm>
#include <string.h>

#include "itkLightProcessObject.h"

#include <itksys/SystemTools.hxx>
#include "itkRegularExpressionSeriesFileNames.h"

#include "GoFigureFileInfoHelper.h"

namespace itk
{

class ITK_EXPORT Lsm3DSerieImport: public LightProcessObject
{
public:
  typedef std::vector < int >         IntVectorType;
  typedef std::vector < std::string > StringVectorType;

  /** Standard class typedefs.      */
  typedef Lsm3DSerieImport            Self;
  typedef LightProcessObject          Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( Lsm3DSerieImport, LightProcessObject );

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  void SetFileName( std::string name );

  FileListType* GetOutput();

  void Update(void);

  void SetGroupId( int Uservalue );

protected:
  Lsm3DSerieImport();
  ~Lsm3DSerieImport();

  void Glob();
  void CreateOutput();
  

private:
  Lsm3DSerieImport (const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  IntVectorType    m_numGroupStart;
  IntVectorType    m_numGroupLength;
  int              m_GroupId;
  StringVectorType m_FileNameS;
  FileListType     m_OutputFileList;
  std::string      m_FileName;

};

// #ifndef ITK_MANUAL_INSTANTIATION
// #include "itkLsm3DSerieImport.txx"
// #endif

}

#endif
