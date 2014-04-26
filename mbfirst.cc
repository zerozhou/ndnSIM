#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
using namespace ns3;

int main(int argc, char *argv[])
{
    NodeContainer nodes;
    nodes.Create(2);
/*
    PointToPointHelper  pointToPointer;
    pointToPointer.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPointer.SetDeviceAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPointer.Install(nodes);
*/
    MobilityHelper mbHelper;
    mbHelper.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", DoubleValue(1.0), 
                                 "MinY", DoubleValue(1.0), "DeltaX", DoubleValue(5.0), 
                                 "DeltaY", DoubleValue(5.0), "GridWidth", UintegerValue(3),
                                 "LayoutType", StringValue("RowFirst"));
    mbHelper.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue("Time"), "Time", StringValue("2s"),
                             "Speed", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"),
                             "Bounds", RectangleValue(Rectangle(0.0, 20.0, 0.0, 20.0)));
    mbHelper.Install(nodes);
    mbHelper.AssignStreams(nodes, 0);
    
    AsciiTraceHelper ascii;
    MobilityHelper::EnableAsciiAll(ascii.CreateFileStream("mobility-trace-example.mob"));

    Simulator::Stop(Seconds(5.0));
    Simulator::Run;
    Simulator::Destroy();
    return 0;


}

