import random
import math

sign = lambda x: int(math.copysign(1, x))

def draw_field(field):
    n = len(field)
    print("-" * n * 4 + "-")

    for i in range(n):
        print("| ", end='')
        for j in range(n):
            print(str(field[i][j]), end=" | ")
        print('\n' + "-" * n * 4 + "-")
    print('')

class Cheese:
    def __init__(self, field):
        n = len(field)
        self.x = random.randint(0, n - 1)
        self.y = random.randint(0, n - 1)

        while field[self.x][self.y] != 0:
            self.x = random.randint(0, n - 1)
            self.y = random.randint(0, n - 1)

        field[self.x][self.y] = 1

# transforming reward matrix to Q-matrix
class RtoQ:
    def __init__(self, R):
        self.R = R
        self.Q = {}
        for state in self.R.keys():
            self.Q[state] = [0, 0, 0, 0]

        self.states = len(self.R)
        self.train()

    def get_random_state(self):
        return list(self.R.keys())[random.randint(0, self.states - 1)]

    def get_next_state(self, state):
        actions = [i for i in range(len(self.R[state])) if self.R[state][i] >= 0]

        next_states = []
        for action in actions:
            next_state = list(state)
            if action == 0:
                next_state[3] += 1
            elif action == 1:
                next_state[3] -= 1
            elif action == 2:
                next_state[2] -= 1
            elif action == 3:
                next_state[2] += 1
            next_state = tuple(next_state)
            next_states.append(next_state)

        dead_end = True
        for state in next_states:
            if state in self.R.keys():
                dead_end = False
                break

        if dead_end:
            return None, None

        while True:
            action = random.randint(0, len(next_states) - 1)
            if next_states[action] in self.R.keys():
                break

        return action, next_states[action]

    def train(self, gamma = 0.6, alpha = 0.8, episode = 100):
        Q, R = self.Q, self.R
        for i in range(episode):
            state = self.get_random_state()
            countdown = 10000
            while countdown:
                action, ss = self.get_next_state(state)
                if action is None:
                    break
                Q[state][action] += alpha * (R[state][action] + gamma * max(Q[ss]) - Q[state][action])
                state = ss
                countdown -= 1
        self.Q = Q

R = {}
class Mouse:
    def train_move(self, field, cheese, cat):
        n = len(field)
        action = ""
        x = self.x
        y = self.y

        field[self.x][self.y] = 0
        # in case if mouse in corner
        if (self.x == 0 and self.y == 0) or (self.x == n - 1 and self.y == n - 1) \
                or (self.x == 0 and self.y == n - 1) or (self.x == n - 1 and self.y == 0):
            choice = random.randint(0, 1)
            if choice == 0:
                if self.x == 0:
                    self.x += 1
                    action = 'right'
                else:
                    self.x -= 1
                    action = 'left'
            else:
                if self.y == 0:
                    self.y += 1
                    action = 'up'
                else:
                    self.y -= 1
                    action = 'down'

        # in case if mouse on the side
        elif self.x == 0 or self.x == n - 1 or self.y == 0 or self.y == n - 1:
            choice = random.randint(0, 2)
            if choice == 0:
                if self.x == 0 or self.x == n - 1:
                    self.y += 1
                    action = 'up'
                else:
                    self.x += 1
            elif choice == 1:
                if self.x == 0 or self.x == n - 1:
                    self.y -= 1
                    action = 'down'
                else:
                    self.x -= 1
                    action = 'left'
            elif choice == 2:
                if self.x == 0:
                    self.x += 1
                    action = 'right'
                elif self.x == n - 1:
                    self.x -= 1
                    action = 'left'

        # otherwise move anywhere
        else:
            choice = random.randint(0, 3)
            if choice == 0:
                self.x -= 1
                action = 'left'
            elif choice == 1:
                self.x += 1
                action = 'right'
            elif choice == 2:
                self.y -= 1
                action = 'down'
            elif choice == 3:
                self.y += 1
                action = 'up'

        if cat.x == self.x and cat.y == self.y:
            field[self.x][self.y] = 3
        else:
            field[self.x][self.y] = 2

        draw_field(field)
        choice = -1
        if action == 'up':
            choice = 0
        elif action == 'down':
            choice = 1
        elif action == 'left':
            choice = 2
        elif action == 'right':
            choice = 3

        # constructing R matrix

        if (cat.x, cat.y, x, y, cheese.x, cheese.y) not in R.keys():
            R[(cat.x, cat.y, x, y, cheese.x, cheese.y)] = [0] * 4

        if y == 0:
            R[(cat.x, cat.y, x, y, cheese.x, cheese.y)][1] = -1
        if x == 0:
            R[(cat.x, cat.y, x, y, cheese.x, cheese.y)][2] = -1
        if y == n - 1:
            R[(cat.x, cat.y, x, y, cheese.x, cheese.y)][0] = -1
        if x == n - 1:
            R[(cat.x, cat.y, x, y, cheese.x, cheese.y)][3] = -1

        if cat.x == self.x and cat.y == self.y:
            R[(cat.x, cat.y, x, y, cheese.x, cheese.y)][choice] = -100
        elif self.x == cheese.x and self.y == cheese.y:
            R[(cat.x, cat.y, x, y, cheese.x, cheese.y)][choice] = 100
        else:
            R[(cat.x, cat.y, x, y, cheese.x, cheese.y)][choice] = 0


    def test_move(self, field, cheese, cat, Q):

        field[self.x][self.y] = 0
        n = len(field)
        state = (cat.x, cat.y, self.x, self.y, cheese.x, cheese.y)
        if state not in Q.keys():
            self.train_move(field, cheese, cat)
        else:
            q = [(Q[state][i], i) for i in range(len(Q[state]))]
            q.sort(reverse=True)

            field[self.x][self.y] = 0

            for value, choice in q:
                if choice == 0 and self.y != n - 1:
                    self.y += 1
                    break
                elif choice == 1 and self.y != 0:
                    self.y -= 1
                    break
                elif choice == 2 and self.x != 0:
                    self.x -= 1
                    break
                elif choice == 3 and self.x != n - 1:
                    self.x += 1
                    break

            # in case if objects overlap each other we should put cat on mouse, mouse on cheese

            if self.x == cheese.x and self.y == cheese.y:
                field[self.x][self.y] = 2
            elif self.x == cat.x and self.y == cat.y:
                field[self.x][self.y] = 3
            else:
                field[self.x][self.y] = 2

    def __init__(self, field):
        n = len(field)
        self.x = random.randint(0, n - 1)
        self.y = random.randint(0, n - 1)
        while field[self.x][self.y] != 0:
            self.x = random.randint(0, n - 1)
            self.y = random.randint(0, n - 1)

        field[self.x][self.y] = 2

class Cat:
    def move(self, mouse, cheese, field):
        delta_x = mouse.x - self.x
        delta_y = mouse.y - self.y

        field[self.x][self.y] = 0

        s1, s2 = sign(delta_y), sign(delta_x)

        rule_for_y = ((abs(delta_y) > 0 and self.y + s1 != cheese.y) or (self.x + s2 == cheese.x and self.y == cheese.y)) \
                     and self.y + s1 >= 0 and self.y + s1 <= n - 1
        rule_for_x = ((abs(delta_x) > 0 and self.x + s1 != cheese.x) or (self.y + s1 == cheese.y and self.x == cheese.x)) \
                     and self.x + s2 >= 0 and self.x + s2 <= n - 1

        # cat is getting closer to mouse
        if delta_y == 0 and delta_x == 0:
            pass
        elif abs(delta_y) == 1 and self.x == mouse.x and self.y + s1 >= 0 and self.y + s1 <= n - 1:
            self.y += s1
        elif abs(delta_x) == 1 and self.y == mouse.y and self.x + s2 >= 0 and self.x + s2 <= n - 1:
            self.x += s2
        elif rule_for_x and rule_for_y:
            choice = random.randint(0, 1)
            if choice:
                self.y += s1
            else:
                self.x += s2
        elif rule_for_y:
            self.y += s1
        elif rule_for_x:
            self.x += s2

        field[self.x][self.y] = 3

    def __init__(self, field):
        n = len(field)
        self.x = random.randint(0, n - 1)
        self.y = random.randint(0, n - 1)
        while field[self.x][self.y] != 0:
            self.x = random.randint(0, n - 1)
            self.y = random.randint(0, n - 1)

        field[self.x][self.y] = 3

class Main:
    def __init__(self, n, iterations, option):
        field = [[0] * n for i in range(n)]

        # creating instances of objects: 3 for cat, 2 for mouse, 1 for cheese
        cheese = Cheese(field)
        cat = Cat(field)
        mouse = Mouse(field)

        draw_field(field)

        name = 'q.txt'
        Q = {}

        if option == 'test':
            Q = self.read_file(name)

        for it in range(iterations):
            if option == 'train':
                mouse.train_move(field, cheese, cat)
            elif option == 'test':
                mouse.test_move(field, cheese, cat, Q)
            draw_field(field)

            cat.move(mouse, cheese, field)
            draw_field(field)

            if mouse.x == cat.x and mouse.y == cat.y:
                mouse = Mouse(field)
                draw_field(field)
                
            if mouse.x == cheese.x and mouse.y == cheese.y:
                cheese = Cheese(field)
                draw_field(field)
                
            if cat.x == cheese.x and cat.y == cheese.y:
                cheese = Cheese(field)
                draw_field(field)

        if option == 'train':
            Q = RtoQ(R).Q
            self.write_file(name, Q)

    def read_file(self, name):
        f = open(name, 'r')
        content = f.read()
        f.close()
        matrix = {}
        for line in content.split('\n'):
            arr = [float(it) for it in line.split(' ')[:-1]]
            k = tuple(arr[0:6])
            ans = arr[6:]
            matrix[k] = ans
        return matrix

    def write_file(self, name, dict):
        s = ""

        for k in dict.keys():
            for t in k:
                s += str(t) + " "
            for i in dict[k]:
                s += str(i) + " "
            s += "\n"
        f = open(name, 'w')
        f.write(s)
        f.close()

n = 5
# iterations = 100
# option = 'test'

option = input('Select an option (train/test):\n')
iterations = int(input('Select amount of iterations for algorithm:\n'))
Main(n, iterations, option)