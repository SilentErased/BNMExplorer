#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <string>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

extern "C" int stb_vorbis_decode_memory(const unsigned char* mem, int len, int* channels, int* sample_rate, short** output);

extern "C" void bnm_audio_free_pcm(void* ptr) {
    free(ptr);
}

static const unsigned long long bnm_audio_max_samples = 32000000ULL;

extern "C" int bnm_audio_decode(const uint8_t* data, size_t size, float** out_pcm, long* out_frames, int* out_channels, int* out_rate, char* err, size_t err_size) {
    *out_pcm = nullptr;
    *out_frames = 0;
    *out_channels = 0;
    *out_rate = 0;
    if (!data || size < 16) {
        snprintf(err, err_size, "data is too small to be audio");
        return 0;
    }
    bool is_wav = size >= 12 && !memcmp(data, "RIFF", 4) && !memcmp(data + 8, "WAVE", 4);
    bool is_ogg = size >= 4 && !memcmp(data, "OggS", 4);
    bool is_mp3 = (size >= 2 && data[0] == 0xFF && (data[1] & 0xE0) == 0xE0) || (size >= 3 && !memcmp(data, "ID3", 3));
    (void)is_mp3;
    if (is_wav) {
        unsigned int channels = 0;
        unsigned int rate = 0;
        drwav_uint64 frames = 0;
        float* pcm = drwav_open_memory_and_read_pcm_frames_f32(data, size, &channels, &rate, &frames, nullptr);
        if (!pcm) {
            snprintf(err, err_size, "wav decode failed");
            return 0;
        }
        if (!frames || !channels || !rate) {
            drwav_free(pcm, nullptr);
            snprintf(err, err_size, "wav file contains no audio");
            return 0;
        }
        if (frames * channels > bnm_audio_max_samples) {
            drwav_free(pcm, nullptr);
            snprintf(err, err_size, "audio is too long (over 32M samples)");
            return 0;
        }
        *out_pcm = pcm;
        *out_frames = (long)frames;
        *out_channels = (int)channels;
        *out_rate = (int)rate;
        return 1;
    }
    if (is_ogg) {
        int channels = 0;
        int rate = 0;
        short* raw = nullptr;
        int frames = stb_vorbis_decode_memory(data, (int)size, &channels, &rate, &raw);
        if (!raw || frames <= 0 || !channels || !rate) {
            if (raw) free(raw);
            snprintf(err, err_size, "ogg decode failed");
            return 0;
        }
        if ((long long)frames * channels > (long long)bnm_audio_max_samples) {
            free(raw);
            snprintf(err, err_size, "audio is too long (over 32M samples)");
            return 0;
        }
        float* pcm = (float*)malloc(sizeof(float) * (size_t)frames * (size_t)channels);
        if (!pcm) {
            free(raw);
            snprintf(err, err_size, "out of memory");
            return 0;
        }
        for (long i = 0; i < (long)frames * channels; i++) pcm[i] = raw[i] / 32768.0f;
        free(raw);
        *out_pcm = pcm;
        *out_frames = frames;
        *out_channels = channels;
        *out_rate = rate;
        return 1;
    }
    if (true) {
        drmp3 mp3;
        if (!drmp3_init_memory(&mp3, data, (size_t)size, nullptr)) {
            snprintf(err, err_size, "unknown audio format (not wav, ogg or mp3)");
            return 0;
        }
        drmp3_uint64 frames = drmp3_get_pcm_frame_count(&mp3);
        int channels = (int)mp3.channels;
        int rate = (int)mp3.sampleRate;
        if (!frames || !channels || !rate) {
            drmp3_uninit(&mp3);
            snprintf(err, err_size, "mp3 contains no audio");
            return 0;
        }
        if (frames * (drmp3_uint64)channels > bnm_audio_max_samples) {
            drmp3_uninit(&mp3);
            snprintf(err, err_size, "audio is too long (over 32M samples)");
            return 0;
        }
        float* pcm = (float*)malloc(sizeof(float) * (size_t)frames * (size_t)channels);
        if (!pcm) {
            drmp3_uninit(&mp3);
            snprintf(err, err_size, "out of memory");
            return 0;
        }
        drmp3_uint64 got = drmp3_read_pcm_frames_f32(&mp3, frames, pcm);
        drmp3_uninit(&mp3);
        if (!got) {
            free(pcm);
            snprintf(err, err_size, "mp3 decode failed");
            return 0;
        }
        *out_pcm = pcm;
        *out_frames = (long)got;
        *out_channels = channels;
        *out_rate = rate;
        return 1;
    }
    snprintf(err, err_size, "unknown audio format");
    return 0;
}
