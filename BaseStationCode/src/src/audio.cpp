/**
 * @file audio.cpp
 * @brief Plays audio
 *
 * @author Jotham Gates
 * @version 0.1
 * @date 2023-08-18
 */
#include "audio.h"
#ifdef PIN_SPEAKER

extern SemaphoreHandle_t serialMutex;
extern QueueHandle_t audioQueue;

// Converted from 'Doctor_Who_Theme_Tuneplayer' by TunePlayer Musescore plugin V1.8.2
const uint16_t Doctor_Who_Theme_Tuneplayer[] PROGMEM = {
    0xe0c8, // Tempo change to 200 BPM
    0x4618,0x4618,0x4618,0x4638,0x4618,0x4618,0x4618,0x4618,0x4638,0x4618,
    0x4618,0x4618,0x4618,0x4638,0x4618,0x7638,0x7638,0x5618,0x2618,0x4618,
    0x4618,0x4618,0x4638,0x4618,0x4618,0x4618,0x4618,0x4638,0x4618,0xb438,
    0xb438,0xb438,0xb418,0xb418,0xb418,0x0618,0x1618,0x2618,
    0xd027, // Repeat going back 39 notes, repeating 1 time(s)
    0x4658,0x0858,0xb618,0xb618,0xb618,0xb638,0xb618,0xb618,0xb618,0xb618,
    0xb638,0xb618,0x6618,0x6618,0x6618,0x6638,0x6618,0x2858,0x9658,0xb618,
    0xb618,0xb618,0xb638,0xb618,0x7618,0x7618,0x7618,0x7638,0x7618,0x9618,
    0x9618,0x9618,0x9638,0x9618,0x9618,0x9618,0x9618,0x9618,0x9618,0x9618,
    0xb658,0x7658,0xb4b8,0x2698,0x0618,0xb418,0xb418,0xb418,0xb438,0xb418,
    0xb418,0xb418,0xb418,0xb438,0xb418,0xb418,0xb418,0xb418,0xb438,0xb418,
    0xb418,0xb418,0xb418,0xb438,0xb418,0xb77c,0xb6b8,0x7658,0xb658,0x9678,
    0x7618,0x6618,0x7618,0x7618,0x7618,0x7638,0x7618,0x7618,0x7618,0x7618,
    0x7638,0x7618,0x7658,0x2858,0x4878,0x2818,0x0818,0x2858,0x7658,0x4858,
    0x2838,0x0818,0x2858,0x7638,0xb618,0x9658,0x6658,0x7658,0x9658,0x0878,
    0xb618,0x9618,0xb618,0xb618,0xb618,0xb638,0xb618,0x4618,0x4618,0x4618,
    0x4638,0x4618,0x6658,0x7658,0x6618,0x6618,0x6618,0x6638,0x6618,0x6618,
    0x6618,0x6618,0x6638,0x6618,0x4658,0x4658,0x6658,0x7658,0x9618,0x9618,
    0x9618,0x9638,0x9618,0x9618,0x9618,0x9618,0x0858,0xb618,0xb618,0xb618,
    0xb638,0xb618,0xb618,0xb618,0xb618,0xb638,0xb618,0xb618,0xb618,0xb618,
    0xb638,0xb618,0xb618,0xb618,0xb618,0xb638,0xb618,0x4978,
    0xf000 // End of tune. Stop playing.
};

// Converted from 'Doorbell' by TunePlayer Musescore plugin V1.9.0_debugging (MS3.6.2)
const uint16_t Doorbell[] PROGMEM = {
    0xe064, // Tempo change to 100 BPM
    0xb638,0x7678,
    0xf000 // End of tune. Stop playing.
};

FlashTuneLoader flashLoader;  // Where the notes come from
ToneSound piezo(PIN_SPEAKER); // What plays the notes
TunePlayer tune;              // Coordinates everything and does things at the right times.

// Located here for now as the library doesn't like being included from other files.
void audioTask(void *pvParameters)
{
    // begin processing
    LOGD("AUDIO", "Beginning");
    bool skipWait = false;
    while (true)
    {
        // // Wait for something to play.
        LOGD("AUDIO", "Waiting for a play instruction.");
        AlarmState state;
        if (!skipWait)
        {
            xQueueReceive(audioQueue, (void *)&state, portMAX_DELAY);
        }
        skipWait = false;

        if (state != ALARM_OFF)
        {
            // Start playing
            LOGI("AUDIO", "Playing.");
            // Choose what to play
            const uint16_t *selectedTune;
            switch (state)
            {
                case ALARM_HIGH:
                case ALARM_MEDIUM:
                    selectedTune = Doctor_Who_Theme_Tuneplayer;
                    break;
                case ALARM_DOORBELL:
                    selectedTune = Doorbell;
            }
            flashLoader.setTune(selectedTune);
            tune.begin(&flashLoader, &piezo);
            tune.play();

            // Play the file
            do
            {
                tune.update();
                yield();
                if (xQueueReceive(audioQueue, (void *)&state, 0))
                {
                    skipWait = true;
                }
            } while (tune.isPlaying() && !skipWait);
            tune.stop(); // Go back to beginning.
        }
    }
}
#endif