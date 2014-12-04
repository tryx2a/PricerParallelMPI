cmake_minimum_required(VERSION 2.8)
project(CPF C CXX)

# Prefix
if (NOT PREFIX)
    set(PREFIX ${CMAKE_CURRENT_BINARY_DIR})
endif (NOT PREFIX)
set(CMAKE_INSTALL_PREFIX ${PREFIX} CACHE STRING "Installation prefix." FORCE)

if (CXX)
    set(CMAKE_CXX_COMPILER ${CXX})
endif (CXX)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Release or Debug
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif ()


# Detect MPI
set(MPI_LIBS)
find_package(MPI REQUIRED)
if (MPI_C_FOUND AND MPI_CXX_FOUND)
    include_directories(${MPI_C_INCLUDE_PATH})
    include_directories(${MPI_CXX_INCLUDE_PATH})
    set (MPI_LIBS ${MPI_LIBS} ${MPI_C_LIBRARIES})
    set (MPI_LIBS ${MPI_LIBS} ${MPI_CXX_LIBRARIES})
else (MPI_C_FOUND AND MPI_CXX_FOUND)
    message(STATUS "MPI not found.")
endif (MPI_C_FOUND AND MPI_CXX_FOUND)

# Detect OPM
find_package(OpenMP QUIET)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS ${CMAKE_C_CFLAGS} ${OpenMP_C_FLAGS})
    set (CMAKE_CXX_FLAGS ${CMAKE_CXX_CFLAGS} ${OpenMP_CXX_FLAGS})
else (OPENMP_FOUND)
    message(STATUS "OpenMP not found.")
endif (OPENMP_FOUND)

# Detect PNL
find_package(Pnl)
set(LIBS ${LIBS} ${PNL_LIBRARIES})
include_directories(${PNL_INCLUDE_DIRS})

# Add math lib
set(LIBS ${LIBS} m)


add_executable(main src/parser.cpp src/utils.cpp src/option.cpp src/optionBarrierLow.cpp src/optionBarrierUp.cpp src/optionAsian.cpp src/optionBasket.cpp src/optionBarrier.cpp src/optionPerformance.cpp src/bs.cpp src/mc.cpp src/main.cpp )
target_link_libraries(main ${MPI_LIBS} ${LIBS})

#add_executable(main_test src/main_test.cpp )
#target_link_libraries(main_test ${MPI_LIBS} ${LIBS})


add_executable(main_pack_pnl src/main_packing_pnl.cpp )
target_link_libraries(main_pack_pnl ${MPI_LIBS} ${LIBS})


#add_executable(main_pack_b src/main_pack_basic.cpp )
#target_link_libraries(main_pack_b ${MPI_LIBS} ${LIBS})

add_executable(main_bs src/main_mpi_bs.cpp src/bs.cpp src/utils.cpp src/option.cpp src/optionBarrierLow.cpp src/optionBarrierUp.cpp src/optionAsian.cpp src/optionBasket.cpp src/optionBarrier.cpp src/optionPerformance.cpp src/mc.cpp src/parser.cpp)
target_link_libraries(main_bs ${MPI_LIBS} ${LIBS})