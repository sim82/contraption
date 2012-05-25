cmake_minimum_required(VERSION 2.8)
add_subdirectory(ivy_mike)
add_subdirectory(papara_nt)

find_package(Qt4 4.4.3 REQUIRED QtCore QtGui QtXml)
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

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -march=native")


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
       main_widget.h
       TextGrid.h
       FileSelector.h
       main.h
  )

  # some .ui files
  SET( SAMPLE_UIS
#        dialog.ui
       main_widget.ui
  )


set( LARGALI_SRCS
	large_ali.cpp
	TextGrid.cpp
	)

set( LARGALI_UIS
	large_ali.ui
	)

set( LARGALI_MOC_HDRS
    large_ali.h
	TextGrid.h
	)

# this will run uic on .ui files:
QT4_WRAP_UI( SAMPLE_UI_HDRS ${SAMPLE_UIS} )
QT4_WRAP_UI( LARGALI_UI_HDRS ${LARGALI_UIS} )

# and finally this will run moc:
QT4_WRAP_CPP( SAMPLE_MOC_SRCS ${SAMPLE_MOC_HDRS} )

QT4_WRAP_CPP( LARGALI_MOC_SRCS ${LARGALI_MOC_HDRS} )
# we need this to be able to include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

# here we instruct CMake to build "sample" executable from all of the source files

add_executable( test WIN32 main.cpp main_widget.cpp TextGrid.cpp FileSelector.cpp ${SAMPLE_MOC_SRCS} ${SAMPLE_UI_HDRS})
add_executable( largali largali_main.cpp large_ali.cpp TextGrid.cpp ${LARGALI_MOC_SRCS} ${LARGALI_UI_HDRS})


target_link_libraries(test ${QT_LIBRARIES} papara_core ivymike ublas_jama )
target_link_libraries(largali ${QT_LIBRARIES} ivymike ublas_jama )