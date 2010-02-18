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

#ifndef __LSMToMegaCapture_h
#define __LSMToMegaCapture_h

#include <vector>
#include <string>
#include "GoFigureGlobalDefinition.h"

class vtkLSMReader;


/**
\brief Convert 1 LSM (5D file) into megacapture files
*/
class LSMToMegaCapture
{
  public:
    LSMToMegaCapture( );
    ~LSMToMegaCapture( );

    /**
     * \brief Set input lsm file. (Extract m_BaseName)
     * \param iFileName
     */
    void SetFileName( const std::string& iFileName );


    /**
     * \brief Set the output format: PNG (default) or TIFF
     * \param iFileType
     */
    void SetOutputFileType( const GoFigure::FileType& iFileType );
    void Export( const std::string& iDirectoryPath );

    void SetPlaque( const unsigned int& iPlaque );
    void SetRow( const unsigned int& iRow );
    void SetColumn( const unsigned int& iCol );
    void SetXTile( const unsigned int& iXt );
    void SetYTile( const unsigned int& iYt );
    void SetZTile( const unsigned int& iZt );
    std::vector< vtkLSMReader* > GetLSMReaders();

  protected:
    std::string m_FileName;
    std::vector< vtkLSMReader* > m_LSMReaders;
    unsigned int m_Plaque;
    unsigned int m_Row;
    unsigned int m_Column;
    unsigned int m_XTile;
    unsigned int m_YTile;
    unsigned int m_ZTile;
    double m_XOverlap;
    double m_YOverlap;
    double m_ZOverlap;
    unsigned int m_NumberOfChannels;
    unsigned int m_NumberOfTimePoints;
    GoFigure::FileType m_FileType;
    std::string m_BaseName;

};
#endif
