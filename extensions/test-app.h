#ifndef TEST_APP_H_
#define TEST_APP_H_
#include "ns3/ndn-app.h"
namespace ns3 {

class ServerApp : public ndn::App
{
public:
	static TypeId GetTypeId();
	virtual void StartApplication();
	virtual void StopApplication();
	virtual void OnInterest(Ptr<const ndn::Interest> interest);
	virtual void OnData(Ptr<const ndn::Data> contentObject);
};

class ClientApp : public ndn::App
{
public:
	static TypeId GetTypeId();
	virtual void StartApplication();
	virtual void StopApplication();
	virtual void OnInterest(Ptr<const ndn::Interest> interest);
	virtual void OnData(Ptr<const ndn::Data> contentObject);
	virtual void Period();
private:	
	void SendInterest(const std::string &name);
};

} //end namespace ns3

#endif
