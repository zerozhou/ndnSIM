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
NS_LOG_COMPONENT_DEFINE("SimpleMobility");
void PrintInfo(NodeContainer allNodes)
{
    NS_LOG_DEBUG(allNodes.GetN());
   // NodeContainer :: iterator i;
    for(int i =0; i < allNodes.GetN(); ++i){
        NS_LOG_DEBUG( "ha hah" );
    }
    Simulator::Schedule(Seconds(1), PrintInfo, allNodes);
}
int main(int argc, char* argv[])
{
    Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
    Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
    Config::SetDefault("ns3::DropTailQueue::MaxPackets", StringValue("20"));

    CommandLine cmd;
    cmd.Parse(argc, argv);
    
    NS_LOG_DEBUG("11111\n");
   /* 
    PointToPointHelper p2p;
    PointToPointGridHelper grid(3, 3, p2p);
    grid.BoundingBox(10, 10, 90, 90);
*/
    AnnotatedTopologyReader topoReader("", 25);
    topoReader.SetFileName("topo/grid-3×3.txt");
    topoReader.Read();
    

    NS_LOG_DEBUG("testdhhhhi----1\n");
    //wifi
    Ptr<Node> wifiNodes = Names::Find<Node>("Node9");
    //wifiNodes.Create(1);
    Ptr<Node> wifiApNode1=Names::Find<Node>("Node0");
    Ptr<Node> wifiApNode2=Names::Find<Node>("Node1");

    //NodeContainer wifiApNode2=grid.GetNode(0,1);

    //NodeContainer wifiApNode1=nodes.Get(0);
    //NodeContainer wifiApNode2=nodes.Get(1);


    //PointToPointHelper p2p;
    //p2p.Install(nodes.Get(0), nodes.Get(1));
    //p2p.Install(nodes.Get(1), nodes.Get(2));

    
    NS_LOG_DEBUG("testi----0\n");  //ok in the above
    YansWifiChannelHelper channel=YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy= YansWifiPhyHelper::Default();
    phy.SetChannel(channel.Create());

    WifiHelper wifi = WifiHelper::Default();
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");
    
    NqosWifiMacHelper mac = NqosWifiMacHelper::Default();
    Ssid ssid = Ssid("ns-3-ssid");

    //install NetDevice in wifiNodes and wifiApNodes
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));

    NS_LOG_DEBUG("2-2-2-2-2--2\n");//ok in the above
    NetDeviceContainer staDevices;
    staDevices=wifi.Install(phy, mac, wifiNodes);
    NS_LOG_DEBUG("Install wifi on wifiNodesi\n");
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

    NS_LOG_DEBUG("testi----1\n");
    
    MobilityHelper mobilityAP1, mobilityAP2;
    
    mobilityAP1.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", DoubleValue(50),"MinY", DoubleValue(50));
    mobilityAP1.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobilityAP1.Install(wifiApNode1);
    //mobilityAP2.SetPositionAllocator("ns3::GridPositionAllocator");  // positionAlloc = (0,0)
    mobilityAP2.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", DoubleValue(75),"MinY", DoubleValue(50));
   
    mobilityAP2.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobilityAP2.Install(wifiApNode2);

// In the below, we will accomplish network layer and application layer
    
    NS_LOG_DEBUG("44444444\n");
    ndn::StackHelper ndnHelper;
/*
    //ndnHelper.AddRoute(wifiNodes.Get(0), "/prefix", grid.GetNode(0,1), 1);
    ndnHelper.AddRoute("Node0", "/prefix", "Node1" , 1);
    NS_LOG_DEBUG("44444444\n");
    ndnHelper.AddRoute("Node1", "/prefix", "Node4", 1);
    ndnHelper.AddRoute("Node4", "/prefix", "Node7", 1);
    ndnHelper.AddRoute("Node7", "/prefix", "Node8", 1);
    //NodeContainer wifiApNode1=grid.GetNode(0,0);
    
*/  //error ??
    NS_LOG_DEBUG("44444444\n");

    ndnHelper.SetDefaultRoutes(true);
    ndnHelper.Install("Node9");  
    ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
    //ndnGlobalRoutingHelper.InstallAll();
    ndnGlobalRoutingHelper.Install(Names::Find<Node>("Node0"));
    NS_LOG_DEBUG("8888\n");
    /*
    ndnGlobalRoutingHelper.Install("Node1");
    ndnGlobalRoutingHelper.Install("Node4");
    ndnGlobalRoutingHelper.Install("Node7");
    ndnGlobalRoutingHelper.Install("Node8");
*/

    NS_LOG_DEBUG("44444444\n");
//    Ptr<Node> consumer = wifiNodes.Get(0);
    ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
    consumerHelper.SetPrefix("/prefix");
    consumerHelper.SetAttribute("Frequency", StringValue("10"));
    consumerHelper.Install (wifiNodes);
    //consumerHelper.Install (consumer);


    ndn::AppHelper producerHelper("ns3::ndn::Producer");
    producerHelper.SetPrefix("/prefix");
    producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
    producerHelper.Install(Names::Find<Node>("Node8"));

    NS_LOG_DEBUG("55555\n");
    ndnGlobalRoutingHelper.AddOrigins ( "/prefix", Names::Find<Node>("Node8"));
    ndn::GlobalRoutingHelper::CalculateRoutes();
    /*
    NodeContainer allNodes;
    for(int i=0, j=0; i < 8&&j < 8; ++i, ++j)
        allNodes.Add(grid.GetNode(i,j));
        */
    NS_LOG_DEBUG("this is a test\n");
    NS_LOG_DEBUG(Names::Find<Node>("Node0"));
   // Simulator::Schedule(Seconds(1), PrintInfo, allNodes);

    Simulator::Stop(Seconds(20.0));
    Simulator::Run();
    Simulator::Destroy();
    return 0;


}
