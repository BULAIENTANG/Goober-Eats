#include "provided.h"
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* m_map;
    DeliveryOptimizer* m_opt;
    PointToPointRouter* m_ppr;
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
{
    m_map = sm;
    m_opt = new DeliveryOptimizer(m_map);
    m_ppr = new PointToPointRouter(m_map);
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
    delete m_opt;
    delete m_ppr;
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    double oldCrowDist;
    double newCrowDist;
    // copy of the delivery requests
    vector<DeliveryRequest> deliveryReq = deliveries;
    list<StreetSegment> route;
    m_opt->optimizeDeliveryOrder(depot, deliveryReq, oldCrowDist, newCrowDist);
    GeoCoord curr = depot;
    
    totalDistanceTravelled = 0;
    
    // following sequence of delivery request
    for(int i = 0; i <= deliveryReq.size(); i++)
    {
        double distance;
        int routeResult = m_ppr->generatePointToPointRoute(curr, deliveryReq[i].location, route, distance);
        if(i == deliveryReq.size()) //move back to depot
            routeResult = m_ppr->generatePointToPointRoute(curr, depot, route, distance);
        
        switch (routeResult) {
            case NO_ROUTE:
                return NO_ROUTE;
                break;
            case BAD_COORD:
                return BAD_COORD;
        }
        totalDistanceTravelled += distance;

        GeoCoord destination = deliveryReq[i].location;
        
        
        // generate command on segments of route
        StreetSegment* prevSeg = nullptr;
        
        for(auto it = route.begin(); it != route.end(); it++)
        {
            // if the current location is destination, deliver
            if(it->start == destination && i!=deliveryReq.size())
            {
                DeliveryCommand cmd = DeliveryCommand();
                cmd.initAsDeliverCommand(deliveryReq[i].item);
                commands.push_back(cmd);
                prevSeg = nullptr;
                break;
            }
            // the distance of the segment
            double dist = distanceEarthMiles(it->start, it->end);
            // the angle of direction of the segment dir
            string dir;
            double angle = angleOfLine(*it);
            if(0 <= angle && angle < 22.5)
                dir = "east";
            if(22.5 <= angle && angle < 67.5)
                dir = "northeast";
            if(67.5 <= angle && angle < 112.5)
                dir = "north";
            if(112.5 <= angle && angle < 157.5)
                dir = "northwest";
            if(157.5 <= angle && angle < 202.5)
                dir = "west";
            if(202.5 <= angle && angle < 247.5)
                dir = "southwest";
            if(247.5 <= angle && angle < 292.5)
                dir = "south";
            if(292.5 <= angle && angle < 337.5)
                dir = "southeast";
            if(337.5 <= angle)
                dir = "east";
            
            // if no previous segment
            if(prevSeg == nullptr)
            {
                DeliveryCommand cmd = DeliveryCommand();
                cmd.initAsProceedCommand(dir, it->name, dist);
                commands.push_back(cmd);
                prevSeg = &(*it);
                continue;
            }
            
            DeliveryCommand turningCmd = DeliveryCommand();
            // the angle between previous segment and current
            double angleBetweenSegs = angleBetween2Lines(*(prevSeg),*it);
            
            // if the segment is the same with previous, proceed on the same street
            if(it->name == prevSeg->name)
                commands.back().increaseDistance(dist);
            
            else if(angleBetweenSegs < 1 || angleBetweenSegs > 359)
            {
                turningCmd.initAsProceedCommand(dir, it->name, dist);
                commands.push_back(turningCmd);
            }
            else if(angleBetweenSegs >= 1 && angleBetweenSegs < 180)
            {
                turningCmd.initAsTurnCommand("left", it->name);
                DeliveryCommand proceedCmd = DeliveryCommand();
                proceedCmd.initAsProceedCommand(dir, it->name, dist);
                commands.push_back(turningCmd);
                commands.push_back(proceedCmd);
            }
            else if(angleBetweenSegs <= 359 && angleBetweenSegs >= 180)
            {
                turningCmd.initAsTurnCommand("right", it->name);
                DeliveryCommand proceedCmd = DeliveryCommand();
                proceedCmd.initAsProceedCommand(dir, it->name, dist);
                commands.push_back(turningCmd);
                commands.push_back(proceedCmd);
            }
            if(it->end == destination && i!=deliveryReq.size())
            {
                DeliveryCommand cmd = DeliveryCommand();
                cmd.initAsDeliverCommand(deliveryReq[i].item);
                commands.push_back(cmd);
                prevSeg = nullptr;
                break;
            }
            prevSeg = &(*it);
        }
        // clear the route
        route.clear();
        if(i!=deliveryReq.size())
            curr = deliveryReq[i].location;
    }
    return DELIVERY_SUCCESS;
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
