import random
import sys
from copy import deepcopy


class SpyAssistCLI(object):

    actions = {"U": (-1, 0), "D": (1, 0), "L": (0, -1), "R": (0, 1)}
    symbols = ['↑', '↓', '←', '→', '•']

    grid = 0
    start = (0, 0)
    goal = (0, 0)

    p_d_plates = float(0)
    p_err = float(0)
    fin_rew = float(1)

    def __init__(self, grid=4, start=(0, 2), goal=(3, 2), p_err=0.12, p_d_plates=0.2, d_pen=-0.5, fin_rew=1, step_pen=-0.01):

        # Initialise environment params
        SpyAssistCLI.grid = grid

        SpyAssistCLI.start = start
        SpyAssistCLI.goal = goal
        if grid != 4:
            SpyAssistCLI.goal = (grid - 1, 2)

        SpyAssistCLI.p_err = p_err
        SpyAssistCLI.p_d_plates = p_d_plates

        # Create the environment
        self.states = self.spawn_states()
        self.present_state = self.states[start[0]][start[1]]

        # Define rewards/penalties
        self.d_pen = d_pen
        self.fin_rew = fin_rew
        self.step_pen = step_pen

        # # Print init config
        # self.print_view(SpyAssistCLI.grid)

    def spawn_states(self):
        states = []

        # Generate states square-wise
        for y in range(SpyAssistCLI.grid):
            temp_states = []
            for x in range(SpyAssistCLI.grid):
                if (y, x) == SpyAssistCLI.start:
                    temp_states.append(State(False, True))
                elif (y, x) == SpyAssistCLI.goal:
                    temp_states.append(State(True))
                else:
                    temp_states.append(State())
                    if (y, x) == (1, 2):
                        temp_states[-1].is_DP = True
                    else:
                        temp_states[-1].is_DP = False
            states.append(temp_states[:])

        return states

    def print_view(self, grid):
        """
        Prints maze with Agent (A), Detector Plates (D) and Goal (G)
        """
        for i in range(grid):
            print("-" * ((2 * grid) + 1))

            for j in range(grid):
                grid_char = ' '
                if self.states[i][j].is_agent:
                    grid_char = 'A'
                if self.states[i][j].is_DP:
                    grid_char = 'D'
                if self.states[i][j].is_target:
                    grid_char = 'G'

                print('|' + grid_char, end='')

            print('|')

        print("-" * ((2 * grid) + 1))

    def print_pol_val(self, grid):
        """
        Prints maze with V value and Policy
        """

        for i in range(grid):
            print("-" * ((14 * grid) + 1))
            for j in range(grid):
                grid_char = ' '
                if self.states[i][j].is_agent:
                    grid_char = 'A'
                if self.states[i][j].is_DP:
                    grid_char = 'D'
                if self.states[i][j].is_target:
                    grid_char = 'G'

                print('|' + ' ' + grid_char + ' ' + '{:7.3f}'.format(self.states[i][j].V) + ' ' + self.symbols[self.states[i][j].P] + ' ', end='')

            print('|')

        print("-" * ((14 * grid) + 1))

    def state_in_bounds(self, potential_state):
        """
        Checks if the potential_state is within map boundaries
        """

        assert len(potential_state) == 2

        if potential_state[0] >= 0 and potential_state[1] >= 0:
            if potential_state[0] <= self.grid - 1 and potential_state[1] <= self.grid - 1:
                return True

        return False

    def next_state_mapper(self, present_state, action):
        """
        Returns the state resulting from the given action in the present state
        """

        potential_state = tuple(sum(item) for item in zip(self.actions[action], present_state))

        if self.state_in_bounds(potential_state):
            next_state = potential_state
        else:
            next_state = present_state

        return next_state

    def reward(self, next_state):
        """
        Returns reward for going to next_state
        """

        # Initialise reward
        rew = 0

        # Select next_state
        next_state = self.states[next_state[0]][next_state[1]]

        # Penalise if detector plate
        if next_state.is_DP:
            rew = self.d_pen

        # Add step_pen for each step
        return rew + self.step_pen

    def calc_q_star(self, present_state, action):
        """
        Calculates Q* for a given action in the present_state
        """

        # Calculate probability of correct transmission
        correct_prob = 1 - SpyAssistCLI.p_err

        # Calculate probability of other (wrong) actions
        diff_prob = SpyAssistCLI.p_err / 3

        # Initialise arrays for possible actions and their probabilities
        pos_actions = [action]
        pos_actions.extend(list(set(SpyAssistCLI.actions.keys()).difference(set([action]))))
        pos_probs = [correct_prob, diff_prob, diff_prob, diff_prob]

        # For each possible resultant action, add its expected reward
        result = 0
        for i in range(len(pos_actions)):
            next_state = self.next_state_mapper(present_state, pos_actions[i])
            result += pos_probs[i] * (self.reward(next_state) + self.states[next_state[0]][next_state[1]].V)

        return result

    def check_diff(self, old_states, mode='v'):
        """
        Calculates average difference for a given set of states with its
        previous states
        """

        if mode == 'v':
            diff = [abs(old_ij - new_ij) for old_i, new_i in zip(old_states, self.states) for old_ij, new_ij in zip(old_i, new_i)]
        elif mode == 'p':

            diff = [abs(old_ij.P - new_ij.P) for old_i, new_i in zip(old_states, self.states) for old_ij, new_ij in zip(old_i, new_i)]
        else:
            raise ValueError

        assert len(diff) == SpyAssistCLI.grid ** 2

        return sum(diff) / len(diff)

    def calc_pol_val(self, y, x):
        """
        Calculates Policy and V value for given (y, x) state
        """

        # calculate Q
        self.states[y][x].Q = [self.calc_q_star((y, x), action) for action in list(SpyAssistCLI.actions)]

        # Calculate max Q
        argmax = -1
        max_Q = -1
        for q_index, q in enumerate(self.states[y][x].Q):
            if q > max_Q:
                max_Q = q
                argmax = q_index

        return (max_Q, argmax)

    def update_states(self, mode='v'):
        """
        Updates Values or Policies of each state depending on the mode
        """

        for y in range(SpyAssistCLI.grid):
            for x in range(SpyAssistCLI.grid):

                # If destination, skip it as only one action left
                if (y, x) == SpyAssistCLI.goal:
                    continue

                # Otherwise, calculate V value for the state
                else:
                    if mode == 'v':
                        self.states[y][x].V, _ = self.calc_pol_val(y, x)
                    elif mode == 'p':
                        _, self.states[y][x].P = self.calc_pol_val(y, x)
                    else:
                        raise ValueError

    def value_iteration(self):
        """
        Does Value Iteration for the MDP, halts when difference of V values
        is less than a pre-determined epsilon
        """

        # Initialise parameters
        iters = 0
        old_states = []
        diff = float('inf')

        # Begin algorithm
        while diff > 0.01:

            # Copy previous states
            old_states = deepcopy(self.states)

            # Update V values
            self.update_states(mode='v')

            # Update difference and iterations
            diff = self.check_diff(old_states)
            iters += 1

        # Update policy to optimal
        self.update_states(mode='p')

        # Print results
        print("Finished in", iters, "iterations with difference of", diff)

        print("\n\n------------- Optimal Policy with V values -------------")
        self.print_pol_val(SpyAssistCLI.grid)

    def policy_eval(self):
        """
        Updates the V values for a given set of policies
        """

        # Make a list of actions for reference
        actions = list(self.actions)

        # For each state, calculate V value for the given policy
        for y in range(SpyAssistCLI.grid):
            for x in range(SpyAssistCLI.grid):
                if (y, x) == SpyAssistCLI.goal:
                    continue
                else:
                    self.states[y][x].V = self.calc_q_star((y, x), actions[self.states[y][x].P])

    def policy_iteration(self):
        """
        Does Policy Iteration for the MDP, halts when difference of Policies
        is less than a pre-determined epsilon
        """

        # Initialise parameters
        iters = 0
        old_states = []
        diff = float('inf')

        # Begin algorithm
        while diff > 0.01 or iters < 3:

            # Copy previous states
            old_states = deepcopy(self.states)

            # Evaluate the current policy
            self.policy_eval()

            # Update the policy wrt evaluation
            self.update_states(mode='p')

            # Update difference and iterations
            diff = self.check_diff(old_states, mode='p')
            iters += 1

        # Update values to optimal
        self.update_states(mode='v')

        # Print results
        print("Finished in", iters, "iterations with difference of", diff)

        print("\n\n------------- Optimal Policy with V values -------------")
        self.print_pol_val(SpyAssistCLI.grid)


class State(SpyAssistCLI):

    def __init__(self, target=False, start=False):

        # Initalise parameters of state
        self.V = 0
        self.Q = 0
        self.P = random.randint(0, 3)
        self.is_DP = False

        # Mark detector plate with probability
        if(target is False and start is False):
            if(random.random() <= super().p_d_plates):
                self.is_DP = True

        # Mark square with agent
        self.is_agent = start

        # Mark square with goal
        self.is_target = target
        if target is True:
            self.V = super().fin_rew
            self.P = 4

    def __sub__(self, other):
        return self.V - other.V


# File reading
grid, p_err, d_pen, fin_rew, step_pen = 0, float(0), float(0), float(0), float(0)
with open(sys.argv[1], 'r') as file:
    skip = False
    for line in file:
        if skip is False:
            skip = True

            vals = line.replace('\n', '').split(',')
            grid = int(vals[0])
            p_err = float(vals[1])
            d_pen = float(vals[2])
            fin_rew = float(vals[3])
            step_pen = float(vals[4])

# Check for values
if grid < 3:
    print("Please keep grid bigger than or equal to 3x3 at least")
    raise ValueError

if step_pen > 0 or d_pen > 0:
    print("Hey there! Penalties can't be greater than 0 :P")
    raise ValueError

if fin_rew <= 0:
    print("Hey there! Rewards can't be lesser than or equal to 0 :P")
    raise ValueError

if p_err > 1 or p_err < 0:
    print("Hey there! Probability lies within [0, 1] :P")
    raise ValueError

# Initialise our environment
S = SpyAssistCLI(grid=grid, p_err=p_err, d_pen=d_pen, fin_rew=fin_rew, step_pen=step_pen)

# Print initial configuration
print("------------- The Environment -------------")
S.print_view(SpyAssistCLI.grid)

# Perform Value Iteration
print("\n\n\n------------- Value Iteration for an MDP -------------")
S.value_iteration()

# Reset
S = SpyAssistCLI(grid=grid, p_err=p_err, d_pen=d_pen, fin_rew=fin_rew, step_pen=step_pen)

# Perform Policy Iteration
print("\n\n\n------------- Policy Iteration for an MDP -------------")
S.policy_iteration()
