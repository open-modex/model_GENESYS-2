#ifndef VERSION_H_
#define VERSION_H_

#define VERSION_MAJOR 1
#define VERSION_MINOR 5
#define VERSION_PATCH 3
#define VERSION_BUILD 0

#define QU(x) #x
#define QUH(x) QU(x)
#define VERSION_STRING QUH(VERSION_MAJOR) "." QUH(VERSION_MINOR) "." QUH(VERSION_PATCH) " build " QUH(VERSION_BUILD)


#endif /* VERSION_H_ */
