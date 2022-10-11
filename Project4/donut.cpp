#include <getopt.h>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <math.h>
#include <algorithm>

#include <limits>

using namespace std;


struct DonutShop
{
    int x;
    int y;
    char country;
};

struct Node
{
    double dv = numeric_limits<double>::infinity();
    bool kv = false;
    unsigned int pv = 0;
};

struct MST
{
    vector<Node> primsVec;
    double totalWeight;
    vector<int> pathFromFASTTSP;
    
    double partialMSTTotalWeight;
    double upperBound;
    double actualCost;
    vector<int> bestPath;
    
    vector<vector<double>> distanceMatrix;
    
    double runningWeight = 0.0;
    
    void computeMST(vector<DonutShop>& donutShops, unsigned int& numOfShops);
    void getTotalWeight(unsigned int& numOfShops);
    void printMST(unsigned int& numOfShops);
    
    void computeFASTTSP(vector<DonutShop>& donutShops, unsigned int& numOfShops);
    void getTotalWeightFASTTSP(vector<DonutShop>& donutShops, unsigned int& numOfShops);
    void printFASTTSP(unsigned int& numOfShops);
    
    void setDistanceMatrix(vector<DonutShop>& donutShops, unsigned int& numOfShops);
    
    void computeMSTUnvisited(unsigned int& numOfFixed, vector<int>& bestPathSoFar, unsigned int& numOfShops);
    void computeOPTTSP(unsigned int& numOfShops);
    void printOPTTSP(unsigned int& numOfShops);
    void genPerms(vector<int>& path, unsigned int permLength, unsigned int numOfShops);
    bool promising(unsigned int numOfFixed, vector<int>& bestPathSoFar, unsigned int& numOfShops, double& runningWeight);
    double computeEdges(unsigned int numOfFixed, vector<int>& bestPathSoFar, unsigned int& numOfShops);
    
};

string getMode(int argc, char* argv[])
{
    string mode;
    
    opterr = true;
    int choice;
    int option_index = 0;
    
    option long_options[] = {
        { "help",    no_argument,       nullptr, 'h'},
        { "mode",   required_argument,  nullptr, 'm'},
        { nullptr, 0,                   nullptr, '\0' }
    };
    
    while ((choice = getopt_long(argc, argv, "hm:", long_options, &option_index)) != -1) {
        switch (choice) {
            case 'h':
                cout << "Time to Eat the Donuts!\n";
                exit(0);
            case 'm':{
                mode = optarg;
                if ((mode!="MST") && (mode!="FASTTSP") && (mode!="OPTTSP")){
                    cerr << "Wrong mode input.\n";
                    exit(1);
                }
            }
                break;
                
            default:
                cerr << "Error: invalid option\n";
                exit(1);
        }
    }
    return mode;
}

double calculateDistance(int x1, int y1, int x2, int y2)
{
    double x = (double)(x1 - x2);
    double y = (double)(y1 - y2);
    double dist;
    
    dist = sqrt(x*x + y*y);
    
    return dist;
}

double triangleDistance(int x1, int y1, int x2, int y2, int x3, int y3)
{
    return calculateDistance(x1, y1, x3, y3) +
    calculateDistance(x3, y3, x2, y2) -
    calculateDistance(x1, y1, x2, y2);
}


int main(int argc, char* argv[])
{
    cout << std::setprecision(2);
    cout << std::fixed;
    
    string mode = getMode(argc, argv);
    
    
    unsigned int numOfShops;
    
    vector<DonutShop> donutShops;
    
    bool shopInUS = false;
    bool shopInCanada = false;
    bool shopInBorder = false;
    
    cin >> numOfShops;
    donutShops.reserve(numOfShops);
    for(unsigned int i=0; i<numOfShops; ++i)
    {
        DonutShop donutShopInput;
        cin >> donutShopInput.x;
        cin >> donutShopInput.y;
        if((donutShopInput.x>0 && donutShopInput.y==0) ||
           (donutShopInput.x==0 && donutShopInput.y>0) ||
           (donutShopInput.x==0 && donutShopInput.y==0) )
        {
            donutShopInput.country = 'b';
            shopInBorder = true;
        }
        else if(donutShopInput.x>0 && donutShopInput.y>0)
        {
            donutShopInput.country = 'c';
            shopInCanada = true;
        }
        else
        {
            donutShopInput.country = 'u';
            shopInUS = true;
        }
        donutShops.push_back(donutShopInput);
    }
    
    if(mode == "MST")
    {
        if(shopInUS && shopInCanada)
        {
            if(!shopInBorder)
            {
                cerr << "Cannot construct MST\n";
                exit(1);
            }
        }
        
        MST mst;
        mst.computeMST(donutShops, numOfShops);
        mst.getTotalWeight(numOfShops);
        mst.printMST(numOfShops);
        
        
    }
    else if(mode == "FASTTSP")
    {
        MST mst;
        mst.computeFASTTSP(donutShops, numOfShops);
        mst.getTotalWeightFASTTSP(donutShops, numOfShops);
        mst.printFASTTSP(numOfShops);
    }
    else if(mode == "OPTTSP")
    {
        MST mst;
        mst.setDistanceMatrix(donutShops, numOfShops);
        mst.computeFASTTSP(donutShops, numOfShops);
        mst.getTotalWeightFASTTSP(donutShops, numOfShops);
        mst.computeOPTTSP(numOfShops);
        mst.printOPTTSP(numOfShops);
        
    }
    
    
    
    return 0;
}

// --------------------------------------------------------------------
//                    MST struct Definitions
// --------------------------------------------------------------------


void MST::computeMST(vector<DonutShop>& donutShops, unsigned int& numOfShops)
{
    // unsigned int numOfShops = donutShops.size();
    primsVec.reserve(numOfShops);
    
    for(unsigned int i=0; i<numOfShops; i++)
    {
        primsVec[i].dv = numeric_limits<double>::infinity();
        primsVec[i].kv = false;
        primsVec[i].pv = 0;
    }
    
    
    primsVec[0].dv = 0;
    primsVec[0].kv = true;
    
    
    for(unsigned int i=1;i<numOfShops;i++)
    {
        if( (donutShops[0].country == 'b') ||
           (donutShops[i].country == 'b') ||
           (donutShops[0].country == donutShops[i].country) )
        {
            primsVec[i].dv = calculateDistance(donutShops[0].x, donutShops[0].y,
                                               donutShops[i].x, donutShops[i].y);
        }
        
        
    }
    
    
    
    for(unsigned int j=1;j<numOfShops;j++)
    {
        int shortestIndex = 0;
        double shortestDist = numeric_limits<double>::infinity();
        
        for(unsigned int i=0;i<numOfShops;i++)
        {
            
            if((primsVec[i].kv == false) && (primsVec[i].dv < shortestDist))
            {
                shortestIndex = i;
                shortestDist = primsVec[i].dv;
            }
        }
        
        
        
        primsVec[shortestIndex].kv = true;
        for(unsigned int i=0;i<numOfShops;i++)
        {
            
            if(primsVec[i].kv == false &&
               primsVec[i].dv > calculateDistance(donutShops[shortestIndex].x, donutShops[shortestIndex].y, donutShops[i].x, donutShops[i].y))
            {
                if((donutShops[shortestIndex].country == 'b') ||
                   (donutShops[i].country == 'b') ||
                   (donutShops[shortestIndex].country == donutShops[i].country))
                {
                    primsVec[i].dv = calculateDistance(donutShops[shortestIndex].x, donutShops[shortestIndex].y, donutShops[i].x, donutShops[i].y);
                    primsVec[i].pv = shortestIndex;
                    
                }
                
            }
            
        }
        
    }
    
}

void MST::getTotalWeight(unsigned int& numOfShops)
{
    totalWeight = 0.0;
    for(unsigned int i=0; i<numOfShops; i++)
    {
        totalWeight += primsVec[i].dv;
    }
}

void MST::printMST(unsigned int& numOfShops)
{
    cout << totalWeight << '\n';
    for(unsigned int i=1; i<numOfShops; i++)
    {
        if (primsVec[i].pv < i)
        {
            cout << primsVec[i].pv << ' ' << i << '\n';
        }
        else
        {
            cout << i << ' ' << primsVec[i].pv << '\n';
        }
    }
}

void MST::computeFASTTSP(vector<DonutShop>& donutShops, unsigned int& numOfShops)
{
    
    vector<bool> isVisited(numOfShops, false);
    double shortestDist = numeric_limits<double>::infinity();
    int shortestIndex = 0;
    isVisited[0] = true;
    pathFromFASTTSP.push_back(0);
    for(unsigned int i=1;i<numOfShops;i++)
    {
        if(calculateDistance(donutShops[0].x, donutShops[0].y,
                             donutShops[i].x, donutShops[i].y) < shortestDist)
        {
            shortestDist = calculateDistance(donutShops[0].x, donutShops[0].y,
                                             donutShops[i].x, donutShops[i].y);
            shortestIndex = i;
        }
    }
    
    pathFromFASTTSP.push_back(shortestIndex);
    pathFromFASTTSP.push_back(0);
    
    isVisited[shortestIndex] = true;
    
    unsigned int numOfShopsVisited = 2;
    
    while(numOfShopsVisited != numOfShops)
    {
        
        
        int randomIndex = rand() % numOfShops;
        while(isVisited[randomIndex]==true)
        {
            randomIndex = rand() % numOfShops;
        }
        
        int insertIndex;
        double minCost = numeric_limits<double>::infinity();
        double currentCost;
        for(unsigned int p=0; p<numOfShopsVisited; p++)
        {
            currentCost = triangleDistance(donutShops[pathFromFASTTSP[p]].x, donutShops[pathFromFASTTSP[p]].y,
                                           donutShops[pathFromFASTTSP[p+1]].x, donutShops[pathFromFASTTSP[p+1]].y,
                                           donutShops[randomIndex].x, donutShops[randomIndex].y);
            if(currentCost<minCost)
            {
                minCost = currentCost;
                insertIndex = p+1;
            }
        }
        
        pathFromFASTTSP.insert(pathFromFASTTSP.begin() + insertIndex, randomIndex);
        
        isVisited[randomIndex] = true;
        numOfShopsVisited++;
        
    }
    
}

void MST::getTotalWeightFASTTSP(vector<DonutShop>& donutShops, unsigned int& numOfShops)
{
    totalWeight = 0.0;
    
    for(unsigned int i=0; i<numOfShops; i++)
    {
        totalWeight+=calculateDistance(donutShops[pathFromFASTTSP[i]].x,
                                       donutShops[pathFromFASTTSP[i]].y,
                                       donutShops[pathFromFASTTSP[i+1]].x,
                                       donutShops[pathFromFASTTSP[i+1]].y);
    }
}

void MST::printFASTTSP(unsigned int& numOfShops)
{
    cout << totalWeight << '\n';
    cout << pathFromFASTTSP[0];
    for(unsigned int i=1; i<numOfShops; i++)
    {
        cout << " " << pathFromFASTTSP[i];
    }
    cout << '\n';
}



void MST::setDistanceMatrix(vector<DonutShop>& donutShops, unsigned int& numOfShops)
{
    distanceMatrix.reserve(numOfShops);
    while(distanceMatrix.size() < numOfShops)
    {
        vector<double> temp(numOfShops, 0);
        distanceMatrix.push_back(temp);
    }
    
    for(unsigned int i=0; i<numOfShops; i++)
    {
        for(unsigned int j=0; j<numOfShops; j++)
        {
            distanceMatrix[i][j] = calculateDistance(donutShops[i].x, donutShops[i].y, donutShops[j].x, donutShops[j].y);
        }
    }
}



void MST::computeMSTUnvisited(unsigned int& numOfFixed, vector<int>& bestPathSoFar, unsigned int& numOfShops)
{
    unsigned int numOfUnvisited = numOfShops - numOfFixed;
    primsVec.reserve(numOfUnvisited);
    
    for(unsigned int i=0; i<numOfUnvisited; i++)
    {
        primsVec[i].dv = numeric_limits<double>::infinity();
        primsVec[i].kv = false;
        primsVec[i].pv = 0;
    }
    
    primsVec[0].dv = 0;
    primsVec[0].kv = true;
    
    for(unsigned int i=1; i<numOfUnvisited; i++)
    {
        primsVec[i].dv = distanceMatrix[bestPathSoFar[numOfFixed]][bestPathSoFar[numOfFixed+i]];
    }
    
    for(unsigned int j=1; j<numOfUnvisited; j++)
    {
        int shortestIndex = 0;
        double shortestDist = numeric_limits<double>::infinity();
        for(unsigned int i=0; i<numOfUnvisited; i++)
        {
            if((primsVec[i].kv == false) && (primsVec[i].dv < shortestDist))
            {
                shortestIndex = i;
                shortestDist = primsVec[i].dv;
            }
        }
        primsVec[shortestIndex].kv = true;
        
        for(unsigned int i=0; i<numOfUnvisited; i++)
        {
            if(primsVec[i].kv == false &&
               primsVec[i].dv > distanceMatrix[bestPathSoFar[shortestIndex + numOfFixed]][bestPathSoFar[numOfFixed + i]])
            {
                primsVec[i].dv = distanceMatrix[bestPathSoFar[shortestIndex + numOfFixed]][bestPathSoFar[numOfFixed + i]];
                primsVec[i].pv = shortestIndex;
            }
        }
    }
    
    partialMSTTotalWeight = 0.0;
    
    for(unsigned int i=0; i<numOfUnvisited; i++)
    {
        partialMSTTotalWeight += primsVec[i].dv;
    }
    
    
    
    
}

void MST::computeOPTTSP(unsigned int& numOfShops)
{
    upperBound = totalWeight;
    bestPath = pathFromFASTTSP;
    pathFromFASTTSP.pop_back();
    
    
    
    genPerms(pathFromFASTTSP, 1, numOfShops);
    
    
}

void MST::printOPTTSP(unsigned int& numOfShops)
{
    cout << upperBound << '\n';
    cout << bestPath[0];
    for(unsigned int i=1; i<numOfShops; i++)
    {
        cout << " " << bestPath[i];
    }
    cout << '\n';
}

void MST::genPerms(vector<int>& path, unsigned int permLength, unsigned int numOfShops)
{
    
    
    if (path.size() == permLength)
    {
        
        if(runningWeight + distanceMatrix[path[0]][path[permLength-1]] < upperBound)
        {
            upperBound = runningWeight + distanceMatrix[path[0]][path[permLength-1]];
            bestPath = path;
        }
        return;
    }
    if (!promising(permLength, path, numOfShops, runningWeight))
        return;
    for (size_t i = permLength; i < path.size(); ++i)
    {
        swap(path[permLength], path[i]);
        runningWeight += distanceMatrix[path[permLength]][path[permLength-1]];
        genPerms(path, permLength + 1, numOfShops);
        runningWeight -= distanceMatrix[path[permLength]][path[permLength-1]];
        swap(path[permLength], path[i]);
    }
}

bool MST::promising(unsigned int numOfFixed, vector<int>& bestPathSoFar, unsigned int& numOfShops, double& runningWeight)
{
    
    unsigned int k = numOfShops - numOfFixed;
    if (k < 5)
    {
        return true;
    }
    
    computeMSTUnvisited(numOfFixed, bestPathSoFar, numOfShops);
    
    if(partialMSTTotalWeight + runningWeight + computeEdges(numOfFixed, bestPathSoFar, numOfShops) < upperBound)
    {
        return true;
    }
    else
    {
        return false;
    }
}

double MST::computeEdges(unsigned int numOfFixed, vector<int>& bestPathSoFar, unsigned int& numOfShops)
{
    double edge1 = numeric_limits<double>::infinity();
    double edge2 = numeric_limits<double>::infinity();
    
    if (bestPathSoFar.empty())
    {
        return 0;
    }
    for (unsigned int i=numOfFixed; i<numOfShops; i++)
    {
        double d1 = distanceMatrix[0][bestPathSoFar[i]];
        double d2 = distanceMatrix[bestPathSoFar[numOfFixed-1]][bestPathSoFar[i]];
        
        if (d1 < edge1)
        {
            edge1 = d1;
        }
        if (d2 < edge2)
        {
            edge2 = d2;
        }
    }
    return edge1 + edge2;
}
