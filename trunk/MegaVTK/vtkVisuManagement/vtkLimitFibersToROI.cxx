/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkLimitFibersToROI.cxx 681 2008-02-15 18:33:09Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-02-15 13:33:09 -0500 (Fri, 15 Feb 2008) $
Version:   $Revision: 681 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkLimitFibersToROI.h"

#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkPolyData.h>
#include <vtkObjectFactory.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkUnsignedCharArray.h>

#include <algorithm>

extern int vtkrint(double a);

vtkCxxRevisionMacro(vtkLimitFibersToROI, "$Revision: 681 $");
vtkStandardNewMacro(vtkLimitFibersToROI);

vtkLimitFibersToROI::vtkLimitFibersToROI()
{
  MaskImage = 0;
  for( unsigned int i=0; i<256; i++)
  {
    this->BooleanOperationVector[i] = 1;
  }
}

vtkLimitFibersToROI::~vtkLimitFibersToROI()
{
  if( MaskImage )
    MaskImage->Delete();
}


int vtkLimitFibersToROI::RequestData (vtkInformation *vtkNotUsed(request),
                                       vtkInformationVector **inputVector,
                                       vtkInformationVector *outputVector)
{

  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  output->Initialize();
  output->Allocate();

  vtkPoints* points = input->GetPoints();
  output->SetPoints ( points );
  vtkDataArray* scalars = input->GetPointData()->GetScalars();
  output->GetPointData()->SetScalars ( scalars );

  for( int i=0; i<input->GetPointData()->GetNumberOfArrays(); i++)
  {
    output->GetPointData()->AddArray ( input->GetPointData()->GetArray (i) );
  }

  vtkUnsignedCharArray* allColors = vtkUnsignedCharArray::SafeDownCast ( input->GetCellData()->GetScalars() );

  vtkCellArray* lines = input->GetLines();
  if( lines==0 )
  {
    return 0;
  }

  if (MaskImage == 0)
  {
    output->SetLines (lines);
    if( allColors )
    {
      output->GetCellData()->SetScalars (allColors);
    }
    return 1;
  }


  vtkUnsignedCharArray* newColors = vtkUnsignedCharArray::New();
  newColors->SetNumberOfComponents (3);


  int* dim        = MaskImage->GetDimensions();
  double* origin  = MaskImage->GetOrigin();
  double* spacing = MaskImage->GetSpacing();

  /**
     Automatically extract labels (i.e. any scalar value except 0) contained
     in MaskImage.
  */
  std::vector<int> Labels;
  unsigned char*   MaskValues = (unsigned char*)MaskImage->GetScalarPointer();

  for (int i=0;i<MaskImage->GetNumberOfPoints();i++)
  {
    int val = (int)(*MaskValues);
    if (val>0)
    {
      bool IsInserted = false;
      for (unsigned int j=0; j<Labels.size(); j++)
      {
        if ( Labels[j]==val )
        {
          IsInserted = true;
          break;
        }
      }

      if ( !IsInserted )
      {
        Labels.push_back (val);
      }
    }

    ++MaskValues;
  }

  std::sort ( Labels.begin(), Labels.end() );
  unsigned int numLabels = Labels.size();

  std::cout << "Number Of Valid ROIs: " << numLabels << std::endl;



  // Test if the BooleanOperationVector size matches the max label
  if( Labels[numLabels-1]>255 )
  {
    vtkErrorMacro (<<"Error: The BoolearOperationVector variable has an incompatible length with the mask image.");
    newColors->Delete();
    return 0;
  }



  //reset the Scalar Pointer
  MaskValues= (unsigned char*)MaskImage->GetScalarPointer();


  /**
     Algorithm: For each fiber, we test the number of regions (i.e. labels)
     it passes through. Along with the BooleanOperationVector, we determine
     if the fiber should be retained or not.
  */

  lines->InitTraversal();

  vtkIdType  npts  = 0;
  vtkIdType* ptids = 0;
  vtkIdType   test = lines->GetNextCell (npts, ptids);
  int cellId = 0;

  char tmp[256];
  sprintf (tmp,"%d ROIs to process.", (int)Labels.size());
  this->SetProgressText (tmp);

  if( !Labels.size() )
  {
    vtkDebugMacro( << "There is no label to process." );
    return 1;
  }


  this->UpdateProgress (0.0);

  int iterProgress = 0;
  int maxProgress  = lines->GetNumberOfCells();
  int step         = maxProgress/100;



  // for All the Fibers
  while ( test )
  {

    //bool found = false;

    std::vector<int> FiberRegions (numLabels, 0);

    // for All the points of the fiber
    for (int k=0; k<npts; k++)
    {
      double pt[3];
      points->GetPoint (ptids[k], pt);

      int c[3];
//       c[0] = (int)( vtkrint( (pt[0]-origin[0])/spacing[0] ) );
//       c[1] = (int)( vtkrint( (pt[1]-origin[1])/spacing[1] ) );
//       c[2] = (int)( vtkrint( (pt[2]-origin[2])/spacing[2] ) );
      c[0] = static_cast<int>( (pt[0]-origin[0])/spacing[0] );
      c[1] = static_cast<int>( (pt[1]-origin[1])/spacing[1] );
      c[2] = static_cast<int>( (pt[2]-origin[2])/spacing[2] );

      if( c[0]>=0 && c[0]<dim[0] &&
          c[1]>=0 && c[1]<dim[1] &&
          c[2]>=0 && c[2]<dim[2] )
      {

        int index = c[0] + c[1]*dim[0] + c[2]*dim[0]*dim[1];
        MaskValues += index;
        int maskVal = (int)(*MaskValues);
        MaskValues -= index;


        for( unsigned int i=0; i<numLabels; i++)
        {
          if( maskVal==Labels[i] )
          {
            FiberRegions[i]=1;
            break;
          }
        }


      }
    }

    bool insert = true;

    for( unsigned int i=0; i<numLabels; i++)
    {
      if( this->BooleanOperationVector[ Labels[i] ] == 0 )
      {
        insert = insert && !FiberRegions[i];
      }
      else
      {
        insert = insert && FiberRegions[i];
      }
    }


    if (insert)
    {
      output->InsertNextCell(VTK_POLY_LINE, npts, ptids);
      if( allColors )
      {
        unsigned char fiberColor[3];
        allColors->GetTupleValue ( cellId, fiberColor );
        newColors->InsertNextValue (fiberColor[0]);
        newColors->InsertNextValue (fiberColor[1]);
        newColors->InsertNextValue (fiberColor[2]);
      }

    }

    test = lines->GetNextCell (npts, ptids);
    cellId++;

    iterProgress++;
    if( step>0 )
    {
      if ( !(iterProgress%step) )
      {
        this->UpdateProgress (double(iterProgress)/double(maxProgress));
      }
    }

  }

  if( allColors )
  {
    output->GetCellData()->SetScalars (newColors);
  }
  newColors->Delete();

  this->UpdateProgress (1.0);


  return 1;

}
