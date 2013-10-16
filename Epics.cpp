#include <iostream>
#include <iomanip>
#include "Epics.h"

using namespace std;

void Epics::exceptionCallback( exception_handler_args args ) {
  string pvname = ( args.chid ? ca_name( args.chid ) : "unknown" );
  cerr << "CA Exception: " << endl
       << "Status: " << ca_message(args.stat) << endl
       << "Channel: " + pvname << endl;
}

void Epics::connectionCallback( connection_handler_args args ) {
//#ifndef NDEBUG
//  cout << "ConnectionCallback:" << endl
//            << "PV:       " << ca_name( args.chid ) << endl
//            << "Type:     " << ca_field_type( args.chid ) << endl
//            << "Elements: " << ca_element_count( args.chid ) << endl
//            << "Host:     " << ca_host_name( args.chid ) << endl
//            << "Event:    " << ( args.op == CA_OP_CONN_UP ? "Connected" : " Disconnected" )
//            << endl;
//#endif
  
  if ( args.op == CA_OP_CONN_UP ) {
    // channel has connected
    PV* puser = (PV*)ca_puser( args.chid );

    puser->cb(EpicsConnected, 0, 0);
    
    // Create Subscription here or in init function?
    // PRO: 100% sure to use the correct datatype
    // CONTRA: how to invoke ca_pend_event()? should only be invoked after ALL subscribtions are made

    // ca_create_subscription( dbf_type_to_DBR_TIME( ca_field_type ( args.chid ) ), // CA data type
    //                         ca_element_count ( args.chid ),                      // number of elements
    //                         args.chid,                                           // unique channel id
    //                         DBE_VALUE | DBE_ALARM,                               // event mask (change of value and alarm)
    //                         eventCallback,                                       // name of event callback function
    //                         puser,
    //                         &puser->myevid );                                    // unique event id needed to clear subscription

  } else { // args.op == CA_OP_CONN_DOWN
    // channel has disconnected
    cerr << "Epics has lost connection to Channel "
              << ca_name( args.chid )
              << endl;
    PV* puser = (PV*)ca_puser( args.chid );
    puser->cb(EpicsDisconnected, 0, 0);
  }
}

void Epics::eventCallback( event_handler_args args ) {
//#ifndef NDEBUG
//  cout << "eventCallback:" << endl
//	    << "PV:      " << ca_name( args.chid ) << endl
//	    << "Type     " << ca_field_type( args.chid ) << endl
//	    << "Elements " << ca_element_count( args.chid ) << endl
//	    << "Host     " << ca_host_name( args.chid ) << endl;
//#endif

  if ( args.status != ECA_NORMAL ) {
      cerr << "Error in EPICS event callback" << endl;
  } else {
    dbr_time_double* dbr = (dbr_time_double*)args.dbr; // Convert void* to correct data type
    PV* puser = (PV*)ca_puser( args.chid );             // get pointer to corresponding PV struct
    double t = dbr->stamp.secPastEpoch + (double)dbr->stamp.nsec/1e9;
    (puser->cb)(EpicsNewValue, t, dbr->value);
  }
}

void Epics::addPV(const string &pvname, Epics::EpicsCallback cb)
{
    PV* puser = new PV;
    puser->cb = cb;
    int ca_rtn = ca_create_channel( pvname.c_str(),      // PV name
                                    connectionCallback,  // name of connection callback function
                                    puser,               // 
                                    CA_PRIORITY_DEFAULT, // CA Priority
                                    &puser->mychid );    // Unique channel id
    
    SEVCHK(ca_rtn, "ca_create_channel failed");
    
    ca_rtn = ca_create_subscription( DBR_TIME_DOUBLE,          // CA data type
                                     1,                        // number of elements
                                     puser->mychid,            // unique channel id
                                     DBE_VALUE | DBE_ALARM,    // event mask (change of value and alarm)
                                     eventCallback,            // name of event callback function
                                     puser,
                                     &puser->myevid );         // unique event id needed to clear subscription
      
    SEVCHK(ca_rtn, "ca_create_subscription failed");
    
    pvs[pvname] = puser;
    
    // dont know if this is really meaningful here (also done in ctor)
    ca_poll();        
    cout << "PV registered" << endl;
}

void Epics::removePV(const string &name)
{
    PV* pv = pvs[name];
    ca_clear_subscription ( pv->myevid );
    ca_clear_channel( pv->mychid );
    delete pv;
    cout << "PV unregisterd" << endl;
}

void Epics::TestCallback(const Epics::EpicsCallbackMode& m, const double &t, const double& y)
{
    switch(m) {
    case EpicsConnected:
        cout << "Connected to EPICS!" << endl;
        break;
    case EpicsDisconnected:
        cout << "Disconnected from EPICS!" << endl;
        break;
    case EpicsNewValue:
        cout << "New Value: x="<< std::setprecision(12) <<t<<" y="<<y << endl;
        break;
    default:
        break;
    }
}

Epics::Epics () {
    ca_context_create( ca_enable_preemptive_callback );
    ca_add_exception_event( exceptionCallback, NULL );
   
    ca_poll();
}

Epics::~Epics () {
    for (map<string, PV*>::iterator it = pvs.begin(); it != pvs.end(); ++it ) {
        removePV(it->first);
    }
    pvs.clear();
    ca_context_destroy();
    cout << "EPICS dtor" << endl;
}
