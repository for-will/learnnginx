//
// Created by xuyz on 2020/1/5.
//

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

extern ngx_module_t lngx_test_module;

typedef struct lngx_test_conf_s {
    ngx_flag_t enabled;
} lngx_test_conf_t;

//配置指令
static ngx_command_t lngx_test_cmds[] = {
    {
        ngx_string("lgnx_test"),
        NGX_HTTP_LOC_CONF | NGX_CONF_FLAG,
        ngx_conf_set_flag_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(lngx_test_conf_t, enabled),
        NULL
    },
    ngx_null_command
};

static void *create_loc_conf(ngx_conf_t *cf) {
    lngx_test_conf_t *c = (lngx_test_conf_t *) ngx_pcalloc(cf->pool, sizeof(lngx_test_conf_t));
    c->enabled = NGX_CONF_UNSET;// 必须在这里设置初始值，不然会报错
    return c;
}

//业务逻辑
static ngx_int_t handler(ngx_http_request_t *r) {
    lngx_test_conf_t *cf = (lngx_test_conf_t *) ngx_http_get_module_loc_conf(r, lngx_test_module);

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "hello lngx");

    if (cf->enabled) {
        puts("hello nginx");
    } else {
        puts("hello disabled");
    }
    return NGX_DECLINED;
}

//框架注册
static ngx_int_t init(ngx_conf_t *cf) {
    ngx_http_core_main_conf_t *cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);
    ngx_array_t *arr = &cmcf->phases[NGX_HTTP_REWRITE_PHASE].handlers;
    ngx_http_handler_pt *h = ngx_array_push(arr);
    *h = handler;
    return NGX_OK;
}

//模块集成
static ngx_http_module_t lngx_test_ctx = {
    NULL,
    init,
    NULL,
    NULL,
    NULL,
    NULL,
    create_loc_conf,
    NULL,
};

ngx_module_t lngx_test_module = {
    NGX_MODULE_V1,
    &lngx_test_ctx,
    lngx_test_cmds,
    NGX_HTTP_MODULE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NGX_MODULE_V1_PADDING
};