#include "provided.h"
#include "ExpandableHashMap.h"
#include <string>
#include <fstream>
#include <vector>
#include <functional>
using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
    
private:
    ExpandableHashMap<GeoCoord, vector<StreetSegment>>* m_map;
};

StreetMapImpl::StreetMapImpl()
{
    m_map = new ExpandableHashMap<GeoCoord, vector<StreetSegment>>;

}

StreetMapImpl::~StreetMapImpl()
{
    delete m_map;
}

bool StreetMapImpl::load(string mapFile)
{
    m_map->reset();
    ifstream inf(mapFile);
    if(!inf) // if fail to open file
    {
        cerr << "ERROR: Cannot open mapdata.txt!" << endl;
        return false;
    }
    string line;
    string streetName;
    
    while(getline(inf,line)) // traverse through the file
    {
        vector<StreetSegment> segments;
        vector<StreetSegment> RevSegments;
        
        for(int i = 0; i < line.size(); i++)
        {
            if(isalpha(line[i]))
            {
                // set the name of street
                streetName = line;
                break; // exit the for-loop
            }
        }
        
        if(line.size() != 47) // if not lines with GeoCoords
            continue;
        
        string startLat, startLong, endLat, endLong;
        
        for (int i = 0; i <= 9; i++)
            startLat += line[i];
        for (int i = 11; i <= 22; i++)
            startLong += line[i];
        GeoCoord sp = GeoCoord(startLat,startLong); // starting point
        
        for (int i = 24; i <= 33; i++)
            endLat += line[i];
        for (int i = 35; i <= 46; i++)
            endLong += line[i];
        GeoCoord ep = GeoCoord(endLat,endLong); // ending point
        
        // fill the street segments into the vector of (Reverse)segments
        StreetSegment S = StreetSegment(sp, ep, streetName);
        StreetSegment R = StreetSegment(ep, sp, streetName);
        segments.push_back(S);
        RevSegments.push_back(R);

        // associate starting and ending GeoCoord to vector of segments
        if(m_map->find(sp) != nullptr)
            m_map->find(sp)->push_back(S);
        else
            m_map->associate(sp, segments);

        if(m_map->find(ep) != nullptr)
            m_map->find(ep)->push_back(R);
        else
            m_map->associate(ep, RevSegments);
    }
    return true;
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    if(m_map->find(gc) == nullptr)
        return false;
    segs.clear();
    segs = *(m_map->find(gc));
    return true;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}

