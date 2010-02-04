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
  m_ImageWriter->Delete();
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
