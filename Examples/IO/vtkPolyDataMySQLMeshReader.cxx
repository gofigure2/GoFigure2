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
#include <string>
#include "vtkPolyDataReader.h"
#include "vtkPolyDataWriter.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMySQLMeshWriter.h"
#include "vtkPolyDataMySQLMeshReader.h"

int main(int argc, char **argv)
{
  if ( argc != 2 )
    {
    std::cout << "Usage:" << std::endl;
    std::cout << "./vtkPolyDataMySQLMeshWriter vtkfile" << std::endl;
    return EXIT_FAILURE;
    }

  vtkPolyDataReader *vtk_reader = vtkPolyDataReader::New();
  vtk_reader->SetFileName(argv[1]);
  vtk_reader->Update();

  vtkPolyData *input = vtk_reader->GetOutput();

  vtkPolyDataMySQLMeshWriter *convert_writer =
      vtkPolyDataMySQLMeshWriter::New();
  std::string polydata_string = convert_writer->GetMySQLText(input);

  vtkPolyDataMySQLMeshReader *convert_reader =
      vtkPolyDataMySQLMeshReader::New();
  vtkPolyData *output = vtkPolyData::New();
  output->DeepCopy(convert_reader->GetPolyData(polydata_string));

  if ( output->GetNumberOfPoints() != input->GetNumberOfPoints() )
    {
    std::cout << "Number of points have changed!!!" << std::endl;
    std::cout << "output->GetNumberOfPoints() " << output->GetNumberOfPoints()
              << std::endl;
    std::cout << "input->GetNumberOfPoints() " << input->GetNumberOfPoints()
              << std::endl;

    output->Delete();
    convert_reader->Delete();
    convert_writer->Delete();
    vtk_reader->Delete();

    return EXIT_FAILURE;
    }

  output->Delete();
  convert_reader->Delete();
  convert_writer->Delete();
  vtk_reader->Delete();

  return EXIT_SUCCESS;
}
