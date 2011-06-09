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

#ifndef GoImageProcessor_H
#define GoImageProcessor_H

// Required for dynamic libs on Windows (QGoIOExport)
#include "QGoIOConfigure.h"

// external library include
// VTK
#include "vtkSmartPointer.h"
// BOOST
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>

// include project
#include "GoMegaImageStructure.h"

// external include
class vtkLookupTable;
class vtkImageData;
class vtkImageAccumulate;

class QString;

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
  \struct set_lut
  \brief change lut of given structure
  \sa GoMegaImageStructure
  */
struct set_lut
{
  set_lut(vtkSmartPointer<vtkLookupTable> iLUT):lut(iLUT){}

  void operator()(GoMegaImageStructure& iStructure)
  {
    iStructure.setLUT(lut);
  }

private:
  vtkSmartPointer<vtkLookupTable> lut;
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
  \struct set_visibility
  \brief change visibility of given structure
  \sa GoMegaImageStructure
  */
struct set_visibility
{
  set_visibility(bool iVisibility):visibility(iVisibility){}

  void operator()(GoMegaImageStructure& iStructure)
  {
    iStructure.setVisibility(visibility);
  }

private:
  bool visibility;
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
  \struct set_name
  \brief change visibility of given structure
  \sa GoMegaImageStructure
  */
struct set_name
{
  set_name(std::string iName):name(iName){}

  void operator()(GoMegaImageStructure& iStructure)
  {
    iStructure.setName(name);
  }

private:
  std::string name;
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
  \struct set_image
  \brief change visibility of given structure
  \sa GoMegaImageStructure
  */
struct set_image
{
  set_image(vtkImageData* iImage):image(iImage){}

  void operator()(GoMegaImageStructure& iStructure)
  {
    iStructure.setImage(image);
  }

private:
  vtkImageData* image;
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
  \struct set_image
  \brief change visibility of given structure
  \sa GoMegaImageStructure
  */
struct set_PointsRGBA
{
  set_PointsRGBA(std::vector< std::map< unsigned int, unsigned int> > iPoints):points(iPoints){}

  void operator()(GoMegaImageStructure& iStructure)
  {
    iStructure.setPointsRGBA(points);
  }

private:
  std::vector< std::map< unsigned int, unsigned int> > points;
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/* tags for accessing the corresponding indices of GoMegaImageStructure */

struct Index{};
struct Visibility{};
struct Name{};

/* Define a multi_index_container of images with following indices:
 *   - a non-unique index sorted by GoMegaImageStructure::Index,
  *   - a non-unique index sorted by GoMegaImageStructure::Visibility,
 *   - a non-unique index sorted by GoMegaImageStructure::Channel,
 */

typedef boost::multi_index_container<
  GoMegaImageStructure,
  boost::multi_index::indexed_by<
    boost::multi_index::ordered_non_unique<
      boost::multi_index::tag<Index>,  BOOST_MULTI_INDEX_MEMBER(GoMegaImageStructure,unsigned int,Index)>,
    boost::multi_index::ordered_non_unique<
      boost::multi_index::tag<Visibility>,  BOOST_MULTI_INDEX_MEMBER(GoMegaImageStructure,bool,Visibility)>,
    boost::multi_index::hashed_non_unique<
      boost::multi_index::tag<Name>,  BOOST_MULTI_INDEX_MEMBER(GoMegaImageStructure,std::string,Name)> >
> GoMegaImageStructureMultiIndexContainer;

//-----------------------------------------------------------------------------

/**
\defgroup Mega Mega
*/

/**
 * \struct GoImageProcessor
 * \brief  Interface between MegaReader and vtkImageData
 * \ingroup Mega
 */
class QGOIO_EXPORT GoImageProcessor
{
public:

  /** Constructor */
  GoImageProcessor();

  /** Constructor */
  GoImageProcessor( const GoImageProcessor& iE );

  /** Destructor */
  ~GoImageProcessor();
  
  /** Printing one element. std::cout << element << std::endl; */
  friend std::ostream & operator<<
    (std::ostream & os, const GoImageProcessor & c)
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
                                            const double& iAlpha,
                                            const double* iRange);
  /*
   * \brief modify a lookuptable (LUT) given a channel, a time point and a LUT
   * Will remplace the existing one.
   * \param[in] iLUT new LUT
   * \param[in] iChannel channel to be modified
   * \param[in] iTime time point to be modified
   */
  void setLookupTable(vtkSmartPointer<vtkLookupTable> iLUT,
                      const unsigned int& iIndex);

  /*
   * \brief get a lookuptable (LUT) given a channel and a time point
   * \param[in] iChannel requested channel
   * \param[in] iTime requested time point
   * \return current LUT
   */
  vtkSmartPointer<vtkLookupTable> getLookuptable(const unsigned int& iIndex) const;

  vtkSmartPointer<vtkLookupTable> getLookuptable(const std::string& iIndex) const;

  vtkSmartPointer<vtkLookupTable> getLookuptable() const;

  vtkSmartPointer<vtkPiecewiseFunction> getOpacityTransferFunction() const;

  vtkSmartPointer<vtkPiecewiseFunction>
  getOpacityTransferFunction(const std::string& iIndex) const;

  void updateOpacityTransferFunction();

  std::vector<vtkPiecewiseFunction*> getOpacityTransferFunctions();

  std::vector<double> getColor(const unsigned int& iIndex) const;

  std::vector<double> getColor(const std::string& iIndex) const;

  std::vector<std::map<unsigned int, unsigned int> > getRGBA(const std::string& iIndex) const;

  vtkSmartPointer< vtkImageAccumulate>
      getHistogram(const std::string& iIndex) const;

  /*
   * \brief load all the channels for the given time point into the
   * GoMegaImageStructure
   * \param[in] iTime requested time point
   */
  virtual void initTimePoint(const unsigned int& iTime) = 0;

  virtual void setTimePoint(const unsigned int& iTime) = 0;

  /*
   * \brief load all time points of the given channel into the
   * GoMegaImageStructure. Called Doppler View.
   * \param[in] iChannel requested channel
   * \param[in] iFirstTime first time point
   * \param[in] iNumberOfImages number of images to be loaded
   * \param[in] iStepBetweenImages step between each image
   * \note need to store parameters if we want to go through volume
  * efficiently (not reload everything all the time)
   */
  virtual void setDoppler(const unsigned int& iTime,
                          const unsigned int& iPrevious) = 0;

  /*
   * \brief get single channel image given time point and channel from the
   * structure. Will create the new image from the structure.
   * \param[in] iTime requested time point
   * \param[in] iChannel requested channel
   * \return raw image.
   */
  vtkSmartPointer<vtkImageData> getImageBW(const unsigned int& iIndex);
  vtkSmartPointer<vtkImageData> getImageBW();

  void setVisibilityChannel(const unsigned int& iIndex, const bool& iVisibility);

  void setNameChannel(const unsigned int& iIndex, const std::string& iName);

  std::string getNameChannel(const unsigned int& iIndex);

  std::vector<vtkImageData*> getColoredImages();

  template< class PixelType, const unsigned int VImageDimension >
  typename itk::Image< PixelType, VImageDimension >::Pointer
  getImageITK(int& iIndex)
  {
  GoMegaImageStructureMultiIndexContainer::index<Index>::type::iterator it =
      m_MegaImageContainer.get< Index >().find(iIndex);

  if(it!=m_MegaImageContainer.get< Index >().end())
    {
    return it->Convert2ITK<PixelType, VImageDimension>();
    }

    return NULL;
  }

  // find constant paramter and the other one return 4 point?

  /*
   * \brief get all the images present in the containerl. Will create the new
   * image from the structure.
   * \param[in] iChannel requested channel
   * \return colored image.
   */
  vtkSmartPointer<vtkImageData> getAllImages();

  // Image parameters
  //--------------------
  unsigned int* getBoundsTime();
  unsigned int* getBoundsChannel();

  int*          getExtent();

  unsigned int getNumberOfTimePoints();
  unsigned int getNumberOfChannels();

  unsigned int getTimeInterval() const;

  // Doppler parameters
  //--------------------
  unsigned int getDopplerStep();
  void setDopplerStep(unsigned int iStep);
  int* getDopplerTime(unsigned int iTime);
  void setDopplerMode(const bool& iEnable, const unsigned int& iChannel);
  bool getDopplerMode();
  unsigned int getDopplerChannel();

  void visibilityChanged(std::string, bool);

  unsigned int getNumberOfVisibleChannels();

  std::vector<std::string> getVisibilityVector();
  void setVisibilityVector(const std::vector<std::string>& iVisibility);

  unsigned int getContainerSize();

  void updatePoints(QString iChannel, std::vector< std::map< unsigned int, unsigned int> > iPointsRGBA);

protected:
  /*
   * \brief Color an image given the original image and a lookuptable (LUT)
   * \param[in] iImage image to be colored
   * \param[in] iLUT LUT to be applied to the image
   * \return colored image
   */
  vtkSmartPointer<vtkImageData> colorImage(vtkSmartPointer<vtkImageData> iImage,
                                           vtkSmartPointer<vtkLookupTable> iLUT);

  GoMegaImageStructureMultiIndexContainer m_MegaImageContainer;
  vtkSmartPointer<vtkImageData>           m_Output;

  // Image parameters
  //--------------------
  unsigned int m_BoundsTime[2];
  unsigned int m_BoundsChannel[2];
  int          m_Extent[6];
  unsigned int m_TimeInterval;
  //--------------------

  // Doppler view parameters
  //--------------------
  bool         m_DopplerMode;
  unsigned int m_DopplerStep;
  int          m_DopplerTime[3];
  unsigned int m_DopplerChannel;
  //--------------------

private:
  // overload "=" operator
  GoImageProcessor& operator=(const GoImageProcessor &rhs)
  {
    // Only do assignment if RHS is a different object from this.
    if (this != &rhs)
      {
      this->m_MegaImageContainer = rhs.m_MegaImageContainer;
      this->m_BoundsTime[0] = rhs.m_BoundsTime[0];
      this->m_BoundsTime[1] = rhs.m_BoundsTime[1];
      this->m_BoundsChannel[0] = rhs.m_BoundsChannel[0];
      this->m_BoundsChannel[1] = rhs.m_BoundsChannel[1];
      this->m_Extent[0] = rhs.m_Extent[0];
      this->m_Extent[1] = rhs.m_Extent[1];
      this->m_Extent[2] = rhs.m_Extent[2];
      this->m_Extent[3] = rhs.m_Extent[3];
      this->m_Extent[4] = rhs.m_Extent[4];
      this->m_Extent[5] = rhs.m_Extent[5];
      this->m_TimeInterval = rhs.m_TimeInterval;
      this->m_DopplerMode = rhs.m_DopplerMode;
      this->m_DopplerStep = rhs.m_DopplerStep;
      this->m_DopplerTime[0] = rhs.m_DopplerTime[0];
      this->m_DopplerTime[1] = rhs.m_DopplerTime[1];
      this->m_DopplerTime[2] = rhs.m_DopplerTime[2];
      this->m_DopplerChannel = rhs.m_DopplerChannel;
      }
    return *this;
  }

  vtkSmartPointer<vtkPiecewiseFunction> m_OpacityTF;

};

#endif // GoImageProcessor_H
