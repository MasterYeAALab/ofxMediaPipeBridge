meta:
	ADDON_NAME = ofxMediaPipeBridge
	ADDON_DESCRIPTION = openFramworks addon for receiving MediaPipe data via OSC
	ADDON_AUTHOR = @design-io
	ADDON_TAGS = "addon" "ofx" "mediapipe" "osc"
	ADDON_URL = https://github.com/design-io/ofxMediaPipeBridge

common:
	# dependencies with other addons, a list of them separated by spaces 
	# or use += in several lines
	ADDON_DEPENDENCIES = ofxOsc ofxGui
	
	# any special flag that should be passed to the linker when using this
	# addon, also used for system libraries with -lname
	# ADDON_LDFLAGS =
	
	# include search paths, this will be usually parsed from the file system
	# but if the addon or addon libraries need special search paths they can be
	# specified here separated by spaces or one per line using +=
	
	ADDON_DEFINES = OFX_MEDIAPIPE_EXCLUDE_TRACKERS OF_ADDON_HAS_OFX_OSC
