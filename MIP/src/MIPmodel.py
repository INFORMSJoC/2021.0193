# -*- coding: utf-8 -*-
import os

import numpy as np
from gurobipy import *
import json
import time

class VRPModel():

    def __init__(self, file_path, title, output_path=None):
        self.path = file_path
        self.title = title
        self.output_path = output_path
        m, n, t, q, demands, distance = self._load_data()
        vrp_dict = self.process_data(m, n, t, demands, distance)

        self.route = vrp_dict["route"]
        self.service = vrp_dict["service"]
        self.remain_capacity = vrp_dict["remain_capacity"]
        self.depot = vrp_dict["depot"]
        self.tour_cost = vrp_dict["tour_cost"]
        self.vehicle = vrp_dict["vehicle"]
        self.distinct_arc = vrp_dict["distinct_arc"]
        self.customer_demand = vrp_dict["customer_demand"]
        self.customer = vrp_dict["customer"]
        self.vertex = vrp_dict["vertex"]
        self.depot_vehicle = vrp_dict["depot_vehicle"]
        self.num_vehicle = vrp_dict["num_vehicle"]
        self.vertexs = vrp_dict["vertexs"]
        self.q = q
        self.M = 1000

    @staticmethod
    def get_distinct_arc(arc, depot):
        distinct_arc = []
        for i, j in arc:
            if i != j:  # any arc must have different starting point and end point
                distinct_arc.append((i, j))
        return distinct_arc

    @staticmethod
    def get_route(vehicle, distinct_arc, depot):
        route = []
        for k, l in vehicle:
            for i, j in distinct_arc:
                route.append((k, l, i, j))
        return route

    def process_data(self, m, n, t, demands, distance):
        depot = ["depot_{}".format(str(i + 1)) for i in range(t)]
        customer = ["customer_{}".format(str(i + 1)) for i in range(n)]
        vertex = customer + depot
        vehicle = [("depot_{}".format(str(k + 1)), "vehicle_{}".format(str(l + 1))) for k in range(t) for l in
                   range(m[k])]
        arc = [(i, j) for i in vertex for j in vertex]
        distinct_arc = self.get_distinct_arc(arc, depot)
        num_vehicle = tupledict([(i, num) for i, num in zip(depot, m)])
        tour_cost = tupledict([((i, j), dis) for (i, j), dis in zip(arc, distance)])
        customer_demand = tupledict([(i, j) for i, j in zip(customer, demands)])

        service = tuplelist([(k, l, i) for k, l in vehicle for i in vertex])
        remain_capacity = tuplelist([(k, l, i) for k, l in vehicle for i in vertex])
        route = tuplelist(self.get_route(vehicle, distinct_arc, depot))

        depot_vehicle = []
        for k in range(t):
            depot_name = depot[k]
            vehicle_name = ["vehicle_{}".format(str(i + 1)) for i in range(m[k])]
            depot_vehicle.append([depot_name, vehicle_name])
        depot_vehicle = tupledict(depot_vehicle)

        vertexs = []
        for i in vertex:
            other_vertexs = []
            for j in vertex:
                if i != j:
                    if (i in depot) and (j in depot):
                        continue
                    else:
                        other_vertexs.append(j)
            # other_vertexs = [j for j in vertex if i != j]
            vertexs.append([i, other_vertexs])
        vertexs = tupledict(vertexs)

        # 字典
        vrp_dict = {
            "depot": depot,
            "customer": customer,
            "vertex": vertex,
            "vehicle": vehicle,
            "arc": arc,
            "distinct_arc": distinct_arc,
            "num_vehicle": num_vehicle,
            "tour_cost": tour_cost,
            "customer_demand": customer_demand,
            "service": service,
            "remain_capacity": remain_capacity,
            "depot_vehicle": depot_vehicle,
            "vertexs": vertexs,
            "route": route}
        return vrp_dict

    @staticmethod
    def euclidean_distance(loc1, loc2):
        return np.sqrt(np.sum((loc2 - loc1) ** 2))

    def get_distance(self, locates):
        distance = []
        n = len(locates)
        for i in range(n):
            for j in range(n):
                dis = self.euclidean_distance(locates[i], locates[j])
                distance.append(dis)
        return distance

  
    def load_data(self):
        text = open(self.path).read()
        info = text.split("\n")
        basic_info = list(filter(None, info[0].split(" ")))
        q = int(basic_info[1])  # maximum load of vehicle  f_path.split("/")[1]
        n = int(basic_info[0])  # number of customers
        m = []
        for i in range(2, len(basic_info)):
            m.append(int(basic_info[i]))
        # demands =
        demands = list(filter(None, info[1].split(" ")))
        demands = [int(d) for d in demands]
        n = len(demands)  # number of customers
        t = len(m)  # number of depot
        # locates
        locates = []
        # locations for customers
        for idx in range(t + 2, len(info)):
            line = info[idx].split(" ")
            line = list(filter(None, line))
            if len(line) == 0:
                break
            locate = np.array([float(line[0]), float(line[1])])
            locates.append(locate)
        #  locations for depots
        for idx in range(2, t + 2):
            line = info[idx].split(" ")
            line = list(filter(None, line))
            locate = np.array([float(line[0]), float(line[1])])
            locates.append(locate)
        distance = self.get_distance(locates)
        return m, n, t, q, demands, distance

    def _load_data(self):
        text = open(self.path).read()
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

    def buildModel(self):
        model = Model("MDSDVRP")
        ########################### variables  #############################
        X = model.addVars(self.route, name="X", vtype=GRB.BINARY)  # binary variable
        delta = model.addVars(self.service, name="service")  # continuous variable
        mu = model.addVars(self.remain_capacity, name="remain_capacity")  # continuous variable

        ########################### objective ##############################
        # minimize the total tour cost.
        model.setObjective(
            quicksum(X[k, l, i, j] * self.tour_cost[i, j] for k, l, i, j in self.route),  #objective (EC.16)
            GRB.MINIMIZE)

        ########################## constrains ##############################

        # Constraint (EC.17)
        model.addConstrs(
            (quicksum(X[k, l, k, j] for _, _, _, j in self.route.select(k, l, k, '*'))  
             <= 1
             for k, l in self.vehicle), "c3(#32)")

        # Constraint (EC.18)
        model.addConstr(
            (quicksum(X[k, l, i, j] for k, l in self.vehicle
                      for i in self.depot if i != k
                      for _, _, _, j in self.route.select(k, l, i, '*') if j not in self.depot)
             == 0), "c10(#39)")

        # Constraint (EC.19)
        model.addConstrs(
            (quicksum(X[k, l, i, j] for _, _, _, j in self.route.select(k, l, i, '*'))  
             -
             quicksum(X[k, l, j, i] for _, _, j, _ in self.route.select(k, l, '*', i))  
             == 0
             for k, l in self.vehicle for i in self.vertex), "c4(#33)")  


        #Constraint (EC.20)
        model.addConstrs((quicksum(delta[k, l, i] for k, l in self.vehicle)
                          == self.customer_demand[i]
                          for i in self.customer), "c1(#30)")


        # Constraint (EC.21)
        model.addConstrs((quicksum(delta[k, l, i] for i in self.customer)
                          <= self.q
                          for k, l in self.vehicle), "c2(#31)")
       
      
        # Constraint (EC.22)
        model.addConstrs((delta[k, l, i]
                          >= 0
                          for k, l in self.vehicle for i in self.customer), "c9_1(#38)")
       
        model.addConstrs((delta[k, l, i]
                          <= self.q * quicksum(X[k, l, i, j] for _, _, _, j in self.route.select(k, l, i, '*'))
                          for k, l in self.vehicle for i in self.customer), "c9_2(#38)")

        model.addConstrs((delta[k, l, i]
                          <= self.customer_demand[i] * quicksum(
            X[k, l, i, j] for _, _, _, j in self.route.select(k, l, i, '*'))
  
                          for k, l in self.vehicle for i in self.customer), "c9_3(#38)")
       

        # Constraint (EC.23) 
        model.addConstrs((self.q * (1 - X[k, l, i, j]) + mu[k, l, i]
                          >= mu[k, l, j] + delta[k, l, i]  # self.customer + [k]
                          for k, l in self.vehicle for i in self.customer for _, _, _, j in
                          self.route.select(k, l, i, '*')), "c8(#37)")  

        # Constraint (EC.24)
        model.addConstrs((mu[k, l, i]
                          == [0, self.q]
                          for k, l in self.vehicle for i in self.customer), "c11(#40)")

        return model

    def get_lp(self, output_path):
        model = self.buildModel()
        model.update()
        output_path_ = os.getcwd()+output_path+"newConstrains.lp"
        print(output_path_)
        model.write(output_path_)

    def run(self, time_limit, output_path):
        start_time = time.time()
        model = self.buildModel()
        model.setParam('TimeLimit', time_limit * 60)
        model.optimize()
        end_time = time.time()
        sec = end_time - start_time
        print(f"the process is finished! {sec}s")
        if not os.path.exists(os.getcwd() + output_path):
            os.makedirs(os.getcwd() + output_path)
        output_path = os.getcwd() + output_path + self.title
        file = open(output_path+".txt", mode='w')
        print(f'Objective value:{model.objVal}')
        file.write(f'Spend time:{sec}s')
        file.write("\n")
        file.write(f'Objective value:{model.objVal}')
        file.write("\n")
        result = json.loads(model.getJSONSolution())
        file.write(f'Objective bound:{result["SolutionInfo"]["ObjBound"]}')
        file.write("\n")
        print("Solution value:")
        try:
            for v in model.getVars():
                if "Rgc" in v.varname:
                    break
                if v.x > 0:
                    file.write(f"{v.varname}:{v.x}")
                    file.write("\n")
        except:
            file.write("the problem can't be solved in this scenario!")

        file.close()
        return model.getJSONSolution()


# %%
file_path = ["/home/li/CLionProjects/attach/data/k_2"]  # ,"data/SET_1_k_6",

time_limit = [60]


def run(time_limit):
    for f_path in file_path:
        file_list = os.listdir(f_path)
        for t_limit in time_limit:
            for f in file_list:
                file_name = f.split(".")[0]
                file = os.path.join(f_path, f)
                vrpModel = VRPModel(file, file_name)
                dir_list = f_path.split("/")
                output_path = "/time_limit_" + str(t_limit) + "/" + f_path.split("/")[len(dir_list)-1] + "/"
                vrpModel.run(t_limit, output_path)
                vrpModel.get_lp(output_path)


run(time_limit)

