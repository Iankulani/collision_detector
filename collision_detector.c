#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MAX_NODES 10
#define MAX_DISTANCE 100  // Maximum distance between nodes (arbitrary units)
#define TRANSMISSION_RANGE 50  // Distance within which nodes can hear each other (arbitrary units)

typedef struct {
    int node_id;
    int x, y;  // Position in a 2D space (x, y)
    int is_transmitting;
} Node;

void *node_activity(void *arg);
int detect_collision(Node *transmitting_node, Node nodes[], int num_nodes);
double get_distance(Node *node1, Node *node2);
int can_hear(Node *node1, Node *node2);

int main() {
    srand(time(NULL));

    // Get user input for number of nodes
    int num_nodes;
    printf("Enter the number of nodes in the network (max %d):", MAX_NODES);
    scanf("%d", &num_nodes);
    if (num_nodes > MAX_NODES) {
        printf("Limiting number of nodes to %d.\n", MAX_NODES);
        num_nodes = MAX_NODES;
    }

    Node nodes[num_nodes];

    // Create nodes with random positions
    for (int i = 0; i < num_nodes; i++) {
        nodes[i].node_id = i;
        nodes[i].x = rand() % (MAX_DISTANCE + 1);
        nodes[i].y = rand() % (MAX_DISTANCE + 1);
        nodes[i].is_transmitting = 0;
    }

    // Create threads for each node
    pthread_t threads[num_nodes];
    for (int i = 0; i < num_nodes; i++) {
        pthread_create(&threads[i], NULL, node_activity, &nodes[i]);
    }

    // Run the simulation for 30 seconds
    sleep(30);

    // End simulation and cleanup
    printf("\nSimulation completed. Closing network activity...\n");
    return 0;
}

void *node_activity(void *arg) {
    Node *node = (Node *)arg;

    while (1) {
        // Randomly decide whether the node should transmit (30% chance)
        if (rand() % 100 < 30) {
            printf("Node %d is preparing to transmit.\n", node->node_id);

            // Detect collision
            int collision = detect_collision(node, (Node *)arg, MAX_NODES);
            if (collision) {
                printf("Collision detected at Node %d!\n", node->node_id);
            } else {
                node->is_transmitting = 1;
                printf("Node %d is transmitting data...\n", node->node_id);
                sleep(rand() % 2 + 1);  // Simulate transmission duration (1 to 2 seconds)
                node->is_transmitting = 0;
                printf("Node %d has finished transmitting.\n", node->node_id);
            }
        }

        // Wait before trying to transmit again (random time between 1 and 3 seconds)
        sleep(rand() % 3 + 1);
    }

    return NULL;
}

int detect_collision(Node *transmitting_node, Node nodes[], int num_nodes) {
    int collision_detected = 0;

    // Check if there are other nodes transmitting and within range
    for (int i = 0; i < num_nodes; i++) {
        if (nodes[i].node_id != transmitting_node->node_id && nodes[i].is_transmitting) {
            // Check if the transmitting nodes are within range of each other (hidden terminal)
            if (can_hear(transmitting_node, &nodes[i])) {
                // No collision, as both nodes can hear each other
                continue;
            } else {
                // Nodes are within range of the receiver but cannot hear each other (hidden terminal)
                printf("Hidden terminal collision: Node %d and Node %d are transmitting at the same time!\n",
                       transmitting_node->node_id, nodes[i].node_id);
                collision_detected = 1;
                break;
            }
        }
    }

    return collision_detected;
}

double get_distance(Node *node1, Node *node2) {
    return sqrt(pow(node1->x - node2->x, 2) + pow(node1->y - node2->y, 2));
}

int can_hear(Node *node1, Node *node2) {
    double distance = get_distance(node1, node2);
    return distance <= TRANSMISSION_RANGE;
}
