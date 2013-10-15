#include <iostream>
#include <cadef.h>
#include "Epics.h"

using namespace std;
using namespace PiGLPlot;


struct PiGLPlot::PV {
  chid            mychid;
  evid            myevid;
  dbr_time_double dbrval;
  bool            connected;
};

static void exceptionCallback( exception_handler_args args ) {
  string pvname = ( args.chid ? ca_name( args.chid ) : "unknown" );
  cerr << "CA Exception: " << endl
       << "Status: " << ca_message(args.stat) << endl
       << "Channel: " + pvname << endl;
}

static void connectionCallback( connection_handler_args args ) {
#ifndef NDEBUG
  cout << "ConnectionCallback:" << endl
            << "PV:       " << ca_name( args.chid ) << endl
            << "Type:     " << ca_field_type( args.chid ) << endl
            << "Elements: " << ca_element_count( args.chid ) << endl
            << "Host:     " << ca_host_name( args.chid ) << endl
            << "Event:    " << ( args.op == CA_OP_CONN_UP ? "Connected" : " Disconnected" )
            << endl;
#endif
  
  if ( args.op == CA_OP_CONN_UP ) {
    // channel has connected
    PV* puser = (PV*)ca_puser( args.chid );
    puser->connected = true;

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
    puser->connected = false;
  }
}

static void eventCallback( event_handler_args args ) {
#ifndef NDEBUG
  cout << "eventCallback:" << endl
	    << "PV:      " << ca_name( args.chid ) << endl
	    << "Type     " << ca_field_type( args.chid ) << endl
	    << "Elements " << ca_element_count( args.chid ) << endl
	    << "Host     " << ca_host_name( args.chid ) << endl;
#endif

  if ( args.status != ECA_NORMAL ) {
    // some error
  } else {
    dbr_time_double* temp = (dbr_time_double*)args.dbr; // Convert void* to correct data type
    PV* puser = (PV*)ca_puser( args.chid );             // get pointer to corresponding PV struct
    puser->dbrval = *temp;                              // Store the new value
  }
}

PiGLPlot::Epics::Epics () {

}

PiGLPlot::Epics::~Epics () {
}

void PiGLPlot::Epics::init() {

  if( isInitialized_ ) {
    cout << "Epics has already been initialized." << endl;
    return;
  }
  
  ca_context_create( ca_enable_preemptive_callback );
  ca_add_exception_event( exceptionCallback, NULL );
  PV* puser = new PV;
  subscribe("MyTestRecord", puser);
  ca_pend_event(1.e-12);
  pvs.push_back(puser); 

  isInitialized_ = true;
}

void PiGLPlot::Epics::close() {
  std::vector<PV*>::const_iterator it;
  for ( it = pvs.begin(); it != pvs.end(); ++it ) {
    ca_clear_subscription ( (*it)->myevid );
    ca_clear_channel( (*it)->mychid );
  }
  
  pvs.clear();
  ca_context_destroy();
}

void PiGLPlot::Epics::subscribe(const std::string& pvname, PV* puser ) {
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
  pvs.push_back(puser); 
}
