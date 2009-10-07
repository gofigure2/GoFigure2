/*=========================================================================
  Author: $Author: arnaudgelas $  // Author of last commit
  Version: $Rev: 526 $  // Revision of last commit
  Date: $Date: 2009-08-05 17:07:54 -0400 (Wed, 05 Aug 2009) $  // Date of last commit
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

#include "itkMegaCaptureImport.h"

namespace itk
{

//--------------------------------------------------------------------------------
MegaCaptureImport::
MegaCaptureImport( ) : IsProgressBarSet( false ), m_TimeBased( true )
{
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
MegaCaptureImport::
~MegaCaptureImport()
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MegaCaptureImport::SetTimeBased( const bool& iBool )
{
  m_TimeBased = iBool;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MegaCaptureImport::SetProgressBar( QProgressBar* PB )
{
  if( PB )
    {
    m_ProgressBar = PB;
    IsProgressBarSet = true;
    }
}

//-----------------------------------------------------------------------------
void
MegaCaptureImport::
SetFileName( std::string name )
{
  if (!m_FileName.empty()&& !name.empty()&& m_FileName.compare(name)!= 0)
    {
    return;
    }
  if (name.empty() && m_FileName.empty())
    {
    return;
    }
  m_FileName = name;
  this->Modified();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MegaCaptureImport::
CreateOutput()
{
  if( this->IsProgressBarSet )
    {
    this->m_ProgressBar->setValue( 60 );
    }

  unsigned int counter = 0;
  std::vector<std::string>::iterator nit;
  for( nit = m_FileNameS.begin();
       nit != m_FileNameS.end();
       nit++)
    {
    GoFigureFileInfoHelper tempInfo;
    tempInfo.m_Filename = (*nit);
    std::string origFileName =
      itksys::SystemTools::GetFilenameName( (*nit).c_str() );

    IntVectorType::reverse_iterator numGroupLengthItr =
      this->m_numGroupLength.rbegin();
    IntVectorType::reverse_iterator numGroupStartItr  =
      this->m_numGroupStart.rbegin();
    std::vector< unsigned int > NumericalValues( 6, 0 );
    int megaCaptureNumericalGroupCounter = 0;
    while( numGroupLengthItr != m_numGroupLength.rend() &&
           numGroupStartItr != m_numGroupStart.rend()   &&
           megaCaptureNumericalGroupCounter < 6 )
      {
      std::string ValueAsString(
        origFileName,
        (*numGroupStartItr),
        (*numGroupLengthItr) );
      NumericalValues[5-megaCaptureNumericalGroupCounter]
        = atof( ValueAsString.c_str() );
      ++numGroupLengthItr;
      ++numGroupStartItr;
      ++megaCaptureNumericalGroupCounter;

      } // end for each numerical group

    tempInfo.m_CTile  = NumericalValues[0];
    tempInfo.m_RTile  = NumericalValues[1];
    tempInfo.m_YCoord = NumericalValues[2];
    tempInfo.m_XCoord = NumericalValues[3];
    tempInfo.m_TCoord = NumericalValues[4];
    tempInfo.m_ZCoord = NumericalValues[5];

    m_OutputFileList.push_back( tempInfo );

    if( this->IsProgressBarSet )
      {
      int value = 60 + 30 * ( static_cast< float >(counter) /
          static_cast< float >( m_FileNameS.size() ) );
      this->m_ProgressBar->setValue( value );
      }

    counter++;
    } // end for each filename

  m_FileNameS.clear();

  if( m_TimeBased )
    {
    GoFigureFileInfoHelperTimeBasedCompare comparison;
    std::sort( m_OutputFileList.begin(), m_OutputFileList.end(), comparison );
    }
  else
    {
    GoFigureFileInfoHelperZCoordBasedCompare comparison;
    std::sort( m_OutputFileList.begin(), m_OutputFileList.end(), comparison );
    }

  if( this->IsProgressBarSet )
    {
    this->m_ProgressBar->setValue( 99 );
    }

#if !defined(ITK_LEAN_AND_MEAN) && !defined(__BORLANDC__) && !defined(NDEBUG)
  FileListType::iterator myIt = m_OutputFileList.begin();
  while( myIt != m_OutputFileList.end() )
    {
    itkDebugMacro(
        << (*myIt).m_Filename
        << " " << (*myIt).m_Channel
        << " " << (*myIt).m_TCoord
        << " " << (*myIt).m_ZCoord );
      myIt++;
    }
#endif
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
MegaCaptureImport::
Glob()
{
  if( this->IsProgressBarSet )
    {
    this->m_ProgressBar->show();
    this->m_ProgressBar->setValue( 1 );
    }

  m_numGroupStart.clear();
  m_numGroupLength.clear();

  // glob all jpeg file names
  std::string unixArchetype = m_FileName;
  itksys::SystemTools::ConvertToUnixSlashes(unixArchetype);
  if (itksys::SystemTools::FileIsDirectory( unixArchetype.c_str() ))
    {
    return;
    }

  // Parse the fileNameName and fileNamePath
  std::string origFileName =
    itksys::SystemTools::GetFilenameName( unixArchetype.c_str() );
  std::string fileNamePath =
    itksys::SystemTools::GetFilenamePath( unixArchetype.c_str() );
  std::string pathPrefix;

  // "Clean" the filename by escaping any special characters with backslashes.
  // This allows us to pass in filenames that include these special characters.
  std::string fileName;
  for( unsigned int j = 0; j < origFileName.length(); j++ )
    {
    char oneChar = origFileName[j];
    if(
      oneChar == '^' ||
      oneChar == '$' ||
      oneChar == '.' ||
      oneChar == '[' ||
      oneChar == ']' ||
      oneChar == '-' ||
      oneChar == '*' ||
      oneChar == '+' ||
      oneChar == '?' ||
      oneChar == '(' ||
      oneChar == ')' )
      {
      fileName += "\\";
      }
    fileName += oneChar;
    }

  // If there is no "/" in the name, the directory is not specified.
  // In that case, use the default ".".
  // This is necessary for the RegularExpressionSeriesFileNames.
  if (fileNamePath == "")
    {
    fileNamePath = ".";
    pathPrefix = "./";
    }
  else
    {
    pathPrefix = "";
    }

  std::string regExpString = "([0-9]+)";
  int sIndex;
  // parse and keep it for ouput generation
  std::string::iterator sit;
  for (sit = fileName.begin(); sit < fileName.end(); sit++)
    {
    // If the element is a number, find its starting index and length.
    if ((*sit) >= '0' && (*sit) <= '9')
      {
      sIndex = static_cast< int >( sit - fileName.begin() );
      //numGroupStart contains the index of all the starting numbers of
      //the numerical group:
      m_numGroupStart.push_back( sIndex );

      // Loop to one past the end of the group of numbers.
      while ( sit != fileName.end() && (*sit) >= '0' && (*sit) <= '9' )
        {
        ++sit;
        }

      m_numGroupLength.push_back( static_cast< int >(sit - fileName.begin()) - sIndex );

      if( sit == fileName.end() )
        {
        break;
        }
      }
    }

  // create the regular expression to glob the entire set of file
  std::string regExpFileName = fileName;
  IntVectorType::reverse_iterator numGroupLengthItr = m_numGroupLength.rbegin();
  IntVectorType::reverse_iterator numGroupStartItr  = m_numGroupStart.rbegin();
  int megaCaptureNumericalGroupCounter = 0;
  while( numGroupLengthItr != m_numGroupLength.rend() &&
         numGroupStartItr != m_numGroupStart.rend()   &&
         megaCaptureNumericalGroupCounter < 6 )
    {
    regExpFileName.replace(*numGroupStartItr,*numGroupLengthItr,regExpString);
    ++numGroupLengthItr;
    ++numGroupStartItr;
    ++megaCaptureNumericalGroupCounter;
    }

  if( this->IsProgressBarSet )
    {
    this->m_ProgressBar->setValue( 5 );
    }

  // Include only filenames that exactly match this regular expression.  Don't
  // match filenames that have this string as a substring (ie. that have extra
  // prefixes or suffixes).
  regExpFileName = "^" + regExpFileName + "$";

  // Use a RegularExpressionSeriesFileNames to find the files to return
  itk::RegularExpressionSeriesFileNames::Pointer fit = itk::RegularExpressionSeriesFileNames::New();
  fit->SetDirectory( fileNamePath.c_str() );
  fit->SetRegularExpression( regExpFileName.c_str() );
  fit->SetSubMatch(1);
  fit->NumericSortOn();

  m_FileNameS = fit->GetFileNames();

  if( this->IsProgressBarSet )
    {
    this->m_ProgressBar->setValue( 45 );
    }

  // re parse the indexes and length without the escape caracters
  for (sit = origFileName.begin(); sit < origFileName.end(); sit++)
    {
    // If the element is a number, find its starting index and length.
    if ((*sit) >= '0' && (*sit) <= '9')
      {
      sIndex = static_cast< int >( sit - origFileName.begin() );
      m_numGroupStart.push_back( sIndex );

      // Loop to one past the end of the group of numbers.
      while ( sit != origFileName.end() && (*sit) >= '0' && (*sit) <= '9' )
        {
        ++sit;
        }

      m_numGroupLength.push_back( static_cast< int >(sit - origFileName.begin()) - sIndex );

      if( sit == origFileName.end() )
        {
        break;
        }
      }
    }

  if( this->IsProgressBarSet )
    {
    this->m_ProgressBar->setValue( 55 );
    }

}
//-----------------------------------------------------------------------------

}
