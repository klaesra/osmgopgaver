#define NORTH 0
#define SOUTH 1
#define NO_DIRECTION -1

//mutexes for departing busses in each direction
pthread_mutex_t depN_lock;
pthread_mutex_t depS_lock;

pthread_cond_t depN_cond;
pthread_cond_t depS_cond;

void stop_other_direction();

void Approach(int direction);

void Depart(int direction);

int go_this_way(int direction);
