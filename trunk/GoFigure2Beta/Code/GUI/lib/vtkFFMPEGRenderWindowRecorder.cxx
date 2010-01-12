#include "vtkFFMPEGRenderWindowRecorder.h"

#include "vtkRenderWindow.h"
#include "vtkWindowToImageFilter.h"
#include "vtkFFMPEGWriter.h"

#include "vtkObjectFactory.h"
#include "vtkDebugLeaks.h"


/**
 * \brief Constructor
 */
vtkFFMPEGRenderWindowRecorder::
vtkFFMPEGRenderWindowRecorder() :
  m_FrameRate (30), m_VideoQuality (1),
  m_FileName("goFigure2"), m_ControlIfVideoStarted (false)
{
	m_ImageFilter  = vtkWindowToImageFilter::New();
	m_ImageWriter  = vtkFFMPEGWriter::New();
}

/**
 * \brief Destructor
 */
vtkFFMPEGRenderWindowRecorder::
~vtkFFMPEGRenderWindowRecorder()
{
  if(m_ImageFilter != NULL)
  	{
	  m_ImageFilter ->Delete();
  	}
	if(m_ImageWriter != NULL)
		{
    m_ImageWriter ->Delete();
		}
}

/**
 * \brief Constructor to create a vtkObject from the vtkObjectFactory
 */
vtkFFMPEGRenderWindowRecorder*
vtkFFMPEGRenderWindowRecorder::
New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret =
    vtkObjectFactory::CreateInstance("vtkFFMPEGRenderWindowRecorder");
  if ( ret )
    {
    return static_cast<vtkFFMPEGRenderWindowRecorder *>(ret);
    }

  return new vtkFFMPEGRenderWindowRecorder;
}



/**
 * \brief Set the name of the video
 * \param[in] iFileName Name of the video
 */
void
vtkFFMPEGRenderWindowRecorder::
SetFileName( const std::string& iFileName)
{
	m_FileName = iFileName;
}

/**
 * \brief Set the rendering window to be observed
 * \param[in] iRenderWindow Rendering Window to be observed
 */
void
vtkFFMPEGRenderWindowRecorder::
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
vtkFFMPEGRenderWindowRecorder::
StartCapture()
{
	m_ImageWriter->SetFileName(m_FileName.c_str());
	m_ImageWriter->SetQuality(m_VideoQuality);
	m_ImageWriter->SetRate(m_FrameRate);

	if (m_ImageWriter->GetInput() != NULL)
		{
    m_ImageWriter->Start();
    m_ControlIfVideoStarted = true;
		}
}

/**
 * \brief End the video acquisition
 */
void
vtkFFMPEGRenderWindowRecorder::
EndCapture()
{
	if (m_ControlIfVideoStarted)
	  {
	  m_ImageWriter->End();
	  m_ControlIfVideoStarted = false;
	  }
}

/**
 * \brief Take a snapshot of the current rendering window
 */
void
vtkFFMPEGRenderWindowRecorder::
TakeSnapshot()
{
	if (m_ControlIfVideoStarted)
		{
	  m_ImageFilter->Modified();
	  m_ImageWriter->Write();
		}
}
