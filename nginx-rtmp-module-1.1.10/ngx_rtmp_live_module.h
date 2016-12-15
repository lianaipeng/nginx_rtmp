
/*
 * Copyright (C) Roman Arutyunyan
 */


#ifndef _NGX_RTMP_LIVE_H_INCLUDED_
#define _NGX_RTMP_LIVE_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include "ngx_rtmp.h"
#include "ngx_rtmp_cmd_module.h"
#include "ngx_rtmp_bandwidth.h"
#include "ngx_rtmp_streams.h"

#include "ngx_rtmp_relay_module.h"

#include "ngx_rtmp_codec_module.h"

typedef struct ngx_rtmp_live_ctx_s ngx_rtmp_live_ctx_t;
typedef struct ngx_rtmp_live_stream_s ngx_rtmp_live_stream_t;
typedef struct ngx_rtmp_live_push_cache_s ngx_rtmp_live_push_cache_t;
typedef struct ngx_rtmp_live_app_conf_s ngx_rtmp_live_app_conf_t;


typedef struct {
    unsigned                            active:1;
    uint32_t                            timestamp;
    uint32_t                            csid;
    uint32_t                            dropped;
} ngx_rtmp_live_chunk_stream_t;


struct ngx_rtmp_live_ctx_s {
    ngx_rtmp_session_t                 *session;
    ngx_rtmp_live_stream_t             *stream;
    ngx_rtmp_live_ctx_t                *next;
    ngx_uint_t                          ndropped;
    ngx_rtmp_live_chunk_stream_t        cs[2];
    ngx_uint_t                          meta_version;
    ngx_event_t                         idle_evt;
    unsigned                            active:1;
    unsigned                            publishing:1;
    unsigned                            silent:1;
    unsigned                            paused:1;

    ngx_uint_t                          newflag;
};

// 添加缓存结构体
struct ngx_rtmp_live_push_cache_s{
    ngx_int_t                           frame_type;
    ngx_int_t                           frame_len;
    int64_t                             frame_pts;
    ngx_flag_t                          frame_flag;
    ngx_chain_t                         *frame_buf;
    ngx_rtmp_header_t                   frame_header;
    ngx_rtmp_live_push_cache_t          *next;

    ngx_flag_t                          has_closed;  // 流是否被关闭过
    ngx_int_t                           mandatory;   // H264 header type
}; 

typedef struct {
    ngx_uint_t                  width;
    ngx_uint_t                  height;
    ngx_uint_t                  duration;
    ngx_uint_t                  frame_rate;
    ngx_uint_t                  video_data_rate;
    ngx_uint_t                  video_codec_id;
    ngx_uint_t                  audio_data_rate;
    ngx_uint_t                  audio_codec_id;
    ngx_uint_t                  aac_profile;
    ngx_uint_t                  aac_chan_conf;
    ngx_uint_t                  aac_sbr;
    ngx_uint_t                  aac_ps;
    ngx_uint_t                  avc_profile;
    ngx_uint_t                  avc_compat;
    ngx_uint_t                  avc_level;
    ngx_uint_t                  avc_nal_bytes;
    ngx_uint_t                  avc_ref_frames;
    ngx_uint_t                  sample_rate;    /* 5512, 11025, 22050, 44100 */
    ngx_uint_t                  sample_size;    /* 1=8bit, 2=16bit */
    ngx_uint_t                  audio_channels; /* 1, 2 */
    /*
    u_char                      profile[32];
    u_char                      level[32];
    
    ngx_uint_t                  meta_version;
    */
    
    // publish 
    ngx_uint_t                  number;
    ngx_str_t                   addr_text;
    ngx_uint_t                  epoch;
    ngx_str_t                   flashver;
    ngx_str_t                   page_url;
    ngx_str_t                   swf_url;
    ngx_str_t                   tc_url;
} ngx_rtmp_stream_codec_ctx_t;

struct ngx_rtmp_live_stream_s {
    u_char                              name[NGX_RTMP_MAX_NAME];
    ngx_rtmp_live_stream_t             *next;
    ngx_rtmp_live_ctx_t                *ctx;
    ngx_rtmp_bandwidth_t                bw_in;
    ngx_rtmp_bandwidth_t                bw_in_audio;
    ngx_rtmp_bandwidth_t                bw_in_video;
    ngx_rtmp_bandwidth_t                bw_out;
    ngx_rtmp_bandwidth_t                bw_out_audio;
    ngx_rtmp_bandwidth_t                bw_out_video;
    ngx_msec_t                          epoch;
    unsigned                            active:1;
    unsigned                            publishing:1;
        
    ngx_rtmp_live_push_cache_t          *push_cache_head;
    ngx_rtmp_live_push_cache_t          *push_cache_tail;
    ngx_int_t                            push_cache_count;

    ngx_rtmp_live_push_cache_t          *idle_cache_head; // 空闲队列 
    ngx_rtmp_live_push_cache_t          *idle_cache_tail;
    ngx_int_t                            idle_cache_count;
    
    ngx_pool_t                          *pool;  // 缓存池
    ngx_chain_t                         *free;  // 空闲内存队列
    
    // 时间校验　
    ngx_uint_t                           push_cache_expts; //end expect time
    
    ngx_event_t                          push_cache_event;
    
    // 当publish断开之后 才启用这三个参数 否则默认为空
    ngx_rtmp_session_t                  *session;    
    ngx_rtmp_live_app_conf_t            *lacf;
    ngx_rtmp_core_srv_conf_t            *cscf;
    ngx_rtmp_live_chunk_stream_t        cs[2];
    
    ngx_uint_t                          closed_count; // 关闭次数
    ngx_uint_t                          meta_version;
    ngx_chain_t                         *meta; // 不知道是什么
    ngx_chain_t                         *aac_header; // 保存第一次的头
    ngx_chain_t                         *avc_header; // 保存第一次的头
    ngx_chain_t                         *coheader; // 保存第一次的头
    
    ngx_rtmp_relay_ctx_t                *relay_ctx;    //push_realy

    void                  **main_conf;
    void                  **srv_conf;
    void                  **app_conf;
    
    // 监控状态
    ngx_rtmp_stream_codec_ctx_t         codec_ctx;
    ngx_msec_t                          current_time;
    //ngx_uint_t                          cache_len;
    ngx_msec_t                          push_cache_alts;   // audio last timestamp 
    ngx_msec_t                          push_cache_vlts;   // video last timestamp  
    ngx_msec_t                          push_cache_delta;
    ngx_uint_t                          ndropped;
    ngx_flag_t                          interleave;
};


//typedef struct{
struct ngx_rtmp_live_app_conf_s{
    ngx_int_t                           nbuckets;
    ngx_rtmp_live_stream_t            **streams;
    ngx_flag_t                          live;
    ngx_flag_t                          meta;
    ngx_msec_t                          sync;
    ngx_msec_t                          idle_timeout;
    ngx_flag_t                          atc;
    ngx_flag_t                          interleave;
    ngx_flag_t                          wait_key;
    ngx_flag_t                          wait_video;
    ngx_flag_t                          publish_notify;
    ngx_flag_t                          play_restart;
    ngx_flag_t                          idle_streams;
    ngx_msec_t                          buflen;
    ngx_pool_t                         *pool;
    ngx_rtmp_live_stream_t             *free_streams;

    ngx_flag_t                          push_cache;
    ngx_int_t                           push_cache_time_len;
    ngx_int_t                           push_cache_frame_num;
    ngx_flag_t                          publish_delay_close;
    
    
    ngx_uint_t                          stream_push_reconnect;
    ngx_array_t                 pulls;         /* ngx_rtmp_relay_target_t * */
    ngx_array_t                 pushes;        /* ngx_rtmp_relay_target_t * */
    ngx_array_t                 static_pulls;  /* ngx_rtmp_relay_target_t * */
    ngx_array_t                 static_events; /* ngx_event_t * */
    ngx_log_t                  *log;
    //ngx_uint_t                  nbuckets;
    //ngx_msec_t                  buflen;
    ngx_flag_t                  session_relay;
    ngx_msec_t                  push_reconnect;
    ngx_msec_t                  pull_reconnect;
};


extern ngx_module_t  ngx_rtmp_live_module;


#endif /* _NGX_RTMP_LIVE_H_INCLUDED_ */
