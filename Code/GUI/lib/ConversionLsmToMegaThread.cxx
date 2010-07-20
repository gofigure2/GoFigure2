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
#include "ConversionLsmToMegaThread.h"

#include "LSMToMegaCapture.h"
#include "vtkLSMReader.h"

#include <fstream>
#include <sstream>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "vtkImageWriterHelper.h"
#include "vtkExtractVOI.h"
#include "vtkImageData.h"
#include "vtkLSMReader.h"
#include "vtkSmartPointer.h"

#include "vtkPNGWriter.h"
#include "vtkTIFFWriter.h"

#include <iostream>

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
ConversionLsmToMegaThread::
ConversionLsmToMegaThread () : m_BaseName(""), m_LsmPath(""), m_MegaPath(""),
  m_FileType(GoFigure::PNG), m_LSMReaders(0), m_Plaque (0), m_Row(0), m_Column (0),
  m_XTile(0), m_YTile(0), m_ZTile (0), m_XOverlap(0), m_YOverlap(0), m_ZOverlap(0),
  m_NumberOfChannels(0), m_NumberOfTimePoints(0), m_Dim(0)
  {
  }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
ConversionLsmToMegaThread::~ConversionLsmToMegaThread()
  {
  if (!m_LSMReaders.empty())
    {
    for (unsigned int i = 0; i < m_LSMReaders.size(); i++)
      {
      m_LSMReaders[i]->Delete();
      }
    }
  }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
ConversionLsmToMegaThread::
run()
{
  // Allocate memory to avoid automatic suppression at the end of this function
  /**
   * \todo Reimplement these methods in this function
   */
  LSMToMegaCapture* converter = new LSMToMegaCapture;
  converter->SetFileName(m_LsmPath);
  converter->SetOutputFileType(m_FileType);

  m_LSMReaders = converter->GetLSMReaders();
  m_NumberOfChannels = m_LSMReaders[0]->GetNumberOfChannels();
  m_NumberOfTimePoints = m_LSMReaders[0]->GetNumberOfTimePoints();
  int dim[5];
  m_LSMReaders[0]->GetDimensions(dim);
  m_Dim = dim[2];

  // Start conversion
  emit InitialisationProgressSent();

  //converter.Export( m_MegaPath );
  this->ExportWithReimplemented(m_MegaPath);

  // send message "conversion terminated"
  emit ConversionTerminatedSent();
}

void
ConversionLsmToMegaThread::
SetBaseName(std::string iBaseName)
{
  m_BaseName = iBaseName;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
ConversionLsmToMegaThread::
SetLsmPath(std::string iLsmPath)
{
  m_LsmPath = iLsmPath;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
ConversionLsmToMegaThread::
SetMegaPath(std::string iMegaPath)
{
  m_MegaPath = iMegaPath;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
ConversionLsmToMegaThread::
SetOutputFileType(const GoFigure::FileType& iFileType)
{
  m_FileType = iFileType;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
ConversionLsmToMegaThread::
ExportWithReimplemented(std::string iMegaPath)
{
  double spacing[3];
  m_LSMReaders[0]->GetVoxelSizes(spacing);

  int dim[5];
  m_LSMReaders[0]->GetDimensions(dim);

  std::string headerfilename = iMegaPath;
  headerfilename += m_BaseName;
  headerfilename += ".meg";

  std::ofstream file(headerfilename.c_str());
  file << "MegaCapture" << std::endl;
  file << "<ImageSessionData>" << std::endl;
  file << "Version 3.0" << std::endl;
  file << "ExperimentTitle " << std::endl;
  file << "ExperimentDescription ";
  if (m_LSMReaders[0]->GetDescription())
    {
    file << m_LSMReaders[0]->GetDescription();
    }
  file << std::endl;
  file << "TimeInterval " << m_LSMReaders[0]->GetTimeInterval() << std::endl;
  file << "Objective " << m_LSMReaders[0]->GetObjective() << std::endl;
  file << "VoxelSizeX " << spacing[0] * 1000000 << std::endl;
  file << "VoxelSizeY " << spacing[1] * 1000000 << std::endl;
  file << "VoxelSizeZ " << spacing[2] * 1000000 << std::endl;
  file << "DimensionX " << dim[0] << std::endl;
  file << "DimensionY " << dim[1] << std::endl;
  file << "DimensionPL " << m_Plaque << std::endl;
  file << "DimensionCO " << m_Column << std::endl;
  file << "DimensionRO " << m_Row << std::endl;
  file << "DimensionZT " << m_ZTile << std::endl;
  file << "DimensionYT " << m_YTile << std::endl;
  file << "DimensionXT " << m_XTile << std::endl;
  file << "DimensionTM " << m_NumberOfTimePoints << std::endl;
  file << "DimensionZS " << dim[2] << std::endl;
  file << "DimensionCH " << m_NumberOfChannels << std::endl;

  unsigned int i, j, k;
  for (i = 0; i < m_NumberOfChannels; i++)
    {
    int r = m_LSMReaders[0]->GetChannelColorComponent(i, 0);
    int g = m_LSMReaders[0]->GetChannelColorComponent(i, 1);
    int b = m_LSMReaders[0]->GetChannelColorComponent(i, 2);

    file << "ChannelColor" << i << " " << r * 256 * 256 + g * 256 + b << std::endl;

    //send signal for progress bar
    emit ProgressSent();
    }

  file << "ChannelDepth 8" << std::endl;
  file << "FileType PNG" << std::endl;
  file << "</ImageSessionData>" << std::endl;

  if (m_NumberOfChannels > 1)
    {
    for (i = 1; i < m_NumberOfChannels; i++)
      {
      m_LSMReaders.push_back(vtkLSMReader::New());
      m_LSMReaders[i]->SetFileName(m_LsmPath.c_str());
      m_LSMReaders[i]->SetUpdateChannel(i);
      //send signal for progress bar
      emit ProgressSent();
      }
    }

  int extent[6];

  char        timeStr[100] = "";
  struct stat buf;

  if (!stat(m_LsmPath.c_str(), &buf))
    {
    strftime(timeStr, 100, "%Y-%m-%d %H:%M:%S", localtime(&buf.st_mtime));
    }

  for (i = 0; i < m_NumberOfTimePoints; i++)
    {
    for (k = 0; k < m_NumberOfChannels; k++)
      {
      m_LSMReaders[k]->SetUpdateTimePoint(i);
      //send signal for progress bar
      emit ProgressSent();
      }
    for (j = 0; j < m_NumberOfChannels; j++)
      {
      m_LSMReaders[j]->Update();
      vtkImageData* image3d = m_LSMReaders[j]->GetOutput();
      image3d->GetExtent(extent);

      for (k = 0; k < static_cast<unsigned int>(dim[2]); k++)
        {
        std::stringstream filename;
        filename << m_BaseName << "-PL" << setfill('0') << setw(2) << m_Plaque;
        filename << "-CO" << setfill('0') << setw(2) << m_Column;
        filename << "-RO" << setfill('0') << setw(2) << m_Row;
        filename << "-ZT" << setfill('0') << setw(2) << m_ZTile;
        filename << "-YT" << setfill('0') << setw(2) << m_YTile;
        filename << "-XT" << setfill('0') << setw(2) << m_XTile;
        filename << "-TM" << setfill('0') << setw(4) << i;
        filename << "-ch" << setfill('0') << setw(2) << j;
        filename << "-zs" << setfill('0') << setw(4) << k;

        switch (m_FileType)
          {
          default:
          case GoFigure::PNG:
            {
            filename << ".png";
            break;
            }
          case GoFigure::TIFF:
            {
            filename << ".tiff";
            break;
            }
          }

        file << "<Image>" << std::endl;
        file << "Filename " << filename.str() << std::endl;
        file << "DateTime " << timeStr << std::endl;
        file << "StageX 1000" << std::endl;
        file << "StageY -1000" << std::endl;
        file << "Pinhole 44.216" << std::endl;
        file << "</Image>" << std::endl;

        vtkSmartPointer<vtkExtractVOI> extract =
          vtkSmartPointer<vtkExtractVOI>::New();
        extract->SetSampleRate(1, 1, 1);
        extract->SetInput(image3d);
        extract->SetVOI(extent[0], extent[1], extent[2], extent[3], k, k);
        extract->Update();

        vtkImageData* image2d = extract->GetOutput();

        std::string final_filename = iMegaPath;
        final_filename += filename.str();

        switch (m_FileType)
          {
          default:
          case GoFigure::PNG:
            {
            vtkWriteImage<vtkPNGWriter>(image2d, final_filename);
            break;
            }
          case GoFigure::TIFF:
            {
            vtkWriteImage<vtkTIFFWriter>(image2d, final_filename);
            break;
            }
          }
        //send signal for progress bar
        emit ProgressSent();
        }
      }
    }
  file.close();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int
ConversionLsmToMegaThread::
GetNumberOfPoints()
{
  int total = m_NumberOfChannels * m_NumberOfTimePoints
              + m_NumberOfChannels
              + m_NumberOfTimePoints * m_NumberOfChannels * m_Dim;

  return total;
}
//-------------------------------------------------------------------------
