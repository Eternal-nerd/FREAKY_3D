#include "slider.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Slider::init(const std::string& id, OverlayMode mode, UIQuad knobQuad, UIQuad barQuad) {
    util::log(name_, "initializing ui slider");

    id_ = id;
    mode_ = mode;
    //position_ = position;


}

