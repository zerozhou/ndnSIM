#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/waypoint-mobility-model.h"
#include "ns3/steady-state-random-waypoint-mobility-model.h"

using namespace ns3;
int main(int argc, char* argv[])
{
    Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
    Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
    Config::SetDefault("ns3::DropTailQueue::MaxPackets", StringValue("20"));

    CommandLine cmd;
    cmd.Parse(argc, argv);
    
    PointToPointHelper p2p;
    PointToPointGridHelper grid(3, 3, p2p);
    grid.BoundingBox(10, 10, 90, 90);
    //NodeContainer nodes;
    //nodes.Create(3);

    //wifi
    NodeContainer wifiNodes;
    wifiNodes.Create(1);
    NodeContainer wifiApNode1=grid.GetNode(0,0);
    NodeContainer wifiApNode2=grid.GetNode(0,1);

    //NodeContainer wifiApNode1=nodes.Get(0);
    //NodeContainer wifiApNode2=nodes.Get(1);


    //PointToPointHelper p2p;
    //p2p.Install(nodes.Get(0), nodes.Get(1));
    //p2p.Install(nodes.Get(1), nodes.Get(2));

    
    YansWifiChannelHelper channel=YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy= YansWifiPhyHelper::Default();
    phy.SetChannel(channel.Create());

    WifiHelper wifi = WifiHelper::Default();
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");
    
    NqosWifiMacHelper mac = NqosWifiMacHelper::Default();
    Ssid ssid = Ssid("ns-3-ssid");

    //install NetDevice in wifiNodes and wifiApNodes
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));

    NetDeviceContainer staDevices;
    staDevices=wifi.Install(phy, mac, wifiNodes);
    
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    NetDeviceContainer apDevices;
    apDevices = wifi.Install(phy,mac,wifiApNode1);
    apDevices = wifi.Install(phy,mac,wifiApNode2);
    // in the above, we have accomplished the phy, mac layer install
    //mobility module install for wifiNodes and wifiApNodes
    MobilityHelper mobility;
    
    mobility.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", DoubleValue(60.0),
                                  "MinY", DoubleValue(20.0), "DeltaX", DoubleValue(5.0),
                                 "DeltaY", DoubleValue(5.0), "GridWidth", UintegerValue(10),
                                 "LayoutType", StringValue("RowFirst"));

    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Mode", StringValue("Time"),
                              "Time", StringValue("2s"),
                              "Speed", StringValue("ns3::ConstantRandomVariable[Constant=3.0]"),
                              "Bounds", RectangleValue(Rectangle(40, 100, 30, 31)));
    
    mobility.Install(wifiNodes);
    /*
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(5.0, 0.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::WaypointMobilityModel");
    
    mobility.Install(wifiNodes);
    */

    
    MobilityHelper mobilityAP1, mobilityAP2;
    
    mobilityAP1.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", DoubleValue(50),"MinY", DoubleValue(50));
    mobilityAP1.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobilityAP1.Install(wifiApNode1);
    //mobilityAP2.SetPositionAllocator("ns3::GridPositionAllocator");  // positionAlloc = (0,0)
    mobilityAP2.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", DoubleValue(75),"MinY", DoubleValue(50));
   
    mobilityAP2.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobilityAP2.Install(wifiApNode2);

// In the below, we will accomplish network layer and application layer
   /*
    //ndn::StackHelper ndnHelper;
    //ndnHelper.AddRoute(wifiNodes.Get(0), "/prefix", grid.GetNode(0,1), 1);
    ndnHelper.AddRoute(grid.GetNode(0,0), "/prefix", grid.GetNode(0,1), 1);
    ndnHelper.AddRoute(grid.GetNode(0,1), "/prefix", grid.GetNode(1,1), 1);
    ndnHelper.AddRoute(grid.GetNode(1,1), "/prefix", grid.GetNode(1,2), 1);
    ndnHelper.AddRoute(grid.GetNode(1,2), "/prefix", grid.GetNode(2,2), 1);
    //NodeContainer wifiApNode1=grid.GetNode(0,0);
    
*/
   //ndnHelper.SetDefaultRoutes(true);
    //ndnHelper.InstallAll();
    
    ndn::StackHelper ndnHelper;
    ndnHelper.SetForwardingStrategy("ns3::ndn::fw::BestRoute");
    //ndnHelper.SetForwardingStrategy("ns3::ndn::fw::CustomStrategy");
    ndnHelper.InstallAll();

    ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
    ndnGlobalRoutingHelper.InstallAll();
//    Ptr<Node> consumer = wifiNodes.Get(0);
    ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
    consumerHelper.SetPrefix("/prefix");
    consumerHelper.SetAttribute("Frequency", StringValue("10"));
    consumerHelper.Install (wifiNodes.Get(0));
    //consumerHelper.Install (consumer);


    ndn::AppHelper producerHelper("ns3::ndn::Producer");
    producerHelper.SetPrefix("/prefix");
    producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
    producerHelper.Install(grid.GetNode(2,2));

    ndnGlobalRoutingHelper.AddOrigins ( "/prefix", grid.GetNode(2,2) );
    ndn::GlobalRoutingHelper::CalculateRoutes();

    Simulator::Stop(Seconds(20.0));
    Simulator::Run();
    Simulator::Schedule(Seconds(8.0), ndn::GlobalRoutingHelper::CalculateRoutes);
    Simulator::Destroy();
    return 0;


}
