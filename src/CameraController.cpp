#include "CameraController.hpp"

namespace antifurto {

CameraController::CameraController()
    : metronome_(config::monitorCycleDuration())
{ }

} // namespace antifurto
