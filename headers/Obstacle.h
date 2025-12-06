#pragma once

#include <vector>
#include "SSD/SimPoint3D.h"
#include "SSD/SimString.h"
#include <string>
#include <SimOneServiceAPI.h>

class Obstacle
{
public:
    struct ObstacleEntry
    {
        ESimOne_Obstacle_Type type;   // 障碍物类型
        float posX;                   // 位置X坐标
        float posY;                   // 位置Y坐标
        float posZ;                   // 位置Z坐标
        float speed;                  // 合成后的速度
        float lateralSpeed;           // 横向速度
        float length;                 // 长度
        float width;                  // 宽度
        float height;                 // 高度
        SSD::SimString laneId;        // 所在的道路ID
        double distanceToMainVehicle; // 与主车的直线距离
        int id;                       // 障碍物ID
        bool isFront = true;          // 是否在主车前方（已弃用，建议自己根据位置计算）
        bool isBehind;                // 是否在主车后方（已弃用，建议自己根据位置计算）
        bool isSide;                  // 是否在主车侧方（已弃用，建议自己根据位置计算）
    };

    Obstacle();
    ~Obstacle();

    // 更新障碍物列表
    void updateObstacles(const SimOne_Data_Obstacle *pObstacle, const SSD::SimPoint3D &mainVehiclePos);

    // 计算速度的辅助函数
    static float calculateSpeed(float velX, float velY, float velZ);

    // 障碍物类型
    void printObstacleType(const ObstacleEntry &obstacle);

    // 所有障碍物信息
    void printAllObstacles();

    // 获取最近的障碍物
    const ObstacleEntry *getNearestObstacle(const SSD::SimString &mainVehicleLaneId = "");

    // 获取最近的侧方障碍物
    const Obstacle::ObstacleEntry *getNearestSideObstacle(const SSD::SimString &mainVehicleLaneId);

    // 清空障碍物列表
    void clear();

    // 获取障碍物数量
    size_t size();

    // 获取障碍物引用
    ObstacleEntry *getObstacleById(int id);

    // 获取所有障碍物的引用
    std::vector<ObstacleEntry> &getAllObstacles();

private:
    std::vector<ObstacleEntry> obstacles;
};
