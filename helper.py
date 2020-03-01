configs = [(3, 5, 0.0001, 0.75, 50)]
args = ['TestInputs/euc_100', 'TestInputs/euc_250', 'TestInputs/noneuc_100']
costs = []
progress = tqdm(total=len(configs) * 4 * 3, desc='Progress', position=0)
for config in configs:
    arg_costs = []
    for arg in args:
        T = TSP(arg)

        cost = 0
        for i in range(4):
            cost += T.ant_colony_optimizer(10, config[0], config[1], config[2] * best_dist, config[3], config[4])
            progress.update()

        arg_costs.append(cost / 4)

    costs.append(arg_costs)

print(arg_costs)
print(sum(costs[0])/3)
exit()

sorted_configs = [x for _, x in sorted(zip(costs, configs))]
costs.sort()
for i in range(10):
    print(sorted_configs[i], ":", costs[i])

exit()

num_ants_den = [1, 2, 3, 0.5, 0.75]
alphas = [3, 5]
betas = [3, 5]
qs = [0.0001, 0.001, 0.01]
retains = [10, 25, 50, 75]

total = len(num_ants_den) * len(alphas) * len(betas) * len(qs) * len(retains)

best_cost = float('Inf')
best_tuple = ()

prog = tqdm(total=total, desc='Progress', position=0)
tour_costs = []
tour_configs = []
cost_dict = {}

for nad in num_ants_den:
    for alpha in alphas:
        for beta in betas:
            for q in qs:
                for retain in retains:
                    try:
                        cost = T.ant_colony_optimizer(10, alpha, beta, q * best_dist, nad, retain)
                        tour_costs.append(cost)
                        tour_configs.append((alpha, beta, q, nad, retain))
                        cost_dict[(alpha, beta, q, nad, retain)] = cost

                        if cost < best_cost:
                            best_cost = cost
                            best_tuple = (alpha, beta, q, nad, retain)

                        prog.update()
                    except TypeError as e:
                        print(best_cost, best_tuple)
                        raise e

print(best_cost, best_tuple)

sorted_configs = [x for _, x in sorted(zip(tour_costs, tour_configs))]

for i in sorted_configs:
    if cost_dict[i] > 1700:
        break
    print(i, ":", cost_dict[i])
