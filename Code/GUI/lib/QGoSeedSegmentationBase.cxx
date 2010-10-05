/*=========================================================================
  Author: $Author: nicolasrannou $  // Author of last commit
  Version: $Rev: 2037 $  // Revision of last commit
  Date: $Date: 2010-08-23 16:33:20 -0400 (Mon, 23 Aug 2010) $  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
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
#include "QGoSeedSegmentationBase.h"

// vtk stuff
#include "vtkPoints.h"

#include "QGoSeedBaseWidget.h"

//--------------------------------------------------------------------------
QGoSeedSegmentationBase::
QGoSeedSegmentationBase(QObject* parent, QWidget* parentWidget, vtkPoints* seeds)
{
  m_Seeds = seeds;
  // initialize to 0 leads to segfaults
  m_Radius = 3.;

  m_SeedsPosition[0] = 0.;
  m_SeedsPosition[1] = 0.;
  m_SeedsPosition[3] = 0.;

  //Create associated Widget
  m_BaseAlgorithmSegmentationWidget = new QGoSeedBaseWidget(parentWidget);
  // connect with 3DwT to add the good number of channels
  QObject::connect(this, SIGNAL(addChannel(QString)),
      m_BaseAlgorithmSegmentationWidget, SLOT(AddChannel(QString)));
  // mesh has been created by a filter
  QObject::connect(m_BaseAlgorithmSegmentationWidget, SIGNAL(MeshCreated(vtkPolyData*)),
      this, SIGNAL(MeshCreated(vtkPolyData*)));
  // contour has been created by a filter
  QObject::connect(m_BaseAlgorithmSegmentationWidget, SIGNAL(ContourCreated(vtkPolyData*)),
      this, SIGNAL(ContourCreated(vtkPolyData*)));
  // image has been processed by a filter
  // should return the new image as argument
  QObject::connect(m_BaseAlgorithmSegmentationWidget, SIGNAL(ImageProcessed()),
      this, SIGNAL(ImageProcessed()));
  QObject::connect(m_BaseAlgorithmSegmentationWidget, SIGNAL(Radius(double)),
                   this, SLOT(setRadius(double)));

  QObject::connect(m_BaseAlgorithmSegmentationWidget, SIGNAL(UpdateSeeds()),
      this, SIGNAL(UpdateSeeds()));
  QObject::connect(m_BaseAlgorithmSegmentationWidget, SIGNAL(SegmentationFinished()),
      this, SIGNAL(SegmentationFinished()));
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoSeedSegmentationBase::
~QGoSeedSegmentationBase()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoSeedSegmentationBase::
setSeed(vtkPoints* iSeed)
{
  m_Seeds = iSeed;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPoints*
QGoSeedSegmentationBase::
getSeed()
{
  return m_Seeds;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoSeedSegmentationBase::
setRadius(double iRadius)
{
  m_Radius = iRadius;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
double
QGoSeedSegmentationBase::
getRadius()
{
  return m_Radius;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
double*
QGoSeedSegmentationBase::
getSeedsPosition()
{
  return m_SeedsPosition;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoSeedSegmentationBase::
ConnectSignals(QGoSeedBaseWidget* iWidget)
{
  QObject::connect(this, SIGNAL(addChannel(QString)),
      iWidget, SLOT(AddChannel(QString)));
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoSeedSegmentationBase::
SetChannel(int i)
{
  emit addChannel( QString::number(i, 10) );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QWidget*
QGoSeedSegmentationBase::
getWidget()
{
  return m_BaseAlgorithmSegmentationWidget;
}
//--------------------------------------------------------------------------
