#ifndef __VTKFFMPEGRENDERWINDOWRECORDER_H
#define __VTKFFMPEGRENDERWINDOWRECORDER_H

#include <iostream>
#include <string>

#include "vtkObject.h"

class vtkRenderWindow;
class vtkWindowToImageFilter;
class vtkFFMPEGWriter;

class vtkFFMPEGRenderWindowRecorder : public vtkObject
{
   public:
     static vtkFFMPEGRenderWindowRecorder *New();
     vtkTypeMacro(vtkFFMPEGRenderWindowRecorder,vtkObject);

     // set/get the quality of the video
     vtkSetMacro( m_VideoQuality, int );
     vtkGetMacro( m_VideoQuality, int );

     // set/get the frame rate of the video
     vtkSetMacro( m_FrameRate, int );
     vtkGetMacro( m_FrameRate, int );

     // set the name of the video (can be a path)
     void SetFileName( const std::string& );
     // set the render window to be observed
     void SetRenderingWindow( vtkRenderWindow* );
     // start the video acquisition
     void StartCapture();
     // end the video acquisition
     void EndCapture();
     // take a snapshot of the current render window
     void TakeSnapshot( );

   protected:
     vtkFFMPEGRenderWindowRecorder();
     ~vtkFFMPEGRenderWindowRecorder();

     vtkRenderWindow*        m_RenderWindow;
     vtkWindowToImageFilter* m_ImageFilter;
     vtkFFMPEGWriter*        m_ImageWriter;
     int                     m_FrameRate;
     int                     m_VideoQuality;
     std::string             m_FileName;
     bool                    m_ControlIfVideoStarted;
};
#endif
