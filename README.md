# GENESYS v2.0 - European Power System Model & Transformation Optimisation
========================

GENESYS v2 is an optimisation tool for the optimisation of the transformation and operation of the European Power System. It is implemented using the CMA-ES Optimiser [(1) Nikolaus Hansen et.al.], with the implementation of libcmaes by [(2)Emmanuel Benazera]

GENESYS v2 is based on the ideas and experience from a prior research project (GENESYS v1) which aimed at the study of possible realisation of a fully renewable power system for EUMENA.

GENESYS v2 is implemented by RWTH Aachen University within a collaboration of the Institute for Power Generation and Storage Systems (PGS/ISEA) and the Institute of Power Systems and Power Economics (IAEW).

Learn more: http://www.genesys.rwth-aachen.de

# Dependencies

- libcmaes  (https://github.com/beniz/libcmaes)
- eigen3 (http://eigen.tuxfamily.org/index.php?title=Main_Page)

# References

- (1) Hansen, N. and A. Ostermeier (2001). Completely Derandomized Self-Adaptation in Evolution Strategies. Evolutionary Computation, 9(2), pp. 159-195.

- (2) libcmaes,  designed and implemented by Emmanuel Benazera with help of Nikolaus Hansen, on behalf of Inria Saclay / Research group TAO and Laboratoir de l'Accélérateur linéaire, research group Appstats, https://github.com/beniz/libcmaes

# Funding
The project GENESYS is funded by the German Ministry for Economic Affairs and Energy under the project number FKZ 0325366


# Installation Guide
Necessary library configuration for the genesys2
version: 2016-08-16
editor: pst

checkout as new general project
convert to c++ project (Executable, Linux GCC)

(Eclipse Project) Configuration 


C/C++ Build -> Settings -> GCC C++ COMPILER

Dialect:
	ISO C++11 (-std=c++0x)
Preprocessor: -D
	__cplusplus=201103L
include paths (-l)
	/usr/include/eigen3
	/usr/local/include/libcmaes
	"${workspace_loc:/genesys_2/}" (or "${workspace_loc:/${ProjName}}")
Miscellaneous
	other flags: add -fopenmp

C/C++ Build -> Settings -> GCC C++ LINKER
libraries (-l)
	cmaes
library search path (-L)
	/usr/local/lib
Miscellaneous
	linker flags: add -fopenmp
