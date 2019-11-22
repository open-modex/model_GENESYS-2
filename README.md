<img src="/images/banner.png" />
<br/>

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
Genesys v2.0 is running solely on Linux based machines (see dependencies above). To be able to run the programm stick to the instructions below:

Necessary library configuration for Genesys v2.0<br/>
version: 2016-08-16

**All instructions refer to the usage of Eclipse Oxigen as development environment**

**STEP 0: Setup OS (Centos7)
- Install epel repository : "sudo yum install -y epel-release
- Install eigen3 lib : "sudo yum install -y eigen3-devel
- Clone libcmaes :"git clone https://github.com/beniz/libcmaes.git
	Follow setup instructions in README.MD:
	./autogen.sh
	echo "#define CMAES_EXPORT" > cmaes_export.h
	./configure
	make
	(the compilation takes some time!)


**STEP 1: Download Code and import it into Eclipse**
- (1.1) Checkout the code as new general project
- (1.2) Convert the project to a c++ project (Executable, Linux GCC)
- (1.3) Check if all required libraries are installed (see dependencies above)

**Step 2: Change Eclipse-Project-Configurations**<br/>

<ins>GO TO:</ins> File → Properties → C/C++ Build
- (2.1) set Configuration to '[All configurations]' 

<table><tr><td>
    <img src="/images/all_configs.PNG" />
</td></tr></table>
<br/>
<br/>

<ins>GO TO:</ins> File → Properties → C/C++ Build → Settings → GCC C++ Compiler

- (2.2) Dialect: ISO C++11 (-std=c++0x)

<table><tr><td>
    <img src="/images/dialect.PNG" />
</td></tr></table>
<br/>
<br/>

- (2.3) Preprocessor: -D __cplusplus=201103L

<table><tr><td>
    <img src="/images/preprocessor.PNG" />
</td></tr></table>
<br/>
<br/>

- (2.4) include paths (-l):	-> /usr/include/eigen3<br/>
				-> /usr/local/include/libcmaes<br/>
				-> "${workspace_loc:/genesys_2/}" (or "${workspace_loc:/${ProjName}}")
				
<table><tr><td>
    <img src="/images/includes.PNG" />
</td></tr></table>
<br/>
<br/>
				
- (2.5) Miscellaneous - other flags: add -fopenmp

<table><tr><td>
    <img src="/images/miscellaneous_comp.PNG" />
</td></tr></table>
<br/>
<br/>

*GO TO: C/C++ Build -> Settings -> GCC C++ LINKER*

- (1) libraries (-l): 			cmaes
- (2) library search path (-L): 	/usr/local/lib

<table><tr><td>
    <img src="/images/libraries.PNG" />
</td></tr></table>
<br/>
<br/>

- (3) Miscellaneous - linker flags: 	add -fopenmp

<table><tr><td>
    <img src="/images/miscellaneous_link.PNG" />
</td></tr></table>
<br/>
<br/>
