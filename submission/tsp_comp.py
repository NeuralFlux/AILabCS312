import random
import sys
import numpy as np
from time import time


class Ant(object):
    def __init__(self, num_cities):
        self.tour = [random.randint(0, num_cities - 1)]

    def reset(self):
        self.tour = []

    def simulate(self, graph, pheromone, num_cities, alpha, beta):
        while len(self.tour) != num_cities:
            # initialise allowed
            curr_city = self.tour[-1]
            allowed = list(set(list(range(num_cities))).difference(set(self.tour)))

            # calculate cumulative probability vector
            net_prob = 0
            cum_probs = []
            for j in range(len(allowed)):
                if allowed[j] != curr_city:
                    net_prob += (pheromone[curr_city][allowed[j]] ** alpha) * ((1 / graph[curr_city][allowed[j]]) ** beta)
                    cum_probs.append(net_prob)

            # choose next city
            next_city = random.choices(allowed, cum_weights=cum_probs)[0]
            self.tour.append(next_city)

        assert len(self.tour) == num_cities
        return self.tour


class TSP(object):
    def __init__(self, filename):
        with open(filename, 'r') as inp_file:
            lines = inp_file.readlines()

        self.graph = []
        self.num_cities = int(lines[1].strip())
        for idx in range(self.num_cities + 2, len(lines)):
            temp_line = lines[idx].strip()
            self.graph.append(list(map(float, temp_line.split(" "))))

    def cost_of_tour(self, tour):
        cost = 0

        for city in range(self.num_cities - 1):
            cost += self.graph[tour[city]][tour[city + 1]]

        return cost

    def ant_colony_optimizer(self, alpha, beta, Q, nad, retain):
        # Init hyperparams
        rho = 0.3
        num_ants = int(self.num_cities // nad)

        # Init pheromone
        rand_f = random.random() / 10
        pheromone = np.ones((self.num_cities, self.num_cities)) * rand_f
        init_pheromone = pheromone

        # Init best tour
        best_tour, best_cost = [], float('Inf')
        assert best_cost == float('Inf')

        # start algo
        last_cost = float('Inf')
        identical_cost = 0
        broken, last_iter = False, False
        while True:
            if (time() - start_time) > 220:
                last_iter = True

            tours = []
            # print(f"Iter: {curr_iter}", f"Best cost: {best_cost}")

            tour_costs = []
            tours = []

            # Simulate all ants
            try:
                for l in range(num_ants):
                    if last_iter is True:
                        if (time() - start_time) > 290:
                            broken = True
                            break
                    ant = Ant(self.num_cities)
                    curr_tour = ant.simulate(self.graph, pheromone, self.num_cities, alpha, beta)
                    curr_cost = self.cost_of_tour(curr_tour)

                    if curr_cost < best_cost:
                        best_cost = curr_cost
                        best_tour = curr_tour[:]

                    tour_costs.append(curr_cost)
                    tours.append(curr_tour)
            except TypeError as e:
                print(num_ants)
                raise e

            if broken is True:
                break

            # Update pheromone of top retain% ants
            assert len(tour_costs) == num_ants
            ants = list(range(num_ants))
            sorted_ant_costs = [x for _, x in sorted(zip(tour_costs, ants))]
            ants_to_update = (num_ants * retain) // 100

            update_pher = np.zeros((self.num_cities, self.num_cities))
            for k in sorted_ant_costs[:ants_to_update]:
                for city in range(len(tours[k]) - 1):
                    update_pher[tours[k][city]][tours[k][city + 1]] += Q / tour_costs[k]

            pheromone = rho * pheromone + (1 - rho) * update_pher

            if last_cost == best_cost:
                identical_cost += 1
            else:
                identical_cost = 0

            last_cost = best_cost

            if identical_cost >= 10:
                pheromone = init_pheromone
                identical_cost = 0

        # print best solution
        print("Best Cost: ", best_cost)
        print(best_tour)

        return best_cost


start_time = time()

T = TSP(sys.argv[1])
T.ant_colony_optimizer(3, 5, 0.0001 * 1500, 0.75, 50)
