import pygame
import struct
import os
import time

# Constants
SCREEN_WIDTH = 1000
SCREEN_HEIGHT = 1000
AGENT_RADIUS = 1  # For 2 pixels wide (diameter 2)
BG_COLOR = (0, 0, 0)  # Black
AGENT_COLOR = (255, 124, 0)  # Red
FILE_PATH = "data.bin"
EXPECTED_FILE_SIZE = 92000  # 1000 agents * 23 floats * 4 bytes
NUM_AGENTS = 1000
NUM_FLOATS_PER_AGENT = 23

# Initialize Pygame
pygame.init()
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption("Agent Simulation Display")
clock = pygame.time.Clock()

# List to hold current agent positions (x, y)
agents = []

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # Attempt to read the file
    try:
        if os.path.exists(FILE_PATH):
            with open(FILE_PATH, 'rb') as f:
                data = f.read()
            if len(data) == EXPECTED_FILE_SIZE:
                # Unpack the binary data (little-endian floats)
                all_floats = struct.unpack('<' + 'f' * (NUM_FLOATS_PER_AGENT * NUM_AGENTS), data)
                new_agents = []
                for i in range(NUM_AGENTS):
                    base = i * NUM_FLOATS_PER_AGENT
                    x = all_floats[base]
                    y = all_floats[base + 1]
                    # Only append if within [0,1] for display, but actually draw anyway
                    new_agents.append((x, y))
                agents = new_agents  # Update only if successful
    except Exception as e:
        # Silently handle errors (file in use, corrupt, etc.)
        pass

    # Clear screen
    screen.fill(BG_COLOR)

    # Draw agents
    for x, y in agents:
        pixel_x = int(x * SCREEN_WIDTH)
        pixel_y = int(y * SCREEN_HEIGHT)
        # Clamp to screen if needed, but for now draw even if off-screen (Pygame handles it)
        if 0 <= pixel_x < SCREEN_WIDTH and 0 <= pixel_y < SCREEN_HEIGHT:
            pygame.draw.circle(screen, AGENT_COLOR, (pixel_x, pixel_y), AGENT_RADIUS)

    # Update display
    pygame.display.flip()

    # Cap FPS
    clock.tick(30)

pygame.quit()