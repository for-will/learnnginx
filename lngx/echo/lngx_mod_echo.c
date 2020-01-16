//
// Created by xuyz on 2020/1/15.
//
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


extern ngx_module_t lngx_echo_module;

ngx_int_t send_body(ngx_http_request_t *r, ngx_str_t str);

typedef struct echo_conf_s {
    ngx_str_t msg;
} echo_conf_t;

static void *create(ngx_conf_t *cf) {
    return ngx_palloc(cf->pool, sizeof(echo_conf_t));
}

static ngx_int_t handler(ngx_http_request_t *r) {

    if (!(r->method & NGX_HTTP_GET)) {
        return NGX_HTTP_NOT_ALLOWED;
    }
    ngx_http_discard_request_body(r);
    echo_conf_t *cf = (echo_conf_t *) ngx_http_get_module_loc_conf(r, lngx_echo_module);
    size_t len = cf->msg.len;
    if (r->args.len > 0) {
        len += r->args.len + 1;
    }

    r->headers_out.content_length_n = len;
    r->headers_out.status = NGX_HTTP_OK;

    ngx_int_t rc = ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK) {
        return rc;
    }

    if (r->args.len > 0) {
//        ngx_buf_t *buf = ngx_calloc_buf(r->pool);
//        buf->pos = r->args.data;
//        buf->last = r->args.data + r->args.len;
//
//        buf->start = r->args.data;
//        buf->end = r->args.data + r->args.len;
//
//        buf->memory = 1;
//
//        ngx_chain_t *out = ngx_alloc_chain_link(r->pool);
//        out->next = NULL;
//
//        out->buf = buf;
//
//        ngx_http_output_filter(r, out);
        send_body(r, r->args);
        ngx_str_t sep = ngx_string(",");
        send_body(r, sep);
    }

    send_body(r, cf->msg);

    return ngx_http_send_special(r, NGX_HTTP_LAST);
}

ngx_int_t send_body(ngx_http_request_t *r, ngx_str_t str) {
    ngx_buf_t buf;
    buf.pos = str.data;
    buf.last = str.data + str.len;

    buf.start = buf.pos;
    buf.end = buf.last;

    buf.memory = 1;

    ngx_chain_t out;
    out.buf = &buf;
    out.next = NULL;
    return ngx_http_output_filter(r, &out);
}

static char *set_echo(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {
    char *rc = ngx_conf_set_str_slot(cf, cmd, conf);
    if (rc != NGX_OK) {
        return rc;
    }

//    ngx_http_conf_ctx_t *ctx = cf->ctx;
    ngx_http_core_loc_conf_t *loc = (ngx_http_core_loc_conf_t *) ngx_http_conf_get_module_loc_conf(cf,
                                                                                                   ngx_http_core_module);
    loc->handler = handler;
    return NGX_CONF_OK;
}

static ngx_command_t cmds[] = {
    {
        ngx_string("lngx_echo"),
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        set_echo,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(echo_conf_t, msg),
        NULL,
    },
    ngx_null_command
};

static ngx_http_module_t lngx_echo_module_ctx = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    create,
    NULL,
};

ngx_module_t lngx_echo_module = {
    NGX_MODULE_V1,
    &lngx_echo_module_ctx,
    cmds,
    NGX_HTTP_MODULE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NGX_MODULE_V1_PADDING,
};

