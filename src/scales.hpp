#pragma once

namespace algawolf::scales
{

    struct Scale
    {
        bool notes[12];

        bool operator==(Scale const &rhs)
        {
            for (int i = 0; i < 12; i++)
            {
                if (this->notes[i] != rhs.notes[i])
                    return false;
            }

            return true;
        }
    };

    // C chromatic
    static const Scale CHROMATIC =
        {
            {
                true, // 0, // C
                true, // 1, // C#
                true, // 2, // D
                true, // 3, // D#
                true, // 4, // E
                true, // 5, // F
                true, // 6, // F#
                true, // 7, // G
                true, // 8, // G#
                true, // 9, // A
                true, // 10, // A#
                true  // 11  // B
            }};

    // C major
    static const Scale MAJOR =
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
                true,  // 9, // A
                false, // 10, // A#
                true   // 11  // B
            }};

    // C minor
    static const Scale MINOR =
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
                true,  // 8, // G#
                false, // 9, // A
                true,  // 10, // A#
                false  // 11  // B
            }};

    // C harmonic minor
    static const Scale HARMONIC_MINOR =
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
                true,  // 8, // G#
                false, // 9, // A
                false, // 10, // A#
                true   // 11  // B
            }};

    // C blues
    static const Scale BLUES =
        {
            {
                true,  // 0, // C
                false, // 1, // C#
                true,  // 2, // D
                true,  // 3, // D#
                true,  // 4, // E
                true,  // 5, // F
                false, // 6, // F#
                true,  // 7, // G
                false, // 8, // G#
                true,  // 9, // A
                false, // 10, // A#
                false  // 11  // B
            }};

    // C pentatonic
    static const Scale PENTATONIC =
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
                false  // 11  // B
            }};


} // namespace w0lf::music
