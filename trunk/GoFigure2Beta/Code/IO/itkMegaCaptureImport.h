/*=========================================================================
  Author: $Author: agouaillard $  // Author of last commit
  Version: $Rev: 392 $  // Revision of last commit
  Date: $Date: 2009-07-14 17:06:11 -0400 (Tue, 14 Jul 2009) $  // Date of last commit
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

#ifndef __itkMegaCaptureImport_h
#define __itkMegaCaptureImport_h

#include <vector>
#include <algorithm>
#include <string.h>

#include "itkLightProcessObject.h"

#include <itksys/SystemTools.hxx>
#include "itkRegularExpressionSeriesFileNames.h"

#include "GoFigureFileInfoMultiIndexContainerHelper.h"
// #include "GoFigureFileInfoHelper.h"

// #include <QProgressBar>

namespace itk
{
/**\class MegaCaptureImport
\brief
\bug If the filenames imported are of old megacapture format(v2)
and that the text of the beginning of the filename (before the values
to be stored in the database) contains more than 3 numerical groups,
the files are considered as new megacapture...*/
class ITK_EXPORT MegaCaptureImport: public LightProcessObject
{
public:
  //typedef std::list<int>                     IntListType;
  typedef std::vector< int >                        IntVectorType;
  typedef std::pair< IntVectorType, IntVectorType > PairIntVectorType;
  typedef std::vector< std::string >                StringVectorType;

   /** Standard class typedefs.      */
  typedef MegaCaptureImport           Self;

  typedef LightProcessObject          Superclass;

  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( MegaCaptureImport, LightProcessObject );

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  void SetFileName( const std::string& iName );

  void SetTimeBased( const bool& iBool );
//   void SetProgressBar( QProgressBar* PB );

  void Glob();

  void CreateOutput();

  GoFigureFileInfoHelperMultiIndexContainer GetOutput();

  void Update();

  std::string GetHeaderFilename();

  /** \brief return true if the filename is of new megacapture format,
  false if it is the old one*/
  static bool IsNewMegaCapture( const std::string& iFilename );
  /** \brief return a modified cleaned filename */
  static std::string CleanFileName( const std::string& iFilename );
  /** \brief return the list of the Index and the list of the length of
  all the numerical group present in the filename*/
  static PairIntVectorType GetStartAndLengthOfNumericalGroupFilename(
    const std::string& iFilename);
  static bool AreTheseNumericalGroupNewMegaCapture(
    PairIntVectorType StartAndLengthNumericalGroup);


protected:
  MegaCaptureImport();
  ~MegaCaptureImport();

  /** \brief  Used for the database version1*/
  void OldMegaCaptureFile( GoFigureFileInfoHelper& ioTempInfo,
    const std::vector< unsigned int >& iNumericalValues);
  /** \brief  Used for the database version2*/
  void NewMegaCaptureFile( GoFigureFileInfoHelper& ioTempInfo,
    const std::vector< unsigned int >& iNumericalValues );

private:
  MegaCaptureImport (const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  PairIntVectorType m_StartAndLengthNumGroup;
  StringVectorType  m_FileNameS;
  GoFigureFileInfoHelperMultiIndexContainer      m_OutputFileList;
  std::string       m_FileName;
  std::string       m_fileNameModified;
  std::string       m_HeaderFileName;
//   QProgressBar*     m_ProgressBar;
//   bool              IsProgressBarSet;
  int               m_NbSignificantMegaCaptureNumGroup;

};

}

#endif
