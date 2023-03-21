# coding: utf-8
import numpy as np
import os
import sys


class Check_Program():
    def __init__(self, instance_path, result_path):
        self.case_path = instance_path
        self.result_path = result_path

    @staticmethod
    def euclidean_distance(loc1, loc2):
        return np.sqrt(np.sum((loc2 - loc1) ** 2))

    def get_distance(self, locates):
        """
        get the distance of locates
        :param locates:
        :return:
        """
        distance = []
        n = len(locates)
        for i in range(n):
            for j in range(n):
                dis = self.euclidean_distance(locates[i], locates[j])
                distance.append(dis)
        return distance

    def load_data_SD(self):
        """
        get instance of SD instances
        :return:
        """
        text = open(self.case_path).read()
        info = text.split("\n")
        basic_info = list(filter(None, info[0].split(" ")))
        q = int(basic_info[1])  # maximum load of vehicle  f_path.split("/")[1]
        n = int(basic_info[0])  # number of customers
        m = []
        for i in range(2, len(basic_info)):
            m.append(int(basic_info[i]))
        demands = list(filter(None, info[1].split(" ")))
        demands = [int(d) for d in demands]
        t = len(m)  # number of depot
        locates = []
        # location of customers
        for idx in range(t + 2, len(info)):
            line = info[idx].split(" ")
            line = list(filter(None, line))
            if len(line) == 0:
                break
            locate = np.array([float(line[0]), float(line[1])])
            locates.append(locate)
        # location of depots
        for idx in range(2, t + 2):
            line = info[idx].split(" ")
            line = list(filter(None, line))
            locate = np.array([float(line[0]), float(line[1])])
            locates.append(locate)
        distance = self.get_distance(locates)
        return m, n, t, q, demands, distance

    def load_data_P(self):
        """
        get instance of P instances
        :return:
        """
        text = open(self.case_path).read()
        info = text.split("\n")
        basic_info = info[0].split(" ")
        m = int(basic_info[1])  # number of vehicles in each depot
        n = int(basic_info[2])  # number of customers
        t = int(basic_info[3])  # number of depots
        q = int(info[1].split(" ")[1])  # maximum load of vehicle
        demands = []
        locates = []
        for idx in range(t + 1, len(info)):
            line = info[idx].split(" ")
            line = list(filter(None, line))
            if len(line) == 0:
                break
            demand = int(line[4])
            locate = np.array([float(line[1]), float(line[2])])
            if len(demands) < n:
                demands.append(demand)
            locates.append(locate)
        distance = self.get_distance(locates)
        m = [m for i in range(t)]
        return m, n, t, q, demands, distance

    def process_data(self, n, t, distance):
        depot = ["depot_{}".format(str(i + 1)) for i in range(t)]
        customer = ["customer_{}".format(str(i + 1)) for i in range(n)]
        vertex = customer + depot
        arc = [(i, j) for i in vertex for j in vertex]
        tour_cost = {}
        for (i, j), dis in zip(arc, distance):
            tour_cost.update({(i, j): dis})
        return tour_cost

    def load_result(self, m, n, t):
        """
        :param m: number of vehicles in each depot
        :param n: number of customers
        :param t: number of depots
        :return:
        obj: Objective value
        """
        text = open(self.result_path).read()
        infos = text.split("\n")
        X = []
        Y = []
        Z = []
        # service, remain_capacity ={}
        if infos[1].split(':')[1] == 'inf':
            return None, None, None, None, None, None
        else:
            obj = float(infos[1].split(':')[1])
        customer_service = [0 for x in range(n)]
        vehicle_service = [[0 for x in range(m[y])] for y in range(t)]
        for info in infos:
            if info.startswith('X'):
                x = info.split(':')
                e = x[0][2:-1].split(',')
                X.append((e[2], e[3]))
                Y.append((int((e[0].split('_')[1])), int((e[1].split('_')[1])), e[2], e[3]))
                o = float(x[1])
                Z.append(o)
            if info.startswith('service'):
                s = info.split(':')
                vehicle = int(s[0][8:-1].split(',')[1].split('_')[1])
                customer = int(s[0][8:-1].split(',')[2].split('_')[1])
                depot = int(s[0][8:-1].split(',')[0].split('_')[1])
                vehicle_service[depot-1][vehicle-1] = vehicle_service[depot-1][vehicle-1] + float(s[1])
                customer_service[customer-1] = customer_service[customer - 1] + float(s[1])
        X_num = {}
        for (i, j), p in zip(X, Z):
            X_num.update({(i, j): p})
        return obj, X, Y, customer_service, vehicle_service, X_num


    def check_1(self, X, Y, obj, X_num):
        """
        Check obj whether satisfy tour cost
        :return:
        """
        distance = 0
        for key in X:
            distance = distance + X_num[key] * Y[key]
        error = abs(distance - obj)/obj
        if error <= 0.03:
            return True
        else:
            print(error)
            return False

    def check_2(self, t, m, Y):
        """
        Check whether the in and out degree of depot is less than or equal to 1
        :return:
        """
        for i in range(1, t+1):
            for j in range(1, m[i-1]+1):
                degree_out = 0
                degree_in = 0
                for y in Y:
                    if (y[0] == i) & (y[1] == j) & (y[2] == 'depot_'+str(i)):
                        degree_out += 1
                    if (y[0] == i) & (y[1] == j) & (y[3] == 'depot_'+str(i)):
                        degree_in += 1
                if degree_out > 1 | degree_in > 1:
                    return False
        return True

    def check_3(self, t, m, X):
        """
        Check whether the number of vehicles in each depot is less than or equal to m
        :return:
        """
        for i in range(1, t+1):
            degree_out = 0
            degree_in = 0
            for x in X:
                if x[0] == 'depot_'+str(i):
                    # print(x)
                    degree_out = degree_out + 1
                if x[1] == 'depot_'+str(i):
                    # print(x)
                    degree_in = degree_in + 1
            if degree_out > m[i-1] | degree_in > m[i-1]:
                # print(i, degree_out, degree_in)
                return False
        return True

    def check_4(self, n, X):
        """
        Check if each customer is served at least once
        :return:
        """
        s = set()
        for x in X:
            if x[0].startswith('customer'):
                s.add(x[0])
            if x[1].startswith('customer'):
                s.add(x[1])
            if len(s) == n:
                return True
        return False

    def check_5(self, demands, customer_service, n):
        """
        Check whether the demand of each customer is satisfied
        :return:
        """
        for i in range(n):
            if(abs(customer_service[i]-demands[i])/customer_service[i]>=0.001):
                return False
        return True

    def check_6(self, vehicle_service, t, q):
        """
        Check whether the capacity of each vehicle is satisfied
        :return:
        """
        for i in range(t):
            for j in range(len(vehicle_service[i])):
                if (vehicle_service[i][j] > q):
                    if ((vehicle_service[i][j] - q)/vehicle_service[i][j] >= 0.001):
                        return False
        return True

    def check_7(self, customer_service, X, n):
        """
        Check if there is a vehicle service j that does not pass through customer
        :return:
        """
        is_service = [0 for x in range(n)]
        for x in X:
            if x[1].startswith('customer'):
                j = int(x[1].split('_')[1]) - 1
                is_service[j] = is_service[j]+1
        for k in range(n):
            if customer_service[k] != 0:
                if is_service[k] == 0:
                    return False
        return True


if __name__ == '__main__':
    # Absolute path of results
    result_paths = ["/Users/Documents/Daily/2021.0193/MIP/results/P set/28800s",
                    "/Users/Documents/Daily/2021.0193/MIP/results/SD set/k=2/28800s"]
    # Absolute path of instances
    data_paths = ["/Users/Documents/Daily/2021.0193/data/P set",
                  "/Users/Documents/Daily/2021.0193/data/SD set/k=2"]
    for i in range(len(data_paths)):
        r_path = result_paths[i]
        d_path = data_paths[i]
        file_list = os.listdir(r_path)
        file_list.remove('Data Description.txt')
        for f in file_list:
            file_name = f.split(".")[0]
            result_path = os.path.join(r_path, f)
            file_path = os.path.join(d_path, f)
            check = Check_Program(file_path, result_path)
            if f.startswith('p'):
                m, n, t, q, demands, distance = check.load_data_P()
            elif f.startswith('SD'):
                m, n, t, q, demands, distance = check.load_data_SD()
            else:
                continue
            obj, X, Y, customer_service, vehicle_service, X_num = check.load_result(m, n, t)
            if obj is None:
                continue
            tour_cost = check.process_data(n, t, distance)
            C1 = check.check_1(X, tour_cost, obj, X_num)
            C2 = check.check_2(t, m, Y)
            C3 = check.check_3(t, m, X)
            C4 = check.check_4(n, X)
            C5 = check.check_5(demands, customer_service, n)
            C6 = check.check_6(vehicle_service, t, q)
            C7 = check.check_7(customer_service, X, n)

            if C1 & C2 & C3 & C4 & C5 & C6 & C7:
                print(f, 'Success')
            else:
                print(C1, C2, C3, C4, C5, C6, C7)
                print(f, 'Check Again!!')



