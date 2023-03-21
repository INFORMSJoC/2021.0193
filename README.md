# 2021.0193
# An Approximation Algorithm for k-Depot Split Delivery Vehicle Routing Problem
## Abstract
 A multi-depot capacitated vehicle routing problem aims to serve customers’ demands using a fleet of capacitated vehicles located in multiple depots, 
such that the total travel cost of the vehicles is minimized. We study a variant of this problem, the k-depot split delivery vehicle routing problem (or k-DSDVRP in short), for the situation where each customer’s demand can be served by more than one vehicle, and the total number of depots, denoted by k ≥ 2, is a fixed constant. This is a challenging problem with broad
applications in the logistics industry, for which no constant ratio approximation algorithm is known. We
develop a new approximation algorithm for the k-DSDVRP, ensuring an approximation ratio of (6 − 4/k)
and a polynomial running time for any fixed constant k ≥ 2. To achieve this, we propose a novel solution
framework based on a new relaxation of the problem, a cycle splitting procedure, and a vehicle assignment
procedure. To further enhance its efficiency for practical usage, we adapt the newly developed approximation
algorithm to a heuristic, which runs in polynomial time even when k is arbitrarily large. Experimental
results show that this heuristic outperforms a commercial optimization solver and a standard vehicle routing
heuristic. Moreover, our newly proposed solution framework can be applied to developing new constant ratio
approximation algorithms for several other variants of the k-DSDVRP with k ≥ 2 being a fixed constant.

## Key words
 Approximation Algorithm; Multiple Depot; Vehicle Routing Problem; Split Delivery.

## Instances and results
We have considered two classes of test instances, called Class P and Class SD, which can be found in the two folders "P set" and "SD set" of the subdirectory"data", respectively. The specific illustration of instances can be found in the file "Data Format Description.txt"  in each folder for the two classes of instances. The source files for the method APPX  is in "APPX/src". The source files for the benchmark methods, i.e., methods MIP3600/MIP28800 and method GREEDY, are provided in "MIP/src" and "GREEDY/src", respectively. The methods APPX and GREEDY are coded with C++, and the MIP is coded with Python. Guidances for implementation can be found in the files "APPX/README.pdf", "MIP/README.pdf" and "GREEDY/README.pdf", respectively. Meanwhile, the results output by these three methods are available in folders“APPX/results”, “MIP/results” and “GREEDY/results” . Each folder also contains a script to verify the correctness of the results. Note that, to run the source code for the MIP in the folder "MIP/src", a license for Gurobi should be downloaded by following the instruction in "MIP/README.pdf" in the subdirectory "MIP". Finally, the source codes of the method APPX have used a standard algorithm from the Chinese Software Development Network (CSDN) to find a minimum perfect matching in a general graph. Due to the copyright issue, one may need to download the codes in CSDN, following the instruction shown in the file "APPX/README.pdf".

## Paper Entry
Xiaofan Lai, Liang Xu, Zhou Xu, Yang Du. "An Approximation Algorithm for k-Depot Split Delivery Vehicle Routing Problem". INFORMS Journal On Computing, 2023 (to appear)

## Cite
To cite this code, please cite the paper using its DOI and the code itself, using the following DOI.\
DOI:10.1287/ijoc.2021.0193.cd

Below is the BibTex for citing this version of the code.
~~~
@article{kdepotcode,
  title={An Approximation Algorithm for k-Depot Split Delivery Vehicle Routing Problem},
  author={Xiaofan Lai and Liang Xu and Zhou Xu and Yang Du},
  publisher={{INFORMS Journal on Computing}},
  year={2023},
  doi={10.1287/ijoc.2021.0193.cd},
  note={available for download at https://github.com/INFORMSJoC/2021.0193}
}
~~~
