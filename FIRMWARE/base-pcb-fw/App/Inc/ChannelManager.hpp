#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>

#define TC_NUM_CHANNELS  8

enum class TcType : uint8_t {
    K = 0, J, T, E, N, R, S, B, INVALID = 0xFF
};

enum class ChanStatus : uint8_t {
    OK = 0, OPEN, SHORT
};

struct ChannelConfig {
    bool     enabled;
    TcType   type;
    float    offset;
    float    gain;
    float    last_raw;     // raw ADC value
    float    last_value;   // compensated temperature
    ChanStatus status;
};

struct ErrorEntry {
    int32_t  code;
    char     msg[48];
};

class ChannelManager {
public:
    static constexpr int MAX_ERRORS = 16;

    ChannelConfig channels[TC_NUM_CHANNELS];
    bool cjc_ext_mode;       // false=INT, true=EXT
    float cjc_value;         // external CJC temperature
    float sampling_freq;
    bool sampling_active;

    void reset();

    int  ch_index(const char *name);

    // channel config
    bool set_type(int ch, TcType t);
    TcType get_type(int ch);
    bool set_enabled(int ch, bool en);
    bool get_enabled(int ch);

    // calibration
    void set_offset(int ch, float val);
    float get_offset(int ch);
    void set_gain(int ch, float val);
    float get_gain(int ch);
    void reset_cal(int ch);

    // measurement simulation
    float measure(int ch);
    void measure_all(float *out);

    // CJC
    void set_cjc_mode(bool ext);
    bool get_cjc_mode();
    void set_cjc_value(float v);
    float get_cjc_value();

    // sampling
    void set_freq(float hz);
    float get_freq();
    void start();
    void stop();
    bool is_active();

    // status
    ChanStatus get_channel_status(int ch);
    bool is_overrange(int ch);

    // error queue
    void err_push(int32_t code, const char *fmt, ...);
    bool err_pop(int32_t *code, char *msg, size_t msg_len);

    // memory slots
    void store(uint8_t slot);
    bool recall(uint8_t slot);

    static const char *type_str(TcType t) { return tc_type_str(t); }
    void clear_errors();

private:
    ErrorEntry err_queue[MAX_ERRORS];
    int err_head = 0;
    int err_tail = 0;
    int err_count = 0;

    float last_snapshot[TC_NUM_CHANNELS];

    void simulate_measurement(int ch);

    static constexpr float TC_MILLIVOLTS[8][41] = {};
    static float tc_seebeck(TcType type, float temp_c);

    static const char *tc_type_str(TcType t);
    static TcType tc_type_from_str(const char *s);
};

extern ChannelManager chanMgr;
