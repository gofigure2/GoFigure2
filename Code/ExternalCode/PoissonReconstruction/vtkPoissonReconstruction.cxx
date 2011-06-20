/*=========================================================================
 Modifications were made by the GoFigure Dev. Team.
 while at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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

/*=========================================================================
 Authors: David Doria at Rensselaer Polytechnic Institute and
   Arnaud Gelas at Harvard Medical School

 Copyright (c) 2010, David Doria at Rensselaer Polytechnic Institute and
   Arnaud Gelas at Harvard Medical School,
   All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 Neither the name of the Rensselaer Polytechnic Institute and of Harvard
 Medical School nor the names of its contributors may be used to endorse
 or promote products derived from this software without specific prior
 written permission.

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
#include "vtkPoissonReconstruction.h"

#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkDataObject.h"
#include "vtkSmartPointer.h"
#include "vtkTriangle.h"
#include "vtkCellArray.h"

#include "MultiGridOctest.h"
#include "MultiGridOctreeData.h"

vtkStandardNewMacro(vtkPoissonReconstruction);

vtkPoissonReconstruction::vtkPoissonReconstruction()
{
  this->Depth = 8;
  this->KernelDepth = 6;

  this->SolverDivide = 8;
  this->IsoDivide = 8;
  this->Refine = 3;

  this->SamplesPerNode = 1.0;
  this->Scale = 1.25;

  this->Verbose = 0;
  this->Confidence = 0;

  this->NoResetSamples = 0;
  this->NoClipTree = 0;
  this->Refine = 0;
}

int vtkPoissonReconstruction::RequestData(vtkInformation *vtkNotUsed(request),
                                          vtkInformationVector **inputVector,
                                          vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkPolyData *input = vtkPolyData::SafeDownCast(
      inInfo->Get( vtkDataObject::DATA_OBJECT() ) );

  vtkPolyData *output = vtkPolyData::SafeDownCast(
      outInfo->Get( vtkDataObject::DATA_OBJECT() ) );

  const int Degree = 2;

  Point3D< float >      center;
  Real                  scale = 1.0;
  Real                  isoValue = 0;
  Octree< Degree >      tree;
  PPolynomial< Degree > reconstructionFunction = PPolynomial< Degree >::GaussianApproximation();

  center.coords[0] = 0;
  center.coords[1] = 0;
  center.coords[2] = 0;

  TreeOctNode::SetAllocator(MEMORY_ALLOCATOR_BLOCK_SIZE);

  this->KernelDepth = this->Depth - 2;

  tree.setFunctionData( reconstructionFunction, this->Depth, 0,
                        Real(1.0) / ( 1 << this->Depth ) );

  //DumpOutput("Function Data Set In: %lg\n",Time()-t);
  //size_t memoryusage = MemoryInfo::Usage();
  //DumpOutput("Memory Usage: %.3f MB\n",float(memoryusage)/(1<<20));
  if ( this->KernelDepth > this->Depth )
    {
    fprintf(stderr, "KernelDepth can't be greater than Depth: %d <= %d\n", this->KernelDepth, this->Depth);
    return EXIT_FAILURE;
    }

  tree.setTree(input, this->Depth, this->KernelDepth, Real(this->SamplesPerNode),
               this->Scale, center, scale, !this->NoResetSamples,
               this->Confidence);

  //DumpOutput("Leaves/Nodes: %d/%d\n",tree.tree.leaves(),tree.tree.nodes());
  //DumpOutput("   Tree Size: %.3f
  // MB\n",float(sizeof(TreeOctNode)*tree.tree.nodes())/(1<<20));
  //DumpOutput("Memory Usage: %.3f MB\n",float(MemoryInfo::Usage())/(1<<20));

  if ( !NoClipTree )
    {
    tree.ClipTree();
    //DumpOutput("Tree Clipped In: %lg\n",Time()-t);
    //DumpOutput("Leaves/Nodes: %d/%d\n",tree.tree.leaves(),tree.tree.nodes());
    //DumpOutput("   Tree Size: %.3f
    // MB\n",float(sizeof(TreeOctNode)*tree.tree.nodes())/(1<<20));
    }

  tree.finalize1(this->Refine);
  //DumpOutput("Finalized 1 In: %lg\n",Time()-t);
  //DumpOutput("Leaves/Nodes: %d/%d\n",tree.tree.leaves(),tree.tree.nodes());
  //DumpOutput("Memory Usage: %.3f MB\n",float(MemoryInfo::Usage())/(1<<20));

//  tree.maxMemoryUsage=0;
  tree.SetLaplacianWeights();
  //DumpOutput("Memory Usage: %.3f MB\n",float(MemoryInfo::Usage())/(1<<20));

  tree.finalize2(this->Refine);
  //DumpOutput("Finalized 2 In: %lg\n",Time()-t);
  //DumpOutput("Leaves/Nodes: %d/%d\n",tree.tree.leaves(),tree.tree.nodes());
  //DumpOutput("Memory Usage: %.3f MB\n",float(MemoryInfo::Usage())/(1<<20));

  //tree.maxMemoryUsage=0;
  tree.LaplacianMatrixIteration(this->SolverDivide);
  //DumpOutput("Memory Usage: %.3f MB\n",float(MemoryInfo::Usage())/(1<<20));

  CoredVectorMeshData mesh;
  //tree.maxMemoryUsage=0;
  isoValue = tree.GetIsoValue();
  //DumpOutput("Got average in: %f\n",Time()-t);
  //DumpOutput("Iso-Value: %e\n",isoValue);
  //DumpOutput("Memory Usage: %.3f MB\n",float(tree.MemoryUsage()));

  if ( this->IsoDivide )
    {
    tree.GetMCIsoTriangles(isoValue, this->IsoDivide, &mesh, 0, 1, true);
    }
  else
    {
    tree.GetMCIsoTriangles(isoValue, &mesh, 0, 1, true);
    }

  //PlyWriteTriangles(Out.value,&mesh,PLY_BINARY_NATIVE,center,scale,comments,commentNum);

  //create output
  vtkSmartPointer< vtkPoints > points =
    vtkSmartPointer< vtkPoints >::New();
  Point3D< float > p;

  for ( size_t i = 0; i < mesh.inCorePoints.size(); i++ )
    {
    p = mesh.inCorePoints[i];
    points->InsertNextPoint(p.coords[0] * scale + center.coords[0],
                            p.coords[1] * scale + center.coords[1],
                            p.coords[2] * scale + center.coords[2]);
    }
  for ( unsigned int i = 0; i < static_cast< unsigned int >( mesh.outOfCorePointCount() ); i++ )
    {
    mesh.nextOutOfCorePoint(p);
    points->InsertNextPoint(p.coords[0] * scale + center.coords[0],
                            p.coords[1] * scale + center.coords[1],
                            p.coords[2] * scale + center.coords[2]);
    }

  // write faces

  vtkSmartPointer< vtkCellArray > triangles =
    vtkSmartPointer< vtkCellArray >::New();

  TriangleIndex tIndex;
  int           inCoreFlag;

  unsigned int nr_faces = mesh.triangleCount();
  for ( unsigned int i = 0; i < nr_faces; i++ )
    {
    vtkSmartPointer< vtkTriangle > triangle =
      vtkSmartPointer< vtkTriangle >::New();

    mesh.nextTriangle(tIndex, inCoreFlag);

    if ( !( inCoreFlag & CoredMeshData::IN_CORE_FLAG[0] ) )
      {
      tIndex.idx[0] += int( mesh.inCorePoints.size() );
      }
    if ( !( inCoreFlag & CoredMeshData::IN_CORE_FLAG[1] ) )
      {
      tIndex.idx[1] += int( mesh.inCorePoints.size() );
      }
    if ( !( inCoreFlag & CoredMeshData::IN_CORE_FLAG[2] ) )
      {
      tIndex.idx[2] += int( mesh.inCorePoints.size() );
      }

    for ( unsigned int j = 0; j < 3; j++ )
      {
      triangle->GetPointIds()->SetId(j, tIndex.idx[j]);
      }

    triangles->InsertNextCell(triangle);
    }

  output->SetPoints(points);
  output->SetPolys(triangles);

  return 1;
}

//----------------------------------------------------------------------------
void vtkPoissonReconstruction::PrintSelf(ostream & os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}