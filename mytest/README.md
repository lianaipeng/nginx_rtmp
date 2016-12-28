

在http模块中添加：

http {
    server {
        listen 8004;

        location /test{
            mytest;
        }
    }
}
