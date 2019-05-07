FROM alpine:3.9

# Arguments
ARG LIBREPCB_VERSION=0.1.0

# Install dependencies
RUN apk add --no-cache \
    g++ \
    make \
    qt5-qtbase \
    qt5-qtbase-dev \
    qt5-qttools-dev \
    wget \
    xvfb \
    zip \
    && set PATH="$PATH:/usr/lib/qt5/bin"

# Install LibrePCB
RUN wget "https://download.librepcb.org/releases/$LIBREPCB_VERSION/librepcb-$LIBREPCB_VERSION-source.zip" \
    && unzip "./librepcb-$LIBREPCB_VERSION-source.zip" \
    && cd "./librepcb-$LIBREPCB_VERSION" \
    && qmake-qt5 -r librepcb.pro \
    && make -j8 \
    && make install \
    && rm -rf ./librepcb-*

# Set working directory to /work so users don't have to change the working
# directory if they mount a volume to this path
WORKDIR /work

# Set entrypoint
ADD entrypoint.sh /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
