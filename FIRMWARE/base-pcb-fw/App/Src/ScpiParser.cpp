#include "ScpiParser.hpp"
#include "ChannelManager.hpp"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cstdarg>

// ── helpers ──────────────────────────────────────────────────────────────────
static inline bool ieq(const char *a, const char *b) {
    while (*a && *b) {
        if (toupper((unsigned char)*a) != toupper((unsigned char)*b)) return false;
        a++; b++;
    }
    return *a == 0 && *b == 0;
}

static char scratch[256]; // reusable scratch buffer

// ── tokenise ─────────────────────────────────────────────────────────────────
// Fills ctx->path[] (colon-separated), ctx->query, and saves raw command.
int ScpiParser::tokenise(const char *cmd, CmdCtx *ctx) {
    ctx->depth     = 0;
    ctx->arg_count = 0;
    ctx->query     = false;
    ctx->raw[0]    = 0;

    // save raw uppercase copy for later arg extraction
    char *rd = ctx->raw;
    for (const char *p = cmd; *p && *p != ';' && (size_t)(rd - ctx->raw) < sizeof(ctx->raw) - 2; p++) {
        if (*p == '?' || *p == ':' || *p == ',' || *p == ' ' || *p == '\t')
            *rd++ = *p;
        else if (*p != '\r' && *p != '\n')
            *rd++ = (char)toupper((unsigned char)*p);
    }
    *rd = 0;

    // extract colon-separated path tokens (everything before first space)
    char *bp = scratch;
    const char *src = ctx->raw;

    // Phase 1: copy path tokens (colon-separated) into scratch
    while (*src && *src != ' ') {
        if (*src == '?') {
            ctx->query = true;
        } else if (*src == ':') {
            *bp++ = 0;
        } else {
            *bp++ = *src;
        }
        src++;
    }
    *bp = 0;

    // populate path from scratch
    ctx->depth = 0;
    char *tp = scratch;
    while (*tp && ctx->depth < 8) {
        ctx->path[ctx->depth] = tp;
        ctx->depth++;
        while (*tp) tp++;
        tp++;
    }

    return 0;
}

// ── extract comma-separated args from raw ────────────────────────────────────
void ScpiParser::extract_args(CmdCtx *ctx) {
    ctx->arg_count = 0;
    const char *p = ctx->raw;

    // skip to first space (end of path)
    while (*p && *p != ' ') p++;
    if (!*p) return; // no args
    p++; // skip space

    char buf[64];
    int bi = 0;
    while (*p && ctx->arg_count < SCPI_MAX_ARGS) {
        if (*p == ',') {
            if (bi > 0) {
                buf[bi] = 0;
                strncpy(ctx->args[ctx->arg_count], buf, sizeof(ctx->args[0]) - 1);
                ctx->arg_count++;
                bi = 0;
            }
        } else if (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') {
            // skip whitespace between args
        } else if (*p == '?') {
            // skip
        } else {
            if (bi < (int)sizeof(buf) - 1) buf[bi++] = *p;
        }
        p++;
    }
    if (bi > 0 && ctx->arg_count < SCPI_MAX_ARGS) {
        buf[bi] = 0;
        strncpy(ctx->args[ctx->arg_count], buf, sizeof(ctx->args[0]) - 1);
        ctx->arg_count++;
    }
}

// ── ScpiParser ───────────────────────────────────────────────────────────────
ScpiParser::ScpiParser() {}

void ScpiParser::append(CmdCtx *ctx, const char *s) {
    size_t n = strlen(s);
    if (ctx->out_len + n < ctx->out_cap - 1) {
        memcpy(ctx->out + ctx->out_len, s, n);
        ctx->out_len += n;
    }
}

void ScpiParser::append_f(CmdCtx *ctx, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(ctx->out + ctx->out_len, ctx->out_cap - ctx->out_len - 1, fmt, ap);
    va_end(ap);
    if (n > 0) ctx->out_len += (size_t)n;
    if (ctx->out_len > ctx->out_cap - 1) ctx->out_len = ctx->out_cap - 1;
}

int ScpiParser::execute(const char *cmd, char *out, size_t out_size) {
    if (!cmd || !out || out_size == 0) return 0;
    out[0] = 0;

    CmdCtx ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.out_cap = out_size < sizeof(ctx.out) ? out_size : sizeof(ctx.out);

    // handle multiple commands separated by ;
    const char *p = cmd;
    while (*p) {
        while (*p && (unsigned char)*p <= ' ') p++;
        if (!*p) break;

        const char *end = p;
        while (*end && *end != ';') end++;

        char single[160];
        size_t len = (size_t)(end - p);
        if (len >= sizeof(single)) len = sizeof(single) - 1;
        memcpy(single, p, len);
        single[len] = 0;

        // trim
        char *s = single;
        while (*s && (unsigned char)*s <= ' ') s++;
        char *t = s + strlen(s);
        while (t > s && (unsigned char)*(t - 1) <= ' ') *--t = 0;

        if (*s) {
            tokenise(s, &ctx);
            dispatch(&ctx);
        }

        p = (*end == ';') ? end + 1 : end;
    }

    ctx.out[ctx.out_len] = 0;
    size_t flen = ctx.out_len;
    if (flen + 3 <= out_size) {
        memcpy(out, ctx.out, flen);
        out[flen]   = '\r';
        out[flen+1] = '\n';
        out[flen+2] = 0;
        return (int)flen + 2;
    }
    memcpy(out, ctx.out, flen);
    out[flen] = 0;
    return (int)flen;
}

// ── dispatch ─────────────────────────────────────────────────────────────────
void ScpiParser::dispatch(CmdCtx *ctx) {
    if (ctx->depth == 0) { append(ctx, "-113,\"Undefined header\""); return; }

    const char *r0 = ctx->path[0];

    if (r0[0] == '*') {
        if (ieq(r0 + 1, "IDN"))   { h_idn(ctx); return; }
        if (ieq(r0 + 1, "RST"))   { h_rst(ctx); return; }
        if (ieq(r0 + 1, "CLS"))   { h_cls(ctx); return; }
        append(ctx, "-113,\"Undefined header\"");
        return;
    }

    if (ieq(r0, "SYST")) {
        if (ctx->depth >= 2 && ieq(ctx->path[1], "ERR")) { h_syst_err(ctx); return; }
        append(ctx, "-113,\"Undefined header\"");
        return;
    }

    if (ieq(r0, "CONF")) {
        if (ctx->depth < 2) { append(ctx, "-113,\"Undefined header\""); return; }
        const char *r1 = ctx->path[1];
        if (ieq(r1, "TC")) {
            if (ctx->depth >= 3 && ieq(ctx->path[2], "TYPE")) { h_conf_tc_type(ctx); return; }
            append(ctx, "-113,\"Undefined header\"");
            return;
        }
        if (ieq(r1, "CH")) {
            if (ctx->depth >= 3 && ieq(ctx->path[2], "ENAB")) { h_conf_ch_enab(ctx); return; }
            append(ctx, "-113,\"Undefined header\"");
            return;
        }
        if (ieq(r1, "CJC")) {
            if (ctx->depth >= 3) {
                if (ieq(ctx->path[2], "MODE"))  { h_conf_cjc_mode(ctx); return; }
                if (ieq(ctx->path[2], "VALUE")) { h_conf_cjc_value(ctx); return; }
            }
            append(ctx, "-113,\"Undefined header\"");
            return;
        }
        append(ctx, "-113,\"Undefined header\"");
        return;
    }

    if (ieq(r0, "MEAS")) {
        if (ctx->depth >= 2 && ieq(ctx->path[1], "TEMP")) { h_meas_temp(ctx); return; }
        append(ctx, "-113,\"Undefined header\"");
        return;
    }

    if (ieq(r0, "READ")) { h_read(ctx); return; }
    if (ieq(r0, "INIT")) { h_init(ctx); return; }
    if (ieq(r0, "STOP")) { h_stop(ctx); return; }
    if (ieq(r0, "FREQ")) { h_freq(ctx); return; }

    if (ieq(r0, "DATA")) {
        if (ctx->depth < 2) { append(ctx, "-113,\"Undefined header\""); return; }
        if (ieq(ctx->path[1], "LAST")) { h_data_last(ctx); return; }
        if (ieq(ctx->path[1], "BUF"))  { h_data_buf(ctx); return; }
        append(ctx, "-113,\"Undefined header\"");
        return;
    }

    if (ieq(r0, "CAL")) {
        if (ctx->depth < 2) { append(ctx, "-113,\"Undefined header\""); return; }
        if (ieq(ctx->path[1], "OFFSET")) { h_cal_offset(ctx); return; }
        if (ieq(ctx->path[1], "GAIN"))   { h_cal_gain(ctx); return; }
        if (ieq(ctx->path[1], "RESET"))  { h_cal_reset(ctx); return; }
        append(ctx, "-113,\"Undefined header\"");
        return;
    }

    if (ieq(r0, "STAT")) {
        if (ctx->depth < 2) { append(ctx, "-113,\"Undefined header\""); return; }
        if (ieq(ctx->path[1], "CHAN")) { h_stat_chan(ctx); return; }
        if (ieq(ctx->path[1], "OVER")) { h_stat_over(ctx); return; }
        if (ieq(ctx->path[1], "ADC"))  { h_stat_adc(ctx); return; }
        append(ctx, "-113,\"Undefined header\"");
        return;
    }

    if (ieq(r0, "MEM")) {
        if (ctx->depth < 2) { append(ctx, "-113,\"Undefined header\""); return; }
        if (ieq(ctx->path[1], "STORE"))  { h_mem_store(ctx); return; }
        if (ieq(ctx->path[1], "RECALL")) { h_mem_recall(ctx); return; }
        if (ieq(ctx->path[1], "LIST"))   { h_mem_list(ctx); return; }
        append(ctx, "-113,\"Undefined header\"");
        return;
    }

    append(ctx, "-113,\"Undefined header\"");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  HANDLER IMPLEMENTATIONS
// ═══════════════════════════════════════════════════════════════════════════════

void ScpiParser::h_idn(CmdCtx *ctx) {
    append(ctx, "MyCompany,TC-8CH Thermocouple Logger,SN12345,1.0");
}

void ScpiParser::h_rst(CmdCtx *ctx) {
    chanMgr.reset();
    append(ctx, "0,\"Command OK\"");
}

void ScpiParser::h_cls(CmdCtx *ctx) {
    chanMgr.clear_errors();
    append(ctx, "0,\"Command OK\"");
}

void ScpiParser::h_syst_err(CmdCtx *ctx) {
    int32_t code;
    char msg[48];
    if (chanMgr.err_pop(&code, msg, sizeof(msg)))
        append_f(ctx, "%ld,\"%s\"", (long)code, msg);
    else
        append(ctx, "0,\"No error\"");
}

// ── CONFigure ────────────────────────────────────────────────────────────────

void ScpiParser::h_conf_tc_type(CmdCtx *ctx) {
    extract_args(ctx);
    if (ctx->query) {
        if (ctx->arg_count < 1) { append(ctx, "-224,\"Missing parameter\""); return; }
        int ch = chanMgr.ch_index(ctx->args[0]);
        if (ch < 0) { append(ctx, "-224,\"Illegal channel\""); return; }
        TcType t = chanMgr.get_type(ch);
        if (t == TcType::INVALID) append(ctx, "?");
        else append_f(ctx, "%s", chanMgr.type_str(t));
    } else {
        if (ctx->arg_count < 2) { append(ctx, "-224,\"Missing parameter\""); return; }
        int ch = chanMgr.ch_index(ctx->args[0]);
        if (ch < 0) { append(ctx, "-224,\"Illegal channel\""); return; }
        TcType t = TcType::INVALID;
        const char *s = ctx->args[1];
        if      (ieq(s, "K")) t = TcType::K;
        else if (ieq(s, "J")) t = TcType::J;
        else if (ieq(s, "T")) t = TcType::T;
        else if (ieq(s, "E")) t = TcType::E;
        else if (ieq(s, "N")) t = TcType::N;
        else if (ieq(s, "R")) t = TcType::R;
        else if (ieq(s, "S")) t = TcType::S;
        else if (ieq(s, "B")) t = TcType::B;
        if (t == TcType::INVALID) { append(ctx, "-224,\"Illegal parameter value\""); return; }
        chanMgr.set_type(ch, t);
        append(ctx, "0,\"Command OK\"");
    }
}

void ScpiParser::h_conf_ch_enab(CmdCtx *ctx) {
    extract_args(ctx);
    if (ctx->query) {
        if (ctx->arg_count < 1) { append(ctx, "-224,\"Missing parameter\""); return; }
        int ch = chanMgr.ch_index(ctx->args[0]);
        if (ch < 0) { append(ctx, "-224,\"Illegal channel\""); return; }
        append_f(ctx, "%d", chanMgr.get_enabled(ch) ? 1 : 0);
    } else {
        if (ctx->arg_count < 2) { append(ctx, "-224,\"Missing parameter\""); return; }
        int ch = chanMgr.ch_index(ctx->args[0]);
        if (ch < 0) { append(ctx, "-224,\"Illegal channel\""); return; }
        int en = atoi(ctx->args[1]);
        chanMgr.set_enabled(ch, en != 0);
        append(ctx, "0,\"Command OK\"");
    }
}

void ScpiParser::h_conf_cjc_mode(CmdCtx *ctx) {
    extract_args(ctx);
    if (ctx->query) {
        append(ctx, chanMgr.get_cjc_mode() ? "EXT" : "INT");
    } else {
        if (ctx->arg_count == 0) { append(ctx, "-224,\"Missing parameter\""); return; }
        if (ieq(ctx->args[0], "EXT")) { chanMgr.set_cjc_mode(true); }
        else if (ieq(ctx->args[0], "INT")) { chanMgr.set_cjc_mode(false); }
        else { append(ctx, "-224,\"Illegal parameter value\""); return; }
        append(ctx, "0,\"Command OK\"");
    }
}

void ScpiParser::h_conf_cjc_value(CmdCtx *ctx) {
    extract_args(ctx);
    if (ctx->query) {
        append_f(ctx, "%.2f", (double)chanMgr.get_cjc_value());
    } else {
        if (ctx->arg_count == 0) { append(ctx, "-224,\"Missing parameter\""); return; }
        float v = (float)atof(ctx->args[0]);
        if (v < -50.0f || v > 150.0f) { append(ctx, "-224,\"Illegal parameter value\""); return; }
        chanMgr.set_cjc_value(v);
        append(ctx, "0,\"Command OK\"");
    }
}

// ── MEASure ──────────────────────────────────────────────────────────────────

void ScpiParser::h_meas_temp(CmdCtx *ctx) {
    extract_args(ctx);
    if (ctx->arg_count == 0) { append(ctx, "-224,\"Missing parameter\""); return; }
    if (ieq(ctx->args[0], "ALL")) {
        float vals[TC_NUM_CHANNELS];
        chanMgr.measure_all(vals);
        for (int i = 0; i < TC_NUM_CHANNELS; i++) {
            if (i > 0) append(ctx, ", ");
            append_f(ctx, "%.2f", (double)vals[i]);
        }
    } else {
        int ch = chanMgr.ch_index(ctx->args[0]);
        if (ch < 0) { append(ctx, "-224,\"Illegal channel\""); return; }
        float v = chanMgr.measure(ch);
        append_f(ctx, "%.2f", (double)v);
    }
}

void ScpiParser::h_read(CmdCtx *ctx) {
    for (int i = 0; i < TC_NUM_CHANNELS; i++) {
        if (i > 0) append(ctx, ", ");
        append_f(ctx, "%.2f", (double)chanMgr.channels[i].last_value);
    }
}

// ── INIT / STOP / FREQ ───────────────────────────────────────────────────────

void ScpiParser::h_init(CmdCtx *ctx) {
    chanMgr.start();
    append(ctx, "0,\"Command OK\"");
}

void ScpiParser::h_stop(CmdCtx *ctx) {
    chanMgr.stop();
    append(ctx, "0,\"Command OK\"");
}

void ScpiParser::h_freq(CmdCtx *ctx) {
    extract_args(ctx);
    if (ctx->query) {
        append_f(ctx, "%.1f", (double)chanMgr.get_freq());
    } else {
        if (ctx->arg_count == 0) { append(ctx, "-224,\"Missing parameter\""); return; }
        float hz = (float)atof(ctx->args[0]);
        chanMgr.set_freq(hz);
        append(ctx, "0,\"Command OK\"");
    }
}

// ── DATA ─────────────────────────────────────────────────────────────────────

void ScpiParser::h_data_last(CmdCtx *ctx) {
    h_read(ctx);
}

void ScpiParser::h_data_buf(CmdCtx *ctx) {
    append(ctx, "0,\"Not implemented\"");
}

// ── CALibration ──────────────────────────────────────────────────────────────

void ScpiParser::h_cal_offset(CmdCtx *ctx) {
    extract_args(ctx);
    if (ctx->arg_count < 2) { append(ctx, "-224,\"Missing parameter\""); return; }
    int ch = chanMgr.ch_index(ctx->args[0]);
    if (ch < 0) { append(ctx, "-224,\"Illegal channel\""); return; }
    float v = (float)atof(ctx->args[1]);
    chanMgr.set_offset(ch, v);
    append(ctx, "0,\"Command OK\"");
}

void ScpiParser::h_cal_gain(CmdCtx *ctx) {
    extract_args(ctx);
    if (ctx->arg_count < 2) { append(ctx, "-224,\"Missing parameter\""); return; }
    int ch = chanMgr.ch_index(ctx->args[0]);
    if (ch < 0) { append(ctx, "-224,\"Illegal channel\""); return; }
    float v = (float)atof(ctx->args[1]);
    if (v <= 0.0f) { append(ctx, "-224,\"Gain must be > 0\""); return; }
    chanMgr.set_gain(ch, v);
    append(ctx, "0,\"Command OK\"");
}

void ScpiParser::h_cal_reset(CmdCtx *ctx) {
    extract_args(ctx);
    if (ctx->arg_count < 1) { append(ctx, "-224,\"Missing parameter\""); return; }
    int ch = chanMgr.ch_index(ctx->args[0]);
    if (ch < 0) { append(ctx, "-224,\"Illegal channel\""); return; }
    chanMgr.reset_cal(ch);
    append(ctx, "0,\"Command OK\"");
}

// ── STATus ───────────────────────────────────────────────────────────────────

void ScpiParser::h_stat_chan(CmdCtx *ctx) {
    extract_args(ctx);
    if (ctx->arg_count < 1) { append(ctx, "-224,\"Missing parameter\""); return; }
    int ch = chanMgr.ch_index(ctx->args[0]);
    if (ch < 0) { append(ctx, "-224,\"Illegal channel\""); return; }
    switch (chanMgr.get_channel_status(ch)) {
        case ChanStatus::OK:    append(ctx, "OK");    break;
        case ChanStatus::OPEN:  append(ctx, "OPEN");  break;
        case ChanStatus::SHORT: append(ctx, "SHORT"); break;
    }
}

void ScpiParser::h_stat_over(CmdCtx *ctx) {
    for (int i = 0; i < TC_NUM_CHANNELS; i++) {
        if (chanMgr.is_overrange(i)) { append(ctx, "1"); return; }
    }
    append(ctx, "0");
}

void ScpiParser::h_stat_adc(CmdCtx *ctx) {
    extract_args(ctx);
    if (ctx->arg_count > 0) {
        int ch = chanMgr.ch_index(ctx->args[0]);
        if (ch < 0) { append(ctx, "-224,\"Illegal channel\""); return; }
        append_f(ctx, "%.2f", (double)chanMgr.channels[ch].last_raw);
    } else {
        for (int i = 0; i < TC_NUM_CHANNELS; i++) {
            if (i > 0) append(ctx, ", ");
            append_f(ctx, "%.2f", (double)chanMgr.channels[i].last_raw);
        }
    }
}

// ── MEMory ───────────────────────────────────────────────────────────────────

void ScpiParser::h_mem_store(CmdCtx *ctx) {
    extract_args(ctx);
    if (ctx->arg_count < 1) { append(ctx, "-224,\"Missing parameter\""); return; }
    int slot = atoi(ctx->args[0]);
    if (slot < 0 || slot > 9) { append(ctx, "-224,\"Slot 0-9\""); return; }
    chanMgr.store((uint8_t)slot);
    append_f(ctx, "0,\"Stored slot %d\"", slot);
}

void ScpiParser::h_mem_recall(CmdCtx *ctx) {
    extract_args(ctx);
    if (ctx->arg_count < 1) { append(ctx, "-224,\"Missing parameter\""); return; }
    int slot = atoi(ctx->args[0]);
    if (slot < 0 || slot > 9) { append(ctx, "-224,\"Slot 0-9\""); return; }
    if (chanMgr.recall((uint8_t)slot))
        append_f(ctx, "0,\"Recalled slot %d\"", slot);
    else
        append(ctx, "-230,\"Slot empty\"");
}

void ScpiParser::h_mem_list(CmdCtx *ctx) {
    append(ctx, "NOT IMPLEMENTED");
}
