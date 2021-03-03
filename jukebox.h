#ifndef _JUKEBOX_H_
#define _JUKEBOX_H_

typedef struct {
    int tempo;
    const int *notes;
    int num_notes;
} Song;

void jukebox_init();
void jukebox_update();
void jukebox_play(const Song *song);

#endif
