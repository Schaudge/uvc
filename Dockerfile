FROM yunying_centos8_basic:1.1
# docker build -t 10.168.2.67:5000/uvc-yy .
# docker push 10.168.2.67:5000/uvc-yy
# Maintainer: wangxz
# Date: 2023-01-31

RUN groupadd -g 10001 yunying \
    && useradd -g yunying -u 10001 yunying

RUN mkdir -p /yunying/data \
    && chown -R yunying:yunying /yunying \
    && chmod 775 /yunying

COPY ext/htslib-1.16.tar.gz /opt/

RUN cd /opt && tar zxvf htslib-1.16.tar.gz \
    && cd htslib-1.16 && ./configure && make

COPY ext/bcftools-1.16.tar.gz /opt/

RUN cd /opt && tar zxvf bcftools-1.16.tar.gz \
    && cd bcftools-1.16 && ./configure && make install

RUN mkdir /opt/CLI11-1.7.1
COPY CLI11-1.7.1/CLI11.hpp /opt/CLI11-1.7.1/

COPY gitdiff.txt version.h bin/uvcTN.sh *.hpp *.cpp /opt/
COPY Makefile.Dockerfile /opt/Makefile

RUN cd /opt && make && mv /opt/uvc-1-fopenmp-thread /opt/uvc1

COPY integrate_haplotype_variation.cpp /opt/

RUN cd /opt && g++ -std=c++14 -Wall -Wno-unused-variable -fconcepts -o haplotype -I htslib-1.16 \
    -DNDEBUG -DCOMMIT_VERSION=\"latest\" -DCOMMIT_DIFF_SH=\"Schaudge\" -DCOMMIT_DIFF_FULL=\"\" \
    integrate_haplotype_variation.cpp htslib-1.16/libhts.a -pthread -lm -lz -lbz2 -llzma -lcurl

