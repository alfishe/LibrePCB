FROM alpine:3.9

# Arguments
ARG LIBREPCB_VERSION=0.1.1

# Install dependencies
RUN apk add --no-cache g++ make qt5-qtbase qt5-qtbase-dev qt5-qttools-dev wget zip
ENV PATH="$PATH:/usr/lib/qt5/bin"

# Install LibrePCB
RUN wget "https://download.librepcb.org/releases/$LIBREPCB_VERSION/librepcb-$LIBREPCB_VERSION-source.zip" \
    && unzip "./librepcb-$LIBREPCB_VERSION-source.zip" \
    && cd "./librepcb-$LIBREPCB_VERSION" \
    && qmake-qt5 -r librepcb.pro \
    && make -j8 \
    && make install \
    && rm -rf ./librepcb-*

RUN apk add --no-cache xvfb
ENV DISPLAY=:99
RUN echo "Xvfb :99 & librepcb-cli \"$@\"" > /entrypoint.sh \
    && chmod a+x /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
