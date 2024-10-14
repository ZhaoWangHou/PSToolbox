// g++ -I/usr/local/include/eigen3 -L/Users/hoscsaba/program/PSToolbox -lPSToolbox -lmy_tools -pedantic -O3 -Wall -Wno-c++11-long-long test11_runner.cpp

#include <stdio.h>
#include "/Users/hoscsaba/Documents/GitHub/PSToolbox/SCP.h"
#include "/Users/hoscsaba/Documents/GitHub/PSToolbox/Connector.h"
#include "/Users/hoscsaba/Documents/GitHub/PSToolbox/PSToolboxBaseEdge.h"
#include "/Users/hoscsaba/Documents/GitHub/PSToolbox/PSToolboxRunner.h"

using namespace std;

int main(int argc, char **argv) {
  bool DEBUG=false;
  vector<PSToolboxBaseEdge *> edges;
  vector<Connector *> cons;

  //=============== THIS SECTION GOES TO THE DATA READER
  // define edges
  edges.push_back(new SCP("p1","n1","n2",1000,1300,100,0.1,0.02,0,0,false));
  edges.push_back(new SCP("p2","n2","n3",1000,1100,100,0.1,0.02,0,0,false));

  // define nodes (connectors)
  double demand1 = 0.;
  double demand2 = 0.;
  double demand3 = 0.;
  cons.push_back(new Connector(edges.at(0),true,"Pressure",1.e5,demand1,DEBUG));
  cons.push_back(new Connector(edges.at(0),false,edges.at(1),true,demand2,DEBUG));
  cons.push_back(new Connector(edges.at(1),false,"Velocity",0.,demand3,DEBUG));

  // We need to add here connectivity info, e.g. by adding edge pointer + start/end info if relevant
  // con_at_edge_start.at(i) stores the idx of the connector connected to the start of the edge 
  // con_at_edges_end.at(i)   stores the idx of the connector connected to the end of the edge 
  vector<int> con_at_edge_start(edges.size(),-1);
  vector<int> con_at_edge_end(edges.size(),-1);
  con_at_edge_start.at(0)=0; con_at_edge_end.at(0)=1;
  con_at_edge_start.at(1)=1; con_at_edge_end.at(1)=2;
  // =============== END OF DATA READER SECTION

  // Initialization
  for (unsigned int i=0; i<edges.size(); i++)
    edges.at(i)->Ini();

  // Simulation
  double t_global=0.,t_max=1.;
  double t_next;
  int update_idx;
  vector<bool> update_edges(edges.size());

  while (t_global<t_max){
    // Find the edge that needs update
    // TODO: if two edges are very close to each other, we need to update them at once
    for (unsigned int i=0; i<edges.size(); i++){
      fill(update_edges.begin(),update_edges.end(),false);
      t_next=t_global+1.e5;
      for (unsigned int i=0; i<edges.size(); i++)
        if (edges.at(i)->Get_tnext()<t_next){
          update_idx=i;
          t_next=edges.at(i)->Get_tnext();
        }
    }
    update_edges.at(update_idx)=true;

    // Perform update. 
    for (unsigned int i=0; i<edges.size(); i++)
      if (update_edges.at(i)){
        // This updates internal points only, without any info on the BCs
        edges.at(i)->Step();
        // Take care of the BCs
        cons.at(con_at_edge_start.at(update_idx))->Update(t_next);
        cons.at(con_at_edge_end.at(update_idx))->Update(t_next);
      }
    t_global=t_next;
    cout<<endl<<t_global/t_max<<" -> update: "<<update_idx;
  }
}
