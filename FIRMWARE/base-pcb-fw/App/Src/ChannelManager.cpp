#include "ChannelManager.hpp"
#include <cstdio>
#include <cstdarg>
#include <cmath>
#include <cstring>
#include <cctype>

ChannelManager chanMgr;

static const char *tc_names[] = { "K", "J", "T", "E", "N", "R", "S", "B" };

const char *ChannelManager::tc_type_str(TcType t) {
    int idx = (int)t;
    if (idx < 0 || idx > 7) return "?";
    return tc_names[idx];
}

TcType ChannelManager::tc_type_from_str(const char *s) {
    for (int i = 0; i < 8; i++) {
        const char *a = s;
        const char *b = tc_names[i];
        bool match = true;
        while (*a && *b) {
            if (toupper((unsigned char)*a) != toupper((unsigned char)*b)) {
                match = false;
                break;
            }
            a++; b++;
        }
        if (match && *a == 0 && *b == 0)
            return (TcType)i;
    }
    return TcType::INVALID;
}

void ChannelManager::reset() {
    for (int i = 0; i < TC_NUM_CHANNELS; i++) {
        channels[i].enabled   = (i < 4);  // first 4 on by default
        channels[i].type      = TcType::K;
        channels[i].offset    = 0.0f;
        channels[i].gain      = 1.0f;
        channels[i].last_raw  = 0.0f;
        channels[i].last_value = 0.0f;
        channels[i].status    = ChanStatus::OK;
    }
    cjc_ext_mode  = false;
    cjc_value     = 25.0f;
    sampling_freq = 1.0f;
    sampling_active = false;
    err_head = err_tail = err_count = 0;

    for (int i = 0; i < TC_NUM_CHANNELS; i++)
        last_snapshot[i] = 0.0f;
}

int ChannelManager::ch_index(const char *name) {
    if (!name) return -1;
    // skip "CH" prefix
    while (*name == 'c' || *name == 'C') name++;
    while (*name == 'h' || *name == 'H') name++;
    int n = 0;
    while (*name >= '0' && *name <= '9')
        n = n * 10 + (*name++ - '0');
    if (n < 1 || n > TC_NUM_CHANNELS) return -1;
    return n - 1;
}

bool ChannelManager::set_type(int ch, TcType t) {
    if (ch < 0 || ch >= TC_NUM_CHANNELS || t == TcType::INVALID) return false;
    channels[ch].type = t;
    return true;
}

TcType ChannelManager::get_type(int ch) {
    if (ch < 0 || ch >= TC_NUM_CHANNELS) return TcType::INVALID;
    return channels[ch].type;
}

bool ChannelManager::set_enabled(int ch, bool en) {
    if (ch < 0 || ch >= TC_NUM_CHANNELS) return false;
    channels[ch].enabled = en;
    return true;
}

bool ChannelManager::get_enabled(int ch) {
    if (ch < 0 || ch >= TC_NUM_CHANNELS) return false;
    return channels[ch].enabled;
}

void ChannelManager::set_offset(int ch, float val) {
    if (ch >= 0 && ch < TC_NUM_CHANNELS)
        channels[ch].offset = val;
}

float ChannelManager::get_offset(int ch) {
    if (ch >= 0 && ch < TC_NUM_CHANNELS)
        return channels[ch].offset;
    return 0.0f;
}

void ChannelManager::set_gain(int ch, float val) {
    if (ch >= 0 && ch < TC_NUM_CHANNELS)
        channels[ch].gain = val;
}

float ChannelManager::get_gain(int ch) {
    if (ch >= 0 && ch < TC_NUM_CHANNELS)
        return channels[ch].gain;
    return 1.0f;
}

void ChannelManager::reset_cal(int ch) {
    if (ch >= 0 && ch < TC_NUM_CHANNELS) {
        channels[ch].offset = 0.0f;
        channels[ch].gain = 1.0f;
    }
}

// --- Measurement simulation ---
void ChannelManager::simulate_measurement(int ch) {
    // simulated raw value based on channel number + time variation
    // In real implementation: read ADC -> convert via TC lookup table -> compensate CJC
    float t = (float)(ch * 5) + 20.0f + (float)(ch * 3) * 0.1f;
    channels[ch].last_raw = t;
    channels[ch].last_value = t * channels[ch].gain + channels[ch].offset;
    channels[ch].status = ChanStatus::OK;
}

float ChannelManager::measure(int ch) {
    if (ch < 0 || ch >= TC_NUM_CHANNELS || !channels[ch].enabled)
        return 0.0f;
    simulate_measurement(ch);
    last_snapshot[ch] = channels[ch].last_value;
    return channels[ch].last_value;
}

void ChannelManager::measure_all(float *out) {
    for (int i = 0; i < TC_NUM_CHANNELS; i++) {
        if (channels[i].enabled)
            out[i] = measure(i);
        else
            out[i] = 0.0f;
    }
}

void ChannelManager::set_cjc_mode(bool ext) { cjc_ext_mode = ext; }
bool ChannelManager::get_cjc_mode() { return cjc_ext_mode; }
void ChannelManager::set_cjc_value(float v) { cjc_value = v; }
float ChannelManager::get_cjc_value() { return cjc_value; }

void ChannelManager::clear_errors() {
    err_head = err_tail = err_count = 0;
}

void ChannelManager::set_freq(float hz) {
    if (hz >= 0.1f && hz <= 1000.0f)
        sampling_freq = hz;
}
float ChannelManager::get_freq() { return sampling_freq; }
void ChannelManager::start() { sampling_active = true; }
void ChannelManager::stop()  { sampling_active = false; }
bool ChannelManager::is_active() { return sampling_active; }

ChanStatus ChannelManager::get_channel_status(int ch) {
    if (ch < 0 || ch >= TC_NUM_CHANNELS) return ChanStatus::OPEN;
    return channels[ch].status;
}

bool ChannelManager::is_overrange(int ch) {
    if (ch < 0 || ch >= TC_NUM_CHANNELS) return false;
    return (channels[ch].last_value > 2000.0f || channels[ch].last_value < -200.0f);
}

void ChannelManager::err_push(int32_t code, const char *fmt, ...) {
    if (err_count >= MAX_ERRORS) return;
    va_list ap;
    va_start(ap, fmt);
    err_queue[err_head].code = code;
    vsnprintf(err_queue[err_head].msg, sizeof(err_queue[err_head].msg), fmt, ap);
    va_end(ap);
    err_head = (err_head + 1) % MAX_ERRORS;
    err_count++;
}

bool ChannelManager::err_pop(int32_t *code, char *msg, size_t msg_len) {
    if (err_count == 0) return false;
    *code = err_queue[err_tail].code;
    strncpy(msg, err_queue[err_tail].msg, msg_len);
    msg[msg_len - 1] = 0;
    err_tail = (err_tail + 1) % MAX_ERRORS;
    err_count--;
    return true;
}

// --- Memory slots ---
static struct {
    bool used;
    ChannelConfig channels[TC_NUM_CHANNELS];
    bool cjc_ext;
    float cjc_val;
} mem_slots[10];

void ChannelManager::store(uint8_t slot) {
    if (slot >= 10) return;
    mem_slots[slot].used = true;
    memcpy(mem_slots[slot].channels, channels, sizeof(channels));
    mem_slots[slot].cjc_ext = cjc_ext_mode;
    mem_slots[slot].cjc_val = cjc_value;
    err_push(0, "Stored slot %u", slot);
}

bool ChannelManager::recall(uint8_t slot) {
    if (slot >= 10 || !mem_slots[slot].used) {
        err_push(-220, "Slot %u empty", slot);
        return false;
    }
    memcpy(channels, mem_slots[slot].channels, sizeof(channels));
    cjc_ext_mode = mem_slots[slot].cjc_ext;
    cjc_value    = mem_slots[slot].cjc_val;
    err_push(0, "Recalled slot %u", slot);
    return true;
}
