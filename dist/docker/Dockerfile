FROM alpine:3.9

RUN apk add --no-cache wget zip
RUN wget https://download.librepcb.org/releases/0.1.1/librepcb-0.1.1-source.zip
RUN unzip librepcb-0.1.1-source.zip
RUN cd librepcb-0.1.1
RUN apk add --no-cache qt5-qtbase qt5-qtbase-dev
RUN apk add --no-cache g++ make
RUN cd librepcb-0.1.1 && qmake-qt5 -r librepcb.pro && make -j8
RUN cd librepcb-0.1.1 && make install
ENTRYPOINT ["librepcb-cli"]
