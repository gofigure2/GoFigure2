/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkLookupTableManager.h 700 2008-02-22 16:19:25Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-02-22 11:19:25 -0500 (Fri, 22 Feb 2008) $
Version:   $Revision: 700 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtk_LookupTableManager_h_
#define _vtk_LookupTableManager_h_

#include <vector>

#include <vtkObject.h>
#include <vtkLookupTable.h>

class vtkLookupTableManager: public vtkObject
{

 public:

  //BTX
  enum LookupTableIds
  {
    LUT_BW = 0,
    LUT_BWINV,
    LUT_SPECTTUM,
    LUT_HOTMETAL,
    LUT_GECOLORS,
    LUT_FLOW,
    LUT_LONI,
    LUT_LONI2,
    LUT_ASYMETRY,
    LUT_PVALUE,
    LUT_ROI
  };  
  //ETX
  
  static vtkLookupTableManager* New();
  vtkTypeRevisionMacro (vtkLookupTableManager, vtkObject);

  static vtkLookupTable* GetBWLookupTable (void);

  static vtkLookupTable* GetBWInverseLookupTable (void);

  static vtkLookupTable* GetSpectrumLookupTable (void);
  
  static vtkLookupTable* GetHotMetalLookupTable (void);
  
  static vtkLookupTable* GetGEColorLookupTable (void);
  
  static vtkLookupTable* GetFlowLookupTable (void);

  static vtkLookupTable* GetLONILookupTable (void);

  static vtkLookupTable* GetLONI2LookupTable (void);

  static vtkLookupTable* GetAsymmetryLookupTable (void);

  static vtkLookupTable* GetPValueLookupTable (void);
    
  static std::vector<std::string> GetAvailableLookupTables(void);

  static vtkLookupTable* GetROILookupTable (void);
    
  static vtkLookupTable* GetLookupTable(const int&);

 protected:
  vtkLookupTableManager();
  ~vtkLookupTableManager();
  
  

 private:
  

};



#endif
