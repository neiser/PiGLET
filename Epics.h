//******************************************************************************
//! @file    CtrlSysAccessInterfaceEpics.h
//!
//! @author  F. Feldbauer
//!
//! @brief   Singleton class for the interface between Cooling control and
//!          Slow Control or Simulation, respectively
//!          Derived class for EPICS interfacing
//!
//! @version 1.0.0
//******************************************************************************

#ifndef INTERFACE_EPICS_H
#define INTERFACE_EPICS_H

//_____ I N C L U D E S ________________________________________________________
// System headers
#include <map>
#include <vector>

// Local headers
#include "interface/CtrlSysAccessInterface.h"

//_____ D E F I N I T I O N S __________________________________________________
namespace ChillerRegulator {

  // define max length for EPICS PV names
  const size_t MAX_PV_NAME_LEN = 40;

  // Forward declaration
  struct PV;
  typedef struct PV PV;

  class CtrlSysAccessInterfaceEpics : public CtrlSysAccessInterface {
  public:
    static void create();

    double meanTemperature( const eTemperature );
    double worstHumidity();
    ChillerStatus* chiller( const eChiller );
    void setTemperature( const double, const double, const double );
    void setPumpstage( const double, const double, const double );
    void writeMeanValues( const double, const double );
    double setValue();
    bool mode();
    void writeBackMeanTemps();

    void init( std::string& );
    void close();

  private:
    enum channelType { Tset, PumpSet, Switch, Mean };

    CtrlSysAccessInterfaceEpics ();
    CtrlSysAccessInterfaceEpics ( const CtrlSysAccessInterfaceEpics& );
    ~CtrlSysAccessInterfaceEpics ();

    void subscribe( std::string&, PV* );
    void createChannel( std::string&, PV*, channelType );

    // Storage for values
    std::vector<PV*> pvs_T_F_; // temperature sensors at front
    std::vector<PV*> pvs_T_B_; // temperature sensors at back
    std::vector<PV*> pvs_H_;   // humidity sensors
    std::vector<PV*> pvs_C_F_; // chiller for front cooling
    std::vector<PV*> pvs_C_B_; // chiller for main cooling
    std::vector<PV*> pvs_C_A_; // chiller for air cooling

    PV* chiller_front_setpoint_;
    PV* chiller_main_setpoint_;
    PV* chiller_air_setpoint_;
    PV* chiller_front_pumpstage_;
    PV* chiller_main_pumpstage_;
    PV* chiller_air_pumpstage_;
    PV* chiller_front_switch_;
    PV* chiller_main_switch_;
    PV* chiller_air_switch_;
    PV* reg_setVal_;
    PV* reg_mode_;
    PV* mean_front_;
    PV* mean_back_;

  };

}

#endif

//******************************************************************************
//! EOF
//******************************************************************************
