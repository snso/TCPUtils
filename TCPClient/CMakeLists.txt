#设置CMake支持的最低版本
cmake_minimum_required(VERSION 2.8)

# SET(PLATFORM "Linux")
# SET(ANDROID_ABI "x64")

SET(PLATFORM "Android")
SET(ANDROID_ABI "arm64-v8a")

SET(CMAKE_SYSTEM_NAME ${PLATFORM})

SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "set build type to Release")

SET(CMAKE_CXX_VISIBILITY_PRESET hidden)
SET(CMAKE_C_VISIBILITY_PRESET hidden)
SET(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

ADD_DEFINITIONS("-fPIC")
ADD_DEFINITIONS("-Wl,-Bsymbolic -lc -lm")
ADD_DEFINITIONS("-std=c++11")
	
#Choose Compiler
if(CMAKE_SYSTEM_NAME MATCHES "Android")
	SET(CMAKE_SYSTEM_VERSION 23)
	#choose Android ABI. Mine is armeabi-v7a
	#SET(CMAKE_ANDROID_ARCH_ABI armeabi-v7a) 	
	SET(CMAKE_ANDROID_ARCH_ABI ${ANDROID_ABI}) 
	
	# SET(NDK_STANDALONE_TOOLCHAIN /usr/ndk/android-ndk-r15c) 
	SET(NDK_STANDALONE_TOOLCHAIN /opt/android-ndk-r15c) 
	SET(CMAKE_ANDROID_NDK ${NDK_STANDALONE_TOOLCHAIN})
	SET(CMAKE_ANDROID_STL_TYPE gnustl_static)

	# SET(CMAKE_C_COMPILER arm-linux-androideabi-gcc) 
	# SET(CMAKE_CXX_COMPILER arm-linux-androideabi-g++) 

	

	SET(CMAKE_C_FLAGS "-fno-exceptions -fpermissive -nostdlib -fno-use-cxa-atexit -fPIE -pie  -fvisibility=hidden -D__STDINT_LIMITS -D__STDINT_MACROS -D__ANDROID__ -DANDROID")
	SET(CMAKE_CXX_FLAGS "-fno-exceptions -fpermissive -nostdlib -fno-use-cxa-atexit -fPIE -pie  -fvisibility=hidden -D__STDINT_LIMITS -D__STDINT_MACROS -D__ANDROID__ -DANDROID")

	set(ANDROID_LINKER_FLAGS "--sysroot=${ANDROID_SYSROOT}")
	add_definitions("--sysroot=${NDK_STANDALONE_TOOLCHAIN}/sysroot")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	SET(CMAKE_C_COMPILER "gcc")
	SET(CMAKE_CXX_COMPILER "g++")

	SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -fvisibility=hidden" CACHE TYPE STRING)
	SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -fvisibility=hidden" CACHE TYPE STRING)
endif()

PROJECT(TCPClientTest)


INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/TCPClient/Inc)


#FILE(GLOB_RECURSE LibFiles "*.h")
#ADD_CUSTOM_TARGET(headers SOURCES ${LibFiles})

#设置库路径
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/${PLATFORM}/${ANDROID_ABI}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/${PLATFORM}/${ANDROID_ABI}/bin)

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/TCPClient)

#source directory
SET(project_name runTCPClientTest)


#实现文件
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/Test MainSrcS)


AUX_SOURCE_DIRECTORY(./TCPTest SRS)
#add executable file
ADD_EXECUTABLE(${project_name} 	${SRS})

#add link library
TARGET_LINK_LIBRARIES(${project_name} YTCPClient)

