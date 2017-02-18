#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static char* ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);  
//static ngx_int_t ngx_http_mytest_str_handler(ngx_http_request_t *r);  
static ngx_int_t ngx_http_mytest_json_handler(ngx_http_request_t *r);  
//static char* ngx_conf_set_myconfig(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void* ngx_http_mytest_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_mytest_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);

typedef struct{
    ngx_str_t my_config_str;
    ngx_int_t my_config_num;   
}ngx_http_mytest_myconf_t;

typedef struct{
    ngx_flag_t my_flag;
    ngx_str_t my_str;
    ngx_int_t my_num;
    //size_t my_size;
    ngx_msec_t my_msec;
    ngx_http_mytest_myconf_t my_config;
}ngx_http_mytest_conf_t;


//处理配置项    
// 当出现mytest的时候 调用ngx_http_mytest函数 
static ngx_command_t  ngx_http_mytest_commands[] =  
{
    {  
        // 配置项名称
        // ngx_str_t name 
        ngx_string("mytest"),  
        // 决定了这个配置项可以在那些块中出现，以及可以携带的参数类型和个数等。
        // ngx_uint_t type
        NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_HTTP_LMT_CONF | NGX_CONF_NOARGS,   
        // 在某个配置块中出现mytest配置项时，nginx将会调用ngx_http_mytest方法  即处理参数的回调函数，也可以用系统自定义的。
        // char *(*set)(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
        ngx_http_mytest,  
        // 在配置文件中的偏移量
        // ngx_uint_t offset
        NGX_HTTP_LOC_CONF_OFFSET,  
        0,  
        // 配置项读取后的处理方法，必须是ngx_conf_post_t结构的指针
        NULL  },  
    {
        ngx_string("test_flag"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
        ngx_conf_set_flag_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, my_flag),
        NULL  },
    {
        ngx_string("test_str"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
        ngx_conf_set_str_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, my_str),
        NULL  },
    {
        ngx_string("test_num"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
        ngx_conf_set_num_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, my_num),
        NULL  },
    {
        ngx_string("test_msec"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
        ngx_conf_set_msec_slot, 
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_mytest_conf_t, my_msec),
        NULL  },
        /*
    {
        ngx_string("test_myconfig"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE12,
        ngx_conf_set_myconfig,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL  },
        */
    ngx_null_command  
};

/*
// 读取location的配置文件的回调
static char* ngx_conf_set_myconfig(ngx_conf_t *cf, ngx_command_t *cmd, void *conf){
    // 注意 参数conf就是HTTP框架传给用户的在ngx_http_mytest_create_loc_conf回调方法中分配的结构体ngx_http_mytest_myconf_t 
    ngx_http_mytest_myconf_t *mycf = conf;

    // cf->args 是一个ngx_array_t队列，他的成员都是ngx_str_t结构。我们用value指向ngx_array_t的elts内容，其中value[1]就是第一个参数，同理，value[2]是第二个参数
    ngx_str_t* value = cf->args->elts;

    // ngx_array_t 的nelts表示参数个数
    if(cf->args->nelts > 1)
    {
        // 直接赋值即可，ngx_str_t结构只是指针的传递
        mycf->my_config_str = value[1];
    }
    if(cf->args->nelts > 2)
    {
        // 将字符串形式的第二个参数转换为整形
        mycf->my_config_num = ngx_atoi(value[2].data, value[2].len);
        // 如果字符串转换整型失败，将报“invalid number”错误，Nginx启动失败
        if(mycf->my_config_num == NGX_ERROR){
            return "invalid number";
        }
    }
    
    // 不显示了 
    ngx_log_error(NGX_LOG_INFO, cf->log, 0 ,"##############");
    ngx_log_debug(NGX_LOG_DEBUG_HTTP, cf->log, 0, "set debug");
    
    // 返回成功
    return NGX_CONF_OK;
}
*/


//模块上下文 
// 如果没有什么工作是必须在HTTP框架初始化时完成的，就不需要实现ngx_http_module_t的8个回调方法 初始化为NULL即可。
static ngx_http_module_t  ngx_http_mytest_module_ctx =  
{  
    // 解析配置文件之前调用
    // ngx_int_t (*preconfiguration)(ngx_conf_t ＊cf);
    NULL,                       /* preconfiguration */
    // 完成配置文件的解析后调用
    // ngx_int_t   (*postconfiguration)(ngx_conf_t *cf);  
    NULL,                       /* postconfiguration */  
    
    // 当创建数据结构用于存储main级别的（直属于http{...}块的配置项）的全局配置项时，可以通过create_main_conf回调方法创建存储全局配置项的结构体
    // void       *(*create_main_conf)(ngx_conf_t *cf);
    NULL,                       /* create main configuration */ 
    // 常用于初始化main级别配置项
    // char       *(*init_main_conf)(ngx_conf_t *cf, void *conf); 
    NULL,                       /* init main configuration */  
    
    // 当需要创建数据结构用于存储srv级别的配置项时，可以通过实现该回调方法创建存储srv级别配型项的结构体
    // void       *(*create_srv_conf)(ngx_conf_t *cf);
    NULL,                       /* create server configuration */
    // 该回调方法主要用于合并main级别和srv级别下的同名配置项
    // char       *(*merge_srv_conf)(ngx_conf_t *cf, void *prev, void *conf);  
    NULL,                       /* merge server configuration */  
     
    // 创建loc级别
    // void       *(*create_loc_conf)(ngx_conf_t *cf);
    //NULL,                       /* create location configuration */ 
    ngx_http_mytest_create_loc_conf,
    // 合并srv和loc级别下的同名配置 
    // *(*merge_loc_conf)(ngx_conf_t *cf, void *prev, void *conf); 
    //NULL                        /* merge location configuration */  
    ngx_http_mytest_merge_loc_conf
};

static void* ngx_http_mytest_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_mytest_conf_t *mycf;
    mycf = (ngx_http_mytest_conf_t *)ngx_pcalloc(cf->pool, sizeof(ngx_http_mytest_conf_t));
    if(mycf == NULL){
        return NULL;
    }
    
    // 如果用系统的slot函数 必须初始化为该值。否则在解析时会报错
    mycf->my_flag = NGX_CONF_UNSET;
    mycf->my_num  = NGX_CONF_UNSET;
    mycf->my_msec = NGX_CONF_UNSET_MSEC;
    
    return mycf; 
}
static char *ngx_http_mytest_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_mytest_conf_t *prev = (ngx_http_mytest_conf_t *)parent;
    ngx_http_mytest_conf_t *conf = (ngx_http_mytest_conf_t *)child;
    // 将父配置块的值 赋予 子配置块（父配置块优先级更高）
    // 反过来也可以，表示自配置块的优先级更高
    ngx_conf_merge_str_value(conf->my_str, prev->my_str, "defaultstr");
    //ngx_conf_merge_str_value(conf->my_config.my_config_str, prev->my_config.my_config_str, "defaultstr");
    
    /*
    u_char buffer[1024];
    ngx_snprintf(buffer, 1024, "%V", &prev->my_str);
    //ngx_snprintf(buffer, 1024, "%s", prev->my_str.data);
    ngx_log_error(NGX_LOG_ALERT, cf->log, 0 ,"##############");
    ngx_log_error(NGX_LOG_ALERT, cf->log, 0, "%d %V", prev->my_str.len, &prev->my_str);
    */
    //ngx_log_error(NGX_LOG_ALERT, cf->log, 0 ,"############## %V, %d", &prev->my_config.my_config_str ,prev->my_config.my_config_num);
    //ngx_log_debug(NGX_LOG_DEBUG_HTTP, cf->log, 0, "$$$$$$$$$$$");

    //ngx_log_error(NGX_LOG_ALERT, cf->log, 0, "#parent->my_flag:%d,parent->my_str:%V,parent->my_num:%d,parent->my_msec:%uL", prev->my_flag, &prev->my_str, prev->my_num, prev->my_msec);
    //ngx_log_error(NGX_LOG_ALERT, cf->log, 0, "#child->my_flag:%d,child->my_str:%V,child->my_num:%d,child->my_msec:%uL", conf->my_flag, &conf->my_str, conf->my_num, conf->my_msec);
    
    return NGX_CONF_OK; 
}


//新模块定义
//应该是nginx的入口配置  
ngx_module_t  ngx_http_mytest_module =  
{  
    // 有7个值
    // ngx_uint_t ctx_index; 表示当前模块在这类模块中的序号 
    // ngx_uint_t index; 表示当前模块在ngx_modules数组中的序号，表示当前模块在所有模块中的序号。
    // ngx_uint_t spare0,spare1,spare2,spare3; 4个为预留，暂时未用
    // ngx_uint_t version; 模块的版本，便于将来的扩展，目前只有一种，默认为1 
    NGX_MODULE_V1,
    
    // ctx用于指向一类模块的上下文结构体。因为每个模块都有了自己的特性，而ctx将会指向特定类型的公共接口。例如在HTTP模块中，ctx需要指向ngx_http_module_t结构体   
    &ngx_http_mytest_module_ctx,           /* module context */  
    
    // commands 将处理nginx.conf中的配置项
    ngx_http_mytest_commands,              /* module directives */
    
    // ngx_uint_t type 表示该模块的类型，他与ctx指针是紧密相关的 HTTP CORE CONF EVENT MAIL 五大模块  
    NGX_HTTP_MODULE,                       /* module type */  
    
    // 在nginx启动、停止过程中，一下7个函数指针表示有7个执行点会分别调用这7种方法，对于任一个方法，如果不需要在某个时刻执行它，可以为空。
    NULL,                                  /* init master */  
    NULL,                                  /* init module */  
    NULL,                                  /* init process */  
    NULL,                                  /* init thread */  
    NULL,                                  /* exit thread */  
    NULL,                                  /* exit process */  
    NULL,                                  /* exit master */  

    // 以下8个spare_hook 变量也是保留字段，目前没有使用。下面的宏用作填充8个0
    NGX_MODULE_V1_PADDING  
};

//配置项对应的回调函数   
// ngx_command_t中的set成员，当在某个配置块中出现mytest配置项时，调用该方法
static char * ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)  
{  
    //printf("ngx_http_mytest name:%s, args:%s, nelts:%ld, size:%ld\n", cf->name, (char *)cf->args->elts, cf->args->nelts, cf->args->size);
    ngx_http_core_loc_conf_t  *clcf;  
       
    //ngx_log_error(NGX_LOG_ERR, cf->log, 0 ,"#######IIII#######");
    //首先找到mytest配置项所属的配置块，clcf貌似是location块内的数据  
    //结构，其实不然，它可以是main、srv或者loc级别配置项，也就是说在每个  
    //http{}和server{}内也都有一个ngx_http_core_loc_conf_t结构体  
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);  
    
    //http框架在处理用户请求进行到NGX_HTTP_CONTENT_PHASE阶段时，如果  
    //请求的主机域名、URI与mytest配置项所在的配置块相匹配，就将调用我们  
    //实现的ngx_http_mytest_handler方法处理这个请求  
    //clcf->handler = ngx_http_mytest_str_handler;  
    clcf->handler = ngx_http_mytest_json_handler;  
    
    return NGX_CONF_OK;  
}

static ngx_int_t ngx_http_mytest_json_handler(ngx_http_request_t *r)  
{
    //必须是GET或者HEAD方法，否则返回405 Not Allowed  
    if (!(r->method & (NGX_HTTP_GET | NGX_HTTP_HEAD)))  
    {  
        return NGX_HTTP_NOT_ALLOWED;  
    }  

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0 ,"#######DDD#######");

    //丢弃请求中的包体  
    ngx_int_t rc = ngx_http_discard_request_body(r);  
    if (rc != NGX_OK)  
    {  
        return rc;  
    }  

    //设置返回的Content-Type。注意，ngx_str_t有一个很方便的初始化宏  
    //ngx_string，它可以把ngx_str_t的data和len成员都设置好  
    //ngx_str_t type = ngx_string("application/json");
    ngx_str_t type = ngx_string("text/plain");  
    //返回的包体内容  
    ngx_str_t response = ngx_string("{\"key\"\r\n:\"Hello World!\"}");  
    //设置返回状态码  
    r->headers_out.status = NGX_HTTP_OK;  
    //响应包是有包体内容的，所以需要设置Content-Length长度  
    r->headers_out.content_length_n = response.len;  
    //设置Content-Type  
    r->headers_out.content_type = type;  

    //发送http头部  
    rc = ngx_http_send_header(r);  
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only)  
    {  
        return rc;  
    }  

    //构造ngx_buf_t结构准备发送包体  
    ngx_buf_t *b;  
    b = ngx_create_temp_buf(r->pool, response.len);  
    if (b == NULL)  
    {  
        return NGX_HTTP_INTERNAL_SERVER_ERROR;  
    }  
    //将Hello World拷贝到ngx_buf_t指向的内存中  
    ngx_memcpy(b->pos, response.data, response.len);  
    //注意，一定要设置好last指针  
    b->last = b->pos + response.len;  
    //声明这是最后一块缓冲区  
    b->last_buf = 1;  
    
    //构造发送时的ngx_chain_t结构体  
    ngx_chain_t     out;  
    //赋值ngx_buf_t  
    out.buf = b;  
    //设置next为NULL  
    out.next = NULL;  
    
    //最后一步发送包体，http框架会调用ngx_http_finalize_request方法  
    //结束请求  
    return ngx_http_output_filter(r, &out);  
}
/*
//实际完成处理的回调函数    
static ngx_int_t ngx_http_mytest_str_handler(ngx_http_request_t *r)  
{
    //必须是GET或者HEAD方法，否则返回405 Not Allowed  
    if (!(r->method & (NGX_HTTP_GET | NGX_HTTP_HEAD)))  
    {  
        return NGX_HTTP_NOT_ALLOWED;  
    }  

    //丢弃请求中的包体  
    ngx_int_t rc = ngx_http_discard_request_body(r);  
    if (rc != NGX_OK)  
    {  
        return rc;  
    }  

    // 获取配置文件的信息
    //####################
    // http://www.cnblogs.com/runnyu/p/4887256.html
    ngx_http_mytest_conf_t* my_conf;
    my_conf = ngx_http_get_module_loc_conf(r, ngx_http_mytest_module);
    ngx_str_t response = my_conf->my_str;
    
    //ngx_int_t tmp_num = my_conf->my_num;
    //ngx_log_error(NGX_LOG_ALERT, r->connection->log, 0 ,"##ALERT############");
    //ngx_log_error(NGX_LOG_INFO, r->connection->log, 0 ,"##INFO############ %V", &response);
    //ngx_log_error(NGX_LOG_DEBUG, r->connection->log, 0 ,"##DEBUG############");
    // http://haoningabc.iteye.com/blog/1960168
    //####################


    //设置返回的Content-Type。注意，ngx_str_t有一个很方便的初始化宏  
    //ngx_string，它可以把ngx_str_t的data和len成员都设置好  
    //ngx_str_t type = ngx_string("text/plain");  
    ngx_str_t type = ngx_string("application/json");
    //返回的包体内容  
    //ngx_str_t response = ngx_string("Hello World!\n");  
    //设置返回状态码  
    r->headers_out.status = NGX_HTTP_OK;  
    //响应包是有包体内容的，所以需要设置Content-Length长度  
    r->headers_out.content_length_n = response.len;  
    //设置Content-Type  
    r->headers_out.content_type = type;  

    //发送http头部  
    rc = ngx_http_send_header(r);  
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only)  
    {  
        return rc;  
    }  

    //构造ngx_buf_t结构准备发送包体  
    ngx_buf_t *b;  
    b = ngx_create_temp_buf(r->pool, response.len);  
    if (b == NULL)  
    {  
        return NGX_HTTP_INTERNAL_SERVER_ERROR;  
    }  
    //将Hello World拷贝到ngx_buf_t指向的内存中  
    ngx_memcpy(b->pos, response.data, response.len);  
    //注意，一定要设置好last指针  
    b->last = b->pos + response.len;  
    //声明这是最后一块缓冲区  
    b->last_buf = 1;  
    
    //构造发送时的ngx_chain_t结构体  
    ngx_chain_t     out;  
    //赋值ngx_buf_t  
    out.buf = b;  
    //设置next为NULL  
    out.next = NULL;  
    
    //最后一步发送包体，http框架会调用ngx_http_finalize_request方法  
    //结束请求  
    return ngx_http_output_filter(r, &out);  
}
*/
//#####################################################################






/*
//#####################################################################
// 该接口是初始化用于存储定时器的全局变量 ngx_event_timer_rbtree
ngx_int_t ngx_event_timer_init(ngx_log_t *log)
{
    ngx_rbtree_init(&ngx_event_timer_rbtree, &ngx_event_timer_sentinel,ngx_rbtree_insert_timer_value);
    return NGX_OK;
}

// 用来保存定时器
ngx_rbtree_t    ngx_event_timer_rbtree;
// 系统当前时间
volatile  ngx_msec_t  ngx_current_msec;
nxg_msec_t ngx_event_find_timer(void)
{
    ngx_msec_int_t timer;
    ngx_rgtree_node_t *node, *root, *sentinel;
    
    if( ngx_event_timer_rbtree.root == &ngx_event_timer_sentinel ){
        return NGX_TIMER_INFINITE;
    }
    
    root = ngx_event_timer_rbtree.root;
    sentinel = ngx_event_timer_rbtree.sentinel;
    
    node = ngx_rbtree_min(root, sentinel);
    timer = (ngx_msec_min_t)(node->key - ngx_current_msec);
    
    return (ngx_msec_t)(timer>0timer:0);  
}
// 超时事件处理
void  ngx_event_expire_timers(void)
{
    ngx_event_t  * ev;
    ngx_rbtree_node_t  *node, *root, *sentinel;

    sentinel = ngx_event_timer_rbtree.sentinel;
    for( ; ; ) {
        root = ngx_event_timer_rbtree.root;
        if(root == sentinel) {
            return ;
        }

        node = ngx_rbtree_min(root, sentinel);

        if( (ngx_msec_int_t)(node->key - ngx_current_msec) > 0 ) {
            // 最早的一个定时器都未超时, 直接返回
            return ;
        }

        // 取出定时器事件
        ev = (ngx_event_t*)((char*)node - offsetof(ngx_event_t, timer));
        // 删除定时器
        ngx_rbtree_delete(&ngx_event_timer_rbtree, &ev->timer);

        ev->timer_set = 0;
        // 设置超时标识
        ev->timedout = 1;
        // 回调处理
        ev->handler(ev);
    }
}

// 添加定时器
static  ngx_inline  void  ngx_event_add_timer( ngx_event_t * ev, ngx_msec_t timer)
{
    ngx_msec_t  key;
    ngx_msec_int_t  diff;

    // 计算超时的时间
    key = ngx_current_msec + timer;

    if( ev->timer_set ) {
        // 如果该事件已经设置过定时器, 先后两次设置定时器必须大于默认的最小时间
        diff = (ngx_msec_int_t)(key - ev->timer.key);
        if(ngx_abs(diff) < NGX_TIMER_LAZY_DELAY) {
            return;
        }
        // 大于默认的最小时间, 则删除老的, 以新的为准
        ngx_del_timer(ev);
    }

    ev->timer.key = key;
    // 添加到定时器 红黑树中
    ngx_rbtree_insert( &ngx_event_timer_rbtree, &ev->timer);
    // 置标识位
    ev->timer_set = 1;
}

// 删除定时器
static ngx_inline void  ngx_event_del_timer( ngx_event_t * ev )
{
        ngx_rbtree_delete( &ngx_event_timer_rbtree, &ev->timer);

            ev->timer_set = 0;
}
*/


