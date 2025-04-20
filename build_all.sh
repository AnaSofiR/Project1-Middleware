#!/bin/bash

# Ruta al plugin grpc_cpp_plugin
GRPC_PLUGIN="$HOME/vcpkg/installed/x64-linux/tools/grpc/grpc_cpp_plugin"

# Ruta al plugin protoc
PROTOC_BIN="$HOME/vcpkg/installed/x64-linux/tools/protobuf/protoc"

# Verificar que el plugin existe
if [ ! -f "$GRPC_PLUGIN" ]; then
  echo "❌ No se encontró grpc_cpp_plugin en: $GRPC_PLUGIN"
  echo "➡️ Ejecuta: ./vcpkg install grpc"
  exit 1
fi

echo "📁 Creando carpeta de archivos generados..."
mkdir -p build/generated

# Lista de archivos .proto a procesar
PROTO_FILES=("proto/mom.proto" "proto/replication.proto")

for proto in "${PROTO_FILES[@]}"; do
  echo "🧪 Procesando: $proto"

  $PROTOC_BIN -I=proto "$proto" \
    --cpp_out=build/generated \
    --grpc_out=build/generated \
    --plugin=protoc-gen-grpc="$GRPC_PLUGIN"

  if [ $? -ne 0 ]; then
    echo "❌ Error generando archivos para: $proto"
    exit 1
  fi
done

echo "✅ Archivos .pb.h y .grpc.pb.h generados correctamente."
ls build/generated/*.h
