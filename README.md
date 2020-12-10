# 2D Conduction Heat Transfer utilizing Parallel Processing

In this project, we implement a conduction heat transfer simulation on a 2D plate, utilizing [parallel processing](https://en.wikipedia.org/wiki/Parallel_computing) with the use of [MPI](https://en.wikipedia.org/wiki/Message_Passing_Interface), [OpenMP](https://en.wikipedia.org/wiki/OpenMP) and [CUDA](https://en.wikipedia.org/wiki/CUDA).

## Initial software
Some initial reference software that performs the required simulation can be found [here](https://computing.llnl.gov/tutorials/mpi/samples/C/mpi_heat2D.c).<br>
However, this software underperforms and our goal was to make it better. So, we wrote our own version, from scratch.

## Software design and implementation

As a simplification, we assume every subplate is constantly heated in its central area, instead of only the center of the whole plate.

### MPI
We split the computations accross multiple compute nodes, so each node is assigned a part of the computations. If fact, we split the 2D plate in subplates and each node computes conduction heat transfer on its own plate. Then, every node exchanges temperature information in their plate's boundaries with is neighbors, which are other nodes whose plates are adjacent to the node's.<br>

* The number of nodes utilized must be a perfect square in order to have a perfectly balanced split.<br>
  So, N<img src="https://render.githubusercontent.com/render/math?math=\in">n<sup>2</sup>, n=1,2,3,...<br>
  e.g. If we have 50 nodes, then N must be in {1,4,9,15,25,36,49}. 
* Since the plates are squared, the number of cells that consist the plate must be a perfect square, which must be chosen so that it can be evenly split accross the N nodes.<br>
  So, C = LCM(all possible number of nodes)<img src="https://render.githubusercontent.com/render/math?math={}^2 \cdot">n<img src="https://render.githubusercontent.com/render/math?math={}^2">, n=1,2,3,...<br>
  e.g. if N<img src="https://render.githubusercontent.com/render/math?math=\leq"> 25 then LCM(1,4,9,16,25)=420 so C must be of the form 420<img src="https://render.githubusercontent.com/render/math?math={}^2 \cdot"> n<img src="https://render.githubusercontent.com/render/math?math={}^2">, n=1,2,3,...

To execute the software:
1. Use a shell to navigate to the [MPI](/sources/MPI) directory.
2. Edit the [mpd.hosts](/sources/MPI/mpd.hosts) file and enter the IPs of the available nodes in the network.
3. Edit the [machines](/sources/MPI/machines) file and for each available note in the network enter the node name and the number of CPUs it has.
4. Build the software using the [Makefile](/sources/MPI/Makefile) using the following shell command:
```bash
make
```
5. Execute the software using the following shell command:
```bash
sh exeg_prog.sh [number of processes] [number of cells] [number of epochs]
```

### OpenMP
This part is an extension to the MPI part, as it combines MPI with OpenMP.<br>
In every computation node we utilize OpenMP to split the computations within the node accross 4 threads, each one communicating with a single neighbor node (North, South, East, West)<br>

1. Use a shell to navigate to the [OpenMP](/sources/OpenMP) directory.
2. Edit the [mpd.hosts](/sources/OpenMP/mpd.hosts) file and enter the IPs of the available nodes in the network.
3. Edit the [machines](/sources/OpenMP/machines) file and for each available note in the network enter the node name and the number of CPUs it has.
4. Build the software using the [Makefile](/sources/OpenMP/Makefile) using the following shell command:
```bash
make
```
5. Execute the software with the following shell command:
```bash
sh exeg_prog.sh [number of processes] [number of cells] [number of epochs]
```

### CUDA
This part is independent from the 2 previous parts, as it does not utilize MPI at all.<br>
In a single compute node with a modern Nvidia GPU, we follow the GPGPU paradigm and utilize CUDA to perform the computations within the GPU.<br>
On the same node, we also utilize OpenMP as a reference for comparisons.<br>

1. Use a shell to navigate to the [CUDA](/sources/CUDA) directory.
2. Build the software using the [CUDA](/sources/CUDA/Makefile) using the following shell command:
```bash
make
```
3. Execute the software with the following shell command:
```bash
sh exeg_prog.sh [number of cells] [number of epochs]
```

## Results

The [measurements](/results/measurements.xls) are in as well as the [plots](/results/plots) that visualize the performance improvement are located in the [results](/results) directory.<br>
In these measurements we compare our software with the underperforming initial software in order to show the performance improvements we achieved.
We also compare the 3 different versions of our own software against each other to show the improvements we can achieve while utilizing different technologies.
