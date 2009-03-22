/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkImageFuse.h 608 2008-01-14 08:21:23Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-01-14 09:21:23 +0100 (Mon, 14 Jan 2008) $
Version:   $Revision: 608 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtk_ImageFuse_h_
#define _vtk_ImageFuse_h_

#include <vtkImageAlgorithm.h>

/**
   \class vtkImageFuse vtkImageFuse.h "vtkImageFuse.h"
   \brief takes two input images and outputs a combination of them (grid or fuse)
   \author: Nicolas Toussaint
*/


class vtkImageCheckerboard;
class vtkImageBlend;
class vtkImageClamp;
class vtkLookupTable;
class vtkImageMapToColors;


class vtkImageFuse: public vtkImageAlgorithm
{
  
 public:


  enum FuseModeIds
  {
    FUSEMODE_GRID = 0,
    FUSEMODE_BLEND
  };  
  
  static vtkImageFuse* New();
  vtkTypeRevisionMacro(vtkImageFuse, vtkImageAlgorithm);

  // Description:
  // Replace one of the input connections with a new input.  You can
  // only replace input connections that you previously created with
  // AddInputConnection() or, in the case of the first input,
  // with SetInputConnection().
  virtual void ReplaceNthInputConnection(int idx, vtkAlgorithmOutput* input);

  // Description:
  // Set an Input of this filter.  This method is only for support of
  // old-style pipeline connections.  When writing new code you should
  // use SetInputConnection(), AddInputConnection(), and
  // ReplaceNthInputConnection() instead.
  void SetInput(int num, vtkDataObject *input);
  void SetInput(vtkDataObject *input) { this->SetInput(0, input); };

  // Description:
  // Get one input to this filter. This method is only for support of
  // old-style pipeline connections.  When writing new code you should
  // use vtkAlgorithm::GetInputConnection(0, num).
  vtkDataObject *GetInput(int num);
  vtkDataObject *GetInput() { return this->GetInput(0); };

  // Description:
  // Get the number of inputs to this filter. This method is only for
  // support of old-style pipeline connections.  When writing new code
  // you should use vtkAlgorithm::GetNumberOfInputConnections(0).
  int GetNumberOfInputs() { return this->GetNumberOfInputConnections(0); };

  vtkGetMacro (FuseMode, unsigned int);
  vtkSetMacro (FuseMode, unsigned int);

  vtkGetMacro (UseClamp, int);
  void SetUseClamp (int val);
  vtkBooleanMacro (UseClamp, int);
  vtkGetMacro (UseColorMaps, int);
  void SetUseColorMaps (int val);
  vtkBooleanMacro (UseColorMaps, int);
  
  
  
  vtkGetMacro(BlendAlpha, double);
  void SetBlendAlpha (double val);
  vtkGetMacro(CheckerAlpha, double);
  void SetCheckerAlpha (double val);

  vtkGetObjectMacro (ColorMap1, vtkLookupTable);
  vtkGetObjectMacro (ColorMap2, vtkLookupTable);
  void SetLookupTable1 (vtkLookupTable* lut);
  void SetLookupTable2 (vtkLookupTable* lut);

  virtual void SetupPipeline();
  
 protected:
  
  vtkImageFuse();
  ~vtkImageFuse();  

  
  
  // see vtkAlgorithm for docs.
  virtual int FillInputPortInformation(int, vtkInformation*);

  virtual int RequestInformation (vtkInformation *, 
                                  vtkInformationVector **,
                                  vtkInformationVector *);
  
  // see vtkAlgorithm for docs.
  virtual int RequestData(vtkInformation* request,
				 vtkInformationVector** inputVector,
				 vtkInformationVector* outputVector);


  virtual void PlugInput1(vtkDataObject *input);
  virtual void PlugInput2(vtkDataObject *input);
  
  vtkImageCheckerboard* CheckerBoardFilter;
  vtkImageBlend* BlendFilter;
  vtkImageClamp* ClampFilter1;
  vtkImageClamp* ClampFilter2;
  vtkImageMapToColors* ColorMapper1;
  vtkImageMapToColors* ColorMapper2;
  
  vtkLookupTable* ColorMap1;
  vtkLookupTable* ColorMap2;

  int UseClamp;
  int UseColorMaps;
  
  unsigned int FuseMode;
  double BlendAlpha;
  double CheckerAlpha;
  int Divisions[3];

  
  
 private:

  vtkImageFuse(const vtkImageFuse&);  // Not implemented.
  void operator=(const vtkImageFuse&);  // Not implemented.
  

};


#endif



  
