#include "Obstacle.h"
#include "hdmap/SampleGetNearMostLane.h"
#include "SimOneHDMapAPI.h"
#include "UtilMath.h"
#include <iostream>
#include <limits>
#include "string"
#include "SimOneServiceAPI.h"
#include "iomanip"
#include "algorithm"

Obstacle::Obstacle()
{
}

Obstacle::~Obstacle()
{
    clear();
}

void Obstacle::updateObstacles(const SimOne_Data_Obstacle *pObstacle, const SSD::SimPoint3D &mainVehiclePos)
{
    obstacles.clear();

    for (size_t i = 0; i < pObstacle->obstacleSize; ++i)
    {
        ObstacleEntry entry;

        entry.type = pObstacle->obstacle[i].type;     // 障碍物类型
        entry.posX = pObstacle->obstacle[i].posX;     // 障碍物的位置：X坐标
        entry.posY = pObstacle->obstacle[i].posY;     // 障碍物的位置：Y坐标
        entry.posZ = pObstacle->obstacle[i].posZ;     // 障碍物的位置：Z坐标
        entry.length = pObstacle->obstacle[i].length; // 障碍物的长度
        entry.width = pObstacle->obstacle[i].width;   // 障碍物的宽度
        entry.height = pObstacle->obstacle[i].height; // 障碍物的高度
        entry.id = pObstacle->obstacle[i].id;         // 障碍物的ID

        // 计算合成速度
        entry.speed = calculateSpeed(pObstacle->obstacle[i].velX,
                                     pObstacle->obstacle[i].velY,
                                     pObstacle->obstacle[i].velZ);

        // 计算横向速度
        entry.lateralSpeed = std::abs(pObstacle->obstacle[i].velY);

        // 获取障碍物所在车道
        SSD::SimPoint3D obstaclePos(entry.posX, entry.posY, entry.posZ);
        entry.laneId = SampleGetNearMostLane(obstaclePos);

        // 计算与主车的直线距离
        entry.distanceToMainVehicle = UtilMath::planarDistance(mainVehiclePos, obstaclePos);

        obstacles.push_back(entry);
    }
}

// 通过ID获取障碍物引用
Obstacle::ObstacleEntry *Obstacle::getObstacleById(int id)
{
    for (auto &obs : obstacles)
    {
        if (obs.id == id)
            return &obs;
    }
    return nullptr;
}

// 获取所有障碍物的引用
std::vector<Obstacle::ObstacleEntry> &Obstacle::getAllObstacles()
{
    return obstacles;
}

static float Obstacle::calculateSpeed(float velX, float velY, float velZ)
{
    return std::sqrt(velX * velX + velY * velY + velZ * velZ);
}

void Obstacle::printObstacleType(const ObstacleEntry &obstacle)
{
    std::string typeStr;

    switch (obstacle.type)
    {
    case ESimOne_Obstacle_Type_Unknown:
        typeStr = "未知障碍物";
        break;
    case ESimOne_Obstacle_Type_Pedestrian:
        typeStr = "行人";
        break;
    case ESimOne_Obstacle_Type_Pole:
        typeStr = "柱子";
        break;
    case ESimOne_Obstacle_Type_Car:
        typeStr = "小汽车";
        break;
    case ESimOne_Obstacle_Type_Static:
        typeStr = "静态物体";
        break;
    case ESimOne_Obstacle_Type_Bicycle:
        typeStr = "自行车";
        break;
    case ESimOne_Obstacle_Type_Fence:
        typeStr = "栅栏";
        break;
    case ESimOne_Obstacle_Type_RoadMark:
        typeStr = "路标";
        break;
    case ESimOne_Obstacle_Type_TrafficSign:
        typeStr = "交通标志";
        break;
    case ESimOne_Obstacle_Type_TrafficLight:
        typeStr = "交通信号灯";
        break;
    case ESimOne_Obstacle_Type_Truck:
        typeStr = "卡车";
        break;
    case ESimOne_Obstacle_Type_Bus:
        typeStr = "公交车";
        break;
    case ESimOne_Obstacle_Type_Motorcycle:
        typeStr = "摩托车";
        break;
    default:
        typeStr = "其他类型";
        break;
    }

    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "| ID: " << std::left << std::setw(5) << obstacle.id
              << "| 类型: " << std::left << std::setw(10) << typeStr << std::endl;
    // << "| 位置: " << std::left << std::setw(6) << posStr << " |" << std::endl;

    std::cout << "| 距离: " << std::fixed << std::setw(6) << std::setprecision(2) << obstacle.distanceToMainVehicle << "m"
              << "| 速度: " << std::fixed << std::setw(6) << std::setprecision(2) << obstacle.speed << "m/s"
              << "| 横向速度: " << std::fixed << std::setw(6) << std::setprecision(2) << obstacle.lateralSpeed << "m/s |" << std::endl;

    std::cout << "| 尺寸: " << std::fixed << std::setw(4) << std::setprecision(1) << obstacle.length << "x"
              << std::fixed << std::setw(4) << std::setprecision(1) << obstacle.width << "x"
              << std::fixed << std::setw(4) << std::setprecision(1) << obstacle.height << "m"
              << "| 车道: " << std::left << std::setw(15) << obstacle.laneId.GetString() << " |" << std::endl;
}

void Obstacle::printAllObstacles()
{
    std::cout << "当前场景中共有 " << obstacles.size() << " 个障碍物:" << std::endl;

    std::vector<ObstacleEntry *> sortedObstacles;
    for (auto &obs : obstacles)
    {
        sortedObstacles.push_back(&obs);
    }

    // 根据距离排序
    std::sort(sortedObstacles.begin(), sortedObstacles.end(),
              [](const ObstacleEntry *a, const ObstacleEntry *b)
              {
                  return a->distanceToMainVehicle < b->distanceToMainVehicle;
              });

    // 前3个最近的障碍物
    size_t count = std::min(size_t(3), obstacles.size());
    for (size_t i = 0; i < count; i++)
    {
        const ObstacleEntry *obs = sortedObstacles[i];
        printObstacleType(*obs);
        std::cout << std::endl;
    }
}

const Obstacle::ObstacleEntry *Obstacle::getNearestObstacle(const SSD::SimString &mainVehicleLaneId)
{
    if (obstacles.empty())
    {
        return nullptr;
    }

    const ObstacleEntry *nearest = nullptr;
    double minDistance = std::numeric_limits<double>::max();

    for (const auto &obs : obstacles)
    {
        // 如果指定了车道ID，则只考虑同车道上的障碍物
        if (!mainVehicleLaneId.Empty() && obs.laneId != mainVehicleLaneId)
        {
            continue;
        }

        if (obs.distanceToMainVehicle < minDistance)
        {
            minDistance = obs.distanceToMainVehicle;
            nearest = &obs;
        }
    }

    return nearest;
}

// 获取相邻车道上的障碍物（有速度的）
const Obstacle::ObstacleEntry *Obstacle::getNearestSideObstacle(const SSD::SimString &mainVehicleLaneId)
{
    if (obstacles.empty())
    {
        return nullptr;
    }

    const ObstacleEntry *nearestSide = nullptr;
    if (!mainVehicleLaneId.Empty())
    {
        HDMapStandalone::MLaneLink CURRENT_LANE_LINK;
        if (SimOneAPI::GetLaneLink(mainVehicleLaneId, CURRENT_LANE_LINK))
        {
            SSD::SimString leftNeighborLaneId = CURRENT_LANE_LINK.leftNeighborLaneName;
            SSD::SimString rightNeighborLaneId = CURRENT_LANE_LINK.rightNeighborLaneName;

            // 获取左侧车道上的障碍物
            for (const auto &obs : obstacles)
            {
                if (obs.laneId == leftNeighborLaneId && obs.speed > 2.f)
                {
                    if (nearestSide == nullptr || obs.distanceToMainVehicle < nearestSide->distanceToMainVehicle)
                    {
                        nearestSide = &obs;
                    }
                }
            }
            // 获取右侧车道上的障碍物
            // ...
        }
    }
    return nearestSide;
}

void Obstacle::clear()
{
    obstacles.clear();
}

size_t Obstacle::size()
{
    return obstacles.size();
}
