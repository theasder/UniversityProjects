from numpy import *

def simplify(angle):
    rounded = angle /  pi
    return (rounded - trunc(rounded)) * pi
    # return angle

def rotation(angles):
    psi = float(angles[2][0])
    theta = float(angles[1][0])
    phi = float(angles[0][0])

    # R = matrix([[
    #     cos(phi) * cos(psi) - cos(theta) * sin(phi) * sin(psi),
    #     - cos(psi) * sin(phi) - cos(phi) * cos(theta) * sin(psi),
    #     sin(theta) * sin(psi)
    # ],[
    #     cos(theta) * cos(psi) * sin(phi) + cos(phi) * sin(psi),
    #     cos(phi) * cos(theta) * cos(psi) - sin(phi) * sin(psi),
    #     - cos(psi) * sin(theta)
    # ],[
    #     sin(theta) * sin(phi),
    #     cos(phi) * sin(theta),
    #     cos(theta)
    # ]])

    # R = matrix([[
    #     cos(phi) * cos(theta),
    #     cos(psi) * sin(theta) * sin(phi),
    #     cos(psi) * sin(theta) * cos(phi) + sin(psi) * sin(phi)
    # ], [
    #     sin(psi) * cos(theta),
    #     sin(psi) * sin(theta) * sin(phi) + cos(theta) * cos(phi),
    #     sin(psi) * sin(theta) * cos(phi) - cos(psi) * sin(theta)
    # ], [
    #     - sin(theta),
    #     cos(theta) * sin(phi),
    #     cos(theta) * cos(phi)
    # ]])

    # psi = float(angles[2][0])
    # theta = float(angles[1][0])
    # phi = float(angles[0][0])
    #
    R = matrix([[
        cos(phi) * cos(theta),
        cos(phi) * sin(theta) * sin(psi) - cos(psi) * sin(phi),
        sin(phi) * sin(psi) + cos(phi) * cos(psi) * sin(theta)
    ], [
        cos(theta) * sin(phi),
        cos(phi) * cos(psi) + sin(phi) * sin(theta) * sin(psi),
        cos(psi) * sin(phi) * sin(theta) - cos(phi) * sin(psi)
    ], [
        - sin(theta),
        cos(theta) * sin(psi),
        cos(theta) * cos(psi)
    ]])

    return R

