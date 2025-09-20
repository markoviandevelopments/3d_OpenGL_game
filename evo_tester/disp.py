import struct
import matplotlib.pyplot as plt
import numpy as np
import os

# Constants matching the C program
NUM_AGENTS = 120
AGENT_STRUCT_SIZE = 20  # 5 floats * 4 bytes each

def read_agents_from_file(filename):
    """
    Read Agent structs from a binary file and extract x_move_c and y_move_c.
    Returns lists of x_move_c and y_move_c values.
    """
    # Check if file exists
    if not os.path.exists(filename):
        raise FileNotFoundError(f"Error: File '{filename}' not found")
    
    # Check file size
    file_size = os.path.getsize(filename)
    expected_size = NUM_AGENTS * AGENT_STRUCT_SIZE
    if file_size != expected_size:
        raise ValueError(f"Error: File size {file_size} bytes, expected {expected_size} bytes")

    x_move_c_values = []
    y_move_c_values = []

    with open(filename, "rb") as file:
        for _ in range(NUM_AGENTS):
            # Read 20 bytes (5 floats) for each Agent
            data = file.read(AGENT_STRUCT_SIZE)
            if len(data) != AGENT_STRUCT_SIZE:
                raise ValueError("Error: Incomplete data read from file")
            
            # Unpack 5 floats (x, y, food, x_move_c, y_move_c)
            # '<' ensures little-endian, 'f' is for 32-bit float
            agent_data = struct.unpack("<5f", data)
            x_move_c = agent_data[3]  # Fourth float is x_move_c
            y_move_c = agent_data[4]  # Fifth float is y_move_c
            x_move_c_values.append(x_move_c)
            y_move_c_values.append(y_move_c)

    return x_move_c_values, y_move_c_values

def plot_agents(x_move_c_values, y_move_c_values):
    """
    Create a scatter plot of x_move_c vs y_move_c.
    """
    plt.figure(figsize=(8, 8))
    plt.scatter(x_move_c_values, y_move_c_values, color='blue', alpha=0.5, s=50)
    plt.xlabel('x_move_c')
    plt.ylabel('y_move_c')
    plt.title('Agent Movement Coefficients (x_move_c vs y_move_c)')
    plt.grid(True)
    plt.xlim(0, 1)  # x_move_c is in [0, 1]
    plt.ylim(0, 1)  # y_move_c is in [0, 1]
    plt.tight_layout()
    plt.show()

def main():
    filename = "data.bin"
    try:
        x_move_c_values, y_move_c_values = read_agents_from_file(filename)
        print(f"Successfully read {len(x_move_c_values)} agents from {filename}")
        plot_agents(x_move_c_values, y_move_c_values)
    except Exception as e:
        print(f"Error: {str(e)}")

if __name__ == "__main__":
    main()