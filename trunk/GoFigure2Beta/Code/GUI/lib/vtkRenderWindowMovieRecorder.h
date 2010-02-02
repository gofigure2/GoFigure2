/*
 * vtkRenderWindowMovieRecorder.h
 *
 *  Created on: Feb 1, 2010
 *      Author: nicolas
 */

#ifndef VTKRENDERWINDOWMOVIERECORDER_H_
#define VTKRENDERWINDOWMOVIERECORDER_H_

#include <iostream>
#include <string>

#include "vtkProcessObject.h"
#include "vtkGenericMovieWriter.h"

class vtkRenderWindow;
class vtkWindowToImageFilter;

class vtkRenderWindowMovieRecorder : public vtkProcessObject
{
   public:
      //static vtkRenderWindowMovieRecorder *New();
      vtkTypeMacro(vtkRenderWindowMovieRecorder,vtkProcessObject);


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
     vtkRenderWindowMovieRecorder();
     ~vtkRenderWindowMovieRecorder();

     virtual void SetSpecificParameters() = 0;

     vtkRenderWindow*                           m_RenderWindow;
     vtkWindowToImageFilter*                    m_ImageFilter;
     vtkGenericMovieWriter*                     m_ImageWriter;
     std::string                                m_FileName;
     bool                                       m_ControlIfVideoStarted;
};

#endif /* VTKRENDERWINDOWMOVIERECORDER_H_ */
