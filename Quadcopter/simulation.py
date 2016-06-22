# -*- coding: utf-8 -*-

from rotation import *
from controller import backstepping_control, control_to_motors
import matplotlib.pyplot as plt

def thrust(motors, k):
    return matrix([[0], [0], [k * sum(motors)]])

def torques(motors, L, b, k):
    return matrix([
        [L * k * (motors[0] - motors[2])[0]],
        [L * k * (motors[1] - motors[3])[0]],
        [b * (motors[0] - motors[1] + motors[2] - motors[3])[0]]
    ])

def angle_vel2angular_vel(angle_velocities, angles):
    phi = angles[0]
    theta = angles[1]
    W = matrix([[1, 0, -sin(theta)],
                [0, cos(phi), cos(theta) * sin(phi)],
                [0, -sin(phi), cos(theta) * cos(phi)]])
    angular_velocity = W * angle_velocities

    return angular_velocity

def angular_vel2angle_vel(angular_velocity, angles):
    phi = angles[0]
    theta = angles[1]

    W = matrix([[1, 0, -sin(theta)],
                [0, cos(phi), cos(theta) * sin(phi)],
                [0, -sin(phi), cos(theta) * cos(phi)]])
    angle_velocities = linalg.inv(W) * angular_velocity
    return angle_velocities

def drag_force(velocity):
    K_t = matrix(diag([0.1, 0.1, 0.15]))
    return K_t * velocity

def acceleration(motors, angles, vels, m, g, k):
    gravity = array([[0], [0], [-g]])
    R = rotation(angles)

    T = R * (thrust(motors, k))
    T = matrix(diag([-1, 1, 1])) * T
    Fd = - drag_force(vels)
    a = gravity + 1 / m * T + Fd
    return a

def drag_moments(angle_velocities):
    K_r = matrix(diag([0.1, 0.1, 0.15]))
    return K_r * angle_velocities

def angular_accel(motors, angle_velocities, angular_velocity, I, L, b, k):
    tau = torques(motors, L, b, k)

    I_r = 6e-5
    Omega_r = - abs(motors[0]) ** 0.5 + abs(motors[1]) ** 0.5 - abs(motors[2]) ** 0.5 + abs(motors[3]) ** 0.5

    return linalg.inv(I) * (tau - drag_moments(angle_velocities) -
                            cross(angular_velocity.transpose(), array([0, 0, I_r * Omega_r])).transpose() -
                            cross(angular_velocity.transpose(), (I * angular_velocity).transpose()).transpose())

def result(controller, start_time, end_time, dt, coord_controller_params):
# def result(start_time, end_time, dt):

    times = arange(start_time, end_time, dt)

    g = 9.81
    m = 0.65
    L = 0.23
    J_r = 6e-5

    k = 3.13e-5
    b = 7.5e-7
    I_xx, I_yy, I_zz = 7.5e-3, 7.5e-3, 1.3e-2
    I = diag([I_xx, I_yy, I_zz])
    kd = 0.25


    a_1 = (I_yy - I_zz) / I_xx
    a_2 = J_r / I_xx
    a_3 = (I_zz - I_xx) / I_yy
    a_4 = J_r / I_yy
    a_5 = (I_xx - I_yy) / I_zz
    b_1 = L / I_xx
    b_2 = L / I_yy
    b_3 = L / I_zz

    N = len(times)


    coords_out = zeros((N, 3))
    angles_out = zeros((N, 3))
    angle_velocities_out = zeros((N, 3))


    state = {'dt': dt, 'I': I, 'k': k, 'L': L, 'b': b, 'm': m, 'g': g,
             'a1': a_1, 'a2': a_2, 'a3': a_3, 'a4': a_4, 'a5': a_5,
             'b1': b_1, 'b2': b_2, 'b3': b_3}

    coords = matrix([[0], [0], [10]])
    velocity = zeros((3, 1))
    velocity_d = zeros((3, 1))
    angles = zeros((3, 1))
    angle_accelerations = zeros((3, 1))
    angle_accelerations_d = zeros((3, 1))

    deviation = pi * 0.8
    deviation2 = pi * 0.6
    angle_velocities = zeros((3, 1))
    # angles = (2 * deviation2 - 1) * random.rand(3, 1)
    # angle_velocities = (2 * deviation - 1) * random.rand(3, 1)


    # angle_velocities = array([[3.26189557],
    #    [0.72438711],
    #    [1.8440186]])

    angle_velocities_d = zeros((3, 1))
    # angle_velocities_d = array([[0], [-pi / 3], [0]])
    angles_d = array([[0], [0], [0]])
    counter = 0
    epsilon = 0.001

    coords_d = matrix([[0], [0], [15]])
    motors = zeros((4, 1))

    z_dot_d = 0
    z_d = 15
    z_dotdot_d = 0
    attempts = 0
    u1 = 48500
    u2 = -1
    u3 = -1
    offset = 100
    T = 150 * 2
    accelerate = 2.5

    for ind in range(N):

        # (motors, state) = controller(state, angle_velocities, angle_velocities_d, angles_d,
        #                                 float(coords[2][0]), height_d)
        Omega_r = - abs(motors[0]) ** 0.5 + abs(motors[1]) ** 0.5 - abs(motors[2]) ** 0.5 + abs(motors[3]) ** 0.5

        # motors = controller(state, Omega_r, angles, angle_velocities, coords, coords_d, velocity,
        #                                     velocity_d, z_dotdot_d, angles_d, angle_velocities_d, angle_accelerations_d)
        # z = float(coords[2][0])
        # z_dot = float(velocity[2][0])

        # if offset < ind < T / accelerate + offset:
        #     motors = control_to_motors(u1, u2 * accelerate, 0, 0)
        # else:


        if ind < T:
            angle_velocities_d = array([[0], [-pi / 2.5], [0]])
        else:
            angle_velocities_d = zeros((3, 1))

        if T + offset < ind < T + T / accelerate + offset:
            motors = control_to_motors(u1, 0, u3 * accelerate, 0)
        else:
            motors = backstepping_control(state, Omega_r, angles, angle_velocities, angles_d, angle_velocities_d,
                                      angle_accelerations_d, coords, coords_d, velocity, velocity_d, z_dotdot_d)

        angular_velocity = angle_vel2angular_vel(angle_velocities, angles)
        accel = acceleration(motors, angles, velocity, m, g, k)

        angular_acceleration = angular_accel(motors, angle_velocities, angular_velocity, I, L, b, k)
        angular_velocity = angular_velocity + dt * angular_acceleration
        angle_velocities = angular_vel2angle_vel(angular_velocity, angles)
        angles = simplify(angles + dt * angle_velocities)
        velocity = velocity + dt * accel
        coords = coords + dt * velocity

        phi_dot = float(angle_velocities[0][0])
        theta_dot = float(angle_velocities[1][0])
        psi_dot = float(angle_velocities[2][0])
        Omega_r = - abs(motors[0]) ** 0.5 + abs(motors[1]) ** 0.5 - abs(motors[2]) ** 0.5 + abs(motors[3]) ** 0.5

        U_2 = k * (motors[3] - motors[1])
        U_3 = k * (motors[0] - motors[2])
        U_4 = b * (motors[0] - motors[1] + motors[2] - motors[3])

        angle_accelerations[0][0] = a_1 * theta_dot * psi_dot - a_2 * theta_dot * Omega_r + b_1 * U_2
        angle_accelerations[1][0] = a_3 * phi_dot * psi_dot + a_4 * phi_dot * Omega_r + b_2 * U_3
        angle_accelerations[2][0] = a_5 * phi_dot * theta_dot + b_3 * U_4

        coords_out[ind] = coords.transpose()
        angles_out[ind] = angles.transpose()
        angle_velocities_out[ind] = angle_velocities.transpose()

        counter += 1

    return {'coords': coords_out, 'angles': angles_out, 'times': N, 'step': dt}

#
# plt.plot(times, angles_out[:, 0], 'b-', times, angles_out[:, 1], 'g-', times, angles_out[:, 2], 'r-')
#
# plt.title(u'Изменение углов с использованием скользящего контроля')
# plt.xlabel(u'Время, с')
# plt.ylabel(u'Угол, рад')
# plt.show()

# plt.plot(times, angle_velocities_out[:, 0], 'b-', times, angle_velocities_out[:, 1],
#          'g-', times, angle_velocities_out[:, 2], 'r-')
#
# plt.title(u'Изменение угловых скоростей с использованием скользящего контроля')
# plt.xlabel(u'Время, с')
# plt.ylabel(u'Угловая скорость, рад/c')
# plt.show()
# angles_d_x_y = coord_controller(state, motors, float(angles[2][0]), coords, coords_d,
#                                 velocity, velocity_d, coord_controller_params)
# angles_d = array([[angles_d_x_y[0]], [angles_d_x_y[1]], [0]])
#
# print(angles_d)

# print(float(angle_velocities[1][0]), float(angle_velocities_d[1][0]))
#
# if abs(float(angle_velocities[0][0])) < epsilon and \
#                 abs(float(angle_velocities[1][0])) < epsilon:
#     angle_velocities_d = array([[0], [-pi], [0]])

# if counter % int(500 * (random.random() + 0.1)) == 0 and attempts < 2:
#     angle_velocities = (2 * deviation - 1) * random.rand(3, 1)
#     attempts += 1
# if ind > T + T / accelerate + offset:
#     angles[0][0] = 0
#     angles[1][0] = 0
# print(float(angles[2][0]))
# print(float(angles[0][0]), float(angles[1][0]), float(angles[2][0]))