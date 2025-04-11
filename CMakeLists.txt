cmake_minimum_required(VERSION 3.17)
project(MOMProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# 🔗 Usa vcpkg toolchain
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/../vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "Vcpkg toolchain file")
endif()

# 📦 Requiere Protobuf y gRPC
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

# 📁 Archivos .proto
file(GLOB PROTO_FILES "${CMAKE_SOURCE_DIR}/proto/*.proto")

# 📂 Carpeta de archivos generados
set(GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${GENERATED_DIR})

# 🔧 Generar archivos .pb y .grpc.pb manualmente
set(PROTO_SRCS "")
set(PROTO_HDRS "")

foreach(proto_file ${PROTO_FILES})
  get_filename_component(proto_name ${proto_file} NAME_WE)

  execute_process(
    COMMAND protoc
    --proto_path=${CMAKE_SOURCE_DIR}/proto
    --cpp_out=${GENERATED_DIR}
    ${proto_file}
  )

  execute_process(
    COMMAND protoc
    --proto_path=${CMAKE_SOURCE_DIR}/proto
    --grpc_out=${GENERATED_DIR}
    --plugin=protoc-gen-grpc=${CMAKE_SOURCE_DIR}/../grpc/cmake/build/grpc_cpp_plugin
    ${proto_file}
  )

  list(APPEND PROTO_SRCS
    ${GENERATED_DIR}/${proto_name}.pb.cc
    ${GENERATED_DIR}/${proto_name}.grpc.pb.cc
  )
  list(APPEND PROTO_HDRS
    ${GENERATED_DIR}/${proto_name}.pb.h
    ${GENERATED_DIR}/${proto_name}.grpc.pb.h
  )
endforeach()

# 📁 Archivos fuente principales
file(GLOB_RECURSE SRC_FILES src/*.cpp)

# 🧠 Incluir encabezados
include_directories(
    include
    ${GENERATED_DIR}
    ${Protobuf_INCLUDE_DIRS}
    ${PQXX_INCLUDE_DIRS}
)

link_directories(${PQXX_LIBRARY_DIRS})
add_definitions(${PQXX_CFLAGS_OTHER})

# 🧱 Ejecutable
add_executable(mom
    ${SRC_FILES}
    ${PROTO_SRCS}
    ${PROTO_HDRS}
)

# 🔗 Linkeo de librerías necesarias
target_link_libraries(mom
    protobuf::libprotobuf
    gRPC::grpc++
    gRPC::grpc
    OpenSSL::Crypto
    ${PQXX_LIBRARIES}
    pthread
)

message(STATUS "✅ Proyecto MOM configurado y listo para compilar.")
