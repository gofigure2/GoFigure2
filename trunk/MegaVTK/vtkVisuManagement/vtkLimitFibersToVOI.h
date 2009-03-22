/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkLimitFibersToVOI.h 608 2008-01-14 08:21:23Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-01-14 03:21:23 -0500 (Mon, 14 Jan 2008) $
Version:   $Revision: 608 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtk_LimitFibersToVOI_h_
#define _vtk_LimitFibersToVOI_h_

#include "vtkPolyDataAlgorithm.h"
#include <vector>

class vtkLimitFibersToVOI: public vtkPolyDataAlgorithm
{

 public:
  static vtkLimitFibersToVOI *New();
  vtkTypeRevisionMacro(vtkLimitFibersToVOI, vtkPolyDataAlgorithm);
    
  void PrintSelf (ostream& os, vtkIndent indent){};
  
  
  /** Set the VOI */
  void SetVOI (const int&, const int&, const int&,
               const int&, const int&, const int&);


  /**
     Set the boolean operator: switch between concatenating the fibers (AND - 1)
     and removing the fibers (NOT - 0).
   */
  void SetBooleanOperation (int n)
  {
    this->BooleanOperation = n;
  }
  

  /**
     Set the boolean operator: switch between concatenating the fibers (AND - 0)
     and removing the fibers (NOT - 1).
   */
  void SetBooleanOperationToAND (void)
  {
    this->BooleanOperation = 1;
  }
  

  /**
     Set the boolean operator: switch between concatenating the fibers (AND - 0)
     and removing the fibers (NOT - 1).
   */
  void SetBooleanOperationToNOT (void)
  {
    this->BooleanOperation = 0;
  }

  vtkGetMacro (BooleanOperation, int);
  
  
 protected:
  vtkLimitFibersToVOI();
  ~vtkLimitFibersToVOI(){};
  
  // Usual data generation method
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  

 private:
  vtkLimitFibersToVOI (const vtkLimitFibersToVOI&);
  void operator=(const vtkLimitFibersToVOI&);

  int BooleanOperation;

  int m_XMin;
  int m_XMax;
  int m_YMin;
  int m_YMax;
  int m_ZMin;
  int m_ZMax;
  
};



#endif
