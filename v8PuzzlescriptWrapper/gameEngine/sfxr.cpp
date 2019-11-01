#include <iostream>
#include <vector>
#include "sfxr.hpp"

using namespace std;

int bufferLength;

double gain;
double sound_vol = 0.5;
float sfxr_master_vol =0.05f;
bool playing_sample=false;

int phase;
double fperiod;
double fmaxperiod;
double fslide;
double fdslide;
int period;
float square_duty;
float square_slide;
int env_stage;
int env_time;
int env_length[3];
int env_total_length;
float env_vol;
float fphase;
float fdphase;
int iphase;
float phaser_buffer[1024];
int ipp;
float noise_buffer[32];
float fltp;
float fltdp;
float fltw;
float fltw_d;
float fltdmp;
float fltphp;
float flthp;
float flthp_d;
float vib_phase;
float vib_speed;
float vib_amp;
int rep_time;
int rep_limit;
int arp_time;
int arp_limit;
double arp_mod;


float* vselected=NULL;
int vcurbutton=-1;

SDL_AudioSpec des;

SfxrSound* sound=NULL;

float frnd(float range)
{
	return (float)rnd(10000)/10000*range;
}


void repeat() {
    rep_time = 0;
    fperiod = 100.0 / (sound->p_base_freq * sound->p_base_freq + 0.001);
    period = (int)fperiod;
    fmaxperiod = 100.0 / (sound->p_freq_limit * sound->p_freq_limit + 0.001);
    fslide = 1.0 - pow((double)sound->p_freq_ramp, 3.0) * 0.01;
    fdslide = -pow((double)sound->p_freq_dramp, 3.0) * 0.000001;
    square_duty = 0.5f - sound->p_duty * 0.5f;
    square_slide = -sound->p_duty_ramp * 0.00005f;
    if (sound->p_arp_mod >= 0.0f)
    {
        arp_mod = 1.0 - pow((double)sound->p_arp_mod, 2.0) * 0.9;
    }
    else
    {
        arp_mod = 1.0 + pow((double)sound->p_arp_mod, 2.0) * 10.0;
    }
    arp_time = 0;
    arp_limit = (int)(pow(1.0f - sound->p_arp_speed, 2.0f) * 20000 + 32);
    if (sound->p_arp_speed == 1.0f)
    {
        arp_limit = 0;
    }
 };

void ResetSample(bool restart)
{
    if(!restart)
    {
        phase=0;
    }

    repeat();
    if(!restart)
    {
        // reset filter
        fltp=0.0f;
        fltdp=0.0f;
        fltw=pow(sound->p_lpf_freq, 3.0f)*0.1f;
        fltw_d=1.0f+sound->p_lpf_ramp*0.0001f;
        fltdmp=5.0f/(1.0f+pow(sound->p_lpf_resonance, 2.0f)*20.0f)*(0.01f+fltw);
        if(fltdmp>0.8f) fltdmp=0.8f;
        fltphp=0.0f;
        flthp=pow(sound->p_hpf_freq, 2.0f)*0.1f;
        flthp_d=1.0+sound->p_hpf_ramp*0.0003f;
        // reset vibrato
        vib_phase=0.0f;
        vib_speed=pow(sound->p_vib_speed, 2.0f)*0.01f;
        vib_amp=sound->p_vib_strength*0.5f;
        // reset envelope
        env_vol=0.0f;
        env_stage=0;
        env_time=0;
        env_length[0]=(int)(sound->p_env_attack*sound->p_env_attack*100000.0f);
        env_length[1]=(int)(sound->p_env_sustain*sound->p_env_sustain*100000.0f);
        env_length[2]=(int)(sound->p_env_decay*sound->p_env_decay*100000.0f);

        env_total_length = env_length[0] + env_length[1] + env_length[2];

        fphase=pow(sound->p_pha_offset, 2.0f)*1020.0f;
        if(sound->p_pha_offset<0.0f) fphase=-fphase;
        fdphase=pow(sound->p_pha_ramp, 2.0f)*1.0f;
        if(sound->p_pha_ramp<0.0f) fdphase=-fdphase;
        iphase=abs((int)fphase);
        ipp=0;
        for(int i=0;i<1024;i++)
            phaser_buffer[i]=0.0f;

        for(int i=0;i<32;i++)
            noise_buffer[i]=frnd(2.0f)-1.0f;

        rep_time=0;
        rep_limit=(int)(pow(1.0f-sound->p_repeat_speed, 2.0f)*20000+32);
        if(sound->p_repeat_speed==0.0f)
            rep_limit=0;
    }
}

void SynthSample(int length, float* buffer)
{
	for(int i=0;i<length;i++)
	{
		if(!playing_sample)
			break;

		rep_time++;
		if(rep_limit!=0 && rep_time>=rep_limit)
		{
			rep_time=0;
			ResetSample(true);
		}

		// frequency envelopes/arpeggios
		arp_time++;
		if(arp_limit!=0 && arp_time>=arp_limit)
		{
			arp_limit=0;
			fperiod*=arp_mod;
		}
		fslide+=fdslide;
		fperiod*=fslide;
		if(fperiod>fmaxperiod)
		{
			fperiod=fmaxperiod;
			if(sound->p_freq_limit>0.0f)
				playing_sample=false;
		}
		float rfperiod=fperiod;
		if(vib_amp>0.0f)
		{
			vib_phase+=vib_speed;
			rfperiod=fperiod*(1.0+sin(vib_phase)*vib_amp);
		}
		period=(int)rfperiod;
		if(period<8) period=8;
		square_duty+=square_slide;
		if(square_duty<0.0f) square_duty=0.0f;
		if(square_duty>0.5f) square_duty=0.5f;
		// volume envelope
		env_time++;
		if(env_time>env_length[env_stage])
		{
			env_time=0;
			env_stage++;
			if(env_stage==3)
				playing_sample=false;
		}
		if(env_stage==0)
			env_vol=(float)env_time/env_length[0];
		if(env_stage==1)
			env_vol=1.0f+pow(1.0f-(float)env_time/env_length[1], 1.0f)*2.0f*sound->p_env_punch;
		if(env_stage==2)
			env_vol=1.0f-(float)env_time/env_length[2];

		// phaser step
		fphase+=fdphase;
		iphase=abs((int)fphase);
		if(iphase>1023) iphase=1023;

		if(flthp_d!=0.0f)
		{
			flthp*=flthp_d;
			if(flthp<0.00001f) flthp=0.00001f;
			if(flthp>0.1f) flthp=0.1f;
		}

		float ssample=0.0f;
		for(int si=0;si<8;si++) // 8x supersampling
		{
			float sample=0.0f;
			phase++;
			if(phase>=period)
			{
//				phase=0;
				phase%=period;
				if(sound->wave_type==3)
					for(int i=0;i<32;i++)
						noise_buffer[i]=frnd(2.0f)-1.0f;
			}
			// base waveform
			float fp=(float)phase/period;
			switch(sound->wave_type)
			{
                case SQUARE: // square
                    if(fp<square_duty)
                        sample=0.5f;
                    else
                        sample=-0.5f;
                break;
                case SAWTOOTH: // sawtooth
                    sample=1.0f-fp*2;
                break;
                case SINE: // sine
                    sample=(float)sin(fp*2*PI);
                break;
                case NOISE: // noise
                    sample=noise_buffer[phase*32/period];
                break;
                case TRIANGLE: // triangle
                    sample = abs(1 - fp * 2) - 1;
                break;
                case BREAKER: // breaker
                    sample = abs(1 - fp * fp * 2) - 1;
                break;
			}
			// lp filter
			float pp=fltp;
			fltw*=fltw_d;
			if(fltw<0.0f) fltw=0.0f;
			if(fltw>0.1f) fltw=0.1f;
			if(sound->p_lpf_freq!=1.0f)
			{
				fltdp+=(sample-fltp)*fltw;
				fltdp-=fltdp*fltdmp;
			}
			else
			{
				fltp=sample;
				fltdp=0.0f;
			}
			fltp+=fltdp;
			// hp filter
			fltphp+=fltp-pp;
			fltphp-=fltphp*flthp;
			sample=fltphp;
			// phaser
			phaser_buffer[ipp&1023]=sample;
			sample+=phaser_buffer[(ipp-iphase+1024)&1023];
			ipp=(ipp+1)&1023;
			// final accumulation and envelope application
			ssample+=sample*env_vol;
		}
		ssample=ssample/8*sfxr_master_vol;

		ssample*=2.0f*sound->sound_vol;

		if(buffer!=NULL)
		{
			if(ssample>1.0f) ssample=1.0f;
			if(ssample<-1.0f) ssample=-1.0f;
			*buffer++=ssample;
		}
	}
}

std::vector<float> generate()
{
    std::vector<float> output;

    // Misnomer, repeat resets things at start
    repeat();

    // Filter
    fltp = 0.0f;
    fltdp = 0.0f;
    fltw = pow(sound->p_lpf_freq, 3.0f) * 0.1f;
    fltw_d = 1.0f + sound->p_lpf_ramp * 0.0001f;
    fltdmp = 5.0f / (1.0f + pow(sound->p_lpf_resonance, 2.0f) * 20.0f) *
             (0.01f + fltw);
    if (fltdmp > 0.8f)
    {
        fltdmp = 0.8f;
    }
    fltphp = 0.0f;
    flthp = pow(sound->p_hpf_freq, 2.0f) * 0.1f;
    flthp_d = 1.0 + sound->p_hpf_ramp * 0.0003f;

    // Vibrato
    vib_phase = 0.0f;
    vib_speed = pow(sound->p_vib_speed, 2.0f) * 0.01f;
    vib_amp = sound->p_vib_strength * 0.5f;

    // Envelope
    env_vol = 0.0f;
    env_stage = 0;
    env_time = 0;
    env_length[0] = (int)(sound->p_env_attack * sound->p_env_attack * 100000.0f);
    env_length[1] =
        (int)(sound->p_env_sustain * sound->p_env_sustain * 100000.0f);
    env_length[2] = (int)(sound->p_env_decay * sound->p_env_decay * 100000.0f);
    env_total_length = env_length[0] + env_length[1] + env_length[2];

    // Phaser
    phase = 0;
    fphase = pow(sound->p_pha_offset, 2.0f) * 1020.0f;
    if (sound->p_pha_offset < 0.0f)
    {
        fphase = -fphase;
    }
    fdphase = pow(sound->p_pha_ramp, 2.0f) * 1.0f;
    if (sound->p_pha_ramp < 0.0f)
    {
        fdphase = -fdphase;
    }
    iphase = abs((int)fphase);
    ipp = 0;
    for (int i = 0; i < 1024; i++)
    {
        phaser_buffer[i] = 0.0f;
    }

    // Noise
    for (int i = 0; i < 32; i++)
    {
        noise_buffer[i] = frnd(2.0f) - 1.0f;
    }

    // Repeat
    rep_limit = (int)(pow(1.0f - sound->p_repeat_speed, 2.0f) * 20000 + 32);
    if (sound->p_repeat_speed == 0.0f)
    {
        rep_limit = 0;
    }

    gain = exp(sound_vol) - 1.0;

    // Generate
    bool buffer_complete = false;
    int buffer_length = env_total_length;
    for (int t = 0; ; ++t)
    {
        // Repeats
        if (rep_limit != 0 && ++rep_time >= rep_limit)
        {
            repeat();
        }

        // Arpeggio (single)
        if (arp_limit != 0 && t >= arp_limit)
        {
            arp_limit = 0;
            fperiod *= arp_mod;
        }

        // Frequency slide
        fslide += fdslide;
        fperiod *= fslide;
        if (fperiod > fmaxperiod)
        {
            fperiod = fmaxperiod;
            if (sound->p_freq_limit > 0.0f)
            {
                buffer_complete = true;
            }
        }

        // Vibrato
        float rfperiod = fperiod;
        if (vib_amp > 0.0f)
        {
            vib_phase += vib_speed;
            rfperiod = fperiod * (1.0 + sin(vib_phase) * vib_amp);
        }
        period = (int)rfperiod;
        if (period < 8)
        {
            period = 8;
        }

        // Duty cycle slide
        square_duty += square_slide;
        if (square_duty < 0.0f)
        {
            square_duty = 0.0f;
        }
        if (square_duty > 0.5f)
        {
            square_duty = 0.5f;
        }

        // Volume envelope
        env_time++;
        if (env_time > env_length[env_stage])
        {
            env_time = 1;
            env_stage++;
            while (env_stage < 3 && env_length[env_stage] == 0)
            {
                env_stage++;
            }
            if (env_stage == 3)
            {
                break;
            }
        }
        if (env_stage == 0)
        {
            env_vol = (float)env_time / env_length[0];
        }
        else if (env_stage == 1)
        {
            env_vol = 1.0f + pow(1.0f - (float)env_time / env_length[1], 1.0f) * 2.0f * sound->p_env_punch;
        }
        else
        {
            // env_stage == 2
            env_vol = 1.0f - (float)env_time / env_length[2];
        }

        // Phaser step
        fphase += fdphase;
        iphase = abs((int)fphase);
        if (iphase > 1023) iphase = 1023;

        if (flthp_d != 0.0f)
        {
            flthp *= flthp_d;
            if (flthp < 0.00001f)
            {
                flthp = 0.00001f;
            }
            if (flthp > 0.1f)
            {
                flthp = 0.1f;
            }
        }

        // 8x supersampling
        float sample = 0.0f;
        for (int si = 0; si < 8; si++)
        {
            float sub_sample = 0.0f;
            phase++;
            if (phase >= period)
            {
                phase %= period;
                if (sound->wave_type == NOISE)
                {
                    for (int i = 0; i < 32; i++)
                    {
                        noise_buffer[i] = frnd(2.0f) - 1.0f;
                    }
                }
            }
            // base waveform
            float fp = (float)phase / period;
            switch (sound->wave_type)
            {
                case SQUARE:
                    if (fp < square_duty)
                    {
                        sub_sample = 0.5f;
                    }
                    else
                    {
                        sub_sample = -0.5f;
                    }
                    break;
                case SAWTOOTH:
                    sub_sample = 1.0f - fp * 2.0f;
                    break;
                case SINE:
                    sub_sample = (float)sin(fp * 2.0f * PI);
                    break;
                case NOISE:
                    sub_sample = noise_buffer[(int)(phase * 32 / period)];
                    break;
                case TRIANGLE:
                    sub_sample = fabs(1.0f - fp * 2.0f) - 1.0f;
                    break;
                case BREAKER:
                    sub_sample = fabs(1.0f - fp * fp * 2.0f) - 1.0f;
                    break;
                default:
                    break;
            }

            // Low-pass filter
            float pp = fltp;
            fltw *= fltw_d;
            if (fltw < 0.0f)
            {
                fltw = 0.0f;
            }
            if (fltw > 0.1f)
            {
                fltw = 0.1f;
            }
            if (sound->p_lpf_freq != 1.0f)
            {
                fltdp += (sub_sample - fltp) * fltw;
                fltdp -= fltdp * fltdmp;
            }
            else
            {
                fltp = sub_sample;
                fltdp = 0.0f;
            }
            fltp += fltdp;

            // High-pass filter
            fltphp += fltp - pp;
            fltphp -= fltphp * flthp;
            sub_sample = fltphp;

            // Phaser
            phaser_buffer[ipp & 1023] = sub_sample;
            sub_sample += phaser_buffer[(ipp - iphase + 1024) & 1023];
            ipp = (ipp + 1) & 1023;

            // Final accumulation and envelope application
            sample += sub_sample * env_vol;
        }
        sample = sample / 8.0f * sfxr_master_vol;
        sample *= gain;

        output.push_back(sample);
    }
    return output;
}

static void SDLAudioCallback(void *userdata, Uint8 *stream, int len)
{
    if (playing_sample)
	{
		unsigned int l = len/2;
		//vector<float> fbuf = generate();
		float *fbuf = new float[l];
		memset(fbuf, 0, sizeof(fbuf));
		SynthSample(l, fbuf);

		while (l--)
		{
			float f = fbuf[l];
			if (f < -1.0) f = -1.0;
			if (f > 1.0) f = 1.0;
			((Sint16*)stream)[l] = (Sint16)(f * 32767);
		}

		delete [] fbuf;
	}
	else memset(stream, 0, len);
}

void initSfxr()
{
    bufferLength = 512;
	// return;
	SDL_AudioDeviceID dev;

	des.freq = 44100; // 22050; // 5512; //
	des.format = AUDIO_S16SYS; // AUDIO_S8; //
	des.channels = 1;
	des.samples = bufferLength;
	des.callback = SDLAudioCallback;
	des.userdata = NULL;
	//SDL_OpenAudio(&des, NULL);
	//SDL_PauseAudio(0);
	dev = SDL_OpenAudioDevice(NULL, 0, &des, NULL, 0);
	if (dev == 0) {
		SDL_Log("Failed to open audio: %s", SDL_GetError());
	}
	SDL_PauseAudioDevice(dev, 0);
}

// class SfxrSound

Mix_Chunk* SfxrSound::generateSound()
{
	sound = this;
	ResetSample(false);

	Mix_Chunk* chunk = new Mix_Chunk();
	playing_sample = true;
	Uint8 abuf[500000] = { 0 };
	int i = 0;
	do
	{
		SDLAudioCallback(nullptr, abuf + bufferLength * i, bufferLength * 2);
		i++;
	} while (playing_sample);

	chunk->abuf = abuf;
	chunk->alen = i * bufferLength;
	chunk->allocated = true;
	chunk->volume = MIX_MAX_VOLUME;

	return chunk;
}

// Constructors
SfxrSound::SfxrSound()
{
    ResetParams();
}

SfxrSound::SfxrSound(
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
)
    : wave_type(wave_type),
     p_base_freq(p_base_freq),
     p_freq_limit(p_freq_limit),
     p_freq_ramp(p_freq_ramp),
     p_freq_dramp(p_freq_dramp),
     p_duty(p_duty),
     p_duty_ramp(p_duty_ramp),

    p_vib_strength(p_vib_strength),
    p_vib_speed(p_vib_speed),
    p_vib_delay(0),

    p_env_attack(p_env_attack),
    p_env_sustain(p_env_sustain),
    p_env_decay(p_env_decay),
    p_env_punch(p_env_punch),

    p_lpf_resonance(p_lpf_resonance),
    p_lpf_freq(p_lpf_freq),
    p_lpf_ramp(p_lpf_ramp),
    p_hpf_freq(p_hpf_freq),
    p_hpf_ramp(p_hpf_ramp),

    p_pha_offset(p_pha_offset),
    p_pha_ramp(p_pha_ramp),

    p_repeat_speed(p_repeat_speed),

    p_arp_speed(p_arp_speed),
    p_arp_mod(p_arp_mod),
    sound_vol(sound_vol),
    sample_rate(sample_rate),
    bit_depth(bit_depth)
{

}

SfxrSound::~SfxrSound()
{

}

void SfxrSound::ResetParams()
{
    wave_type=0;

    p_base_freq=0.3f;
    p_freq_limit=0.0f;
    p_freq_ramp=0.0f;
    p_freq_dramp=0.0f;
    p_duty=0.0f;
    p_duty_ramp=0.0f;

    p_vib_strength=0.0f;
    p_vib_speed=0.0f;
    p_vib_delay=0.0f;

    p_env_attack=0.0f;
    p_env_sustain=0.3f;
    p_env_decay=0.4f;
    p_env_punch=0.0f;

    p_lpf_resonance=0.0f;
    p_lpf_freq=1.0f;
    p_lpf_ramp=0.0f;
    p_hpf_freq=0.0f;
    p_hpf_ramp=0.0f;

    p_pha_offset=0.0f;
    p_pha_ramp=0.0f;

    p_repeat_speed=0.0f;

    p_arp_speed=0.0f;
    p_arp_mod=0.0f;

    sample_rate = 44100;
    bit_depth = AUDIO_S16SYS;
}

void SfxrSound::playSample()
{
    sound = this;
    // this->trace();
    ResetSample(false);
    playing_sample=true;
}

void SfxrSound::trace()
{
    cout << "p_env_attack : " << p_env_attack << endl;
    cout << "p_env_sustain : " << p_env_sustain << endl;
    cout << "p_env_punch : " << p_env_punch << endl;
    cout << "p_env_decay : " << p_env_decay << endl;


    cout << "p_base_freq : " << p_base_freq << endl;
    cout << "p_freq_limit : " << p_freq_limit << endl;
    cout << "p_freq_ramp : " << p_freq_ramp << endl;
    cout << "p_freq_dramp : " << p_freq_dramp << endl;

    cout << "p_vib_strength : " << p_vib_strength << endl;
    cout << "p_vib_speed : " << p_vib_speed << endl;

    cout << "p_arp_mod : " << p_arp_mod << endl;
    cout << "p_arp_speed : " << p_arp_speed << endl;

    cout << "p_duty : " << p_duty << endl;
    cout << "p_duty_ramp : " << p_duty_ramp << endl;

    cout << "p_repeat_speed : " << p_repeat_speed << endl;

    cout << "p_pha_offset : " << p_pha_offset << endl;
    cout << "p_pha_ramp : " << p_pha_ramp << endl;

    cout << "p_lpf_freq : " << p_lpf_freq << endl;
    cout << "p_lpf_ramp : " << p_lpf_ramp << endl;
    cout << "p_lpf_resonance : " << p_lpf_resonance << endl;

    cout << "p_hpf_freq : " << p_hpf_freq << endl;
    cout << "p_hpf_ramp : " << p_hpf_ramp << endl;

    cout << "sound_vol : " << sound_vol << endl;
    cout << "sample_rate : " << sample_rate << endl;
    cout << "bit_depth : " << bit_depth << endl;
    cout << "wave_type : " << wave_type << endl;
}
