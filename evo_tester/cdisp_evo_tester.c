#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GRID_W 100
#define GRID_H 100
#define NUM_AGENTS 1000
#define LOAD_FROM_FILE 0
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000
#define CAN_MUTATE 0

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
    int can_die;
    int id;
} Agent;

GLFWwindow* window;

void initialize_random_agent(Agent *agent, int id) {
    agent->x = ((float)(rand() % 1000)) / 1000.0f;
    agent->y = ((float)(rand() % 1000)) / 1000.0f;
    agent->food = 50.0f;
    for (int i = 0; i < 4; i++) {
        float temp = ((float)(rand() % 1000)) / 1000.0f;
        agent->nodes_i[i] = temp;
        agent->nodes[i] = temp;
    }
    for (int i = 0; i < 12; i++) {
        float temp = ((float)(rand() % 1000)) / 1000.0f * 4.0f - 2.0f;
        agent->weights[i] = temp;
    }
    agent->can_die = 1;
    agent->id = id;
}

void agent_eat(Agent *agent) {
    int x_index = (int)((agent->x) * ((float)GRID_W));
    int y_index = (int)((agent->y) * ((float)GRID_H));
    if (x_index < 0) x_index = 0;
    if (x_index >= GRID_W) x_index = GRID_W - 1;
    if (y_index < 0) y_index = 0;
    if (y_index >= GRID_H) y_index = GRID_H - 1;

    if (environment.food[y_index][x_index] > 0.0f) {
        float food_transfered = 0.1f * environment.food[y_index][x_index];
        environment.food[y_index][x_index] -= food_transfered;
        agent->food += food_transfered;
    }
}

void agent_move(Agent *agent) {
    float nodes[4];
    for (int i = 0; i < 4; i++) {
        nodes[i] = agent->nodes[i];
    }

    int weight_index = 0;
    for (int j = 0; j < 4; j++) {
        agent->nodes[j] = agent->nodes_i[j];
        for (int i = 0; i < 4; i++) {
            if (i == j) {
                continue;
            }
            agent->nodes[j] += nodes[i] * agent->weights[weight_index] * 0.3333333f * 0.5f;
            weight_index++;
        }
    }
    for (int i = 0; i < 4; i++) {
        if (agent->nodes[i] > 1.0f) agent->nodes[i] = 1.0f;
        if (agent->nodes[i] < 0.0f) agent->nodes[i] = 0.0f;
    }
    int highest_index = 0;
    float highest_val = -10000.0f;
    float nodes_sum = 0.0f;
    for (int i = 0; i < 4; i++) {
        // if (agent->nodes[i] > highest_val) {
        //     highest_index = i;
        //     highest_val = agent->nodes[i];
        // }
        nodes_sum += agent->nodes[i];
    }
    float r = (((float)(rand() % 1000)) / 1000.0f) * nodes_sum;

    if (r < agent->nodes[0]){
        agent->x += 0.01f;
    } else if (r < agent->nodes[0] + agent->nodes[1]) {
        agent->y += 0.01f;
    } else if (r < agent->nodes[0] + agent->nodes[1] + agent->nodes[2]) {
        agent->x -= 0.01f;
    } else if (r < agent->nodes[0] + agent->nodes[1] + agent->nodes[2] + agent->nodes[3]) {
        agent->y -= 0.01f;
    }

    // if (highest_index == 0) agent->x += 0.01f;
    // if (highest_index == 1) agent->x -= 0.01f;
    // if (highest_index == 2) agent->y += 0.01f;
    // if (highest_index == 3) agent->y -= 0.01f;

    if (agent->x > 1.0f) agent->x = 0.9999f;
    if (agent->y > 1.0f) agent->y = 0.9999f;
    if (agent->x < 0.0f) agent->x = 0.0f;
    if (agent->y < 0.0f) agent->y = 0.0f;
}

void add_food_to_environment() {
    for (int x=0;x<GRID_W;x++) {
        for (int y=0;y<GRID_H;y++) {
            environment.food[y][x] += 0.1f;
        }
    }
}

int save_agents_to_file(const Agent *agents, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file for writing");
        return -1;
    }

    size_t written = fwrite(agents, sizeof(Agent), NUM_AGENTS, file);
    if (written != NUM_AGENTS) {
        fprintf(stderr, "Error writing agents to file: wrote %zu of %d agents\n", written, NUM_AGENTS);
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

int load_agents_from_file(Agent *agents, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file for reading");
        return -1;
    }

    size_t read = fread(agents, sizeof(Agent), NUM_AGENTS, file);
    if (read != NUM_AGENTS) {
        fprintf(stderr, "Error reading agents from file: read %zu of %d agents\n", read, NUM_AGENTS);
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

void init_opengl() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(1);
    }

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Agent Simulation", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        exit(1);
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f); // Map [0,1]x[0,1] to window
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void render_agents(const Agent *agents) {
    glClear(GL_COLOR_BUFFER_BIT);
    glPointSize(2.0f); // 2x2 pixel agents
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f); // White agents
    for (int i = 0; i < NUM_AGENTS; i++) {
        if (agents[i].id == 999) {
            glColor3f(1.0f, 0.0f, 0.0f);
        }
        glVertex2f(agents[i].x, agents[i].y);
    }
    glEnd();
    glfwSwapBuffers(window);
}

int main() {
    for (int x = 0; x < GRID_W; x++) {
        for (int y = 0; y < GRID_H; y++) {
            environment.food[y][x] = 5.0f;
        }
    }

    srand(time(NULL));
    Agent agent[NUM_AGENTS];
    if (LOAD_FROM_FILE) {
        if (load_agents_from_file(agent, "data.bin") != 0) {
            fprintf(stderr, "Failed to load agents, initializing new agents\n");
            for (int i = 0; i < NUM_AGENTS; i++) {
                initialize_random_agent(&agent[i], i);
            }
        }
    } else {
        for (int i = 0; i < NUM_AGENTS; i++) {
            initialize_random_agent(&agent[i], i);
            printf("Index: %d x: %f, y: %f, food: %f\n",
                   i, agent[i].x, agent[i].y, agent[i].food);
        }
    }

    Agent optimal_agent = {
    .x = 0.5f,          // Start in the center of the grid
    .y = 0.5f,          // Start in the center of the grid
    .food = 50.0f,      // Same initial food as other agents
    .nodes = {0.5f, 0.5f, 0.5f, 0.5f}, // Balanced initial node values
    .nodes_i = {0.5f, 0.5f, 0.5f, 0.5f}, // Balanced initial node inputs
    .weights = {
        // Node 0 (right) influences: node 1 (up), node 2 (left), node 3 (down)
        1.5f,  // node 0 -> node 1: strong influence to transition to up
        0.2f,  // node 0 -> node 2: weak influence
        0.2f,  // node 0 -> node 3: weak influence
        // Node 1 (up) influences: node 0 (right), node 2 (left), node 3 (down)
        0.2f,  // node 1 -> node 0: weak influence
        1.5f,  // node 1 -> node 2: strong influence to transition to left
        0.2f,  // node 1 -> node 3: weak influence
        // Node 2 (left) influences: node 0 (right), node 1 (up), node 3 (down)
        0.2f,  // node 2 -> node 0: weak influence
        0.2f,  // node 2 -> node 1: weak influence
        1.5f,  // node 2 -> node 3: strong influence to transition to down
        // Node 3 (down) influences: node 0 (right), node 1 (up), node 2 (left)
        1.5f,  // node 3 -> node 0: strong influence to transition to right
        0.2f,  // node 3 -> node 1: weak influence
        0.2f   // node 3 -> node 2: weak influence
    },
    .can_die = 0,        // Prevent resetting to maintain optimized parameters
    .id = 999
    };

    agent[999] = optimal_agent;

    init_opengl();
    double last_time = glfwGetTime();
    const double frame_time = 1.0 / 60.0; // 60 FPS

    for (int j = 0; j < 1000000000; j++) {
        double current_time = glfwGetTime();
        if (current_time - last_time >= frame_time) {
            for (int i = 0; i < NUM_AGENTS; i++) {
                agent_move(&agent[i]);
                agent_eat(&agent[i]);
                agent[i].food -= 0.1f;
                if (agent[i].food < 0.0f && agent[i].can_die) {
                    agent[i].food = 0.0f;
                    if (CAN_MUTATE) {
                        int rand_agent_index = rand() % NUM_AGENTS;
                        float random_mag = ((float)pow(2.0, -1.0f * ((double)(rand() % 10))));
                        int any_mut = rand() % 2;
                        for (int n = 0; n < 4; n++) {
                            agent[i].nodes_i[n] = agent[rand_agent_index].nodes_i[n];
                            if (rand() % 4 == 0 && any_mut) {
                                agent[i].nodes_i[n] += (((float)(rand() % 1000)) / 1000.0f - 0.5f) * 2.0f * random_mag;
                                if (agent[i].nodes_i[n] > 1.0f) agent[i].nodes_i[n] = 1.0f;
                                if (agent[i].nodes_i[n] < -1.0f) agent[i].nodes_i[n] = -1.0f;
                            }
                        }
                        for (int n = 0; n < 12; n++) {
                            agent[i].weights[n] = agent[rand_agent_index].weights[n];
                            if (rand() % 12 == 0 && any_mut) {
                                agent[i].weights[n] += (((float)(rand() % 1000)) / 1000.0f - 0.5f) * 2.0f * random_mag;
                                if (agent[i].weights[n] > 2.0f) agent[i].weights[n] = 2.0f;
                                if (agent[i].weights[n] < -2.0f) agent[i].weights[n] = -2.0f;
                            }
                        }
                    }
                }
                printf("Index: %d x: %.2f, y: %.2f, food: %.2f    \t%.2f\t%.2f\t%.2f\t%.2f\n", i, agent[i].x, agent[i].y, agent[i].food,agent[i].nodes[0],agent[i].nodes[1],agent[i].nodes[2],agent[i].nodes[3]);
            }
            add_food_to_environment();

            render_agents(agent);
            last_time = current_time;

            if (j % 1000 == 0) {
                if (save_agents_to_file(agent, "data.bin") != 0) {
                    fprintf(stderr, "Failed to save agents\n");
                    glfwTerminate();
                    return 1;
                }
                printf("Agents saved to file\n");
            }
            printf("~~~~~~~~~~~~ITERATION %d~~~~~~~~~~~\n", j);
        }

        glfwPollEvents();
        if (glfwWindowShouldClose(window)) {
            break;
        }
    }

    glfwTerminate();
    return 0;
}