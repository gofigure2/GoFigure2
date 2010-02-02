#ifndef __VTKAVIRENDERWINDOWRECORDER_H
#define __VTKAVIRENDERWINDOWRECORDER_H

#include <iostream>
#include <string>

#include "vtkRenderWindowMovieRecorder.h"
#include "vtkObject.h"

class vtkAVIRenderWindowRecorder : public vtkRenderWindowMovieRecorder
{
   public:
      static vtkAVIRenderWindowRecorder *New();
      vtkTypeMacro(vtkAVIRenderWindowRecorder,vtkObject);
     // set/get the quality of the video
     int m_VideoQuality;
     int m_FrameRate;
     void SetSpecificParameters();

   protected:
     vtkAVIRenderWindowRecorder();
     ~vtkAVIRenderWindowRecorder();
};
#endif /*__VTKAVIRENDERWINDOWRECORDER_H*/
