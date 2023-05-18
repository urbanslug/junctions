import re
import sys


def dot_to_gfa(dot):
    nodes = {}
    edges = []

    # Extract node and edge information from DOT format
    lines = dot.split('\n')
    for line in lines:
        #print(line);
        line = line.strip()
        #print(line);
        if  "color" in line:
            match = re.search(r'\w+', line)
            if match:
                node_label = match.group(0)
                nodes[node_label] = node_label
        elif "->" in line:
            match = re.search( r'(\w+)\s*->\s*(\w+)', line)

            #print(match.group(2))
            if match:
                edge_id = match.group(1)
                #edge_id = match.group(0)
                node_id = match.group(2)
                edges.append((edge_id, node_id))

    # Generate GFA format
    gfa = ''

    # Add sequence records
    for node_id, node_label in nodes.items():
        gfa += f'S\t{node_id}\t{node_label}\n'

    # Add link records
    for edge_id, node_id in edges:
        gfa += f'L\t{edge_id}\t+\t{node_id}\t+\t0M\n'

    return gfa


# Read stdin into a string
dot_graph = sys.stdin.read()

# Print the input string
#print("Input string:")
#print(dot_graph)

gfa_graph = dot_to_gfa(dot_graph)
print(gfa_graph)
