#include <stdio.h>

int main() {
    const char *str = "+QGPSLOC: 082011.0,4544.4713N,12637.2670E,1.6,198.0,2,0.00,0.0,0.0,060623,05";
    
    // 跳过字符串开始的空白字符
    str += strspn(str, " \t\r\n");

    char time[10];
    char latitude[12];
    char longitude[12];
    
    sscanf(str, "+QGPSLOC: %[^,],%[^,],%[^,]", time, latitude, longitude);

    // 对时间进行处理
    char hour[3];
    char minute[3];
    char second[3];
    sscanf(time, "%2s%2s%2s", hour, minute, second);
    char new_time[10];
    sprintf(new_time, "%s-%s-%s", hour, minute, second);

    // 对经纬度进行处理
    double lat_degree, lat_minute, lon_degree, lon_minute;
    sscanf(latitude, "%2lf%lf", &lat_degree, &lat_minute);
    sscanf(longitude, "%3lf%lf", &lon_degree, &lon_minute);
    double new_latitude = lat_degree + lat_minute / 60.0;
    double new_longitude = lon_degree + lon_minute / 60.0;

    printf("New Time: %s\n", new_time);
    printf("New Latitude: %lf\n", new_latitude);
    printf("New Longitude: %lf\n", new_longitude);
    
    return 0;
}
