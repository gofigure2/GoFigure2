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

// external library include
// VTK
#include "vtkSmartPointer.h"
// BOOST
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

// include project
#include "MegaImageStructure.h"
#include "itkMegaCaptureReader.h"

// external include
class vtkLookupTable;
class vtkImageData;

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
  \struct set_lut
  \brief change lut of given structure
  \sa MegaImageStructure
  */
struct set_lut
{
  set_lut(vtkSmartPointer<vtkLookupTable> iLUT):lut(iLUT){}

  void operator()(MegaImageStructure& iStructure)
  {
    iStructure.setLUT(lut);
  }

private:
  vtkSmartPointer<vtkLookupTable> lut;
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
using boost::multi_index_container;
using namespace boost::multi_index;

/* tags for accessing the corresponding indices of megaImageStructure */

struct Time{};
struct Channel{};

/* Define a multi_index_container of employees with following indices:
 *   - a unique index sorted by MegaImageStructure::Time,
 *   - a non-unique index sorted by MegaImageStructure::Channel,
 */

typedef multi_index_container<
  MegaImageStructure,
  indexed_by<
    ordered_unique<
      tag<Time>,  BOOST_MULTI_INDEX_MEMBER(MegaImageStructure,unsigned int,Time)>,
    ordered_non_unique<
      tag<Channel>, BOOST_MULTI_INDEX_MEMBER(MegaImageStructure,unsigned int,Channel)> >
> MegaImageStructureMultiIndexContainer;

//-----------------------------------------------------------------------------

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
  MegaImageProcessor(itk::MegaCaptureReader::Pointer iReader);

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

  /*
   * \brief create a lookuptable (LUT) given r, g, b and a.
   * LUT will go from black to the color.
   * \param[in] iRed red value
   * \param[in] iGreen green value
   * \param[in] iBlue blue value
   * \param[in] iAlpha alpha value
   * \return new LUT
   */
  vtkSmartPointer<vtkLookupTable> createLUT(const double& iRed,
                                            const double& iGreen,
                                            const double& iBlue,
                                            const double& iAlpha = 0);
  /*
   * \brief modify a lookuptable (LUT) given a channel, a time point and a LUT
   * Will remplace the existing one.
   * \param[in] iLUT new LUT
   * \param[in] iChannel channel to be modified
   * \param[in] iTime time point to be modified
   */
  void setLookupTable(vtkSmartPointer<vtkLookupTable> iLUT,
                      const unsigned int& iChannel,
                      const unsigned int& iTime);

  /*
   * \brief get a lookuptable (LUT) given a channel and a time point
   * \param[in] iChannel requested channel
   * \param[in] iTime requested time point
   * \return current LUT
   */
  vtkSmartPointer<vtkLookupTable> getLookuptable(const unsigned int& iChannel,
                                                 const unsigned int& iTime) const;

  /*
   * \brief load all the channels for the given time point into the
   * MegaImageStructure
   * \param[in] iTime requested time point
   */
  void setTimePoint(const unsigned int& iTime);

  /*
   * \brief load all time points of the given channel into the
   * MegaImageStructure. Called Doppler View.
   * \param[in] iChannel requested channel
   * \param[in] iFirstTime first time point
   * \param[in] iNumberOfImages number of images to be loaded
   * \param[in] iStepBetweenImages step between each image
   * \note need to store parameters if we want to go through volume
  * efficiently (not reload everything all the time)
   */
  void setDoppler(const unsigned int& iChannel, const unsigned int& iFirstTime,
                  const unsigned int& iNumberOfImages,
                  const unsigned int& iStepBetweenImages = 1);

  /*
   * \brief get single channel image given time point and channel from the
   * structure. Will create the new image from the structure.
   * \param[in] iTime requested time point
   * \param[in] iChannel requested channel
   * \return colored image.
   */
  vtkSmartPointer<vtkImageData> getImage(const unsigned int& iTime,
                                         const unsigned int& iChannel);

  /*
   * \brief get all channels image given a time point. Will create the new
   * image from the structure.
   * \param[in] iTime requested time point
   * \return colored image.
   */
  vtkSmartPointer<vtkImageData> getTimeAllChannels(const unsigned int& iTime);

  /*
   * \brief get all time points image given a channel. Will create the new
   * image from the structure.
   * \param[in] iChannel requested channel
   * \return colored image.
   */
  vtkSmartPointer<vtkImageData> getChannelAllTimes(const unsigned int& iChannel);

private:

  void initialize(); // to be called in contructor - First T all channels with associated color from Readers
  
  // Mega reader - might not be necessary...? should be required in constructor...
  //void setMegaReader(itkMegaCaptureReader* iReader);

  // public or private..?
  void update();

  /*
   * \brief Color an image given the original image and a lookuptable (LUT)
   * \param[in] iImage image to be colored
   * \param[in] iLUT LUT to be applied to the image
   * \return colored image
   */
  vtkSmartPointer<vtkImageData> colorImage(vtkSmartPointer<vtkImageData> iImage,
                                           vtkSmartPointer<vtkLookupTable> iLUT);

  itk::MegaCaptureReader::Pointer        m_MegaImageReader;
  MegaImageStructureMultiIndexContainer  m_MegaImageContainer;
  vtkSmartPointer<vtkImageData>          m_Output;
};

#endif // MEGAIMAGEPROCESSOR_H
