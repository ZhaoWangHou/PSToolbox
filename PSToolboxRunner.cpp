#include "PSToolboxRunner.h"
#include "PSToolboxBaseEdge.h"
#include "Connector.h"

PSToolboxRunner::PSToolboxRunner(
    vector<PSToolboxBaseEdge *> _e, 
    vector<Connector *> _c,
    vector<int> _con_at_edge_start,
    vector<int> _con_at_edge_end){
  edges = _e;
  cons = _c;
  con_at_edge_start = _con_at_edge_start;
  con_at_edge_end = _con_at_edge_end;
  save_data=false;
};


void PSToolboxRunner::Run(double t_max){
  // Initialization
  for (unsigned int i=0; i<edges.size(); i++)
    edges.at(i)->Ini();

  // Simulation
  double t_global=0., dt_out=t_max/10., t_out=-1.e-10;
  double t_next;
  int update_idx;
  vector<bool> update_edges(edges.size());

  while (t_global<t_max){
    //outputFile<<t_global<<endl;
    if (t_global>t_out){
      cout<<endl<<round(t_global/t_max*100)<<"%";
      t_out+=dt_out;
    }
    // Find the edge that needs update
    // TODO: if two edges are very close to each other, we need to update them at once

    fill(update_edges.begin(),update_edges.end(),false);

    update_idx=0;
    t_next=edges.at(update_idx)->Get_tnext();
    for (unsigned int i=0; i<edges.size(); i++){
             cout<<endl<<" pipe "<<i<<" ("<<edges.at(i)->Get_name()<<"): t="<<edges.at(i)->Get_t();
                cout<<", tnext="<<edges.at(i)->Get_tnext()<<", current: "<<t_next;
      if (edges.at(i)->Get_tnext()<t_next){
        update_idx=i;
        t_next=edges.at(i)->Get_tnext();
   cout<<"   *";
      }
    }
    cout<<endl<<"Updating element "<<update_idx;
    cin.get();

    update_edges.at(update_idx)=true;

    edges.at(update_idx)->Step();
      cout<<edges.at(0)->Info();
      cin.get();
    
      cons.at(con_at_edge_start.at(update_idx))->Update(t_next,update_idx);
      cout<<edges.at(0)->Info();
      cin.get();

    cons.at(con_at_edge_end.at(update_idx))->Update(t_next,update_idx);
      cout<<edges.at(0)->Info();
    
      cin.get();
    edges.at(update_idx)->UpdateTime(t_next);

    t_global=t_next;

    if (save_data)
      for (unsigned int i=0; i<edges.size(); i++)
        edges.at(i)->Save_data();
  
    if (update_idx==0){ 
      cout<<edges.at(0)->Info();
      cin.get();
    }
  
  }

  if (save_data)
    for (unsigned int i=0; i<edges.size(); i++)
      edges.at(i)->Write_data();
  //outputFile.close();


}
