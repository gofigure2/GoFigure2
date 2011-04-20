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

MESSAGE(STATUS "${ITK_MODULES_ENABLED}")

  # check if requiered modules are enabled
 list(FIND ITK_MODULES_ENABLED ITK-Common ITK-Common_FOUND)
 IF( ITK-Common_FOUND LESS 0 )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-Common turned ON")
 ENDIF( ITK-Common_FOUND LESS 0  )

 list(FIND ITK_MODULES_ENABLED ITK-Review ITK-Review_FOUND)
 IF( ITK-Review_FOUND LESS 0 )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-Review turned ON")
 ENDIF( ITK-Review_FOUND LESS 0 )

list(FIND ITK_MODULES_ENABLED ITK-IO-Base ITK-IO-Base_FOUND)
 IF( ITK-IO-Base_FOUND LESS 0 )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-Base turned ON")
 ENDIF( ITK-IO-Base_FOUND LESS 0 )

 list(FIND ITK_MODULES_ENABLED ITK-IO-JPEG ITK-IO-JPEG_FOUND)
 IF( ITK-IO-JPEG_FOUND LESS 0 )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-JPEG turned ON")
 ENDIF( ITK-IO-JPEG_FOUND LESS 0 )

 list(FIND ITK_MODULES_ENABLED ITK-IO-BMP ITK-IO-BMP_FOUND)
 IF( ITK-IO-BMP_FOUND LESS 0 )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-BMP turned ON")
 ENDIF( ITK-IO-BMP_FOUND LESS 0 )

 list(FIND ITK_MODULES_ENABLED ITK-IO-GDCM ITK-IO-GDCM_FOUND)
 IF( ITK-IO-GDCM_FOUND LESS 0 )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-GDCM turned ON")
 ENDIF( ITK-IO-GDCM_FOUND LESS 0 )

 list(FIND ITK_MODULES_ENABLED ITK-IO-LSM ITK-IO-LSM_FOUND)
 IF( ITK-IO-LSM_FOUND LESS 0 )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-LSM turned ON")
 ENDIF( ITK-IO-LSM_FOUND LESS 0 )

 list(FIND ITK_MODULES_ENABLED ITK-IO-PNG ITK-IO-PNG_FOUND)
 IF( ITK-IO-PNG_FOUND LESS 0 )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-PNG turned ON")
 ENDIF( ITK-IO-PNG_FOUND LESS 0 )

 list(FIND ITK_MODULES_ENABLED ITK-IO-TIFF ITK-IO-TIFF_FOUND)
 IF( ITK-IO-TIFF_FOUND LESS 0 )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-TIFF turned ON")
 ENDIF( ITK-IO-TIFF_FOUND LESS 0 )

 list(FIND ITK_MODULES_ENABLED ITK-IO-VTK ITK-IO-VTK_FOUND)
 IF( ITK-IO-VTK_FOUND LESS 0 )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-VTK turned ON")
 ENDIF( ITK-IO-VTK_FOUND LESS 0 )

 list(FIND ITK_MODULES_ENABLED ITK-IO-Stimulate ITK-IO-Stimulate_FOUND)
 IF( ITK-IO-Stimulate_FOUND LESS 0 )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-Stimulate turned ON")
 ENDIF( ITK-IO-Stimulate_FOUND LESS 0 )

 list(FIND ITK_MODULES_ENABLED ITK-IO-BioRad ITK-IO-BioRad_FOUND)
 IF( ITK-IO-BioRad_FOUND LESS 0 )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-BioRad turned ON")
 ENDIF( ITK-IO-BioRad_FOUND LESS 0 )

 list(FIND ITK_MODULES_ENABLED ITK-IO-Meta ITK-IO-Meta_FOUND)
 IF( ITK-IO-Meta_FOUND LESS 0 )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-Meta turned ON")
 ENDIF( ITK-IO-Meta_FOUND LESS 0 )

 list(FIND ITK_MODULES_ENABLED ITK-IO-NIFTI ITK-IO-NIFTI_FOUND)
 IF( ITK-IO-NIFTI_FOUND LESS 0 )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-NIFTI turned ON")
 ENDIF( ITK-IO-NIFTI_FOUND LESS 0 )

 list(FIND ITK_MODULES_ENABLED ITK-IO-NRRD ITK-IO-NRRD_FOUND)
 IF( ITK-IO-NRRD_FOUND LESS 0 )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-NRRD turned ON")
 ENDIF(ITK-IO-NRRD_FOUND LESS 0 )
 
 list(FIND ITK_MODULES_ENABLED ITK-IO-GIPL ITK-IO-GIPL_FOUND)
 IF(ITK-IO-GIPL_FOUND LESS 0 )
   MESSAGE( SEND_ERROR "GoFigure2 requires ITK to be compiled with ITK-IO-GIPL turned ON")
 ENDIF( ITK-IO-GIPL_FOUND LESS 0 )

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
