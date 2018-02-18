#ifndef __SFXR_HPP__
#define __SFXR_HPP__
#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <cstring>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>

#define rnd(n) (rand()%(n+1))

#define PI 3.14159265f

using namespace std;

struct SoundParams {
    float p_env_attack;
    float p_env_sustain;
    float p_env_punch;
    float p_env_decay;

    float p_base_freq;
    float p_freq_limit;
    float p_freq_ramp;
    float p_freq_dramp;

    float p_vib_strength;
    float p_vib_speed;

    float p_arp_mod;
    float p_arp_speed;

    float p_duty;
    float p_duty_ramp;

    float p_repeat_speed;

    float p_pha_offset;
    float p_pha_ramp;

    float p_lpf_freq;
    float p_lpf_ramp;
    float p_lpf_resonance;

    float p_hpf_freq;
    float p_hpf_ramp;

    float sound_vol;
    int sample_rate;
    int bit_depth;
    int wave_type;
};

float frnd(float range);

class SfxrSound;


void ResetSample(bool restart);

void SynthSample(int length, float* buffer);

static void SDLAudioCallback(void *userdata, Uint8 *stream, int len);

void initSfxr();

class SfxrSound
{
    public:

    float *fbuf;
	// Params
	int wave_type;

	float p_base_freq;
	float p_freq_limit;
	float p_freq_ramp;
	float p_freq_dramp;
	float p_duty;
	float p_duty_ramp;

	float p_vib_strength;
	float p_vib_speed;
	float p_vib_delay;

	float p_env_attack;
	float p_env_sustain;
	float p_env_decay;
	float p_env_punch;

	float p_lpf_resonance;
	float p_lpf_freq;
	float p_lpf_ramp;
	float p_hpf_freq;
	float p_hpf_ramp;

	float p_pha_offset;
	float p_pha_ramp;

	float p_repeat_speed;

	float p_arp_speed;
	float p_arp_mod;

    float sound_vol;

    int sample_rate;
    int bit_depth;

	// Constructors
	SfxrSound();

	SfxrSound(
		float p_env_attack,
		float p_env_sustain,
		float p_env_punch,
		float p_env_decay,

		float p_base_freq,
		float p_freq_limit,
		float p_freq_ramp,
		float p_freq_dramp,

		float p_vib_strength,
		float p_vib_speed,

		float p_arp_mod,
		float p_arp_speed,

		float p_duty,
		float p_duty_ramp,

		float p_repeat_speed,

		float p_pha_offset,
		float p_pha_ramp,

		float p_lpf_freq,
		float p_lpf_ramp,
        float p_lpf_resonance,

		float p_hpf_freq,
		float p_hpf_ramp,

		float sound_vol,
        int sample_rate,
        int bit_depth,

        int wave_type
	);
    ~SfxrSound();
	void ResetParams();
	void generateSound();

	void playSample();
	void trace(bool traceBuffer);
};

#endif
