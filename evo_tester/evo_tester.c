#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    float x;
    float y;
    // float r;
    // float food;
    // float x_move_c;
    // float y_move_c;
} Agent;

typedef struct {
    Agent agent[3];
} Agents;

void initialize_random_agent(Agent *agent) {
    agent -> x = ((float) (rand() % 1000)) / 1000.0f;
    agent -> y = ((float) (rand() % 1000)) / 1000.0f;
}

Agents agents;

int main(){

    srand(time(NULL));
    Agent agent;
    initialize_random_agent(&agent);
    printf("x: %f, y: %f\n", agent.x, agent.y);

}
