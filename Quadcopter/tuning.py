import simulation
import controller
import numpy as np
import statsmodels.api as sm


def tune():
    attempts = 10000

    min_cost = 1e10
    best_params = -1

    for i in range(attempts):
        costs, params = minimize()
        if costs[-1:] < min_cost:
            min_cost = costs[-1:]
            best_params = params
        print(best_params, costs[-1:])
    return best_params

def minimize():
    params = np.random.random(3) * 5

    alpha = 0.03

    max_iterations = 500

    average_length = 3
    costs = np.zeros(max_iterations)

    for iteration in range(max_iterations):
        costs[iteration] = mean_value(cost, tuple(params), average_length)

        num_costs = 50

        if iteration > num_costs + 5:
            recent_costs = costs[-num_costs:]

            y = np.array([recent_costs]).transpose()
            X = np.column_stack([np.ones(num_costs), np.arange(1, num_costs + 1)])

            mod = sm.OLS(y, X)
            res = mod.fit()

            koeffs = res.conf_int(0.01)
            # lr = np.linalg.lstsq(x, y)
            # koeffs = lr[0].transpose()[0]
            # degrees_of_freedom = num_costs - 3
            # sum_of_qr_residuals = float(lr[1][0])
            # std_err = (sum_of_qr_residuals / degrees_of_freedom) ** 0.5

            upper = max(koeffs[1, :])
            lower = min(koeffs[1, :])

            if 0 < upper and 0 > lower:
                break

    if iteration > 100:
        alpha = 0.001
        average_length = 8
    elif iteration > 200:
        average_length = 15
        alpha = 0.0005

    grad = mean_value(gradient, tuple(params), average_length)

    params = params - alpha * grad
    return (costs[:iteration + 1], params)

def mean_value(func, input, n):
    value = func(input)

    for i in range(1, n):
        value += func(input)
    return value / n

def gradient(params):

    delta = 0.001
    grad = np.zeros(len(params))

    for i in range(len(params)):
        var = np.zeros(len(params))
        var[i] = 1

        left_cost = cost(tuple(np.array(params) + delta * var))
        right_cost = cost(tuple(np.array(params) - delta * var))

        grad[i] = (left_cost - right_cost) / (2 * delta)

    return grad


# def cost(*params):
#     control = controller.controller('slc', *params)
#     data = simulation.result(control, 0, 1, 0.05)
#     errors = np.sum(data['angles'].transpose() ** 2, axis = 0) ** 0.5
#     J = sum(errors ** 2) * data['step']
#     return J

def cost(*params):
    c1 = 1.74070476
    k1 = 1.23444907
    k2 = 1.6318585

    control = controller.controller('slc', (c1, k1, k2))
    data = simulation.result(control, 0, 1, 0.05, *params)
    errors = np.sum(data['coords'][:, :2].transpose() ** 2, axis = 0) ** 0.5
    J = sum(errors ** 2) * data['step']
    return J

print(tune())