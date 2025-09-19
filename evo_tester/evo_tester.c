#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define GRID_W 10
#define GRID_H 10
#define NUM_AGENTS 100
#define LOAD_FROM_FILE 0

FILE *fptr;


typedef struct {
    float food[GRID_H][GRID_W];
} Environment;

Environment environment;

typedef struct {
    float x;
    float y;
    float food;
    float x_move_c;
    float y_move_c;
} Agent;

void initialize_random_agent(Agent *agent) {
    agent -> x = ((float) (rand() % 1000)) / 1000.0f;
    agent -> y = ((float) (rand() % 1000)) / 1000.0f;
    agent -> food = 50.0f;
    agent -> x_move_c = ((float) (rand() % 1000)) / 1000.0f;
    agent -> y_move_c = ((float) (rand() % 1000)) / 1000.0f;
}

void agent_eat(Agent *agent) {
    int x_index = (int) ((agent -> x) * ((float) GRID_W));
    int y_index = (int) ((agent -> y) * ((float) GRID_H));
    if (x_index < 0) x_index = 0;
    if (x_index >= GRID_W) x_index = GRID_W - 1;

    if (y_index < 0) y_index = 0;
    if (y_index >= GRID_H) y_index = GRID_H - 1;

    if (environment.food[y_index][x_index] > 0.0f) {
        float food_transfered = 0.1f * environment.food[y_index][x_index];
        environment.food[y_index][x_index] -= food_transfered;
        agent -> food += food_transfered;
    }
}

void agent_move(Agent *agent) {
    float r1 = ((float) (rand() % 1000)) / 1000.0f;
    float r2 = ((float) (rand() % 1000)) / 1000.0f;

    if (r1 < (agent -> x_move_c)) {
        agent -> x += ((float) (rand() % 1000)) / 1000.0f * 0.2f - 0.1f;
        if (agent -> x >= 1.0f) {
            agent -> x = 0.99999f;
        }
        if (agent -> x < 0.0f) {
            agent -> x = 0.0f;
        }

    }
    if (r2 < (agent -> y_move_c)) {
        agent -> y += ((float) (rand() % 1000)) / 1000.0f * 0.2f - 0.1f;

        if (agent -> y >= 1.0f) {
            agent -> y = 0.99999f;
        }
        if (agent -> y < 0.0f) {
            agent -> y = 0.0f;
        }
    }


}

void add_food_to_environment() {
    int x_index = rand() % GRID_W;
    int y_index = rand() % GRID_H;
    environment.food[y_index][x_index] += 0.1f * ((float) (NUM_AGENTS));
}



int main(){



    for (int x=0;x<GRID_W;x++) {
        for (int y=0;y<GRID_H;y++) {
            environment.food[y][x] = 5.0f;
        }
    }

    srand(time(NULL));
    Agent agent[NUM_AGENTS];
    if (LOAD_FROM_FILE) {
        fptr = fopen("data.bin", "r");
        fread(&agent[NUM_AGENTS],sizeof(Agent) * (unsigned long) NUM_AGENTS,1,fptr);
        fclose(fptr);
    } else {
        for (int i=0;i<NUM_AGENTS;i++) {
            initialize_random_agent(&agent[i]);
            printf("Index: %d x: %f, y: %f, food: %f, x_move_c: %f, y_move_c: %f\n", i, agent[i].x, agent[i].y,agent[i].food,agent[i].x_move_c,agent[i].y_move_c);
        }
    }

    // Agent optimal_agent = {
    // .x = 0.5f,         // Start in the middle of a grid cell (e.g., cell [5,5])
    // .y = 0.5f,         // Middle of a grid cell for balanced exploration
    // .food = 50.0f,     // Same initial food as random agents
    // .x_move_c = 1.0f,  // Always move in x-direction to explore food hotspots
    // .y_move_c = 1.0f   // Always move in y-direction to explore food hotspots
    // };

    // agent[0] = optimal_agent;

    fptr = fopen("data.bin", "w");
    fclose(fptr);

    for (int j=0;j<1;j++) {
        if (j % 100 == 0) {
            fptr = fopen("data.bin", "w");
            fwrite(&agent[NUM_AGENTS],sizeof(Agent) * (unsigned long) NUM_AGENTS,1,fptr);
            fclose(fptr);
        }
        printf("~~~~~~~~~~~~ITERATION %d~~~~~~~~~~~\n", j);
        for (int i=0;i<NUM_AGENTS;i++) {
            agent_move(&agent[i]);
            agent_eat(&agent[i]);
            agent[i].food -= 0.1f;
            if (agent[i].food < 0.0f) {
                agent[i].food = 0.0f;
                int rand_agent_index = rand()% NUM_AGENTS;
                agent[i].x_move_c = agent[rand_agent_index].x_move_c;
                agent[i].y_move_c = agent[rand_agent_index].y_move_c;
                if (rand() % 4 == 0) {
                    agent[i].x_move_c += ((float) (rand() % 1000)) / 1000.0f * 0.2f - 0.1f * ((float) pow(2, (double) (-2 * (rand() % 10))));
                    if (agent[i].x_move_c < 0.0f) agent[i].x_move_c = 0.0f;
                    if (agent[i].x_move_c > 1.0f) agent[i].x_move_c = 1.0f;
                }
                if (rand() % 4 == 0) {
                    agent[i].y_move_c += ((float) (rand() % 1000)) / 1000.0f * 0.2f - 0.1f * ((float) pow(2, (double) (-2 * (rand() % 10))));
                    if (agent[i].y_move_c < 0.0f) agent[i].y_move_c = 0.0f;
                    if (agent[i].y_move_c > 1.0f) agent[i].y_move_c = 1.0f;
                }
            }
            
            
            printf("Index: %d x: %.2f, y: %.2f, food: %.2f, x_move_c: %.5f, y_move_c: %.5f,\t total: %.2f\n", i, agent[i].x, agent[i].y,agent[i].food,agent[i].x_move_c,agent[i].y_move_c,agent[i].x_move_c+agent[i].y_move_c);

        }
        add_food_to_environment();

    }
    
    

}
