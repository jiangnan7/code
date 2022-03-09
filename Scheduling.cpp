#include "../header/Scheduling.h"
#include "../header/Common.h"


using namespace std; 

namespace HWCGRA
{

unordered_map<string, size_t> Scheduling::computeASAP()
{   
    unordered_map<string, bool>         usedVertexDFG; 
    unordered_map<string, size_t>       dfgASAP; 
    vector<string,int> num;
    vector<string> vertices; 

    for(const auto &vertex: _DFG.vertices())
    {
        vertices.push_back(vertex.first); 
        usedVertexDFG[vertex.first] = false;
        dfgASAP[vertex.first] = -1;
    }

    size_t num_scheduled = 0;

    for(auto vertice: vertices)
    {   
        string tmp = vertice;
        //if(vertice.degreesIn.size())
        if(_DFG.edgesIn(tmp).size() == 0)
        {
            dfgASAP[tmp] = num_scheduled;
            usedVertexDFG[tmp] = true;
            num_scheduled++;
        }
    }
    random_shuffle(vertices.begin(),vertices.end());
    size_t max_latency = 0;
                        //bool predSched = usedVertexDFG[tmp];
    while(num_scheduled <= _DFG.vertices().size())
    {
        for(auto vertice: vertices)
        {   
            string tmp = vertice;
            if(!usedVertexDFG[tmp])
            {
                cout << "tmp: "  << tmp <<endl;
                string portsNameDFG = getPostfix(tmp);
                if(!portsNameDFG.empty())
                {
                    for(const auto &edge: _DFG.edgesIn(tmp))
                    {
                        if(usedVertexDFG[edge.from()])
                        {
                            dfgASAP[tmp] = num_scheduled;
                            num_scheduled++;
                            usedVertexDFG[tmp] = true;
                        }
                        if(portNameDFG.find("in")!=portNameDFG.npos)
                        {
                            if(getPrefix(edge.from()) == getPrefix(edge.to()))
                            {
                                if(!usedVertexDFG[edge.from()])
                                {
                                    dfgASAP[edge.to()] = num_scheduled;
                                    num_scheduled++;
                                    usedVertexDFG[edge.to()] = true;
                                }

                            }
                        }
                    }
                }
                else
                {
                    bool flag = false;
                    for(const auto &edge: _DFG.edgesIn(tmp))
                    {
                        if(!usedVertexDFG[edge.from()])
                        {
                            flag = false;
                            break;
                        }
                            //edgeFrom.push_back(edge.from());
                        flag = true;
                    }   
                    if(flag)
                    {
                        dfgASAP[tmp] = num_scheduled;
                        num_scheduled++;
                        usedVertexDFG[tmp] = true;
                    }
                }
            }
                
                
        }
    }
    cout << "dfgASAP" << dfgASAP << endl;
    sort(dfgASAP.begin(), dfgASAP.end(), [&](const std::string &str1, const std::string &str2){
        return dfgASAP[str1] < dfgASAP[str2]; 
    }); 

    std::vector<std::pair<string, int>> tmp;
    for (auto& i : dfgASAP)
        tmp.push_back(i);
    std::sort(tmp.begin(), tmp.end(), 
          [=](std::pair<string, int>& a, std::pair<string, int>& b) { return a.second < b.second; });
    cout << tmp << endl;

    return dfgASAP;
}

    //             string portNameDFG = getPostfix(tmp);
    //             unordered_set<string> portsNameDFG; 
    //             if(!portNameDFG.empty())
    //             {
    //                 if(portNameDFG.find("in")!=portNameDFG.npos)
    //                 {
    //                     string fuNameDFG = getPrefix(tmp);
    //                 for(const auto &edge: _DFG.edgesIn(tmp))
    //                 {
    //                     //portsNameDFG.insert(edge.from());
    //                     if(usedVertexDFG[edge.from()])
    //                         {
    //                             dfgASAP[edge.to()] = num_scheduled;
    //                             num_scheduled++;
    //                             usedVertexDFG[edge.to()] = true;
    //                         }
    //                 }    
    //                 cout << "portsNameDFG: " << portsNameDFG << endl;
    //                 bool flag1 = false;
    //                 for(const auto &port: portsNameDFG)
    //                 {   
    //                     //_DFG.edgesIn(port)

    //                     /* for(const auto &edge: _DFG.edgesIn(port))
    //                     {
    //                         if(usedVertexDFG[edge.from()])
    //                         {
    //                             dfgASAP[edge.to()] = num_scheduled;
    //                             num_scheduled++;
    //                             usedVertexDFG[edge.to()] = true;
    //                         }
    //                         cout << "edge: " << edge.from() << endl;
    //                     } */
    //                     // for(const auto &edge: _DFG.edgesIn(port))
    //                     // {
    //                         if(usedVertexDFG[edge.from()])
    //                         {
    //                             dfgASAP[edge.to()] = num_scheduled;
    //                             num_scheduled++;
    //                             usedVertexDFG[edge.to()] = true;
    //                         }
    //                         cout << "edge: " << edge.from() << endl;
    //                     //}
                
    //                 }
    //                 }
    //                 else
    //                 {
    //                     string fuNameDFG = getPrefix(tmp);
    //                     if(usedVertexDFG[fuNameDFG])
    //                     {
    //                         dfgASAP[tmp] = num_scheduled;
    //                         num_scheduled++;
    //                         usedVertexDFG[tmp] = true;
    //                     }
    //                     /* for(const auto &edge: _DFG.edgesOut(fuNameDFG))
    //                     {
    //                         portsNameDFG.insert(edge.to());
    //                     }    
    //                     cout << "portsNameDFG: " << portsNameDFG << endl;
    //                     for(const auto &port: portsNameDFG)
    //                     {   
    //                     //_DFG.edgesIn(port)
    //                     for(const auto &edge: _DFG.edgesIn(port))
    //                     {
    //                         if(usedVertexDFG[edge.from()])
    //                         {
    //                             dfgASAP[tmp] = num_scheduled;
    //                             num_scheduled++;
    //                             usedVertexDFG[tmp] = true;
    //                         }
    //                         cout << "edge: " << edge.from() << endl;
    //                     }
    //                     } */
    //                 }
    //             }
    //             else
    //             {
    //                 bool flag = false;
    //                 vector<string> edgeFrom;
    //                 for(const auto &edge: _DFG.edgesIn(tmp))
    //                 {
    //                     cout << "edge.from()" << edge.from() <<endl;
    //                     if(!usedVertexDFG[edge.from()])
    //                     {
    //                         flag = false;
    //                         break;
    //                     }
    //                         //edgeFrom.push_back(edge.from());
    //                     flag = true;
    //                 }
    //                 if(flag)
    //                 {
    //                     dfgASAP[tmp] = num_scheduled;
    //                     num_scheduled++;
    //                     usedVertexDFG[tmp] = true;
    //                 }
                    
    //             }
    //         //bool predSched = usedVertexDFG[tmp];//usedVertexDFG[tmp] &&
    //             for(const auto &edge: _DFG.edgesIn(tmp))
    //             {
    //                 vector<string> tmp;
    //                 tmp.push_back(edge.from());
    //             }
    //             size_t edge_num = 1;
    //             for(auto edge: tmp)
    //             {
    //                 if(!usedVertexDFG[edge])
    //                     break;
    //                 else
    //                     edge_num++;
    //                 if(edge_num == tmp.size())
    //                     {
    //                         dfgASAP[tmp] = num_scheduled;
    //                         num_scheduled++;
    //                         usedVertexDFG[tmp] = true;
    //                     }
    //                     //if(usedVertexDFG[edge.from()] && !usedVertexDFG[edge.to()])
                        
    //             }
                
    //         //bool predSched = usedVertexDFG[edge.from()];
    //         }
    //     }
    // }
    

    
    std::map<int, int> tmp;
std::transform(m.begin(), m.end(), std::inserter(tmp, tmp.begin()), 
               [](std::pair<int, int> a) { return std::pair<int, int>(a.second, a.first); });

void Scheduling::computeALAP()
{

}
void computeMobility()
{
    
}
}