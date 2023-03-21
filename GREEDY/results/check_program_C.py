# coding: utf-8
import numpy as np
import os
import sys
import re


class Check_Program_C():
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


    def load_result(self):
        """
        :param m: number of vehicles in each depot
        :param n: number of customers
        :param t: number of depots
        :return:
        """
        text = open(self.result_path).read()
        infos = text.split("\n")
        path_list = []
        all_demand = []
        customer_demand_list = []
        for i in range(3, len(infos)-1):
            info = infos[i]
            path = []
            cus_demand = {}
            info_list = re.split(r"[ ]+", info)
            info_lens = len(info_list)
            for j in range(info_lens-2):
                if j != 0 and j != info_lens-3:
                    cus_demand.update({info_list[j].split('_')[0]: info_list[j].split('_')[1]})
                path.append(int(info_list[j].split('_')[0]))
            demand = info_list[info_lens-1].split(":")[1]
            all_demand.append(demand)
            path_list.append(path)
            customer_demand_list.append(cus_demand)
        return path_list, all_demand, customer_demand_list


    def check_1(self, path_list):
        """
        Check whether the start points are same as the end points
        :return:
        """
        for path in path_list:
            if(path[0] != path[len(path)-1]):
                return False
        return True


    def check_2(self, m, path_list):
        """
        Check whether the vehicle number is not more than the vehicles in depots
        :return:
        """
        depot_num = len(m)
        vehicle_num = len(path_list)
        for i in range(depot_num):
            count = 0
            for j in range(vehicle_num):
                if i == path_list[j][0]:
                    count += 1
            if count > m[i]:
                return False
        return True


    def check_3(self, q, all_demand):
        """
        Check whether the demand serviced is not more than the capacity of vehicle.
        :return:
        """
        for service in all_demand:
            if int(service) > q:
                return False
        return True


    def check_4(self, t, demands, customer_demand_list):
        """
        Check whether demand of each customer is satisfied.
        :return:
        """
        n = len(demands)
        demand_num = [0] * n
        for cus_dem in customer_demand_list:
            for key, value in cus_dem.items():
                demand_num[int(key)-t] += int(value)
        # print(demand_num)
        for i in range(n):
            if demand_num[i] != demands[i]:
                return False

        return True


    def check_5(self, n, t, path_list):
        """
        Check whether each customer appears in the optimal path at once.
        :return:
        """
        cus_set = set()
        for path in path_list:
            for p in path:
                cus_set.add(int(p))
        count = 0
        for cus in cus_set:
            if cus >= t:
                count += 1
        if count == n:
            return True
        else:
            return False


if __name__ == '__main__':
    result_base = os.getcwd() +'/RESULTS_MDVRP/'       # base path of results
    data_base = os.getcwd() + '/instance/'              # base path of instances
    paths = ["k=2/", "k=4/", "k=6/"]
    # paths = ['k=6/']
    for path in paths:
        r_path = result_base + path
        d_path = data_base + path
        file_list = os.listdir(r_path)
        for f in file_list:
            file_name = f.split(".")[0]
            result_path = os.path.join(r_path, f)
            file_path = os.path.join(d_path, f)
            check = Check_Program_C(file_path, result_path)
            if f.startswith('p'):
                m, n, t, q, demands, distance = check.load_data_P()
            elif f.startswith('SD'):
                m, n, t, q, demands, distance = check.load_data_SD()
            else:
                continue
            path_list, all_demand, customer_demand_list = check.load_result()
            C1 = check.check_1(path_list)
            C2 = check.check_2(m, path_list)
            C3 = check.check_3(q, all_demand)
            C4 = check.check_4(t, demands, customer_demand_list)
            C5 = check.check_5(n, t, path_list)

            if C1 & C2 & C3 & C4 & C5:
                print(path + f, 'Success')
            else:
                print(C1, C2, C3, C4, C5)
                print(path + f, 'Check Again!!')



