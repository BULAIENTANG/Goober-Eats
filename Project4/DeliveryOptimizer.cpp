#include "provided.h"
#include <vector>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
private:
    const StreetMap* m_map;
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
    m_map = sm;
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}


inline
bool operator<(const DeliveryRequest& lhs, const DeliveryRequest& rhs)
{
    return (lhs.location < rhs.location);
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    // crow distance is the direction straight line distance that ignores the streets below
    // initialize the OCD to zero
    oldCrowDistance = 0;
    GeoCoord curr = depot;
    // compute the OCD
    for(auto it = deliveries.begin(); it != deliveries.end(); it++)
    {
        oldCrowDistance += distanceEarthMiles(curr, it->location);
        curr = it->location;
    }

    
    // optimizing the delivery order by sorting with the geocoord's latitude and longitude

    sort(deliveries.begin(), deliveries.end());

    newCrowDistance = 0;
    // compute the NCD
    curr = depot;
    for(auto it = deliveries.begin(); it != deliveries.end(); it++)
    {
        newCrowDistance += distanceEarthMiles(curr, it->location);
        curr = it->location;
    }
}


//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
