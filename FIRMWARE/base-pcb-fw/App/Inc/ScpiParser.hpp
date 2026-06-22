#pragma once

#include <cstdint>
#include <cstddef>

#define SCPI_MAX_RESPONSE  512
#define SCPI_MAX_ARGS      6

class ScpiParser {
public:
    struct CmdCtx {
        char        out[SCPI_MAX_RESPONSE];
        size_t      out_len;
        size_t      out_cap;
        bool        query;
        const char *path[8];
        int         depth;          // number of path segments
        char        raw[160];       // raw command copy
        char        args[SCPI_MAX_ARGS][32];
        int         arg_count;
    };

    ScpiParser();
    int execute(const char *cmd, char *out, size_t out_size);

private:
    static void append(CmdCtx *ctx, const char *s);
    static void append_f(CmdCtx *ctx, const char *fmt, ...);

    static int  tokenise(const char *cmd, CmdCtx *ctx);
    static void extract_args(CmdCtx *ctx);
    static void dispatch(CmdCtx *ctx);

    // handlers
    static void h_idn(CmdCtx *);
    static void h_rst(CmdCtx *);
    static void h_cls(CmdCtx *);
    static void h_syst_err(CmdCtx *);
    static void h_conf_tc_type(CmdCtx *);
    static void h_conf_ch_enab(CmdCtx *);
    static void h_conf_cjc_mode(CmdCtx *);
    static void h_conf_cjc_value(CmdCtx *);
    static void h_meas_temp(CmdCtx *);
    static void h_read(CmdCtx *);
    static void h_init(CmdCtx *);
    static void h_stop(CmdCtx *);
    static void h_freq(CmdCtx *);
    static void h_data_last(CmdCtx *);
    static void h_data_buf(CmdCtx *);
    static void h_cal_offset(CmdCtx *);
    static void h_cal_gain(CmdCtx *);
    static void h_cal_reset(CmdCtx *);
    static void h_stat_chan(CmdCtx *);
    static void h_stat_over(CmdCtx *);
    static void h_stat_adc(CmdCtx *);
    static void h_mem_store(CmdCtx *);
    static void h_mem_recall(CmdCtx *);
    static void h_mem_list(CmdCtx *);
};
