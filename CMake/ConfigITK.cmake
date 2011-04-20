#----------------------------------------------------------
FIND_PACKAGE( ITK REQUIRED )
  IF( ITK_FOUND )
  #----------------------------------------------------------
  INCLUDE( ${ITK_USE_FILE} )

  #----------------------------------------------------------
  # Check the version of ITK
  # ( by checking the existence of a variable
  # can't check version: 4.0.0 is modularized and old style...)
  #----------------------------------------------------------
  IF( NOT DEFINED ITK_MODULES_ENABLED )
    MESSAGE( SEND_ERROR "WRONG VERSION OF ITK, REQUIERES MODULARIZED ITK, CMAKE WILL STOP NOW")
  ENDIF ( NOT DEFINED ITK_MODULES_ENABLED )

  # check if requiered modules are enabled
 list(FIND ITK_MODULES_ENABLED ITK-Common ITK-Common_FOUND)
 IF( NOT ITK-Common_FOUND )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-Common turned ON")
 ENDIF( NOT ITK-Common_FOUND  )
 
 list(FIND ITK_MODULES_ENABLED ITK-Review ITK-Review_FOUND)
 IF( NOT ITK-Review_FOUND )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-Review turned ON")
 ENDIF( NOT ITK-Review_FOUND  )

list(FIND ITK_MODULES_ENABLED ITK-IO-Base ITK-IO-Base_FOUND)
 IF( NOT ITK-IO-Base_FOUND )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-Base turned ON")
 ENDIF( NOT ITK-IO-Base_FOUND  )

 list(FIND ITK_MODULES_ENABLED ITK-IO-JPEG ITK-IO-JPEG_FOUND)
 IF( NOT ITK-IO-JPEG_FOUND )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-JPEG turned ON")
 ENDIF( NOT ITK-IO-JPEG_FOUND  )

 list(FIND ITK_MODULES_ENABLED ITK-IO-BMP ITK-IO-BMP_FOUND)
 IF( NOT ITK-IO-BMP_FOUND )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-BMP turned ON")
 ENDIF( NOT ITK-IO-BMP_FOUND  )

 list(FIND ITK_MODULES_ENABLED ITK-IO-GDCM ITK-IO-GDCM_FOUND)
 IF( NOT ITK-IO-GDCM_FOUND )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-GDCM turned ON")
 ENDIF( NOT ITK-IO-GDCM_FOUND  )

 list(FIND ITK_MODULES_ENABLED ITK-IO-LSM ITK-IO-LSM_FOUND)
 IF( NOT ITK-IO-LSM_FOUND )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-LSM turned ON")
 ENDIF( NOT ITK-IO-LSM_FOUND  )

 list(FIND ITK_MODULES_ENABLED ITK-IO-PNG ITK-IO-PNG_FOUND)
 IF( NOT ITK-IO-PNG_FOUND )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-PNG turned ON")
 ENDIF( NOT ITK-IO-PNG_FOUND  )

 list(FIND ITK_MODULES_ENABLED ITK-IO-TIFF ITK-IO-TIFF_FOUND)
 IF( NOT ITK-IO-TIFF_FOUND )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-TIFF turned ON")
 ENDIF( NOT ITK-IO-TIFF_FOUND  )

 list(FIND ITK_MODULES_ENABLED ITK-IO-VTK ITK-IO-VTK_FOUND)
 IF( NOT ITK-IO-VTK_FOUND )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-VTK turned ON")
 ENDIF( NOT ITK-IO-VTK_FOUND  )

 list(FIND ITK_MODULES_ENABLED ITK-IO-Stimulate ITK-IO-Stimulate_FOUND)
 IF( NOT ITK-IO-Stimulate_FOUND )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-Stimulate turned ON")
 ENDIF( NOT ITK-IO-Stimulate_FOUND  )

 list(FIND ITK_MODULES_ENABLED ITK-IO-BioRad ITK-IO-BioRad_FOUND)
 IF( NOT ITK-IO-BioRad_FOUND )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-BioRad turned ON")
 ENDIF( NOT ITK-IO-BioRad_FOUND  )

 list(FIND ITK_MODULES_ENABLED ITK-IO-Meta ITK-IO-Meta_FOUND)
 IF( NOT ITK-IO-Meta_FOUND )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-Meta turned ON")
 ENDIF( NOT ITK-IO-Meta_FOUND  )

 list(FIND ITK_MODULES_ENABLED ITK-IO-NIFTI ITK-IO-NIFTI_FOUND)
 IF( NOT ITK-IO-NIFTI_FOUND )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-NIFTI turned ON")
 ENDIF( NOT ITK-IO-NIFTI_FOUND  )

 list(FIND ITK_MODULES_ENABLED ITK-IO-NRRD ITK-IO-NRRD_FOUND)
 IF( NOT ITK-IO-NRRD_FOUND )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-NRRD turned ON")
 ENDIF( NOT ITK-IO-NRRD_FOUND  )
 
 list(FIND ITK_MODULES_ENABLED ITK-IO-GIPL ITK-IO-GIPL_FOUND)
 IF( NOT ITK-IO-GIPL_FOUND )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-GIPL turned ON")
 ENDIF( NOT ITK-IO-GIPL_FOUND  )

  set (ITK-IO  "ITK-IO-Base"
               "ITK-IO-JPEG"
               "ITK-IO-BMP"
               "ITK-IO-GDCM"
	       "ITK-IO-LSM"
	       "ITK-IO-PNG"
	       "ITK-IO-TIFF"
	       "ITK-IO-VTK"
	       "ITK-IO-Stimulate"
	       "ITK-IO-BioRad"
	       "ITK-IO-Meta"
	       "ITK-IO-NIFTI"
	       "ITK-IO-NRRD"
	       "ITK-IO-GIPL"
               # MRC is in review....
               #"ITK-IO-MRC"
               # test IO review
               "ITK-Review"
               
)

ELSE( ITK_FOUND )
    MESSAGE( SEND_ERROR "ITK NOT FOUND, CMAKE WILL STOP NOW")
ENDIF( ITK_FOUND )
