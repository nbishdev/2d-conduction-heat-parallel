# 2D Conduction Heat Transfer utilizing Parallel Processing

In this project, we implement a conduction heat transfer simulation on a 2D plate, utilizing [parallel processing](https://en.wikipedia.org/wiki/Parallel_computing) with the use of [MPI](https://en.wikipedia.org/wiki/Message_Passing_Interface), [OpenMP](https://en.wikipedia.org/wiki/OpenMP) and [CUDA](https://en.wikipedia.org/wiki/CUDA).

## MPI
We split the computations accross multiple compute nodes, so each node is assigned a part of the computations. If fact, we split the 2D plate in subplates and each node computes conduction heat transfer on its own plate. Then, every node exchanges temperature information in their plate's boundaries with is neighbors, which are other nodes whose plates are adjacent to the node's.<br>
As a simplification, we assume every subplate is constantly heated in its central area, instead of only the center of the whole plate.<br>
* The number of nodes utilized must be a perfect square in order to have a perfectly balanced split. So, <img src="https://render.githubusercontent.com/render/math?math=\text{N} \in {\text{n}^2\text{, n=1,2,3,...}}">.<br>
  e.g. If we have 50 nodes, then N must be in {1,4,9,15,25,36,49}. 
* Since the plates are squared, the number of cells that consist the plate must be a perfect square, which must be chosen so that it can be evenly split accross the N nodes. So, C=LCM(all possible number of nodes)<img src="https://render.githubusercontent.com/render/math?math={}^2 \cdot \text{n}^2\text{, n=1,2,3,...}">. E.g. if <img src="https://render.githubusercontent.com/render/math?math=\text{N} \leq 25"> then LCM(1,4,9,16,25)=420 so C must be of the form <img src="https://render.githubusercontent.com/render/math?math=420^2 \cdot \text{n}^2\text{, n=1,2,3,...}">.
To execute the code, use a shell to navigate to the [MPI](/sources/MPI) directory and execute the following command:
```bash
sh exeg_prog.sh [number of processes] [number of cells] [number of epochs]
```
