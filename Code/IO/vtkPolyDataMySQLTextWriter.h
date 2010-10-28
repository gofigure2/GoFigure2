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

#ifndef __vtkPolyDataMySQLTextWriter_h
#define __vtkPolyDataMySQLTextWriter_h

#include <string>
#include <sstream>

#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkIdList.h"

#include "QGoIOConfigure.h"

/**
\defgroup MySQLWriter MySQLWriter
\defgroup Contour Contour
\defgroup Mesh Mesh
\defgroup Trace Trace
*/

/**
\class vtkPolyDataMySQLTextWriter
\brief Reads a string and convert it into a contour/mesh polydata
\ingroup MySQLWriter Contour Mesh Trace
*/

class QGOIO_EXPORT vtkPolyDataMySQLTextWriter:public vtkObject
{
public:
  /*
   * \brief Public constructor
   */
  static vtkPolyDataMySQLTextWriter * New();

  vtkTypeRevisionMacro(vtkPolyDataMySQLTextWriter, vtkObject);

  /*
   * \brief Generate a string from a contour/mesh plolydata
   * \param[in] iPolyData Polydata to generate the string
   * \return string containing the contour/mesh polydata information
   */
  std::string GetMySQLText(vtkPolyData *iPolyData);

  /*
   * \brief Get a contour or a mesh
   * \return true: generate a contour, false: generate a mesh
   */
  bool GetIsContour() const { return IsContour; }

protected:
  vtkPolyDataMySQLTextWriter();
  ~vtkPolyDataMySQLTextWriter();

  vtkPolyData *m_PolyData;
  bool IsContour;

  bool IsPlanarContour();

  std::string ContourProcessing();

  std::string MeshProcessing();

private:
  vtkPolyDataMySQLTextWriter(const vtkPolyDataMySQLTextWriter &);
  void operator=(const vtkPolyDataMySQLTextWriter &);
};
#endif
