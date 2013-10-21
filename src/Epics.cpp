#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "Epics.h"
#include "Structs.h"

using namespace std;

Epics::Epics () {
    // modify the PATH variable such that caRepeater can be
    // found by EPICS. This avoids also a "defunct" thread
    stringstream mypath;
    mypath << getenv("PATH") << ":" << EPICS_BIN_PATH;
    if(setenv("PATH", mypath.str().c_str(), 1) != 0) {
        perror("Cannot set PATH variable for EPICS");
    }
  
    ca_context_create( ca_enable_preemptive_callback );
    ca_add_exception_event( exceptionCallback, NULL );
    ca_poll();
    t0 = epicsTime::getCurrent();        
    cout << "EPICS ctor" << endl;
}

Epics::~Epics () {
    for (map<string, PV*>::iterator it = pvs.begin(); it != pvs.end(); ++it ) {
        removePV(it->first);
    }
    pvs.clear();
    ca_context_destroy();
    
    cout << "EPICS dtor" << endl;
}

void Epics::exceptionCallback( exception_handler_args args ) {
    string pvname = ( args.chid ? ca_name( args.chid ) : "unknown" );
    cerr << "CA Exception: " << endl
         << "Status: " << ca_message(args.stat) << endl
         << "Channel: " + pvname << endl;
}

void Epics::deleteDataItem(DataItem* i) {
    // well, the void* data must be deleted as well
    // but we need to know the data type 
    // (see the callbacks below where these items are generated)
    switch (i->type) {
    case Epics::NewValue:
        delete static_cast<vec2_t*>(i->data);
        break;
    case Epics::NewProperties:
        delete static_cast<dbr_ctrl_double*>(i->data);
        break;
    default:
        // all other types currently 
        // don't fill something
        // in the i->data
        break;
    }            
    // delete the current item
    delete i;    
}

void Epics::fillList(DataItem *head, vector<DataItem*>& list)
{
    // scan the linked list
    list.reserve(100);
    DataItem* d = head; // start from the head
    while(d->prev != NULL) {
        list.push_back(d);
        d = d->prev;
    }
}

void Epics::connectionCallback( connection_handler_args args ) { 
    DataItem* pNew = new DataItem;
    
    if ( args.op == CA_OP_CONN_UP ) {
        // channel has connected
        pNew->type = Connected;
    } 
    else { // args.op == CA_OP_CONN_DOWN
        pNew->type = Disconnected;
    }
    
    appendToList((PV*)ca_puser(args.chid), pNew);
}

void Epics::eventCallback( event_handler_args args ) {
    if ( args.status != ECA_NORMAL ) {
        cerr << "Error in EPICS event callback" << endl;
        return;
    } 
    
    // since the content of dbr is only valid within
    // this callback, we create some new memory space here
    // and hardcopy it
    
    PV* pv = (PV*)ca_puser(args.chid);   
    DataItem* pNew = new DataItem;        
    
    if(args.type == DBR_TIME_DOUBLE) {
        vec2_t* data = new vec2_t;
        dbr_time_double* dbr = (dbr_time_double*)args.dbr; // Convert void* to correct data type
        epicsTime time(dbr->stamp);
        data->x= time - Epics::I().t0;
        data->y = dbr->value;
        
        // pack it together
        pNew->type = NewValue;
        pNew->data = data;   
    }
    else if(args.type == DBR_CTRL_DOUBLE) {
        
        dbr_ctrl_double* dbr = (dbr_ctrl_double*)args.dbr; // Convert void* to correct data type
        // make a hardcopy
        dbr_ctrl_double* data = new dbr_ctrl_double(*dbr);
        
        //cout << "ctrl received " << data->upper_warning_limit << " " << data->upper_alarm_limit << endl;
        
        // pack it together
        pNew->type = NewProperties;
        pNew->data = data;
    }
    
    appendToList(pv, pNew);    
}

void Epics::appendToList(Epics::PV* pv, Epics::DataItem *pNew)
{
    while (1) { // concurrency loop
        DataItem* pOld = pv->head;  // copy the state of the world. We operate on the copy
        pNew->prev = pOld; // chain the new node to the current head of recycled items
        if (__sync_bool_compare_and_swap(&pv->head, pOld, pNew))  // switch head of recycled items to new node
            break; // success
    }
    //cout << "Something was appended. Contents:" << endl;
}

Epics::DataItem** Epics::addPV(const string &pvname)
{    
    PV* pv = initPV();
    
    // subscribe to value and control
    subscribe(pvname, pv);
    
    // save the pv
    pvs[pvname] = pv;
    cout << "PV " << pvname << " registered" << endl;
    
    return &pv->head; // tell where to find the head (for reading the list)
}

Epics::PV* Epics::initPV() {
    // create the one and only data structure,
    // initialize the linked list (atomically modified by EPICS threads)
    PV* pv = new PV;
    DataItem* head = new DataItem; // create the first dummy item
    head->prev = NULL; // ensure it points to nothing before  
    pv->head = head; // save the pointer in the pv as a starting point
    return pv;    
} 

void Epics::subscribe(const string &pvname, PV* pv) {
    
    // create/subscribe for value
    int ca_rtn = ca_create_channel( pvname.c_str(),      // PV name
                                    connectionCallback,  // name of connection callback function
                                    pv,               // 
                                    CA_PRIORITY_DEFAULT, // CA Priority
                                    &pv->chid_val );    // Unique channel id
    SEVCHK(ca_rtn, "ca_create_channel for value failed");
    ca_rtn = ca_create_subscription( DBR_TIME_DOUBLE,          // CA data type
                                     1,                        // number of elements
                                     pv->chid_val,            // unique channel id
                                     DBE_VALUE,    // event mask (change of value and alarm)
                                     eventCallback,            // name of event callback function
                                     pv,
                                     &pv->evid_val );         // unique event id needed to clear subscription
    SEVCHK(ca_rtn, "ca_create_subscription for value failed");
    
    // create/subscribe for control (all interesting properties)
    ca_rtn = ca_create_channel( pvname.c_str(),      // PV name
                                NULL,  // name of connection callback function
                                pv,               // 
                                CA_PRIORITY_DEFAULT, // CA Priority
                                &pv->chid_ctrl );    // Unique channel id    
    SEVCHK(ca_rtn, "ca_create_channel for ctrl failed");
    ca_rtn = ca_create_subscription( DBR_CTRL_DOUBLE,          // CA data type
                                     1,                        // number of elements
                                     pv->chid_ctrl,            // unique channel id
                                     DBE_PROPERTY | DBE_ALARM,             // event mask (change of properties)
                                     eventCallback,            // name of event callback function
                                     pv,
                                     &pv->evid_ctrl);         // unique event id needed to clear subscription
    SEVCHK(ca_rtn, "ca_create_subscription for ctrl failed");
    
    // dont know if this is really meaningful here (also done in ctor)
    //ca_poll();
}

void Epics::removePV(const string& pvname)
{
    // hopefully, the pvname exists :)
    PV* pv = pvs[pvname];
    
    // cancel the subscription/channels
    int ca_rtn = ca_clear_subscription (pv->evid_val);
    SEVCHK(ca_rtn, "ca_clear_subscription for value failed");
    
    ca_rtn = ca_clear_channel(pv->chid_val);
    SEVCHK(ca_rtn, "ca_clear_channel for value failed");
    
    ca_rtn = ca_clear_subscription (pv->evid_ctrl);
    SEVCHK(ca_rtn, "ca_clear_subscription for ctrl failed");
    
    ca_rtn = ca_clear_channel(pv->chid_ctrl);
    SEVCHK(ca_rtn, "ca_clear_channel for ctrl failed");
    
    ca_poll();    
    
    // properly delete the linked list
    typedef vector<DataItem*> list_t;
    list_t list;
    fillList(pv->head, list);
    for(list_t::iterator it = list.begin(); it != list.end(); ++it) {
        deleteDataItem(*it);
    }
    
    // the item itself
    delete pv;    
    
    // remove it from container
    pvs.erase(pvname);    
    cout << "PV " << pvname << " unregistered" << endl;
}

double Epics::GetCurrentTime()
{
    return epicsTime::getCurrent()-t0;
}



