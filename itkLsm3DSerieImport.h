#ifndef __itkLsm3DSerieImport_h__
#define __itkLsm3DSerieImport_h__

#include <vector>
#include <algorithm>

#include "itkLightProcessObject.h"

#include <itksys/SystemTools.hxx>
#include "itkRegularExpressionSeriesFileNames.h"

#include "FileInfo.h"

namespace itk
{
	
class ITK_EXPORT Lsm3DSerieImport: public LightProcessObject
{
public:
  typedef std::vector < int >         IntVectorType;
  typedef std::vector < std::string > StringVectorType;
		
  /** Standard class typedefs.      */
  typedef Lsm3DSerieImport Self;
	
  typedef LightProcessObject Superclass;
		
  typedef SmartPointer< Self >  Pointer ;
  typedef SmartPointer< const Self >  ConstPointer ;
		
  /** Run-time type information (and related methods). */
  itkTypeMacro( Lsm3DSerieImport, LightProcessObject ) ;
		
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
		
  void SetFileName( char * name );
		
  FileListType* GetOutput();
		
  void Update(void);
 
  void SetGroupId( int Uservalue );
		
protected:
  void Glob();
		
  void CreateOutput();

  Lsm3DSerieImport() { this->m_FileName = NULL; this->m_GroupId = 0; };
  ~Lsm3DSerieImport();
		
private:
  Lsm3DSerieImport (const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
		
  IntVectorType    m_numGroupStart;
  IntVectorType    m_numGroupLength;
  int              m_GroupId;
  StringVectorType m_FileNameS;
  FileListType     m_OutputFileList;
  char*            m_FileName;
	
};
	
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLsm3DSerieImport.txx"
#endif

}

#endif
