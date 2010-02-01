/*
 * vtkAVIRenderWindowRecorder.cxx
 *
 *  Created on: Feb 1, 2010
 *      Author: nicolas
 */

#include "vtkAVIRenderWindowRecorder.h"

#include "vtkRenderWindow.h"
#include "vtkWindowToImageFilter.h"
#include "vtkAVIWriter.h"

#include "vtkObjectFactory.h"

/**
 * \brief Constructor
 */
vtkAVIRenderWindowRecorder::
vtkAVIRenderWindowRecorder() :
  m_FrameRate (30), m_VideoQuality (1),
  m_FileName("goFigure2"), m_ControlIfVideoStarted (false)
{
  m_ImageFilter  = vtkWindowToImageFilter::New();
  m_ImageWriter  = vtkAVIWriter::New();
}

/**
 * \brief Destructor
 */
vtkAVIRenderWindowRecorder::
~vtkAVIRenderWindowRecorder()
{

  m_ImageFilter->Delete();

  // automatically deleted somehow...
  // m_ImageWriter->Delete();
}

/**
 * \brief Constructor to create a vtkObject from the vtkObjectFactory
 */
vtkAVIRenderWindowRecorder*
vtkAVIRenderWindowRecorder::
New()
{
  // First try to creaFFMPEGte the object from the vtkObjectFactory
  vtkObject* ret =
    vtkObjectFactory::CreateInstance("vtkAVIRenderWindowRecorder");
  if ( ret )
    {
    return static_cast<vtkAVIRenderWindowRecorder *>(ret);
    }

  return new vtkAVIRenderWindowRecorder;
}



/**
 * \brief Set the name of the video
 * \param[in] iFileName Name of the video
 */
void
vtkAVIRenderWindowRecorder::
SetFileName( const std::string& iFileName)
{
  m_FileName = iFileName;
}

/**
 * \brief Set the rendering window to be observed
 * \param[in] iRenderWindow Rendering Window to be observed
 */
void
vtkAVIRenderWindowRecorder::
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
vtkAVIRenderWindowRecorder::
StartCapture()
{
  m_ImageWriter->SetFileName(m_FileName.c_str());
  m_ImageWriter->SetQuality(m_VideoQuality);
  m_ImageWriter->SetRate(m_FrameRate);

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
vtkAVIRenderWindowRecorder::
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
vtkAVIRenderWindowRecorder::
TakeSnapshot()
{
  if (m_ControlIfVideoStarted)
    {
    m_ImageFilter->Modified();
    m_ImageWriter->Write();
    }
}
