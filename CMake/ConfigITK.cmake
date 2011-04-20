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

  # Check if "REVIEW" is "ON"
  IF( ${ITK_USE_REVIEW} MATCHES "OFF" )
    MESSAGE( SEND_ERROR
      "GoFigure2 requires ITK to be compiled with Module_ITK-Review turned ON" )
  ENDIF( ${ITK_USE_REVIEW} MATCHES "OFF" )

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
