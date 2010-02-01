/*
 * vtkAVIRenderWindowRecorder.h
 *
 *  Created on: Feb 1, 2010
 *      Author: nicolas
 */

#ifndef VTKAVIRENDERWINDOWRECORDER_H_
#define VTKAVIRENDERWINDOWRECORDER_H_

#include <iostream>
#include <string>

#include "vtkObject.h"

class vtkRenderWindow;
class vtkWindowToImageFilter;
class vtkAVIWriter;

class vtkAVIRenderWindowRecorder : public vtkObject
{
   public:
     static vtkAVIRenderWindowRecorder *New();
     vtkTypeMacro(vtkAVIRenderWindowRecorder,vtkObject);

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
     vtkAVIRenderWindowRecorder();
     ~vtkAVIRenderWindowRecorder();

     vtkRenderWindow*                           m_RenderWindow;
     vtkWindowToImageFilter*                    m_ImageFilter;
     vtkAVIWriter*                              m_ImageWriter;
     int                                        m_FrameRate;
     int                                        m_VideoQuality;
     std::string                                m_FileName;
     bool                                       m_ControlIfVideoStarted;
};

#endif /* VTKAVIRENDERWINDOWRECORDER_H_ */
