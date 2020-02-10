import random
import sys


class Ant(object):
    def __init__(self, num_cities):
        self.tour = []

    def reset(self):
        self.tour = []


class TSP(object):
    def __init__(self, filename):
        with open(filename, 'r') as inp_file:
            lines = inp_file.readlines()

        self.graph = []
        self.num_cities = int(lines[1].strip())
        for idx in range(self.num_cities + 2, len(lines)):
            temp_line = lines[idx].strip()
            self.graph.append(list(map(float, temp_line.split(" "))))

    def ant_colony_optimizer(self, num_ants, num_iters):
        # Init hyperparams
        alpha, beta, Q, rho = 1, 1, 5, 0.3

        # Init ants
        ants = []
        for i in range(num_ants):
            temp_ant = Ant()
            ants.append(temp_ant)

        # Init pheromone
        rand_f = random.random()
        pheromone = [[rand_f for j in range(self.num_cities)] for i in range(self.num_cities)]

        # Init best tour
        best_tour, best_len = [], 9223372036854775807
        assert best_len == 9223372036854775807

        for curr_iter in range(num_iters):
            tours = []
            print(f"Iter: {curr_iter}")

            for l in range(num_ants):
                while len(ants[l].tour) != self.num_cities:
                    curr_city = ants[l].tour[-1]

                    net_prob = 0
                    probs = []
                    for j in range(self.num_cities):
                        if j != curr_city:
                            net_prob += (pheromone[curr_city][j] ** alpha) * ((1 / graph[curr_city][j]) ** beta)
                    



T = TSP(sys.argv[1])
