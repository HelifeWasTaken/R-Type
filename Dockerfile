FROM fedora:34 AS build

RUN dnf install -y \
    glibc-devel \
    libglvnd-glx \
    libglvnd-devel \
    libX11-devel \
    libXrandr-devel \
    libXinerama-devel \
    libXi-devel \
    libfreetype6-devel \
    libglew1.5-devel \
    libjpeg8-devel \
    libsndfile1-devel \
    libopenal-devel \
    libudev-devel

RUN mkdir -p /usr/src
COPY . /usr/src
WORKDIR /usr/src
RUN ./build.sh

FROM fedora:34
RUN mkdir -p /data
COPY --from=build /usr/src/build/target/Server /data

EXPOSE 4242
EXPOSE 4243
CMD ["/data/Server/binary/r-type_server"]