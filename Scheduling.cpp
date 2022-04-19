#include "../header/Scheduling.h"



using namespace std; 

namespace HWCGRA
{


void Scheduling::getPEinfoWithgetDFGorder(peInfo &peinfo, const string &fuPath)
{
    //string coordPath = fuPath + "coord.txt";
    string coordPath = "./arch/ADRES/coord.txt";
    // for(const auto &rrg: _RRG.vertices())
    // {
    //     cout << rrg.first << endl;

    //     cout << rrg.second.name()<<endl;
    //     string device = rrg.second.getAttr("device").getStr();
    // }
    cout << coordPath ;
    std::string peType;
    std::string pePort;//NULL
    int pe_x;
    int pe_y;
    int pe_time;

    string peName;
    ifstream fin(coordPath); 
    while(!fin.eof())
    {
        string line; 
        getline(fin, line); 
        //cout << "line: " << line << endl;
        if(line.empty())
        {
            continue; 
        }
        istringstream sin(line); 
        string tmp;  sin >> tmp; 
        peName = tmp;
        
        tmp.clear(); sin >> tmp;
        peType = tmp;
        tmp.clear(); sin >> tmp;
        pe_x = atoi(tmp.c_str());
        
        tmp.clear(); sin >> tmp;
        pe_y = atoi(tmp.c_str());
        
        peinfo[peName] = pe_info{peType, .pePort = "null", pe_x, pe_y, .pe_time = 0};
    }
    for(const auto &temp: peinfo)
    {
        cout << "name: " << temp.first << " Type: " << temp.second.peType << " ";
        cout << "port: " << temp.second.pePort << " pe_x: " << temp.second.pe_x << " ";
        cout << "pe_y: " << temp.second.pe_y << " pe_time: " << temp.second.pe_time << " " << endl;
    }
    
    return ;
}




unordered_map<string, size_t> Scheduling::computeASAP()
{   
    unordered_map<string, bool>         usedVertexDFG; 
    unordered_map<string, size_t>       dfgASAP; 

    _DFG.degreesIn();
    vector<string> vertices; 
    

    for(const auto &vertex: _DFG.vertices())
    {
        vertices.push_back(vertex.first); 
        usedVertexDFG[vertex.first] = false;
        dfgASAP[vertex.first] = -1;
    }

    size_t num_scheduled = 0;
    for(const auto &vertice: vertices)
    {   
        //string tmp = vertice;
        //if(vertice.degreesIn.size())
        if(_DFG.edgesIn(vertice).size() == 0)
        {
            dfgASAP[vertice] = num_scheduled;
            usedVertexDFG[vertice] = true;
            num_scheduled++;         
        }
    }
    while(num_scheduled < _DFG.vertices().size())
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
                        cout << "edge.from()1:" << edge.from() << endl;
                        if(usedVertexDFG[edge.from()])
                        {
                            cout << "a"<<endl;
                            dfgASAP[tmp] = num_scheduled;
                            num_scheduled++;
                            usedVertexDFG[tmp] = true;
                        }
                        if(portsNameDFG.find("in")!=portsNameDFG.npos)
                        {
                            if(getPrefix(edge.from()) == getPrefix(edge.to()))
                            {
                                cout<<"edge.from()3: "<<edge.from()<<endl;
                                if(!usedVertexDFG[edge.to()])
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
                        cout<< "edge.from()2"<<edge.from()<<endl;
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
                        cout<<"n"<<endl;
                        dfgASAP[tmp] = num_scheduled;
                        num_scheduled++;
                        usedVertexDFG[tmp] = true;
                    }
                }
            }
                
                
        }
    }
    
    cout << "dfgASAP" << dfgASAP << endl;
        std::vector<std::pair<string, int>> aa;
    for (auto& i : dfgASAP)
        aa.push_back(i);
    std::sort(aa.begin(), aa.end(), 
          [=](std::pair<string, int>& a, std::pair<string, int>& b) { return a.second < b.second; });
    cout << aa<<endl;
    return dfgASAP;

}

unordered_map<string, size_t> Scheduling::computeALAP()
{
    //TODO:Latency
    unordered_map<string, bool>       usedVertexDFG;
    unordered_map<string, size_t>     dfgALAP;
    vector<string> vertices; 

    for(const auto &vertex: _DFG.vertices())
    {
        vertices.push_back(vertex.first); 
        usedVertexDFG[vertex.first] = false;
        dfgALAP[vertex.first] = -1;
    }

    size_t num_scheduled = _DFG.vertices().size();
    
    vector<string> criticalPath;
    vector<string> temps;
    string zhongdian;
    for(const auto &vertice: vertices)
    {   
        //string tmp = vertice;
        //if(vertice.degreesIn.size())
        if(_DFG.edgesOut(vertice).size() == 0)//edgesOut(a) edge.from = a;
        {
            dfgALAP[vertice] = num_scheduled;
            usedVertexDFG[vertice] = true;
            num_scheduled--;
            zhongdian = vertice;
        }
        if(_DFG.edgesIn(vertice).size() == 0)//edgeIn(a) edge.to = a;
        { 
            temps.push_back(vertice);      
        }
    }
    vector<string> criticalPathTemp;
    for(const auto temp: temps)
    {
        criticalPathTemp = _DFG.findPathDFS(temp,zhongdian);
        if(criticalPathTemp.size() > criticalPath.size())
            criticalPath = criticalPathTemp;
    }
    dfgALAP[criticalPath[0]] = 1;
    usedVertexDFG[criticalPath[0]] = true;
    
    while(num_scheduled > 1)
    {
        cout << usedVertexDFG << endl;
        cout << dfgALAP<<endl;
        for(const auto &vertice: vertices)
        {
            if(!usedVertexDFG[vertice])
            {
                cout << "vertice: " << vertice << endl;
                string portNameDFG = getPostfix(vertice);
                if(portNameDFG.empty())
                {
                    bool flag = false;
                    for(const auto &edge: _DFG.edgesOut(vertice))
                    {
                        if(!usedVertexDFG[edge.to()])
                        {
                            flag = false;
                            break;
                        }
                        flag = true;
                    }
                    if(flag)
                    {
                        dfgALAP[vertice] = num_scheduled;
                        usedVertexDFG[vertice] = true;
                        num_scheduled--;
                    }
                }
                else
                {
                    for(const auto &edge: _DFG.edgesOut(vertice))
                    {
                        if(usedVertexDFG[edge.to()])
                        {
                            dfgALAP[vertice] = num_scheduled;
                            usedVertexDFG[vertice] = true;
                            num_scheduled--;
                        }
                        
                    }
                }
            }
        }
    }
    cout << "dfgALAP" << dfgALAP << endl;
    std::vector<std::pair<string, int>> aa;
    for (auto& i : dfgALAP)
        aa.push_back(i);
    std::sort(aa.begin(), aa.end(), 
          [=](std::pair<string, int>& a, std::pair<string, int>& b) { return a.second < b.second; });
    cout << aa<<endl;
    return dfgALAP;
    
}




void Scheduling::computePriority(const unordered_map<string, unordered_set<string>> &compatFUs)
{
    unordered_map<string, size_t>   dfgASAP = this->computeASAP();
    unordered_map<string, size_t>   dfgALAP = this->computeALAP();

    vector<string> orderDFG;
    vector<string> orderasap;
    vector<string> orderalap;
    std::vector<std::pair<string, int>> orderASAP;
    std::vector<std::pair<string, int>> orderALAP;

    for (auto& i : dfgASAP)
        orderASAP.push_back(i);
    std::sort(orderASAP.begin(), orderASAP.end(), 
          [=](std::pair<string, int>& a, std::pair<string, int>& b) { return a.second < b.second; });
    for (auto& i : dfgALAP)
        orderALAP.push_back(i);
    std::sort(orderALAP.begin(), orderALAP.end(), 
          [=](std::pair<string, int>& a, std::pair<string, int>& b) { return a.second < b.second; });


    unordered_map<string, int> mobility;

    size_t num_ASAP = 0;
    size_t num_ALAP = 0;

    assert(orderALAP.size() == orderASAP.size());
    for(size_t i = 0; i < orderALAP.size(); i++)
    {
        if(compatFUs.find(orderALAP[i].first) != compatFUs.end())
            orderalap.push_back(orderALAP[i].first);
        if(compatFUs.find(orderASAP[i].first) != compatFUs.end())
            orderasap.push_back(orderASAP[i].first); 
    }
    cout << "orderalap: " << orderalap << endl;
    cout << "orderasap: " << orderasap << endl;
    for(const auto &dfg: orderalap)
    {
        size_t alap = find(orderalap.begin(),orderalap.end(),dfg)-orderalap.begin();
        size_t asap = find(orderasap.begin(),orderasap.end(),dfg)-orderasap.begin();
        mobility[dfg] = abs(int(alap-asap));
    }
    std::vector<std::pair<string, int>> order;
    for (auto& i : mobility)
        order.push_back(i);
    std::sort(order.begin(), order.end(), 
          [=](std::pair<string, int>& a, std::pair<string, int>& b) { return a.second < b.second; });
    cout << endl;
    cout << "order: " << order << endl;


}


//schedule
vector<string> Scheduling::computePriority(dfgInfo &dfginfo,const int &backII)
{

    unordered_map<string, bool>         usedVertexDFG;
    vector<string> VertexDFG;
    std::vector<std::pair<string, int>> orderASAP = this->Schedule_ASAP();
    std::vector<std::pair<string, int>> orderALAP = this->Schedule_ALAP();

    unordered_map<std::string, int> toposort = this->topoSort();
    unordered_map<size_t, unordered_set<string>> toposet;

    auto get_node_id = [](std::string str){
        int i =0;
        std::string final_str = "";
        for ( ; i < str.length(); i++ ){ if ( isdigit(str[i]) ) final_str+= str[i]; }
        return  atoi(final_str .c_str());
    };

    for(const auto &dfgalap: orderALAP)
    {
        int id = get_node_id(dfgalap.first);
        usedVertexDFG[dfgalap.first] = false;
        //VertexDFG.push_back(dfgalap.first);
        vector<std::pair<string, int>>::iterator iter;
        for(iter = orderASAP.begin(); iter != orderASAP.end(); iter++)
        {
            if(dfgalap.first == iter->first)
            {
                //orderdfg.push_back(pair<string, int>(dfgalap.first, dfgalap.second - iter->second));
                //back_orderASAP.erase(iter);
                dfginfo.emplace(id, dfg_info{iter->second, dfgalap.second, dfgalap.second - iter->second, toposort[dfgalap.first]});
                //dfginfo.emplace(id, dfg_info{.alap = 1, .asap =2});
                break;
            }
        }       
    }
    
    for(const auto &dfg: dfginfo)
    {
        cout << "dfg_id: " << dfg.first << " " << dfg.second.alap << " " << dfg.second.asap << " " << dfg.second.mobility << " " << dfg.second.topolevel << endl;
    }
    for(const auto &topo: toposort)
    {
        if(toposet.find(topo.second) == toposet.end())
        {
            toposet[topo.second] = unordered_set<string>(); 
        }
        toposet[topo.second].insert(topo.first); 
    }
    // for(const auto &topo: toposet)
    // {
    //     cout << topo << endl;
    // }

    vector<string> try_dfg;
    //set start
    for(const auto &dfg: _DFG.vertices())
    {
        int id = get_node_id(dfg.first);
        if(dfginfo.at(id).mobility == 0 && dfginfo.at(id).topolevel ==0)
        {
            try_dfg.push_back(dfg.first);
            usedVertexDFG[dfg.first] = true;
        }
    }
    // for(const auto &order: orderdfg)
    // {
    //     if(order.second == 0 && toposet[0].find(order.first) != toposet[0].end())
    //     {
    //         try_dfg.push_back(order.first);
    //         usedVertexDFG[order.first] = true;
    //     }
    // }


    string totry = try_dfg.front();
    vector<string> currentDFGpred;
    
    vector<string> toTry = {totry};

    while(try_dfg.size() < orderASAP.size())
    {
        vector<string> currentDFGsucc = getSucc(toTry.front());
        // vector<string>::iterator s = find(currentDFGsucc.begin(), currentDFGsucc.end(), toTry.front());
        // currentDFGsucc.erase(s);
        totry = toTry.front();
        toTry.erase(toTry.begin());
        if(currentDFGsucc.empty())
        {
            vector<string> otherDFG;
            for(const auto &dfg: usedVertexDFG)
            {
                if(!dfg.second)
                {
                    otherDFG.push_back(dfg.first);
                    toTry.push_back(dfg.first);
                }
            }
            random_shuffle(otherDFG.begin(), otherDFG.end()); 
            cout << otherDFG << "a" << endl;
            vector<string> currentotherDFGsucc = getSucc(otherDFG.front());
            if(usedVertexDFG[currentotherDFGsucc.front()])
            {
                vector<string>::iterator it = find(try_dfg.begin(), try_dfg.end(), currentotherDFGsucc.front());
                try_dfg.insert(it, otherDFG.front());
                usedVertexDFG[otherDFG.front()] = true;
            }
      
            otherDFG.erase(otherDFG.begin());
            
        }

        //sort + delete
        vector<string> orderDFGsucc;

        vector<string> backDFGsucc = currentDFGsucc;
            
            for(const auto &dfgsucc: backDFGsucc)//guan jian lu jing
            {
                vector<std::pair<string, int>>::iterator iter;
                for(iter = orderdfg.begin(); iter != orderdfg.end(); iter++)
                {
                    int dfgsucclevel;
                    int totry_topo;
                    for(const auto &topo: toposort)
                    {
                        if(topo.first == dfgsucc)
                            dfgsucclevel = topo.second;
                        if(topo.first == totry)
                            totry_topo = topo.second;
                    }

                    if(iter->first == dfgsucc && iter->second == 0 && (dfgsucclevel - totry_topo) <= 1)
                    {
                        orderDFGsucc.push_back(dfgsucc);
                        vector<string>::iterator s = find(currentDFGsucc.begin(), currentDFGsucc.end(), dfgsucc);
                        currentDFGsucc.erase(s);
                    }
                }
                
            }
            if(!currentDFGsucc.empty())
            {
                vector<std::pair<string, int>> backDFGtopo;
                int totry_topo;
                for(const auto &dfgsucc : currentDFGsucc)
                {
                    vector<std::pair<string, int>>::iterator order;
                    for(order = toposort.begin(); order != toposort.end(); order++)
                    {
                        if(order->first == totry)
                            totry_topo = order->second;
                        if(order->first == dfgsucc)
                            backDFGtopo.push_back(pair<string, int> (dfgsucc, order->second));
                    }
                }
                for(const auto &dfgsucc: backDFGtopo)
                {
                    if((dfgsucc.second - totry_topo) <= 1)
                    {
                        orderDFGsucc.push_back(dfgsucc.first);
                        vector<string>::iterator s = find(currentDFGsucc.begin(), currentDFGsucc.end(), dfgsucc.first);
                        currentDFGsucc.erase(s);
                    }
                    else
                    {
                        vector<string>::iterator s = find(currentDFGsucc.begin(), currentDFGsucc.end(), dfgsucc.first);
                        currentDFGsucc.erase(s);
                    }
                }
            }
        
        currentDFGsucc = orderDFGsucc;
        
        for(const auto &currentdfgsucc: currentDFGsucc)
        {
            currentDFGpred = getPred(currentdfgsucc);
            int currentdfgsucc_level;
            for(const auto &topo: toposort)
            {
                if(topo.first == currentdfgsucc)
                    currentdfgsucc_level= topo.second;
            }
            int currentdfgpred_level;
            for(const auto &currentdfgpred: currentDFGpred)
            {
                if(currentdfgsucc == currentdfgpred)
                    continue;

                for(const auto &topo: toposort)
                {
                    if(topo.first == currentdfgpred)
                         currentdfgpred_level= topo.second;
                }
                if(currentdfgsucc_level - currentdfgpred_level >= 0)
                {
                    if(!usedVertexDFG[currentdfgpred])
                    {
                         usedVertexDFG[currentdfgpred] = true;
                        //  vector<string>::iterator dfgpred = find(try_dfg.begin(), try_dfg.end(),currentdfgpred);
                        // if(dfgpred == try_dfg.end())
                            try_dfg.push_back(currentdfgpred);
                         cout << "currentdfgpred: " << currentdfgpred << endl;
                    }
                }
            }
            cout << "currentdfgsucc: " << currentdfgsucc << endl;
            if(usedVertexDFG[currentdfgsucc])
                continue;;
            usedVertexDFG[currentdfgsucc] = true;
            vector<string>::iterator dfgsucc = find(try_dfg.begin(), try_dfg.end(),currentdfgsucc);
            if(dfgsucc == try_dfg.end())
                try_dfg.push_back(currentdfgsucc);

            toTry.push_back(currentdfgsucc);

        }
        cout << try_dfg << endl;
        
    }
    cout << "Order: "<< endl;
    cout << try_dfg << endl;

    return try_dfg;

    std::sort(order.begin(), order.end(), 
          [=](std::pair<string, int>& a, std::pair<string, int>& b) { return a.second < b.second; });
    cout << order << endl;

}

// Schedule operations as soon as all predecessors are completed
vector<std::pair<string, int>> Scheduling::Schedule_ASAP()
{
    clog << "ASAP Started." << endl;
    
    vector<string> scheduled;
    vector<string> to_be_scheduled;
    unordered_map<string, size_t> dfgASAP;
    unordered_map<string, bool>   usedVertexDFG; 
    vector<string> vertices; 

    size_t latest_time = 0;
    size_t schedule_time = 0;
    for(const auto &vertex: _DFG.vertices())
    {
        vertices.push_back(vertex.first);
        to_be_scheduled.push_back(vertex.first); 
        usedVertexDFG[vertex.first] = false;
    }
    for(const auto &vertex: vertices)
    {
        if(_DFG.edgesIn(vertex).size() == 0)
        {   
            usedVertexDFG[vertex] = true;
            scheduled.push_back(vertex);
//             to_be_scheduled = Subtract(to_be_scheduled, scheduled);
//             scheduled.pop_back();
            vector<string>::iterator s = find(to_be_scheduled.begin(), to_be_scheduled.end(), vertex);
            to_be_scheduled.erase(s);
            //to_be_scheduled.erase(vertex);
            dfgASAP[vertex] = schedule_time;
            //clog << "Node: " << vertex << " is scheduled at time " << schedule_time << endl;
        }
    }
    while(to_be_scheduled.size() > 0)
    {
        for(size_t i=0; i < to_be_scheduled.size(); i++)
        {
            
            //vector<size_t> schedule_ok =  is_ready_to_schedule_ASAP(usedVertexDFG,dfgASAP,to_be_scheduled[i]);
            //cout << "schedule_ok " <<schedule_ok << endl;
            //cout << "to_be_scheduled: " << to_be_scheduled[i] << endl;
            string portNameDFG = getPostfix(to_be_scheduled[i]);
            if(!portNameDFG.empty())
            {
                for(const auto &edge: _DFG.edgesIn(to_be_scheduled[i]))
                {
                    if(usedVertexDFG[edge.from()])
                    {
                        dfgASAP[to_be_scheduled[i]] = dfgASAP[edge.from()] + 1;
                        usedVertexDFG[to_be_scheduled[i]] = true;

                        scheduled.push_back(to_be_scheduled[i]);
                        vector<string>::iterator s = find(to_be_scheduled.begin(), to_be_scheduled.end(), to_be_scheduled[i]);
                        to_be_scheduled.erase(s);
                        break;
                    }
                    if(portNameDFG.find("in") != portNameDFG.npos)
                    {
                        int edgeTo = getPrefix_FUnum(edge.to());
                        int edgeFrom = getPrefix_FUnum(edge.from());
                        //cout << "edge: " << edge.from() << edge.to() << edgeFrom - edgeTo << endl;
                        if((getPrefix_FU(edge.from()) == getPrefix_FU(edge.to())) && (edgeFrom - edgeTo) >= 0)
                        {

                            string dfg = getPrefix(edge.to());
                            //cout << dfg << "a " << endl;
                            for(const auto &pred: _DFG.edgesIn(dfg))
                                {
                                    if(getPostfix(pred.from()) != portNameDFG && usedVertexDFG[pred.from()])
                                    {
                                        dfgASAP[to_be_scheduled[i]] = dfgASAP[pred.from()];
                                        usedVertexDFG[to_be_scheduled[i]] = true;

                                        scheduled.push_back(to_be_scheduled[i]);
                                        vector<string>::iterator s = find(to_be_scheduled.begin(), to_be_scheduled.end(), to_be_scheduled[i]);
                                        to_be_scheduled.erase(s);
                                        break; 
                                    }
                                }
                            
                            
                            
                        }
                    }
                }
            }
            else
            {
                bool flag = false;
                vector<size_t> scheduletime;
                for(const auto &edge: _DFG.edgesIn(to_be_scheduled[i]))
                {
                    if(!usedVertexDFG[edge.from()])
                    {
                        flag = false;
                        break;
                    }
                    flag = true;
                    scheduletime.push_back(dfgASAP[edge.from()]);
                }
                if(flag)
                {
                    auto maxScheduletime = max_element(scheduletime.begin(), scheduletime.end());
                    dfgASAP[to_be_scheduled[i]] = *maxScheduletime + 1;
                    usedVertexDFG[to_be_scheduled[i]] = true;

                    scheduled.push_back(to_be_scheduled[i]);
                    vector<string>::iterator s = find(to_be_scheduled.begin(), to_be_scheduled.end(), to_be_scheduled[i]);
                    to_be_scheduled.erase(s);
                    break;
                }

            }
        }

        //     if(schedule_ok[0])
        //     {
        //         schedule_time = schedule_ok[1];
        //         dfgASAP[to_be_scheduled[i]] = schedule_time;
        //         usedVertexDFG[to_be_scheduled[i]] = true;
                
        //         //clog << "Node: " << to_be_scheduled[i] << " is scheduled at time " << schedule_time << endl;
               
        //         if (schedule_time > latest_time)
        //         {
        //             latest_time = schedule_time;
        //         }
        //         scheduled.push_back(to_be_scheduled[i]);
        //         vector<string>::iterator s = find(to_be_scheduled.begin(), to_be_scheduled.end(), to_be_scheduled[i]);
        //         to_be_scheduled.erase(s);
        //         //to_be_scheduled = Subtract(to_be_scheduled, scheduled);
        //         //scheduled.pop_back();
        //         break;
        //     }
            
        // }
    }
    clog << "ASAP Finished." << endl;
    // unordered_map<string,size_t>::iterator i;
    // for (i = dfgASAP.begin(); i != dfgASAP.end(); i++)
    // {
        
    //     if(getPostfix(i->first).empty())
    //         orderASAP[i->first] = i -> second;
    // }


    // order dfgasap
    // std::vector<std::pair<string, int>> temp;
    // for (const auto &i : dfgASAP)
    // {
    //     temp.push_back(i);
    // }
    // std::sort(temp.begin(), temp.end(), 
    //       [=](std::pair<string, int>& a, std::pair<string, int>& b) { return a.second < b.second; });
    // cout << temp << endl;
    

    std::vector<std::pair<string, int>> orderASAP;
    for (const auto &i : dfgASAP)
    {
        if(getPostfix(i.first).empty())
            orderASAP.push_back(i);
    }
    std::sort(orderASAP.begin(), orderASAP.end(), 
          [=](std::pair<string, int>& a, std::pair<string, int>& b) { return a.second < b.second; });
    //cout << orderASAP << endl;
    for(size_t i = 1; i < orderASAP.size(); i++)
    {
        if(dfgASAP[orderASAP[i].first] > dfgASAP[orderASAP[i-1].first])
            orderASAP[i].second = orderASAP[i-1].second + 1;
        else
            orderASAP[i].second = orderASAP[i-1].second; 
    }
    cout << orderASAP << endl;
    return orderASAP;
}

vector<std::pair<string, int>> Scheduling::Schedule_ALAP()
{
    //TODO: mults1
    clog << "ALAP Started." << endl;
    
    vector<string> scheduled;
    vector<string> to_be_scheduled;
    unordered_map<string, size_t> dfgALAP;
    unordered_map<string, bool>   usedVertexDFG; 
    vector<string> vertices; 

    size_t latest_time = 0;
    size_t schedule_time =  _DFG.nVertices();
    for(const auto &vertex: _DFG.vertices())
    {
        vertices.push_back(vertex.first);
        to_be_scheduled.push_back(vertex.first); 
        usedVertexDFG[vertex.first] = false;
    }
    for(const auto &vertex: vertices)
    {
        if(_DFG.edgesOut(vertex).size() == 0)
        {   
            usedVertexDFG[vertex] = true;
            scheduled.push_back(vertex);
            to_be_scheduled = Subtract(to_be_scheduled, scheduled);

            //to_be_scheduled.erase(vertex);
            dfgALAP[vertex] = schedule_time;
            //clog << "Node: " << vertex << " is scheduled at time " << schedule_time << endl;
        }
    }
    while(to_be_scheduled.size() > 0)
    {
        bool reStart = false;

        for(size_t i=0; i < to_be_scheduled.size(); i++)
        {
            // cout << dfgALAP << endl;
            // cout << "to_be_scheduled: " << to_be_scheduled[i] << endl;
            string portNameDFG = getPostfix(to_be_scheduled[i]);
            if(portNameDFG.find("out") != portNameDFG.npos)
            {
                bool flag = false;
                vector<size_t> scheduletime; 
                for(const auto &edge: _DFG.edgesOut(to_be_scheduled[i]))
                {
                    pair<bool, string> temp  = addOp(to_be_scheduled[i]);
                    if(temp.first)
                    {
                        usedVertexDFG[temp.second] = true;
                        // for(const auto &temp: _DFG.edgesOut(edge.from()))
                        // {
                        //     dfgALAP[edge.to()] = dfgALAP[temp.to()] - 2;
                        //     usedVertexDFG[edge.to()] = true;
                        //     vector<string>::iterator s = find(to_be_scheduled.begin(), to_be_scheduled.end(),edge.to());
                        //     to_be_scheduled.erase(s);
                        // }
                    }

                    //cout << "edge.to(): " << edge.to() << endl;
                    if(!usedVertexDFG[edge.to()])
                    {
                        flag = false;
                        break;
                    }
                    flag = true;
                    if(dfgALAP.find(edge.to()) != dfgALAP.end())
                        scheduletime.push_back(dfgALAP[edge.to()]);
           
                }
                if(flag && !scheduletime.empty())
                {

                    auto mixScheduletime = min_element(scheduletime.begin(), scheduletime.end());
                    dfgALAP[to_be_scheduled[i]] = *mixScheduletime - 1;
                    // cout << "successful1: " << to_be_scheduled[i] << " " << dfgALAP[to_be_scheduled[i]] << endl;
                    usedVertexDFG[to_be_scheduled[i]] = true;

                    scheduled.push_back(to_be_scheduled[i]);
                    vector<string>::iterator s = find(to_be_scheduled.begin(), to_be_scheduled.end(), to_be_scheduled[i]);
                    //cout << "erase: " << *s << endl;
                    to_be_scheduled.erase(s);
                    
                    //reStart = true;
                }
                

            }
            else
            {
                for(const auto &edge: _DFG.edgesOut(to_be_scheduled[i]))
                {
                    if(usedVertexDFG[edge.to()])
                    {
                        dfgALAP[to_be_scheduled[i]] = dfgALAP[edge.to()] - 1;
                        usedVertexDFG[to_be_scheduled[i]] = true;

                        scheduled.push_back(to_be_scheduled[i]);
                        // cout << "successful2: " << to_be_scheduled[i] << " " << dfgALAP[to_be_scheduled[i]] << endl;
                        vector<string>::iterator s = find(to_be_scheduled.begin(), to_be_scheduled.end(), to_be_scheduled[i]);
                        //cout << "erase: " << *s << endl;
                        to_be_scheduled.erase(s);
                         
                        //reStart = true;
                        break;
                    }
                }
            }
            //vector<size_t> schedule_ok =  is_ready_to_schedule_ALAP(usedVertexDFG,dfgALAP,to_be_scheduled[i]);
            //cout << "cur " << to_be_scheduled[i] << endl;
            // if(schedule_ok[0])
            // {

            //     schedule_time = schedule_ok[1];
            //     dfgALAP[to_be_scheduled[i]] = schedule_time;
            //     usedVertexDFG[to_be_scheduled[i]] = true;
            //     //to_be_scheduled.erase(to_be_scheduled[i]);
                
            //     //clog << "Node: " << to_be_scheduled[i] << " is scheduled at time " << schedule_time << endl;
            //     if (schedule_time < latest_time)
            //     {
            //         latest_time = schedule_time;
            //     }
            //     scheduled.push_back(to_be_scheduled[i]);
                
            //     to_be_scheduled = Subtract(to_be_scheduled, scheduled);
            //     break;
            // // }
            // if (reStart)
            // {
            //     break;
            // }
            
        }
    }
    clog << "ALAP Finished." << endl;
    std::vector<std::pair<string, int>> orderALAP;
    for (const auto &i : dfgALAP)
    {
        if(getPostfix(i.first).empty())
            orderALAP.push_back(i);
    }
    std::sort(orderALAP.begin(), orderALAP.end(), 
          [=](std::pair<string, int>& a, std::pair<string, int>& b) { return a.second < b.second; });
    //cout << orderALAP << endl;
    orderALAP[0].second = 0;
    for(size_t i = 1; i < orderALAP.size(); i++)
    {
        if(dfgALAP[orderALAP[i].first] > dfgALAP[orderALAP[i-1].first])
            orderALAP[i].second = orderALAP[i-1].second + 1;
        else
            orderALAP[i].second = orderALAP[i-1].second; 
    }
    cout << orderALAP << endl;
    return orderALAP;
}



vector<size_t> Scheduling::is_ready_to_schedule_ASAP( unordered_map<string, bool> &usedVertexDFG, unordered_map<string, size_t> &dfgASAP, string &vertex)
{
    size_t schedule_time = 0;
    vector<string> dfgNames;
    vector<size_t> schedule;
    for(const auto &edge: _DFG.edgesIn(vertex))
    {
        string port = getPostfix(edge.to());
        if(!port.empty())
        //cout << "to " << edge.to() << endl;
        {
            if(usedVertexDFG[edge.from()])
            {   
                //cout << "from() " << edge.from() << endl;
                schedule.push_back(1);
                schedule_time = dfgASAP[edge.from()] + 1;
                schedule.push_back(schedule_time);
                return schedule;
            }
            if(getPrefix(edge.to()) == getPrefix(edge.from()))
            {
                string dfg = getPrefix(edge.to());
                vector<string> ports;
                string otherport;
                for(const auto &pred: _DFG.edgesIn(dfg))
                {
                    ports.push_back(getPostfix(pred.from()));
                    if(usedVertexDFG[pred.from()])
                    {
                        otherport = pred.from();
                    }
                }
                auto iter = std::find(ports.begin(), ports.end(), edge.to());
                if (iter != ports.end())
                {
                    if(!usedVertexDFG[edge.to()])
                    {
                        dfgASAP[edge.to()] = dfgASAP[otherport];
                        usedVertexDFG[edge.to()] = true;
                    }
                }

            }
        }
        else
        {

        }

    }
    schedule.push_back(0);
    return schedule;

}



vector<size_t> Scheduling::is_ready_to_schedule_ALAP( unordered_map<string, bool> &usedVertexDFG, unordered_map<string, size_t> &dfgALAP, string &vertex)
{
    size_t schedule_time = 0;
    vector<string> dfgNames;
    vector<size_t> schedule;
    for(const auto &edge: _DFG.edgesOut(vertex))
    {
        //cout << "from " << edge.from() << endl;
        if(usedVertexDFG[edge.to()])
        {   
            //cout << "to() " << edge.to() << endl;
            schedule.push_back(1);
            schedule_time = dfgALAP[edge.to()] - 1;
            schedule.push_back(schedule_time);
            return schedule;
        }

    }
    schedule.push_back(0);
            return schedule;
}


// returns set1 - set2
vector<string> Scheduling::Subtract(vector<string> &set1, vector<string> &set2)
{
  vector<string> retVal;
  vector<string>::iterator iNode1;
  //cout << set1.size() << endl;
  if(set1.size() == 1)
      return retVal;
//   vector<string>::iterator s = find(set1.begin(), set1.end(), set2[0]);
//   set1.erase(s);
  for (iNode1 = set1.begin(); iNode1 != set1.end(); iNode1++)
  {
    vector<string>::iterator s = find(set2.begin(), set2.end(), *iNode1);
	if (s != set2.end())
	{
		set1.erase(iNode1);
	}
  }
  return set1;
}

unordered_map<string, int> Scheduling::topoSort()
//vector<pair<string, int>> Scheduling::topoSort()
{
    unordered_map<string, int> result; 
    int count = 0; 

    Graph tmpGraph1 = _DFG; 
    Graph tmpGraph2; 
    
    unordered_map<std::string, std::vector<Edge>> edgesOut = _DFG.edgesOut();

    while(true)
    {
        if(tmpGraph1.empty())
        {
            break; 
        }
        unordered_map<string, int> degs = tmpGraph1.degreesIn(); 
        for(const auto &deg: degs)
        {
            if(deg.second == 0)
            {
                //cout << "deg.first: " << deg.first << " " << "count: " << count << endl; 
                result[deg.first] = count; 
            }
            else
            {
                tmpGraph2.addVertex(deg.first); 
            }
            
        }
        for(const auto &arrEdges: edgesOut)
        {
            for(const auto &edge: arrEdges.second)
            {
                bool flag = false;
                if(degs[edge.from()] > 0)
                {
                    string portNameDFG = getPostfix(edge.from());
                    if(portNameDFG.find("out") != portNameDFG.npos)
                    {
                        int edgeTo = getPrefix_FUnum(edge.to());
                        int edgeFrom = getPrefix_FUnum(edge.from());
                        

                        if(getPrefix_FU(edge.to()) == getPrefix_FU(edge.from()) && (edgeFrom - edgeTo) >= 0)// && getPrefix(edge.from())=="add")
                            flag = true;
                    }
                    if(!flag)
                        tmpGraph2.addEdge(edge); 
                }
            }
        }
        tmpGraph1 = tmpGraph2; 
        tmpGraph2 = Graph(); 
        count++; 
    }
    //cout << result << endl;

    std::vector<std::pair<string, int>> topoSortDFG;
    for (const auto &i : result)
    {
        if(getPostfix(i.first).empty())
            topoSortDFG.push_back(i);
    }
    std::sort(topoSortDFG.begin(), topoSortDFG.end(), 
          [=](std::pair<string, int>& a, std::pair<string, int>& b) { return a.second < b.second; });
    for(size_t i = 1; i < topoSortDFG.size(); i++)
    {
        if(result[topoSortDFG[i].first] > result[topoSortDFG[i-1].first])
            topoSortDFG[i].second = topoSortDFG[i-1].second + 1;
        else
            topoSortDFG[i].second = topoSortDFG[i-1].second; 
    }
    cout << "TOPOSORT" <<endl;
    cout << topoSortDFG << endl;
    return result; 

}
pair<bool, string> Scheduling::addOp(string name)
{
    for(const auto &edge: _DFG.edgesOut(name))
    {
        if(getPrefix_FU(edge.to()) == getPrefix_FU(edge.from()))
        {
            size_t edgeTo = getPrefix_FUnum(edge.to());
            size_t edgeFrom = getPrefix_FUnum(edge.from());
           // cout << "edge.to()" << edge.to() << edgeTo<<endl;
           // cout << "edge.from()" << edge.from() << edgeFrom<< endl;
            size_t temp = edgeFrom - edgeTo;
            if(temp >= 0)
                return {true, edge.to()};

        }
    }
    return {false, ""};
}


vector<string> Scheduling::getSucc(string name)
{
    vector<string> dfgSucc;
    if(getPostfix(name).empty())
    {
        string dfgout;
        for(const auto &out: _DFG.edgesOut(name))
        {
            dfgout = out.to();
        }
        cout << dfgout << endl;

        for(const auto &succ: _DFG.edgesOut(dfgout))
        {
            if(getPrefix(succ.to()) == name)
                continue;
            dfgSucc.push_back(getPrefix(succ.to()));
        }
        
    }
    cout << "dfgSucc: " << dfgSucc << endl;
    return dfgSucc;
}
vector<string> Scheduling::getPred(string name)
{
    vector<string> dfgpred;
    if(getPostfix(name).empty())
    {
        for(const auto &in: _DFG.edgesIn(name))
        {
            for(const auto &pred: _DFG.edgesIn(in.from()))
            {
                dfgpred.push_back(getPrefix(pred.from()));
            }
        }
    }
    cout << "dfgpred: " << dfgpred << endl;
    return dfgpred;
}

}
