#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "oneLane.h"

int apprS = 0;
int depS = 0;
int apprN = 0;
int depN = 0;

int waiting = NO_DIRECTION;



void stop_other_direction() {
    if (waiting != NO_DIRECTION) {//if we are moving busses in some direction
        if (waiting == NORTH) {//if this direction is north
            pthread_mutex_lock(&depN_lock);
            while (depN > 0) {
                pthread_cond_wait(&depN_cond, &depN_lock);
            }
            pthread_mutex_unlock(&depN_lock);
        }
        else {//the direction is south
            pthread_mutex_lock(&depS_lock);
            while (depS > 0) {
                pthread_cond_wait(&depS_cond, &depS_lock);
            }
            pthread_mutex_unlock(&depS_lock);
        }
        waiting = (waiting + 1) % 2;
    }
}

void Approach(int direction) {
    direction = direction;
}

void Depart(int direction) {
    direction = direction;
}

int go_this_way(int direction) {
    if (direction == waiting) {
        stop_other_direction();
    }
        return direction;

}

int main () {
    return 0;
}
