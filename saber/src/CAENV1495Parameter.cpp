
#include "CAENV1495Parameter.h"
#include <cstring>

using namespace std;


CAENV1495Parameter::CAENV1495Parameter() : VMEBoardParameter(){
    // set default values.
    link_number = 0;
    board_number = 0;
    base_addr = 0x10000000;

    output_mode = CRYSTAL;

    retrig_xystal = false;
    retrig_veto = false;

    veto_mask = 0x3ff;
    maj_lev = 3;
    sig_delay = 0x2;
    trig_time = 0x1;

    gate_len_xystal = 0x0003;
    gate_len_veto = 0x0003;

    dead_time = 50;
    veto_time = 50;
}


CAENV1495Parameter::~CAENV1495Parameter(){;}


void CAENV1495Parameter::Print(){
/*
    VMEBoardParameter::Print();

    cout<<"\n\tTrigger mode is ";
    switch(output_mode){
        case CRYSTAL:
            cout<<"crystal"; break;
        case LSCINTILLATOR:
            cout<<"pc"; break;
        case VETO:
            cout<<"veto";   break;
        case CALIBRATION:
            cout<<"calibration";    break;
        default:    break;
    }
    cout<<"\n\tCrystal re-trigger "<< retrig_xystal ? "enabled" : "disabled";
    cout<<"\n\tLiq. scintillator re-trigger "<< retrig_veto ? "enabled" : "disabled";
    cout<<"\n\tVeto mask  -";
    for( int i=9; i<0; --i)
        cout<< ((veto_mask>>i) & 0x1)!=0 ? '1' : '0';
    cout<<"\n\tMajority level on L.S. trigger "<<dec<<maj_lev;
    cout<<"\n\tSignal delay in veto mode "<<dec<<sig_delay;
    cout<<"\n\tTrigger duration "<<dec<<trig_time;
    cout<<"\n\tCrystal coincidence window "<<dec<<gate_len_xystal;
    cout<<"\n\tPC coincidence window "<<dec<<gate_len_veto;
    cout<<"\n\tDead time "<<dec<<dead_time;
    cout<<"\n\tVeto window in veto mode "<<dec<<veto_time;
    cout<<endl;
*/
}


void CAENV1495Parameter::SetParamFromConfig( ConfigParser* p, string dir){

    if(!p->Find(dir)){
        PushMessage( "Cannot find trigger parameters (/module/daq/trigger/).\n" );
        status = -1;
        return ;
    }

    SetLinkNumber( p->GetInt( "/module/daq/trigger/link_number", link_number ) );
    SetBoardNumber( p->GetInt( "/module/daq/trigger/board_number", board_number ) );
    SetBaseAddr( p->GetInt( "/module/daq/trigger/address", base_addr ) );


    string s,t;
    s = p->GetString( "/module/daq/trigger/trigger_mode", "crystal" );
    if(s=="crystal")    output_mode = CRYSTAL;
    else if(s=="veto")  output_mode = VETO;
    else if(s=="pc")    output_mode = LSCINTILLATOR;
    else if(s=="calibration")   output_mode = CALIBRATION;
    else{
        PushMessage( "Invalid trigger mode specified.\n");
        status = -1;
        return ;
    }

    retrig_xystal = p->GetBool( "/module/daq/trigger/enable_crystal_retrigger", retrig_xystal );
    retrig_veto = p->GetBool( "/module/daq/trigger/enable_pc_retrigger", retrig_veto );

    veto_mask = p->GetInt( "/module/daq/trigger/veto_mask", veto_mask );
    maj_lev = p->GetInt( "/module/daq/trigger/majority_level", maj_lev );
    sig_delay = p->GetInt( "/module/daq/trigger/signal_delay", sig_delay );
    trig_time = p->GetInt( "/module/daq/trigger/trigger_time", trig_time );

    gate_len_xystal = p->GetInt( "/module/daq/trigger/crystal_gate_length", gate_len_xystal );
    gate_len_veto = p->GetInt( "/module/daq/trigger/pc_gate_length", gate_len_veto );

    dead_time = p->GetInt( "/module/daq/trigger/dead_time", dead_time );
    veto_time = p->GetInt( "/module/daq/trigger/veto_time", veto_time );

    return ;
}


void CAENV1495Parameter::Serialize( char* p ){

    int bytes_copied = 0;

    vector<uint32_t> data;

    data.push_back( base_addr);
    data.push_back( output_mode);
    data.push_back( retrig_xystal);
    data.push_back( retrig_veto);
    data.push_back( veto_mask);
    data.push_back( maj_lev);
    data.push_back( sig_delay);
    data.push_back( trig_time);
    data.push_back( gate_len_xystal);
    data.push_back( gate_len_veto);
    data.push_back( dead_time);
    data.push_back( veto_time);

    for( unsigned int i=0; i<data.size(); ++i){
        memcpy( p+bytes_copied, &data[i], sizeof( uint32_t) );
        bytes_copied += sizeof(uint32_t);       
    }
}


void CAENV1495Parameter::Deserialize( char* p, bool flip ){

    vector<uint32_t> data;
    uint32_t temp = 0;

    for( int i=0; i<GetHeaderSize(); ++i){
        memcpy( &temp, p+i*sizeof(uint32_t), sizeof( uint32_t) );
        data.push_back(temp);
    }

    int offset = 0;

    base_addr = data[offset++];
    output_mode = static_cast<V1495_OMODE>(data[offset++]);
    retrig_xystal = data[offset++];
    retrig_veto = data[offset++];
    veto_mask = data[offset++];
    maj_lev = data[offset++];
    sig_delay = data[offset++];
    trig_time = data[offset++];
    gate_len_xystal = data[offset++];
    gate_len_veto = data[offset++];
    dead_time = data[offset++];
    veto_time = data[offset++];
}
