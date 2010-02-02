#include "vtkFFMPEGRenderWindowRecorder.h"

#include "vtkFFMPEGWriter.h"
#include "vtkObjectFactory.h"

/**
 * \brief Constructor
 */
vtkFFMPEGRenderWindowRecorder::
vtkFFMPEGRenderWindowRecorder()
{
  m_ImageWriter  = vtkFFMPEGWriter::New();
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
  vtkFFMPEGWriter* TEMP = vtkFFMPEGWriter::SafeDownCast( m_ImageWriter);
  TEMP->SetQuality(m_VideoQuality);
  TEMP->SetRate(m_FrameRate);
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
