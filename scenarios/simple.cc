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
//additional
#include "ns3/net-device.h"
#include "ns3/node.h"
#include "ns3/ndn-net-device-face.h"
#include "ns3/ndnSIM/model/ndn-net-device-face.h"
#include "ns3/ndn-l3-protocol.h"
#include "ns3/object.h"
using namespace ns3;
NS_LOG_COMPONENT_DEFINE("Simple");
int main(int argc, char* argv[])
{
    Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
    Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
    Config::SetDefault("ns3::DropTailQueue::MaxPackets", StringValue("20"));

    CommandLine cmd;
    cmd.Parse(argc, argv);
    
  /*  AnnotatedTopologyReader topoReader("", 25);
    topoReader.SetFileName("topo/node3.txt");
    topoReader.Read();
    */

    //wifi
    //Ptr<Node> wifiNode = Names::Find<Node>("Node0");
    NodeContainer ApNodes, wifiNode;
    wifiNode.Create(1);

    ApNodes.Create(2);
    //wifiNodes.Create(1);
    //Ptr<Node> wifiApNode1=Names::Find<Node>("Node1");
    //Ptr<Node> wifiApNode2=Names::Find<Node>("Node2");

    //初始化信道
    YansWifiChannelHelper channel;
    channel = YansWifiChannelHelper::Default();
    channel.SetPropagationDelay("ns3::ConstantSpeedProgationDelayModel");
    channel.AddPropagationLoss("ns3::FixedRssLossModel", "Rss", "DoubleValue(rss)");

    //phy layer
    YansWifiPhyHelper phy;
    phy = YansWifiPhyHelper::Default();
    phy.SetChannel(channel.Create());

    WifiHelper wifi = WifiHelper::Default();
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");
    
    // mac layer
    NqosWifiMacHelper mac = NqosWifiMacHelper::Default();
    Ssid ssid = Ssid("ns-3-ssid");
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));
    
    //net device
    NetDeviceContainer staDevices;
    staDevices=wifi.Install(phy, mac, wifiNode);
    // Ptr<NetDevice> netDevice_wifi = wifi.Install(py, mac, wifiNodes);

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
    
    mobility.Install(wifiNode);

    NS_LOG_DEBUG("test----1\n");
    
    MobilityHelper mobilityAP1, mobilityAP2;
    
    mobilityAP1.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", DoubleValue(50),"MinY", DoubleValue(50));
    mobilityAP1.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobilityAP1.Install(wifiApNode1);
    //mobilityAP2.SetPositionAllocator("ns3::GridPositionAllocator");  // positionAlloc = (0,0)
    mobilityAP2.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", DoubleValue(75),"MinY", DoubleValue(50));
   
    mobilityAP2.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobilityAP2.Install(wifiApNode2);

// In the below, we will accomplish network layer and application layer
    
    NS_LOG_DEBUG("test----2\n");
    ndn::StackHelper ndnHelper;
    /*
    // Ptr<NetDevice> netDevice_wifi = wifi.Install(py, mac, wifiNodes);
    Ptr<NetDevice> NetDevice_wifi = wifiNode->GetDevice(0);//获取网卡
    Ptr<ndn::NetDeviceFace> face_wifi = CreateObject<ndn::NetDeviceFace>(wifiNode, NetDevice_wifi);//获取网卡上的接口
    */
/*
    Ptr<NetDevice> NetDevice_wifi = wifiNode->GetDevice(0);//获取网卡
    Ptr<ndn::L3Protocol> ndn = wifiNode->GetObject<ndn::L3Protocol>();
    //Ptr<ndn::Face> face = ndn->GetFace(0);  //error, invild conversion from ...to ... the faceID =0, or  = 1,2,3.
    Ptr<ndn::NetDeviceFace> face = ndnHelper.DefaultNetDeviceCallback(wifiNode, ndn, NetDevice_wifi);
*/
    ndnHelper.AddRoute("Node0", "/prefix", 1, 1);//第三个参数是uint32_t 类型
    ndnHelper.AddRoute("Node1", "/prefix", "Node2" , 1);
    
    NS_LOG_DEBUG("test----3\n");
    NS_LOG_DEBUG(wifiNode->GetNDevices());
    //ndnHelper.SetDefaultRoutes(true);
    //ndnHelper.InstallAll();
    /*
    ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
    //ndnGlobalRoutingHelper.InstallAll();
    ndnGlobalRoutingHelper.Install(Names::Find<Node>("Node0"));
    NS_LOG_DEBUG("8888\n");
    ndnGlobalRoutingHelper.Install("Node1");
    ndnGlobalRoutingHelper.Install("Node4");
    ndnGlobalRoutingHelper.Install("Node7");
    ndnGlobalRoutingHelper.Install("Node8");
*/

    NS_LOG_DEBUG("test----4\n");
//    Ptr<Node> consumer = wifiNodes.Get(0);
    ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");

    NS_LOG_DEBUG("test----5\n");
    consumerHelper.SetPrefix("/prefix");
    consumerHelper.SetAttribute("Frequency", StringValue("10"));
    consumerHelper.Install (wifiNode);
    //consumerHelper.Install (consumer);


    ndn::AppHelper producerHelper("ns3::ndn::Producer");
    producerHelper.SetPrefix("/prefix");
    producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
    producerHelper.Install(Names::Find<Node>("Node1"));

    NS_LOG_DEBUG("test----6\n");
    //ndnGlobalRoutingHelper.AddOrigins ( "/prefix", Names::Find<Node>("Node8"));
    //ndn::GlobalRoutingHelper::CalculateRoutes();
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
