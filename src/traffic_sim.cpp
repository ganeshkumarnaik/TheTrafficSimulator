#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <cstring>
#include <algorithm>
#include <cstdlib>
#include <thread>
#include <atomic>
#include <fstream>
#include <ctime>
#include <sstream>
#include <mutex>
#include <memory>

/* Global Variables */
std::atomic<int> vehicleDensity(10);
std::mutex vehicle_mtx;
enum class vehicleType : unsigned int {
    BIKE = 0,
    CAR,
    TRUCK,
};

/* Global Functions */
void setVehicleDensity()
{
    while(1)
    {
        //Change vehicle density every 5 seconds
        vehicleDensity = 10;
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        vehicleDensity = 50;
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
}

/* Class definitions */
class vehicle
{
    public:
        char symbol = 'V';
        int currentPosition = 0;
        double currentSpeed = 0;
        double targetSpeed = 0;
        int previousGap = 0;
        int vehicleId = 0;

        vehicle()
        {
            static int id = 0;
            vehicleId = id++;
        }

        // Virtual destructor so that derived classes can also be destroyed when base class is destroyed.
        virtual ~vehicle() = default;

        virtual void setVehicleSymbol()
        {
            symbol = 'V';
        }

        virtual void setVehicleCharacteristics(int position, int cspeed, int tspeed)
        {
            currentPosition = position;
            currentSpeed = cspeed;
            targetSpeed = tspeed;
        }

        //Other methods, stay in base class.
        void updateCurrentSpeed(int speed)
        {
            currentSpeed = speed;
        }

        void updatePosition()
        {
            currentPosition += currentSpeed;
        };

        bool isVehicleBeyondRoadLength(int roadLength)
        {
            return (currentPosition >= roadLength);
        }
};

/* Concrete vehicle classes */
class car : public vehicle
{
    public:
        car()
        {
            setVehicleSymbol();
        }
        virtual void setVehicleCharacteristics(int position, int cspeed, int tspeed) override
        {
            currentPosition = position;
            currentSpeed = 1;
            targetSpeed = 3;
        }
        virtual void setVehicleSymbol() override
        {
            symbol = 'C';
        }
};

class bike : public vehicle
{
    public:
        bike()
        {
            setVehicleSymbol();
        }
        virtual void setVehicleCharacteristics(int position, int cspeed, int tspeed) override
        {
            currentPosition = position;
            currentSpeed = 1;
            targetSpeed = 2;
        }
        virtual void setVehicleSymbol() override
        {
            symbol = 'B';
        }
};

class truck : public vehicle
{
    public:
        truck()
        {
            setVehicleSymbol();
        }
        virtual void setVehicleCharacteristics(int position, int cspeed, int tspeed) override
        {
            currentPosition = position;
            currentSpeed = 1;
            targetSpeed = 1;
        }
        virtual void setVehicleSymbol() override
        {
            symbol = 'T';
        }
};

class vehicleFactory
{
    public:
        static std::unique_ptr<vehicle> createVehicle(vehicleType vType)
        {
            switch(vType)
            {
                case vehicleType::CAR  : 
                {
                    return std::make_unique<car>();
                }
                case vehicleType::TRUCK: 
                {
                    return std::make_unique<truck>();
                }
                case vehicleType::BIKE:
                {
                    return std::make_unique<bike>();
                }
                default                : return nullptr;
            }  
        }
};


class road
{
    static constexpr int length = 75;
    char roadSymbol = '_';
    char roadBreakSymbol = ' ';
    unsigned int vehiclePresentAtPosition[length] = {0};
    char vehicleSymbolAtPosition[length] = "";

    public:
        std::vector <std::unique_ptr<vehicle>> vehicles;

        road()
        {
            spawnLeadVehicle();
        }

        int getLength()
        {
            return length;
        }

        void spawnLeadVehicle()
        {
            auto leadVehicle = vehicleFactory::createVehicle(vehicleType::CAR);
            leadVehicle->setVehicleCharacteristics(0,2,2);
            vehicles.push_back(std::move(leadVehicle));
        }

        void randomSpawnVehicle()
        {
            if(vehicles.empty())
            {
                //Continue with spawn logic
                spawnVehicle();
            }
            else
            {
                if(vehicles.back()->currentPosition >= 3)
                {
                    spawnVehicle();
                }
                else
                {
                    /* Skip spawning */
                }
            }
        }

        void spawnVehicle()
        {
            // Read Vehicle Density. Vehicle Density of 10 == Spawn probability of 10%
            bool toSpawn = (rand() % 100) < vehicleDensity;
            double tSpeed = (rand() % 3) + 1;
            double cSpeed = 1;
            // std::cout << "Vehicle density: " << vehicleDensity;
            if (toSpawn)
            {
                vehicleType vType;
                if(tSpeed == 1)
                {
                    vType = vehicleType::TRUCK;
                }
                else if(tSpeed == 2)
                {
                    vType = vehicleType::BIKE;
                }
                else
                {
                    vType = vehicleType::CAR;
                }
                auto newVehicle = vehicleFactory::createVehicle(vType);
                newVehicle->setVehicleCharacteristics(0,cSpeed,tSpeed);
                vehicles.push_back(std::move(newVehicle));
            }
        }

        void updateAllVehicles()
        {
            /* Smart update. loop through all vehicles from the back of the vector */
            if(vehicles.size() > 1)
            {
                for(int i = vehicles.size()-1; i >= 1; i--)
                {
                    vehicle &leadVehicle = *vehicles[i-1];
                    vehicle &followingVehicle = *vehicles[i];

                    int currentGap = leadVehicle.currentPosition - followingVehicle.currentPosition - 1;
                    
                    if(currentGap <= followingVehicle.previousGap && currentGap <= 2)
                    {
                        followingVehicle.currentSpeed = leadVehicle.currentSpeed;
                    }
                    else
                    {
                        if(followingVehicle.currentSpeed < followingVehicle.targetSpeed)
                        {
                            followingVehicle.currentSpeed++;
                        }
                    }

                    followingVehicle.previousGap = currentGap;
                }
            }

            /* Loop through all vehicles in the vector and update their positions */
            for (auto &eachVehicle : vehicles)
            {
                eachVehicle->updatePosition();
            }
            /* Identify vehicles to remove */
            auto newEndInVector = std::remove_if
                            (
                                vehicles.begin(), 
                                vehicles.end(), 
                                [this](const std::unique_ptr<vehicle> &v)
                                {
                                    return v->isVehicleBeyondRoadLength(length);
                                } 
                            );
            vehicles.erase(newEndInVector, vehicles.end());
        }

        void getAllVehiclePositionsSymbol()
        {
            for(auto &eachVehicle: vehicles)
            {
                vehiclePresentAtPosition[eachVehicle->currentPosition] = 1;
                vehicleSymbolAtPosition[eachVehicle->currentPosition] = eachVehicle->symbol;
            }
        }

        void clearAllVehiclePositions()
        {
            memset(vehiclePresentAtPosition, 0, sizeof(vehiclePresentAtPosition));
            memset(vehicleSymbolAtPosition, 0, sizeof(vehicleSymbolAtPosition));
        }

        void drawRoad()
        {
            for(unsigned int i=0; i < length; i++)
            {
                /* if there is a vehicle in current position, print symbol. else print _ */
                if (vehiclePresentAtPosition[i] == 1)
                {
                    std::cout << vehicleSymbolAtPosition[i] << " ";
                }
                else
                {
                    std::cout << "_" << " ";
                }
            }
        }

        void printAllVehicles()
        {
            for(auto &eachVehicle: vehicles)
            {
                std::cout << eachVehicle->currentPosition << " ";
            }
        }
};

class Telemetry{
    /* This must be a singleton class. Following are the rules.
       1. Make all constructors private.
       2. Delete all copy constructors.
       3. Make a private static pointer that can point to same class object(singleton).
       4. Make a public static function that returns pointer to the same class object.
    */

    private:
    //private constructor.
    Telemetry(){};
    static Telemetry* singletonInstancePtr;

    public:
        //Deleting copy constructor. &obj is reference to old object.
        Telemetry(const Telemetry &obj) = delete;

        //Static method to get singleton instance as REFERENCE
        static Telemetry& getInstance()
        {
            //No need to use a mutex. compiler assures this is thread safe.
            static Telemetry singletonInstanceRef;
            return singletonInstanceRef;
        }

        void logTelemetry(road& singleRoad, std::string fileName)
        {
            //Make this singleton
            while(1)
            {
                //Read position and speed of each vehicle on road and write to csv per timestamp
                //how to get road class to this thread?
                std::this_thread::sleep_for(std::chrono::milliseconds(500));

                //Open CSV file, write vehicle data, close CSV file.
                std::ofstream myFile(fileName, std::ios::app);

                //Get time stamp
                auto timeNow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

                std::string stringTimeNow = ctime(&timeNow);
                std::string rowEntry;
                std::string rowEntryTimeStamp;
                std::string rowEntryVehicleData;

                stringTimeNow.erase(std::remove_if(stringTimeNow.begin(), stringTimeNow.end(), 
                        [&](char ch) 
                        { return std::iscntrl(static_cast<unsigned char>(ch));}), 
                        stringTimeNow.end());

                rowEntryTimeStamp = stringTimeNow + ",";

                vehicle_mtx.lock();
                for(auto &eachVehicle: singleRoad.vehicles)
                {
                    rowEntryVehicleData = rowEntryVehicleData + std::to_string(eachVehicle->vehicleId) + "," + std::to_string(eachVehicle->currentPosition) + "," +  std::to_string(eachVehicle->currentSpeed) + ",";
                }
                vehicle_mtx.unlock();
                
                rowEntry = rowEntryTimeStamp + rowEntryVehicleData + "\n";
                myFile << rowEntry;
                myFile.close();
            }
        }
};

int main()
{
    /* Instantiate all objects */
    road SingleRoad;
    Telemetry &telemetry = Telemetry::getInstance(); // Scope resolution operator because static methods belong to class, not objects
    // NOTE: telemetry is reference to the object, not the object itself

    /* Start thread to control Vehicle Density */
    std::thread t1(setVehicleDensity);

    // Get current time
    auto now = std::chrono::system_clock::now();
    auto localTime = std::chrono::system_clock::to_time_t(now);
    std::string stringLocalTime = ctime(&localTime);
    
    // Remove control characters from timestamp
    stringLocalTime.erase(std::remove_if(stringLocalTime.begin(), stringLocalTime.end(), 
            [&](char ch) 
            { return std::iscntrl(static_cast<unsigned char>(ch));}), 
            stringLocalTime.end());

    // Append time to file name
    std::string fileName = "logs/Telemetry_"+ stringLocalTime + ".csv";

    /* Start thread to handle Logging */
    std::thread t2(&Telemetry::logTelemetry, &telemetry, std::ref(SingleRoad), fileName); //still dont understand this

    while(1)
    {
        /* Main thread */
        std::system("clear");
        SingleRoad.drawRoad();
        std::cout << std::endl;
        SingleRoad.clearAllVehiclePositions();
        SingleRoad.updateAllVehicles();
        SingleRoad.randomSpawnVehicle();
        SingleRoad.getAllVehiclePositionsSymbol();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    t1.join();
    t2.join();

    return 0;
}