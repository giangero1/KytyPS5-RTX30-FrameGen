#ifndef KYTY_GRAPHICS_PRESENTATION_WINDOW_HOST_INPUT_H_
#define KYTY_GRAPHICS_PRESENTATION_WINDOW_HOST_INPUT_H_

#include <cstdint>

union SDL_Event;

namespace Libs::Graphics {

void               HostInputInit();
void               HostInputKey(int key_code, bool down);
// The keymap (--keymap) gives that key to a control of the pad.
[[nodiscard]] bool HostInputMapsKey(int key_code);
void               HostInputMouseButton(uint8_t mouse_button, bool down);
void               HostInputToggleMouseToJoystick();
[[nodiscard]] bool HostInputWaitEvent(SDL_Event* event);

} // namespace Libs::Graphics

#endif /* KYTY_GRAPHICS_PRESENTATION_WINDOW_HOST_INPUT_H_ */
