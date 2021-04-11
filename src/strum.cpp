#include "strum.hpp"

void Strum::process(const ProcessArgs &args)
{
	// CURRENT STATE
	m_gateVoltage = inputs[GATE_INPUT].getVoltageSum();
	m_sampleTime = args.sampleTime;

	// LEDS
	if (m_lightDivider.process())
		resetLeds();

	// CHECK INPUTS
	if (m_inputDivider.process())
	{
		setParams();
		checkMutes();
	}

	// ADD NOTES
	if (m_addNotes && m_gateVoltage == 10.f && inputs[GATE_INPUT].isMonophonic())
	{
		addUngatedNotes();
		m_addNotes = false;
	}
	else if (m_addNotes)
	{
		addGatedNotes();
		m_addNotes = false;
	}
	// add notes on next sample to allow for any voct inputs to update after triggers
	m_addNotes = m_gateVoltage != m_previousGateVoltage;

	// CV SEQUENCING
	if (inputs[SEQ_CV_INPUT].isConnected())
	{
		// alternate to reduce cpu load
		if (m_alternate)
			cvAddSeqNotes();
		else
			cvPlaySeq();

		setChannels();
	}
	// GATE / TRIGGER SEQUENCING
	else
	{
		addSeqNotes();
		if (m_seqRunning)
		{
			playSeq();
			setChannels();
		}
	}

	// NEXT STATE
	m_alternate = !m_alternate;
	m_previousGateVoltage = m_gateVoltage;
	m_time = m_time + m_sampleTime;
}

inline void Strum::cvAddSeqNotes()
{
	// skip when changing key
	if (m_previousGateVoltage != 0.f && m_gateVoltage != 10.f)
		m_rootVoltageOffset = std::fmin(std::fmax(inputs[VOCT_INPUT].getVoltage(), -5.f), 5.f) - m_notes[0].voltage;

	float repeat = m_repeat.getValue();
	float seqCvVoltage = wdsp::fold_uni_10(inputs[SEQ_CV_INPUT].getVoltage() * repeat);
	seqCvVoltage = repeat < 0.f ? seqCvVoltage + 10.f : seqCvVoltage;
	float gateLength = m_gate.getValue();
	float voltPerNote = m_mutesSetLength ? 10.f / (m_noteCount - 1) : 1.25f;
	bool cvOffsetConnected = inputs[OFFSET_CV_INPUT].isConnected();
	bool cvOffsetIsMono = inputs[OFFSET_CV_INPUT].isMonophonic();
	bool cvXConnected = inputs[X_CV_INPUT].isConnected();
	bool cvXIsMono = inputs[X_CV_INPUT].isMonophonic();
	bool slideConnected = outputs[SLIDE_OUTPUT].isConnected();

	NoteGate *gate;
	float nearestLast = FLT_MAX;
	float nearestNext = FLT_MAX;
	float lastNoteVoltage = 0.f;
	float nextNoteVoltage = 0.f;
	float noteDistance;
	float noteCvVoltage;
	for (int i = 0; i < 8; i++)
	{
		gate = &m_gates[i];
		if (gate->note->muted)
			continue;

		if (m_time > gate->close || slideConnected)
		{
			if (cvOffsetConnected)
			{
				gate->note->delayOffset = inputs[OFFSET_CV_INPUT].getVoltage(cvOffsetIsMono ? 0 : i) * 0.2f - 1.f;
			}
			if (cvXConnected)
			{
				gate->note->cv = inputs[X_CV_INPUT].getVoltage(cvXIsMono ? 0 : i);
			}

			noteCvVoltage = std::fabs((i * voltPerNote) + (gate->note->delayOffset * voltPerNote));
			noteCvVoltage = noteCvVoltage > 10.f ? 10.f - (noteCvVoltage - 10.f) : noteCvVoltage;
			// epsilon = 0.01f volts
			if (std::fabs(seqCvVoltage - noteCvVoltage) < 0.01f)
			{
				gate->note = &m_notes[i];
				gate->voltage = std::fmin(std::fmax(gate->note->voltage + m_rootVoltageOffset, -5.f), 5.f);
				gate->open = m_time;
				gate->close = gate->open + gateLength;
				gate->isOpen = false;
			}

			// cache slide notes
			noteDistance = noteCvVoltage - seqCvVoltage;
			if (noteDistance < 0.f && noteDistance < nearestLast)
			{
				lastNoteVoltage = gate->note->voltage;
				nearestLast = -noteDistance;
			}
			else if (noteDistance < nearestNext)
			{
				nextNoteVoltage = gate->note->voltage;
				nearestNext = noteDistance;
			}
		}
	}

	// set slide note
	if (slideConnected)
	{
		float range = nearestLast + nearestNext;
		nearestLast = range > 0.f ? 1.f - (nearestLast / range) : 1.f;
		nearestNext = range > 0.f ? 1.f - (nearestNext / range) : 0.f;

		float x = (lastNoteVoltage * nearestLast) + (nextNoteVoltage * nearestNext);
		outputs[SLIDE_OUTPUT].setVoltage(x);
	}
}

inline void Strum::cvPlaySeq()
{
	NoteGate *gate;
	for (int i = 0; i < 8; i++)
	{
		gate = &m_gates[i];
		if (m_time < gate->close && m_time >= gate->open)
		{
			if (!gate->note->muted)
			{
				if (!gate->isOpen)
				{
					int index = gate->note->index;
					outputs[GATE_OUTPUT].setVoltage(10.f, index);
					outputs[VOCT_OUTPUT].setVoltage(gate->voltage, index);
					outputs[X_OUTPUT].setVoltage(gate->note->cv, index);
					gate->isOpen = true;
				}
				lights[LED_LIGHT + (i << 1)].setBrightness(0.f);
				lights[LED_LIGHT + (i << 1) + 1].setBrightness(1.f);
			}
			else
			{
				lights[LED_LIGHT + (i << 1)].setBrightness(.15f);
				lights[LED_LIGHT + (i << 1) + 1].setBrightness(.15f);
			}
		}
		else if (gate->isOpen && m_time > gate->close)
		{
			gate->isOpen = false;
			outputs[GATE_OUTPUT].setVoltage(0.f, gate->note->index);
		}
	}
}

inline void Strum::addSeqNotes()
{
	if (m_previousGateVoltage != 0.f || m_gateVoltage != 10.f || m_noteCount == 0)
		return;

	resetGates();

	// check / set note cv delay values
	if (inputs[OFFSET_CV_INPUT].isConnected())
	{
		if (inputs[OFFSET_CV_INPUT].isMonophonic())
		{
			float voltage = inputs[OFFSET_CV_INPUT].getVoltage() * 0.2f - 1.f;
			for (int i = 0; i < 8; i++)
			{
				m_notes[i].delayOffset = voltage;
			}
		}
		else
		{
			for (int i = 0; i < 8; i++)
			{
				m_notes[i].delayOffset = inputs[OFFSET_CV_INPUT].getVoltage(i) * 0.2f - 1.f;
			}
		}
	}

	// check / set note cv values
	if (inputs[X_CV_INPUT].isConnected())
	{
		if (inputs[X_CV_INPUT].isMonophonic())
		{
			float voltage = inputs[X_CV_INPUT].getVoltage();
			for (int i = 0; i < 8; i++)
			{
				m_notes[i].cv = voltage;
			}
		}
		else
		{
			for (int i = 0; i < 8; i++)
			{
				m_notes[i].cv = inputs[X_CV_INPUT].getVoltage(i);
			}
		}
	}

	// check root note offset
	if (inputs[VOCT_INPUT].isConnected())
	{
		float v = std::fmin(std::fmax(inputs[VOCT_INPUT].getVoltage(), -5.f), 5.f);
		m_rootVoltageOffset = v - m_notes[0].voltage;
		if (m_rootVoltageOffset != 0.f)
		{
			m_notes[0].voltage = v;
			params[NOTE_PARAM].setValue(v);
			for (int i = 1; i < 8; i++)
			{
				m_notes[i].voltage = std::fmin(std::fmax(m_notes[i].voltage + m_rootVoltageOffset, -5.f), 5.f);
				params[NOTE_PARAM + i].setValue(m_notes[i].voltage);
			}
		}
	}

	// set note values
	m_gateStart = 0;
	float pattern = m_pattern.getValue();
	float repeat = m_repeat.getValue();
	float delay = m_delay.getValue();
	float gateLength = m_gate.getValue();

	// generate patten note gates
	// 5 note pattern - [0, 1, 2, 3]  [4, 3, 2, 1]  [0, 1, 2, 3] [4]
	// multiplication is faster, 1 / 8 = 0.125f
	int patternCount = int(std::floor((float(m_noteCount) * 0.125f) * pattern));
	int nextReverse = m_noteCount - 1;
	bool reversed = repeat < 0.f ? true : false;
	int nI;
	NoteGate *gate;
	NoteGate *prevGate;
	NoteGate gateBuff;
	for (int i = 0; i < patternCount; i++)
	{
		nI = i % nextReverse;
		if (i > 0 && nI == 0)
		{
			reversed = !reversed;
		}

		nI = reversed ? nextReverse - nI : nI;

		gate = &m_gates[i];
		gate->note = &m_notes[nI];
		gate->voltage = m_notes[nI].voltage;
		gate->open = std::fmax((i * delay) + (delay * m_notes[nI].delayOffset), 0.f);
		gate->close = gate->open + gateLength;
		gate->isOpen = false;

		if (i > 0)
		{
			// reverse note if order has been switched by note delay offset
			if (gate->open < prevGate->open)
			{
				// buffer current gate
				gateBuff = *gate;
				// overwrite current gate with previous gate
				m_gates[i] = *prevGate;
				// assign current gate to previous gate
				m_gates[i - 1] = gateBuff;
			}

			prevGate->timeToNextNote = gate->open - prevGate->open;
		}

		prevGate = gate;
	}

	// repeat pattern note gates
	m_gateEnd = int(std::floor(patternCount * std::fabs(repeat)));
	for (int i = patternCount; i < m_gateEnd; i++)
	{
		nI = i % patternCount;
		gate = &m_gates[i];
		gate->note = m_gates[nI].note;
		gate->voltage = m_gates[nI].note->voltage;
		gate->open = std::fmax((i * delay) + (delay * m_gates[nI].note->delayOffset), 0.f);
		gate->close = gate->open + gateLength;
	}

	m_time = 0.f;
	m_seqRunning = true;
}

inline void Strum::playSeq()
{
	if (m_previousGateVoltage != 0.f && m_gateVoltage != 10.f)
		return;

	bool prevNoteEnded = true;
	int endMinusOne = m_gateEnd - 1;
	bool slideConnected = outputs[SLIDE_OUTPUT].isConnected();
	// float lerpVoltage = 0.f;
	NoteGate *gate;
	// note window must allow for all but one muted note which could be upto 14 steps
	int noteWindow = std::fmin(m_gateStart + 15, m_gateEnd);
	for (int i = 0; i < noteWindow; i++)
	{
		gate = &m_gates[i];
		// time has passed note open
		if (m_time >= gate->open)
		{
			// time is in note window
			if (m_time < gate->close)
			{
				if (!gate->note->muted)
				{
					if (!gate->isOpen)
					{
						int index = gate->note->index;
						outputs[GATE_OUTPUT].setVoltage(10.f, index);
						outputs[VOCT_OUTPUT].setVoltage(gate->voltage, index);
						outputs[X_OUTPUT].setVoltage(gate->note->cv, index);
						gate->isOpen = true;
					}

					int ledIndex = gate->note->index << 1;
					lights[LED_LIGHT + ledIndex].setBrightness(0.f);
					lights[LED_LIGHT + ledIndex + 1].setBrightness(1.f);
				}
				else
				{
					int ledIndex = gate->note->index << 1;
					lights[LED_LIGHT + ledIndex].setBrightness(0.15f);
					lights[LED_LIGHT + ledIndex + 1].setBrightness(0.15f);
				}
				prevNoteEnded = false;
			}
			// time has passed note end
			else if (gate->isOpen)
			{
				if (prevNoteEnded || i == 0)
				{
					m_gateStart = i + 1;
					prevNoteEnded = true;
					gate->isOpen = false;
				}
				outputs[GATE_OUTPUT].setVoltage(0.f, gate->note->index);
			}

			// lerp
			if (slideConnected && i < endMinusOne)
			{
				float x = gate->timeToNextNote > 0.f ? (m_time - gate->open) / gate->timeToNextNote : 1.f;
				x = (gate->voltage * (1.0f - x)) + (m_gates[i + 1].voltage * x);
				outputs[SLIDE_OUTPUT].setVoltage(x);
			}
		}
	}

	m_seqRunning = m_gateStart < m_gateEnd;
}

inline void Strum::addUngatedNotes()
{
	int noteCount = 0;
	int channels = inputs[VOCT_INPUT].getChannels();
	if (channels == 0)
		return;

	if (m_noteMuteOverwrite)
		mute(true);

	float interval = 0.f;
	while (noteCount < 8)
	{
		for (int c = 0; c < channels; c++)
		{
			m_notes[noteCount].voltage = std::fmin(std::fmax(inputs[VOCT_INPUT].getVoltage(c) + interval, -5.f), 5.f);
			params[NOTE_PARAM + noteCount].setValue(m_notes[noteCount].voltage);

			if (m_noteMuteOverwrite && interval == 0.f)
				m_notes[noteCount].muted = false;

			noteCount++;

			if (noteCount >= 8)
				break;
		}
		interval += m_addNoteInterval;
	}

	quantize();

	if (m_noteMuteOverwrite)
		m_noteCount = m_mutesSetLength ? noteCount : 8;
}

inline void Strum::addGatedNotes()
{
	// reset setting voltage on all gate release
	if (m_gateVoltage == 0.f)
	{
		m_settingGateVoltage = 20.f;
		return;
	}

	// only allow setting notes after notes have been released or when two or more gates are triggered
	if (m_gateVoltage < m_settingGateVoltage)
		return;

	if (m_noteMuteOverwrite)
		mute(true);

	int noteCount = 0;
	float_4 voltages;
	int channels = inputs[GATE_INPUT].getChannels();
	float interval = 0.f;
	while (noteCount < 8)
	{
		for (int c = 0; c < channels; c += 4)
		{
			if (noteCount == 8)
				break;

			voltages = inputs[GATE_INPUT].getPolyVoltageSimd<float_4>(c);
			for (int cI = 0; cI < 4; cI++)
			{
				if (voltages[cI] > 0.f)
				{
					m_notes[noteCount].voltage = std::fmin(std::fmax(inputs[VOCT_INPUT].getVoltage(c + cI) + interval, -5.f), 5.f);
					params[NOTE_PARAM + noteCount].setValue(m_notes[noteCount].voltage);

					if (interval == 0.f)
					{
						if (m_noteMuteOverwrite)
						{
							m_notes[noteCount].muted = false;
						}
						m_settingGateVoltage = (noteCount + 1) * 10.f;
					}
					noteCount++;

					if (noteCount == 8)
						break;
				}
			}
		}
		interval += m_addNoteInterval;
	}

	quantize();

	if (m_noteMuteOverwrite)
		m_noteCount = m_mutesSetLength ? noteCount : 8;
}

inline void Strum::checkMutes()
{
	if (!m_mutesSetLength)
		m_noteCount = 8;

	for (int i = 0; i < 8; i++)
	{
		if (t_mutes[i].process(params[MUTE_PARAM + i].getValue()))
		{
			m_notes[i].muted = !m_notes[i].muted;
		}

		if (m_mutesSetLength && !m_notes[i].muted)
		{
			m_noteCount = i + 1;
		}
	}
}

// also called from KNOTE KnobEvents
inline void Strum::updateQuantizerNotes()
{
	for (int i = 0; i < 24; i++)
	{
		int closestNote = 0;
		int closestDist = INT_MAX;
		for (int note = -12; note <= 24; note++)
		{
			if (!m_scale.notes[eucMod(note, 12)])
				continue;

			int dist = std::abs((i + 1) / 2 - note);
			if (dist < closestDist)
			{
				closestNote = note;
				closestDist = dist;
			}
			else
			{
				break;
			}
		}
		m_quantizeIntervals[i] = closestNote;
	}
}

// also called from KNOTE KnobEvents
inline void Strum::quantize()
{
	if (!m_quantize)
		return;

	float root = params[NOTE_PARAM].getValue();
	root -= eucDiv(std::floor(root * 24), 24);
	for (int i = 0; i < 8; i++)
	{
		float pitch = params[NOTE_PARAM + i].getValue() - root;
		int range = std::floor(pitch * 24);
		int octave = eucDiv(range, 24);
		range -= octave * 24;
		int note = m_quantizeIntervals[range] + octave * 12;
		m_notes[i].voltage = (float(note) / 12) + (std::floor(root * 24.f) / 24);
		params[NOTE_PARAM + i].setValue(m_notes[i].voltage);
	}
}

inline void Strum::setParams()
{
	// set note offsets
	if (inputs[OFFSET_CV_INPUT].isConnected())
	{
		for (int i = 0; i < 8; i++)
		{
			params[DELAYOFFSET_PARAM + i].setValue(m_notes[i].delayOffset);
		}
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			m_notes[i].delayOffset = params[DELAYOFFSET_PARAM + i].getValue();
		}
	}
	// set note cv
	if (inputs[X_CV_INPUT].isConnected())
	{
		for (int i = 0; i < 8; i++)
		{
			params[X_PARAM + i].setValue(m_notes[i].cv);
		}
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			m_notes[i].cv = params[X_PARAM + i].getValue();
		}
	}

	for (int i = 0; i < 8; i++)
	{
		m_notes[i].voltage = params[NOTE_PARAM + i].getValue();
	}
}

inline void Strum::mute(bool _mute)
{
	for (int i = 0; i < MAX_NOTES; i++)
	{
		m_notes[i].muted = _mute;
	}
}

inline void Strum::resetGates()
{
	float_4 v0 = 0.f;
	for (int i = 0; i < m_channelCount; i += 4)
	{
		outputs[GATE_OUTPUT].setVoltageSimd(v0, i);
	}
}

inline void Strum::setChannels()
{
	m_channelCount = 8;
	outputs[VOCT_OUTPUT].setChannels(m_channelCount);
	outputs[GATE_OUTPUT].setChannels(m_channelCount);
	outputs[X_OUTPUT].setChannels(m_channelCount);

	if (outputs[SLIDE_OUTPUT].isConnected())
		outputs[SLIDE_OUTPUT].setChannels(1);
}

Model *modelStrum = createModel<Strum, StrumWidget>("Strum");
