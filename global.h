#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>

extern double g_freMin;  // 声明全局最小频率
extern double g_freMax;  // 声明全局最大频率
//离散端口设置
extern int source_type_int;
extern int impedance;
extern int min_x;
extern int max_x;
extern int min_y;
extern int max_y;
extern int min_z;
extern int max_z;
extern QString  DiscretePorts_direction;

//近场探针设置
extern int nearfieldprobe_num;
extern int nearfieldprobe_x;
extern int nearfieldprobe_y;
extern int nearfieldprobe_z;
extern QString nearfieldprobe_EH;
extern QString nearfieldprobe_direction;

//网格边界
extern int connectDistance ;
extern int cpmlDistance ;
extern int cpmlWidth_x ;
extern int cpmlWidth_y ;
extern int cpmlWidth_z ;
extern int extraDistance ;



#endif // GLOBAL_H
