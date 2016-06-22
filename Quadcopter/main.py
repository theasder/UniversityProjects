# -*- coding: utf-8 -*-
from visual import *
import simulation
import controller
# import simulation_advanced

class Global:

    def rotate_obj(self, obj, delta_roll, delta_pitch, delta_yaw, center):
        obj.rotate(angle = delta_yaw, axis = vector(0, 1, 0), origin = center)
        obj.rotate(angle = delta_roll, axis = vector(1, 0, 0), origin = center)
        obj.rotate(angle = delta_pitch, axis = vector(0, 0, 1), origin = center)

    def __init__(self):
        self.offset = -10


class Quadcopter(Global):

    def translate_to_state(self, x, z, y):
        delta_x = x - self.body.pos.x
        delta_y = y - self.body.pos.y
        delta_z = z - self.body.pos.z
        self.translate(delta_x, delta_y, delta_z)

    def translate(self, delta_x, delta_y, delta_z):
        self.body.pos = vector(self.body.pos.x + delta_x, self.body.pos.y + delta_y, self.body.pos.z + delta_z)

        for i in range(4):
            a_pos = self.arms[i].pos
            m_pos = self.motors[i].pos
            self.arms[i].pos = vector(a_pos.x + delta_x, a_pos.y + delta_y, a_pos.z + delta_z)
            self.motors[i].pos = vector(m_pos.x + delta_x, m_pos.y + delta_y, m_pos.z + delta_z)

    def rotate_to_state(self, roll, pitch, yaw):

        self.rotate(-self.roll, 0, 0)
        self.rotate(0, -self.pitch, 0)
        self.rotate(0, 0, -self.yaw)

        self.rotate(0, 0, yaw)
        self.rotate(0, pitch, 0)
        self.rotate(roll, 0, 0)


    def rotate(self, delta_roll, delta_pitch, delta_yaw, center = None):
        if center == None:
            center = self.body.pos

        self.rotate_obj(self.body, delta_roll, delta_pitch, delta_yaw, center)

        for i in range(4):
            self.rotate_obj(self.arms[i], delta_roll, delta_pitch, delta_yaw, center)
            self.rotate_obj(self.motors[i], delta_roll, delta_pitch, delta_yaw, center)

        self.yaw += delta_yaw
        self.roll += delta_roll
        self.pitch += delta_pitch

    def __init__(self):
        Global.__init__(self)

        self.yaw = 0
        self.pitch = 0
        self.roll = 0

        self.height = 15 + self.offset
        armlen = 10
        motor_h = 2
        rad_a = 0.5
        rad_m = 0.6

        self.body = box(pos = vector(0, self.height, 0), length = 5, height = 3, width = 5)

        self.arms = []
        self.motors = []

        for i in range(4):
            self.arms.append(cylinder(pos = self.body.pos, axis = vector(armlen, 0, 0), radius = rad_a))
            self.arms[i].rotate(angle = i * pi / 2, axis = vector(0, 1, 0), origin = self.body.pos)

            motor_p = self.body.pos + self.arms[i].axis
            motor_p[1] -= rad_a * 2

            self.motors.append(cylinder(pos = motor_p, axis = vector(0, motor_h, 0), radius = rad_m))


class Scene(Global):

    def rotate_scene_obj(self, obj, yaw, pitch, roll):
        self.rotate_obj(obj, yaw, pitch, roll, vector(0, 0, 0))

    def rotate_view(self, yaw, pitch, roll):
        self.quad.rotate(yaw, pitch, roll, vector(0, 0, 0))
        self.rotate_scene_obj(self.ground, yaw, pitch, roll)

    def __init__(self):
        Global.__init__(self)
        scene.width = 800
        scene.height = 600
        scene.title = "Симуляция квадрокоптера"
        scene.autoscale = False
        scene.center = vector(30, 5, 100)

        self.ground = box(pos = vector(0, self.offset, 0), length = 50, height = 0.1, width = 50, color = color.green)

        self.quad = Quadcopter()

        # PID coeffs after 456 experiments
        pParam = 3.71465074
        iParam = 4.31005646
        dParam = 4.92253341

        start_time = 0
        end_time = 25

        dt = 0.01

        # cntrllr = controller.controller('pid', (pParam, iParam, dParam))
        # result = simulation.result(cntrllr, start_time, end_time, dt)

        c1 = 0.76106523
        k1 = 1.02742286
        k2 = 0.48344825

        p, i, d = 3.93171837, 0.33270504, 2.57237279

        cntrllr = controller.controller('slc', (c1, k1, k2))
        result = simulation.result(cntrllr, start_time, end_time, dt, (p, i, d))

        times = result['times']


        for i in range(times):
            rate(times / (end_time / 1.3))
            self.quad.translate_to_state(*result['coords'][i])
            self.quad.rotate_to_state(*result['angles'][i])
        # while True:
        #     ev = scene.waitfor('click keydown')
        #     if ev.event != 'click':
        #         if ev.key == 'a':
        #             self.rotate_view(pi / 12, 0, 0)
        #         if ev.key == 'd':
        #             self.rotate_view(-pi / 12, 0, 0)


if __name__ == "__main__":
    Scene()