#pragma once

namespace algawolf::chords
{
    struct Chord
    {
        bool notes[12];

        bool operator==(Chord const &rhs)
        {
            for (int i = 0; i < 12; i++)
            {
                if (this->notes[i] != rhs.notes[i])
                    return false;
            }

            return true;
        }
    };

    static const int NUMBER_CHORDS = 28;
    static const Chord CHORDS[NUMBER_CHORDS] =
        {
            // static const Chord MAJOR =
            {{
                true,  // 0, // C
                false, // 1, // C#
                false, // 2, // D
                false, // 3, // D#
                true,  // 4, // E
                false, // 5, // F
                false, // 6, // F#
                true,  // 7, // G
                false, // 8, // G#
                false, // 9, // A
                false, // 10, // A#
                false  // 11  // B
            }},
            // static const Chord MINOR =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    false, // 2, // D
                    true,  // 3, // D#
                    false, // 4, // E
                    false, // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    false, // 10, // A#
                    false  // 11  // B
                }},
            // static const Chord SEVENTH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    false, // 2, // D
                    false, // 3, // D#
                    true,  // 4, // E
                    false, // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    true,  // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord MAJOR_SEVENTH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    false, // 2, // D
                    false, // 3, // D#
                    true,  // 4, // E
                    false, // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    false, // 10, // A#
                    true   // 11  // B
                }},

            // static const Chord MINOR_SEVENTH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    false, // 2, // D
                    true,  // 3, // D#
                    false, // 4, // E
                    false, // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    true,  // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord SIXTH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    false, // 2, // D
                    false, // 3, // D#
                    true,  // 4, // E
                    false, // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    true,  // 9, // A
                    false, // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord MINOR_SIXTH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    false, // 2, // D
                    true,  // 3, // D#
                    false, // 4, // E
                    false, // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    true,  // 9, // A
                    false, // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord DIMINISHED =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    false, // 2, // D
                    true,  // 3, // D#
                    false, // 4, // E
                    false, // 5, // F
                    true,  // 6, // F#
                    false, // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    false, // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord DIMINISHED_SEVENTH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    false, // 2, // D
                    true,  // 3, // D#
                    false, // 4, // E
                    false, // 5, // F
                    true,  // 6, // F#
                    false, // 7, // G
                    false, // 8, // G#
                    true,  // 9, // A
                    false, // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord HALF_DIMINISHED_SEVENTH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    false, // 2, // D
                    true,  // 3, // D#
                    false, // 4, // E
                    false, // 5, // F
                    true,  // 6, // F#
                    false, // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    true,  // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord AUGMENTED =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    false, // 2, // D
                    false, // 3, // D#
                    true,  // 4, // E
                    false, // 5, // F
                    false, // 6, // F#
                    false, // 7, // G
                    true,  // 8, // G#
                    false, // 9, // A
                    false, // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord SEVENTH_SHARP_FIFTH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    false, // 2, // D
                    false, // 3, // D#
                    true,  // 4, // E
                    false, // 5, // F
                    false, // 6, // F#
                    false, // 7, // G
                    true,  // 8, // G#
                    false, // 9, // A
                    true,  // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord NINETH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    true,  // 2, // D
                    false, // 3, // D#
                    true,  // 4, // E
                    false, // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    true,  // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord SEVENTH_SHARP_NINE =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    false, // 2, // D
                    true,  // 3, // D#
                    true,  // 4, // E
                    false, // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    true,  // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord MAJOR_NINETH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    true,  // 2, // D
                    false, // 3, // D#
                    true,  // 4, // E
                    false, // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    false, // 10, // A#
                    true   // 11  // B
                }},

            // static const Chord ADDED_NINETH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    true,  // 2, // D
                    false, // 3, // D#
                    true,  // 4, // E
                    false, // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    false, // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord MINOR_NINETH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    true,  // 2, // D
                    true,  // 3, // D#
                    false, // 4, // E
                    false, // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    true,  // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord MINOR_ADD_NINETH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    true,  // 2, // D
                    true,  // 3, // D#
                    false, // 4, // E
                    false, // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    false, // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord ELEVENTH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    true,  // 2, // D
                    false, // 3, // D#
                    true,  // 4, // E
                    true,  // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    true,  // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord MINOR_ELEVENTH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    true,  // 2, // D
                    true,  // 3, // D#
                    false, // 4, // E
                    true,  // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    true,  // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord SEVENTH_SHARP_ELEVEN =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    true,  // 2, // D
                    false, // 3, // D#
                    true,  // 4, // E
                    false, // 5, // F
                    true,  // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    true,  // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord MAJOR_SEVENTH_SHARP_ELEVEN =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    true,  // 2, // D
                    false, // 3, // D#
                    true,  // 4, // E
                    false, // 5, // F
                    true,  // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    false, // 10, // A#
                    true   // 11  // B
                }},

            // static const Chord THIRTEENTH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    true,  // 2, // D
                    false, // 3, // D#
                    true,  // 4, // E
                    false, // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    true,  // 9, // A
                    true,  // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord MAJOR_THIRTEENTH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    true,  // 2, // D
                    false, // 3, // D#
                    true,  // 4, // E
                    false, // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    true,  // 9, // A
                    false, // 10, // A#
                    true   // 11  // B
                }},

            // static const Chord MINOR_THIRTEENTH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    true,  // 2, // D
                    true,  // 3, // D#
                    false, // 4, // E
                    true,  // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    true,  // 9, // A
                    false, // 10, // A#
                    true   // 11  // B
                }},

            // static const Chord SUS_FOURTH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    false, // 2, // D
                    false, // 3, // D#
                    false, // 4, // E
                    true,  // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    false, // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord SUS_SECOND =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    true,  // 2, // D
                    false, // 3, // D#
                    false, // 4, // E
                    false, // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    false, // 10, // A#
                    false  // 11  // B
                }},

            // static const Chord POWER_FIFTH =
            {
                {
                    true,  // 0, // C
                    false, // 1, // C#
                    false, // 2, // D
                    false, // 3, // D#
                    false, // 4, // E
                    false, // 5, // F
                    false, // 6, // F#
                    true,  // 7, // G
                    false, // 8, // G#
                    false, // 9, // A
                    false, // 10, // A#
                    false  // 11  // B
                }},

    }; // namespace w0lf::chords

} // namespace w0lf::chords
