#include "provided.h"
#include "ExpandableHashMap.h"
#include <list>
#include <set>
#include <queue>
using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* m_map;
    struct PriorityQueue
    {
      typedef pair<double, GeoCoord> element;
      priority_queue<element, std::vector<element>,greater<element>> elements;

      inline bool empty() const {
         return elements.empty();
      }

      inline void insert(GeoCoord item, double priority) {
        elements.emplace(priority, item);
      }

      GeoCoord get() {
        GeoCoord g = elements.top().second;
        elements.pop();
        return g;
      }
    };
};

inline double heuristic(GeoCoord a, GeoCoord b)
{
  return std::abs(a.latitude - b.latitude) + std::abs(a.longitude - b.longitude);
}

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    m_map = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    PriorityQueue frontier;
    frontier.insert(start, 0);
    
    ExpandableHashMap<GeoCoord, GeoCoord> came_from;
    ExpandableHashMap<GeoCoord, double> cost_so_far;
    came_from.associate(start, start);
    cost_so_far.associate(start, 0);
    
    while (!frontier.empty()) {
        GeoCoord current = frontier.get();
        
        if (current == end)
        {
            totalDistanceTravelled = 0;
            GeoCoord curr = end;
            GeoCoord* prev = came_from.find(curr);
            while(prev!=nullptr && *prev != start)
            {
                vector<StreetSegment> temp;
                m_map->getSegmentsThatStartWith(*prev, temp);
                for(int i = 0; i < temp.size(); i++)
                {
                    if(temp[i].end == curr)
                    {
                        route.push_front(temp[i]);
                        totalDistanceTravelled += distanceEarthMiles(*prev, curr);
                    }
                }
                curr = *prev;
                prev = came_from.find(curr);
            }
            return DELIVERY_SUCCESS;
        }
        
        vector<GeoCoord> visted;
        vector<StreetSegment> s;
        m_map->getSegmentsThatStartWith(current, s);
        visted.push_back(current);
        for (StreetSegment it : s) {
            GeoCoord next = it.end;
            double new_cost = *cost_so_far.find(current) + distanceEarthMiles(current, next);
            if (cost_so_far.find(next) == nullptr || new_cost < *cost_so_far.find(next)){
                cost_so_far.associate(next,new_cost);
                double priority = new_cost + heuristic(next, end);
                frontier.insert(next, priority);
                came_from.associate(next, current);
            }
        }
    }
    return NO_ROUTE;
}











//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}
