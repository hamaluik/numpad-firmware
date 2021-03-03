#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>
#include <WProgram.h>
#include "jukebox.h"

static const int pin = 10;
static Song* current_song = 0;
static uint32_t last_time = 0, current_delay = 0;
static int wholenote, divider, note_duration;
static int note = 0;
static bool done = false;

void jukebox_init() {
    pinMode(pin, OUTPUT);
}

void jukebox_update() {
    if(current_song == 0) return;
    uint32_t now = millis();

    if(now - last_time >= current_delay) {
        noTone(pin);

        if(done) {
            current_song = 0;
            return;
        }

        divider = current_song->notes[note + 1];
        if(divider > 0) {
            note_duration = wholenote / divider;
        }
        else {
            note_duration = wholenote / abs(divider);
            note_duration *= 1.5;
        }

        tone(pin, current_song->notes[note], note_duration * 0.9);
        current_delay = note_duration;
        last_time = now;

        note += 2;

        if(note >= current_song->num_notes * 2) {
            done = true;
        }
    }
}

void jukebox_play(const Song *song) {
    wholenote = (60000 * 2) / song->tempo;
    note = 0;
    current_song = (Song *)song;
    last_time = millis();
    current_delay = 0;
    done = false;
}

