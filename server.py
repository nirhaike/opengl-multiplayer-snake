#######################################################
#
#   Snake Game Server
#
#   Written by Nir H.
#
#######################################################
#
# Usage: server.py <num-of-players> <snake-start-size>
#   <max-wait-time> <amount_apples>
# where the waiting time is in seconds.
#
#######################################################
#
# When the server runs, it waits for the players to
# join. if the number of players is greater than 1,
# the game begins, otherwise the server is closed.
# The number of players cannot reach above the
# given argument.
#
#######################################################
#
#   TODO List:
#   1. Validation Check (check for illegal moves and
#      behavior).
#
#######################################################
import sys
import time
import random
import threading
import socket
import traceback


PORT = 1234

GRID_WIDTH = 80
GRID_HEIGHT = 60

class GameServer(object):
    """ This class represents the game server. """
    def __init__(self, max_players, wait_time, snake_size, amount_apples):
        # game info
        self.max_players = max_players
        self.wait_time = wait_time
        self.snake_size = snake_size
        self.amount_apples = amount_apples
        # game objects
        self.players = []
        self.apples = []
        # server handling
        self.lock = threading.Lock()
        self.sock = None

    def start(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.bind(("0.0.0.0", PORT))
        self.sock.settimeout(0.5)
        self.sock.listen(5)
        # player index
        index = 0
        # get players
        self.debug("Server started running..")
        self.wait_room()
        # check if the game should begin
        if len(self.players) < 2:
            # game shutdown message
            self.broadcast_message("\x02")
            for player in players:
                try:
                    player.sock.close()
                except:
                    pass
            self.sock.close()
            return
        # add the apples
        for i in range(self.amount_apples):
            self.apples.append(self.generate_point())
            move_apple(i, generate_spot=False)
        # start the game
        self.broadcast_message("\x03")
        self.debug("Game started!")


    def wait_room(self):
        """ This function waits for players to join the server """
        while self.wait_time > 0 and len(self.players) < self.max_players:
            try:
                client, addr = self.sock.accept()
                # get the name from the client
                name = client.recv(1024)
                # send the game details and rules
                client.send(self.get_game_info())
                # generate a spawn point
                x, y = self.generate_point()
                # notify other players
                self.broadcast_message("\x01" + player.get_data())
                # tell the new player about the older ones
                client.send(self.get_players_info())
                # add the player to the game
                player = Player(index, name, x, y, client)
                self.add_player(player)
                # print that the player joined the server
                self.debug(client[0] + " (" + name + ") has joined.")
                # handle the player's socket output stream
                handler = threading.Thread(player_handler, args=(self, player))
                handler.start()
                # increment the player's index
                index += 1
            except socket.timeout:
                pass
            except:
                traceback.print_exc()
            self.wait_time -= 0.5

    def debug(self, message):
        """
            A thread-safe function for printing a debug message.
        """
        self.lock.acquire()
        print message
        self.lock.release()

    def get_game_info(self):
        data = chr(self.max_players) \
                + chr(self.wait_time) \
                + chr(self.snake_size)
        return data

    def get_players_info(self):
        data = chr(len(self.players))
        for player in self.players:
            data += player.get_data()
        return data

    def add_player(self, player):
        """
            This function adds a player to the game.
        """
        self.lock.acquire()
        self.players.append(player)
        self.lock.release()

    def remove_player(self, player):
        """
            This function removes a player from the game.
        """
        # try to close the player's socket
        player.sock.close()
        # remove the player
        self.lock.acquire()
        self.players.remove(player)
        self.lock.release()
        # tell other players that the player has left
        server.broadcast_message("\x00" + chr(player.get_index()))

    def generate_point(self, min_distance=5):
        """
            This function generates a point in the grid,
            and makes sure that the point is far from
            every player.
        """
        found = False
        x = 0
        y = 0
        while not found:
            # generate the point
            x = random.randint(0, GRID_WIDTH-1)
            y = random.randint(0, GRID_HEIGHT-1)
            # make sure it's fine
            found = True
            self.lock.acquire()
            for player in self.players:
                # check the manhattan distance
                dst = abs(player.get_x()-x) + abs(player.get_y()-y)
                if dst < min_distance:
                    found = False
            for apple in self.apples:
                # check the manhattan distance
                dst = abs(apple[0]-x) + abs(apple[1]-y)
                if dst < min_distance:
                    found = False
            self.lock.release()
        return (x, y)

    def broadcast_message(self, msg, sender=None):
        self.lock.acquire()
        for player in self.players:
            if player != sender:
                player.send(msg)
        self.lock.release()

    def move_apple(self, index, generate_spot=True):
        # generate a new spot if requested
        if generate_spot:
            self.apples[index] = generate_point()
        # send the coordinates
        x = self.apples[index][0]
        y = self.apples[index][1]
        self.broadcast_message("\x12"+chr(index)+chr(x)+chr(y))

    def close(self):
        self.sock.close()


class Player(object):
    """
        This class represents a game player.
    """
    def __init__(self, index, name, x, y, sock):
        self.index = index
        self.name = name
        self.x = x
        self.y = y
        self.sock = sock

    def move(self, dx, dy):
        self.x += dx
        self.y += dy

    def get_data(self):
        # create a binary representation for the data
        data = chr(self.index)
        # add name as a pascal string
        data += chr(len(self.name)) + self.name
        # add the x and y coordinates
        data += chr(self.x) + chr(self.y)
        return data

    def get_x(self):
        return self.x

    def get_y(self):
        return self.y

    def get_index(self):
        return self.index

    def send(self, data):
        self.sock.send(data)

    def recv(self):
        return self.sock.recv(1024)

    def close(self):
        self.sock.close()


def player_handler(server, player):
    running = True
    while running:
        try:
            msg = player.recv()
        except:
            server.remove_player(player)
        # handle the player's command
        command = ord(msg[0])
        if command == 0x00: # quit game
            server.remove_player(player)
        elif command == 0x01: # move
            dx = ord(msg[1])
            dy = ord(msg[2])
            # validation check
            if dx in [0, 1] and dy in [0, 1]:
                player.move(dx, dy)
                broadcast = "\x10" + chr(player.get_index()) + msg[1] + msg[2]
                server.broadcast_message(broadcast, sender=player)
        elif command == 0x02: # apple eaten
            ind = ord(msg[1])
            # make other players know that the apple was eaten
            # and the snake was enlarged
            broadcast = "\x11" + chr(player.get_index()) + msg[1]
            server.broadcast_message(broadcast, sender=player)
            # move the apple
            server.move_apple(ind)
        elif command == 0x03: # defeated
            server.remove_player(player)

# validate commandline arguments
all_integers = True
for i in range(1, len(sys.argv)):
    try:
        int(sys.argv[i])
    except:
        all_integers = False
if len(sys.argv) < 5 or not all_integers:
    print "Usage: server.py <num-of-players> <snake-start-size> <max-wait-time> <amount_apples>"

# get the commandline arguments
max_players = int(sys.argv[1])
snake_size = int(sys.argv[2])
wait_time = int(sys.argv[3])
amount_apples = int(sys.argv[4])

server = GameServer(max_players, wait_time, snake_size, amount_apples)
server.start()