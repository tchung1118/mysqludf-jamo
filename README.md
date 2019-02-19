# MySQL UDF for Hangul Jamo Decomposition
## How to use
Below commands should bring up a MariaDB container with jamodecomp() UDF.
```shell
>>> docker build --tag mysqludf/jamotest .
>>> docker run -it -p 3306:3306 --name jamotest -e MYSQL_ROOT_PASSWORD=password -d mysqludf/jamotest:latest
```