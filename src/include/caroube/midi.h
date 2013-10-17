
#include <caroube.h>

namespace caroube
{
    using namespace std ;

    class MidiCanalListener;

    class ActivMidiListener : public map<MidiCanalListener *, int> {
	public:
	    void push (MidiCanalListener *p);
    };

    //!the table of per canal active midi listeners
    CAROUBE_H_SCOPE ActivMidiListener midil[16];


    class MidiCanalListener {
	protected:
	    int canal;
	public:
	    MidiCanalListener (int canal);

	    
	    bool activate (int canal = -1);
	    bool desactivate (void);

	    virtual void signalback (jack_midi_event_t & event) = 0;
    };
}


