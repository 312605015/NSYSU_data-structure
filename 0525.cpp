#include <stdio.h>
#include <portaudio.h>

#define FILE_PATH "ai.wav"

int main()
{
    Pa_Initialize();
    
    PaStream *stream;
    Pa_OpenDefaultStream(&stream, 0, 1, paInt16, 44100, paFramesPerBufferUnspecified, NULL, NULL);
    
    FILE *file = fopen(FILE_PATH, "rb");
    if (!file) {
        printf("無法打開檔案: %s\n", FILE_PATH);
        return 1;
    }
    
    short buffer[1024];
    while (!feof(file)) {
        fread(buffer, sizeof(short), sizeof(buffer)/sizeof(short), file);
        Pa_WriteStream(stream, buffer, sizeof(buffer)/sizeof(short));
    }
    
    fclose(file);
    
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
    
    return 0;
}

