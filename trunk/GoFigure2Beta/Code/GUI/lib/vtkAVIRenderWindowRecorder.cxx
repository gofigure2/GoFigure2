#include "vtkAVIRenderWindowRecorder.h"

#include "vtkAVIWriter.h"
#include "vtkObjectFactory.h"

/**
 * \brief Constructor
 */
vtkAVIRenderWindowRecorder::
vtkAVIRenderWindowRecorder()
{
  m_ImageWriter  = vtkAVIWriter::New();
}

/**
 * \brief Destructor
 */
vtkAVIRenderWindowRecorder::
~vtkAVIRenderWindowRecorder()
{

}

void
vtkAVIRenderWindowRecorder::
SetSpecificParameters()
{
  vtkAVIWriter* TEMP = vtkAVIWriter::SafeDownCast( m_ImageWriter);
  TEMP->SetQuality(m_VideoQuality);
  TEMP->SetRate(m_FrameRate);
}

vtkAVIRenderWindowRecorder*
vtkAVIRenderWindowRecorder::
New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret =
    vtkObjectFactory::CreateInstance("vtkAVIRenderWindowRecorder");
  if ( ret )
    {
    return static_cast<vtkAVIRenderWindowRecorder *>(ret);
    }

  return new vtkAVIRenderWindowRecorder;
}
