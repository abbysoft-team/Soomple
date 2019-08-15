#pragma once

// All strings that can be found in vst should be here for later translation and so
// TODO: move to file with strings (search for i18n library)

namespace Strings {
  constexpr const char *BUTTON_OPEN_FILE_TEXT = "Load sample";
  constexpr const char *BUTTON_PLAY_SAMPLE_TEXT = "Play sample";
  constexpr const char *BUTTON_STOP_SAMPLE_TEXT = "Stop sample";
  constexpr const char *OPEN_FILE_DIALOG_TEXT = "Please select sample file you want to load...";
  constexpr const char *NO_SAMPLE_LOADED_TEXT = "Choose sample in the left side of the window to start\n";
  constexpr const char *UNSUPPORTED_FILE_EXTENSION_ERROR_TITLE = "Error";
  constexpr const char *UNSUPPORTED_FILE_EXTENSION_ERROR_MESSAGE = "Cannot read this type of file";
  constexpr const char *SAMPLE_LENGTH_TOO_LONG = "Sample is too long. Soompler supports samples up to 30 sec";
}
