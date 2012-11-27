cmake_minimum_required(VERSION 2.8)


project(papara_epa_gui_tools)





SET( PAPARA_NO_IVYMIKE TRUE CACHE BOOL "tell papara that ivy_mike is present in super-project" FORCE)


add_subdirectory(ivy_mike)
add_subdirectory(papara_nt)

find_package(Qt4 4.4.3 REQUIRED QtCore QtGui QtScript )
include(${QT_USE_FILE})


IF(WIN32)
IF(NOT BOOST_ROOT)
set(BOOST_ROOT C:\\src\\boost_1_47_0)
ENDIF()

include_directories( ${BOOST_ROOT} )

include_directories( e:\\src\\boost_1_48_0)
file(GLOB IM_HEADERS *.h)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_SCL_SECURE_NO_WARNINGS") # as long as there is no support for std::array, these warnings are plain stupid!
ELSE()

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -msse3")

ENDIF()

include_directories(${IVYMIKE_INCLUDE} papara_nt papara_nt/ublasJama-1.0.2.3 )






 # with SET() command you can change variables or define new ones
  # here we define SAMPLE_SRCS variable that contains a list of all .cpp files
  # note that we don't need \ at the end of line
# SET( SAMPLE_SRCS
# #        dialog.cpp
#        main_widget.cpp
#        TextGrid.cpp
#        FileSelector.cpp
#   )

  # another list, this time it includes all header files that should be treated with moc
  SET( SAMPLE_MOC_HDRS
#        dialog.h
       PaparaMainWidget.h
       TextGrid.h
       FileSelector.h
       PhyloTreeView.h
       main.h
  )

  # some .ui files
  SET( SAMPLE_UIS
#        dialog.ui
       PaparaMainWidget.ui
  )


set( LARGALI_SRCS
	LargeAli.cpp
	TextGrid.cpp
	)

set( LARGALI_UIS
	LargeAli.ui
	)

set( LARGALI_MOC_HDRS
    LargeAli.h
	TextGrid.h
	)


set( EPA_VIEWER_SRCS
        EPAViewerWidget.cpp
        PhyloTreeView.cpp
        )

set( EPA_VIEWER_UIS
    EPAViewerWidget.ui
        )

set( EPA_VIEWER_MOC_HDRS
    EPAViewerWidget.h
    PhyloTreeView.h
)

# this will run uic on .ui files:
QT4_WRAP_UI( SAMPLE_UI_HDRS ${SAMPLE_UIS} )
QT4_WRAP_UI( LARGALI_UI_HDRS ${LARGALI_UIS} )
QT4_WRAP_UI( EPA_VIEWER_UI_HDRS ${EPA_VIEWER_UIS} )



# and finally this will run moc:
QT4_WRAP_CPP( SAMPLE_MOC_SRCS ${SAMPLE_MOC_HDRS} )
QT4_WRAP_CPP( LARGALI_MOC_SRCS ${LARGALI_MOC_HDRS} )
QT4_WRAP_CPP( EPA_VIEWER_MOC_SRCS ${EPA_VIEWER_MOC_HDRS} )


# we need this to be able to include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

# here we instruct CMake to build "sample" executable from all of the source files

add_executable( visual_papara WIN32 main.cpp PaparaMainWidget.cpp TextGrid.cpp FileSelector.cpp PhyloTreeView.cpp ${SAMPLE_MOC_SRCS} ${SAMPLE_UI_HDRS})
add_executable( largali largali_main.cpp LargeAli.cpp TextGrid.cpp ${LARGALI_MOC_SRCS} ${LARGALI_UI_HDRS})

add_executable( epa_viewer epa_viewer_main.cpp EPAViewerWidget.cpp PhyloTreeView.cpp ${EPA_VIEWER_MOC_SRCS} ${EPA_VIEWER_UI_HDRS})



target_link_libraries(visual_papara ${QT_LIBRARIES} papara_core ivymike ublas_jama  )
target_link_libraries(largali ${QT_LIBRARIES} ivymike ublas_jama )
target_link_libraries(epa_viewer ${QT_LIBRARIES} ivymike ublas_jama )


INSTALL_TARGETS( /bin visual_papara epa_viewer largali )

if(WIN32) 
    INSTALL(FILES
        "${QT_BINARY_DIR}/QtCore${QT_VERSION_MAJOR}.dll"
#        "${QT_BINARY_DIR}/QtXml${QT_VERSION_MAJOR}.dll"
#        "${QT_BINARY_DIR}/QtTest${QT_VERSION_MAJOR}.dll"
#        "${QT_BINARY_DIR}/QtWebKit${QT_VERSION_MAJOR}.dll"
        "${QT_BINARY_DIR}/QtGui${QT_VERSION_MAJOR}.dll"
#        "${QT_BINARY_DIR}/QtNetwork${QT_VERSION_MAJOR}.dll"
        "${QT_BINARY_DIR}/QtScript${QT_VERSION_MAJOR}.dll"
        DESTINATION bin
    )

	set(CPACK_NSIS_MENU_LINKS "bin/visual_papara" "Visual PaPaRa"
                          "bin/epa_viewer" "EPA jplace file viewer"
                          "bin/largali" "Large PHYLIP file viewer"
						  )
endif()
set( CPACK_PACKAGE_NAME visual_papara )
set( CPACK_PACKAGE_VERSION 2.3 )
set( CPACK_PACKAGE_CONTACT "simberger@gmail.com" )
#set( CPACK_DEBIAN_PACKAGE_DEPENDS


SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "VisualPaPaRa 2 GUI frontend")
SET(CPACK_PACKAGE_VENDOR "Simon A. Berger")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/INSTALL")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_PACKAGE_VERSION_MAJOR "2")
SET(CPACK_PACKAGE_VERSION_MINOR "3")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "simberger@gmail.com")

INCLUDE(CPack)