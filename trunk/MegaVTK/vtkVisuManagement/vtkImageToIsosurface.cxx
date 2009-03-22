/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkImageToIsosurface.cxx 477 2007-11-20 17:46:10Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2007-11-20 12:46:10 -0500 (Tue, 20 Nov 2007) $
Version:   $Revision: 477 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkImageToIsosurface.h"

#include <vtkProperty.h>
#include <vtkObjectFactory.h>

#include <sstream>
#include <assert.h>
#include <cmath>

vtkCxxRevisionMacro(vtkImageToIsosurface, "$Revision: 477 $");
vtkStandardNewMacro(vtkImageToIsosurface);

vtkImageToIsosurface::vtkImageToIsosurface()
{
  //allocate
  this->Thresholder        = vtkImageThreshold::New();
  this->ContourFilter      = vtkContourFilter::New();
  this->Decimate           = vtkDecimatePro::New();
  this->Smoother           = vtkSmoothPolyDataFilter::New();
  this->Mapper             = vtkPolyDataMapper::New();
  this->Actor              = vtkActor::New();
  this->Caster             = vtkImageCast::New();
  this->ContourFilter->ComputeScalarsOff();
  
  this->Input = 0;
  this->Caster->SetOutputScalarTypeToFloat();
  this->Thresholder->SetInput ( this->Caster->GetOutput() );
  this->ContourFilter->SetInput (this->Thresholder->GetOutput());

  //this->Smoother->FeatureEdgeSmoothingOn();
  this->Smoother->SetNumberOfIterations(200);
  //this->Smoother->SetInput ( this->Decimate->GetOutput() );
  this->Smoother->SetInput ( this->ContourFilter->GetOutput() );
  
  //this->Decimate->SetInput ( this->ContourFilter->GetOutput() );
  this->Decimate->SetInput ( this->Smoother->GetOutput() );
  this->Decimate->SetTargetReduction(0.9);
  //this->Decimate->PreserveTopologyOn();

  this->Mapper->SetInput (this->Decimate->GetOutput());
  //this->Mapper->SetInput (this->Smoother->GetOutput());
    
  this->Actor->GetProperty()->SetAmbient (0.1);
  this->Actor->GetProperty()->SetDiffuse (0.6);
  this->Actor->GetProperty()->SetSpecularPower(10.0);
  this->Actor->GetProperty()->SetInterpolationToGouraud();
  this->Actor->SetVisibility (true);
}

vtkImageToIsosurface::~vtkImageToIsosurface()
{
  this->Caster->Delete();
  this->Thresholder->Delete();
  this->Decimate->Delete();
  this->Smoother->Delete();
  this->ContourFilter->Delete();
  this->Mapper->Delete();
  this->Actor->Delete();

  if( this->Input )
  {
    Input->Delete();
  }
}

void vtkImageToIsosurface::Update()
{
  if (!this->Input)
  {
    return;
  }

  // compute data
  this->Caster->SetInput (Input);
  
  //this->ContourFilter->Update();
  //this->Decimate->Update();
  this->Mapper->GetInput()->Update();
  
  this->Actor->SetMapper ( this->Mapper );
 
  
}

void vtkImageToIsosurface::SetParameters (int  val, double color[4])
{
  double dval = (double)val;

  this->Thresholder->ThresholdBetween (dval-0.5, dval+0.5);
  this->Thresholder->SetInValue (dval);
  this->Thresholder->SetOutValue (0.0);
  this->ContourFilter->SetValue(0, dval-0.5);

  this->Actor->GetProperty()->SetColor ( color[0],
                                     color[1],
                                     color[2] );
  this->Actor->GetProperty()->SetOpacity (color[3]);
  
}
