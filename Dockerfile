# Base image for msbuild (uses Mono)
FROM ubuntu:17.10

WORKDIR /home/ci/

# Update and install cross-platform build dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \    
    libsdl2-dev \
    libsdl2-mixer-dev \
    libglfw3-dev \
    && rm -rf /var/lib/apt/lists/*


