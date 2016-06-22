from rotation import *

def error2motors(state, error, total):
    e1 = error[0]
    e2 = error[1]
    e3 = error[2]
    Ix = state['I'][0][0]
    Iy = state['I'][1][1]
    Iz = state['I'][2][2]
    k = state['k']
    L = state['L']
    b = state['b']
    motors = zeros((4, 1))
    motors[0] = total / 4 - (2 * b * e1 * Ix + e3 * Iz * k * L) / (4 * b * k * L)
    motors[1] = total / 4 + e3 * Iz / (4 * b) - (e2 * Iy) / (2 * k * L)
    motors[2] = total / 4 - (-2 * b * e1 * Ix + e3 * Iz * k * L) / (4 * b * k * L)
    motors[3] = total / 4 + e3 * Iz / (4 * b) + (e2 * Iy) / (2 * k * L)

    # print(motors)
    return motors

def pid_controller(state, angle_velocities, angle_velocities_d, angles_d, Kd, Kp, Ki, height, height_d): #, height, angles):
    if 'integral' not in state.keys():
        state['integral'] = zeros((3, 1))
        state['integral2'] = zeros((3, 1))

    if max(abs(state['integral2'])) > 0.01:
        state['integral2'] = zeros((3, 1))

    k = 1000
    error_z = k * (height_d - height)
    total = state['m'] * state['g'] / state['k'] / (cos(state['integral'][0]) * cos(state['integral'][1])) + error_z

    error = Kd * (angle_velocities - angle_velocities_d) + Kp * state['integral'] - Ki * state['integral2']


    motors = error2motors(state, error, total)
    state['integral'] = simplify(state['integral'] + state['dt'] * angle_velocities - angles_d)
    state['integral2'] = state['integral2'] + state['dt'] * state['integral']

    return (motors, state)

def control_to_motors(u1, u2, u3, u4):
    K_f = 3.13e-5
    K_m = 7.5e-7
    k1 = 1 / (4 * K_f)
    k2 = 2 * k1
    k3 = 1 / (4 * K_m)

    motors = zeros((4, 1))

    # print(u1)
    # u4 = 0
    motors[0] = abs(u1 + k2 * u3 + k3 * u4)
    motors[1] = abs(u1 - k2 * u2 - k3 * u4)
    motors[2] = abs(u1 - k2 * u3 + k3 * u4)
    motors[3] = abs(u1 + k2 * u2 - k3 * u4)

    # print(motors)
    return motors

def coord_controller(state, motors, psi, coords, coords_d, velocity, velocity_d, params):
    state['integral_x'] = 0
    state['integral_y'] = 0

    p, i, d = params

    accel_x_d = p * float(coords_d[0][0] - coords[0][0]) + i * state['integral_x'] \
              + d * float(velocity_d[0][0] - velocity[0][0])

    state['integral_x'] += float(coords_d[0][0] - coords[0][0])

    accel_y_d = p * float(coords_d[1][0] - coords[1][0]) + i * state['integral_y'] \
              + d * float(velocity_d[1][0] - velocity[1][0])

    state['integral_y'] += float(coords_d[0][0] - coords[0][0])

    u1 = sum(motors)
    phi_d = (state['m'] / u1) * (- accel_x_d * sin(psi) + accel_y_d * cos(psi))
    theta_d = (state['m'] / u1) * (- accel_x_d * cos(psi) + accel_y_d * sin(psi))

    # print(phi_d, theta_d, accel_x_d, accel_y_d)

    return (phi_d, theta_d)

def slight_control(c1, k1, k2, state, Omega_r, angles, angle_velocities, coords, coords_d, velocity, velocity_d,
                   z_dotdot_d, angles_d, angle_velocities_d, angle_accelerations_d):

    err = angles_d - angles
    # print(err)

    err_dot = angle_velocities_d - angle_velocities

    phi = float(angles[0][0])
    phi_dot = float(angle_velocities[0][0])
    theta_dot = float(angle_velocities[1][0])
    theta = float(angles[1][0])
    psi_dot = float(angle_velocities[2][0])

    s = c1 * err + err_dot

    U = k1 * sign(s) + k2 * s + c1 * (angle_velocities_d - angle_velocities) + angle_accelerations_d

    U[0][0] = (U[0][0] + state['a2'] * theta_dot * Omega_r - state['a1'] * theta_dot * psi_dot) / state['b1']
    U[1][0] = (U[1][0] - state['a4'] * phi_dot * Omega_r - state['a3'] * phi_dot * psi_dot) / state['b2']
    U[2][0] = (U[2][0] - state['a5'] * phi_dot * theta_dot) / state['b3']

    if 'integral' not in state.keys():
        state['integral'] = zeros((3, 1))
        state['integral2'] = zeros((3, 1))
        state['integral_z'] = 0

    if max(abs(state['integral2'])) > 0.01:
        state['integral2'] = zeros((3, 1))

    k_p = 1000
    k_d = 200
    k_i = 0
    error_z = k_p * float(coords_d[2][0] - coords[2][0]) + \
              k_d * float(velocity_d[2][0] - velocity[2][0]) + \
              k_i * float(state['integral_z'])

    k_z = 0.24
    U_z = k_z * state['m'] * state['g'] / state['k'] / (cos(state['integral'][0]) * cos(state['integral'][1])) + error_z

    state['integral'] = state['integral'] + state['dt'] * (angle_velocities_d - angle_velocities)
    state['integral2'] = state['integral2'] + state['dt'] * state['integral']
    state['integral_z'] = state['integral_z'] + state['dt'] * float(coords_d[2][0] - coords[2][0])
    # err = z_d - z
    # err_dot = z_dot_d - z_dot
    #
    # s = c1 * err + err_dot
    #
    # U_z = (k1 * sign(s) + k2 * s + c1 * (z_dot - z_dot_d) - state['g'] - z_dotdot_d) * state['m'] / (cos(phi) * cos(theta))
    # U_z = abs(U_z)

    # k = 100000
    # err = k * (z - z_d)
    # U_z = state['m'] * state['g'] / (cos(phi) * cos(theta)) + err
    return control_to_motors(float(U_z), float(U[0][0]), float(U[1][0]), float(U[2][0]))


def backstepping_control(state, Omega_r, angles, angle_velocities, angles_d, angle_velocities_d,
                         angle_accelerations_d, coords, coords_d, velocity, velocity_d, z_dotdot_d):
    c1 = 6.11
    c2 = 7.96
    c3 = 6.11
    c4 = 7.96
    c5 = 6.11
    c6 = 7.96
    # c7 = 6.11
    # c8 = 7.96
    # K_f = 3.13e-5
    z1 = float(angles_d[0][0] - angles[0][0])
    z2 = float(angle_velocities[0][0] - angle_velocities_d[0][0] - c1 * z1)

    u2 = float(- c2 * z2 + z1 - state['a1'] * angle_velocities[1][0] * angle_velocities[2][0] + \
               Omega_r * state['a2'] * angle_velocities[1][0] + angle_accelerations_d[0][0] + \
               c1 * angle_velocities_d[0][0] - c1 * angle_velocities[0][0]) / state['b1']

    z3 = float(angles_d[1][0] - angles[1][0])
    z4 = float(angle_velocities[1][0] - angle_velocities_d[1][0] - c3 * z3)

    u3 = float(- c4 * z4 + z3 - state['a3'] * angle_velocities[0][0] * angle_velocities[2][0] - \
               Omega_r * state['a4'] * angle_velocities[0][0] + angle_accelerations_d[1][0] + \
               c3 * angle_velocities_d[1][0] - c3 * angle_velocities[1][0]) / state['b2']


    z5 = float(angles_d[2][0] - angles[2][0])
    z6 = float(angle_velocities[2][0] - angle_velocities_d[2][0] - c5 * z5)

    u4 = float(- c6 * z6 + z5 - state['a5'] * angle_velocities[0][0] * angle_velocities[2][0] + \
               angle_accelerations_d[2][0] + \
               c5 * angle_velocities_d[2][0] - c5 * angle_velocities[2][0]) / state['b3']

    # z7 = float(z_d - z)
    # z8 = float(z_dot- z_dot_d - c7 * z7)
    #
    # u1 =  (1 / (4 * K_f)) * (state['m'] / float(cos(angles[0][0] * cos(angles[1][0])))) * \
    #       (z7 - state['g'] + z_dotdot_d + c7 * z_dot - c7 * z_dot + c8 * z8)

    if 'integral' not in state.keys():
        state['integral'] = zeros((3, 1))
        state['integral2'] = zeros((3, 1))

    if max(abs(state['integral2'])) > 0.01:
        state['integral2'] = zeros((3, 1))

    k_p = 2000
    k_i = 300
    error_z = k_p * float(coords_d[2][0] - coords[2][0]) + k_i * float(velocity_d[2][0] - velocity[2][0])
    k_z = 0.24
    u1 = k_z * state['m'] * state['g'] / state['k'] / (cos(state['integral'][0]) * cos(state['integral'][1])) + error_z

    state['integral'] = simplify(state['integral'] + state['dt'] * angle_velocities - angles_d)
    state['integral2'] = state['integral2'] + state['dt'] * state['integral']

    return control_to_motors(u1, u2, u3, u4)

def controller(type, params):
    if type == 'pid':
        Kp = params[0]
        Ki = params[1]
        Kd = params[2]

        return lambda state, angle_velocities, angle_velocities_d, angles_d, height, height_d: \
            pid_controller(state, angle_velocities, angle_velocities_d, angles_d, Kd, Kp, Ki, height, height_d)
    elif type == 'slc':
        c1 = params[0]
        k1 = params[1]
        k2 = params[2]

        return lambda state, Omega_r, angles, angle_velocities, z, z_d, z_dot, z_dot_d, z_dotdot_d, \
                      angles_d, angle_velocities_d, angle_accelerations_d: \
            slight_control(c1, k1, k2, state, Omega_r, angles, angle_velocities, z, z_d, z_dot, z_dot_d, z_dotdot_d,
                           angles_d, angle_velocities_d, angle_accelerations_d)
