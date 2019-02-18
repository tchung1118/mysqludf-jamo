FROM mariadb:10.2

RUN apt-get update && apt-get install -y \
  gcc \
  libmariadbclient-dev && rm -rf /var/lib/apt/lists/*

ADD . /root/mysqludf_jamodecompose

WORKDIR /root/mysqludf_jamodecompose

RUN gcc -Wall -bundle -o udf_jamodecompose.so `/usr/local/mysql/bin/mysql_config --cflags` udf_jamodecomp.c