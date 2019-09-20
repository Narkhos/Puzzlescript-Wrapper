var SoundEffect = {};

function traceSoundParams(params) {
		// Envelope
		print('p_env_attack :' + params.p_env_attack);   // Attack time
		print('p_env_sustain :' + params.p_env_sustain);  // Sustain time
		print('p_env_punch :' + params.p_env_punch);    // Sustain punch
		print('p_env_decay :' + params.p_env_decay);    // Decay time

		// Tone
		print('p_base_freq :' + params.p_base_freq);    // Start frequency
		print('p_freq_limit :' + params.p_freq_limit);   // Min frequency cutoff
		print('p_freq_ramp :' + params.p_freq_ramp);    // Slide (SIGNED)
		print('p_freq_dramp :' + params.p_freq_dramp);   // Delta slide (SIGNED)
		
		// Vibrato
		print('p_vib_strength :' + params.p_vib_strength); // Vibrato depth
		print('p_vib_speed :' + params.p_vib_speed);    // Vibrato speed

		// Tonal change
		print('p_arp_mod :' + params.p_arp_mod);      // Change amount (SIGNED)
		print('p_arp_speed :' + params.p_arp_speed);    // Change speed

		// Duty
		print('p_duty :' + params.p_duty);         // Square duty
		print('p_duty_ramp :' + params.p_duty_ramp);    // Duty sweep (SIGNED)

		// Repeat
		print('p_repeat_speed :' + params.p_repeat_speed); // Repeat speed

		// Phaser
		print('p_pha_offset :' + params.p_pha_offset);   // Phaser offset (SIGNED)
		print('p_pha_ramp :' + params.p_pha_ramp);     // Phaser sweep (SIGNED)

		// Low-pass filter
		print('p_lpf_freq :' + params.p_lpf_freq);     // Low-pass filter cutoff
		print('p_lpf_ramp :' + params.p_lpf_ramp);     // Low-pass filter cutoff sweep (SIGNED)
		print('p_lpf_resonance :' + params.p_lpf_resonance);// Low-pass filter resonance
		
		// High-pass filter
		print('p_hpf_freq :' + params.p_hpf_freq);     // High-pass filter cutoff
		print('p_hpf_ramp :' + params.p_hpf_ramp);     // High-pass filter cutoff sweep (SIGNED));
		
		// Main parameters
		print('sound_vol :' + params.sound_vol);
		print('sample_rate :' + params.sample_rate);
		print('bit_depth :' + params.bit_depth);
		
		print('wave_type :' + params.wave_type);
}

SoundEffect.generate = function(seed, params) {
	
	var sound = {};
	sound.seed = seed;
	sound.play = function() {
		nativePlaySound(parseInt(this.seed));
	}
	
	nativeGenerateSound(
		
		parseInt(seed), 
		
		// Envelope
		params.p_env_attack,   // Attack time
		params.p_env_sustain,  // Sustain time
		params.p_env_punch,    // Sustain punch
		params.p_env_decay,    // Decay time

		// Tone
		params.p_base_freq,    // Start frequency
		params.p_freq_limit,   // Min frequency cutoff
		params.p_freq_ramp,    // Slide (SIGNED)
		params.p_freq_dramp,   // Delta slide (SIGNED)
		
		// Vibrato
		params.p_vib_strength, // Vibrato depth
		params.p_vib_speed,    // Vibrato speed

		// Tonal change
		params.p_arp_mod,      // Change amount (SIGNED)
		params.p_arp_speed,    // Change speed

		// Duty
		params.p_duty,         // Square duty
		params.p_duty_ramp,    // Duty sweep (SIGNED)

		// Repeat
		params.p_repeat_speed, // Repeat speed

		// Phaser
		params.p_pha_offset,   // Phaser offset (SIGNED)
		params.p_pha_ramp,     // Phaser sweep (SIGNED)

		// Low-pass filter
		params.p_lpf_freq,     // Low-pass filter cutoff
		params.p_lpf_ramp,     // Low-pass filter cutoff sweep (SIGNED)
		params.p_lpf_resonance,// Low-pass filter resonance
		
		// High-pass filter
		params.p_hpf_freq,     // High-pass filter cutoff
		params.p_hpf_ramp,     // High-pass filter cutoff sweep (SIGNED));
		
		// Main parameters
		params.sound_vol,
		params.sample_rate,
		params.bit_depth,
		
		params.wave_type
	);
	
	
	return sound;
}

var sfxCache = [];

function cacheSeed(seed){
  if (sound in sfxCache) {
    return sound;
  }

  var params = generateFromSeed(seed);
  params.sound_vol = SOUND_VOL;
  params.sample_rate = SAMPLE_RATE;
  params.bit_depth = BIT_DEPTH;

  var sound = SoundEffect.generate(seed, params);
  sfxCache[sound] = sound;

  return sound;
}
