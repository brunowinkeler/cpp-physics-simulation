FROM ghcr.io/brunowinkeler/devcontainer-cpp:latest

# Instalar dependências X11 e SDL3
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    libx11-dev \
    libxext-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxinerama-dev \
    libxi-dev \
    libxfixes-dev \
    libwayland-dev \
    libxkbcommon-dev \
    libdecor-0-dev \
    libegl1-mesa-dev \
    libgles2-mesa-dev \
    libgbm-dev \
    libdrm-dev \
    libpulse-dev \
    libasound2-dev \
    libudev-dev \
    libdbus-1-dev \
    libibus-1.0-dev \
    fcitx-libs-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

HEALTHCHECK NONE
