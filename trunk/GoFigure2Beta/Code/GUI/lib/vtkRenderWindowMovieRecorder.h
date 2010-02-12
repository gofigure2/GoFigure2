/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

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

      virtual void SetSpecificParameters() = 0;

   protected:
     vtkRenderWindowMovieRecorder();
     ~vtkRenderWindowMovieRecorder();

     vtkRenderWindow*                           m_RenderWindow;
     vtkWindowToImageFilter*                    m_ImageFilter;
     vtkGenericMovieWriter*                     m_ImageWriter;
     std::string                                m_FileName;
     bool                                       m_ControlIfVideoStarted;
};

#endif /* VTKRENDERWINDOWMOVIERECORDER_H_ */
