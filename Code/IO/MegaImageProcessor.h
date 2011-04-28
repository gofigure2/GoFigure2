/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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

#ifndef MEGAIMAGEPROCESSOR_H
#define MEGAIMAGEPROCESSOR_H

// Required for dynamic libs on Windows (QGoIOExport)
#include "QGoIOConfigure.h"

// includes from external libs
#include "vtkSmartPointer.h"

// include project
#include "MegaImageStructure.h"

// external include
class vtkLookupTable;
class vtkImageData;
class vtkLookupTable;

// project include
class itkMegaCaptureReader;

/**
\defgroup Mega Mega
*/

/**
 * \struct MegaImageProcessor
 * \brief  Interface between MegaReader and vtkImageData
 * \ingroup Mega
 */
class QGOIO_EXPORT MegaImageProcessor
{
public:

  /** Constructor */
  MegaImageProcessor(itkMegaCaptureReader* iReader);

  /** Constructor */
  MegaImageProcessor( const MegaImageProcessor& iE );

  /** Destructor */
  ~MegaImageProcessor();
  
  /** Printing one element. std::cout << element << std::endl; */
  friend std::ostream & operator<<
    (std::ostream & os, const MegaImageProcessor & c)
  {
    //os << "TraceID " << c.TraceID << std::endl;
    return os;
  }

  // LUT
  vtkSmartPointer<vtkLookupTable> createLUT(const double& iRed,
                                            const double& iGreen,
                                            const double& iBlue,
                                            const double& iAlpha);
  void setLookupTable(vtkSmartPointer<vtkLookupTable> iLUT,
                      const unsigned int& iChannel,
                      const unsigned int& iTime);
  vtkSmartPointer<vtkLookupTable> getLookuptable(const unsigned int& iChannel,
                                                 const unsigned int& iTime);

  // Get processed data
  vtkSmartPointer<vtkImageData> getTime(const unsigned int& iTime,
                                        const unsigned int& iChannel); // all channels for given T // no doppler
  vtkSmartPointer<vtkImageData> getTimeAllChannels(const unsigned int& iTime); // all channels for given T // no doppler
  vtkSmartPointer<vtkImageData> getChannel(const unsigned int& iChannel,
                                           const unsigned int& iTime);
  vtkSmartPointer<vtkImageData> getChannelAllTimes(const unsigned int& iChannel);
  vtkSmartPointer<vtkImageData> getDoppler(const unsigned int& iChannel,
                                           const unsigned int& iFirstTime,
                                           const unsigned int& iNumberOfImages,
                                           const unsigned int& iStepBetweenImages);

  // public or private..?
  void Update();

private:

  void Initialize(); // to be called in contructor - First T all channels with associated color from Readers
  
  // Mega reader - might not be necessary...? should be required in constructor...
  void setMegaReader(itkMegaCaptureReader* iReader);

  itkMegaCaptureReader*         m_MegaImageReader;
  MegaImageStructure            m_MegaImageStructure;
  vtkSmartPointer<vtkImageData> m_Output;
};

#endif // MEGAIMAGEPROCESSOR_H
