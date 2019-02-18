FROM mariadb:10.1

RUN apt-get update && apt-get install -y \
  gcc \
  libmariadbclient18 \
  libmariadbclient-dev && rm -rf /var/lib/apt/lists/*

ADD . /root/mysqludf_jamodecompose

WORKDIR /root/mysqludf_jamodecompose

RUN gcc -Wall -fPIC -O -g jamo.c -c -o jamo.o
RUN gcc -shared jamo.o -Wall -fPIC -o udf_jamodecomp.so `/usr/bin/mysql_config --cflags` udf_jamodecomp.c
RUN cp udf_jamodecomp.so /usr/lib/mysql/plugin/.
RUN cp setup/setup_jamodecomp.sql /docker-entrypoint-initdb.d/.