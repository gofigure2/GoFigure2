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

#include "itkMegaCaptureImport.h"

namespace itk
{
//--------------------------------------------------------------------------------
MegaCaptureImport::MegaCaptureImport()
{
//   this->IsProgressBarSet = false;
}

//-----------------------------------------------------------------------------

//--------------------------------------------------------------------------------
MegaCaptureImport::
~MegaCaptureImport()
{}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MegaCaptureImport::SetFileName(const std::string & iName)
{
  if ( !m_FileName.empty() && !iName.empty() && m_FileName.compare(iName) != 0 )
    {
    return;
    }
  if ( iName.empty() && m_FileName.empty() )
    {
    return;
    }
  m_FileName = iName;
  this->Modified();
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MegaCaptureImport::CreateOutput()
{
//   if( this->IsProgressBarSet )
//     {
//     this->/*m_ProgressBar*/->setValue( 60 );
//     }

  float counter = 0;

  std::vector< std::string >::iterator nit;
  for ( nit = m_FileNameS.begin();
        nit != m_FileNameS.end();
        nit++ )
    {
    GoFigureFileInfoHelper tempInfo;
    tempInfo.m_Filename = ( *nit );

    std::string origFileName =
      itksys::SystemTools::GetFilenameName( ( *nit ).c_str() );

    IntVectorType::reverse_iterator numGroupLengthItr =
      m_StartAndLengthNumGroup.second.rbegin();

    IntVectorType::reverse_iterator numGroupStartItr  =
      m_StartAndLengthNumGroup.first.rbegin();

    // int NbNumericalGroupsInFilename = m_StartAndLengthNumGroup.second.size();

    std::vector< unsigned int > NumericalValues(m_NbSignificantMegaCaptureNumGroup, 0);

    int megaCaptureNumericalGroupCounter = 0;

    while ( ( numGroupLengthItr != m_StartAndLengthNumGroup.second.rend() )
            && ( numGroupStartItr != m_StartAndLengthNumGroup.first.rend() )
            && ( megaCaptureNumericalGroupCounter < m_NbSignificantMegaCaptureNumGroup ) )
      {
      std::string ValueAsString(
        origFileName,
        ( *numGroupStartItr ),
        ( *numGroupLengthItr ) );

      int tempidx = m_NbSignificantMegaCaptureNumGroup - 1 - megaCaptureNumericalGroupCounter;

      NumericalValues[tempidx] = atoi( ValueAsString.c_str() );

      ++numGroupLengthItr;
      ++numGroupStartItr;
      ++megaCaptureNumericalGroupCounter;
      } // end for each numerical group

    if ( m_HeaderFileName == "" )
      {
      // get the beginning of the filename to look for the headerfile:
      // first, get back to the starting position of the last numerical values
      // group which
      // corresponds to the PL number:
      numGroupStartItr = numGroupStartItr - 1;

      // then, substract the "PL-" from the position of the PL Number to find
      // the position of the last character to be part of the headerfile name:
      int LengthHeaderFile = *numGroupStartItr - 3;

      // get the headerfile name and add the .meg
      std::string fileNamePath = itksys::SystemTools::GetFilenamePath(m_FileName);
      m_HeaderFileName = fileNamePath + "/"
                         + origFileName.substr(0, LengthHeaderFile) + ".meg";
      }

    if ( m_NbSignificantMegaCaptureNumGroup == 9 )
      {
      NewMegaCaptureFile(tempInfo, NumericalValues);
      m_OutputFileList.insert(tempInfo);
      }
    if ( m_NbSignificantMegaCaptureNumGroup == 6 )
      {
      OldMegaCaptureFile(tempInfo, NumericalValues);
      m_OutputFileList.insert(tempInfo);
      }

//     if( this->IsProgressBarSet )
//       {
//       int value = 60 + 30 * ( (float)(counter) / (float)(m_FileNameS.size())
// );
//       this->m_ProgressBar->setValue( value );
//       }

    counter += 1;
    } // end for each filename

  m_FileNameS.clear();

//   GoFigureFileInfoHelperTimeBasedCompare comparison;
//   std::sort(
//     m_OutputFileList.begin(), m_OutputFileList.end(), comparison );

//   if( this->IsProgressBarSet )
//     {
//     this->m_ProgressBar->setValue( 99 );
//     }

// #if !defined(ITK_LEAN_AND_MEAN) && !defined(__BORLANDC__) && !defined(NDEBUG)
//   GoFigureFileInfoHelperMultiIndexContainer::iterator myIt =
// m_OutputFileList.begin();
//   while( myIt != m_OutputFileList.end() )
//     {
//     itkDebugMacro(
//         << (*myIt).m_Filename
//         << " " << (*myIt).m_Channel
//         << " " << (*myIt).m_TCoord
//         << " " << (*myIt).m_ZCoord );
//       myIt++;
//     }
// #endif
}

//-----------------------------------------------------------------------------
GoFigureFileInfoHelperMultiIndexContainer
MegaCaptureImport::GetOutput()
{
  return m_OutputFileList;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MegaCaptureImport::Update()
{
  Glob();
  CreateOutput();
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MegaCaptureImport::Glob()
{
//   if( this->IsProgressBarSet )
//     {
//     this->m_ProgressBar->show();
//     this->m_ProgressBar->setValue( 1 );
//     }
  std::string FilenameModified = CleanFileName(m_FileName);

  m_StartAndLengthNumGroup = GetStartAndLengthOfNumericalGroupFilename(FilenameModified);

  // create the regular expression to glob the entire set of file
  std::string                     regExpFileName = FilenameModified;
  IntVectorType::reverse_iterator numGroupLengthItr = m_StartAndLengthNumGroup.second.rbegin();
  IntVectorType::reverse_iterator numGroupStartItr  = m_StartAndLengthNumGroup.first.rbegin();

  if ( AreTheseNumericalGroupNewMegaCapture(m_StartAndLengthNumGroup) )
    {
    m_NbSignificantMegaCaptureNumGroup = 9;
    }
  else
    {
    m_NbSignificantMegaCaptureNumGroup = 6;
    }

  std::string regExpString = "([0-9]+)";
  int         megaCaptureNumericalGroupCounter = 0;

  while ( numGroupLengthItr != m_StartAndLengthNumGroup.second.rend()
          && numGroupStartItr != m_StartAndLengthNumGroup.first.rend()
          && megaCaptureNumericalGroupCounter < m_NbSignificantMegaCaptureNumGroup )
    {
    regExpFileName.replace(*numGroupStartItr, *numGroupLengthItr, regExpString);
    ++numGroupLengthItr;
    ++numGroupStartItr;
    ++megaCaptureNumericalGroupCounter;
    }

//   if( this->IsProgressBarSet )
//     {
//     this->m_ProgressBar->setValue( 5 );
//     }

  // Include only filenames that exactly match this regular expression.  Don't
  // match filenames that have this string as a substring (ie. that have extra
  // prefixes or suffixes).
  regExpFileName = "^" + regExpFileName + "$";

  // Use a RegularExpressionSeriesFileNames to find the files to return
  std::string unixArchetype = m_FileName;
  itksys::SystemTools::ConvertToUnixSlashes(unixArchetype);

  std::string origFileName =
    itksys::SystemTools::GetFilenameName( unixArchetype.c_str() );

  std::string fileNamePath =
    itksys::SystemTools::GetFilenamePath( unixArchetype.c_str() );

  std::string pathPrefix;

  itk::RegularExpressionSeriesFileNames::Pointer fit =
    itk::RegularExpressionSeriesFileNames::New();
  fit->SetDirectory( fileNamePath.c_str() );
  fit->SetRegularExpression( regExpFileName.c_str() );
  fit->SetSubMatch(1);
  fit->NumericSortOn();

  m_FileNameS = fit->GetFileNames();

//   if( this->IsProgressBarSet )
//     {
//     this->m_ProgressBar->setValue( 45 );
//     }

  // re parse the indexes and length without the escape caracters
  std::string::iterator s_start = origFileName.begin();
  std::string::iterator s_end = origFileName.end();
  std::string::iterator s_it = s_start;
  int                   sIndex;

  while ( s_it != s_end )
    {
    // If the element is a number, find its starting index and length.
    if ( ( *s_it ) >= '0' && ( *s_it ) <= '9' )
      {
      sIndex = static_cast< int >( s_it - s_start );
      m_StartAndLengthNumGroup.first.push_back(sIndex);

      // Loop to one past the end of the group of numbers.
      while ( s_it != origFileName.end() && ( *s_it ) >= '0' && ( *s_it ) <= '9' )
        {
        ++s_it;
        }

      m_StartAndLengthNumGroup.second.push_back(static_cast< int >
                                                ( s_it - s_start ) - sIndex);

      if ( s_it == s_end )
        {
        break;
        }
      }
    ++s_it;
    }

//   if( this->IsProgressBarSet )
//     {
//     this->m_ProgressBar->setValue( 55 );
//     }
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool MegaCaptureImport::IsNewMegaCapture(const std::string & iFilename)
{
  std::string FilenameModified = CleanFileName(iFilename);

  PairIntVectorType StartAndLengthNumericalGroup =
    GetStartAndLengthOfNumericalGroupFilename(FilenameModified);

  return AreTheseNumericalGroupNewMegaCapture(StartAndLengthNumericalGroup);
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool MegaCaptureImport::AreTheseNumericalGroupNewMegaCapture(
  MegaCaptureImport::PairIntVectorType StartAndLengthNumericalGroup)
{
  return ( StartAndLengthNumericalGroup.first.size() >= 9 );
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string MegaCaptureImport::CleanFileName(const std::string & iFilename)
{
  // glob all jpeg file names
  std::string unixArchetype = iFilename;

  itksys::SystemTools::ConvertToUnixSlashes(unixArchetype);
  if ( itksys::SystemTools::FileIsDirectory( unixArchetype.c_str() ) )
    {
    // return type supposed to be one std::string!
    std::cout << "iFilename is directory" << std::endl;
    return std::string();
//     return false;
    }

  // Parse the fileNameName and fileNamePath
  std::string origFileName =
    itksys::SystemTools::GetFilenameName( unixArchetype.c_str() );
  std::string fileNamePath =
    itksys::SystemTools::GetFilenamePath( unixArchetype.c_str() );
  std::string pathPrefix;
  std::string ofileNameModified;
  // "Clean" the filename by escaping any special characters with backslashes.
  // This allows us to pass in filenames that include these special characters.
  for ( unsigned int j = 0; j < origFileName.length(); j++ )
    {
    char oneChar = origFileName[j];
    if (
      oneChar == '^'
      || oneChar == '$'
      || oneChar == '.'
      || oneChar == '['
      || oneChar == ']'
      || oneChar == '-'
      || oneChar == '*'
      || oneChar == '+'
      || oneChar == '?'
      || oneChar == '('
      || oneChar == ')' )
      {
      ofileNameModified += "\\";
      }
    ofileNameModified += oneChar;
    }

  // If there is no "/" in the name, the directory is not specified.
  // In that case, use the default ".".
  // This is necessary for the RegularExpressionSeriesFileNames.
  if ( fileNamePath == "" )
    {
    fileNamePath = ".";
    pathPrefix = "./";
    }
  else
    {
    pathPrefix = "";
    }
  return ofileNameModified;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MegaCaptureImport::PairIntVectorType
MegaCaptureImport::GetStartAndLengthOfNumericalGroupFilename(const std::string & iFilename)
{
  // parse and keep it for ouput generation
  PairIntVectorType oStartAndLength;

  std::string::const_iterator s_start = iFilename.begin();
  std::string::const_iterator s_it = s_start;
  std::string::const_iterator s_end = iFilename.end();

  int sIndex;

  while ( s_it != s_end )
    {
    // If the element is a number, find its starting index and length.
    if ( ( *s_it ) >= '0' && ( *s_it ) <= '9' )
      {
      sIndex = static_cast< int >( s_it - s_start );

      //the first vector of oStartAndLength contains the index of all the
      // starting numbers of
      //the numerical group:
      oStartAndLength.first.push_back(sIndex);

      // Loop to one past the end of the group of numbers.
      while ( ( s_it != s_end ) && ( ( *s_it ) >= '0' ) && ( ( *s_it ) <= '9' ) )
        {
        ++s_it;
        }
      //the second vector of oStartAndLength contains the length of the
      // numerical group:
      oStartAndLength.second.push_back(
        static_cast< int >( s_it - s_start ) - sIndex);

      if ( s_it == s_end )
        {
        break;
        }
      }
    ++s_it;
    }
  return oStartAndLength;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MegaCaptureImport::OldMegaCaptureFile(GoFigureFileInfoHelper & ioTempInfo,
                                           const std::vector< unsigned int > & iNumericalValues)
{
  ioTempInfo.m_CCoord = iNumericalValues[0];
  ioTempInfo.m_RCoord = iNumericalValues[1];
  ioTempInfo.m_YCoord = iNumericalValues[2];
  ioTempInfo.m_XCoord = iNumericalValues[3];
  ioTempInfo.m_TCoord = iNumericalValues[4];
  ioTempInfo.m_ZCoord = iNumericalValues[5];
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MegaCaptureImport::NewMegaCaptureFile(GoFigureFileInfoHelper & ioTempInfo,
                                           const std::vector< unsigned int > & iNumericalValues)
{
  ioTempInfo.m_PCoord     = iNumericalValues[0];
  ioTempInfo.m_CCoord     = iNumericalValues[1];
  ioTempInfo.m_RCoord     = iNumericalValues[2];
  ioTempInfo.m_ZTileCoord = iNumericalValues[3];
  ioTempInfo.m_YTileCoord = iNumericalValues[4];
  ioTempInfo.m_XTileCoord = iNumericalValues[5];
  ioTempInfo.m_TCoord     = iNumericalValues[6];
  ioTempInfo.m_Channel    = iNumericalValues[7];
  ioTempInfo.m_ZCoord     = iNumericalValues[8];
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string MegaCaptureImport::GetHeaderFilename()
{
  return m_HeaderFileName;
}
}
