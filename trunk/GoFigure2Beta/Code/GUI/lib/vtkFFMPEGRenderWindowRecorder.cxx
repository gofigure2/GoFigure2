#include "vtkFFMPEGRenderWindowRecorder.h"

#include "vtkFFMPEGWriter.h"
#include "vtkObjectFactory.h"

/**
 * \brief Constructor
 */
vtkFFMPEGRenderWindowRecorder::
vtkFFMPEGRenderWindowRecorder(): m_VideoQuality( 1 ), m_FrameRate( 10 )
{
  m_ImageWriter  = vtkFFMPEGWriter::New();

  // initialise values in the writer
  vtkFFMPEGWriter* tempFFFFMPEG = vtkFFMPEGWriter::SafeDownCast( m_ImageWriter);
  tempFFFFMPEG->SetQuality(m_VideoQuality);
  tempFFFFMPEG->SetRate(m_FrameRate);
}

/**
 * \brief Destructor
 */
vtkFFMPEGRenderWindowRecorder::
~vtkFFMPEGRenderWindowRecorder()
{
}

void
vtkFFMPEGRenderWindowRecorder::
SetSpecificParameters()
{
  vtkFFMPEGWriter* tempFFFFMPEG = vtkFFMPEGWriter::SafeDownCast( m_ImageWriter);
  tempFFFFMPEG->SetQuality(m_VideoQuality);
  tempFFFFMPEG->SetRate(m_FrameRate);
}

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

void
vtkFFMPEGRenderWindowRecorder::
SetVideoQuality( int value)
{
  m_VideoQuality = value;
}

void
vtkFFMPEGRenderWindowRecorder::
SetFrameRate( int value)
{
  m_FrameRate = value;
}
