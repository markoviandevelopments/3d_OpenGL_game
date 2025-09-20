#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define GRID_W 40
#define GRID_H 40
#define NUM_AGENTS 1000
#define LOAD_FROM_FILE 0




typedef struct {
    float food[GRID_H][GRID_W];
} Environment;

Environment environment;

typedef struct {
    float x;
    float y;
    float food;
    float nodes[4];
    float nodes_i[4];
    float weights[12];
} Agent;

void initialize_random_agent(Agent *agent) {
    agent -> x = ((float) (rand() % 1000)) / 1000.0f;
    agent -> y = ((float) (rand() % 1000)) / 1000.0f;
    agent -> food = 50.0f;
    for (int i=0;i<4;i++) {
        float temp = ((float) (rand() % 1000)) / 1000.0f;
        agent -> nodes_i[i] = temp;
        agent -> nodes[i] = temp;
    }
    for (int i = 0; i < 12; i++)
    {
        float temp = ((float)(rand() % 1000)) / 1000.0f * 2.0f;
        agent->weights[i] = temp;
    }
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
    float nodes[4];
    for (int i=0;i<4;i++) {
        nodes[i] = agent->nodes_i[i];
    }

    int weight_index = 0;
    for (int j=0;j<4;j++) {
        agent->nodes[j] = 0.0f;
        for (int i=0;i<4;i++){
            if (i==j) {
                continue;
            }
            agent->nodes[j] += nodes[i] * agent->weights[weight_index];
            weight_index++;
        }
    }
    for (int i = 0; i < 4; i++)
    {
        if (agent->nodes[i] > 1.0f) agent->nodes[i] = 1.0f;
        if (agent->nodes[i] < 0.0f) agent->nodes[i] = 0.0f;
    }
    int highest_index = 0;
    float highest_val = -1000.0f;
    for (int i = 0; i < 4; i++)
    {
        if (agent->nodes[i] > highest_val) {
            highest_index = i;
            highest_val = agent->nodes[i];
        }
    }
    if (highest_index == 0) agent->x += 0.01f;
    if (highest_index == 1) agent->x -= 0.01f;
    if (highest_index == 2) agent->y += 0.01f;
    if (highest_index == 3) agent->y -= 0.01f;

}

void add_food_to_environment() {
    for (int i=0;i<20;i++) {
        int x_index = rand() % GRID_W;
        int y_index = rand() % GRID_H;
        environment.food[y_index][x_index] += 0.1f * ((float) (NUM_AGENTS));
    }
}

int save_agents_to_file(const Agent *agents, const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        perror("Error opening file for writing");
        return -1;
    }

    size_t written = fwrite(agents, sizeof(Agent), NUM_AGENTS, file);
    if (written != NUM_AGENTS)
    {
        fprintf(stderr, "Error writing agents to file: wrote %zu of %d agents\n", written, NUM_AGENTS);
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

int load_agents_from_file(Agent *agents, const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        perror("Error opening file for reading");
        return -1;
    }

    size_t read = fread(agents, sizeof(Agent), NUM_AGENTS, file);
    if (read != NUM_AGENTS)
    {
        fprintf(stderr, "Error reading agents from file: read %zu of %d agents\n", read, NUM_AGENTS);
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

int main(){


    for (int x=0;x<GRID_W;x++) {
        for (int y=0;y<GRID_H;y++) {
            environment.food[y][x] = 5.0f;
        }
    }

    srand(time(NULL));
    Agent agent[NUM_AGENTS];
    if (LOAD_FROM_FILE)
    {
        if (load_agents_from_file(agent, "data.bin") != 0)
        {
            fprintf(stderr, "Failed to load agents, initializing new agents\n");
            for (int i = 0; i < NUM_AGENTS; i++)
            {
                initialize_random_agent(&agent[i]);
            }
        }
    }
    else
    {
        for (int i = 0; i < NUM_AGENTS; i++)
        {
            initialize_random_agent(&agent[i]);
            printf("Index: %d x: %f, y: %f, food: %f\n",
                   i, agent[i].x, agent[i].y, agent[i].food);
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


    for (int j=0;j<100000;j++) {
        if (j % 10000 == 0)
        {
            if (save_agents_to_file(agent, "data.bin") != 0)
            {
                fprintf(stderr, "Failed to save agents\n");
                return 1;
            }
            printf("Agents saved to file\n");
        }
        printf("~~~~~~~~~~~~ITERATION %d~~~~~~~~~~~\n", j);
        for (int i=0;i<NUM_AGENTS;i++) {
            agent_move(&agent[i]);
            agent_eat(&agent[i]);
            agent[i].food -= 0.1f;
            if (agent[i].food < 0.0f) {
                agent[i].food = 0.0f;
                int rand_agent_index = rand()% NUM_AGENTS;
                float random_mag = ((float) pow(2.0, -1.0f * ((double) (rand() % 10))));
                int any_mut = rand()%2;
                for (int n=0;n<4;n++) {
                    agent[i].nodes_i[n] = agent[rand_agent_index].nodes_i[n];
                    if (rand() % 4 == 0 && any_mut) {
                        agent[i].nodes_i[n] += (((float)(rand() % 1000)) / 1000.0f - 0.5f) * 2.0f * random_mag;
                        if (agent[i].nodes_i[n] > 1.0f) agent[i].nodes_i[n] = 1.0f;
                        if (agent[i].nodes_i[n] < -1.0f) agent[i].nodes_i[n] = -1.0f;
                    }
                }
                for (int n=0;n<16;n++) {
                    agent[i].weights[n] = agent[rand_agent_index].weights[n];
                    if (rand() % 16 == 0 && any_mut) {
                        agent[i].weights[n] += (((float)(rand() % 1000)) / 1000.0f - 0.5f) * 2.0f * random_mag;
                        if (agent[i].weights[n] > 2.0f) agent[i].weights[n] = 2.0f;
                        if (agent[i].weights[n] < -2.0f) agent[i].weights[n] = -2.0f;
                    }
                }
            }
            
            
            printf("Index: %d x: %.2f, y: %.2f, food: %.2f\n", i, agent[i].x, agent[i].y,agent[i].food);

        }
        add_food_to_environment();

    }
    
    

}
