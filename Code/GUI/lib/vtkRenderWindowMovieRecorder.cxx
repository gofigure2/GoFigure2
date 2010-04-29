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

#include "vtkRenderWindowMovieRecorder.h"

#include "vtkRenderWindow.h"
#include "vtkWindowToImageFilter.h"
#include "vtkGenericMovieWriter.h"

#include "vtkObjectFactory.h"

/**
 * \brief Constructor
 */
vtkRenderWindowMovieRecorder::
vtkRenderWindowMovieRecorder() : m_VideoQuality( 1 ), m_FrameRate( 10 )
    /*:
  m_FileName("goFigure2"), m_ControlIfVideoStarted (false)*/
{
  m_ImageFilter = vtkWindowToImageFilter::New();
}

/**
 * \brief Destructor
 */
vtkRenderWindowMovieRecorder::
~vtkRenderWindowMovieRecorder()
{
  m_ImageFilter->Delete();
}

/**
 * \brief Set the name of the video
 * \param[in] iFileName Name of the video
 */
void
vtkRenderWindowMovieRecorder::
SetFileName( const std::string& iFileName)
{
  m_FileName = iFileName;
}

/**
 * \brief Set the rendering window to be observed
 * \param[in] iRenderWindow Rendering Window to be observed
 */
void
vtkRenderWindowMovieRecorder::
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
vtkRenderWindowMovieRecorder::
StartCapture()
{
  m_ImageWriter->SetFileName(m_FileName.c_str());

  if( m_ImageWriter->GetInput() )
    {
    m_ImageWriter->Start();
    m_ControlIfVideoStarted = true;
    }
}

/**
 * \brief End the video acquisition
 */
void
vtkRenderWindowMovieRecorder::
EndCapture()
{
  if( m_ControlIfVideoStarted )
    {
    m_ImageWriter->End();
    m_ControlIfVideoStarted = false;
    }
}

/**
 * \brief Take a snapshot of the current rendering window
 */
void
vtkRenderWindowMovieRecorder::
TakeSnapshot()
{
  if (m_ControlIfVideoStarted)
    {
    m_ImageFilter->Modified();
    m_ImageWriter->Write();
    }
}

void
vtkRenderWindowMovieRecorder::
SetVideoQuality( int value)
{
  m_VideoQuality = value;
}

void
vtkRenderWindowMovieRecorder::
SetFrameRate( int value)
{
  m_FrameRate = value;
}
