#ifndef __itkMegaCaptureImport_h__
#define __itkMegaCaptureImport_h__

#include <vector>
#include <algorithm>

#include "itkLightProcessObject.h"

#include <itksys/SystemTools.hxx>
#include "itkRegularExpressionSeriesFileNames.h"

#include "GoFigureFileInfoHelper.h"

namespace itk
{

class ITK_EXPORT MegaCaptureImport: public LightProcessObject
{
public:
  typedef std::vector < int >         IntVectorType;
  typedef std::vector < std::string > StringVectorType;

   /** Standard class typedefs.      */
  typedef MegaCaptureImport Self;

  typedef LightProcessObject Superclass;

  typedef SmartPointer< Self >  Pointer ;
  typedef SmartPointer< const Self >  ConstPointer ;

  /** Run-time type information (and related methods). */
  itkTypeMacro( MegaCaptureImport, LightProcessObject ) ;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  void SetFileName( char * name );

  void Glob();

  void CreateOutput();

  FileListType* GetOutput() { return( &(this->m_OutputFileList) ); };

  void Update(void) { Glob(); CreateOutput(); };

protected:
  MegaCaptureImport() { this->m_FileName = NULL; };
  ~MegaCaptureImport();


private:
  MegaCaptureImport (const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  IntVectorType    m_numGroupStart;
  IntVectorType    m_numGroupLength;
  StringVectorType m_FileNameS;
  FileListType     m_OutputFileList;
  char*            m_FileName;

};

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMegaCaptureImport.txx"
#endif

}

#endif
