#ifndef __VTKFFMPEGRENDERWINDOWRECORDER_H
#define __VTKFFMPEGRENDERWINDOWRECORDER_H

#include <iostream>
#include <string>

#include "vtkRenderWindowMovieRecorder.h"
#include "vtkObject.h"

class vtkFFMPEGRenderWindowRecorder : public vtkRenderWindowMovieRecorder
{
   public:
      static vtkFFMPEGRenderWindowRecorder *New();
      vtkTypeMacro(vtkFFMPEGRenderWindowRecorder,vtkObject);
     // set/get the quality of the video

     void SetSpecificParameters();
     void SetVideoQuality( int );
     void SetFrameRate( int );

   private:
     int m_VideoQuality;
     int m_FrameRate;

   protected:
     vtkFFMPEGRenderWindowRecorder();
     ~vtkFFMPEGRenderWindowRecorder();
};
#endif
