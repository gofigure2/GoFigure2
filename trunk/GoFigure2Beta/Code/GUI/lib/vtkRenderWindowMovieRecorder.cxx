/*
 * vtkRenderWindowMovieRecorder.cxx
 *
 *  Created on: Feb 1, 2010
 *      Author: nicolas
 */

#include "vtkRenderWindowMovieRecorder.h"

#include "vtkRenderWindow.h"
#include "vtkWindowToImageFilter.h"
#include "vtkGenericMovieWriter.h"

#include "vtkObjectFactory.h"

/**
 * \brief Constructor
 */
vtkRenderWindowMovieRecorder::
vtkRenderWindowMovieRecorder() /*:
  m_FileName("goFigure2"), m_ControlIfVideoStarted (false)*/
{
  m_ImageFilter = vtkWindowToImageFilter::New();
}

/**
 * \brief Destructor
 */
vtkRenderWindowMovieRecorder::
~vtkRenderWindowMovieRecorder()
{
  m_ImageFilter->Delete();
}

/**
 * \brief Constructor to create a vtkObject from the vtkObjectFactory
 */
/*vtkRenderWindowMovieRecorder*
vtkRenderWindowMovieRecorder::
New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret =
    vtkObjectFactory::CreateInstance("vtkRenderWindowMovieRecorder");
  if ( ret )
    {
    return static_cast<vtkRenderWindowMovieRecorder *>(ret);
    }

  return new vtkRenderWindowMovieRecorder;
}*/
/**
 * \brief Set the name of the video
 * \param[in] iFileName Name of the video
 */
void
vtkRenderWindowMovieRecorder::
SetFileName( const std::string& iFileName)
{
  m_FileName = iFileName;
}

/**
 * \brief Set the rendering window to be observed
 * \param[in] iRenderWindow Rendering Window to be observed
 */
void
vtkRenderWindowMovieRecorder::
SetRenderingWindow( vtkRenderWindow* iRenderWindow)
{
  m_RenderWindow = iRenderWindow;
  m_ImageFilter->SetInput(m_RenderWindow);

  m_ImageWriter->SetInput(m_ImageFilter->GetOutput());
}

/**
 * \brief Start the video acquisition
 */
void
vtkRenderWindowMovieRecorder::
StartCapture()
{
  m_ImageWriter->SetFileName(m_FileName.c_str());

  if( m_ImageWriter->GetInput() )
    {
    m_ImageWriter->Start();
    m_ControlIfVideoStarted = true;
    }
}

/**
 * \brief End the video acquisition
 */
void
vtkRenderWindowMovieRecorder::
EndCapture()
{
  if( m_ControlIfVideoStarted )
    {
    m_ImageWriter->End();
    m_ControlIfVideoStarted = false;
    }
}

/**
 * \brief Take a snapshot of the current rendering window
 */
void
vtkRenderWindowMovieRecorder::
TakeSnapshot()
{
  if (m_ControlIfVideoStarted)
    {
    m_ImageFilter->Modified();
    m_ImageWriter->Write();
    }
}

