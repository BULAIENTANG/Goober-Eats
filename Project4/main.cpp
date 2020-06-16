//
//#include <iostream>
//#include <list>
//#include <functional>
//#include "ExpandableHashMap.h"
//#include "provided.h"
//using namespace std;
//
//int main(int argc, const char * argv[]) {
//    StreetMap m;
//    m.load("/Users/tang/Desktop/skeleton/mapdata.txt");
//    GeoCoord p("34.0547000" , "-118.4794734"); // 10th Helena Drive
//    GeoCoord q("34.0549825" , "-118.4795629"); //11 th
//    GeoCoord r("34.0734335" , "-118.4449143"); //westwood 1
//    GeoCoord w("34.0670755" , "-118.4451231"); //westwood 2
//    GeoCoord e("34.0668846", "-118.4450991"); //westwood 3
//    GeoCoord f("34.0558289" , "-118.4798296"); // 13th helena
//
//    list<StreetSegment> route;
//    vector<StreetSegment> s;
//
//    double distanceTravelled = 0.0;
//    PointToPointRouter router(&m);
//    router.generatePointToPointRoute(p, e, route, distanceTravelled);
//    cout << "Total distance travelled is " << distanceTravelled << " miles" << endl;
//
//    cout << "===" << endl;
//
//    distanceTravelled = 0.0;
//    router.generatePointToPointRoute(p, q, route, distanceTravelled);
//    cout << "Total distance travelled is " << distanceTravelled << " miles" << endl;
//    cout << endl;
//
//
//
//    DeliveryOptimizer del(&m);
//    double oldDis, newDis;
//    DeliveryRequest x("Chicken", q); // 1
//    DeliveryRequest y("beer", r); //2
//    DeliveryRequest z("pizza", w); //2
//    DeliveryRequest a("burger", e); //2
//    DeliveryRequest c("haha", f); //1
//
//
//    GeoCoord b("34.0636671", "-118.4461842");
//    GeoCoord depot("34.0625329", "-118.4470263");
//    DeliveryRequest B("Sardines", b);
//    vector<DeliveryRequest> dr;
//    dr.push_back(B);
//    vector<DeliveryCommand> commands;
//    DeliveryPlanner deliPlanner(&m);
//    deliPlanner.generateDeliveryPlan(depot, dr, commands, distanceTravelled);
//    cout << distanceTravelled << endl;
//    for(int i = 0; i < commands.size(); i++){
//        cout << commands[i].description() << endl;
//    }
//
//    cout << "=================\n" << endl;
//
//    vector<DeliveryRequest> deli;
//    deli.push_back(a);
//    deli.push_back(c);
//    deli.push_back(x);
////    deli.push_back(y);
////    deli.push_back(z);
//
//    cout << "Original delivery order:\n" << deli[0].item << " " << deli[1].item << " " << deli[2].item << " " << deli[3].item << " " << deli[4].item <<endl << endl;
//    del.optimizeDeliveryOrder(p, deli, oldDis, newDis);
//    cout << "Optimized delivery order:\n" << deli[0].item << " " << deli[1].item << " " << deli[2].item << " " << deli[3].item << " " << deli[4].item <<endl;
//
//    cout << "old crow dist: \n" <<oldDis << endl << "new crow dist: \n" << newDis << " after delivery optimization\n\n" << "number of deliveries: " << deli.size()<< endl << endl;
//
//
//    vector<DeliveryCommand> dc;
//    cout << "===================================\n" << endl;
//    distanceTravelled = 0;
////     generate delivery plan
//    DeliveryPlanner dp(&m);
//    dp.generateDeliveryPlan(p, deli, dc, distanceTravelled);
//    cout << "=====\n" << endl;
//
//    cout << distanceTravelled << endl;
//    for(int i = 0; i < dc.size(); i++){
//        cout << dc[i].description() << endl;
//    }
//}

//
//#include "provided.h"
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <string>
//#include <vector>
//#include "ExpandableHashMap.h"
//#include <functional>
//using namespace std;
//
//
//int main(int argc, const char * argv[]) {
//    StreetMap m;
//    if(m.load("/Users/tang/Desktop/skeleton/mapdata.txt"))
//        cerr << "Load success..." << endl;
//
//    GeoCoord p("34.0547000", "-118.4794734");
//    //10 works
//    GeoCoord q("34.0549825", "-118.4795629");
//    //11 not working
//    GeoCoord e("34.0552554", "-118.4796392");
//    //12 works
//    GeoCoord f("34.0558289", "-118.4798296");
//    //13 works
//    GeoCoord w("34.0732851", "-118.4931016");
//    GeoCoord r("34.0724746", "-118.4923463");
//    // Glenmere way
//    GeoCoord c("34.0625329", "-118.4470263");
//    GeoCoord g("34.0712323", "-118.4505969");
//
//    GeoCoord z("34.0480886", "-118.3814353");
//    GeoCoord y("34.0436968", "-118.4800519");
//    GeoCoord x("34.0721826", "-118.4435144");
//
//    list<StreetSegment> route;
//    double distanceTravelled = 0.0;
//    PointToPointRouter router(&m);
//    DeliveryResult d = router.generatePointToPointRoute(p, c, route, distanceTravelled);
//    cerr << d << endl;
//    for(auto it = route.begin(); it!=route.end(); it++){
//        cout << it->name << endl;
//        cout << it->start.latitudeText << " " << it->start.longitudeText << endl;
//    }
//    cout << distanceTravelled << endl;
//    DeliveryPlanner dp(&m);
//}




#include "provided.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

bool loadDeliveryRequests(string deliveriesFile, GeoCoord& depot, vector<DeliveryRequest>& v);
bool parseDelivery(string line, string& lat, string& lon, string& item);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " mapdata.txt deliveries.txt" << endl;
        return 1;
    }

    StreetMap sm;

    if (!sm.load(argv[1]))
    {
        cout << "Unable to load map data file " << argv[1] << endl;
        return 1;
    }

    GeoCoord depot;
    vector<DeliveryRequest> deliveries;
    if (!loadDeliveryRequests(argv[2], depot, deliveries))
    {
        cout << "Unable to load delivery request file " << argv[2] << endl;
        return 1;
    }

    cout << "Generating route...\n\n";

    DeliveryPlanner dp(&sm);
    vector<DeliveryCommand> dcs;
    double totalMiles;
    DeliveryResult result = dp.generateDeliveryPlan(depot, deliveries, dcs, totalMiles);
    if (result == BAD_COORD)
    {
        cout << "One or more depot or delivery coordinates are invalid." << endl;
        return 1;
    }
    if (result == NO_ROUTE)
    {
        cout << "No route can be found to deliver all items." << endl;
        return 1;
    }
    cout << "Starting at the depot...\n";
    for (const auto& dc : dcs)
        cout << dc.description() << endl;
    cout << "You are back at the depot and your deliveries are done!\n";
    cout.setf(ios::fixed);
    cout.precision(2);
    cout << totalMiles << " miles travelled for all deliveries." << endl;
}

bool loadDeliveryRequests(string deliveriesFile, GeoCoord& depot, vector<DeliveryRequest>& v)
{
    ifstream inf(deliveriesFile);
    if (!inf)
        return false;
    string lat;
    string lon;
    inf >> lat >> lon;
    inf.ignore(10000, '\n');
    depot = GeoCoord(lat, lon);
    string line;
    while (getline(inf, line))
    {
        string item;
        if (parseDelivery(line, lat, lon, item))
            v.push_back(DeliveryRequest(item, GeoCoord(lat, lon)));
    }
    return true;
}

bool parseDelivery(string line, string& lat, string& lon, string& item)
{
    const size_t colon = line.find(':');
    if (colon == string::npos)
    {
        cout << "Missing colon in deliveries file line: " << line << endl;
        return false;
    }
    istringstream iss(line.substr(0, colon));
    if (!(iss >> lat >> lon))
    {
        cout << "Bad format in deliveries file line: " << line << endl;
        return false;
    }
    item = line.substr(colon + 1);
    if (item.empty())
    {
        cout << "Missing item in deliveries file line: " << line << endl;
        return false;
    }
    return true;
}
