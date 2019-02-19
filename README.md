## MySQL UDF for Hangul Jamo Decomposition
### Docker commands
Below commands should bring up a MariaDB container with jamodecomp() UDF.
```shell
>>> docker build --tag mysqludf/jamotest .
>>> docker run -it -p 3306:3306 --name jamotest -e MYSQL_ROOT_PASSWORD=password -d mysqludf/jamotest:latest
```

### Using the function
```sql
SELECT jamodecomp('정태원') as decomposed;
SELECT * from users u where jamodecomp(u.name) LIKE jamodecomp('정ㅌ%');
...
```