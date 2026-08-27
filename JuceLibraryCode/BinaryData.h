/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   disabledleft_png;
    const int            disabledleft_pngSize = 19709;

    extern const char*   disabledright_png;
    const int            disabledright_pngSize = 22022;

    extern const char*   left_png;
    const int            left_pngSize = 14621;

    extern const char*   right_png;
    const int            right_pngSize = 15522;

    extern const char*   disabledmic_png;
    const int            disabledmic_pngSize = 31838;

    extern const char*   mic_png;
    const int            mic_pngSize = 34663;

    extern const char*   stoprecording_png;
    const int            stoprecording_pngSize = 13750;

    extern const char*   startrecord_png;
    const int            startrecord_pngSize = 21490;

    extern const char*   pen_png;
    const int            pen_pngSize = 14505;

    extern const char*   loop_png;
    const int            loop_pngSize = 10184;

    extern const char*   noloop_png;
    const int            noloop_pngSize = 5951;

    extern const char*   pause_png;
    const int            pause_pngSize = 2851;

    extern const char*   play_png;
    const int            play_pngSize = 12638;

    extern const char*   add_png;
    const int            add_pngSize = 26197;

    extern const char*   delete_png;
    const int            delete_pngSize = 20139;

    extern const char*   remove_png;
    const int            remove_pngSize = 34063;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 16;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
