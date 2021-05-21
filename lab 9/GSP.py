import sys
from BlocksWorld import BlocksWorld

# Initialise environment
B = BlocksWorld()

# Read inputs
goal_state = B.read_inputs(sys.argv[1])

# Goal Stack Planning
B.goal_stack_planning(goal_predicates=goal_state)

# Write to output
with open(sys.argv[2], 'w+') as file:
    B.write_to_file(file)
