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

**STEP 0: Only for Windows users: Install a virtual box with Centos7**<br/>
- (0.1) Download/ Install Oracle VM VirtualBox for Windows: ```VirtualBox-6.0.14-133895-Win```  https://download.virtualbox.org/virtualbox/6.0.14/ <br/>
- (0.2) Download a Centos7 distribution eg: ```CentOS-7-x86_64-DVD-1908.iso``` 
http://ftp.rz.uni-frankfurt.de/pub/mirrors/centos/7.7.1908/isos/x86_64/<br/> 
- (0.3) Add the distribution to the virtual box clicking on 'New' (Neu) and setting the Version to Red Hat (64-Bit) <br/>
- (0.4) Follow further instructions to finally set up the machine. In the end it should look similar as shown below in the screenshot<br/>
<table><tr><td>
    <img src="/images/virtual box.PNG" />
</td></tr></table>
<br/>
<br/>


**STEP 1: Install libraries/dependencies/development software in Centos7** <br/>
- (1.1) Download/install eclipse oxigen for C/C++ development: https://www.eclipse.org/downloads/download.php?file=/technology/epp/downloads/release/2019-09/R/eclipse-cpp-2019-09-R-linux-gtk-x86_64.tar.gz
- (1.2) Install epel repository : ``` sudo yum install -y epel-release ``` <br/>
- (1.3) Install eigen3 lib : ```sudo yum install -y eigen3-devel``` <br/>
- (1.4) Clone libcmaes : ```git clone https://github.com/beniz/libcmaes.git``` <br/>
	Follow setup instructions in README.MD: <br/>
	```./autogen.sh``` <br/>
	```echo "#define CMAES_EXPORT" > cmaes_export.h``` <br/>
	```./configure``` <br/>
	```make``` (the compilation takes some time!) -> agree to install in standard directory <br/>
	```sudo make install``` <br/>
	add libraries to shared library path: ``` ldconfig /usr/local/lib ```
	
	helpful linux commands within the installation:<br/>
	```chmod -R 777 file```	(grant recursive access to file for all users and groups)


**STEP 2: Download Genesys-Code and import it into Eclipse**
- (2.1) Clone repository from GitHub: ```git clone https://github.com/open-modex/model_GENESYS-2.git```
- (2.2) Checkout the submodule genesys2 as new general project in Eclipse via the Git-Symbol in the upper right corner
- (2.3) Convert the project to a c++ project (Executable, Linux GCC) by right clicking on the project in the project explorer

**Step 3: Set Eclipse-Project-Configurations**<br/>

<ins>GO TO:</ins> File → Properties → C/C++ Build
- (3.1) set Configuration to '[All configurations]' 

<table><tr><td>
    <img src="/images/all_configs.PNG" />
</td></tr></table>
<br/>
<br/>

<ins>GO TO:</ins> File → Properties → C/C++ Build → Settings → GCC C++ Compiler
- (3.2) Dialect: ISO C++11 (-std=c++0x)

<table><tr><td>
    <img src="/images/dialect.PNG" />
</td></tr></table>
<br/>
<br/>

- (3.3) Preprocessor(-D): __cplusplus=201103L

<table><tr><td>
    <img src="/images/preprocessor.PNG" />
</td></tr></table>
<br/>
<br/>

- (3.4) include paths (-l):<br/>
/usr/include/eigen3<br/>
/usr/local/include/libcmaes<br/>
"${workspace_loc:/genesys_2/}" (or "${workspace_loc:/${ProjName}}")
				
<table><tr><td>
    <img src="/images/includes.PNG" />
</td></tr></table>
<br/>
<br/>
				
- (2.5) Miscellaneous - other flags: add '-fopenmp'

<table><tr><td>
    <img src="/images/miscellaneous_comp.PNG" />
</td></tr></table>
<br/>
<br/>

*GO TO: C/C++ Build → Settings → GCC C++ LINKER*
libraries (-l): cmaes
library search path (-L): /usr/local/lib

<table><tr><td>
    <img src="/images/libraries.PNG" />
</td></tr></table>
<br/>
<br/>

- (3) Miscellaneous - linker flags: add '-fopenmp'

<table><tr><td>
    <img src="/images/miscellaneous_link.PNG" />
</td></tr></table>
<br/>
<br/>


**Step 4: Change Eclipse-Project-Configurations**<br/>

- (4.1) Open window *Run → Run Configurations* and create 2 new C/C++ Applications in the panel on the left side for 'analysis' and 'optimisation'
- (4.2) Fill in Spaces for name, project and C/C++ Application for the 'Main'-tab of the analysis application
<table><tr><td>
    <img src="/images/config genesys analysis.PNG" />
</td></tr></table>
<br/>
<br/>

- (4.3) Enter '--mode=analysis -j=1' in the Arguments-tab of the analysis application
- (4.4) Important: set the working directory to the Debug-folder location in your project as marked in red below!
<table><tr><td>
    <img src="/images/config genesys analysis arguments neu.PNG" />
</td></tr></table>
<br/>
<br/>

- (4.4) Repeat all upper steps for the optimisation application as shown below in the screenshots
<table><tr><td>
    <img src="/images/config genesys optimise.PNG" />
</td></tr></table>
<br/>
<br/>

<table><tr><td>
    <img src="/images/config genesys optimise arguments neu.PNG" />
</td></tr></table>
<br/>
<br/>



